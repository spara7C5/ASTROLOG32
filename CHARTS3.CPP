/*
** Astrolog File: charts3.c
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
**
*/

#include "astrolog.h"

double lonz1[NUMBER_OBJECTS], lonz2[NUMBER_OBJECTS], latz1[NUMBER_OBJECTS], latz2[NUMBER_OBJECTS];

/*
******************************************************************************
** Multiple Chart Scanning Routines.
******************************************************************************
*/

/* Search through a day, and print out the times of exact aspects among the  */
/* planets during that day, as specified with the -d switch, as well as the  */
/* times when a planet changes sign or direction. To do this, we cast charts */
/* for the beginning and end of the day, or a part of a day, and do a linear */
/* equation check to see if anything exciting happens during the interval.   */
/* (This is probably the single most complicated procedure in the program.)  */

void ChartInDaySearch(BOOL fProg) {
	char sz[cchSzDef];
	int source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY],
		sign1[MAXINDAY], sign2[MAXINDAY], D1, D2, counttotal = 0, occurcount,
		division, div, fYear, yea0, yea1, yea2, i, j, k, l, s1, s2;
	double time[MAXINDAY], divsiz, d1, d2, e1, e2, f1, f2, g, time1, time2;
	CI ciT;

	/* If parameter 'fProg' is set, look for changes in a progressed chart. */

	ciT = ciTran;
	fYear = us.fInDayMonth && (ciTran.mon == 0);
	division = (fYear || fProg) ? 1 : us.nDivision;
	divsiz = 24.0 / (double) division *60.0;

	/* If -dY in effect, then search through a range of years. */

	yea1 = fProg ? ciTran.yea : ciMain.yea;
	yea2 = fYear ? (yea1 + us.nEphemYears - 1) : yea1;
	for (yea0 = yea1; yea0 <= yea2; yea0++)
	{

		/* If -dm in effect, then search through the whole month, day by day. */

		if (us.fInDayMonth)
		{
			D1 = 1;
			if (fYear)
			{
				ciTran.mon = 1;
				D2 = DayInYearHi(yea0);
			}
			else
				D2 = DayInMonth(fProg ? ciTran.mon : ciMain.mon, yea0);
		}
		else
			D1 = D2 = ciMain.day;

		/* Start searching the day or days in question for exciting stuff. */

		for (ciTran.day = D1; ciTran.day <= D2; ciTran.day = AddDay(ciMain.mon, ciTran.day, yea0, 1))
		{
			occurcount = 0;

			/* Cast chart for beginning of day and store it for future use. */

			SetCI(&ciCore, fYear ? ciTran.mon : ciMain.mon, ciTran.day, yea0, 0.0, 
				ciMain.dst, ciMain.zon, ciMain.lon, ciMain.lat, ciMain.loc);

			if (us.fProgressUS = fProg)
			{
				is.JDp = MdytszToJulian(ciTran.mon, ciCore.day, yea0, 0.0, ciMain.dst, ciMain.zon);
				ciCore = ciMain;
			}
			CastChart(TRUE);
			cp2 = cp0;

			/* Now divide the day into segments and search each segment in turn. */
			/* More segments is slower, but has slightly better time accuracy.   */

			for (div = 1; div <= division; div++)
			{

				/* Cast the chart for the ending time of the present segment. The   */
				/* beginning time chart is copied from the previous end time chart. */

				SetCI(&ciCore, fYear ? ciTran.mon : ciMain.mon, ciTran.day, yea0,
					  DecDeg2DegMin(24.0 * (double) div / (double) division), ciMain.dst, 
					  ciMain.zon, ciMain.lon, ciMain.lat, ciMain.loc);
				if (fProg)
				{
					is.JDp = MdytszToJulian(ciTran.mon, ciCore.day + 1, yea0, 0.0, ciMain.dst, ciMain.zon);
					ciCore = ciMain;
				}
				CastChart(TRUE);
				cp1 = cp2;
				cp2 = cp0;

				if (us.fParallel)
				{
					for (i = 0; i <= cObj; i++)
						if (!ignore[i])
						{
							if (us.fEquator)
							{
								lonz1[i] = cp1.longitude[i];
								latz1[i] = cp1.latitude[i];
								lonz2[i] = cp2.longitude[i];
								latz2[i] = cp2.latitude[i];
							}
							else
							{
								lonz1[i] = Deg2Rad(Tropical(cp1.longitude[i]));
								latz1[i] = Deg2Rad(cp1.latitude[i]);
								EclToEqu(&lonz1[i], &latz1[i]);
								latz1[i] = Rad2Deg(latz1[i]);
								lonz2[i] = Deg2Rad(Tropical(cp2.longitude[i]));
								latz2[i] = Deg2Rad(cp2.latitude[i]);
								EclToEqu(&lonz2[i], &latz2[i]);
								latz2[i] = Rad2Deg(latz2[i]);
							}
						}
				}

				/* Now search through the present segment for anything exciting. */

				for (i = 0; i <= cObj; i++)
					if (!FIgnore(i) && (fProg || FThing(i)))
					{
						s1 = Z2Sign(cp1.longitude[i]) - 1;
						s2 = Z2Sign(cp2.longitude[i]) - 1;

						if (!us.fParallel)
						{

							/* Does the current planet change into the next or previous sign? */

							if (s1 != s2 && !us.fIgnoreSign)
							{
								source[occurcount] = i;
								aspect[occurcount] = aSig;
								dest[occurcount] = s2 + 1;
								time[occurcount] = MinDistance(cp1.longitude[i], 
									(double) (cp1.vel_longitude[i] >= 0.0 ? s2 : s1) * 30.0) /
									MinDistance(cp1.longitude[i], cp2.longitude[i]) * divsiz + (double) (div - 1) * divsiz;
								sign1[occurcount] = sign2[occurcount] = s1 + 1;
								occurcount++;
							}

							/* Does the current planet go retrograde or direct? */

							if ((cp1.vel_longitude[i] < 0.0) != (cp2.vel_longitude[i] < 0.0) && !us.fIgnoreDir)
							{
								source[occurcount] = i;
								aspect[occurcount] = aDir;
								dest[occurcount] = cp2.vel_longitude[i] < 0.0;
								time[occurcount] =
									fabs(cp1.vel_longitude[i]) / (fabs(cp1.vel_longitude[i]) +
														fabs(cp2.vel_longitude[i])) * divsiz + (double) (div - 1) * divsiz;
								sign1[occurcount] = sign2[occurcount] = s1 + 1;
								occurcount++;
							}

							/* Now search for anything making an aspect to the current planet. */

							for (j = i + 1; j <= cObj; j++)
								if (!FIgnore(j) && (fProg || FThing(j)))
									for (k = 1; k <= cAspect; k++)
										if (FAcceptAspect(i, k, j))
										{
											d1 = cp1.longitude[i];
											d2 = cp2.longitude[i];
											e1 = cp1.longitude[j];
											e2 = cp2.longitude[j];
											if (MinDistance(d1, d2) < MinDistance(e1, e2))
											{
												SwapR(d1, e1);
												SwapR(d2, e2);
											}

											/* We are searching each aspect in turn. Let's subtract the  */
											/* size of the aspect from the angular difference, so we can */
											/* then treat it like a conjunction.                         */

											if (MinDistance
												(e1, Mod(d1 - rAspAngle[k])) < MinDistance(e2, Mod(d2 + rAspAngle[k])))
											{
												e1 = Mod(e1 + rAspAngle[k]);
												e2 = Mod(e2 + rAspAngle[k]);
											}
											else
											{
												e1 = Mod(e1 - rAspAngle[k]);
												e2 = Mod(e2 - rAspAngle[k]);
											}

											/* Check to see if the aspect actually occurs during our    */
											/* segment, making sure we take into account if one or both */
											/* planets are retrograde or if they cross the Aries point. */

											f1 = e1 - d1;
											if (fabs(f1) > 180.0)
												f1 -= RSgn(f1) * 360.0;
											f2 = e2 - d2;
											if (fabs(f2) > 180.0)
												f2 -= RSgn(f2) * 360.0;
											if (MinDistance
												(Midpoint(d1, d2), Midpoint(e1, e2)) < 90.0 && RSgn(f1) != RSgn(f2))
											{
												source[occurcount] = i;
												aspect[occurcount] = k;
												dest[occurcount] = j;

												/* Horray! The aspect occurs sometime during the interval.   */
												/* Now we just have to solve an equation in two variables to */
												/* find out where the "lines" cross, i.e. the aspect's time. */

												f1 = d2 - d1;
												if (fabs(f1) > 180.0)
													f1 -= RSgn(f1) * 360.0;
												f2 = e2 - e1;
												if (fabs(f2) > 180.0)
													f2 -= RSgn(f2) * 360.0;
												g = (fabs(d1 - e1) >
													 180.0 ? (d1 - e1) -
													 RSgn(d1 - e1) * 360.0 : d1 - e1) / (f2 - f1);
												time[occurcount] = g * divsiz + (double) (div - 1) * divsiz;
												sign1[occurcount] =
													(int) (Mod
														   (cp1.longitude[i] +
															RSgn(cp2.longitude[i] -
																 cp1.longitude[i]) *
															(fabs
															 (cp2.longitude[i] -
															  cp1.longitude[i]) >
															 180.0 ? -1 :
															 1) * fabs(g) *
															MinDistance(cp1.longitude[i], cp2.longitude[i])) / 30.0) + 1;
												sign2[occurcount] =
													(int) (Mod
														   (cp1.longitude[j] +
															RSgn(cp2.longitude[j] -
																 cp1.longitude[j]) *
															(fabs
															 (cp2.longitude[j] -
															  cp1.longitude[j]) >
															 180.0 ? -1 :
															 1) * fabs(g) *
															MinDistance(cp1.longitude[j], cp2.longitude[j])) / 30.0) + 1;
												occurcount++;
											}
										}

						}
						else
						{

							/* Now search for anything making an parallel to the current planet. */

							for (j = i + 1; j <= cObj; j++)
								if (!FIgnore(j) && (fProg || FThing(j)))
								{
									if (FCusp(i) && FCusp(j))
										continue;
									k = 0;
									d1 = latz1[i];
									d2 = latz2[i];
									e1 = latz1[j];
									e2 = latz2[j];
									if (fabs(d2 - d1) < fabs(e2 - e1))
									{
										SwapR(d1, e1);
										SwapR(d2, e2);
									}
									if (((d2 > d1)
										 && (FBetween(e1, d1, d2)
											 || FBetween(e2, d1, d2)))
										|| ((d2 < d1) && (FBetween(e1, d2, d1) || FBetween(e2, d2, d1))))
									{
										if ((d2 - d1) != (e2 - e1))
											k = 1;
									}
									if (k == 0)
									{
										e1 = -e1;
										e2 = -e2;
										if (((d2 > d1)
											 && (FBetween(e1, d1, d2)
												 || FBetween(e2, d1, d2)))
											|| ((d2 < d1) && (FBetween(e1, d2, d1) || FBetween(e2, d2, d1))))
										{
											if ((d2 - d1) != (e2 - e1))
												k = 2;
										}
									}
									if (k)
									{
										f1 = d2 - d1;
										f2 = e2 - e1;
										g = (e1 - d1) / (f1 - f2);
										time[occurcount] = g * divsiz + (double) (div - 1) * divsiz;
										time1 = divsiz * (double) (div - 1);
										time2 = divsiz * (double) div;
										if (time[occurcount] >= time1 && time[occurcount] <= time2)
										{
											source[occurcount] = i;
											aspect[occurcount] = k;
											dest[occurcount] = j;
											sign1[occurcount] =
												(int) (Mod
													   (cp1.longitude[i] +
														RSgn(cp2.longitude[i] -
															 cp1.longitude[i]) *
														(fabs
														 (cp2.longitude[i] -
														  cp1.longitude[i]) >
														 180.0 ? -1 : 1) *
														fabs(g) * MinDistance(cp1.longitude[i], cp2.longitude[i])) / 30.0) + 1;
											sign2[occurcount] =
												(int) (Mod
													   (cp1.longitude[j] +
														RSgn(cp2.longitude[j] -
															 cp1.longitude[j]) *
														(fabs
														 (cp2.longitude[j] -
														  cp1.longitude[j]) >
														 180.0 ? -1 : 1) *
														fabs(g) * MinDistance(cp1.longitude[j], cp2.longitude[j])) / 30.0) + 1;
											occurcount++;
										}
									}
								}
						}
					}
			}

			/* After all the aspects, etc, in the day have been located, sort   */
			/* them by time at which they occur, so we can print them in order. */

			for (i = 1; i < occurcount; i++)
			{
				j = i - 1;
				while (j >= 0 && time[j] > time[j + 1])
				{
					SwapN(source[j], source[j + 1]);
					SwapN(aspect[j], aspect[j + 1]);
					SwapN(dest[j], dest[j + 1]);
					SwapR(time[j], time[j + 1]);
					SwapN(sign1[j], sign1[j + 1]);
					SwapN(sign2[j], sign2[j + 1]);
					j--;
				}
			}

			/* Finally, loop through and display each aspect and when it occurs. */

			for (i = 0; i < occurcount; i++)
			{
				s1 = (int) time[i] / 60;
				s2 = (int) time[i] - s1 * 60;
				j = ciTran.day;

				if (fYear || fProg)
				{
					l = ciTran.mon;
					while (j > (k = DayInMonth(l, yea0)))
					{
						j -= k;
						l++;
					}
				}

				SetCI(&ciSave, fYear || fProg ? l : ciMain.mon, j, yea0, 
					DecDeg2DegMin(time[i] / 60.0), ciMain.dst, ciMain.zon, ciMain.lon, ciMain.lat, ciMain.loc);
				k = DayOfWeek(fYear || fProg ? l : ciMain.mon, j, yea0);
				AnsiColor(kRainbowA[k + 1]);
				sprintf(sz, "(%c%c%c) ", chDay3(k));
				PrintSz(sz);
				AnsiColor(kDefault);
				sprintf(sz, "%s %s ", 
					SzDate(fYear || fProg ? l : ciMain.mon, j, yea0, 2 * MonthFormat), 
					SzTime(s1, s2, -1));
				PrintSz(sz);
				PrintAspect(source[i], sign1[i], 
					(int) RSgn(cp1.vel_longitude[source[i]]) + (int) RSgn(cp2.vel_longitude[source[i]]), 
					aspect[i], dest[i], sign2[i],
					(int) RSgn(cp1.vel_longitude[dest[i]]) + (int) RSgn(cp2.vel_longitude[dest[i]]), 
					(byte) (fProg ? 'e' : 'd'), 7, 10);
				PrintInDay(source[i], aspect[i], dest[i]);
			}
			counttotal += occurcount;
		}
	}
	if (counttotal == 0)
		PrintSz("No transit events found.\n");

	/* Recompute original chart placements as we've overwritten them. */

	ciCore = ciMain;
	ciTran = ciT;
	CastChart(TRUE);
}


/* Search through a month, year, or years, and print out the times of exact */
/* transits where planets in the time frame make aspect to the planets in   */
/* some other chart, as specified with the -t switch. To do this, we cast   */
/* charts for the start and end of each month, or within a month, and do an */
/* equation check for aspects to the other base chart during the interval.  */

void ChartTransitSearch(BOOL fProg)
{
	double house3[NUMBER_OF_SIGNS + 1], ret3[NUMBER_OBJECTS], time[MAXINDAY];
	double planet3[NUMBER_OBJECTS], lonn[NUMBER_OBJECTS], latn[NUMBER_OBJECTS];
	double divsiz, daysiz, d, e1, e2, f1, f2;
	int source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY], sign[MAXINDAY];
	int isret[MAXINDAY], M1, M2, Y1, Y2, counttotal = 0, occurcount, division;
	int div, nAsp, fCusp, i, j, k, s1, s2, s3;
	char sz[cchSzDef];
	CI ciT;

	/* Save away natal chart and initialize things. */

	if (ciCore.mon == -1)
	{
		cp3 = cp0;
		fCP3 = 1;
	}

	ciT = ciTran;
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		house3[i] = cp0.cusp_pos[i];
	for (i = 0; i <= cObj; i++)
	{
		planet3[i] = cp0.longitude[i];
		ret3[i] = cp0.vel_longitude[i];
	}
	if (fProg)
		fCusp = FALSE;
	else
	{
		fCusp = TRUE;
		for (i = cuspLo; i <= cuspHi; i++)
			fCusp &= ignore2[i];
	}
	division = us.nDivision;
	if (!fProg && !fCusp)
		division = Max(division, 96);
	nAsp = is.fReturn ? aCon : cAspect;

	if (us.fParallel)
	{
		for (i = 0; i <= cObj; i++)
			if (!ignore[i])
			{
				if (us.fEquator)
				{
					lonn[i] = cp0.longitude[i];
					latn[i] = cp0.latitude[i];
				}
				else
				{
					lonn[i] = Deg2Rad(Tropical(cp0.longitude[i]));
					latn[i] = Deg2Rad(cp0.latitude[i]);
					EclToEqu(&lonn[i], &latn[i]);
					latn[i] = Rad2Deg(latn[i]);
				}
			}
	}

	/* Hacks: Searching month number zero means to search the whole year    */
	/* instead, month by month. Searching a negative month means to search  */
	/* multiple years, with the span of the year stored in the "day" field. */

	Y1 = Y2 = ciTran.yea;
	M1 = M2 = ciTran.mon;
	if (ciTran.mon < 1)
	{
		M1 = 1;
		M2 = 12;
		if (ciTran.mon < 0)
		{
			if (ciTran.day < 1)
			{
				Y1 = ciTran.yea + ciTran.day + 1;
				Y2 = ciTran.yea;
			}
			else
			{
				Y1 = ciTran.yea;
				Y2 = ciTran.yea + ciTran.day - 1;
			}
		}
	}

	/* Start searching the year or years in question for any transits. */

	for (ciTran.yea = Y1; ciTran.yea <= Y2; ciTran.yea++)

		/* Start searching the month or months in question for any transits. */

		for (ciTran.mon = M1; ciTran.mon <= M2; ciTran.mon++)
		{
			daysiz = (double) DayInMonth(ciTran.mon, ciTran.yea) * 24.0 * 60.0;
			divsiz = daysiz / (double) division;

			/* Cast chart for beginning of month and store it for future use. */

			SetCI(&ciCore, ciTran.mon, 1, ciTran.yea, 0.0, ciTran.dst, 
				ciTran.zon, ciTran.lon, ciTran.lat, ciTran.loc);
			if (us.fProgressUS = fProg)
			{
				is.JDp = MdytszToJulian(ciCore.mon, ciCore.day, ciCore.yea, 0.0, ciTran.dst, ciTran.zon);
				ciCore = ciMain;
			}
			for (i = 0; i <= cLastMoving; i++)
				SwapN(ignore[i], ignore2[i]);
			CastChart(TRUE);
			for (i = 0; i <= cLastMoving; i++)
				SwapN(ignore[i], ignore2[i]);
			cp2 = cp0;

			/* Divide our month into segments and then search each segment in turn. */

			for (div = 1; div <= division; div++)
			{
				occurcount = 0;

				/* Cast the chart for the ending time of the present segment, and */
				/* copy the start time chart from the previous end time chart.    */

				d = 1.0 + (daysiz / 24.0 / 60.0) * (double) div / (double) division;
				SetCI(&ciCore, ciTran.mon, (int) d, ciTran.yea, DecDeg2DegMin(RFract(d) * 24.0), 
					ciTran.dst, ciTran.zon, ciTran.lon, ciTran.lat, ciTran.loc);
				if (fProg)
				{
					is.JDp = MdytszToJulian(ciCore.mon, ciCore.day, 
						ciCore.yea, 0.0, ciTran.dst, ciTran.zon);
					ciCore = ciMain;
				}
				for (i = 0; i <= cLastMoving; i++)
					SwapN(ignore[i], ignore2[i]);
				CastChart(TRUE);
				for (i = 0; i <= cLastMoving; i++)
					SwapN(ignore[i], ignore2[i]);
				cp1 = cp2;
				cp2 = cp0;

				if (us.fParallel)
				{
					for (i = 0; i <= cObj; i++)
						if (!ignore[i])
						{
							if (us.fEquator)
							{
								lonz1[i] = cp1.longitude[i];
								latz1[i] = cp1.latitude[i];
								lonz2[i] = cp2.longitude[i];
								latz2[i] = cp2.latitude[i];
							}
							else
							{
								lonz1[i] = Deg2Rad(Tropical(cp1.longitude[i]));
								latz1[i] = Deg2Rad(cp1.latitude[i]);
								EclToEqu(&lonz1[i], &latz1[i]);
								latz1[i] = Rad2Deg(latz1[i]);
								lonz2[i] = Deg2Rad(Tropical(cp2.longitude[i]));
								latz2[i] = Deg2Rad(cp2.latitude[i]);
								EclToEqu(&lonz2[i], &latz2[i]);
								latz2[i] = Rad2Deg(latz2[i]);
							}
						}
				}

				/* Now search through the present segment for any transits. Note that */
				/* stars can be transited, but they can't make transits themselves.   */

				for (i = 0; i <= cObj; i++)
					if (!FIgnore(i))
					{
						for (j = 0; j <= cLastMoving; j++)
						{
							if ((is.fReturn ? i != j : FIgnore2(j)) || (fCusp && !FThing(j)))
								continue;

							/* Between each pair of planets, check if they make any aspects. */

							if (!us.fParallel)
							{

								for (k = 1; k <= nAsp; k++)
									if (FAcceptAspect(i, k, j))
									{
										d = planet3[i];
										e1 = cp1.longitude[j];
										e2 = cp2.longitude[j];
										if (MinDistance
											(e1, Mod(d - rAspAngle[k])) < MinDistance(e2, Mod(d + rAspAngle[k])))
										{
											e1 = Mod(e1 + rAspAngle[k]);
											e2 = Mod(e2 + rAspAngle[k]);
										}
										else
										{
											e1 = Mod(e1 - rAspAngle[k]);
											e2 = Mod(e2 - rAspAngle[k]);
										}

										/* Check to see if the present aspect actually occurs during the */
										/* segment, making sure we check any Aries point crossings.      */

										f1 = e1 - d;
										if (fabs(f1) > 180.0)
											f1 -= RSgn(f1) * 360.0;
										f2 = e2 - d;
										if (fabs(f2) > 180.0)
											f2 -= RSgn(f2) * 360.0;
										if (MinDistance(d, Midpoint(e1, e2)) <
											90.0 && RSgn(f1) != RSgn(f2) && occurcount < MAXINDAY)
										{

											/* Ok, we have found a transit. Now determine the time */
											/* and save this transit in our list to be printed.    */

											source[occurcount] = j;
											aspect[occurcount] = k;
											dest[occurcount] = i;
											time[occurcount] =
												fabs(f1) / (fabs(f1) + fabs(f2)) * divsiz + (double) (div - 1) * divsiz;
											sign[occurcount] =
												(int) (Mod (MinDistance (cp1.longitude[j],
														 Mod(d - rAspAngle[k])) < MinDistance(cp2.longitude[j],
																	Mod(d + rAspAngle [k]))
														? d - rAspAngle[k] : d + rAspAngle[k]) / 30.0) + 1;
											isret[occurcount] = (int) RSgn(cp1.vel_longitude[j]) + (int) RSgn(cp2.vel_longitude[j]);
											occurcount++;
										}
									}
							}
							else
							{
								k = 0;
								d = latn[i];
								e1 = latz1[j];
								e2 = latz2[j];
								if (((e2 > e1) && FBetween(d, e1, e2)) || ((e2 < e1) && FBetween(d, e2, e1)))
									k = 1;	/*  Found parallel.  */
								if (!k)
								{
									d = -d;
									if (((e2 > e1) && FBetween(d, e1, e2)) || ((e2 < e1) && FBetween(d, e2, e1)))
										k = 2;	/*  Found contra-parallel.  */
								}
								if (k)
								{
									source[occurcount] = j;
									aspect[occurcount] = k;
									dest[occurcount] = i;
									f1 = fabs(d - e1) / fabs(e2 - e1);
									time[occurcount] = divsiz * f1 + (double) (div - 1) * divsiz;
									sign[occurcount] = (int) (Mod (cp1.longitude[j] + RSgn(cp2.longitude[j] -
										cp1.longitude[j]) * (fabs(cp2.longitude[j] - cp1.longitude[j]) >
										180.0 ? -1 : 1) *
										fabs(f1) * MinDistance(cp1.longitude[j], cp2.longitude[j])) / 30.0) + 1;
									isret[occurcount] = 1;
									occurcount++;
								}
							}
						}
					}

				/* After all transits located, sort them by time at which they occur. */

				for (i = 1; i < occurcount; i++)
				{
					j = i - 1;
					while (j >= 0 && time[j] > time[j + 1])
					{
						SwapN(source[j], source[j + 1]);
						SwapN(aspect[j], aspect[j + 1]);
						SwapN(dest[j], dest[j + 1]);
						SwapR(time[j], time[j + 1]);
						SwapN(sign[j], sign[j + 1]);
						SwapN(isret[j], isret[j + 1]);
						j--;
					}
				}

				/* Now loop through list and display all the transits. */

				for (i = 0; i < occurcount; i++)
				{
					s1 = (uint) time[i] / 24 / 60;
					s3 = (uint) time[i] - s1 * 24 * 60;
					s2 = s3 / 60;
					s3 = s3 - s2 * 60;
					SetCI(&ciSave, ciTran.mon, s1 + 1, ciTran.yea, 
						DecDeg2DegMin((double) ((uint) time[i] - s1 * 24 * 60) / 60.0), 
						ciTran.dst, ciTran.zon, ciTran.lon, ciTran.lat, ciTran.loc);
					sprintf(sz, "%s %s ", 
						SzDate(ciTran.mon, s1 + 1, ciTran.yea, 2 * MonthFormat), SzTime(s2, s3, -1));
					PrintSz(sz);
					PrintAspect(source[i], sign[i], isret[i], aspect[i],
								dest[i], Z2Sign(planet3[dest[i]]),
								(int) RSgn(ret3[dest[i]]), (byte) (fProg ? 'u' : 't'), 7, 10);

					/* Check for a Solar, Lunar, or any other return. */

					if (aspect[i] == aCon && source[i] == dest[i])
					{
						AnsiColor(kWhite);
						sprintf(sz, " (%s Return)",
								source[i] == oSun ? "Solar" : (source[i] == oMoo ? "Lunar" : 
						Lang(tObjName, source[i])));
						PrintSz(sz);
					}
					PrintL();
#ifdef INTERPRET
					if (us.fInterpret)
						InterpretTransit(source[i], aspect[i], dest[i]);
#endif
					AnsiColor(kDefault);
				}
				counttotal += occurcount;
			}
		}
	if (counttotal == 0)
		PrintSz("No transits found.\n");

	/* Recompute original chart placements as we've overwritten them. */

	if (fCP3)
	{
		cp0 = cp3;
		fCP3 = 0;
	}

	ciCore = ciMain;
	ciTran = ciT;
	us.fProgressUS = FALSE;
	CastChart(TRUE);
}


/* Display a list of planetary rising times relative to the local horizon */
/* for the day indicated in the chart information, as specified with the  */
/* -Zd switch. For the day, the time each planet rises (transits horizon  */
/* in East half of sky), sets (transits horizon in West), reaches its     */
/* zenith point (transits meridian in South half of sky), and nadirs      */
/* transits meridian in North), is displayed.                             */

/* In this context more correct termin instead "zenith" and "nadir" is    */
/* astronomical termin "culmination" which means transit over meridian.   */
/* There are two culminations: "upper culmination" when planet has        */
/* highest position and "lower culmination" when planet has lowest        */
/* position. In chart below termins "zeniths" and "nadirs" are replaced   */
/* by "culm.(up)" and "culm.(lo)" respectively.  V.A.                    */

void ChartInDayHorizon()
{
	char sz[cchSzDef];
	int source[MAXINDAY], type[MAXINDAY], sign[MAXINDAY], fRet[MAXINDAY], occurcount, division, div, i, j, fT;
	double time[MAXINDAY], rgalt1[NUMBER_OBJECTS], rgalt2[NUMBER_OBJECTS], azialt[MAXINDAY],
		azi1, azi2, alt1, alt2, lon, lat, mc1, mc2, xA, yA, xV, yV, d, k, se, azia;
	CI ciT;
	byte EquT, MCpolarT;

	fT = us.fSidereal;
	us.fSidereal = FALSE;
	EquT = us.fEquator;
	us.fEquator = FALSE;
	MCpolarT = PolarMCflip;
	PolarMCflip = FALSE;
	lon = Deg2Rad(Mod(DegMin2DecDeg(ciMain.lon)));
	lat = Deg2Rad(DegMin2DecDeg(ciMain.lat));
	division = us.nDivision * 4;
	occurcount = 0;

	ciT = ciTwin;
	ciCore = ciMain;
	ciCore.tim = 0.0;
	CastChart(TRUE);
	mc2 = Deg2Rad(cp0.longitude[oMC]);
	k = Deg2Rad(cp0.latitude[oMC]);
	EclToEqu(&mc2, &k);
	cp2 = cp0;
	for (i = 1; i <= cObj; i++)
	{
		rgalt2[i] = cp0.latitude[i];
	}

	/* Loop through the day, dividing it into a certain number of segments. */
	/* For each segment we get the planet positions at its endpoints.       */

	for (div = 1; div <= division; div++)
	{
		ciCore = ciMain;
		ciCore.tim = DecDeg2DegMin(24.0 * (double) div / (double) division);
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

		/* For our segment, check to see if each planet during it rises, sets, */
		/* reaches its zenith, or reaches its nadir.                           */

		for (i = 1; i <= cObj; i++)
			if (!ignore[i] && FThing(i))
			{
				EclToHorizon(&azi1, &alt1, cp1.longitude[i], rgalt1[i], lon, lat, mc1);
				EclToHorizon(&azi2, &alt2, cp2.longitude[i], rgalt2[i], lon, lat, mc2);
				j = 0;

				/* Check for transits to the horizon. */
				if ((alt1 > 0.0) != (alt2 > 0.0))
				{
					d = fabs(alt1) / (fabs(alt1) + fabs(alt2));
					k = Mod(azi1 + d * MinDifference(azi1, azi2));
					j = 1 + 2 * (MinDistance(k, 180.0) < 90.0);

					/* Check for transits to the meridian. */
				}
				else if (RSgn(MinDifference(azi1, 90.0)) != RSgn(MinDifference(azi2, 90.0)))
				{
					j = 2 + 2 * (MinDistance(azi1, 90.0) < 90.0);
					d = fabs(azi1 - (j > 2 ? 90.0 : 270.0)) / MinDistance(azi1, azi2);
					k = alt1 + d * (alt2 - alt1);
					if (MCpolarT && hRevers)
						j = 2 + 2 * (MinDistance(azi1, 90.0) > 90.0);
				}
				if (j && !ignorez[j - 1] && occurcount < MAXINDAY)
				{
					source[occurcount] = i;
					type[occurcount] = j;
					time[occurcount] = 24.0 * ((double) (div - 1) + d) / (double) division *60.0;

					sign[occurcount] = (int) Mod(cp1.longitude[i] + d * MinDifference(cp1.longitude[i], cp2.longitude[i])) / 30 + 1;
					fRet[occurcount] = (int) RSgn(cp1.vel_longitude[i]) + (int) RSgn(cp2.vel_longitude[i]);
					azialt[occurcount] = k;
					ciSave = ciMain;
					ciSave.tim = DecDeg2DegMin(time[occurcount] / 60.0);
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
			SwapN(sign[j], sign[j + 1]);
			SwapN(fRet[j], fRet[j + 1]);
			SwapR(azialt[j], azialt[j + 1]);
			j--;
		}
	}

	/* Finally display the list showing each event and when it occurs. */

	for (i = 0; i < occurcount; i++)
	{
		ciSave = ciMain;
		ciSave.tim = DecDeg2DegMin(time[i] / 60.0);
		j = DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea);
		AnsiColor(kRainbowA[j + 1]);
		sprintf(sz, "(%c%c%c) ", chDay3(j));
		PrintSz(sz);
		AnsiColor(kDefault);
		sprintf(sz, "%s %s ", SzDate(ciMain.mon, ciMain.day, ciMain.yea, 2 * MonthFormat), 
			SzTim(DecDeg2DegMin(time[i] / 60.0)));
		PrintSz(sz);
		AnsiColor(kObjA[source[i]]);
		sprintf(sz, "%7.7s ", Lang(tObjName, source[i]));
		PrintSz(sz);
		AnsiColor(kSignA(sign[i]));
		sprintf(sz, "%c%c%c%c%c ",
				fRet[i] > 0 ? '(' : (fRet[i] < 0 ? '[' : '<'),
				chSig3(sign[i]), fRet[i] > 0 ? ')' : (fRet[i] < 0 ? ']' : '>'));
		PrintSz(sz);
		AnsiColor(kElemA[type[i] - 1]);
		if (type[i] == 1)
			PrintSz("rises    ");
		else if (type[i] == 2)
			PrintSz("culm.(up)");
		else if (type[i] == 3)
			PrintSz("sets     ");
		else
			PrintSz("culm.(lo)");
		AnsiColor(kDefault);
		PrintSz(" at ");
		if (type[i] & 1)
		{
			if (fNESW)
				azia = Mod(90.0 - azialt[i]);
			else
				azia = azialt[i];
			j = (int) (RFract(azia) * 60.0);
			se = RFract(azia) * 60.0;
			se = RFract(se) * 60.0;
			if (!us.fSeconds)
				sprintf(sz, "%3d%c%02d'", (int) azia, chDeg1, j);
			else
				sprintf(sz, "%3d%c%02d'%02d\"", (int) azia, chDeg1, j, (int) se);
			PrintSz(sz);

			/* For rising and setting events, we'll also display a direction  */
			/* vector to make the 360 degree azimuth value thought of easier. */

			xA = RCosD(azialt[i]);
			yA = RSinD(azialt[i]);
			if (fabs(xA) < fabs(yA))
			{
				xV = fabs(xA / yA);
				yV = 1.0;
			}
			else
			{
				yV = fabs(yA / xA);
				xV = 1.0;
			}
			sprintf(sz, " (%.2f%c %.2f%c)", yV, yA < 0.0 ? 's' : 'n', xV, xA > 0.0 ? 'e' : 'w');
			PrintSz(sz);
		}
		else
			PrintAltitude(azialt[i]);
		PrintL();
	}
	if (occurcount == 0)
		PrintSz("No horizon events found.\n");

	/* Recompute original chart placements as we've overwritten them. */

	ciCore = ciMain;
	ciTwin = ciT;
	us.fSidereal = fT;
	us.fEquator = EquT;
	PolarMCflip = MCpolarT;
	CastChart(TRUE);
}


/* Print out an ephemeris - the positions of the planets (at the time in the */
/* current chart) each day during a specified month, as done with the -E     */
/* switch. Display the ephemeris for the whole year if -Ey is in effect.     */

void ChartEphemeris()
{
	char sz[cchSzDef];
	int yea, yea1, yea2, mon, mon1, mon2, daysiz, i, j, s, d, m;

	/* If -Ey is in effect, then loop through all months in the whole year. */

	if (us.nEphemYears)
	{
		yea1 = ciMain.yea;
		yea2 = yea1 + us.nEphemYears - 1;
		mon1 = 1;
		mon2 = 12;
	}
	else
	{
		yea1 = yea2 = ciMain.yea;
		mon1 = mon2 = ciMain.mon;
	}

	/* Loop through the year or years in question. */

	for (yea = yea1; yea <= yea2; yea++)

		/* Loop through the month or months in question, printing each ephemeris. */

		for (mon = mon1; mon <= mon2; mon++)
		{
			daysiz = DayInMonth(mon, yea);
			PrintSz(us.fEuroDate ? "Dy/Mo/Yr" : "Mo/Dy/Yr");
			for (j = 1; j <= cObj; j++)
			{
				if (!ignore[j] && FThing(j))
				{
					sprintf(sz, "  %s%c%c%c%c", is.fSeconds ? "  " : "",
							chObj3(j), Lang(tObjShortName, j)[3] != 0 ? Lang(tObjShortName, j)[3] : ' ');
					PrintSz(sz);
					PrintTab(' ', us.fParallel ? 2 + is.fSeconds : 1 + 3 * is.fSeconds);
				}
			}
			PrintL();
			for (i = 1; i <= daysiz; i = AddDay(mon, i, yea, 1))
			{

				/* Loop through each day in the month, casting a chart for that day. */

				SetCI(&ciCore, mon, i, yea, ciMain.tim, ciMain.dst, ciMain.zon, ciMain.lon, ciMain.lat, ciMain.loc);
				CastChart(TRUE);
				PrintSz(SzDate(mon, i, yea, -1));
				PrintCh(' ');
				for (j = 0; j <= cObj; j++)
					if (!FIgnore(j) && FThing(j))
					{
						if (!us.fParallel)
						{
							if (is.fSeconds)
								PrintZodiac(cp0.longitude[j]);
							else
							{
								AnsiColor(kObjA[j]);
								s = Z2Sign(cp0.longitude[j]);
								d = (int) cp0.longitude[j] - (s - 1) * 30;
								m = (int) (RFract(cp0.longitude[j]) * 60.0);
								sprintf(sz, "%2d%s%02d", d, Lang(tSignAbbrev, s), m);
								PrintSz(sz);
							}
						}
						else
						{
							AnsiColor(kObjA[j]);
							PrintAltitude(cp0.latitude[j]);
						}
						PrintCh(((cp0.vel_longitude[j] >= 0.0) ? ' ' : '.'));
					}
				PrintL();
				AnsiColor(kDefault);
			}
			if (mon < mon2 || yea < yea2)
				PrintL();
		}

	ciCore = ciMain;			/* Recast original chart. */
	CastChart(TRUE);
}

/* charts3.c */
