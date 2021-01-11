/*
** Astrolog  File: charts2.c
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
** Dual Chart Display Routines.
******************************************************************************
*/

/* Print out an aspect (or midpoint if -g0 switch in effect) grid of a      */
/* relationship chart. This is similar to the ChartGrid() routine; however, */
/* here we have both axes labeled with the planets for the two charts in    */
/* question, instead of just a diagonal down the center for only one chart. */

void ChartGridRelation()
{
	char sz[cchSzDef];
	int i, j, k, tot = cObj, temp, m;
	byte ignoreT[NUMBER_OBJECTS];

	PrintHeader();				/* Display chart info */

#ifdef INTERPRET
	if (us.fInterpret && !us.fGridConfig)
	{
		InterpretGridRelation();
		return;
	}
#endif

	if (us.nRel == rcTransit)
	{
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = ignore2[m];
	}
	else if (us.nRel == rcProgress)
	{
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = ignore3[m];
	}
	else
	{
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = ignore[m];
	}

	PrintSz(" 2>");
	for (temp = 0, i = 0; i <= cObj; i++)
		if (!ignoreT[i] && i != us.objCenter)
		{
			PrintCh(chV);
			AnsiColor(kObjA[i]);
			sprintf(sz, "%c%c%c", chObj3(i));
			PrintSz(sz);
			AnsiColor(kDefault);
			temp++;
		}
	PrintSz("\n1  ");
	for (i = 0; i <= tot; i++)
		if (!ignoreT[i] && i != us.objCenter)
		{
			PrintCh(chV);
			AnsiColor(kSignA(Z2Sign(cp2.longitude[i])));
			sprintf(sz, "%2d%c", (int) cp2.longitude[i] % 30, chDeg0);
			PrintSz(sz);
			AnsiColor(kDefault);
		}
	PrintSz("\nV  ");
	for (i = 0; i <= tot; i++)
		if (!ignoreT[i] && i != us.objCenter)
		{
			PrintCh(chV);
			temp = Z2Sign(cp2.longitude[i]);
			AnsiColor(kSignA(temp));
			sprintf(sz, "%c%c%c", chSig3(temp));
			PrintSz(sz);
			AnsiColor(kDefault);
		}
	PrintL();
	for (j = 0; j <= cObj; j++)
		if (!ignore[j] && j != us.objCenter)
			for (k = 1; k <= 4; k++)
			{
				if (k < 2)
					PrintTab(chH, 3);
				else if (k == 2)
				{
					AnsiColor(kObjA[j]);
					sprintf(sz, "%c%c%c", chObj3(j));
					PrintSz(sz);
				}
				else
				{
					temp = Z2Sign(cp1.longitude[j]);
					AnsiColor(kSignA(temp));
					if (k == 3)
						sprintf(sz, "%2d%c", (int) cp1.longitude[j] - (temp - 1) * 30, chDeg0);
					else
						sprintf(sz, "%c%c%c", chSig3(temp));
					PrintSz(sz);
				}
				if (k > 1)
					AnsiColor(kDefault);
				for (i = 0; i <= tot; i++)
					if (!ignoreT[i] && i != us.objCenter)
					{
						PrintCh((byte) (k < 2 ? chC : chV));
						temp = grid->n[i][j];
						if (k > 1)
						{
							if (i == j)
								AnsiColor(kReverse);
							AnsiColor(us.fGridConfig ? kSignA(temp) : kAspA[temp]);
						}
						if (k < 2)
							PrintTab(chH, 3);
						else if (k == 2)
						{
							if (us.fGridConfig)
								sprintf(sz, "%c%c%c", chSig3(temp));
							else
								sprintf(sz, "%s", temp ? Lang(tAspectAbbrev, temp) : "   ");
							PrintSz(sz);
						}
						else if (k == 3)
						{
							if (us.fGridConfig)
							{
								sprintf(sz, "%2d%c", grid->v[i][j] / 60, chDeg0);
								PrintSz(sz);
							}
							else if (grid->n[i][j])
							{
								if (grid->v[i][j] < 6000)
									sprintf(sz, "%c%2d", us.fAppSep ?
											(grid->v[i][j] < 0 ? 'a' : 's') :
											(grid->v[i][j] < 0 ? '-' : '+'), abs(grid->v[i][j]) / 60);
								else
									sprintf(sz, "%3d", abs(temp) / 60);
								PrintSz(sz);
							}
							else
								PrintSz("   ");
						}
						else
						{
							if (grid->n[i][j])
							{
								sprintf(sz, "%02d'", abs(grid->v[i][j]) % 60);
								PrintSz(sz);
							}
							else
								PrintSz("   ");
						}
						AnsiColor(kDefault);
					}
				PrintL();
			}
}

void ChartAspectRelation()
{
	int ca[cAspect + 1], co[NUMBER_OBJECTS];
	char sz[cchSzDef];
	int ihi, jhi, ahi, i, j, k, m, count = 0;
	double ip, jp, savepower, rPowSum = 0.0, p;
	int orb, saveorb;
	GridInfo tempgrid;
	byte ignoreT[NUMBER_OBJECTS];
	double power2[cLastMoving + 1];


	memset(ca, 0, sizeof(ca));
	memset(co, 0, sizeof(co));
	memcpy(&tempgrid, grid, sizeof(GridInfo));

	PrintHeader();				/* Display chart info */
	PrintL();
	PrintTab(' ', 50);
	PrintSz(Lang(sAspectHdr));

	switch(us.nRel)
	{
	case rcTransit:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore2(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = rTransitInf[i];
		break;

	case rcProgress:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore3(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = ppower1[i];
		break;

	case rcDual:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = ppower2[i];
		break;

	default:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = rObjInf[i];

		break;
	}

	for (;;)
	{
		// we initialize now the two variables used for sorting, but only one 
		// of them will be used for sorting, depending on the sort type
		savepower = -1e10;
		saveorb = 0x7FFF;	

		// search for the next most powerful aspect or next smallest orb

		for (i = 0; i <= cObj; i++)
		{
			if (!ignoreT[i])
			{
				for (j = 0; j < cObj; j++)
				{
					if (!FIgnore(j))
					{
						if (k = tempgrid.n[i][j])
						// this looks for information on the other half of the grid,
						// as compared with ChartAspect, why?
						{
							ip = i <= cLastMoving ? power2[i] : 2.5;
							jp = j <= cLastMoving ? ppower1[j] : 2.5;
							orb = tempgrid.v[i][j];
							p =  rAspInf[k] * sqrt(ip * jp) *
								(1.0 - fabs((double) orb) / 60.0 / GetOrb(i, j, k));

							if (us.fParallel)
								p *= PowerPar;

							// here we sort by decreasing power or increasing orb
							if (fSortAspectsByOrbs && abs(orb) < abs(saveorb) || 
								!fSortAspectsByOrbs && p > savepower)
							{
								saveorb = orb;
								ihi = i;
								jhi = j;
								savepower = p;
								ahi = k;
							}
						}
					}
				}
			}
		}

		if (saveorb == 0x7FFF)		// exit when no other aspect found
			break;

		tempgrid.n[ihi][jhi] = 0;	// mark this aspect as having being processed
		count++;					// Display the current aspect

#ifdef INTERPRET
		if (us.fInterpret)
		{						/* Interpret it if -I in effect. */
			InterpretAspectRelation(jhi, ihi);
			continue;
		}
#endif
		rPowSum += savepower;
		ca[ahi]++;
		co[jhi]++;
		co[ihi]++;
		sprintf(sz, "%3d: ", count);
		PrintSz(sz);
		PrintAspect(jhi, Z2Sign(cp1.longitude[jhi]), (int) RSgn(cp1.vel_longitude[jhi]), ahi,
					ihi, Z2Sign(cp2.longitude[ihi]), (int) RSgn(cp2.vel_longitude[ihi]), 'A', 12, 12);
		AnsiColor(saveorb < 0 ? kWhite : kLtGray);
		sprintf(sz, " | %2d%c%02d' %c",
				abs(saveorb) / 60, chDeg1, abs(saveorb) % 60,
				us.fAppSep ? (saveorb < 0 ? 'a' : 's') : (saveorb < 0 ? '-' : '+'));
		PrintSz(sz);
		AnsiColor(kDkGreen);
		sprintf(sz, " | %9.2f |\n", savepower);
		PrintSz(sz);
		AnsiColor(kDefault);
	}

	if (us.fAspSummary)
	{
		PrintAspectSummary(ca, co, count, rPowSum);
	}
}


/* Display locations of all midpoints between objects in the relationship */
/* comparison chart, one per line, in sorted zodiac order from zero Aries */
/* onward, as specified with the -r0 -m switch combination.               */

void ChartMidpointRelation()
{
	int cs[NUMBER_OF_SIGNS + 1];
	char sz[cchSzDef];
	int mcut = -1, icut, jcut, mlo, ilo, jlo, m, i, j, count = 0, l;
	long lSpanSum = 0;
	byte ignoreT[NUMBER_OBJECTS];

	if (us.nRel == rcTransit)
	{
		for (l = 0; l <= cObj; l++)
			ignoreT[l] = FIgnore2(l);
	}
	else if (us.nRel == rcProgress)
	{
		for (l = 0; l <= cObj; l++)
			ignoreT[l] = FIgnore3(l);
	}
	else
	{
		for (l = 0; l <= cObj; l++)
			ignoreT[l] = FIgnore(l);
	}

	memset(cs, 0, sizeof(cs));

	for(;;)
	{
		mlo = 21600;

		/* Search for the next closest midpoint farther down in the zodiac. */

		for (i = 0; i <= cObj; i++)
			if (!FIgnore(i))
				for (j = 0; j <= cObj; j++)
					if (!ignoreT[j])
					{
						m = (grid->n[j][i] - 1) * 30 * 60 + grid->v[j][i];
						if ((m > mcut || (m == mcut && (i > icut || (i == icut && j > jcut)))) && m < mlo)
						{
							ilo = i;
							jlo = j;
							mlo = m;
						}
					}
		if (mlo >= 21600)		/* Exit when no midpoint farther in zodiac found. */
			break;
		mcut = mlo;
		icut = ilo;
		jcut = jlo;
		count++;				/* Display the current midpoint. */
#ifdef INTERPRET
		if (us.fInterpret)
		{						/* Interpret it if -I in effect. */
			InterpretMidpointRelation(ilo, jlo);
			continue;
		}
#endif
		cs[mlo / 60 / 30 + 1]++;
		sprintf(sz, "%4d: ", count);
		PrintSz(sz);
		PrintZodiac((double) mlo / 60.0);
		PrintCh(' ');
		PrintAspect(ilo, Z2Sign(cp1.longitude[ilo]), (int) RSgn(cp1.vel_longitude[ilo]), 0,
					jlo, Z2Sign(cp2.longitude[jlo]), (int) RSgn(cp2.vel_longitude[jlo]), 'M', 7, 10);
		AnsiColor(kDefault);
		m = (int) (MinDistance(cp1.longitude[ilo], cp2.longitude[jlo]) * 60.0);
		lSpanSum += m;
		sprintf(sz, "-%4d%c%02d' degree span.\n", m / 60, chDeg1, m % 60);
		PrintSz(sz);
	}

	PrintMidpointSummary(cs, count, lSpanSum);
}


/* Calculate any of the various kinds of relationship charts. This involves */
/* computing and storing the planet and house positions for the "core" and  */
/* "second" charts, and then combining them in the main single chart in the */
/* proper manner, e.g. for synastry, composite, time space midpoint charts. */

void CastRelation()
{
	byte ignoreT[NUMBER_OBJECTS];
	int i, saveRev;
	double ratio, t1, t2, t;
	double ppowerT[cLastMoving + 1];
	CI ciT;

	if (ciCore.mon == -1)
	{
		cp3 = cp0;
		fCP3 = 1;
	}

	/* Cast the first chart. */

	ciMain = ciCore;
	t1 = CastChart(TRUE);
	cp1 = cp0;
	saveRev = hRevers;
	if (us.nRel == rcTransit || us.nRel == rcProgress)
		for (i = 1; i <= cObj; i++)
		{
			cp1.vel_longitude[i] = 0.0;
			if (i > cLastMoving)
				continue;
			cp1.vel_latitude[i] = 0.0;
		}

	// why not do the grid and power when animating?? perhaps because the computers
	// were slow? It will be disabled for now

//	if (!wi.fAnimate)
//	{
		if (!FCreateGrid(FALSE))
			return;
		PlanetPPower();
//	}

	/* Cast the second chart. */

	ciCore = ciTwin;
	if (us.nRel == rcTransit)
	{
		for (i = 0; i <= cObj; i++)
		{
			ignoreT[i] = ignore[i];
			if (us.fSector || us.fAstroGraph)
				ignore[i] = ignore[i] && ignore2[i];
			else
				ignore[i] = ignore2[i];
		}
	}
	else if (us.nRel == rcProgress)
	{
		us.fProgressUS = TRUE;
		is.JDp = MdytszToJulian(ciCore.mon, ciCore.day, ciCore.yea, ciCore.tim, ciCore.dst, ciCore.zon);
		ciCore = ciMain;
		for (i = 0; i <= cObj; i++)
		{
			ignoreT[i] = ignore[i];
			if (us.fSector || us.fAstroGraph)
				ignore[i] = ignore[i] && ignore3[i];
			else
				ignore[i] = ignore3[i];
		}
	}
	t2 = CastChart(TRUE);
	if (us.nRel == rcTransit)
	{
		for (i = 0; i <= cObj; i++)
		{
			ignore2[i] = ignore[i];
			ignore[i] = ignoreT[i];
		}
	}
	else if (us.nRel == rcProgress)
	{
		us.fProgressUS = FALSE;
		for (i = 0; i <= cObj; i++)
		{
			ignore3[i] = ignore[i];
			ignore[i] = ignoreT[i];
		}
	}
	cp2 = cp0;

	if (us.nRel == rcDual)
	{
		if (!FCreateGrid(FALSE))
			return;
		for (i = 0; i <= cLastMoving; i++)
			ppowerT[i] = ppower1[i];
		PlanetPPower();
		for (i = 0; i <= cLastMoving; i++)
		{
			ppower2[i] = ppower1[i];
			ppower1[i] = ppowerT[i];
		}
	}

	hRevers = saveRev;
	ciCore = ciMain;

	/* Now combine the two charts based on what relation we are doing.   */
	/* For the standard -r synastry chart, use the house cusps of chart1 */
	/* and the planets positions of chart2.                              */

	ratio = (double) us.nRatio1 / ((double) (us.nRatio1 + us.nRatio2));
	if (us.nRel <= rcSynastry)
	{
		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
			cp0.cusp_pos[i] = cp1.cusp_pos[i];


	}
	/* For the -rc composite chart, take the midpoints of the planets/houses. */
	else if (us.nRel == rcComposite)
	{
		for (i = 0; i <= cObj; i++)
		{
			cp0.longitude[i] = Ratio(cp1.longitude[i], cp2.longitude[i], ratio);
			if (fabs(cp2.longitude[i] - cp1.longitude[i]) > 180.0)
				cp0.longitude[i] = Mod(cp0.longitude[i] + 360.0 * ratio);
			cp0.latitude[i] = Ratio(cp1.latitude[i], cp2.latitude[i], ratio);
			cp0.vel_longitude[i] = Ratio(cp1.vel_longitude[i], cp2.vel_longitude[i], ratio);
		}
		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
		{
			cp0.cusp_pos[i] = Ratio(cp1.cusp_pos[i], cp2.cusp_pos[i], ratio);
			if (fabs(cp2.cusp_pos[i] - cp1.cusp_pos[i]) > 180.0)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + 360.0 * ratio);
		}

		/* Make sure we don't have any 180 degree errors in house cusp    */
		/* complement pairs, which may happen if the cusps are far apart. */

		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
			if (MinDistance(cp0.cusp_pos[sCap], Mod(cp0.cusp_pos[i] - Sign2Z(i + 3))) > 90.0)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + 180.0);

		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
			if (fabs(MinDistance(cp0.cusp_pos[i], cp0.longitude[oAsc - 1 + i])) > 90.0)
				cp0.longitude[oAsc - 1 + i] = Mod(cp0.longitude[oAsc - 1 + i] + 180.0);
	}
	/* For the -rm time space midpoint chart, calculate the midpoint time and */
	/* place between the two charts and then recast for the new chart info.   */
	else if (us.nRel == rcMidpoint)
	{
		ciT = ciCore;
		is.T = Ratio(t1, t2, ratio);
		t = (is.T * 36525.0) + 0.5;
		is.JD = floor(t) + 2415020.0;
		ciCore.tim = RFract(t) * 24.0;
		ciCore.zon = Ratio(DegMin2DecDeg(ciMain.zon), DegMin2DecDeg(ciTwin.zon), ratio);
		ciCore.dst = Ratio(DegMin2DecDeg(ciMain.dst), DegMin2DecDeg(ciTwin.dst), ratio);
		ciCore.tim -= ciCore.zon - ciCore.dst;
		if (ciCore.tim < 0.0)
		{
			ciCore.tim += 24.0;
			is.JD -= 1.0;
		}
		JulianToMdy(is.JD, &ciCore.mon, &ciCore.day, &ciCore.yea);
		ciCore.lon = Ratio(DegMin2DecDeg(ciMain.lon), DegMin2DecDeg(ciTwin.lon), ratio);
		if (fabs(ciTwin.lon - ciMain.lon) > 180.0)
			ciCore.lon = Mod(ciCore.lon + 360.0 * ratio);
		ciCore.lat = Ratio(DegMin2DecDeg(ciMain.lat), DegMin2DecDeg(ciTwin.lat), ratio);
		ciCore.tim = DecDeg2DegMin(ciCore.tim);
		ciCore.dst = DecDeg2DegMin(ciCore.dst);
		ciCore.zon = DecDeg2DegMin(ciCore.zon);
		ciCore.lon = DecDeg2DegMin(ciCore.lon);
		ciCore.lat = DecDeg2DegMin(ciCore.lat);
		ciMain = ciCore;
		CastChart(TRUE);
		ciCore = ciT;


	}
	/* There are a couple of non-astrological charts, which only require the */
	/* number of days that have passed between the two charts to be done.    */
	else
		is.JD = fabs(t2 - t1) * 36525.0;
	ComputeInHouses();

	if (fCP3)
	{
		cp0 = cp3;
		fCP3 = 0;
	}
}


/*
******************************************************************************
** Other Chart Display Routines.
******************************************************************************
*/

/* Given two objects and an aspect between them, or an object and a sign  */
/* that it's entering, print if this is a "major" event, such as a season */
/* change or major lunar phase. This is called from the ChartInDay()      */
/* searching and influence routines. Do an interpretation if need be too. */

void PrintInDay(int source, int aspect, int dest) {
	if (aspect == aSig)
	{
		if (source == oSun)
		{
			AnsiColor(kWhite);
			if (dest == 1)
				PrintSz(" (Vernal Equinox)");	/* If the Sun changes sign, */
			else if (dest == 4)	/* then print out if this   */
				PrintSz(" (Summer Solstice)");	/* is a season change.      */
			else if (dest == 7)
				PrintSz(" (Autumnal Equinox)");
			else if (dest == 10)
				PrintSz(" (Winter Solstice)");
		}
	}
	else if ((aspect > 0) && !us.fParallel)
	{
		if (source == oSun && dest == oMoo)
		{
			if (aspect <= aSqu)
				AnsiColor(kWhite);
			if (aspect == aCon)
				PrintSz(" (New Moon)");	/* Print out if the present */
			else if (aspect == aOpp)	/* aspect is a New, Full,   */
				PrintSz(" (Full Moon)");	/* or Half Moon.            */
			else if (aspect == aSqu)
				PrintSz(" (Half Moon)");
		}
	}
	PrintL();

#ifdef INTERPRET
	if (us.fInterpret)
		InterpretInDay(source, aspect, dest);
#endif
	AnsiColor(kDefault);
}


/* Given two objects and an aspect (or one object, and an event such as a */
/* sign or direction change) display the configuration in question. This  */
/* is called by the many charts which list aspects among items, such as   */
/* the -a aspect lists, -m midpoint lists, -d aspect in day search and    */
/* influence charts, and -t transit search and influence charts.          */

void PrintAspect(int obj1, int sign1, int ret1, int asp, int obj2, 
				 int sign2, int ret2, byte chart, int field_length1, int field_length2)
{
	char sz[cchSzDef];

	AnsiColor(kObjA[obj1]);
	if (chart == 't')
		PrintSz("trans ");
	else if (chart == 'e' || chart == 'u' || chart == 'U')
		PrintSz("progr ");
	sprintf(sz, "%*.*s", field_length1, field_length1, Lang(tObjName, obj1));
	PrintSz(sz);
	AnsiColor(kSignA(sign1));
	sprintf(sz, " %c%c%c%c%c",
			ret1 > 0 ? '(' : (ret1 < 0 ? '[' : '<'), chSig3(sign1), ret1 > 0 ? ')' : (ret1 < 0 ? ']' : '>'));
	PrintSz(sz);
	AnsiColor(asp > 0 ? kAspA[asp] : kWhite);
	PrintCh(' ');
	if (asp == aSig)
		sprintf(sz, "-->");		/* Print a sign change. */
	else if (asp == aDir)
		sprintf(sz, "S/%c", obj2 ? chRet : 'D');	/* Print a direction change. */
	else if (asp == 0)
		sprintf(sz, chart == 'm' ? "&" : "with");
	else
		sprintf(sz, "%s", Lang(tAspectAbbrev, asp));	/* Print an aspect. */
	PrintSz(sz);
	if (asp != aDir)
		PrintCh(' ');
	if (asp == aSig)
	{
		AnsiColor(kSignA(obj2));
		sprintf(sz, "%s", Lang(tSignName, obj2));
		PrintSz(sz);
	}
	else if (asp >= 0)
	{
		AnsiColor(kSignA(sign2));
		if (chart == 't' || chart == 'u' || chart == 'U')
			PrintSz("natal ");
		sprintf(sz, "%c%c%c%c%c ",
				ret2 > 0 ? '(' : (ret2 < 0 ? '[' : '<'), chSig3(sign2), ret2 > 0 ? ')' : (ret2 < 0 ? ']' : '>'));
		PrintSz(sz);
		AnsiColor(kObjA[obj2]);
		sprintf(sz, "%.*s", field_length2, Lang(tObjName, obj2));
		PrintSz(sz);
	}
	if (chart == 'D' || chart == 'T' || chart == 'U' || chart == 'a' || chart == 'A' || chart == 'm' || chart == 'M')
		PrintTab(' ', field_length2 - strlen(Lang(tObjName, obj2)));
}


/* Based on the given chart information, display all the aspects taking   */
/* place in the chart, as specified with the -D switch. The aspects are   */
/* printed in order of influence determined by treating them as happening */
/* outside among transiting planets, such that rare outer planet aspects  */
/* are given more power than common ones among inner planets. (This is    */
/* almost identical to the -a list, except the influences are different.) */

void ChartInDayInfluence()
{
	int source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY], orbs[MAXINDAY];
	double power[MAXINDAY];
	char sz[cchSzDef];
	int occurcount = 0, i, j, k, l, orb;

	/* Go compute the aspects in the chart. */

	i = us.fAppSep;
	us.fAppSep = TRUE;			/* We always want applying vs. separating orbs. */
	FCreateGrid(FALSE);
	us.fAppSep = i;

	/* Search through the grid and build up the list of aspects. */

	for (j = 1; j <= cObj; j++)
	{
		if (FIgnore(j))
			continue;
		for (i = 0; i < j; i++)
		{
			if (FIgnore(i) || (k = grid->n[i][j]) == 0 || occurcount >= MAXINDAY)
				continue;
			source[occurcount] = i;
			aspect[occurcount] = k;
			dest[occurcount] = j;
			orbs[occurcount] = grid->v[i][j];
			power[occurcount] =
				(sqrt) ((i <= cLastMoving ? rTransitInf[i] : 2.5) *
						(j <= cLastMoving ? rTransitInf[j] : 2.5)) *
				rAspInf[k] * (1.0 - (double) abs(orbs[occurcount]) / 60.0 / GetOrb(i, j, k));
			occurcount++;
		}
	}

	/* Sort aspects by order of power or orb. */

	for (i = 1; i < occurcount; i++)
	{
		j = i - 1;
		while (j >= 0 && (!fSortAspectsByOrbs && power[j] < power[j + 1] || 
			fSortAspectsByOrbs && abs(orbs[j]) > abs(orbs[j + 1])))
		{
			SwapN(source[j], source[j + 1]);
			SwapN(aspect[j], aspect[j + 1]);
			SwapN(dest[j], dest[j + 1]);
			SwapN(orbs[j], orbs[j + 1]);
			SwapR(power[j], power[j + 1]);
			j--;
		}
	}

	PrintL();
	PrintTab(' ', 49);
	PrintSz(Lang(sAspectHdr));

	/* Now display each aspect line. */

	for (i = 0; i < occurcount; i++)
	{
		sprintf(sz, "%3d: ", i + 1);
		PrintSz(sz);
		j = source[i];
		k = aspect[i];
		l = dest[i];
		PrintAspect(j, Z2Sign(cp0.longitude[j]), (int) RSgn(cp0.vel_longitude[j]), k, l, 
			Z2Sign(cp0.longitude[l]), (int) RSgn(cp0.vel_longitude[l]), 'D', 12, 12);
		orb = orbs[i];
		AnsiColor(orb < 0 ? kWhite : kLtGray);
		sprintf(sz, "| %2d%c%02d' %c", abs(orb) / 60, chDeg1, abs(orb) % 60, 
			us.fAppSep ? (orb < 0 ? 'a' : 's') : (orb < 0 ? '-' : '+'));
		PrintSz(sz);
		AnsiColor(kDkGreen);
		sprintf(sz, " | %9.2f |", power[i]);
		PrintSz(sz);
		PrintInDay(j, k, l);
	}

	if (occurcount == 0)
		PrintSz("Empty transit aspect list.\n");
}


/* Given an arbitrary day, determine what aspects are made between this */
/* transiting chart and the given natal chart, as specified with the -T */
/* switch, and display the transits in order sorted by influence.       */

void ChartTransitInfluence(BOOL fProg) {
	int source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY], orbs[MAXINDAY];
	double power[MAXINDAY];
	byte ignoreT[NUMBER_OBJECTS];
	char sz[cchSzDef];
	int occurcount = 0, fProgress = us.fProgressUS, i, j, k, l, orb;

	PrintHeader();				/* Display chart info */

	PlanetPPower();

	/* Cast the natal and transiting charts as with a relationship chart. */

	if (ciCore.mon == -1)
	{
		cp3 = cp0;
		fCP3 = 1;
	}

	cp1 = cp0;
	for (i = 0; i <= cObj; i++)
	{
		ignoreT[i] = ignore[i];
		ignore[i] = ignore2[i];
	}

	ciCore = ciTran;
	if (us.fProgressUS = fProg)
	{
		is.JDp = MdytszToJulian(ciCore.mon, ciCore.day, ciCore.yea, ciCore.tim, ciCore.dst, ciCore.zon);
		ciCore = ciMain;
	}
	CastChart(TRUE);
	cp2 = cp0;
	for (i = 0; i <= cObj; i++)
	{
		ignore[i] = ignoreT[i];
	}

	/* Do a relationship aspect grid to get the transits. We have to make and */
	/* restore three changes to get it right for this chart. (1) We make the  */
	/* natal planets have zero velocity so applying vs. separating is only a  */
	/* function of the transiter. (2) We force applying vs. separating orbs   */
	/* regardless if -ga or -ma is in effect or not. (3) Finally we tweak the */
	/* main restrictions to allow for transiting objects not restricted.      */

	for (i = 0; i <= cObj; i++)
	{
		cp0.vel_longitude[i] = cp1.vel_longitude[i];
		cp1.vel_longitude[i] = 0.0;
		ignoreT[i] = ignore[i];
		ignore[i] = ignore[i] && ignore2[i];
	}
	i = us.fAppSep;
	us.fAppSep = TRUE;
	FCreateGridRelation(FALSE);
	us.fAppSep = i;
	for (i = 0; i <= cObj; i++)
	{
		cp1.vel_longitude[i] = cp0.vel_longitude[i];
		ignore[i] = ignoreT[i];
	}

	/* Loop through the grid, and build up a list of the valid transits. */

	for (i = 0; i <= cLastMoving; i++)
	{
		if (FIgnore2(i))
			continue;
		for (j = 0; j <= cObj; j++)
		{
			if (FIgnore(j) || (is.fReturn && i != j) || (k = grid->n[i][j]) == 0 || occurcount >= MAXINDAY)
				continue;
			source[occurcount] = i;
			aspect[occurcount] = k;
			dest[occurcount] = j;
			orbs[occurcount] = grid->v[i][j];
			power[occurcount] = rAspInf[k] *
				(sqrt) ((us.fProgressUS ? ppower1[i] : rTransitInf[i]) *
				(j <= cLastMoving ? ppower1[j] : 2.5)) * 
				(1.0 - (double) abs(orbs[occurcount]) / 60.0 / GetOrb(i, j, k));
			occurcount++;
		}
	}

	/* After all transits located, sort them by their power or orb. */

	for (i = 1; i < occurcount; i++)
	{
		j = i - 1;
		while (j >= 0 && (!fSortAspectsByOrbs && power[j] < power[j + 1] || 
			fSortAspectsByOrbs && abs(orbs[j]) > abs(orbs[j + 1])))
		{
			SwapN(source[j], source[j + 1]);
			SwapN(aspect[j], aspect[j + 1]);
			SwapN(dest[j], dest[j + 1]);
			SwapN(orbs[j], orbs[j + 1]);
			SwapR(power[j], power[j + 1]);
			j--;
		}
	}

	PrintSz("\n          Transit                 Natal           Orb         Power\n\n");

	/* Now loop through list and display each transit in effect at the time. */

	for (i = 0; i < occurcount; i++)
	{
		k = aspect[i];
		l = source[i];
		sprintf(sz, "%3d: ", i + 1);
		PrintSz(sz);
		j = Z2Sign(cp2.longitude[l]);
		PrintAspect(l, j, (int) RSgn(cp2.vel_longitude[l]), k, dest[i], 
			Z2Sign(cp1.longitude[dest[i]]), (int) RSgn(cp1.vel_longitude[dest[i]]), (byte) (fProg ? 'U' : 'T'), 12, 12);
		orb = orbs[i];
		AnsiColor(orb < 0 ? kWhite : kLtGray);
		sprintf(sz, "| %2d%c%02d' %c", abs(orb) / 60, chDeg1, abs(orb) % 60, 
			us.fAppSep ? (orb < 0 ? 'a' : 's') : (orb < 0 ? '-' : '+'));
		PrintSz(sz);
		AnsiColor(kDkGreen);
		sprintf(sz, " | %8.2f |", power[i]);
		PrintSz(sz);
		if (k == aCon && l == dest[i])
		{						
			AnsiColor(kWhite);
			PrintSz(" Return");
		}
		PrintL();
#ifdef INTERPRET
		if (us.fInterpret)
			InterpretTransit(l, k, dest[i]);
#endif
		AnsiColor(kDefault);
	}
	if (occurcount == 0)
		PrintSz("Empty transit list.\n");

	if (fCP3)
	{
		cp0 = cp3;
		fCP3 = 0;
	}

	us.fProgressUS = fProgress;
	ciCore = ciMain;
	CastChart(TRUE);
}


/* Given the zodiac location of a planet in the sky and its declination,   */
/* and a location on the Earth, compute the azimuth and altitude of where  */
/* on the local horizon sky the planet would appear to one at the given    */
/* location. A reference MC position at Greenwich is also needed for this. */

void EclToHorizon(double *azi, double *alt, double obj, double objalt, double lon, double lat, double mc) {
	double lonz, latz;

	lonz = Deg2Rad(obj);
	latz = Deg2Rad(objalt);
	EclToEqu(&lonz, &latz);
	lonz = Deg2Rad(Mod(Rad2Deg(mc - lonz + lon)));
	lonz = Deg2Rad(Mod(Rad2Deg(lonz - lon + rPiHalf)));
	EquToLocal(&lonz, &latz, rPiHalf - lat);
	*azi = 360.0 - Rad2Deg(lonz);
	*alt = Rad2Deg(latz);
}


/* Display a calendar for the given month in the chart, as specified with  */
/* with the -K switch. When color is on, the title is white, weekends are  */
/* highlighted in red, and the specific day in the chart is colored green. */

void ChartCalendarMonth()
{
	char sz[cchSzDef];
	int i, j, k;

	AnsiColor(kWhite);
	PrintTab(' ', (16 - strlen(Lang(tMonth, ciMain.mon))) >> 1);
	sprintf(sz, "%s%5d\n", Lang(tMonth, ciMain.mon), ciMain.yea);
	PrintSz(sz);
	for (i = 0; i < cWeek; i++)
	{
		sprintf(sz, "%c%c%c", Lang(tDay, i)[0], Lang(tDay, i)[1], i < cWeek - 1 ? ' ' : '\n');
		PrintSz(sz);
	}
	j = DayOfWeek(ciMain.mon, 1, ciMain.yea);
	AnsiColor(kDefault);
	for (i = 0; i < j; i++)
	{
		if (i == 0)
			AnsiColor(kRainbowA[1]);
		PrintSz("-- ");
		if (i == 0)
			AnsiColor(kDefault);
	}
	k = DayInMonth(ciMain.mon, ciMain.yea);
	for (i = 1; i <= k; i = AddDay(ciMain.mon, i, ciMain.yea, 1))
	{
		if (i == (int) ciMain.day)
			AnsiColor(kRainbowA[4]);
		else if (j == 0 || j == cWeek - 1)
			AnsiColor(kRainbowA[1]);
		sprintf(sz, "%2d", i);
		PrintSz(sz);
		if (j == 0 || j == cWeek - 1 || i == ciMain.day)
			AnsiColor(kDefault);
		if (j < cWeek - 1)
		{
			j++;
			PrintCh(' ');
		}
		else
		{
			j = 0;
			PrintL();
		}
	}
	while (j > 0 && j < cWeek)
	{
		if (j == cWeek - 1)
			AnsiColor(kRainbowA[1]);
		j++;
		sprintf(sz, "--%c", j < cWeek ? ' ' : '\n');
		PrintSz(sz);
	}
	AnsiColor(kDefault);
}


/* Display a calendar for the entire year given in the chart, as specified */
/* with the -Ky switch. This is just like twelve of the individual month   */
/* calendars above displayed together, with same color highlights and all. */

void ChartCalendarYear()
{
	char sz[cchSzDef];
	int r, w, c, m, d, dy, p[3], l[3], n[3];

	dy = DayOfWeek(1, 1, ciMain.yea);
	for (r = 0; r < 4; r++)
	{							/* Loop over one set of three months */
		AnsiColor(kWhite);
		for (c = 0; c < 3; c++)
		{
			m = r * 3 + c + 1;
			PrintTab(' ', (16 - strlen(Lang(tMonth, m))) >> 1);
			sprintf(sz, "%s%5d", Lang(tMonth, m), ciMain.yea);
			PrintSz(sz);
			if (c < 2)
				PrintTab(' ', 20 + MONTHSPACE - ((16 - strlen(Lang(tMonth, m))) >> 1) - strlen(Lang(tMonth, m)) - 5);
		}
		PrintL();
		for (c = 0; c < 3; c++)
		{
			for (d = 0; d < cWeek; d++)
			{
				sprintf(sz, "%c%c%c", Lang(tDay, d)[0], Lang(tDay, d)[1], d < cWeek - 1 || c < 2 ? ' ' : '\n');
				PrintSz(sz);
			}
			if (c < 2)
				PrintTab(' ', MONTHSPACE - 1);
			m = r * 3 + c + 1;
			p[c] = dy % cWeek;
			l[c] = DayInMonth(m, ciMain.yea);
			n[c] = 0;
			dy += DaysInMonth(m, ciMain.yea);
		}
		for (w = 0; w < cWeek - 1; w++)
		{						/* Loop over one set of week rows */
			for (c = 0; c < 3; c++)
			{					/* Loop over one week in a month  */
				m = r * 3 + c + 1;
				d = 0;
				if (w == 0)
					while (d < p[c])
					{
						if (d == 0)
							AnsiColor(kRainbowA[1]);
						PrintSz("-- ");
						if (d == 0)
							AnsiColor(kDefault);
						d++;
					}
				AnsiColor(kDefault);
				while (d < cWeek && n[c] < l[c])
				{
					n[c] = AddDay(m, n[c], ciMain.yea, 1);
					if (n[c] == ciMain.day && m == ciMain.mon)
						AnsiColor(kRainbowA[4]);
					else if (d == 0 || d == cWeek - 1)
						AnsiColor(kRainbowA[1]);
					sprintf(sz, "%2d%c", n[c], d < cWeek - 1 || c < 2 ? ' ' : '\n');
					PrintSz(sz);
					if (d == 0 || d == cWeek - 1 || (n[c] == ciMain.day && m == ciMain.mon))
						AnsiColor(kDefault);
					d++;
				}
				while (d < cWeek)
				{
					if (d == 0 || d == cWeek - 1)
						AnsiColor(kRainbowA[1]);
					sprintf(sz, "--%c", d < cWeek - 1 || c < 2 ? ' ' : '\n');
					PrintSz(sz);
					if (d == 0)
						AnsiColor(kDefault);
					d++;
				}
				if (c < 2)
					PrintTab(' ', MONTHSPACE - 1);
			}
		}
		if (r < 3)
			PrintL();
	}
	AnsiColor(kDefault);
}


/* Display the time difference in various units between two charts */
void DisplayTimeDifference()
{
	char sz[cchSzDef];
	int i;

	PrintSz("\nDifferences between the dates in the two charts:\n\n");
	for (i = 1; i <= 7; i++)
	{
		AnsiColor(kRainbowA[i]);

		switch (i)
		{
		case 1:
			sprintf(sz, "Years  : %.0f", is.JD / 365.25);
			break;
		case 2:
			sprintf(sz, "Months : %.0f", is.JD / (365.25 / 12));
			break;
		case 3:
			sprintf(sz, "Weeks  : %.0f", is.JD / 7.0);
			break;
		case 4:
			sprintf(sz, "Days   : %.0f", is.JD);
			break;
		case 5:
			sprintf(sz, "Hours  : %.0f", is.JD * 24.0);
			break;
		case 6:
			sprintf(sz, "Minutes: %.0f", is.JD * 24.0 * 60.0);
			break;
		case 7:
			sprintf(sz, "Seconds: %.0f", is.JD * 24.0 * 3600.0);
			break;
		}
		PrintSz(sz);
		PrintL();
	}

	AnsiColor(kDefault);
	return;
}

/* Display a biorhythm chart */
void DisplayBiorhythm(void)
{
#ifdef BIORHYTHM
	char sz[cchSzDef];
	int i;
	int j;
	double k, l;


	/* If we are doing a biorhythm (-rb switch), then we'll calculate it for */
	/* someone born on the older date, at the time of the younger date. Loop */
	/* through the week preceeding and following the date in question.       */

	PrintSz("\nBiorhythm:\n\n");

	is.JD = floor(is.JD + 0.5);
	for (is.JD -= (double) (us.nBioday / 2), i = -us.nBioday / 2; i <= us.nBioday / 2; i++, is.JD += 1.0)
	{
		if (i == 0)
			AnsiColor(kWhite);
		else if (i == 1)
			AnsiColor(kDefault);
		j = abs(i);
		sprintf(sz, "T%c%d%sDay%c:", i < 0 ? '-' : '+', j, j < 10 ? " " : "", j != 1 ? 's' : ' ');
		PrintSz(sz);
		for (j = 1; j <= 3; j++)
		{
			PrintCh(' ');
			switch (j)
			{
			case 1:
				k = brPhy;
				AnsiColor(kRed);
				PrintSz("Physical");
				break;
			case 2:
				k = brEmo;
				AnsiColor(kBlue);
				PrintSz("Emotional");
				break;
			case 3:
				k = brInt;
				AnsiColor(kGreen);
				PrintSz("Intellectual");
				break;
			}
			AnsiColor(i ? kDefault : kWhite);

			/* The biorhythm calculation is below. */

			l = RBiorhythm(is.JD, k);
			sprintf(sz, " at %c%3.0f%%", l < 0.0 ? '-' : '+', fabs(l));
			PrintSz(sz);

			/* Print smiley face, medium face, or sad face based on current cycle. */

			AnsiColor(kPurple);
			sprintf(sz, " :%c", l > 50.0 ? ')' : (l < -50.0 ? '(' : '|'));
			PrintSz(sz);
			AnsiColor(i ? kDefault : kWhite);
			if (j < 3)
				PrintCh(',');
		}
		PrintL();
	}

#endif /* BIORHYTHM */
}

/* charts2.c */
