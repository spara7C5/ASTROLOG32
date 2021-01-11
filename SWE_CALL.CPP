/*
** Astrolog File: swe_call.c
**
** IMPORTANT NOTICE: The graphics database and chart display routines
** used in this program are Copyright (C) 1991-1998 by Walter D. Pullen
** (Astara@msn.com, http://www.magitech.com/~cruiser1/astrolog.htm).
** Permission is granted to freely use and distribute these routines
** provided one doesn't sell, restrict, or profit from them in any way.
** Modification is allowed provided these notices remain with any
** altered or edited versions of the program.
**
** The main planetary calculation routines used in this program have
** been Copyrighted and the core of this program is basically a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for personal use but not to sell them or profit from
** them in any way.
**
** The PostScript code within the core graphics routines are programmed
** and Copyright (C) 1992-1993 by Brian D. Willoughby
** (brianw@sounds.wa.com). Conditions are identical to those above.
**
** The extended accurate ephemeris databases and formulas are from the
** calculation routines in the library SWISS EPHEMERIS and are 
** programmed and copyright 1998 by Astrodienst AG. The use of that 
** source code is subject to the Swiss Ephemeris Public License, 
** available at http://www.astro.ch/swisseph. 
**
** This copyright notice must not be changed or removed 
** by any user of this program.
**
** Initial programming 8/28,30, 9/10,13,16,20,23, 10/3,6,7, 11/7,10,21/1991.
** X Window graphics initially programmed 10/23-29/1991.
** PostScript graphics initially programmed 11/29-30/1992.
** Modifications from version 5.40 to 5.41 are by Alois Treindl.
** Modifications from version 5.41 to 5.41g are by Valentin Abramov.
** Windows 32-bits version by Antonio Ramos.
**
** This software is provided "as is", without warranty of any kind, either 
** express or implied. In no event shall the authors or copyright holders be 
** liable for any claim, damages or other liability, arising from the use or 
** performance of this software.
**
*/

#include "astrolog.h"



/*
******************************************************************************
** Auxiliary Functions
******************************************************************************
*/


// converts an astrolog object number to Swiss Ephemeris object number
// returns Swiss Ephemeris object number, or -1 if conversion not possible
static int astrolog_object_2_SE_object(int astrolog_object)
{
	int swiss_object;

	if (FBetween(astrolog_object, oSun, oPlu))
	{
		swiss_object = astrolog_object - oSun + SE_SUN;
	}
	else if (FBetween(astrolog_object, oCeres, oVesta))
	{
		swiss_object = astrolog_object - oCeres + SE_CERES;
	}
	else if (astrolog_object == oNoNode)
	{
		swiss_object = us.fTrueNode ? SE_TRUE_NODE : SE_MEAN_NODE;
	}
	else if (astrolog_object == oLil)
	{
		swiss_object = oscLilith ? SE_OSCU_APOG : SE_MEAN_APOG;
	}
	else if (astrolog_object == oChi)
	{
		swiss_object = SE_CHIRON;
	}
	else if (FBetween(astrolog_object, oCupido, oPoseidon))
	{
		swiss_object = astrolog_object - oCupido + SE_CUPIDO;
	}
	else if (astrolog_object == oProserpina)
	{
		swiss_object = SE_PROSERPINA;
	}
	else if (astrolog_object == oEar)
	{
		swiss_object = SE_EARTH;
	}
	else
	{
		swiss_object = -1;	// no equivalent, conversion not possible
	}

	return swiss_object;
}



/*
******************************************************************************
** Global Functions
******************************************************************************
*/
 
void SetEphemerisPath(void)
{
	file_name_t path;

	// Build an ephemeris path in the same way as FileOpen() in io.c 
	// searchs for a file. First look in the current directory, then 
	// in the executable directory, and finally in the ephemeris directory.
	sprintf(path, "." PATH_SEPARATOR "%s" PATH_SEPARATOR "%s",
		dirs.executable.dir, dirs.ephemeris.dir);

	// now set the directory
	swe_set_ephe_path(path);
}

// 5.41G used SE of 02/12/1998

/* Given an object index and a Julian Day ephemeris time, get ecliptic longitude and	*/
/* latitude of the object and its velocity and distance from the Earth or Sun. 			*/

BOOL CalculatePlanetSE(int ind, double jde, BOOL heliocentric, double SE_coordinates[6])
{
	int iobj;
	long flag;
	char serr[AS_MAXCH];


	iobj = astrolog_object_2_SE_object(ind);

	if(iobj >= 0)
	{
		flag = heliocentric ? SEFLG_SPEED | SEFLG_HELCTR : SEFLG_SPEED;
		if(us.fSidereal)
		{
			flag |= SEFLG_NONUT;
		}

		if (swe_calc(jde, iobj, flag, SE_coordinates, serr) >= 0)
		{
			return TRUE;	// success
		}
	}

	return FALSE; // object not supported or other kinds of errors
}

/* Added by V. Abramov, calls calculation of fixed stars. */

BOOL CalculateStarSE(int ind, double jd, double* longitude, double* latitude)
{
	long flag;
	char serr[AS_MAXCH];
	char iobj[SE_MAX_STNAME * 2];
	char namebuf[SE_MAX_STNAME * 2];
	double jde, SE_coordinates[6];
	int pos;
	char *nomname;


	sprintf(iobj, "%i", ind);
	flag = SEFLG_SWIEPH;
	jde = jd + swe_deltat(jd);

	if (swe_fixstar(iobj, jde, flag, SE_coordinates, serr) >= 0)	
	{
		*longitude = SE_coordinates[0];
		*latitude = SE_coordinates[1];
		rStarBright[ind] = SE_coordinates[2];
		nomname = (char*)_memccpy(namebuf, iobj, ',', SE_MAX_STNAME + 1);
		pos = nomname - namebuf;
		if (pos > starMaxName)
			namebuf[starMaxName] = chNull;
		else
			namebuf[pos - 1] = chNull;
		Lang.SetTableItem(tObjName, ind + starLo - 1, namebuf);
		strcpy(namebuf, iobj);
		namebuf[pos + starMaxNName] = chNull;
		szStarConName[ind] = _strdup(nomname);

		return TRUE;
	}

	if (us.fSeconds)
	{
		ErrorEphem(SE_STARFILE, ind);
	}

	return FALSE;
}

/* Compute the positions of the planets at a certain time using Swiss 					*/
/* Ephemeris. This supersedes the Matrix routine values. Not all objects or modes are	*/
/* available using this function, but some additional values such as Moon and			*/
/* Node velocities are available.													    */

void ComputeWithSwissEphemeris(double t)
{
	int i;
	double SE_coordinates[6];
	double longitude, latitude, distance, speed_longitude, speed_latitude;
	BOOL	use_SE_heliocentric_mode;
	double jd, jde;


	/* We can compute the positions of Sun through Pluto, Chiron, the four	*/
	/* asteroids, Lilith, the (true or mean) North Node and the Uranians 	*/
	/* using Swiss Ephemeris. The other objects must be done elsewhere.     */

	/* Note, that with central planet, other than Earth, Lilith isn't */
	/* calculated at all and we use South Node instead.          V.A. */

	// Calculate planet positions with Earth as centre, or Sun if centre is another planet
	// The reason is that SE cannot calculate positions based on other centres, therefore
	// we first calculate heliocentric, and further down in the function we convert
	// coordinates.
	use_SE_heliocentric_mode = us.objCenter != oEar;

	jd = JulianDayFromTime(t);
	cp0.deltaT = swe_deltat(jd);
	jde = jd + cp0.deltaT; // ephemeris time


	for (i = oEar; i <= uranHi; i++)
	{
		if ((ignore[i] && i > oMoo) || FBetween(i, oFor, cuspHi))
		{
			continue;
		}

		if ((i == oEar && us.objCenter == oEar) || (i == oSun && use_SE_heliocentric_mode))
		{
			// there is no point in calculation a geocentric position for the Earth,
			// also for the Sun if we use SE heliocentric positions
			continue;
		}

		if (CalculatePlanetSE(i, jde, use_SE_heliocentric_mode, SE_coordinates))
		{
			// SE_coordinates is array of 6 doubles for longitude, latitude, distance, 
			// speed in long., speed in lat., and speed in dist.

			longitude = SE_coordinates[0];
			latitude = SE_coordinates[1];
			distance = SE_coordinates[2];
			speed_longitude = SE_coordinates[3];
			speed_latitude = SE_coordinates[4];

			/* Note that this can't compute charts with central planets other */
			/* than the Sun or Earth or relative velocities in current state. */

			/* In this case we use previously calculated velocities, also  */
			/* Node coordinates, which aren't computed  here.        V.A.  */

			if (us.objCenter == oEar || (i != oNoNode && i != oLil))
			{
				cp0.longitude[i] = Mod(longitude + is.rSid);
				cp0.latitude[i] = latitude;
				cp0.vel_longitude[i] = Deg2Rad(speed_longitude);
			}
			cp0.vel_latitude[i] = Deg2Rad(speed_latitude);

			/* Compute x,y,z coordinates from azimuth, altitude, and distance. */

			SphToRec(distance, cp0.longitude[i], cp0.latitude[i], &spacex[i], &spacey[i], &spacez[i]);
		}
	}

	// this is needed before the transformation that follows
	if (use_SE_heliocentric_mode)
	{
		spacex[oSun] = spacey[oSun] = spacez[oSun] = cp0.longitude[oSun] = cp0.latitude[oSun] = cp0.vel_longitude[oSun] = 0.0;
	}
	else
	{
		spacex[oEar] = spacey[oEar] = spacez[oEar] = cp0.longitude[oEar] = cp0.latitude[oEar] = cp0.vel_longitude[oEar] = 0.0;
	}

	if (us.objCenter <= oSun)	// heliocentric or geocentric
	{
		return;	
	}

	/* If other planet centered, shift all positions as in Matrix formulas. */

	for (i = 0; i <= cLastPlanet; i++)
	{
		if (!FIgnore(i))
		{
			spacex[i] -= spacex[us.objCenter];
			spacey[i] -= spacey[us.objCenter];
			spacez[i] -= spacez[us.objCenter];

			// convert the rectangular coordinates of the planet to zodiac position and declination
			ProcessPlanet(i, 0.0);
		}
	}

	// zero coordinates of central planet

	spacex[us.objCenter] = spacey[us.objCenter] = spacez[us.objCenter] = 0.0;
	cp0.longitude[us.objCenter] = cp0.latitude[us.objCenter] = cp0.vel_longitude[us.objCenter] = 0.0;
}

