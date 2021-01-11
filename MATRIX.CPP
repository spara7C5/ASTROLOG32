/*
** Astrolog File: matrix.c
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
** Assorted Calculations.
******************************************************************************
*/

/* Given a month, day, and year, convert it into a single Julian day value, */
/* i.e. the number of days passed since a fixed reference date.             */

long MdyToJulian(int mon, int day, int yea){
	int fGreg = TRUE;

	if (yea < yeaJ2G || (yea == yeaJ2G && (mon < monJ2G || (mon == monJ2G && day < 15))))
		fGreg = FALSE;
	return (long) floor(swe_julday(yea, mon, day, 12.0, fGreg) + 0.5);
}


/* Like above but return a fractional Julian time given the extra info. */

double MdytszToJulian(int mon, int day, int yea, double tim, double dst, double zon) {
	return (double) MdyToJulian(mon, day, yea) + (DegMin2DecDeg(tim) + DegMin2DecDeg(zon) - DegMin2DecDeg(dst)) / 24.0;
}


/* Take a Julian day value, and convert it back into the corresponding */
/* month, day, and year.                                               */

void JulianToMdy(double JD, int *mon, int *day, int *yea) {
	double tim;

	swe_revjul(JD, JD >= 2299171.0 /* October 15, 1582 */, yea, mon, day, &tim);
}

/* This is a subprocedure of CastChart(). Once we have the chart parameters, */
/* calculate a few important things related to the date, i.e. the Greenwich  */
/* time, the Julian day and fractional part of the day, the offset to the    */
/* sidereal, and a couple of other things.                                   */

double ProcessInput(BOOL fDate){
	double Off; 
	double x[6], jd, eph_time, ayanamsa;
	char serr[AS_MAXCH];

	ciCore.tim = RSgn(ciCore.tim) * floor(fabs(ciCore.tim)) + 
		RFract(fabs(ciCore.tim)) * 100.0 / 60.0 + 
		DegMin2DecDeg(ciCore.zon) - DegMin2DecDeg(ciCore.dst);
	ciCore.lon = DegMin2DecDeg(ciCore.lon);
	ciCore.lat = Min(ciCore.lat, 89.9999);		/* Make sure the chart isn't being cast */
	ciCore.lat = Max(ciCore.lat, -89.9999);		/* on the precise north or south pole.  */
	ciCore.lat = Deg2Rad(DegMin2DecDeg(ciCore.lat));

	/* if parameter 'fDate' isn't set, then we can assume that the true time */
	/* has already been determined (as in a -rm switch time midpoint chart). */
 
	if (fDate)
	{
		is.JD = (double) MdyToJulian(ciCore.mon, ciCore.day, ciCore.yea);
		if (!us.fProgressUS || us.fSolarArc)
			is.T = (is.JD + ciCore.tim / 24.0 - 2415020.5) / 36525.0;
		else
		{
			/* Determine actual time that a progressed chart is to be cast for. */

			is.T = ((is.JD + ciCore.tim / 24.0 + (is.JDp - (is.JD + ciCore.tim / 24.0)) / us.rProgDay) - 2415020.5) / 36525.0;
		}
	}


	jd = JulianDayFromTime(is.T);
	eph_time = jd + swe_deltat(jd);

	ayanamsa = swe_get_ayanamsa(eph_time);

	if(ayanamsa > 180.0)
	{
		// swe_get_ayanamsa flips Ayanamsha from 0 to 360 degrees around year 200, 
		// Astrolog doesn't
		ayanamsa -= 360.0;
	}

	// Swiss Ephemeris returns a positive value for ayanamsa
	Off = -ayanamsa;

	// Compute angle that the ecliptic is inclined to the Celestial Equator
	swe_calc(eph_time, SE_ECL_NUT, 0, x, serr);
	
	if(us.fSidereal)
	{
		is.rObliquity = Deg2Rad(x[1]);	// mean obliquity (no nutation)
	}
	else
	{
		is.rObliquity = Deg2Rad(x[0]);	// true obliquity with nutation
	}

	is.rSid = us.fSidereal ? Off + us.rSiderealCorrection : 0.0;

	return Off;
}


/* Convert polar to rectangular coordinates. */

void PolToRec(double A, double R, double*X, double *Y){
	if (A == 0.0)
		A = rSmall;
	*X = R * cos(A);
	*Y = R * sin(A);
}


/* Convert rectangular to polar coordinates. */

void RecToPol(double X, double Y, double*A, double*R){
	if (Y == 0.0)
		Y = rSmall;
	*R = sqrt(X * X + Y * Y);
	*A = Angle(X, Y);
}


/* Convert rectangular to spherical coordinates. */

double RecToSph(double B, double L, double O){
	double R, Q, G, X, Y, A;

	A = B;
	R = 1.0;
	PolToRec(A, R, &X, &Y);
	Q = Y;
	R = X;
	A = L;
	PolToRec(A, R, &X, &Y);
	G = X;
	X = Y;
	Y = Q;
	RecToPol(X, Y, &A, &R);
	A += O;
	PolToRec(A, R, &X, &Y);
	Q = asin(Y);
	Y = X;
	X = G;
	RecToPol(X, Y, &A, &R);
	if (A < 0.0)
		A += 2 * rPi;
	G = A;
	return G;					/* We only ever care about and return one of the coordinates. */
}


/* Do a coordinate transformation: Given a longitude and latitude value,    */
/* return the new longitude and latitude values that the same location      */
/* would have, were the equator tilted by a specified number of degrees.    */
/* In other words, do a pole shift! This is used to convert among ecliptic, */
/* equatorial, and local coordinates, each of which have zero declination   */
/* in different planes. In other words, take into account the Earth's axis. */

void CoorXform(double *azi, double *alt, double tilt){
	double x, y, a1, l1;
	double sinalt, cosalt, sinazi, sintilt, costilt;

	sinalt = sin(*alt);
	cosalt = cos(*alt);
	sinazi = sin(*azi);
	sintilt = sin(tilt);
	costilt = cos(tilt);

	x = cosalt * sinazi * costilt;
	y = sinalt * sintilt;
	x -= y;
	a1 = cosalt;
	y = cosalt * cos(*azi);
	l1 = Angle(y, x);
	a1 = a1 * sinazi * sintilt + sinalt * costilt;
	a1 = asin(a1);
	*azi = l1;
	*alt = a1;
}


/* This is another subprocedure of CastChart(). Calculate a few variables */
/* corresponding to the chart parameters that are used later on. The      */
/* astrological vertex (object number nineteen) is also calculated here.  */

void ComputeVariables(double *vtx){
	double R, R2, B, L, O, G, X, Y, A;

	is.RA = Deg2Rad(Mod((6.6460656 + 2400.0513 * is.T + 2.58E-5 * is.T * is.T + ciCore.tim) * 15.0 - ciCore.lon));
	R2 = is.RA;
	O = -is.rObliquity;
	B = ciCore.lat;
	A = R2;
	R = 1.0;
	PolToRec(A, R, &X, &Y);
	X *= cos(O);
	RecToPol(X, Y, &A, &R);
	is.MC = Mod(is.rSid + Rad2Deg(A));	/* Midheaven */

	L = R2 + rPi;
	B = rPiHalf - fabs(B);
	if (ciCore.lat < 0.0)
		B = -B;
	G = RecToSph(B, L, O) + rPiHalf;

	// keep the vertex always on the western hemisphere in tropical latitudes 
	if(fabs(ciCore.lat) <= is.rObliquity && ModRad(A - G) > rPi)
	{
		G += rPi;
	}

	*vtx = Mod(is.rSid + Rad2Deg(G));	/* Vertex */
}



/*
******************************************************************************
** Planetary Position Calculations.
******************************************************************************
*/

/* Given three values, return them combined as the coefficients of a */
/* quadratic equation as a function of the chart time.               */

double ReadThree(double r0, double r1, double r2){
	return Deg2Rad(r0 + r1 * is.T + r2 * is.T * is.T);
}


/* Another coordinate transformation. This is used by the ComputePlanets() */
/* procedure to rotate rectangular coordinates by a certain amount.        */

void RecToSph2(double AP, double AN, double INu, double *X, double *Y, double *G){
	double R, D, A;

	RecToPol(*X, *Y, &A, &R);
	A += AP;
	PolToRec(A, R, X, Y);
	D = *X;
	*X = *Y;
	*Y = 0.0;
	RecToPol(*X, *Y, &A, &R);
	A += INu;
	PolToRec(A, R, X, Y);
	*G = *Y;
	*Y = *X;
	*X = D;
	RecToPol(*X, *Y, &A, &R);
	A += AN;
	if (A < 0.0)
		A += 2.0 * rPi;
	PolToRec(A, R, X, Y);
}


/* Calculate some harmonic delta error correction factors to add onto the */
/* coordinates of Jupiter through Pluto, for better accuracy.             */

void ErrorCorrect(int ind, double *x, double *y, double *z) {
	double U, V, W, A, S0, T0[4], * pr;
	int IK, IJ, irError;

	irError = rErrorCount[ind - oJup];
	pr = (double *) & rErrorData[rErrorOffset[ind - oJup]];
	for (IK = 1; IK <= 3; IK++)
	{
		if (ind == oJup && IK == 3)
		{
			T0[3] = 0.0;
			break;
		}
		if (IK == 3)
			irError--;
		S0 = ReadThree(pr[0], pr[1], pr[2]);
		pr += 3;
		A = 0.0;
		for (IJ = 1; IJ <= irError; IJ++)
		{
			U = *pr++;
			V = *pr++;
			W = *pr++;
			A += Deg2Rad(U) * cos((V * is.T + W) * rPi / 180.0);
		}
		T0[IK] = Rad2Deg(S0 + A);
	}
	*x += T0[2];
	*y += T0[1];
	*z += T0[3];
}


/* Another subprocedure of the ComputePlanets() routine. Convert the final */
/* rectangular coordinates of a planet to zodiac position and declination. */

void ProcessPlanet(int ind, double aber) {
	double ang, rad;

	RecToPol(spacex[ind], spacey[ind], &ang, &rad);
	cp0.longitude[ind] = Mod(Rad2Deg(ang) /*+ NU */  - aber + is.rSid);
	RecToPol(rad, spacez[ind], &ang, &rad);

	if (us.objCenter == oSun && ind == oSun)
	{
		ang = 0.0;
	}

	ang = Rad2Deg(ang);
	while (ang > 90.0)		/* Ensure declination is from -90..+90 degrees. */
	{
		ang -= 180.0;
	}

	while (ang < -90.0)
	{
		ang += 180.0;
	}

	cp0.latitude[ind] = ang;
}


/* Calculate the position of each body that orbits the Sun. A heliocentric chart is  */
/* most natural; extra calculation is needed to have other central bodies. */

void ComputePlanets()
{
	double helioret[cLastMoving + 1], heliox[cLastMoving + 1];
	double helioy[cLastMoving + 1], helioz[cLastMoving + 1];
	double aber = 0.0, AU, E, EA, E1, M, XW, YW, AP, AN, INu, X, Y, G, XS, YS, ZS;
	int ind = oSun, i;
	OE *poe;

	while (ind <= cLastMoving)
	{
		if (ignore[ind] && ind > oSun)
			goto LNextPlanet;
		poe = &rgoe[IoeFromObj(ind)];

		EA = M = ModRad(ReadThree(poe->ma0, poe->ma1, poe->ma2));
		E = Rad2Deg(ReadThree(poe->ec0, poe->ec1, poe->ec2));
		for (i = 1; i <= 5; i++)
			EA = M + E * sin(EA);	/* Solve Kepler's equation */
		AU = poe->sma;			/* Semi-major axis         */
		E1 = 0.01720209 / (pow(AU, 1.5) * (1.0 - E * cos(EA)));	/* Begin velocity coordinates */
		XW = -AU * E1 * sin(EA);	/* Perifocal coordinates      */
		YW = AU * E1 * pow(1.0 - E * E, 0.5) * cos(EA);
		AP = ReadThree(poe->ap0, poe->ap1, poe->ap2);
		AN = ReadThree(poe->an0, poe->an1, poe->an2);
		INu = ReadThree(poe->in0, poe->in1, poe->in2);	/* Calculate inclination  */
		X = XW;
		Y = YW;
		RecToSph2(AP, AN, INu, &X, &Y, &G);	/* Rotate velocity coords */
		heliox[ind] = X;
		helioy[ind] = Y;
		helioz[ind] = G;		/* Helio ecliptic rectangtular */
		X = AU * (cos(EA) - E);	/* Perifocal coordinates for        */
		Y = AU * sin(EA) * pow(1.0 - E * E, 0.5);	/* rectangular position coordinates */
		RecToSph2(AP, AN, INu, &X, &Y, &G);	/* Rotate for rectangular */
		XS = X;
		YS = Y;
		ZS = G;					/* position coordinates   */
		if (FBetween(ind, oJup, oPlu))
			ErrorCorrect(ind, &XS, &YS, &ZS);
		cp0.vel_longitude[ind] =				/* Helio daily motion */
			(XS * helioy[ind] - YS * heliox[ind]) / (XS * XS + YS * YS);
		spacex[ind] = XS;
		spacey[ind] = YS;
		spacez[ind] = ZS;
		ProcessPlanet(ind, 0.0);
	  LNextPlanet:
		ind += (ind == oSun ? 2 : (ind != cLastPlanet ? 1 : uranLo - cLastPlanet));
	}

	spacex[oEar] = spacex[oSun];
	spacey[oEar] = spacey[oSun];
	spacez[oEar] = spacez[oSun];
	cp0.longitude[oEar] = cp0.longitude[oSun];
	cp0.latitude[oEar] = cp0.latitude[oSun];
	cp0.vel_longitude[oEar] = cp0.vel_longitude[oSun];
	heliox[oEar] = heliox[oSun];
	helioy[oEar] = helioy[oSun];
	// helioret[oEar] = helioret[oSun]; warning C4700: local variable 'helioret' 
	//                                  used without having been initialized
	spacex[oSun] = spacey[oSun] = spacez[oSun] = cp0.longitude[oSun] = cp0.latitude[oSun] = heliox[oSun] = helioy[oSun] = 0.0;
	if (us.objCenter == oSun)
	{
		if (us.fVelocity)
			for (i = 0; i <= cLastMoving; i++)	/* Use relative velocity */
				cp0.vel_longitude[i] = Deg2Rad(1.0);	/* if -v0 is in effect.  */
		return;
	}

	/* A second loop is needed for geocentric charts or central bodies other */
	/* than the Sun. For example, we can't find the position of Mercury in   */
	/* relation to Pluto until we know the position of Pluto in relation to  */
	/* the Sun, and since Mercury is calculated first, another pass needed.  */

	ind = us.objCenter;
	for (i = 0; i <= cLastMoving; i++)
	{
		helioret[i] = cp0.vel_longitude[i];
		if (i != oMoo && i != ind)
		{
			spacex[i] -= spacex[ind];
			spacey[i] -= spacey[ind];
			spacez[i] -= spacez[ind];
		}
	}
	for (i = oEar; i <= cLastMoving; i += (i == oSun ? 2 : (i != cLastPlanet ? 1 : uranLo - cLastPlanet)))
	{
		if ((ignore[i] && i > oSun) || i == ind)
			continue;
		XS = spacex[i];
		YS = spacey[i];
		ZS = spacez[i];
		cp0.vel_longitude[i] = (XS * (helioy[i] - helioy[ind]) - YS * (heliox[i] - heliox[ind])) / (XS * XS + YS * YS);
		if (ind == oEar)
			aber = 0.0057756 * sqrt(XS * XS + YS * YS + ZS * ZS) * Rad2Deg(cp0.vel_longitude[i]);	/* Aberration */
		ProcessPlanet(i, aber);
		if (us.fVelocity)		/* Use relative velocity */
			cp0.vel_longitude[i] = Deg2Rad(cp0.vel_longitude[i] / helioret[i]);	/* if -v0 is in effect   */
	}
	spacex[ind] = spacey[ind] = spacez[ind] = 0.0;
}


/*
******************************************************************************
** Lunar Position Calculations
******************************************************************************
*/

/* Calculate the position and declination of the Moon, and the Moon's North  */
/* Node. This has to be done separately from the other planets, because they */
/* all orbit the Sun, while the Moon orbits the Earth.                       */

void ComputeLunar(double *moonlo, double *moonla, double *nodelo, double *nodela)
{
	double LL, G, N, G1, D, L, ML, L1, MB, T1, Y, M = 3600.0, T2;

	T2 = is.T * is.T;
	LL = 973563.0 + 1732564379.0 * is.T - 4.0 * T2;	/* Compute mean lunar longitude    */
	G = 1012395.0 + 6189.0 * is.T;					/* Sun's mean longitude of perigee */
	N = 933060.0 - 6962911.0 * is.T + 7.5 * T2;		/* Compute mean lunar node         */
	G1 = 1203586.0 + 14648523.0 * is.T - 37.0 * T2;	/* Mean longitude of lunar perigee */
	D = 1262655.0 + 1602961611.0 * is.T - 5.0 * T2;	/* Mean elongation of Moo from Sun */
	L = (LL - G1) / M;
	L1 = ((LL - D) - G) / M;						/* Some auxiliary angles           */
	T1 = (LL - N) / M;
	D = D / M;
	Y = 2.0 * D;

	/* Compute Moon's perturbations. */

	ML = 22639.6 * RSinD(L) - 4586.4 * RSinD(L - Y) + 2369.9 * RSinD(Y) +
		769.0 * RSinD(2.0 * L) - 669.0 * RSinD(L1) -
		411.6 * RSinD(2.0 * T1) - 212.0 * RSinD(2.0 * L - Y) - 206.0 * RSinD(L + L1 - Y);
	ML +=
		192.0 * RSinD(L + Y) - 165.0 * RSinD(L1 - Y) + 148.0 * RSinD(L - L1) -
		125.0 * RSinD(D) - 110.0 * RSinD(L + L1) - 55.0 * RSinD(2.0 * T1 - Y) -
		45.0 * RSinD(L + 2.0 * T1) + 40.0 * RSinD(L - 2.0 * T1);

	*moonlo = G = Mod((LL + ML) / M + is.rSid);	/* Lunar longitude */

	/* Compute lunar latitude. */

	MB = 18461.5 * RSinD(T1) + 1010.0 * RSinD(L + T1) - 999.0 * RSinD(T1 - L) -
		624.0 * RSinD(T1 - Y) + 199.0 * RSinD(T1 + Y - L) - 167.0 * RSinD(L + T1 - Y);
	MB +=
		117.0 * RSinD(T1 + Y) + 62.0 * RSinD(2.0 * L + T1) -
		33.0 * RSinD(T1 - Y - L) - 32.0 * RSinD(T1 - 2.0 * L) - 30.0 * RSinD(L1 + T1 - Y);
	*moonla = MB = RSgn(MB) * ((fabs(MB) / M) / 360.0 - floor((fabs(MB) / M) / 360.0)) * 360.0;

	/* Compute position of the North Lunar Node, either True or Mean. */

	if (us.fTrueNode)
	{
		N = N + 5392.0 * RSinD(2.0 * T1 - Y) - 541.0 * RSinD(L1) -
			442.0 * RSinD(Y) + 423.0 * RSinD(2.0 * T1) - 291.0 * RSinD(2.0 * L - 2.0 * T1);
	}
	*nodelo = Mod(N / M + is.rSid);
	*nodela = 0.0;
}

/* matrix.c */
