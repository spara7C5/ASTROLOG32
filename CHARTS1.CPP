/*
** Astrolog  File: charts1.c
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
** Single Chart Display Routines.
******************************************************************************
*/


/* Print header info showing time and date of the chart being displayed.   */
/* This is used by ChartListing() and the -l sector chart in PrintChart(). */

void PrintHeader()
{
	char sz[cchSzDef];
	int day, fNam, fLoc;
	CI ciT;

	if (NoPrintHeader)
		return;
	NoPrintHeader = 1;

	ciT = ciMain;
	if (us.fProgressUS && !us.nRel)
	{
		ciMain = ciCore;
	}

	fNam = *ciMain.nam > chNull;
	fLoc = *ciMain.loc > chNull;
	AnsiColor(kWhite);
	if (us.nRel == rcDual)
	{
		sprintf(sz, Lang(sDualChart), szAppName);
		PrintSz(sz);
	}
	else if (us.nRel == rcSynastry)
	{
		sprintf(sz, Lang(sSynastryChart), szAppName);
		PrintSz(sz);
	}
	else
	{
		sprintf(sz, Lang(sChart), szAppName);
		PrintSz(sz);
	}
	if (ciMain.mon == -1)
		PrintSz("(No time or space)\n");
	else if (us.nRel == rcComposite)
		PrintSz(Lang(sComposite));
	else
	{
		if (us.nRel == rcMidpoint)
			PrintSz(Lang(sTimeSpaceMidpoint));
		else
		{
			sprintf(sz, Lang(sFor), ciMain.nam);
			PrintSz(sz);
		}
		day = DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea);
		sprintf(sz, "%c%c%c %s %s (%s %s GMT)", chDay3(day),
				SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), 
				SzTim(ciMain.tim), ChDst(ciMain.dst), SzZone(ciMain.zon));
		PrintSz(sz);
		if (us.nRel == rcMidpoint)
		{
			sprintf(sz, " %s\n", SzLocation(ciMain.lon, ciMain.lat));
			PrintSz(sz);
		}
		else
		{
			sprintf(sz, "%c%s%s%s\n", fLoc && !fNam ? '\n' : ' ', 
				ciMain.loc, fLoc ? " " : "", SzLocation(ciMain.lon, ciMain.lat));
			PrintSz(sz);
		}
	}
	if (us.fHorizon)
	{
		if (us.fPrimeVert)
			sprintf(sz, "Prime Vertical\n");
		else
			sprintf(sz, "Local Horizon\n");
		PrintSz(sz);
	}
	if (us.fProgressUS || us.nRel == rcTransit || us.nRel == rcDual
		|| us.nRel == rcSynastry || us.nRel == rcProgress || us.fTransitInf)
	{
		if (us.fProgressUS && !us.nRel)
		{
			if (us.fSolarArc == 1)
				sprintf(sz, "Chart directed (Degree per Year/Month) to\n");
			else if (us.fSolarArc == 2)
				sprintf(sz, "Chart directed (Solar Arc) to\n");
			else
				sprintf(sz, "Chart progressed to\n");
		}
		else if (us.fTransitInf)
		{
			sprintf(sz, Lang(sTransits));
		}
		else
		{
			if (us.nRel == rcProgress)
			{
				if (!us.fInterpret && (us.fInfluence || us.fListing))
				{
					if (us.fSolarArc == 1)
						sprintf(sz, "Natal houses, planets directed (Degree per Year/Month) to\n");
					else if (us.fSolarArc == 2)
						sprintf(sz, "Natal houses, planets directed (Solar Arc) to\n");
					else
						sprintf(sz, "Natal houses, planets progressed to\n");
				}
				else
				{
					if (us.fSolarArc)
						sprintf(sz, "Progressions for\n");
					else
						sprintf(sz, "Progressions for\n");
				}
			}
			else if (us.nRel == rcTransit)
			{
				if (!us.fInterpret && (us.fInfluence || us.fListing))
					sprintf(sz, "\nNatal houses, transit planets for:\n");
				else
					sprintf(sz, "\nTransits for:\n");
			}
			else if (us.nRel == rcSynastry)
			{
				sprintf(sz, "planets for ");
			}
			else if (us.nRel == rcDual)
			{
				sprintf(sz, "\nand ");
			}
			else
				sprintf(sz, "and\n");
		}
		PrintSz(sz);
		if (us.fProgressUS && !us.nRel)
			ciMain = ciT;
		else if (us.fTransitInf)
			ciMain = ciTran;
		else
			ciMain = ciTwin;
		if (us.nRel == rcDual || us.nRel == rcSynastry)
		{
			sprintf(sz, "%s\n", ciMain.nam);
			PrintSz(sz);
		}
		day = DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea);
		sprintf(sz, "%c%c%c %s %s (%s %s GMT)", chDay3(day),
			SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), 
			SzTim(ciMain.tim), ChDst(ciMain.dst), SzZone(ciMain.zon));
		PrintSz(sz);
		sprintf(sz, "%c%s%s%s\n", fLoc && !fNam ? '\n' : ' ', ciMain.loc, 
			fLoc ? " " : "", SzLocation(ciMain.lon, ciMain.lat));
		PrintSz(sz);
		ciMain = ciT;
	}
}


/* Print the straight listing of planet and house positions and specified */
/* by the -v switch, along with the element table, etc.                   */

void ChartListing()
{
	ET et;
	char sz[cchSzDef];
	int obj_left, obj_right, i, j, k, l;
	double rT;

	CreateElemTable(&et);
	PrintHeader();				/* Show time and date of the chart being displayed. */

#ifdef INTERPRET
	if (us.fInterpret)
	{							/* Print an interpretation if -I in effect. */
		if (us.nRel == rcSynastry)
		{
			InterpretSynastry();	/* Print synastry interpretaion for -r -I.  */
		}
		else
		{
#ifdef INTERPRETALT
			if (InterpretAlt && (!us.nRel || us.nRel == rcTransit))
			{
				if (is.fMult)
					PrintL2();

				if (us.nRel == rcTransit)
					TransitInterpret();
				else
					NatalInterpret();

				is.fMult = TRUE;
			}
			else
			{
#endif
				InterpretLocation();	/* Do normal interpretation for just -v -I. */
#ifdef INTERPRETALT
			}
#endif
		}
		return;
	}
#endif

	PrintL();
	AnsiColor(kDefault);
	if (us.fSeconds)
	{
		sprintf(sz,
				Lang(sChartHdr), us.fEquator ? "Declin. " : "Latitude");
		PrintSz(sz);
	}
	else
	{
		sprintf(sz,
				Lang(sChartHdrSeconds),
				us.fEquator ? "Decl" : "Lati", Lang(tHouseSystem, us.nHouseSystem));
		PrintSz(sz);
	}

	/* Ok, now print out the location of each object. */

	for (obj_right = 1 - us.fSeconds, obj_left = 0; obj_right <= cLastMoving; obj_right++, obj_left++)
	{
		if (us.fSeconds)
		{
			if (FIgnore(obj_right))
			{
				continue;
			}
		}
		else
		{
			if (obj_right > cuspLo && ((obj_right < cuspHi) || FIgnore(obj_right)))
			{
				continue;
			}

			while (obj_right <= cuspHi && obj_left <= cuspHi && FIgnore(obj_left))
			{
				obj_left++;
			}
		}

		if (obj_right >= cuspLo && obj_right <= cuspHi - 0 && obj_left > cuspHi - 0)	/* Don't print blank */
		{
			continue;											/* lines.      VA.   */
		}													   /* all 0s were 2s, TK */

		if (obj_right <= cuspHi - 0 && obj_left > cuspHi - 0)
		{
			PrintTab(' ', 52);
		}
		else
		{
			if (obj_right > cuspHi - 0)
			{
				obj_left = obj_right;
			}
			AnsiColor(kObjA[obj_left]);
			sprintf(sz, "%-10.10s: ", Lang(tObjName, obj_left));
			PrintSz(sz);
			PrintZodiac(cp0.longitude[obj_left]);
			sprintf(sz, " %c ", cp0.vel_longitude[obj_left] >= 0.0 ? ' ' : chRet);
			PrintSz(sz);

			if (obj_left <= cThing || obj_left > cuspHi)
			{
				PrintAltitude(cp0.latitude[obj_left]);
			}
			else
			{
				PrintTab('_', us.fSeconds ? 10 : 7);
			}

			sprintf(sz, " (%c)", Dignify(obj_left, Z2Sign(cp0.longitude[obj_left])));
			PrintSz(FCusp(obj_left) ? "    " : sz);
			k = cp0.house_no[obj_left];
			l = Z2Sign(cp0.cusp_pos[k]);
			AnsiColor(kSignA(k));
			sprintf(sz, " [%2d%s] ", k, Lang(tSuffix, k));
			PrintSz(sz);
			AnsiColor(kDefault);
			sprintf(sz, "[%c", Dignify(obj_left, k));
			PrintSz(FCusp(obj_left) ? "    " : sz);
			sprintf(sz, "%c] ", Dignify(obj_left, l));
			PrintSz(FCusp(obj_left) ? " " : sz);

			if ((FObject(obj_left) || ((obj_left == oNoNode || obj_left == oSoNode || obj_left == oLil))))
			{
				PrintCh((byte) (cp0.vel_longitude[obj_right] < 0.0 ? '-' : '+'));
				rT = Rad2Deg(fabs(cp0.vel_longitude[obj_left]));
				sprintf(sz, us.fSeconds ? (rT < 10.0 ? "%9.7f" : "%9.6f") : (rT < 10.0 ? "%5.3f" : "%5.2f"), rT);
				PrintSz(sz);
			}
			else
			{
				PrintTab('_', us.fSeconds ? 10 : 6);
			}
		}
		if (!us.fSeconds)
		{

			/* For some lines, we have to append the house cusp positions. */

			if (obj_right <= NUMBER_OF_SIGNS)
			{
				PrintSz("  -  ");
				AnsiColor(kSignA(obj_right));
				sprintf(sz, "%s %2d: ", Lang(sHouseCusp), obj_right);
				PrintSz(sz);
				PrintZodiac(cp0.cusp_pos[obj_right]);
			}

			/* For some lines, we have to append the element table information. */

			if (obj_right == NUMBER_OF_SIGNS + 2)
			{
				PrintSz(Lang(sCFMT));
			}
			else if (obj_right > NUMBER_OF_SIGNS + 2 && obj_right < NUMBER_OF_SIGNS + 7)
			{
				k = obj_right - (NUMBER_OF_SIGNS + 2) - 1;
				AnsiColor(kElemA[k]);
				sprintf(sz, "  %c%c%c%3d %3d %3d %3d",
						Lang(tElement, k)[0], Lang(tElement, k)[1], Lang(tElement, k)[2],
						et.coElemMode[k][0], et.coElemMode[k][1], et.coElemMode[k][2], et.coElem[k]);
				PrintSz(sz);
				AnsiColor(kDefault);
			}
			else if (obj_right == NUMBER_OF_SIGNS + 7)
			{
				sprintf(sz, "  TOT %2d %3d %3d %3d", et.coMode[0], et.coMode[1], et.coMode[2], et.coSum);
				PrintSz(sz);
			}
			else if (obj_right == oCore)
			{
				PrintTab(' ', 23);
			}
			else if (obj_right >= uranLo)
			{
				sprintf(sz, "  Uranian #%d", obj_right - uranLo + 1);
				PrintSz(sz);
			}
			sz[0] = chNull;

			switch (obj_right - NUMBER_OF_SIGNS - 1)
			{
			case 1:
				sprintf(sz, "   +:%2d", et.coYang);
				break;
			case 2:
				sprintf(sz, "   -:%2d", et.coYin);
				break;
			case 3:
				sprintf(sz, "   M:%2d", et.coMC);
				break;
			case 4:
				sprintf(sz, "   N:%2d", et.coIC);
				break;
			case 5:
				sprintf(sz, "   A:%2d", et.coAsc);
				break;
			case 6:
				sprintf(sz, "   D:%2d", et.coDes);
				break;
			case 7:
				sprintf(sz, "<:%2d", et.coLearn);
				break;
			}
			PrintSz(sz);
		}
		else	// print seconds
		{
			PrintSz(" Decan: ");
			is.fSeconds = FALSE;
			PrintZodiac(Decan(cp0.longitude[obj_right]));
			is.fSeconds = us.fSeconds;
		}
		PrintL();
	}

	/* Do another loop to print out the stars in their specified order. */

	if (us.nStar)
	{
		for (i = starLo; i <= starHi; i++)
		{
			if (!ignore[uranHi + starname[i - uranHi]])
			{
				j = cLastMoving + starname[i - cLastMoving];
				AnsiColor(kObjA[j]);
				sprintf(sz, "%-10.10s: ", Lang(tObjName, j));
				PrintSz(sz);
				PrintZodiac(cp0.longitude[j]);
				PrintSz("   ");
				PrintAltitude(cp0.latitude[j]);
				k = cp0.house_no[j];
				AnsiColor(kSignA(k));
				sprintf(sz, "     [%2d%s]", k, Lang(tSuffix, k));
				PrintSz(sz);
				AnsiColor(kDefault);
				sprintf(sz, "   %s %8.8s  Star #%2d: %5.2f\n",
						us.fSeconds ? "   " : "", szStarConName[j - cLastMoving], i - cLastMoving, rStarBright[j - cLastMoving]);
				PrintSz(sz);
			}
		}
	}
}


/* Print out the aspect and midpoint grid for a chart, as specified with the */
/* -g switch. (Each grid row takes up 4 lines of text.)                      */

void ChartGrid()
{
	char sz[cchSzDef];
	int x, y, r, x1, y1, temp;

	PrintHeader();				/* Display chart info */

#ifdef INTERPRET
	if (us.fInterpret)
	{							/* Print interpretation instead if -I in effect. */
		InterpretGrid();
		return;
	}
#endif

	for (y1 = 0, y = 0; y <= cObj; y++)
		if (FProper(y))
			for (r = 1; r <= 4; r++)
			{
				for (x1 = 0, x = 0; x <= cObj; x++)
					if (FProper(x))
					{
						if (y1 > 0 && x1 > 0 && y + r > 1)
							PrintCh((byte) (r > 1 ? chV : chC));
						if (r > 1)
						{
							temp = grid->n[x][y];

							/* Print aspect rows. */

							if (x < y)
							{
								if (temp)
									AnsiColor(kAspA[temp]);
								if (r == 2)
									PrintSz(temp ? Lang(tAspectAbbrev, temp) : "   ");
								else if (!temp)
									PrintSz("   ");
								else
								{
									if (r == 3)
									{
										if (grid->v[x][y] < 6000)
											sprintf(sz, "%c%2d", us.fAppSep ?
													(grid->v[x][y] <
													 0 ? 'a' : 's') : (grid->
																	   v[x][y]
																	   < 0 ? '-' : '+'), abs(grid->v[x][y]) / 60);
										else
											sprintf(sz, "%3d", abs(grid->v[x][y]) / 60);
									}
									else
										sprintf(sz, "%02d'", abs(grid->v[x][y]) % 60);
									PrintSz(sz);
								}

								/* Print midpoint rows. */

							}
							else if (x > y)
							{
								AnsiColor(kSignA(temp));
								if (r == 2)
								{
									temp = grid->n[x][y];
									sprintf(sz, "%c%c%c", chSig3(temp));
								}
								else if (r == 3)
								{
									sprintf(sz, "%2d%c", grid->v[x][y] / 60, chDeg0);
								}
								else
									sprintf(sz, "%02d'", grid->v[x][y] % 60);
								PrintSz(sz);

								/* Print the diagonal of object names. */

							}
							else
							{
								AnsiColor(kReverse);
								if (r == 2)
								{
									AnsiColor(kObjA[y]);
									sprintf(sz, "%c%c%c", chObj3(y));
								}
								else
								{
									temp = Z2Sign(cp0.longitude[y]);
									AnsiColor(kSignA(temp));
									if (r == 3)
										sprintf(sz, "%2d%c", (int) cp0.longitude[y] - (temp - 1) * 30, chDeg0);
									else
										sprintf(sz, "%c%c%c", chSig3(temp));
								}
								PrintSz(sz);
							}
							AnsiColor(kDefault);
						}
						else if (y1 > 0)
							PrintTab(chH, 3);
						x1++;
					}
				if (y + r > 1)
					PrintL();
				y1++;
			}
	if (y1 == 0)
		PrintSz("Empty aspect grid.\n");
}


/* This is a subprocedure of DisplayGrands(). Here we print out one aspect */
/* configuration found by the parent procedure.                            */

void PrintGrand(byte ac, int i1, int i2, int i3, int i4) {
	char sz[cchSzDef];
	int asp;

	switch (ac)
	{
	case acS:
		asp = aCon;
		break;
	case acGT:
		asp = aTri;
		break;
	case acTS:
		asp = aOpp;
		break;
	case acY:
		asp = aInc;
		break;
	case acGC:
		asp = aSqu;
		break;
	case acC:
		asp = aSex;
		break;
	case acMR:
		asp = aSex;
		break;
	case acK:
		asp = aTri;
		break;
	default:;
	}
	AnsiColor(kAspA[asp]);
	sprintf(sz, "%-11s", Lang(tAspectConfig, ac));
	PrintSz(sz);
	AnsiColor(kDefault);
	sprintf(sz, " %s ", ac == acS || ac == acGT || ac == acGC || ac == acMR || ac == acK ? "with" : "from");
	PrintSz(sz);
	AnsiColor(kObjA[i1]);
	sprintf(sz, "%c%c%c: ", chObj3(i1));
	PrintSz(sz);
	PrintZodiac(cp0.longitude[i1]);
	sprintf(sz, " %s ", ac == acS || ac == acGT || ac == acK ? "and" : "to ");
	PrintSz(sz);
	AnsiColor(kObjA[i2]);
	sprintf(sz, "%c%c%c: ", chObj3(i2));
	PrintSz(sz);
	PrintZodiac(cp0.longitude[i2]);
	sprintf(sz, " %s ", ac == acGC || ac == acC || ac == acMR ? "to " : "and");
	PrintSz(sz);
	AnsiColor(kObjA[i3]);
	sprintf(sz, "%c%c%c: ", chObj3(i3));
	PrintSz(sz);
	PrintZodiac(cp0.longitude[i3]);
	if (ac == acGC || ac == acC || ac == acMR || ac == acK)
	{
		PrintSz(" to ");
		AnsiColor(kObjA[i4]);
		sprintf(sz, "%c%c%c: ", chObj3(i4));
		PrintSz(sz);
		PrintZodiac(cp0.longitude[i4]);
	}
	PrintL();
}


/* Scan the aspect grid of a chart and print out any major configurations, */
/* as specified with the -g0 switch.                                       */

void DisplayGrands()
{
	int cac = 0, i, j, k, l;

	for (i = 0; i <= cObj; i++)
		if (!FIgnore(i))
			for (j = 0; j <= cObj; j++)
				if (j != i && !FIgnore(j))
					for (k = 0; k <= cObj; k++)
						if (k != i && k != j && !FIgnore(k))
						{

							/* Is there a Stellium among the current three planets? */

							if (i < j && j < k && grid->n[i][j] == aCon &&
								grid->n[i][k] == aCon && grid->n[j][k] == aCon)
							{
								cac++;
								PrintGrand(acS, i, j, k, l);

								/* Is there a Grand Trine? */

							}
							else if (i < j && j < k && grid->n[i][j] == aTri
									 && grid->n[i][k] == aTri && grid->n[j][k] == aTri)
							{
								cac++;
								PrintGrand(acGT, i, j, k, l);

								/* Grand Trine is there, may be there is a Kite too ? */
								/* We look, if some planet is in sextiles with some   */
								/* pair of planets of Grand Trine, then it's surely   */
								/* opposed with third one.                            */

								for (l = 0; l <= cObj; l++)
									if (!FIgnore(l))
									{
										if (grid->n[Min(i, l)][Max(i, l)] ==
											aSex && grid->n[Min(j, l)][Max(j, l)] == aSex)
										{
											cac++;
											PrintGrand(acK, i, j, k, l);
										}
										if (grid->n[Min(j, l)][Max(j, l)] ==
											aSex && grid->n[Min(k, l)][Max(k, l)] == aSex)
										{
											cac++;
											PrintGrand(acK, i, j, k, l);
										}
										if (grid->n[Min(i, l)][Max(i, l)] ==
											aSex && grid->n[Min(k, l)][Max(k, l)] == aSex)
										{
											cac++;
											PrintGrand(acK, i, j, k, l);
										}
									}

								/* Is there a T-Square? */

							}
							else if (j < k && grid->n[j][k] == aOpp &&
									 grid->n[Min(i, j)][Max(i, j)] == aSqu && grid->n[Min(i, k)][Max(i, k)] == aSqu)
							{
								cac++;
								PrintGrand(acTS, i, j, k, l);

								/* Is there a Yod? */

							}
							else if (j < k && grid->n[j][k] == aSex &&
									 grid->n[Min(i, j)][Max(i, j)] == aInc && grid->n[Min(i, k)][Max(i, k)] == aInc)
							{
								cac++;
								PrintGrand(acY, i, j, k, l);
							}
							for (l = 0; l <= cObj; l++)
								if (!FIgnore(l))
								{

									/* Is there a Grand Cross among the current four planets? */

									if (i < j && i < k && i < l && j < l
										&& grid->n[i][j] == aSqu
										&& grid->n[Min(j, k)][Max(j, k)] ==
										aSqu
										&& grid->n[Min(k, l)][Max(k, l)] ==
										aSqu && grid->n[i][l] == aSqu
										&& MinDistance(cp0.longitude[i],
													   cp0.longitude[k]) > 150.0 && MinDistance(cp0.longitude[j], cp0.longitude[l]) > 150.0)
									{
										cac++;
										PrintGrand(acGC, i, j, k, l);

										/* Is there a Cradle? */

									}
									else if (i < l
											 && grid->
											 n[Min(i, j)][Max(i, j)] == aSex
											 && grid->
											 n[Min(j, k)][Max(j, k)] == aSex
											 && grid->
											 n[Min(k, l)][Max(k, l)] == aSex
											 && MinDistance(cp0.longitude[i], cp0.longitude[l]) > 150.0)
									{
										cac++;
										PrintGrand(acC, i, j, k, l);

										/* Is there a Mystic Rectangle? */

									}
									else if (i < j && i < k && i < l &&
											 grid->n[Min(i, j)][Max(i, j)] ==
											 aOpp
											 && grid->
											 n[Min(k, l)][Max(k, l)] == aOpp
											 && grid->
											 n[Min(i, k)][Max(i, k)] == aTri && grid->n[Min(j, l)][Max(j, l)] == aTri)
									{
										cac++;
										PrintGrand(acMR, i, j, k, l);
									}
								}
						}
	if (!cac)
		PrintSz("No major configurations in aspect grid.\n");
}


/* This is subprocedure of ChartWheel(). Here we print out the location */
/* of a particular house cusp as well as what house cusp number it is.  */

void PrintHouse(int i, int left) {
	char sz[cchSzDef];
	double r;
	int j;

	if (us.fVedic)
	{
		j = Mod12(12 - i);
		r = Sign2Z(j);
	}
	else
	{
		j = i;
		r = cp0.cusp_pos[i];
	}
	if (!left)
		PrintZodiac(r);
	AnsiColor(kSignA(j));
	sprintf(sz, "<%d>", j);
	PrintSz(sz);
	if (left)
		PrintZodiac(r);
	else
		AnsiColor(kDefault);
}


/* Another subprocedure of ChartWheel(). Print out one of the chart info */
/* rows in the middle of the wheel (which may be blank) given an index.  */

void PrintWheelCenter(int irow) {
	char sz[cchSzDef], szT[16];
	int cch, nT;

	if (*ciMain.nam == chNull && *ciMain.loc == chNull)	/* Try to center */
		irow--;
	if (*ciMain.nam == chNull && irow >= 1)	/* Don't have blank lines if */
		irow++;					/* the name and/or location  */
	if (*ciMain.loc == chNull && irow >= 3)	/* strings are empty.        */
		irow++;
	switch (irow)
	{
	case 0:
		sprintf(sz, "%s v. %s chart", szAppName, szVersionCore);
		break;
	case 1:
		sprintf(sz, "%s", ciMain.nam);
		break;
	case 2:
		nT = DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea);
		sprintf(sz, "%c%c%c %s %s", chDay3(nT), 
			SzDate(ciMain.mon, ciMain.day, ciMain.yea, 2), SzTim(ciMain.tim));
		break;
	case 3:
		sprintf(sz, "%s", ciMain.loc);
		break;
	case 4:
		nT = (int) (RFract(fabs(ciMain.zon)) * 100.0 + 0.5);
		sprintf(sz, "%s %c%02d:%02d, %s", ChDst(ciMain.dst),
				ciMain.zon > 0.0 ? '-' : '+', (int) fabs(ciMain.zon), nT, 
				SzLocation(ciMain.lon, ciMain.lat));
		break;
	case 5:
		nT = us.fEuroTime;
		us.fEuroTime = TRUE;
		sprintf(szT, "%s", SzTim(DecDeg2DegMin(Rad2Deg(is.RA) * (24.0 / 360.0))));
		sprintf(sz, "UT: %s, Sid.T: %s", SzTim(ciMain.tim + ciMain.zon - ciMain.dst), szT);
		us.fEuroTime = nT;
		break;
	case 6:
		sprintf(sz, "%s Houses", Lang(tHouseSystem, us.nHouseSystem));
		break;
	case 7:
		sprintf(sz, "%s / %s", us.fSidereal ? Lang(sSIDEREAL) : Lang(sTROPICAL),
				us.objCenter == oSun ? Lang(sHELIOCENTRIC) :
				(us.objCenter == oEar ? Lang(sGEOCENTRIC) : Lang(tObjName, us.objCenter)));
		break;
	case 8:
		sprintf(sz, "%s %12.4f", Lang(sJULIAN_DAY), JulianDayFromTime(is.T));
		break;
	default:
		*sz = chNull;
	}
	cch = strlen(sz);
	nT = WHEELCOLS * 2 - 1 + is.fSeconds * 8;
	PrintTab(' ', (nT - cch) / 2);
	PrintSz(sz);
	PrintTab(' ', nT - cch - (nT - cch) / 2);
}


/* Yet another subprocedure of ChartWheel(). Here we print out one line */
/* in a particular house cell (which may be blank).                     */

void PrintWheelSlot(int obj) {
	char sz[cchSzDef];

	if (obj >= oEar)
	{
		AnsiColor(kObjA[obj]);
		sprintf(sz, " %c%c%c ", chObj3(obj));
		PrintSz(sz);
		PrintZodiac(cp0.longitude[obj]);
		sprintf(sz, "%c ", cp0.vel_longitude[obj] < 0.0 ? 'r' : ' ');
		PrintSz(sz);
		PrintTab(' ', WHEELCOLS - 15);
	}
	else						/* This particular line is blank. */
		PrintTab(' ', WHEELCOLS - 1 + is.fSeconds * 4);
}


/* Display all the objects in a wheel format on the screen, as specified */
/* with the -w switch. The wheel is divided into the 12 houses and the   */
/* planets are placed accordingly.                                       */

void ChartWheel()
{
	int wheel[NUMBER_OF_SIGNS][WHEELROWS], wheelcols, count = 0, i, j, k, l;

	/* If the seconds (-b0) flag is set, we'll print all planet and house    */
	/* locations to the nearest zodiac second instead of just to the minute. */

	wheelcols = WHEELCOLS + is.fSeconds * 4;

	for (i = 0; i < NUMBER_OF_SIGNS; i++)
		for (j = 0; j < us.nWheelRows; j++)
			wheel[i][j] = -1;	/* Clear out array. */

	/* This section of code places each object in the wheel house array. */

	for (i = 0; i <= cObj && count < us.nWheelRows * 12; i++)
	{
		if (FIgnore(i) || (FCusp(i) && MinDistance(cp0.longitude[i], cp0.cusp_pos[i - oAsc + 1]) < 0.5 / 60.0))
			continue;

		/* Try to put object in its proper house. If no room, */
		/* then overflow over to the next succeeding house.   */

		for (j = (us.fVedic ? Mod12(11 - Z2Sign(cp0.longitude[i])) : cp0.house_no[i]) - 1;
			 j < NUMBER_OF_SIGNS; j = j < NUMBER_OF_SIGNS ? (j + 1) % NUMBER_OF_SIGNS : j)
		{

			/* Now try to find the proper place in the house to put the object. */
			/* This is in sorted order, although a check is made for 0 Aries.   */

			if (wheel[j][us.nWheelRows - 1] >= 0)
				continue;
			l = cp0.cusp_pos[j + 1] > cp0.cusp_pos[Mod12(j + 2)];
			for (k = 0;
				wheel[j][k] >= 0 && (cp0.longitude[i] >= cp0.longitude[wheel[j][k]]
				|| (l && cp0.longitude[i] < 180.0 && cp0.longitude[wheel[j][k]] > 180.0))
				&& !(l && cp0.longitude[i] > 180.0 && cp0.longitude[wheel[j][k]] < 180.0); k++)
				;

			/* Actually insert object in proper place. */

			if (wheel[j][k] < 0)
				wheel[j][k] = i;
			else
			{
				for (l = us.nWheelRows - 1; l > k; l--)
					wheel[j][l] = wheel[j][l - 1];
				wheel[j][k] = i;
			}
			count++;
			j = NUMBER_OF_SIGNS;
		}
	}

	/* Now, if this is really the -w switch and not -w0, then reverse the  */
	/* order of objects in western houses for more intuitive reading. Also */
	/* reverse the order of everything in the reverse order Vedic wheels.  */

	if (us.fVedic)
		for (i = 0; i < NUMBER_OF_SIGNS; i++)
			for (j = 0; j < us.nWheelRows / 2; j++)
			{
				k = us.nWheelRows - 1 - j;
				l = wheel[i][j];
				wheel[i][j] = wheel[i][k];
				wheel[i][k] = l;
			}
	if (!us.fWheelReverse)
		for (i = 3; i < 9; i++)
			for (j = 0; j < us.nWheelRows / 2; j++)
			{
				k = us.nWheelRows - 1 - j;
				l = wheel[i][j];
				wheel[i][j] = wheel[i][k];
				wheel[i][k] = l;
			}

	/* Here we actually print the wheel and the objects in it. */

	PrintCh(chNW);
	PrintTab(chH, WHEELCOLS - 8);
	PrintHouse(11, TRUE);
	PrintTab(chH, WHEELCOLS - 11 + us.fVedic);
	PrintHouse(10, TRUE);
	PrintTab(chH, WHEELCOLS - 10 + us.fVedic);
	PrintHouse(9, TRUE);
	PrintTab(chH, wheelcols - 4);
	PrintCh(chNE);
	PrintL();
	for (i = 0; i < us.nWheelRows; i++)
	{
		for (j = 10; j >= 7; j--)
		{
			PrintCh(chV);
			PrintWheelSlot(wheel[j][i]);
		}
		PrintCh(chV);
		PrintL();
	}
	PrintHouse(12, TRUE);
	PrintTab(chH, WHEELCOLS - 11);
	PrintCh(chC);
	PrintTab(chH, wheelcols - 1);
	PrintCh(chJN);
	PrintTab(chH, wheelcols - 1);
	PrintCh(chC);
	PrintTab(chH, WHEELCOLS - 10);
	PrintHouse(8, FALSE);
	PrintL();
	for (i = 0; i < us.nWheelRows; i++)
	{
		PrintCh(chV);
		PrintWheelSlot(wheel[11][i]);
		PrintCh(chV);
		PrintWheelCenter(i);
		PrintCh(chV);
		PrintWheelSlot(wheel[6][i]);
		PrintCh(chV);
		PrintL();
	}
	PrintHouse(1, TRUE);
	PrintTab(chH, WHEELCOLS - 10 - us.fVedic);
	PrintCh(chJW);
	PrintWheelCenter(us.nWheelRows);
	PrintCh(chJE);
	PrintTab(chH, WHEELCOLS - 10);
	PrintHouse(7, FALSE);
	PrintL();
	for (i = 0; i < us.nWheelRows; i++)
	{
		PrintCh(chV);
		PrintWheelSlot(wheel[0][i]);
		PrintCh(chV);
		PrintWheelCenter(us.nWheelRows + 1 + i);
		PrintCh(chV);
		PrintWheelSlot(wheel[5][i]);
		PrintCh(chV);
		PrintL();
	}
	PrintHouse(2, TRUE);
	PrintTab(chH, WHEELCOLS - 10 - us.fVedic);
	PrintCh(chC);
	PrintTab(chH, wheelcols - 1);
	PrintCh(chJS);
	PrintTab(chH, wheelcols - 1);
	PrintCh(chC);
	PrintTab(chH, WHEELCOLS - 10);
	PrintHouse(6, FALSE);
	PrintL();
	for (i = 0; i < us.nWheelRows; i++)
	{
		for (j = 1; j <= 4; j++)
		{
			PrintCh(chV);
			PrintWheelSlot(wheel[j][i]);
		}
		PrintCh(chV);
		PrintL();
	}
	PrintCh(chSW);
	PrintTab(chH, wheelcols - 4);
	PrintHouse(3, FALSE);
	PrintTab(chH, WHEELCOLS - 10);
	PrintHouse(4, FALSE);
	PrintTab(chH, WHEELCOLS - 10);
	PrintHouse(5, FALSE);
	PrintTab(chH, WHEELCOLS - 7);
	PrintCh(chSE);
	PrintL();
}


/* This is a subprocedure of ChartAspect() and ChartAspectRelation().       */
/* Display summary information about the aspect list, i.e. the total number */
/* of aspects of each type, and the number of aspects to each object, as    */
/* done when the -a0 aspect summary setting is set.                         */

void PrintAspectSummary(int *ca, int *co, int count, double rPowSum) {
	char sz[cchSzDef];
	int i, j, k;

	if (count == 0)
	{
		PrintSz("No aspects in list.\n");
		return;
	}
	PrintL();
	sprintf(sz, "Total Power: %.2f - Average Power: %.2f\n", rPowSum, rPowSum / (double) count);
	PrintSz(sz);

	k = us.fParallel ? aOpp : cAspect;
	
	for (j = 0, i = 1; i <= k; i++)
	{
		if (!ignoreA[i])
		{
			if (!(j & 7))
			{
				if (j)
					PrintL();
			}
			else
				PrintSz("   ");
			AnsiColor(kAspA[i]);
			sprintf(sz, "%s:%3d", Lang(tAspectAbbrev, i), ca[i]);
			PrintSz(sz);
			j++;
		}
	}

	PrintL();
	for (j = 0, i = 0; i <= cObj; i++)
		if (!FIgnore(i))
		{
			if (!(j & 7))
			{
				if (j)
					PrintL();
			}
			else
				PrintSz("   ");
			AnsiColor(kObjA[i]);
			sprintf(sz, "%c%c%c:%3d", chObj3(i), co[i]);
			PrintSz(sz);
			j++;
		}
	PrintL();
}


/* Display all aspects between objects in the chart, one per line, in       */
/* sorted order based on the total "power" of the aspect, as specified with */
/* the -a switch. The same influences used for -I charts are used here.     */

void ChartAspect()
{
	int ca[cAspect + 1], co[NUMBER_OBJECTS];
	char sz[cchSzDef];
	int ihi, jhi, ahi, i, j, k, count = 0;
	double ip, jp, savepower, rPowSum = 0.0, p;
	int orb, saveorb;
	GridInfo tempgrid;


	memset(ca, 0, sizeof(ca));
	memset(co, 0, sizeof(co));
	memcpy(&tempgrid, grid, sizeof(GridInfo));

	PrintHeader();				/* Display chart info */
	PrintL();
	PrintTab(' ', 50);
	PrintSz(Lang(sAspectHdr));

	PlanetPPower();

	for (;;)
	{
		// we initialize now the two variables used for sorting, but only one 
		// of them will be used for sorting, depending on the sort type
		savepower = -1e10;
		saveorb = 0x7FFF;	

		// search for the next most powerful aspect or next smallest orb

		for (i = 0; i <= cObj; i++)
		{
			if (!FIgnore(i))
			{
				for (j = 0; j < i; j++)
				{
					if (!FIgnore(j))
					{
						if (k = tempgrid.n[j][i])
						{
							ip = i <= cLastMoving ? ppower1[i] : 2.5;
							jp = j <= cLastMoving ? ppower1[j] : 2.5;
							orb = tempgrid.v[j][i];
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

		tempgrid.n[jhi][ihi] = 0;	// mark this aspect as having being processed
		count++;					// Display the current aspect

#ifdef INTERPRET
		if (us.fInterpret)
		{						/* Interpret it if -I in effect. */
			InterpretAspect(jhi, ihi);
			continue;
		}
#endif
		rPowSum += savepower;
		ca[ahi]++;
		co[jhi]++;
		co[ihi]++;
		sprintf(sz, "%3d: ", count);
		PrintSz(sz);
		PrintAspect(jhi, Z2Sign(cp0.longitude[jhi]), (int) RSgn(cp0.vel_longitude[jhi]), ahi,
					ihi, Z2Sign(cp0.longitude[ihi]), (int) RSgn(cp0.vel_longitude[ihi]), 'a', 12, 12);
		AnsiColor(saveorb < 0 ? kWhite : kLtGray);
		sprintf(sz, " | %2d%c%02d' %c",
				abs(saveorb) / 60, chDeg1, abs(saveorb) % 60,
				us.fAppSep ? (saveorb < 0 ? 'a' : 's') : (saveorb < 0 ? '-' : '+')); 
		PrintSz(sz);
		AnsiColor(saveorb < 0 ? kWhite : kLtGray);
		sprintf(sz, " | %9.2f |\n", savepower);
		PrintSz(sz);
		AnsiColor(kDefault);
	}

	if (us.fAspSummary)
	{
		PrintAspectSummary(ca, co, count, rPowSum);
		PrintL2();
		DisplayGrands();
	}
}


/* This is a subprocedure of ChartMidpoint() and ChartMidpointRelation().  */
/* Display summary information about the midpoint list, i.e. the total     */
/* number of midpoints in each sign, and their average span in degrees, as */
/* done when the -m0 midpoint summary setting is set.                      */

void PrintMidpointSummary(int *cs, int count, long lSpanSum) {
	char sz[cchSzDef];
	int m, i;

	if (!us.fMidSummary)
		return;
	if (count == 0)
	{
		PrintSz("No midpoints in list.\n");
		return;
	}
	PrintL();
	m = (int) (lSpanSum / count);
	sprintf(sz, "Average span:%4d%c%02d'\n", m / 60, chDeg1, m % 60);
	PrintSz(sz);
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		if (i == sLib)
			PrintL();
		else if (i != sAri)
			PrintSz("   ");
		AnsiColor(kSignA(i));
		sprintf(sz, "%c%c%c:%3d", chSig3(i), cs[i]);
		PrintSz(sz);
	}
	PrintL();
}


/* Display locations of all midpoints between objects in the chart, */
/* one per line, in sorted zodiac order from zero Aries onward, as  */
/* specified with the -m switch.                                    */

void ChartMidpoint()
{
	int cs[NUMBER_OF_SIGNS + 1];
	char sz[cchSzDef];
	int mcut = -1, icut, jcut, mlo, ilo, jlo, m, i, j, k, count = 0;
	long lSpanSum = 0;
	double l, n, mid;

	memset(cs, 0, sizeof(cs));
	is.fSeconds = FALSE;

	for (;;)
	{
		mlo = 21600;

		/* Search for the next closest midpoint farther down in the zodiac. */

		for (i = 0; i < cObj; i++)
			if (!FIgnore(i))
				for (j = i + 1; j <= cObj; j++)
					if (!FIgnore(j))
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
			InterpretMidpoint(ilo, jlo);
			continue;
		}
#endif
		cs[mlo / 60 / 30 + 1]++;
		sprintf(sz, "%4d: ", count);
		PrintSz(sz);
		mid = (double) mlo / 60.0;
		PrintZodiac(mid);
		PrintCh(' ');
		PrintAspect(ilo, Z2Sign(cp0.longitude[ilo]), (int) RSgn(cp0.vel_longitude[ilo]), 0,
					jlo, Z2Sign(cp0.longitude[jlo]), (int) RSgn(cp0.vel_longitude[jlo]), 'm', 7, 10);
		AnsiColor(kDefault);
		m = (int) (MinDistance(cp0.longitude[ilo], cp0.longitude[jlo]) * 60.0);
		lSpanSum += m;
		sprintf(sz, "-%4d%c%02d' degree span.\n", m / 60, chDeg1, m % 60);
		PrintSz(sz);

		/* If the -ma switch is set, determine and display each aspect from one */
		/* of the planets to the current midpoint, and the aspect's orb.        */

		if (!us.fMidAspect)
			continue;
		for (i = 0; i < cObj; i++)
			if (!FIgnore(i))
			{
				l = MinDistance(cp0.longitude[i], mid);
				for (k = cAspect; k >= 1; k--)
				{
					if (!FAcceptAspect(i, k, ilo))
						continue;
					n = l - rAspAngle[k];
					if (fabs(n) < GetOrb(i, ilo, k))
					{
						if (us.fAppSep)
							n = RSgn2((cp0.vel_longitude[ilo] + cp0.vel_longitude[jlo]) / 2.0 - cp0.vel_longitude[i]) *
								RSgn2(MinDifference(cp0.longitude[i], mid)) * RSgn2(n) * fabs(n);
						PrintSz("      Midpoint ");
						PrintZodiac(mid);
						PrintSz(" makes ");
						AnsiColor(kAspA[k]);
						PrintSz(Lang(tAspectAbbrev, k));
						AnsiColor(kDefault);
						PrintSz(" to ");
						AnsiColor(kObjA[i]);
						sprintf(sz, "%.10s", Lang(tObjName, i));
						PrintSz(sz);
						PrintTab(' ', 10 - strlen(Lang(tObjName, i)));
						j = (int) (n * 60.0);
						AnsiColor(j < 0 ? kWhite : kLtGray);
						sprintf(sz, "- orb: %c%d%c%02d'",
								us.fAppSep ? (j < 0 ? 'a' : 's') : (j <
																	0 ? '-' : '+'), abs(j) / 60, chDeg1, abs(j) % 60);
						PrintSz(sz);
						PrintL();
						AnsiColor(kDefault);
					}
				}
			}
	}
	is.fSeconds = us.fSeconds;

	PrintMidpointSummary(cs, count, lSpanSum);
}


/* Display locations of the objects on the screen with respect to the local */
/* horizon, as specified with the -Z switch.                                */

void ChartHorizon()
{
	char sz[cchSzDef], szFormat[cchSzDef];
	double lon, lat, sx, sy, vx, vy, lonz[NUMBER_OBJECTS], latz[NUMBER_OBJECTS], azi[NUMBER_OBJECTS], alt[NUMBER_OBJECTS];
	int fPrime, i, j, k, tot;
	double lonMC;

	PrintHeader();
	PrintL();					/* Display chart info */

	/* Set up some initial variables. */

	fPrime = us.fPrimeVert;

	lon = Deg2Rad(Mod(DegMin2DecDeg(ciMain.lon)));
	lat = Deg2Rad(DegMin2DecDeg(ciMain.lat));

	tot = us.nStar ? cObj : cLastMoving;

	/* First find zenith location on Earth of each object. */

	for (i = 0; i <= tot; i++)
	{
		if (!ignore[i])
		{
			if (us.fEquator)
			{
				lonz[i] = Deg2Rad(cp0.longitude[i]);
				latz[i] = Deg2Rad(cp0.latitude[i]);
			}
			else
			{
				lonz[i] = Deg2Rad(Tropical(cp0.longitude[i]));
				latz[i] = Deg2Rad(cp0.latitude[i]);
				EclToEqu(&lonz[i], &latz[i]);
			}
		}
	}

	if (PolarMCflip && hRevers)
		lonMC = Mod(lonz[oMC] + rPi);
	else
		lonMC = lonz[oMC];

	/* Then, convert this to local horizon altitude and azimuth. */

	for (i = 1; i <= tot; i++)
		if (!ignore[i] /* && i != oMC */ )
		{
			lonz[i] = Deg2Rad(Mod(Rad2Deg(lonMC - lonz[i] + rPiHalf)));
			if (!fEquator)
				EquToLocal(&lonz[i], &latz[i], rPiHalf - lat);
			azi[i] = 360.0 - Rad2Deg(lonz[i]);
			alt[i] = Rad2Deg(latz[i]);
		}

	/* If the -Z0 switch flag is in effect, convert from altitude/azimuth  */
	/* coordinates to prime vertical coordinates that we'll print instead. */

	if (fPrime)
	{
		for (i = 1; i <= tot; i++)
			if (!ignore[i] && i != oMC)
			{
				azi[i] = Deg2Rad(azi[i]);
				alt[i] = Deg2Rad(alt[i]);
				CoorXform(&azi[i], &alt[i], rPiHalf);
				azi[i] = Rad2Deg(azi[i]);
				alt[i] = Rad2Deg(alt[i]);
			}
	}

	/* Now, actually print the location of each object. */

	sprintf(szFormat, is.fSeconds ? " " : "");
	if (fPrime)
	{
		sprintf(sz,
				Lang(sHorizonHdr),
				szFormat, szFormat, szFormat, szFormat, szFormat, szFormat, szFormat, szFormat);
		PrintSz(sz);
	}
	else
	{
		sprintf(sz,
				Lang(sHorizonHdr2),
				szFormat, szFormat, szFormat, szFormat, szFormat, szFormat, szFormat, szFormat);
		PrintSz(sz);
	}
	sprintf(sz, "%s Vector%s%s    Moon Vector\n", us.objCenter != oSun ? "Sun" : "Earth", szFormat, szFormat);
	PrintSz(sz);
	for (k = 1; k <= tot; k++)
	{
		i = k <= cLastMoving ? k : cLastMoving + starname[k - cLastMoving];
		if (ignore[i] /*  || !FThing(i) */ )
			continue;
		AnsiColor(kObjA[i]);
		sprintf(sz, "%-7.7s: ", Lang(tObjName, i));
		PrintSz(sz);
		PrintAltitude(alt[i]);

		/* Determine directional vector based on azimuth. */

		if (fNESW && !fPrime)
		{
			sprintf(sz, " %s", SzDegree(Mod(90.0 - azi[i])));
			PrintSz(sz);
		}
		else
		{
			sprintf(sz, " %s", SzDegree(azi[i]));
			PrintSz(sz);
		}
		sx = cos(Deg2Rad(azi[i]));
		sy = sin(Deg2Rad(azi[i]));
		if (fabs(sx) < fabs(sy))
		{
			vx = fabs(sx / sy);
			vy = 1.0;
		}
		else
		{
			vy = fabs(sy / sx);
			vx = 1.0;
		}
		sprintf(sz, is.fSeconds ? " (%.3f%c" : " (%.2f%c", vy, sy < 0.0 ? (fPrime ? 'u' : 's') : (fPrime ? 'd' : 'n'));
		PrintSz(sz);
		sprintf(sz, is.fSeconds ? " %.2f%c)" : " %.2f%c)", vx, sx > 0.0 ? 'e' : 'w');
		PrintSz(sz);

		/* Determine distance vector of current object from Sun and Moon. */

		vx = azi[1] - azi[i];
		vy = azi[2] - azi[i];
		j = 1 + is.fSeconds;
		sprintf(szFormat, " [%%%d.%df%%%d.%df] [%%%d.%df%%%d.%df]", j + 5, j, j + 5, j, j + 5, j, j + 5, j);
		sprintf(sz, szFormat,
				fabs(vx) < 180.0 ? vx : RSgn(vx) * (360.0 - fabs(vx)),
				alt[1] - alt[i], fabs(vy) < 180.0 ? vy : RSgn(vy) * (360.0 - fabs(vy)), alt[2] - alt[i]);
		PrintSz(sz);
		if (!is.fSeconds && i >= uranLo)
		{
			if (i <= uranHi)
				sprintf(sz, "  Uran. #%d", i - uranLo + 1);
			else
				sprintf(sz, "  Star #%2d", i - starLo + 1);
			PrintSz(sz);
		}
		PrintL();
	}
	AnsiColor(kDefault);
}


/* Display x,y,z locations of each body (in AU) with respect to the Sun */
/* (or whatever the specified center planet is), as in the -S switch.   */
/* These values were already determined when calculating the planet     */
/* positions themselves, so this procedure is basically just a loop.    */

void ChartOrbit()
{
	char sz[cchSzDef], szFormat[cchSzDef];
	double x, y, z;
	int i;

	sprintf(szFormat, is.fSeconds ? " " : "");
	sprintf(sz, "Body%s    Angle%s%s%s%s    ", szFormat, szFormat, szFormat, szFormat, szFormat);
	PrintSz(sz);
	sprintf(sz,
			"%sX axis%s%s%s    %sY axis%s%s%s    %sZ axis%s%s%s    %sLength\n",
			szFormat, szFormat, szFormat, szFormat, szFormat, szFormat,
			szFormat, szFormat, szFormat, szFormat, szFormat, szFormat, szFormat);
	PrintSz(sz);
	for (i = 0; i <= cLastMoving; i++)
	{
		if (ignore[i] || (!FThing(i)))
			continue;
		AnsiColor(kObjA[i]);
		sprintf(sz, "%c%c%c%c: ", chObj3(i), Lang(tObjShortName, i)[3] ? Lang(tObjShortName, i)[3] : ' ');
		PrintSz(sz);
		x = spacex[i];
		y = spacey[i];
		z = spacez[i];
		sprintf(sz,
				is.
				fSeconds ? "[%11.7f] [%11.7f] [%11.7f] [%11.7f] [%11.7f]" :
				"[%7.3f] [%7.3f] [%7.3f] [%7.3f] [%7.3f]", cp0.longitude[i], x, y, z, sqrt(x * x + y * y + z * z));
		PrintSz(sz);
		if (!is.fSeconds && i >= uranLo)
		{
			sprintf(sz, "  Uranian #%d", i - uranLo + 1);
			PrintSz(sz);
		}
		PrintL();
	}
	AnsiColor(kDefault);
}


/* Display locations of the planets on the screen with respect to the 36    */
/* Gauquelin sectors and their plus zones, as specified with the -l switch. */

void ChartSector()
{
	char sz[cchSzDef];
	CP cp;
	int c[cSector + 1], i, sec, pls, kPls, cpls = 0, co = 0, cq = 0;
	double r, rT;
	byte ignoreT[NUMBER_OBJECTS + 1];

	for (i = 1; i <= NUMBER_OBJECTS; i++)
		ignoreT[i] = ignore[i];

	for (i = 1; i <= cSector; i++)
	{
		c[i] = 0;
		cpls += pluszone[i];
	}
	cp = cp0;
	CastSectors();

	AnsiColor(kDkGray);
	PrintSz("Body  Sector ");
	if (is.fSeconds)
		PrintSz("  Plus      House   Sign Loc. Ret. Latitude Velocity Sec.18 Sec.12\n");
	else
		PrintSz("Plus    House   Sign Ret. Latit. Veloc. 18 12\n");
	for (i = 0; i <= cObj; i++)
	{
		if (FIgnore(i) || !FThing(i))
			continue;
		co++;
		AnsiColor(kObjA[i]);
		sprintf(sz, "%c%c%c%c: ", chObj3(i), Lang(tObjShortName, i)[3] ? Lang(tObjShortName, i)[3] : ' ');
		PrintSz(sz);
		r = (360.0 - cp0.longitude[i])/10.0;
		sec = (int) r + 1;
		c[sec]++;
		pls = pluszone[sec];
		cq += pls;
		kPls = (pls ? kRainbowA[1] : kMainA[5]);
		AnsiColor(kDkGray);
		PrintSz("Sec");
		AnsiColor(kPls);
		sprintf(sz, " %2d", sec);
		PrintSz(sz);
		if (is.fSeconds)
		{
			AnsiColor(kDkGray);
			sprintf(sz, "%.3f", RFract(r));
			PrintSz(&sz[1]);
			AnsiColor(kPls);
		}
		sprintf(sz, " %c", (byte) (pls ? '+' : '-'));
		PrintSz(sz);
		AnsiColor(kSignA(cp.house_no[i]));
		sprintf(sz, " [%2d%s house] ", cp.house_no[i], Lang(tSuffix, cp.house_no[i]));
		PrintSz(sz);
		PrintZodiac(cp.longitude[i]);
		sprintf(sz, " %c ", cp.vel_longitude[i] < 0.0 ? chRet : ' ');
		PrintSz(sz);
		PrintAltitude(cp.latitude[i]);
		PrintCh(' ');
		if ((FObject(i) || (i == oNoNode || i == oLil)))
		{
			PrintCh((byte) (cp0.vel_longitude[i] < 0.0 ? '-' : '+'));
			rT = Rad2Deg(fabs(cp0.vel_longitude[i]));
			sprintf(sz, is.fSeconds ? (rT < 10.0 ? "%7.5f" : "%7.4f") : (rT < 10.0 ? "%5.3f" : "%5.2f"), rT);
			PrintSz(sz);
		}
		else
			PrintTab('_', us.fSeconds ? 8 : 6);
		AnsiColor(kPls);
		sprintf(sz, " %2d", (sec - 1) / 2 + 1);
		PrintSz(sz);
		if (is.fSeconds)
		{
			AnsiColor(kDkGray);
			sprintf(sz, "%.3f", RFract(r / 2.0));
			PrintSz(&sz[1]);
		}
		AnsiColor(kPls);
		sprintf(sz, " %2d", (sec - 1) / 3 + 1);
		PrintSz(sz);
		if (is.fSeconds)
		{
			AnsiColor(kDkGray);
			sprintf(sz, "%.3f", RFract(r / 3.0));
			PrintSz(&sz[1]);
		}
		PrintL();
	}

	/* Display summary information, i.e. the planet in plus zone ratio. */

	AnsiColor(kDefault);
	sprintf(sz, "\nPlus zones: %d/%d = %.2f%% - ", cpls, cSector, (double) cpls / (double) 36 * 100.0);
	PrintSz(sz);
	sprintf(sz, "Planets in plus zones: %d/%d = %.2f%%\n", cq, co, co ? (double) cq / (double) co * 100.0 : 0.0);
	PrintSz(sz);

	/* Display more summary information, i.e. the number of planets in each */
	/* sector, as well as whether each sector is a plus zone or not.        */

	PrintSz("\nZone:");
	for (i = 1; i <= cSector / 2; i++)
	{
		pls = pluszone[i];
		AnsiColor(pls ? kRainbowA[1] : kMainA[5]);
		sprintf(sz, " %2d%c", i, pls ? '+' : '-');
		PrintSz(sz);
	}
	AnsiColor(kDefault);
	PrintSz("\nNum :");
	for (i = 1; i <= cSector / 2; i++)
	{
		if (c[i])
		{
			sprintf(sz, " %2d ", c[i]);
			PrintSz(sz);
		}
		else
			PrintSz("  . ");
	}
	PrintSz("\nZone:");
	for (i = cSector; i > cSector / 2; i--)
	{
		pls = pluszone[i];
		AnsiColor(pls ? kRainbowA[1] : kMainA[5]);
		sprintf(sz, " %2d%c", i, pls ? '+' : '-');
		PrintSz(sz);
	}
	AnsiColor(kDefault);
	PrintSz("\nNum :");
	for (i = cSector; i > cSector / 2; i--)
	{
		if (c[i])
		{
			sprintf(sz, " %2d ", c[i]);
			PrintSz(sz);
		}
		else
			PrintSz("  . ");
	}
	PrintL();
	for (i = 1; i <= NUMBER_OBJECTS; i++)
		ignore[i] = ignoreT[i];
	CastChart(TRUE);
}


/* Print the locations of the astro-graph lines on the Earth as specified */
/* with the -L switch. This includes Midheaven and Nadir lines, zenith    */
/* positions, and locations of Ascendant and Descendant lines.            */

void ChartAstroGraph()
{
	CrossInfo *c;
	char sz[cchSzDef];
	double planet1[NUMBER_OBJECTS], planet2[NUMBER_OBJECTS], mc[NUMBER_OBJECTS];
	double ic[NUMBER_OBJECTS], as[NUMBER_OBJECTS], ds[NUMBER_OBJECTS]; 
	double as1[NUMBER_OBJECTS], ds1[NUMBER_OBJECTS];
	double lo, longm, w, x, y, z, ad, oa, am, od, dm;
	int cCross = 0, tot = cObj, i, j, k, l, m, n;

	lo = DegMin2DecDeg(ciMain.lon);

	if (us.fLatitudeCross)
	{
		if ((c = (CrossInfo *) allocate(sizeof(CrossInfo), "crossing table")) == NULL)
			return;
	}

	for (i = 1; i <= cObj; i++)
		if (!ignore[i] || i == oMC)
		{
			planet1[i] = Deg2Rad(Tropical(i == oMC ? is.MC : cp0.longitude[i]));
			planet2[i] = Deg2Rad(cp0.latitude[i]);	/* Calculate zenith location on */
			EclToEqu(&planet1[i], &planet2[i]);	/* Earth of each object.        */
		}

	/* Print header. */

	PrintSz("Object :");
	for (j = 0, i = 1; i <= cObj; i++)
		if (!ignore[i] && FThing(i))
		{
			AnsiColor(kObjA[i]);
			sprintf(sz, " %c%c%c", chObj3(i));
			PrintSz(sz);
			j++;
		}
	AnsiColor(kDefault);
	PrintSz("\n------ :");
	for (i = 1; i <= tot; i++)
		if (!ignore[i] && FThing(i))
			PrintSz(" ###");

	/* Print the longitude locations of the Midheaven lines. */

	PrintSz("\nMidheav: ");
	if (lo < 0.0)
		lo += 360.0;
	for (i = 1; i <= tot; i++)
		if (!ignore[i] && FThing(i))
		{
			AnsiColor(kObjA[i]);
			x = planet1[oMC] - planet1[i];
			if (x < 0.0)
				x += 2.0 * rPi;
			if (x > rPi)
				x -= 2.0 * rPi;
			z = lo + Rad2Deg(x);
			if (z > 180.0)
				z -= 360.0;
			mc[i] = z;
			sprintf(sz, "%3.0f%c", fabs(z), z < 0.0 ? 'e' : 'w');
			PrintSz(sz);
		}
	AnsiColor(kDefault);

	/* The Nadir lines are just always 180 degrees away from the Midheaven. */

	PrintSz("\nNadir  : ");
	for (i = 1; i <= tot; i++)
		if (!ignore[i] && FThing(i))
		{
			AnsiColor(kObjA[i]);
			z = mc[i] + 180.0;
			if (z > 180.0)
				z -= 360.0;
			ic[i] = z;
			sprintf(sz, "%3.0f%c", fabs(z), z < 0.0 ? 'e' : 'w');
			PrintSz(sz);
		}
	AnsiColor(kDefault);

	/* Print the Zenith latitude locations. */

	PrintSz("\nZenith : ");
	for (i = 1; i <= tot; i++)
		if (!ignore[i] && FThing(i))
		{
			AnsiColor(kObjA[i]);
			y = Rad2Deg(planet2[i]);
			sprintf(sz, "%3.0f%c", fabs(y), y < 0.0 ? 's' : 'n');
			PrintSz(sz);
			as[i] = ds[i] = as1[i] = ds1[i] = rLarge;
		}
	PrintL2();

	/* Now print the locations of Ascendant and Descendant lines. Since these */
	/* are curvy, we loop through the latitudes, and for each object at each  */
	/* latitude, print the longitude location of the line in question.        */

	longm = Deg2Rad(Mod(Rad2Deg(planet1[oMC]) + lo));
	for (j = 80; j >= -80; j -= us.nAstroGraphStep)
	{
		AnsiColor(kDefault);
		sprintf(sz, "Asc@%2d%c: ", j >= 0 ? j : -j, j < 0 ? 's' : 'n');
		PrintSz(sz);
		for (i = 1; i <= tot; i++)
			if (!ignore[i] && FThing(i))
			{
				AnsiColor(kObjA[i]);
				ad = tan(planet2[i]) * tan(Deg2Rad(j));
				if (ad * ad > 1.0)
				{
					PrintSz(" -- ");
					as1[i] = ds1[i] = cp2.vel_longitude[i] = rLarge;
				}
				else
				{
					ad = asin(ad);
					oa = planet1[i] - ad;
					if (oa < 0.0)
						oa += 2.0 * rPi;
					am = oa - rPiHalf;
					if (am < 0.0)
						am += 2.0 * rPi;
					z = longm - am;
					if (z < 0.0)
						z += 2.0 * rPi;
					if (z > rPi)
						z -= 2.0 * rPi;
					as1[i] = as[i];
					as[i] = z = Rad2Deg(z);
					cp2.vel_longitude[i] = ad;
					sprintf(sz, "%3.0f%c", fabs(z), z < 0.0 ? 'e' : 'w');
					PrintSz(sz);
				}
			}

		/* Again, the Descendant position is related to the Ascendant's,  */
		/* being a mirror image, so it can be calculated somewhat easier. */

		AnsiColor(kDefault);
		sprintf(sz, "\nDsc@%2d%c: ", j >= 0 ? j : -j, j < 0 ? 's' : 'n');
		PrintSz(sz);
		for (i = 1; i <= tot; i++)
			if (!ignore[i] && FThing(i))
			{
				AnsiColor(kObjA[i]);
				ad = cp2.vel_longitude[i];
				if (ad == rLarge)
					PrintSz(" -- ");
				else
				{
					od = planet1[i] + ad;
					dm = od + rPiHalf;
					z = longm - dm;
					if (z < 0.0)
						z += 2.0 * rPi;
					if (z > rPi)
						z -= 2.0 * rPi;
					ds1[i] = ds[i];
					ds[i] = z = Rad2Deg(z);
					sprintf(sz, "%3.0f%c", fabs(z), z < 0.0 ? 'e' : 'w');
					PrintSz(sz);
				}
			}
		PrintL();

		/* Now, if the -L0 switch is in effect, then take these line positions, */
		/* which we saved in an array above as we were printing them, and       */
		/* calculate and print the latitude crossings.                          */

		if (us.fLatitudeCross)
			for (l = 1; l <= cObj; l++)
				if (!ignore[l] && FThing(l))
					for (k = 1; k <= cObj; k++)
					{
						if (ignore[k] || !FThing(k))
							continue;
						for (n = 0; n <= 1; n++)
						{
							x = n ? ds1[l] : as1[l];
							y = n ? ds[l] : as[l];
							for (m = 0; m <= 1; m++)
							{

								/* Check if Ascendant/Descendant cross Midheaven/Nadir. */

								z = m ? ic[k] : mc[k];
								if (cCross < MAXCROSS && fabs(x - y) < 180.0 && RSgn(z - x) != RSgn(z - y))
								{
									c->obj1[cCross] = n ? -l : l;
									c->obj2[cCross] = m ? -k : k;
									c->lat[cCross] = (double) j + 5.0 * fabs(z - y) / fabs(x - y);
									c->lon[cCross] = z;
									cCross++;
								}

								/* Check if Ascendant/Descendant cross another Asc/Des. */

								w = m ? ds1[k] : as1[k];
								z = m ? ds[k] : as[k];
								if (cCross < MAXCROSS && k > l &&
									fabs(x - y) + fabs(w - z) < 180.0 && RSgn(w - x) != RSgn(z - y))
								{
									c->obj1[cCross] = n ? -l : l;
									c->obj2[cCross] = 100 + (m ? -k : k);
									c->lat[cCross] = (double) j + 5.0 * fabs(y - z) / (fabs(x - w) + fabs(y - z));
									c->lon[cCross] =
										Min(x, y) + fabs(x - y) * fabs(y - z) / (fabs(x - w) + fabs(y - z));
									cCross++;
								}
							}
						}
					}
	}
	if (!us.fLatitudeCross)
		return;
	PrintL();

	/* Now, print out all the latitude crossings we found.  */
	/* First, we sort them in order of decreasing latitude. */

	for (i = 1; i < cCross; i++)
	{
		j = i - 1;
		while (j >= 0 && c->lat[j] < c->lat[j + 1])
		{
			SwapN(c->obj1[j], c->obj1[j + 1]);
			SwapN(c->obj2[j], c->obj2[j + 1]);
			SwapR(c->lat[j], c->lat[j + 1]);
			SwapR(c->lon[j], c->lon[j + 1]);
			j--;
		}
	}
	for (i = 1; i < cCross; i++)
	{
		j = abs(c->obj1[i]);
		AnsiColor(kObjA[j]);
		sprintf(sz, "%c%c%c ", chObj3(j));
		PrintSz(sz);
		AnsiColor(kElemA[c->obj1[i] > 0 ? eFir : eAir]);
		PrintSz(c->obj1[i] > 0 ? "Ascendant " : "Descendant");
		AnsiColor(kWhite);
		PrintSz(" crosses ");
		j = abs(c->obj2[i] - (c->obj2[i] < 50 ? 0 : 100));
		AnsiColor(kObjA[j]);
		sprintf(sz, "%c%c%c ", chObj3(j));
		PrintSz(sz);
		AnsiColor(kElemA[c->obj2[i] < 50 ? (c->obj2[i] > 0 ? eEar : eWat) : (c->obj2[i] > 100 ? eFir : eAir)]);
		sprintf(sz, "%s ",
				c->obj2[i] < 50 ? (c->obj2[i] >
								   0 ? "Midheaven " : "Nadir     ") : (c->obj2[i] > 100 ? "Ascendant " : "Descendant"));
		PrintSz(sz);
		AnsiColor(kDefault);
		sprintf(sz, "at %s%c,", SzDegree(c->lon[i]), c->lon[i] < 0.0 ? 'E' : 'W');
		PrintSz(sz);
		j = (int) (RFract(fabs(c->lat[i])) * 60.0);
		sprintf(sz, "%s%c\n", SzDegree(c->lat[i]), c->lat[i] < 0.0 ? 'S' : 'N');
		PrintSz(sz);
	}
	free(c);
	if (!cCross)
	{
		AnsiColor(kDefault);
		PrintSz("No latitude crossings.\n");
	}
}


/* Another important procedure: Display any of the types of (text) charts    */
/* that the user specified they wanted, by calling the appropriate routines. */

void PrintChart(BOOL fProg) {
	int fCall = FALSE;

	if (us.fListing)
	{
		if (is.fMult)
			PrintL2();
		if (us.nRel < rcDifference)
			ChartListing();
		else if (us.nRel == rcDifference)
		{
			DisplayTimeDifference();
		}
		else
		{
			DisplayBiorhythm();
		}
		is.fMult = TRUE;
	}
	if (us.fWheel)
	{
		if (is.fMult)
			PrintL2();
		ChartWheel();
		is.fMult = TRUE;
	}
	if (us.fGrid)
	{
		if (is.fMult)
			PrintL2();
		if (us.nRel > rcDual)
		{
			fCall = us.fSmartAspects;
			us.fSmartAspects = FALSE;
			if (!FCreateGrid(FALSE))
				return;
			us.fSmartAspects = fCall;
			not(fCall);
			ChartGrid();
			if (us.fGridConfig)
			{					/* If -g0 switch in effect, then  */
				PrintL();		/* display aspect configurations. */
				if (!fCall)
					FCreateGrid(FALSE);
				DisplayGrands();
			}
		}
		else
		{

			/* Do a relationship aspect grid between two charts if -r0 in effect. */

			fCall = us.fSmartAspects;
			us.fSmartAspects = FALSE;
			if (!FCreateGridRelation(us.fGridConfig))
				return;
			us.fSmartAspects = fCall;
			ChartGridRelation();
		}
		is.fMult = TRUE;
	}
	if (us.fAspList)
	{
		if (is.fMult)
			PrintL2();
		if (us.nRel > rcDual)
		{
			if (!fCall)
			{
				fCall = TRUE;
				if (!FCreateGrid(FALSE))
					return;
			}
			ChartAspect();

		}
		else
		{

			if (!FCreateGridRelation(FALSE))
				return;
			ChartAspectRelation();
		}
		is.fMult = TRUE;
	}
	if (us.fMidpoint)
	{
		if (is.fMult)
			PrintL2();
		if (us.nRel > rcDual)
		{
			if (!fCall)
			{
				if (!FCreateGrid(FALSE))
					return;
			}
			ChartMidpoint();
		}
		else
		{
			if (!FCreateGridRelation(TRUE))
				return;
			ChartMidpointRelation();
		}
		is.fMult = TRUE;
	}
	if (us.fHorizon)
	{
		if (is.fMult)
			PrintL2();
		if (us.fHorizonSearch)
			ChartInDayHorizon();
		else
			ChartHorizon();
		is.fMult = TRUE;
	}
	if (us.fOrbit)
	{
		if (is.fMult)
			PrintL2();
		ChartOrbit();
		is.fMult = TRUE;
	}
	if (us.fSector)
	{
		if (is.fMult)
			PrintL2();
		else
			PrintHeader();		/* Print chart header if it hasn't been done yet. */
		ChartSector();
		is.fMult = TRUE;
	}
	if (us.fInfluence)
	{
		if (is.fMult)
			PrintL2();
		ChartInfluence();
		is.fMult = TRUE;
	}
	if (us.fAstroGraph)
	{
		if (is.fMult)
			PrintL2();
		ChartAstroGraph();
		is.fMult = TRUE;
	}
	if (us.fCalendar)
	{
		if (is.fMult)
			PrintL2();
		if (us.fCalendarYear)
			ChartCalendarYear();
		else
			ChartCalendarMonth();
		is.fMult = TRUE;
	}
	if (us.fInDay)
	{
		if (is.fMult)
			PrintL2();
		ChartInDaySearch(fProg);
		is.fMult = TRUE;
	}
	if (us.fInDayInf)
	{
		if (is.fMult)
			PrintL2();
		ChartInDayInfluence();
		is.fMult = TRUE;
	}
	if (us.fEphemeris)
	{
		if (is.fMult)
			PrintL2();
		ChartEphemeris();
		is.fMult = TRUE;
	}
	if (us.fTransit)
	{
		if (is.fMult)
			PrintL2();
		ChartTransitSearch(fProg);
		is.fMult = TRUE;
	}
	if (us.fTransitInf)
	{
		if (is.fMult)
			PrintL2();
		ChartTransitInfluence(fProg);
		is.fMult = TRUE;
	}
#ifdef ARABIC
	if (us.nArabic)
	{
		if (is.fMult)
			PrintL2();
		DisplayArabic();
		is.fMult = TRUE;
	}
#endif

	if (!is.fMult)
	{							/* Assume the -v chart if user */
		us.fListing = TRUE;	/* didn't indicate anything.   */
		PrintChart(fProg);
		is.fMult = TRUE;
	}
}

/* charts1.c */
