/*
** Astrolog  File: calc.c
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

double ObjPrime[cLastMoving + 1];
double StarPrime[cStar + 1];
double Longit = 0.0;
double Latit = 0.0;




/*
******************************************************************************
** Star Position Calculations.
******************************************************************************
*/

/* This is used by the chart calculation routine to calculate the positions */
/* of the fixed stars. Since the stars don't move in the sky over time, 	*/
/* getting their positions is mostly just reading info from an array and	*/
/* converting it to the correct reference frame. However, we have to add	*/
/* in the correct precession for the tropical zodiac, and sort the final	*/
/* index list based on what order the stars are supposed to be printed in.	*/

void ComputeStars(double SD) {
	int i, j;
	double x, y, z;
	double lon, lat, lonMC, latMC, lonz, latz, azi, alt;

	BOOL HaveStarFile;
	double t_plac, longitude, latitude;

	HaveStarFile = TRUE;
	t_plac = JulianDayFromTime(is.T);

	if (!FileFind("fixstars.cat", dirs.main.dir, NULL))
	{
		HaveStarFile = FALSE;
		if (us.fSeconds)
			ErrorEphem("fixstars.cat", -1);
	}

	/* If need, prepare to calculate Prime Vertical of stars. */

	if (us.nStar == 'p' || PrimeRest > 0)
	{
		lon = Deg2Rad(Mod(Longit));
		lat = Deg2Rad(Latit);
		if (us.fEquator)
		{
			lonMC = Deg2Rad(cp0.longitude[oMC]);
			latMC = Deg2Rad(cp0.latitude[oMC]);
		}
		else
		{
			lonMC = Deg2Rad(Tropical(cp0.longitude[oMC]));
			latMC = Deg2Rad(cp0.latitude[oMC]);
			EclToEqu(&lonMC, &latMC);
		}
		if (PolarMCflip && hRevers)
			lonMC = Mod(lonMC + rPi);
	}

	/* Read in star positions. */

	for (i = 1; i <= cStar; i++)
	{
		x = rStarData[i * 6 - 6];
		y = rStarData[i * 6 - 5];
		z = rStarData[i * 6 - 4];
		cp0.longitude[cLastMoving + i] = Deg2Rad(x * 360.0 / 24.0 + y * 15.0 / 60.0 + z * 0.25 / 60.0);
		x = rStarData[i * 6 - 3];
		y = rStarData[i * 6 - 2];
		z = rStarData[i * 6 - 1];
		if (x < 0.0)
		{
			neg(y);
			neg(z);
		}
		else if (fabs(x) == 0.0 && y < 0.0)
		{
			neg(z);
		}
		cp0.latitude[cLastMoving + i] = Deg2Rad(x + y / 60.0 + z / 60.0 / 60.0);
		/* Convert to ecliptic zodiac coordinates. */
		EquToEcl(&cp0.longitude[cLastMoving + i], &cp0.latitude[cLastMoving + i]);
		cp0.longitude[cLastMoving + i] = Mod(Rad2Deg(cp0.longitude[cLastMoving + i]) + rEpoch2000 + SD);
		cp0.latitude[cLastMoving + i] = Rad2Deg(cp0.latitude[cLastMoving + i]);

		if (HaveStarFile)
		{
			if (CalculateStarSE(i, t_plac, &longitude, &latitude))
			{
				cp0.longitude[cLastMoving + i] = Mod(longitude + is.rSid);
				cp0.latitude[cLastMoving + i] = latitude;
			}
		}
		cp0.vel_longitude[cLastMoving + i] = Deg2Rad(360.0 / 26000.0 / 365.25);
		cp0.vel_latitude[cLastMoving + i] = 0.0;
		starname[i] = i;

		/* If sorting by prime vertical needed, compute it. */

		if (us.nStar == 'p' || PrimeRest > 0)
		{
			if (us.fEquator)
			{
				lonz = Deg2Rad(cp0.longitude[cLastMoving + i]);
				latz = Deg2Rad(cp0.latitude[cLastMoving + i]);
			}
			else
			{
				lonz = Deg2Rad(Tropical(cp0.longitude[cLastMoving + i]));
				latz = Deg2Rad(cp0.latitude[cLastMoving + i]);
				EclToEqu(&lonz, &latz);
			}
			lonz = Deg2Rad(Mod(Rad2Deg(lonMC - lonz + lon)));
			lonz = Deg2Rad(Mod(Rad2Deg(lonz - lon + rPiHalf)));
			if (!fEquator)
				EquToLocal(&lonz, &latz, rPiHalf - lat);
			azi = 360.0 - Rad2Deg(lonz);
			alt = Rad2Deg(latz);
			azi = Deg2Rad(azi);
			alt = Deg2Rad(alt);
			CoorXform(&azi, &alt, rPiHalf);
			azi = Rad2Deg(azi);
			alt = Rad2Deg(alt);
			StarPrime[i] = azi;
		}
	}
	if (PrimeRest > 0)
	{
		for (i = 1; i <= cLastMoving; i++)
		{
			if (us.fEquator)
			{
				lonz = Deg2Rad(cp0.longitude[i]);
				latz = Deg2Rad(cp0.latitude[i]);
			}
			else
			{
				lonz = Deg2Rad(Tropical(cp0.longitude[i]));
				latz = Deg2Rad(cp0.latitude[i]);
				EclToEqu(&lonz, &latz);
			}
			lonz = Deg2Rad(Mod(Rad2Deg(lonMC - lonz + lon)));
			lonz = Deg2Rad(Mod(Rad2Deg(lonz - lon + rPiHalf)));
			if (!fEquator)
				EquToLocal(&lonz, &latz, rPiHalf - lat);
			azi = 360.0 - Rad2Deg(lonz);
			alt = Rad2Deg(latz);
			azi = Deg2Rad(azi);
			alt = Deg2Rad(alt);
			CoorXform(&azi, &alt, rPiHalf);
			azi = Rad2Deg(azi);
			alt = Rad2Deg(alt);
			ObjPrime[i] = azi;
		}
	}

	/* Sort the index list if -Uz, -Ul, -Un, or -Ub switch in effect. */

	if (us.nStar > 1)
		for (i = 2; i <= cStar; i++)
		{
			j = i - 1;

			/* Compare star names for -Un switch. */

			if (us.nStar == 'n')
				while (j > 0 && NCompareSz(Lang(tObjName, cLastMoving + starname[j]), Lang(tObjName, cLastMoving + starname[j + 1])) > 0)
				{
					SwapN(starname[j], starname[j + 1]);
					j--;

					/* Compare star brightnesses for -Ub switch. */

				}
			else if (us.nStar == 'b')
				while (j > 0 && rStarBright[starname[j]] > rStarBright[starname[j + 1]])
				{
					SwapN(starname[j], starname[j + 1]);
					j--;

					/* Compare star zodiac locations for -Uz switch. */

				}
			else if (us.nStar == 'z')
				while (j > 0 && cp0.longitude[cLastMoving + starname[j]] > cp0.longitude[cLastMoving + starname[j + 1]])
				{
					SwapN(starname[j], starname[j + 1]);
					j--;

					/* Compare star declinations for -Ul switch. */

				}
			else if (us.nStar == 'l')
				while (j > 0 && cp0.latitude[cLastMoving + starname[j]] < cp0.latitude[cLastMoving + starname[j + 1]])
				{
					SwapN(starname[j], starname[j + 1]);
					j--;

					/* Compare star prime vertical for -Up switch. */

				}
			else if (us.nStar == 'p')
				while (j > 0 && StarPrime[starname[j]] < StarPrime[starname[j + 1]])
				{
					SwapN(starname[j], starname[j + 1]);
					j--;
				}
		}
}


/*
******************************************************************************
** Chart Calculation.
******************************************************************************
*/

/* The following three functions calculate the Midheaven, Ascendant, and  */
/* East Point of the chart in question, based on time and location. The   */
/* first two are also used in some of the house cusp calculation routines */
/* as a quick way to get the 10th and 1st cusps. The East Point object is */
/* technically defined as the Ascendant's position at zero latitude.      */

double CuspMidheaven()
{
	double MC;

	MC = atan(tan(is.RA) / cos(is.rObliquity));
	if (MC < 0.0)
		MC += rPi;
	if (is.RA > rPi)
		MC += rPi;
	return Mod(Rad2Deg(MC) + is.rSid);
}

double CuspAscendant()
{
	double Asc;

	Asc = Angle(-sin(is.RA) * cos(is.rObliquity) - tan(ciCore.lat) * sin(is.rObliquity), cos(is.RA));
	return Mod(Rad2Deg(Asc) + is.rSid);
}

double CuspEastPoint()
{
	double EP;

	EP = Angle(-sin(is.RA) * cos(is.rObliquity), cos(is.RA));
	return Mod(Rad2Deg(EP) + is.rSid);
}

/* Given a zodiac degree, transform it into its Decan sign, where each	  */
/* sign is trisected into the three signs of its element. For example,	  */
/* 1 Aries -> 3 Aries, 10 Leo -> 0 Sagittarius, 25 Sagittarius -> 15 Leo. */

double Decan(double deg) {
	int sign;
	double unit;

	sign = Z2Sign(deg);
	unit = deg - Sign2Z(sign);
	sign = Mod12(sign + 4 * ((int) floor(unit / 10.0)));
	unit = (unit - floor(unit / 10.0) * 10.0) * 3.0;
	return Sign2Z(sign) + unit;
}


/* Given a zodiac degree, transform it into its Navamsa position, where   */
/* each sign is divided into ninths, which determines the number of signs */
/* after a base element sign to use. Degrees within signs are unaffected. */

double Navamsa(double deg) {
	int sign, sign2;
	double unit;

	sign = Z2Sign(deg);
	unit = deg - Sign2Z(sign);
	sign2 = Mod12(((sign - 1 & 3) ^ (2 * (sign - 1 & 1))) * 3 + (int) (unit * 0.3) + 1);
	return Sign2Z(sign2) + unit;
}


/* Transform spherical to rectangular coordinates in x, y, z. */

void SphToRec(double r, double azi, double alt, double *rx, double *ry, double *rz) {
	double rT;

	*rz = r * RSinD(alt);
	rT = r * RCosD(alt);
	*rx = rT * RCosD(azi);
	*ry = rT * RSinD(azi);
}



/* This function generates a chart, calculating the positions of all the  */
/* celestial bodies and house cusps, based on the current chart 		  */
/* information, and saves them for use by any of the display routines.	  */


double CastChart(BOOL fDate) {
	CI ci;
	double housetemp[NUMBER_OF_SIGNS + 1], Off = 0.0, vtx, j, l, m, ll, kk, alt1, alt2;
	double SunDirPos, SunDirection;
	int i, k, n, ii, mi;
	int ignoreT[NUMBER_OBJECTS], ignoreT3[NUMBER_OBJECTS];

	/* Hack: Time zone +/-24 means to have the time of day be in Local Mean */
	/* Time (LMT). This is done by making the time zone value reflect the	*/
	/* logical offset from GMT as indicated by the chart's longitude value. */

	if (fabs(ciCore.zon) == 24.0)
		ciCore.zon = DecDeg2DegMin(DegMin2DecDeg(ciCore.lon) / 15.0);

	if (us.fProgressUS && !us.nRel && !us.fTransitInf && !us.fTransit && !us.fInDay)
	{
		is.JDp = MdytszToJulian(ciMain.mon, ciMain.day, ciMain.yea, ciMain.tim, ciMain.dst, ciMain.zon);
	}

	if (us.fProgressUS && us.fSolarArc == 2)
	{
		us.fSolarArc = 0;
		ci = ciCore;
		Off = ProcessInput(fDate);
		ComputeVariables(&vtx);
		if (us.fGeodetic)		/* Check for -G geodetic chart. */
			is.RA = Deg2Rad(Mod(-ciCore.lon));

		for (mi = 0; mi <= cObj; mi++)
		{
			ignoreT[mi] = ignore[mi];
			ignoreT3[mi] = ignore3[mi];
			ignore[mi] = TRUE;
			ignore3[mi] = TRUE;
		}

		ignore[oSun] = FALSE;
		ignore3[oSun] = FALSE;
		ComputePlanets();
		ComputeWithSwissEphemeris(is.T);
		SunDirPos = cp0.longitude[oSun];

		for (mi = 0; mi <= cObj; mi++)
		{
			ignore[mi] = ignoreT[mi];
			ignore3[mi] = ignoreT3[mi];
		}
		ignore[oSun] = FALSE;
		ignore3[oSun] = FALSE;
		ciCore = ci;
		us.fSolarArc = 2;
	}

	ci = ciCore;
	Longit = DegMin2DecDeg(ciCore.lon);
	Latit = DegMin2DecDeg(ciCore.lat);

	if (ciCore.mon == -1)
	{

		/* Hack: If month is negative, then we know chart was read in through a  */
		/* -o0 position file, so the planet positions are already in the arrays. */

		is.MC = cp0.longitude[oMC];
		is.Asc = cp0.longitude[oAsc];
	}
	else
	{

		for (i = 0; i <= cObj; i++)
		{
			cp0.longitude[i] = cp0.latitude[i] = 0.0;	/* On ecliptic unless we say so.  */
			cp0.vel_longitude[i] = 1.0;					/* Direct until we say otherwise. */
		}

		Off = ProcessInput(fDate);
		ComputeVariables(&vtx);
		if (us.fGeodetic)		/* Check for -G geodetic chart. */
			is.RA = Deg2Rad(Mod(-ciCore.lon));
		is.MC = CuspMidheaven();	/* Calculate our Ascendant & Midheaven. */
		is.Asc = CuspAscendant();

		hRevers = 0;
		if (fabs(ciCore.lat) > Deg2Rad(90.0 - Rad2Deg(is.rObliquity)))
		{
			if (Mod(is.Asc - is.MC) > 180.0)
			{
				hRevers = 1;
			}
		}

		ComputeHouses(us.nHouseSystem); /* Go calculate house cusps. */

		/* Go calculate planet, Moon, and North Node positions. */

		ComputePlanets();
		if (!ignore[oMoo] || !ignore[oNoNode] || !ignore[oSoNode] || !ignore[oFor])
		{
			ComputeLunar(&cp0.longitude[oMoo], &cp0.latitude[oMoo], &cp0.longitude[oNoNode], &cp0.latitude[oNoNode]);
			cp0.vel_longitude[oNoNode] = -1.0;
		}

		/* Compute more accurate ephemeris positions for certain objects. */

		ComputeWithSwissEphemeris(is.T);

		if (us.objCenter != oEar)
		{
			cp0.vel_longitude[oNoNode] = Deg2Rad(-0.053);
			cp0.vel_longitude[oMoo] = Deg2Rad(12.5);
		}

		cp0.longitude[oSoNode] = Mod(cp0.longitude[oNoNode] + 180.0);
		cp0.vel_longitude[oSoNode] = cp0.vel_longitude[oNoNode];

		/* Calculate position of Part of Fortune. */

		j = cp0.longitude[oMoo] - cp0.longitude[oSun];
		if (us.nArabicNight < 0 || (us.nArabicNight == 0 && HousePlaceIn(cp0.longitude[oSun]) < sLib))
			neg(j);
		j = fabs(j) < 90.0 ? j : j - RSgn(j) * 360.0;
		cp0.longitude[oFor] = Mod(j + is.Asc);

		/* Fill in "planet" positions corresponding to house cusps. */

		cp0.longitude[oVtx] = vtx;
		cp0.longitude[oEP] = CuspEastPoint();
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			cp0.longitude[cuspLo + i - 1] = cp0.cusp_pos[i];
		if (!us.fHouseAngle)
		{
			cp0.longitude[oAsc] = is.Asc;
			cp0.longitude[oMC] = is.MC;
			cp0.longitude[oDes] = Mod(is.Asc + 180.0);
			cp0.longitude[oNad] = Mod(is.MC + 180.0);
		}
		for (i = oFor; i <= cuspHi; i++)
			cp0.vel_longitude[i] = Deg2Rad(360.0);
	}

	/* Go calculate star positions if -U switch in effect. */

	if (us.nStar)
		ComputeStars(us.fSidereal ? 0.0 : -Off);

	/* Transform ecliptic to equatorial coordinates if -sr in effect. */

	if (us.fEquator)
	{
		for (i = 0; i <= cObj; i++)
			if (!ignore[i])
			{
				cp0.longitude[i] = Deg2Rad(Tropical(cp0.longitude[i]));
				cp0.latitude[i] = Deg2Rad(cp0.latitude[i]);
				EclToEqu(&cp0.longitude[i], &cp0.latitude[i]);
				cp0.longitude[i] = Rad2Deg(cp0.longitude[i]);
				cp0.latitude[i] = Rad2Deg(cp0.latitude[i]);
			}
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		{
			l = 0.0;
			cp0.cusp_pos[i] = Deg2Rad(Tropical(cp0.cusp_pos[i]));
			EclToEqu(&cp0.cusp_pos[i], &l);
			cp0.cusp_pos[i] = Rad2Deg(cp0.cusp_pos[i]);
		}
	}

	/* Alternative star restrictions - by ecliptic and prime vertical */

	if (us.nStar)
	{
		if (StarRest && !PrimeRest)
		{
			for (i = starLo; i <= starHi; i++)
				ignore[i] = 1;

			if (!us.fParallel)
			{
				for (i = starLo; i <= starHi; i++)
				{
					for (k = 1; k <= uranHi; k++)
						if (!ignore[k])
						{
							for (n = 1; n <= StarAspects; n++)
							{
								l = MinDistance(cp0.longitude[i], cp0.longitude[k]);
								m = l - rAspAngle[n];
								if (fabs(m) < GetOrb(i, k, n))
									ignore[i] = 0;
							}
						}
				}
			}
			else
			{
				for (i = starLo; i <= starHi; i++)
				{
					for (k = 1; k <= uranHi; k++)
						if (!ignore[k] && FThing(k))
						{
							ll = Deg2Rad(cp0.longitude[k]);
							alt1 = Deg2Rad(cp0.latitude[k]);
							EclToEqu(&ll, &alt1);
							alt1 = Rad2Deg(alt1);
							ll = Deg2Rad(cp0.longitude[i]);
							alt2 = Deg2Rad(cp0.latitude[i]);
							EclToEqu(&ll, &alt2);
							alt2 = Rad2Deg(alt2);
							kk = fabs(alt1 - alt2);
							if (kk < GetOrb(i, k, aCon))
								ignore[i] = 0;
							else if (StarAspects > 1)
							{
								kk = fabs(alt2) - fabs(alt1);
								if (fabs(kk) < GetOrb(i, k, aCon))
									ignore[i] = 0;
							}
						}
				}
			}
		}
		if (PrimeRest)
		{
			for (i = 1; i <= cStar; i++)
			{
				ii = starLo + i - 1;
				ignore[ii] = 1;
				if (PrimeRest != 2)
				{
					for (k = 1; k <= uranHi; k++)
						if (!ignore[k] && FThing(k))
						{
							l = StarPrime[i] - ObjPrime[k];
							if (fabs(l) < PrimeOrb1)
								ignore[ii] = 0;
						}
				}
				if (PrimeRest > 1)
				{
					for (k = 0; k <= 8; k++)
					{
						l = StarPrime[i] - k * 45.0;
						if (fabs(l) < PrimeOrb2)
							ignore[ii] = 0;
					}
				}
			}
		}
	}

	/* Now, we may have to modify the base positions we calculated above  */
	/* based on what type of chart we are generating.					  */

	/* Bugfix by VA. When "degree per day/month" charts are calculatd,	  */
	/* is.JDp - is.JD gives wrong difference in time, because is.JD is	  */
	/* calculated for midnigth of birthday (time is set to 00:00:00 GMT), */
	/* so the birthtime isn't used and time difference (in days) is off   */
	/* just by birthtime which gives error in positions up to 10 arcsec.  */
	/* When is.JD below is replaced by JulianDayFromTime(is.T) which is   */
	/* real Julian Day for birthtime, time difference is still off, this  */
	/* time exactly by 12 hours, because Julian Day starts from midday,   */
	/* not from midnight (as in case of is.JDp) and in subroutine		  */
	/* ProcessInput() time is.T is shifted appropriately. (Just compare   */
	/* calculation of is.T in named routine with reverse caclulation	  */
	/* JulianDayFromTime(is.T) and see this shift by 0.5 day). Therefore  */
	/* we have substract here the same 12 ours (or 0.5 days).			  */

	if (us.fProgressUS && us.fSolarArc)
	{							/* Are we doing -p0 solar arc chart? */

		if (us.fSolarArc == 1)
		{
			for (i = 0; i <= cObj; i++)
/*		  planet[i] = Mod(planet[i] + (is.JDp - is.JD) / us.rProgDay);	*/
				cp0.longitude[i] = Mod(cp0.longitude[i] + (is.JDp - JulianDayFromTime(is.T) - 0.5) / us.rProgDay);
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
/*		  chouse[i]  = Mod(chouse[i]  + (is.JDp - is.JD) / us.rProgDay);  */
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + (is.JDp - JulianDayFromTime(is.T) - 0.5) / us.rProgDay);
		}
		if (us.fSolarArc == 2)
		{
			SunDirection = SunDirPos - cp0.longitude[oSun];
			for (i = 0; i <= cObj; i++)
				cp0.longitude[i] = Mod(cp0.longitude[i] + SunDirection);
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + SunDirection);

			for (mi = 0; mi <= cObj; mi++)
			{
				ignore[mi] = ignoreT[mi];
				ignore3[mi] = ignoreT3[mi];
			}
		}
	}
	if (us.nHarmonic > 1.0)		/* Are we doing a -x harmonic chart?	 */
		for (i = 0; i <= cObj; i++)
			cp0.longitude[i] = Mod(cp0.longitude[i] *  us.nHarmonic);
	if (us.objOnAsc)
	{
		if (us.objOnAsc > 0)	/* Is -1 put on Ascendant in effect?	 */
			j = cp0.longitude[us.objOnAsc] - is.Asc;
		else					/* Or -2 put object on Midheaven switch? */
			j = cp0.longitude[-us.objOnAsc] - is.MC;
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)	/* If so, rotate the houses accordingly. */
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + j);
	}

	/* Check to see if we are -F forcing any objects to be particular values. */

	for (i = 0; i <= cObj; i++)
		if (force[i] != 0.0)
		{
			cp0.longitude[i] = force[i] - 360.0;
			cp0.latitude[i] = cp0.vel_longitude[i] = 0.0;
		}

	ComputeInHouses();			/* Figure out what house everything falls in. */

	/* If -f domal chart switch in effect, switch planet and house positions. */

	if (us.fFlip)
	{
		for (i = 0; i <= cObj; i++)
		{
			k = cp0.house_no[i];
			cp0.house_no[i] = Z2Sign(cp0.longitude[i]);
			cp0.longitude[i] = Sign2Z(k) + MinDistance(cp0.cusp_pos[k], cp0.longitude[i]) /
				MinDistance(cp0.cusp_pos[k], cp0.cusp_pos[Mod12(k + 1)]) * 30.0;
		}
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		{
			k = HousePlaceIn(Sign2Z(i));
			housetemp[i] = Sign2Z(k) + MinDistance(cp0.cusp_pos[k], Sign2Z(i)) /
				MinDistance(cp0.cusp_pos[k], cp0.cusp_pos[Mod12(k + 1)]) * 30.0;
		}
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			cp0.cusp_pos[i] = housetemp[i];
	}

	/* If -3 decan chart switch in effect, edit planet positions accordingly. */

	if (us.fDecan)
	{
		for (i = 0; i <= cObj; i++)
			cp0.longitude[i] = Decan(cp0.longitude[i]);
		ComputeInHouses();
	}

	/* If -9 navamsa chart switch in effect, edit positions accordingly. */

	if (us.fNavamsa)
	{
		for (i = 0; i <= cObj; i++)
			cp0.longitude[i] = Navamsa(cp0.longitude[i]);
		ComputeInHouses();
	}

	ciCore = ci;
	return is.T;
}


/* Calculate the position of each planet with respect to the Gauquelin		*/
/* sectors. This is used by the sector charts. Fill out the planet position */
/* array where one degree means 1/10 the way across one of the 36 sectors.	*/

void CastSectors()
{
	int source[MAXINDAY], type[MAXINDAY], occurcount, division, div, i, j, s1, s2, ihouse, fT;
	double time[MAXINDAY], rgalt1[NUMBER_OBJECTS], rgalt2[NUMBER_OBJECTS];
	double azi1, azi2, alt1, alt2, lon, lat, mc1, mc2, d, k;
	byte EquT, MCpolarT;

	/* If the -l0 approximate sectors flag is set, we can quickly get rough   */
	/* positions by having each position be the location of the planet as	  */
	/* mapped into Placidus houses. The -f flip houses flag does this for us. */

	if (us.fSectorApprox)
	{
		ihouse = us.nHouseSystem;
		us.nHouseSystem = cHousePlacidus;
		not(us.fFlip);
		CastChart(TRUE);
		not(us.fFlip);
		us.nHouseSystem = ihouse;
		return;
	}

	/* If not approximating sectors, then they need to be computed the formal */
	/* way: based on a planet's nearest rising and setting times. The code	  */
	/* below is similar to ChartInDayHorizon() accessed by the -Zd switch.	  */

	fT = us.fSidereal;
	us.fSidereal = FALSE;
	EquT = us.fEquator;
	us.fEquator = FALSE;
	MCpolarT = PolarMCflip;
	PolarMCflip = FALSE;
	lon = Deg2Rad(Mod(ciMain.lon));
	lat = Deg2Rad(ciMain.lat);
	division = us.nDivision * 4;
	occurcount = 0;

	/* Start scanning from 18 hours before to 18 hours after the time of the */
	/* chart in question, to find the closest rising and setting times. 	 */

	ciCore = ciMain;
	ciCore.tim -= 18.0;
	if (ciCore.tim < 0.0)
	{
		ciCore.tim += 24.0;
		ciCore.day--;
	}
	CastChart(TRUE);
	mc2 = Deg2Rad(cp0.longitude[oMC]);
	k = Deg2Rad(cp0.latitude[oMC]);
	EclToEqu(&mc2, &k);
	cp2 = cp0;
	for (i = 1; i <= cObj; i++)
	{
		rgalt2[i] = cp0.latitude[i];
	}

	/* Loop through 36 hours, dividing it into a certain number of segments. */
	/* For each segment we get the planet positions at its endpoints.		 */

	for (div = 1; div <= division; div++)
	{
		ciCore = ciMain;
		ciCore.tim = DegMin2DecDeg(ciCore.tim) - 18.0 + 36.0 * (double) div / (double) division;
		if (ciCore.tim < 0.0)
		{
			ciCore.tim += 24.0;
			ciCore.day--;
		}
		else if (ciCore.tim >= 24.0)
		{
			ciCore.tim -= 24.0;
			ciCore.day++;
		}
		ciCore.tim = DecDeg2DegMin(ciCore.tim);
		CastChart(TRUE);
		mc1 = mc2;
		mc2 = Deg2Rad(cp0.longitude[oMC]);
		k = Deg2Rad(cp0.latitude[oMC]);
		EclToEqu(&mc2, &k);
		cp1 = cp2;
		cp2 = cp0;
		for (i = 1; i <= cObj; i++)
		{
			rgalt1[i] = rgalt2[i];
			rgalt2[i] = cp0.latitude[i];
		}

		/* During our segment, check to see if each planet rises or sets. */

		for (i = 0; i <= cObj; i++)
			if (!FIgnore(i) && FThing(i))
			{
				EclToHorizon(&azi1, &alt1, cp1.longitude[i], rgalt1[i], lon, lat, mc1);
				EclToHorizon(&azi2, &alt2, cp2.longitude[i], rgalt2[i], lon, lat, mc2);
				j = 0;
				if ((alt1 > 0.0) != (alt2 > 0.0))
				{
					d = fabs(alt1) / (fabs(alt1) + fabs(alt2));
					k = Mod(azi1 + d * MinDifference(azi1, azi2));
					j = 1 + (MinDistance(k, 180.0) < 90.0);
				}
				if (j && occurcount < MAXINDAY)
				{
					source[occurcount] = i;
					type[occurcount] = j;
					time[occurcount] = 36.0 * ((double) (div - 1) + d) / (double) division *60.0;

					occurcount++;
				}
			}
	}

	/* Sort each event in order of time when it happens during the day. */

	for (i = 1; i < occurcount; i++)
	{
		j = i - 1;
		while (j >= 0 && time[j] > time[j + 1])
		{
			SwapN(source[j], source[j + 1]);
			SwapN(type[j], type[j + 1]);
			SwapR(time[j], time[j + 1]);
			j--;
		}
	}

	/* Now fill out the planet array with the appropriate sector location. */

	for (i = 1; i <= cObj; i++)
		if (!ignore[i] && FThing(i))
		{
			cp0.longitude[i] = 0.0;
			/* Search for the first rising or setting event of our planet. */
			for (s2 = 0; s2 < occurcount && source[s2] != i; s2++)
				;
			if (s2 == occurcount)
			{
			  LFail:
				/* If we failed to find a rising/setting bracket around our time, */
				/* automatically restrict that planet so it doesn't show up.	  */
				ignore[i] = TRUE;
				continue;
			}
		  LRetry:
			/* One rising or setting event was found. Now search for the next one. */
			s1 = s2;
			for (s2 = s1 + 1; s2 < occurcount && source[s2] != i; s2++)
				;
			if (s2 == occurcount)
				goto LFail;
			/* Reject the two events if either (1) they're both the same, i.e. both */
			/* rising or both setting, or (2) they don't bracket the chart's time.	*/
			if (type[s2] == type[s1] || time[s1] > 18.0 * 60.0 || time[s2] < 18.0 * 60.0)
				goto LRetry;
			/* Cool, we've found our rising/setting bracket. The sector position is */
			/* the proportion the chart time is between the two event times.		*/
			cp0.longitude[i] = (18.0 * 60.0 - time[s1]) / (time[s2] - time[s1]) * 180.0;
			if (type[s1] == 2)
				cp0.longitude[i] += 180.0;
			cp0.longitude[i] = Mod(360.0 - cp0.longitude[i]);
		}

	/* Save Gauquelin's cp0 to cp2 for graphics infoborder.  */

	cp2 = cp0;

	/* Restore original chart info as we've overwritten it. */

	ciCore = ciMain;
	us.fSidereal = fT;
	us.fEquator = EquT;
	PolarMCflip = MCpolarT;
}


/*
******************************************************************************
** Aspect Calculations.
******************************************************************************
*/

/* Set up the aspect/midpoint grid. Allocate memory for this array, if not */
/* already done. Allocation is only done once, first time this is called.  */

BOOL FEnsureGrid()
{
	if (grid != NULL)
		return TRUE;
	grid = (GridInfo *) allocate(sizeof(GridInfo), "grid");
	return grid != NULL;
}


/* Indicate whether some aspect between two objects should be shown. */

BOOL FAcceptAspect(int obj1, int asp, int obj2) {
	int fSupp;

	if (ignoreA[asp])			/* If the aspect restricted, reject immediately. */
		return FALSE;

	if (us.nRel > rcDual && FCusp(obj1) && FCusp(obj2))
		return FALSE;

	if (FStar(obj1) && FStar(obj2))
		return FALSE;

	if (us.fSmartAspects)
	{

		/* Allow only conjunctions to the minor house cusps. */

		if ((FMinor(obj1) || FMinor(obj2)) && asp > aCon)
			return FALSE;

		/* Prevent any simultaneous aspects to opposing angle cusps,	 */
		/* e.g. if conjunct one, don't be opposite the other; if trine	 */
		/* one, don't sextile the other; don't square both at once, etc. */

		fSupp = (asp == aOpp || asp == aSex || asp == aSSx || asp == aSes);

		if ((FAngle(obj1) || FAngle(obj2)) &&
			(fSupp || (asp == aSqu && (obj1 == oDes || obj2 == oDes || obj1 == oNad || obj2 == oNad))))
			return FALSE;

		// Prevent any simultaneous aspects to the North and South Node, or 
		// aspects between the nodes. North Node will always be given the 
		// priority for aspects. An aspect is prevented if there exists another
		// aspect defined in Astrolog, the sum of which totals 180 degrees.

		// This test is not foolproof, because if the user enables biquintiles
		// but disables semiquintiles, for example, he will miss one aspect.

		if (obj1 == oNoNode && obj2 == oSoNode || 
			((obj1 == oSoNode || obj2 == oSoNode) && 
			(asp >= aCon && asp <= aSQn && asp != aQui)))
			return FALSE;
	}
	return TRUE;
}


/* This is a subprocedure of FCreateGrid() and FCreateGridRelation().	*/
/* Given two planets, determine what aspect, if any, is present between */
/* them, and save the aspect name and orb in the specified grid cell.	*/

void GetAspect(double *planet1, double *planet2, double *ret1, double *ret2, int i, int j) {
	int k;
	double l, m;

	grid->v[i][j] = grid->n[i][j] = 0;
	l = MinDistance(planet2[i], planet1[j]);

	for (k = cAspect; k >= 1; k--)
	{
		if (!FAcceptAspect(i, k, j))
			continue;
		m = l - rAspAngle[k];
		if (fabs(m) < GetOrb(i, j, k))
		{
			grid->n[i][j] = k;

			/* If -ga switch in effect, then change the sign of the orb to	  */
			/* correspond to whether the aspect is applying or separating.	  */
			/* To do this, we check the velocity vectors to see if the		  */
			/* planets are moving toward, away, or are overtaking each other. */

			if (us.fAppSep)
				m = RSgn2(ret1[j] - ret2[i]) * RSgn2(MinDifference(planet2[i], 
				planet1[j])) * RSgn2(m) * fabs(m);
			grid->v[i][j] = (int) (m * 60.0);
		}
	}
}


/* Very similar to GetAspect(), this determines if there is a parallel or */
/* contraparallel aspect between the given two planets, and stores the	  */
/* result as above. The settings and orbs for conjunction are used for	  */
/* parallel and those for opposition are used for contraparallel.		  */

void GetParallel(double *planet1, double *planet2, double *planetalt1, double *planetalt2, double *ret1, double *ret2, double *altret1, double *altret2, int i, int j) {
	int k;
	double l, alt1, alt2, latdiri, latdirj;

	/* Planet 1 */
	l = Deg2Rad(planet1[j]);
	alt1 = Deg2Rad(planetalt1[j]);
	/* No conversion to Geo-equatorial in a heliocentric chart */
	if (us.objCenter != oSun)
		EclToEqu(&l, &alt1);
	alt1 = Rad2Deg(alt1);

	/* Planet 2*/
	l = Deg2Rad(planet2[i]);
	alt2 = Deg2Rad(planetalt2[i]);
	/* No conversion to Geo-equatorial in a heliocentric chart */
	if (us.objCenter != oSun)
		EclToEqu(&l, &alt2);
	/* */
	alt2 = Rad2Deg(alt2);

	grid->v[i][j] = grid->n[i][j] = 0;
	for (k = aOpp; k >= 1; k--)
	{
		if (!FAcceptAspect(i, k, j))
			continue;
		l = (k == aCon ? fabs(alt1 - alt2) : fabs(alt2) - 
			fabs(alt1));
		if (fabs(l) < GetOrb(i, j, k))
		{
			grid->n[i][j] = k;

			if (us.fAppSep)
			{
				latdiri = ret2[i] * RCosD(planet2
					[i]) * RSinD(Rad2Deg(is.rObliquity))
					+ altret2[i] * RCosD(Rad2Deg
					(is.rObliquity) - fabs(planetalt2[i] - alt2));
				latdirj = ret1[j] * RCosD(planet1
					[j]) * RSinD(Rad2Deg(is.rObliquity))
					+ altret1[j] * RCosD(Rad2Deg
					(is.rObliquity) - fabs(planetalt1[j] - alt1));
				l = RSgn2(latdirj - latdiri) * RSgn2
					(alt1 - alt2) * RSgn2(l) * fabs(l);
			}
			grid->v[i][j] = (int) (l * 60.0);
		}
	}
}



/* Fill in the aspect grid based on the aspects taking place among the */
/* planets in the present chart. Also fill in the midpoint grid.	   */

BOOL FCreateGrid(BOOL fFlip) {
	int i, j, k;
	double l;

	if (!FEnsureGrid())
		return FALSE;

	memset(&grid->n, 0, sizeof(grid->n));

	for (j = 0; j <= cObj; j++)
		if (!FIgnore(j))
			for (i = 0; i <= cObj; i++)
				if (!FIgnore(i))
				{

					/* The parameter 'flip' determines what half of the grid is filled in */
					/* with the aspects and what half is filled in with the midpoints.	  */

					if (fFlip ? i > j : i < j)
					{
						if (us.fParallel)
							GetParallel(cp0.longitude, cp0.longitude, cp0.latitude, cp0.latitude, cp0.vel_longitude, cp0.vel_longitude, 
								cp0.vel_latitude, cp0.vel_latitude, i, j);
						else
							GetAspect(cp0.longitude, cp0.longitude, cp0.vel_longitude, cp0.vel_longitude, i, j);
					}
					else if (fFlip ? i < j : i > j)
					{
						l = Mod(Midpoint(cp0.longitude[i], cp0.longitude[j]));
						k = (int) l;				/* Calculate */
						grid->n[i][j] = k / 30 + 1; /* midpoint. */
						grid->v[i][j] = (int) ((l - (double) (k / 30) * 30.0) * 60.0);
					}
					else
					{
						grid->n[i][j] = Z2Sign(cp0.longitude[j]);
						grid->v[i][j] = (int) (cp0.longitude[j] - (double) (grid->n[i][j] - 1) * 30.0);
					}
				}

	return TRUE;
}

/* This is similar to the previous function; however, this time fill in the */
/* grid based on the aspects (or midpoints if 'acc' set) taking place among */
/* the planets in two different charts, as in the -g -r0 combination.		*/

BOOL FCreateGridRelation(BOOL fMidpoint) {
	int i, j, k;
	double l;

	if (!FEnsureGrid())
		return FALSE;
	for (j = 0; j <= cObj; j++)
		if (!FIgnore(j) || !FIgnore2(j) || !FIgnore3(j))
			for (i = 0; i <= cObj; i++)
				if (!FIgnore(i) || !FIgnore2(i) || !FIgnore3(i))
					if (!fMidpoint)
					{
						if (us.fParallel)
							GetParallel(cp1.longitude, cp2.longitude, cp1.latitude, cp2.latitude,
										cp1.vel_longitude, cp2.vel_longitude, cp1.vel_latitude, cp2.vel_latitude, i, j);
						else
							GetAspect(cp1.longitude, cp2.longitude, cp1.vel_longitude, cp2.vel_longitude, i, j);
					}
					else
					{
						l = Mod(Midpoint(cp2.longitude[i], cp1.longitude[j]));
						k = (int) l;	/* Calculate */
						grid->n[i][j] = k / 30 + 1; /* midpoint. */
						grid->v[i][j] = (int) ((l - (double) (k / 30) * 30.0) * 60.0);
					}
	return TRUE;
}


/*
******************************************************************************
** Other Calculations.
******************************************************************************
*/

/* Fill out tables based on the number of unrestricted planets in signs by	*/
/* element, signs by mode, as well as other values such as the number of	*/
/* objects in yang vs. yin signs, in various house hemispheres (north/south */
/* and east/west), and the number in first six signs vs. second six signs.	*/
/* This is used by the -v chart listing and the sidebar in graphics charts. */

void CreateElemTable(ET *pet) {
	int i, s;

	memset(pet, 0, sizeof(ET));
	for (i = 0; i <= cObj; i++)
		if (!FIgnore(i))
		{
			pet->coSum++;
			s = Z2Sign(cp0.longitude[i]);
			pet->coSign[s - 1]++;
			pet->coElemMode[(s - 1) & 3][(s - 1) % 3]++;
			pet->coElem[(s - 1) & 3]++;
			pet->coMode[(s - 1) % 3]++;
			pet->coYang += (s & 1);
			pet->coLearn += (s < sLib);
			if (!FCusp(i))
			{
				pet->coHemi++;
				s = cp0.house_no[i];
				pet->coHouse[s - 1]++;
				pet->coModeH[(s - 1) % 3]++;
				pet->coMC += (s >= sLib);
				pet->coAsc += (s < sCan || s >= sCap);
			}
		}
	pet->coYin = pet->coSum - pet->coYang;
	pet->coShare = pet->coSum - pet->coLearn;
	pet->coDes = pet->coHemi - pet->coAsc;
	pet->coIC = pet->coHemi - pet->coMC;
}

/* calc.c */
