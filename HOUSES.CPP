/*
** Astrolog  File: houses.c
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
** Auxiliary Functions.
******************************************************************************
*/

static double cusp_topocentric(double deg)
{
	double OA, X, LO;

	OA = ModRad(is.RA + Deg2Rad(deg));
	X = atan(tan(ciCore.lat) / cos(OA));
	LO = atan(cos(X) * tan(OA) / cos(X + is.rObliquity));
	if (LO < 0.0)
		LO += rPi;
	if (sin(OA) < 0.0)
		LO += rPi;
	return LO;
}


static double cusp_placidus(double deg, double FF, BOOL fNeg)
{
	double LO, R1, XS, X;
	int i;

	R1 = is.RA + Deg2Rad(deg);
	X = fNeg ? 1.0 : -1.0;
	/* Looping 10 times is arbitrary, but it's what other programs do. */
	for (i = 1; i <= 10; i++)
	{

		/* This formula works except at 0 latitude (AA == 0.0). */

		XS = X * sin(R1) * tan(is.rObliquity) * tan(ciCore.lat == 0.0 ? 0.0001 : ciCore.lat);
		XS = acos(XS);
		if (XS < 0.0)
			XS += rPi;
		R1 = is.RA + (fNeg ? rPi - (XS / FF) : (XS / FF));
	}
	LO = atan(tan(R1) / cos(is.rObliquity));
	if (LO < 0.0)
		LO += rPi;
	if (sin(R1) < 0.0)
		LO += rPi;
	return Rad2Deg(LO);
}


/*
******************************************************************************
** House Calculations.
******************************************************************************
*/



/* Compute the cusp positions using the Alcabitius house system. */

void HouseAlcabitius()
{
	double rDecl, rSda, rSna, r, hr, hr2;
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		hRevers = 2;
	}

	rDecl = asin(sin(is.rObliquity) * RSinD(is.Asc));
	r = -tan(ciCore.lat) * tan(rDecl);
	rSda = Rad2Deg(acos(r));
	rSna = 180.0 - rSda;
	cp0.cusp_pos[sLib] = Rad2Deg(is.RA) - rSna;
	cp0.cusp_pos[sSco] = Rad2Deg(is.RA) - rSna * 2.0 / 3.0;
	cp0.cusp_pos[sSag] = Rad2Deg(is.RA) - rSna / 3.0;
	cp0.cusp_pos[sCap] = Rad2Deg(is.RA);
	cp0.cusp_pos[sAqu] = Rad2Deg(is.RA) + rSda / 3.0;
	cp0.cusp_pos[sPis] = Rad2Deg(is.RA) + rSda * 2.0 / 3.0;

	for (i = sLib; i <= sPis; i++)
	{
		hr = Deg2Rad(cp0.cusp_pos[i]);
		hr2 = atan(tan(hr) / cos(is.rObliquity));
		if (hr2 < 0.0)
			hr2 += rPi;
		if (sin(hr) < 0.0)
			hr2 += rPi;
		cp0.cusp_pos[i] = Rad2Deg(hr2);
		cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + is.rSid);
	}
	for (i = sAri; i <= sVir; i++)
		cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i + 6] + 180.0);

	if (hRevers && PolarMCflip)
	{
		is.MC = Mod(is.MC - 180.0);
		for (i = 2; i <= 6; i++)
		{
			r = cp0.cusp_pos[i];
			cp0.cusp_pos[i] = cp0.cusp_pos[14 - i];
			cp0.cusp_pos[14 - i] = r;
		}
	}
}

void HouseCampanus()
{
	double KO, DN, X;
	int i;

	if (hRevers)
	{
		hRevers = 2;
		is.Asc = Mod(is.Asc - 180.0);
	}
	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		if (hRevers && !PolarMCflip)
			KO = Deg2Rad(300.000001 - 30.0 * (double) i);
		else
			KO = Deg2Rad(60.000001 + 30.0 * (double) i);
		DN = atan(tan(KO) * cos(ciCore.lat));
		if (DN < 0.0)
			DN += rPi;
		if (sin(KO) < 0.0)
			DN += rPi;
		X = Angle(cos(is.RA + DN) * cos(is.rObliquity) - sin(DN) * tan(ciCore.lat) * sin(is.rObliquity), sin(is.RA + DN));
		cp0.cusp_pos[i] = Mod(Rad2Deg(X) + is.rSid);
	}
	if (hRevers && PolarMCflip)
	{
		is.MC = Mod(is.MC - 180.0);
		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] - 180.0);
	}
}

void HouseEqual()
{
	int i;

	if (hRevers)
	{
		hRevers = 2;
		is.Asc = Mod(is.Asc - 180.0);
		if (PolarMCflip)
			is.MC = Mod(is.MC - 180.0);
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		if (hRevers && PolarMCflip)
			cp0.cusp_pos[i] = Mod(is.Asc + 30.0 - 30.0 * (double) i);
		else
			cp0.cusp_pos[i] = Mod(is.Asc - 30.0 + 30.0 * (double) i);
	}
}

/* This house system is just like the Equal system except that we start */
/* our 12 equal segments from the Midheaven instead of the Ascendant.	*/

void HouseEqualMidheaven()
{
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		if (PolarMCflip)
		{
			hRevers = 2;
			is.MC = Mod(is.MC - 180.0);
		}
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		if (hRevers && PolarMCflip)
			cp0.cusp_pos[i] = Mod(is.MC - 90.0 - 30.0 * (double) (i - 1));
		else
			cp0.cusp_pos[i] = Mod(is.MC - 270.0 + 30.0 * (double) (i - 1));
	}
}

void HouseKoch()
{
	double A1, A2, A3, KN, D, X;
	int i;

	A1 = sin(is.RA) * tan(ciCore.lat) * tan(is.rObliquity);
	A1 = asin(A1);
	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		D = Mod(60.0 + 30.0 * (double) i);
		A2 = D / 90.0 - 1.0;
		KN = 1.0;
		if (D >= 180.0)
		{
			KN = -1.0;
			A2 = D / 90.0 - 3.0;
		}
		A3 = Deg2Rad(Mod(Rad2Deg(is.RA) + D + A2 * Rad2Deg(A1)));
		X = Angle(cos(A3) * cos(is.rObliquity) - KN * tan(ciCore.lat) * sin(is.rObliquity), sin(A3));
		cp0.cusp_pos[i] = Mod(Rad2Deg(X) + is.rSid);
	}
}

void HouseMeridian()
{
	double D, X;
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		if (PolarMCflip)
		{
			is.MC = Mod(is.MC - 180.0);
			hRevers = 2;
		}
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		if (hRevers && PolarMCflip)
			D = Deg2Rad(300.0 - 30.0 * (double) i);
		else
			D = Deg2Rad(60.0 + 30.0 * (double) i);
		X = Angle(cos(is.RA + D) * cos(is.rObliquity), sin(is.RA + D));
		cp0.cusp_pos[i] = Mod(Rad2Deg(X) + is.rSid);
		if (hRevers && PolarMCflip)
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] - 180.0);
	}
}

void HouseMorinus()
{
	double D, X;
	int i;

	if (hRevers)
	{
		hRevers = 2;
		is.Asc = Mod(is.Asc - 180.0);
		if (PolarMCflip)
			is.MC = Mod(is.MC - 180.0);
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		if (hRevers && PolarMCflip)
			D = Deg2Rad(120.0 - 30.0 * (double) i);
		else
			D = Deg2Rad(60.0 + 30.0 * (double) i);
		X = Angle(cos(is.RA + D), sin(is.RA + D) * cos(is.rObliquity));
		cp0.cusp_pos[i] = Mod(Rad2Deg(X) + is.rSid);
	}
}

/* This is a new house system similar in philosophy to Porphyry houses.   */
/* Instead of just trisecting the difference in each quadrant, we do a	  */
/* smooth sinusoidal distribution of the difference around all the cusps. */

void HouseNeoPorphyry()
{
	double delta;
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		hRevers = 2;
	}

	if (hRevers && PolarMCflip)
	{
		is.MC = Mod(is.MC - 180.0);
		delta = (90.0 - MinDistance(is.MC, is.Asc)) / 4.0;
		cp0.cusp_pos[sLib] = Mod(is.Asc + 180.0);
		cp0.cusp_pos[sCap] = is.MC;
		cp0.cusp_pos[sAqu] = Mod(cp0.cusp_pos[sCap] - 30.0 + delta + is.rSid);
		cp0.cusp_pos[sPis] = Mod(cp0.cusp_pos[sAqu] - 30.0 + delta * 2 + is.rSid);
		cp0.cusp_pos[sSag] = Mod(cp0.cusp_pos[sCap] + 30.0 + delta + is.rSid);
		cp0.cusp_pos[sSco] = Mod(cp0.cusp_pos[sSag] + 30.0 + delta * 2 + is.rSid);
		for (i = sAri; i < sLib; i++)
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i + 6] - 180.0);

	}
	else
	{

		delta = (MinDistance(is.MC, is.Asc) - 90.0) / 4.0;
		cp0.cusp_pos[sLib] = Mod(is.Asc + 180.0);
		cp0.cusp_pos[sCap] = is.MC;
		cp0.cusp_pos[sAqu] = Mod(cp0.cusp_pos[sCap] + 30.0 + delta + is.rSid);
		cp0.cusp_pos[sPis] = Mod(cp0.cusp_pos[sAqu] + 30.0 + delta * 2 + is.rSid);
		cp0.cusp_pos[sSag] = Mod(cp0.cusp_pos[sCap] - 30.0 + delta + is.rSid);
		cp0.cusp_pos[sSco] = Mod(cp0.cusp_pos[sSag] - 30.0 + delta * 2 + is.rSid);
		for (i = sAri; i < sLib; i++)
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i + 6] - 180.0);
	}
}

/* In "null" houses, the cusps are always fixed to start at their cor-	  */
/* responding sign, i.e. the 1st house is always at 0 degrees Aries, etc. */

void HouseNull()
{
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		is.MC = Mod(is.MC - 180.0);
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		cp0.cusp_pos[i] = Mod(Sign2Z(i));
	}
}

void HousePlacidus()
{
	int i;

	cp0.cusp_pos[1] = Mod(is.Asc - is.rSid);
	cp0.cusp_pos[4] = Mod(is.MC + 180.0 - is.rSid);
	cp0.cusp_pos[5] = cusp_placidus(30.0, 3.0, FALSE) + 180.0;
	cp0.cusp_pos[6] = cusp_placidus(60.0, 1.5, FALSE) + 180.0;
	cp0.cusp_pos[2] = cusp_placidus(120.0, 1.5, TRUE);
	cp0.cusp_pos[3] = cusp_placidus(150.0, 3.0, TRUE);
	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		if (i <= 6)
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + is.rSid);
		else
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i - 6] + 180.0);
	}
}

void HousePorphyry()
{
	double X, Y;
	int i;

	if (hRevers)
	{
		hRevers = 2;
		is.Asc = Mod(is.Asc - 180.0);
		if (PolarMCflip)
			is.MC = Mod(is.MC - 180.0);
	}

	X = is.Asc - is.MC;
	if (hRevers && PolarMCflip)
		X = -X;
	if (X < 0.0)
		X += 360.0;
	Y = X / 3.0;
	for (i = 1; i <= 2; i++)
	{
		if (hRevers && PolarMCflip)
			cp0.cusp_pos[i + 4] = Mod(180.0 + is.MC - i * Y);
		else
			cp0.cusp_pos[i + 4] = Mod(180.0 + is.MC + i * Y);
	}
	X = Mod(180.0 + is.MC) - is.Asc;
	if (hRevers && PolarMCflip)
		X = -X;
	if (X < 0.0)
		X += 360.0;
	cp0.cusp_pos[1] = is.Asc;
	Y = X / 3.0;
	for (i = 1; i <= 3; i++)
	{
		if (hRevers && PolarMCflip)
			cp0.cusp_pos[i + 1] = Mod(is.Asc - i * Y);
		else
			cp0.cusp_pos[i + 1] = Mod(is.Asc + i * Y);
	}
	for (i = 1; i <= 6; i++)
		cp0.cusp_pos[i + 6] = Mod(cp0.cusp_pos[i] + 180.0);
}

void HouseRegiomontanus()
{
	double D, X;
	int i;

	if (hRevers)
	{
		hRevers = 2;
		is.Asc = Mod(is.Asc - 180.0);
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		if (hRevers && !PolarMCflip)
			D = Deg2Rad(300.0 - 30.0 * (double) i);
		else
			D = Deg2Rad(60.0 + 30.0 * (double) i);
		X = Angle(cos(is.RA + D) * cos(is.rObliquity) - sin(D) * tan(ciCore.lat) * sin(is.rObliquity), sin(is.RA + D));
		cp0.cusp_pos[i] = Mod(Rad2Deg(X) + is.rSid);
	}
	if (hRevers && PolarMCflip)
	{
		is.MC = Mod(is.MC - 180.0);
		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] - 180.0);
	}
}

void HouseShripati(void)
{ 
	double		asc_mc;
	int			i;


	asc_mc = swe_difdeg2n(is.Asc, is.MC);

	if (asc_mc < 0)
	{
		// within polar circle we swap AC/DC if AC is on wrong side
		is.Asc = swe_degnorm(is.Asc + 180);
		asc_mc = swe_difdeg2n(is.Asc, is.MC);
	}

	cp0.cusp_pos[10] = swe_degnorm(is.MC + asc_mc / 6 - 30);
	cp0.cusp_pos[11] = swe_degnorm(cp0.cusp_pos[10] + 30);
	cp0.cusp_pos[12] = swe_degnorm(is.MC + asc_mc / 2);
	cp0.cusp_pos[1] = swe_degnorm(is.Asc - asc_mc / 6);
	cp0.cusp_pos[2] = swe_degnorm(cp0.cusp_pos[1] + 30);
	cp0.cusp_pos[3] = swe_degnorm(cp0.cusp_pos[12] + 90);

	// do the remaining cusps
	for (i = 4; i <= 9; i++)
	{
		cp0.cusp_pos[i] = swe_degnorm(cp0.cusp_pos[mod12(i + 6)] + 180);
	}
}

void HouseTopocentric()
{
	double TL, P1, P2, LT;
	int i;

	cp0.cusp_pos[4] = ModRad(Deg2Rad(is.MC + 180.0 - is.rSid));
	TL = tan(ciCore.lat);
	P1 = atan(TL / 3.0);
	P2 = atan(TL / 1.5);
	LT = ciCore.lat;

	if (hRevers && !PolarMCflip)
	{
		hRevers = 2;
		is.Asc = Mod(is.Asc - 180.0);

		ciCore.lat = P1;
		cp0.cusp_pos[3] = cusp_topocentric(30.0) + rPi;
		ciCore.lat = P2;
		cp0.cusp_pos[2] = cusp_topocentric(60.0) + rPi;
		ciCore.lat = LT;
		cp0.cusp_pos[1] = cusp_topocentric(90.0) + rPi;
		ciCore.lat = P2;
		cp0.cusp_pos[6] = cusp_topocentric(120.0);
		ciCore.lat = P1;
		cp0.cusp_pos[5] = cusp_topocentric(150.0);
		ciCore.lat = LT;
		for (i = 1; i <= 6; i++)
		{
			cp0.cusp_pos[i] = Mod(Rad2Deg(cp0.cusp_pos[i]) + is.rSid);
			cp0.cusp_pos[i + 6] = Mod(cp0.cusp_pos[i] + 180.0);
		}
	}
	else
	{

		ciCore.lat = P1;
		cp0.cusp_pos[5] = cusp_topocentric(30.0) + rPi;
		ciCore.lat = P2;
		cp0.cusp_pos[6] = cusp_topocentric(60.0) + rPi;
		ciCore.lat = LT;
		cp0.cusp_pos[1] = cusp_topocentric(90.0);
		ciCore.lat = P2;
		cp0.cusp_pos[2] = cusp_topocentric(120.0);
		ciCore.lat = P1;
		cp0.cusp_pos[3] = cusp_topocentric(150.0);
		ciCore.lat = LT;
		for (i = 1; i <= 6; i++)
		{
			cp0.cusp_pos[i] = Mod(Rad2Deg(cp0.cusp_pos[i]) + is.rSid);
			cp0.cusp_pos[i + 6] = Mod(cp0.cusp_pos[i] + 180.0);
		}
		if (hRevers && PolarMCflip)
		{
			hRevers = 2;
			is.Asc = Mod(is.Asc - 180.0);
			is.MC = Mod(is.MC - 180.0);
			for (i = 1; i <= NUMBER_OF_HOUSES; i++)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] - 180.0);
		}
	}
}





/* The "Vedic" house system is like the Equal system except each house		*/
/* starts 15 degrees earlier. The Asc falls in the middle of the 1st house. */

void HouseVedic()
{
	int i;

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		cp0.cusp_pos[i] = Mod(is.Asc - 15.0 + Sign2Z(i));
	}
}


/* The "Whole" house system is like the Equal system with 30 degree houses, */
/* where the 1st house starts at zero degrees of the sign of the Ascendant. */

void HouseWhole()
{
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		if (PolarMCflip)
			is.MC = Mod(is.MC - 180.0);
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
		cp0.cusp_pos[i] = Mod((double) ((Z2Sign(is.Asc) - 1) * 30) + Sign2Z(i));
}



// converts an astrolog house system number to Swiss Ephemeris house system number
// returns Swiss Ephemeris house system number, or -1 if conversion not possible
int AstrologHouse2SeHouse(int astrolog_house)
{
	int swiss_house;

	switch(astrolog_house)
	{
	case cHousePlacidus:
		swiss_house = 'P';
		break;

	case cHouseKoch:
		swiss_house = 'K';
		break;

	case cHouseEqual:
		swiss_house = 'E';
		break;

	case cHouseVedic:
		swiss_house = 'V';	// Vehlow equal houses - ascendant in the middle of first house
		break;

	case cHousePorphyry:
		swiss_house = 'O';
		break;

	case cHouseRegiomontanus:
		swiss_house = 'R';
		break;

	case cHouseCampanus:
		swiss_house = 'C';
		break;

	case cHouseMorinus:
		swiss_house = 'M';
		break;

	case cHouseAlcabitius:
		swiss_house = 'B';
		break;

	case cHouseTopocentric:
		swiss_house = 'T';	// Polich/Page topocentric system
		break;

	case cHouseMeridian:
		swiss_house = 'X';	// axial rotation system
		break;

	// there is no equivalent in Astrolog32 to 'H' azimuthal or horizontal system 

	case cHouseNeoPorphyry: // this may be the "Natural Gradation" system created by Colin Evans, and which
							// is briefly described in "The New Waite's Compendium of Natal Astrology"
	case cHouseEqualMidheaven:
	case cHouseWhole:
	case cHouseNull:
	default:
		swiss_house = -1;		// no equivalent in Swiss Ephemeris, conversion not possible
		break;
	}

	return swiss_house;
}

/* Calculate the house cusp positions, using the specified algorithm. */

void ComputeHouses(int housesystem)
{
	char sz[cchSzDef];

	if (fabs(ciCore.lat) > Deg2Rad(90.0 - Rad2Deg(is.rObliquity)) && housesystem < cHouseEqual)
	{
		sprintf(sz, "The %s system of houses is not defined at extreme latitudes.", Lang(tHouseSystem, housesystem));
		PrintWarning(sz);
		ciCore.lat = RSgn2(ciCore.lat) * Deg2Rad(90.0 - Rad2Deg(is.rObliquity));
	}

	/* Flip the Ascendant if it falls in the wrong half of the zodiac. */
	if (MinDifference(is.MC, is.Asc) < 0.0 && housesystem == cHouseVedic)
	{
		is.Asc = Mod(is.Asc + 180.0);
	}

	switch (housesystem)
	{
	case cHousePlacidus:
		HousePlacidus();
		break;

	case cHouseKoch:
		HouseKoch();
		break;

	case cHouseEqual:
		HouseEqual();
		break;

	case cHouseCampanus:
		HouseCampanus();
		break;

	case cHouseMeridian:
		HouseMeridian();
		break;

	case cHouseRegiomontanus:
		HouseRegiomontanus();
		break;

	case cHousePorphyry:
		HousePorphyry();
		break;

	case cHouseMorinus:
		HouseMorinus();
		break;

	case cHouseTopocentric:
		HouseTopocentric();
		break;

	case cHouseAlcabitius:
		HouseAlcabitius();
		break;

	case cHouseEqualMidheaven:
		HouseEqualMidheaven();
		break;

	case cHouseNeoPorphyry:
		HouseNeoPorphyry();
		break;

	case cHouseWhole:
		HouseWhole();
		break;

	case cHouseVedic:
		HouseVedic();
		break;

	case cHouseNull:
		HouseNull();
		break;

	case cHouseShripati:
		HouseShripati();
		break;

	default:
		HousePlacidus();
		break;
	}
}

/* This is a subprocedure of ComputeInHouses(). Given a zodiac position,  */
/* return which of the twelve houses it falls in. Remember that a special */
/* check has to be done for the house that spans 0 degrees Aries.		  */

int HousePlaceIn(double rDeg) {
	int i = 0;

	rDeg = Mod(rDeg + 0.5 / 60.0 / 60.0);

	if ((hRevers < 2) || !PolarMCflip)
	{

		do
		{
			i++;
		}
		while (!(i >= NUMBER_OF_HOUSES ||
				 (rDeg >= cp0.cusp_pos[i] && rDeg < cp0.cusp_pos[Mod12(i + 1)]) ||
				 (cp0.cusp_pos[i] > cp0.cusp_pos[Mod12(i + 1)] && 
						(rDeg >= cp0.cusp_pos[i] || rDeg < cp0.cusp_pos[Mod12(i + 1)]))));
		return i;

	}
	else
	{
		do
		{
			i++;
		}
		while (!(i >= NUMBER_OF_HOUSES ||
				 (rDeg <= cp0.cusp_pos[i] && rDeg > cp0.cusp_pos[Mod12(i + 1)]) ||
				 (cp0.cusp_pos[i] < cp0.cusp_pos[Mod12(i + 1)] && 
						(rDeg <= cp0.cusp_pos[i] || rDeg > cp0.cusp_pos[Mod12(i + 1)]))));
		return i;
	}
}


/* For each object in the chart, determine what house it belongs in. */

void ComputeInHouses()
{
	int i;

	for (i = 0; i <= cObj; i++)
	{
		cp0.house_no[i] = HousePlaceIn(cp0.longitude[i]);
	}
}

/* houses.c */
