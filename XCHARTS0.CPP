/*
** Astrolog File: xcharts0.c
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
** Subchart Graphics Routines.
******************************************************************************
*/

/* Given a string, draw it on the screen using the given color. The       */
/* position of the text is based the saved positions of where we drew the */
/* text the last time the routine was called, being either directly below */
/* in the same column or in the same row just to the right. This is used  */
/* by the sidebar drawing routine to print a list of text on the chart.   */

int DrawPrint(char *sz, int m, int n) {
	static int x0, x, y;

	if (sz == NULL)
	{							/* Null string means just initialize position. */
		x0 = x = m;
		y = n;
		return y;
	}

	if (y >= gs.yWin - 1)		/* Don't draw if we've scrolled off the chart bottom. */
		return y;
	DrawColor(m);
	DrawSz(sz, x, y, dtLeft | dtBottom);

	/* If the second parameter is TRUE, we stay on the same line, otherwise */
	/* when FALSE we go to the next line at the original column setting.    */

	if (n)
		x += strlen(sz) * xFont * gi.nScaleT;
	else
	{
		x = x0;
		n = y;
		y += yFont * gi.nScaleT;
	}
	return y;
}

static void decorate_corners(int corner_type, int corner_size, int number_lines)
{
	int tot, abo, lef;
	int i, y, a, s;


	switch(corner_type)
	{
	case 1: // draw a moire pattern in each corner

		abo = gs.yWin * corner_size / 200;
		lef = gs.xWin * corner_size / 200;

		for (y = 0; y <= 1; y++)
		{
			for (i = 0; i <= 1; i++)
			{
				for (s = 0; s <= 1; s++)
				{
					for (a = 1; a < (s ? lef : abo) * 2; a++)
					{
						DrawColor(a & 1 ? kCornerDecoration : gi.kiOff);
						DrawLine(i ? gs.xWin - 1 - lef : lef,
								 y ? gs.yWin - 1 - abo : abo,
								 s ? (i ? gs.xWin - 1 -
									  a : a) : i * (gs.xWin - 1),
								 s ? y * (gs.yWin - 1) : (y ? gs.yWin - 1 - a : a));
					}
				}
			}
		}
		break;

	case 2: // draw spider web lines in each corner

		DrawColor(kCornerDecoration);
		tot = number_lines + 1;
		abo = gs.yWin * corner_size / 100;
		lef = gs.xWin * corner_size / 100;

		for (y = 0; y <= 1; y++)
		{
			for (i = 0; i <= 1; i++)
			{
				for (a = 1; a < tot; a++)
				{
					DrawLine(i * (gs.xWin - 1), y ? (gs.yWin - 1 - a * abo / tot) : 
							a * abo / tot,
						i ? gs.xWin - 1 - lef + a * lef / tot : 
							lef - a * lef / tot, y * (gs.yWin - 1));
				}
			}
		}
		break;

	default:
		break;
	}
}
	

/* Print text showing the chart information and house and planet positions */
/* of a chart in a "sidebar" to the right of the chart in question. This   */
/* is always done for the -v and -w graphic wheel charts unless the -v0    */
/* switch flag is also set, in which case none of the stuff here is done.  */

void DrawInfo()
{
	char sz[cchSzDef];
	ET et;
	CI ciT;
	int i, y, a, s;
	double r, deg, min, se;
	BOOL print_flag = TRUE;
	char ch = (char)chDeg3, nT;


	if(CornerDecorationType)
	{
		decorate_corners(CornerDecorationType, CornerDecorationSize,
			CornerDecorationNumberLines);
	}

	if (!gs.fText || us.fVelocity)
	{
		/* Don't draw sidebar if -v0 flag is set */
		return;					
	}

	a = us.fAnsiChar;
	us.fAnsiChar = (!gs.fFont || (!gs.fMeta && !gs.fPS)) << 1;
	DrawColor(gi.kiLite);
	if (gs.fBorder)
		DrawLine(gs.xWin - 1, 0, gs.xWin - 1, gs.yWin - 1);
	gs.xWin += (SIDESIZE*gi.nScaleT);
	DrawPrint(NULL, gs.xWin - (SIDESIZE*gi.nScaleT) + (xFont*gi.nScaleT) - gi.nScaleT, yFont * 7 / 5 * gi.nScaleT);

	/* Print chart header and setting information. */

	sprintf(sz, "%s v. %s", szAppName, szVersionCore);
	DrawPrint(sz, gi.kiOn, FALSE);

	if (gs.fBonusMode)
	{
		sprintf(sz, Lang(sBONUS_MODE));
		DrawPrint(sz, gi.kiLite, FALSE);
	}
	if (us.nRel < 0)
	{
		if ((us.nRel != rcTriWheel) && (us.nRel != rcQuadWheel))
			sprintf(sz, Lang(sINNER_RING));
		else
			sprintf(sz, Lang(sOUTER_RING));
		DrawPrint(sz, gi.kiLite, FALSE);
	}
	if (us.fProgressUS && !us.nRel)
	{
		ciT = ciMain;
		ciMain = ciCore;
	}
	if (*ciMain.nam && (us.nRel >= rcDifference || us.nRel <= 0))
		DrawPrint(ciMain.nam, gi.kiLite, FALSE);

	if (ciMain.mon == -1 || us.nRel == rcMidpoint || us.nRel == rcComposite || us.nRel == rcSynastry)
	{
		if (ciMain.mon == -1)
		{
			sprintf(sz, Lang(sNO_TIME_OR_SPACE));
			print_flag = FALSE;
		}
		else if (us.nRel == rcMidpoint)
		{
			sprintf(sz, Lang(sTIME_SPACE_MIDPOINT));
		}
		else if (us.nRel == rcComposite)
		{
			sprintf(sz, Lang(sCOMPOSITE_CHART));
			print_flag = FALSE;
		}
		else if (us.nRel == rcSynastry)
		{
			sprintf(sz, Lang(sSYNASTRY_HOUSES));
		}
		DrawPrint(sz, gi.kiLite, FALSE);
	}

	if (ciMain.mon != -1)
	{
		sprintf(sz, "%c%c%c, %s", chDay3(DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea)), 
			SzDate(ciMain.mon, ciMain.day, ciMain.yea, 1));
		DrawPrint(sz, gi.kiLite, FALSE);
		DrawPrint(SzTim(ciMain.tim), gi.kiLite, TRUE);
		sprintf(sz, " (%s %s GMT)", ChDst(ciMain.dst), SzZone(ciMain.zon));
		DrawPrint(sz, gi.kiLite, FALSE);
	}
	if (*ciMain.loc && us.nRel != rcMidpoint)
		DrawPrint(ciMain.loc, gi.kiLite, FALSE);

	if (ciMain.mon != -1)
		DrawPrint(SzLocation(ciMain.lon, ciMain.lat), gi.kiLite, FALSE);

	if (us.fProgressUS && !us.nRel)
		ciMain = ciT;

	if (us.nRel < 0 || us.nRel == rcSynastry || us.nRel == rcComposite || us.fProgressUS)
	{
		if (us.nRel < 0 || us.nRel == rcComposite || us.nRel == rcSynastry)
		{
			ciT = ciMain;
			ciMain = ciTwin;
		}
		if (us.nRel == rcDual)
		{
			sprintf(sz, Lang(sOUTER_RING));
			DrawPrint(sz, gi.kiLite, FALSE);
			if (*ciMain.nam)
				DrawPrint(ciMain.nam, gi.kiLite, FALSE);
		}
		if (us.nRel == rcTransit)
		{
			sprintf(sz, Lang(sOUTER_RING_TRANSIT));
			DrawPrint(sz, gi.kiLite, FALSE);
		}
		if (us.nRel == rcProgress)
		{
			if (us.fSolarArc)
			{
				if (us.fSolarArc == 1)
				{
					sprintf(sz, Lang(sDEGREE_PER_YEAR_MONTH));
					DrawPrint(sz, gi.kiLite, FALSE);
				}
				if (us.fSolarArc == 2)
				{
					sprintf(sz, Lang(sSOLAR_ARC));
					DrawPrint(sz, gi.kiLite, FALSE);
				}
				sprintf(sz, Lang(sOUTER_RING_DIRECTED_TO));
			}
			else
			{
				sprintf(sz, Lang(sOUTER_RING_PROGRESSED_TO));
			}
			DrawPrint(sz, gi.kiLite, FALSE);
		}
		if (us.nRel == rcTriWheel || us.nRel == rcQuadWheel)
		{
			sprintf(sz, Lang(sSECOND_RING));
			DrawPrint(sz, gi.kiLite, FALSE);
		}
		if (us.fProgressUS && !us.nRel)
		{
			if (us.fSolarArc)
			{
				if (us.fSolarArc == 1)
				{
					sprintf(sz, Lang(sDEGREE_PER_YEAR_MONTH));
					DrawPrint(sz, gi.kiLite, FALSE);
				}
				if (us.fSolarArc == 2)
				{
					sprintf(sz, Lang(sSOLAR_ARC));
					DrawPrint(sz, gi.kiLite, FALSE);
				}
				sprintf(sz, Lang(sCHART_DIRECTED_TO));
			}
			else
			{
				sprintf(sz, Lang(sCHART_PROGRESSED_TO));
			}
			DrawPrint(sz, gi.kiLite, FALSE);
		}
		if (us.nRel == rcSynastry)
		{
			sprintf(sz, Lang(sPLANETS));
			DrawPrint(sz, gi.kiLite, FALSE);
		}
		sprintf(sz, "%c%c%c, %s", chDay3(DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea)), 
			SzDate(ciMain.mon, ciMain.day, ciMain.yea, 1));
		DrawPrint(sz, gi.kiLite, FALSE);
		DrawPrint(SzTim(ciMain.tim), gi.kiLite, TRUE);
		sprintf(sz, " (%s %s GMT)", ChDst(ciMain.dst), SzZone(ciMain.zon));
		DrawPrint(sz, gi.kiLite, FALSE);
		if (us.nRel != rcProgress)
		{
			if (*ciMain.loc)
				DrawPrint(ciMain.loc, gi.kiLite, FALSE);
			DrawPrint(SzLocation(ciMain.lon, ciMain.lat), gi.kiLite, FALSE);
		}
		if (us.nRel < 0 || us.nRel == rcComposite || us.nRel == rcSynastry)
			ciMain = ciT;
	}

	sprintf(sz, Lang(sHOUSES1), Lang(tHouseSystem, us.nHouseSystem));
	DrawPrint(sz, gi.kiLite, FALSE);
	sprintf(sz, "%s, %s.", us.fSidereal ? Lang(sSIDEREAL) : Lang(sTROPICAL),
		us.objCenter == oSun ? Lang(sHELIOCENTRIC) : 
		(us.objCenter == oEar ? Lang(sGEOCENTRIC) : Lang(tObjName, us.objCenter)));
	DrawPrint(sz, gi.kiLite, FALSE);

	if (us.fSidereal)
	{
		sprintf(sz, "Mode: %s", Lang(tAyanamshaAbbrev, us.nSiderealMode));
		DrawPrint(sz, gi.kiLite, FALSE);
		deg = fabs(is.rSid);
		min = RFract(deg) * 60.0;
		se = RFract(min) * 60.0;
		// display ayanamsha with inverted sign, as positive values are commonly used for the present time
		sprintf(sz, "Ayanamsha: %s%d%c%02d'%06.3f\"", (is.rSid > 0.0 ? "-" : ""), (int) deg, ch, (int) min, se);
		DrawPrint(sz, gi.kiLite, FALSE);
	}

	if (print_flag)
	{
		if ((us.fProgressUS || us.nRel == rcProgress) && us.fSolarArc)
		{
			sprintf(sz, "%s %11.4f", Lang(sJULIAN_DAY), is.JDp - 0.5);
		}
		else
		{
			sprintf(sz, "%s %11.4f", Lang(sJULIAN_DAY), JulianDayFromTime(is.T));
		}
		DrawPrint(sz, gi.kiLite, FALSE);

		deg = Rad2Deg(is.rObliquity);
		min = RFract(deg) * 60.0;
		se = RFract(min) * 60.0;
		sprintf(sz, "%s: %2d%c%02d'%07.4f\"", Lang(sOBLIQUITY), (int) deg, ch, (int) min, se);
		DrawPrint(sz, gi.kiLite, FALSE);

		nT = us.fEuroTime;
		us.fEuroTime = TRUE;
		sprintf(sz, Lang(sSIDEREAL_TIME), SzTim(DecDeg2DegMin(Rad2Deg(is.RA) * (24.0 / 360.0))));
		DrawPrint(sz, gi.kiLite, FALSE);
		us.fEuroTime = nT;
	}

	if (print_flag)
	{
		sprintf(sz, "DeltaT: %8.4f", cp0.deltaT * 86400.0);
		DrawPrint(sz, gi.kiLite, FALSE);
	}

#ifdef _DEBUG88
		sprintf(sz, "Rate %d Factor %d Flag %d", gs.nAnimationJumpRate, wi.nAnimationJumpFactor, wi.fAnimate);
		DrawPrint(sz, gi.kiLite, FALSE);
#endif

	if (us.nHarmonic != 1.0)
	{
		sprintf(sz,  "%s = %f", Lang(sHARMONIC), us.nHarmonic);
		DrawPrint(sz, gi.kiLite, FALSE);
	}

	if (InfoFlag && !us.fSector)
	{
		DrawPrint("", gi.kiLite, FALSE);
		sprintf(sz, Lang(sHARM));
		DrawPrint(sz, kAspB[4], TRUE);
		sprintf(sz, "%+4.1f ", SHarmon);
		DrawPrint(sz, gi.kiLite, TRUE);
		sprintf(sz, Lang(sCONJ));
		DrawPrint(sz, kAspB[1], TRUE);
		sprintf(sz, "%4.1f ", SConjunct);
		DrawPrint(sz, gi.kiLite, FALSE);
		sprintf(sz, Lang(sTENS));
		DrawPrint(sz, kAspB[3], TRUE);
		sprintf(sz, "%4.1f ", STension);
		DrawPrint(sz, gi.kiLite, TRUE);
		sprintf(sz, Lang(sTOTAL));
		DrawPrint(sz, kAspB[2], TRUE);
		sprintf(sz, "%4.1f ", SSumma);
		DrawPrint(sz, gi.kiLite, FALSE);
		sprintf(sz, Lang(sSUMM));
		DrawPrint(sz, gi.kiLite, TRUE);
		sprintf(sz, "%+4.1f ", SChart);
		if (SChart > 75.0)
			DrawPrint(sz, kAspB[4], FALSE);
		else if (SChart > 25.0)
			DrawPrint(sz, kAspB[5], FALSE);
		else if (SChart > -25.0)
			DrawPrint(sz, kAspB[1], FALSE);
		else if (SChart > -75.0)
			DrawPrint(sz, kAspB[6], FALSE);
		else
			DrawPrint(sz, kAspB[3], FALSE);
	}

	/* Print house cusp positions. */

	DrawPrint("", gi.kiLite, FALSE);
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		sprintf(sz, "%2d%s %s: ", i, Lang(tSuffix, i), Lang(sHOUSE1));
		y = DrawPrint(sz, kSignB(i), TRUE);
		if (!is.fSeconds && (gs.nScale == 100 || !gs.fFont || !gi.fFile || gs.fBitmap) && y < gs.yWin - 1)
		{
			s = gi.nScale;
			gi.nScale = gi.nScaleT;
			DrawSign(Z2Sign(cp0.cusp_pos[i]), gs.xWin - 12 * gi.nScaleT, y - (yFont / 2 - 1) * gi.nScaleT);
			gi.nScale = s;
		}
		DrawPrint(SzZodiac(cp0.cusp_pos[i]), kSignB(Z2Sign(cp0.cusp_pos[i])), FALSE);
	}

	/* Print planet positions. */

	DrawPrint("", gi.kiLite, FALSE);
	for (i = 0; i <= cLastMoving; i++)
	{
		if (FProper2(i) && (!FCusp(i) || (i == oAsc) || (i == oMC)))
		{
			sprintf(sz, is.fSeconds ? "%3.3s: " : "%4.4s: ", Lang(tObjShortName, i));
			DrawPrint(sz, kObjB[i], TRUE);
			y = DrawPrint(SzZodiac(cp0.longitude[i]), kSignB(Z2Sign(cp0.longitude[i])), TRUE);
			if (!is.fSeconds && i < starLo && gi.nCurrChart != gSector
				&& (gs.nScale == 100 || !gs.fFont || !gi.fFile || gs.fBitmap) && y < gs.yWin - 1)
			{
				s = gi.nScale;
				gi.nScale = gi.nScaleT;
				DrawObject(i, gs.xWin - 12 * gi.nScaleT, y - (yFont / 2 - 1) * gi.nScaleT);
				gi.nScale = s;
			}
			sprintf(sz, "%c ", cp0.vel_longitude[i] < 0.0 ? chRet : ' ');
			DrawPrint(sz, gi.kiOn, TRUE);
			if (gi.nCurrChart != gSector || !is.fSeconds)
			{
				if (FThing(i))
				{
					is.fSeconds = FALSE;
					DrawPrint(SzAltitude(cp0.latitude[i]), gi.kiLite, TRUE);
					is.fSeconds = us.fSeconds;
				}
				else
					DrawPrint("       ", gi.kiOn, TRUE);
			}
			if (gi.nCurrChart == gSector)
			{
				r = ((360.0 - cp2.longitude[i]) / 10.0);
				s = (int) r + 1;
				if (!is.fSeconds)
					sprintf(sz, " %2d", s);
				else
					sprintf(sz, "%6.3f%c", r + 1.0, pluszone[s] ? '+' : '-');
				DrawPrint(sz, pluszone[s] ? kRainbowB[1] : kMainB[5], FALSE);
			}
			else
				DrawPrint("", gi.kiOn, FALSE);
		}
	}

	/* Print star positions. */

	for (i = starLo; i <= starHi; i++)
	{
		if (FProper2(i))
		{
			s = cLastMoving + starname[i - cLastMoving];
			sprintf(sz, is.fSeconds ? "%3.3s: " : "%4.4s: ", Lang(tObjShortName, s));
			DrawPrint(sz, kObjB[s], TRUE);
			DrawPrint(SzZodiac(cp0.longitude[s]), kSignB(Z2Sign(cp0.longitude[s])), TRUE);
			DrawPrint("  ", gi.kiOn, TRUE);
			if (gi.nCurrChart != gSector || !is.fSeconds)
				DrawPrint(SzAltitude(cp0.latitude[s]), gi.kiLite, TRUE);
			if (gi.nCurrChart == gSector)
			{
				r = (360.0 - cp0.longitude[s])/10.0;
				s = (int) r + 1;
				if (!is.fSeconds)
					sprintf(sz, " %2d", s);
				else
					sprintf(sz, "%6.3f%c", r + 1.0, pluszone[s] ? '+' : '-');
				DrawPrint(sz, pluszone[s] ? kRainbowB[1] : kMainB[5], FALSE);
			}
			else
				DrawPrint("", gi.kiOn, FALSE);
		}
	}

	// temporary feature, print in info bar planet positions for second chart

	if(fShowSecondChartInfoBar && (us.nRel == rcTransit || us.nRel == rcQuadWheel || us.nRel == rcTriWheel ||
		us.nRel == rcDual || us.nRel == rcSynastry))
	{
#if 0 
		/* Print house cusp positions. */

		DrawPrint("", gi.kiLite, FALSE);
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		{
			sprintf(sz, "%2d%s " Lang(sHOUSE1) ": ", i, Lang(tSuffix, i));
			y = DrawPrint(sz, kSignB(i), TRUE);
			if (!is.fSeconds && (gs.nScale == 100 || !gs.fFont || !gi.fFile || gs.fBitmap) && y < gs.yWin - 1)
			{
				s = gi.nScale;
				gi.nScale = gi.nScaleT;
				DrawSign(Z2Sign(cp1.cusp_pos[i]), gs.xWin - 12 * gi.nScaleT, y - (yFont / 2 - 1) * gi.nScaleT);
				gi.nScale = s;
			}
			DrawPrint(SzZodiac(cp1.cusp_pos[i]), kSignB(Z2Sign(cp1.cusp_pos[i])), FALSE);
		}
#endif

		/* Print planet positions. */

		DrawPrint("", gi.kiLite, FALSE);

		for (i = 0; i <= cLastMoving; i++)
		{
			if (FProper2(i) && (!FCusp(i) || (i == oAsc) || (i == oMC)))
			{
				sprintf(sz, is.fSeconds ? "%3.3s: " : "%4.4s: ", Lang(tObjShortName, i));
				DrawPrint(sz, kObjB[i], TRUE);
				y = DrawPrint(SzZodiac(cp1.longitude[i]), kSignB(Z2Sign(cp1.longitude[i])), TRUE);
				if (!is.fSeconds && i < starLo && gi.nCurrChart != gSector
					&& (gs.nScale == 100 || !gs.fFont || !gi.fFile || gs.fBitmap) && y < gs.yWin - 1)
				{
					s = gi.nScale;
					gi.nScale = gi.nScaleT;
					DrawObject(i, gs.xWin - 12 * gi.nScaleT, y - (yFont / 2 - 1) * gi.nScaleT);
					gi.nScale = s;
				}
				sprintf(sz, "%c ", cp1.vel_longitude[i] < 0.0 ? chRet : ' ');
				DrawPrint(sz, gi.kiOn, TRUE);
				if (gi.nCurrChart != gSector || !is.fSeconds)
				{
					if (FThing(i))
					{
						is.fSeconds = FALSE;
						DrawPrint(SzAltitude(cp1.latitude[i]), gi.kiLite, TRUE);
						is.fSeconds = us.fSeconds;
					}
					else
						DrawPrint("       ", gi.kiOn, TRUE);
				}
				if (gi.nCurrChart == gSector)
				{
					r = ((360.0 - cp2.longitude[i]) / 10.0);
					s = (int) r + 1;
					if (!is.fSeconds)
						sprintf(sz, " %2d", s);
					else
						sprintf(sz, "%6.3f%c", r + 1.0, pluszone[s] ? '+' : '-');
					DrawPrint(sz, pluszone[s] ? kRainbowB[1] : kMainB[5], FALSE);
				}
				else
					DrawPrint("", gi.kiOn, FALSE);
			}
		}

		/* Print star positions. */

		for (i = starLo; i <= starHi; i++)
		{
			if (FProper2(i))
			{
				s = cLastMoving + starname[i - cLastMoving];
				sprintf(sz, is.fSeconds ? "%3.3s: " : "%4.4s: ", Lang(tObjShortName, s));
				DrawPrint(sz, kObjB[s], TRUE);
				DrawPrint(SzZodiac(cp1.longitude[s]), kSignB(Z2Sign(cp1.longitude[s])), TRUE);
				DrawPrint("  ", gi.kiOn, TRUE);
				if (gi.nCurrChart != gSector || !is.fSeconds)
					DrawPrint(SzAltitude(cp1.latitude[s]), gi.kiLite, TRUE);
				if (gi.nCurrChart == gSector)
				{
					r = (360.0 - cp1.longitude[s])/10.0;
					s = (int) r + 1;
					if (!is.fSeconds)
						sprintf(sz, " %2d", s);
					else
						sprintf(sz, "%6.3f%c", r + 1.0, pluszone[s] ? '+' : '-');
					DrawPrint(sz, pluszone[s] ? kRainbowB[1] : kMainB[5], FALSE);
				}
				else
					DrawPrint("", gi.kiOn, FALSE);
			}
		}
	}
	// end of temporary feature


	/* Print element table information. */

	DrawPrint("", gi.kiLite, FALSE);
	CreateElemTable(&et);
	sprintf(sz, Lang(sFIRE_EARTH), et.coElem[eFir], et.coElem[eEar]);
	DrawPrint(sz, gi.kiLite, FALSE);
	sprintf(sz, Lang(sAIR_WATER), et.coElem[eAir], et.coElem[eWat]);
	DrawPrint(sz, gi.kiLite, FALSE);
	sprintf(sz, Lang(sCAR_FIX_MUT), et.coMode[0], et.coMode[1], et.coMode[2]);
	DrawPrint(sz, gi.kiLite, FALSE);
	sprintf(sz, Lang(sYANG_YIN), et.coYang, et.coYin);
	DrawPrint(sz, gi.kiLite, FALSE);
	sprintf(sz, Lang(sM_N_A_D), et.coMC, et.coIC, et.coAsc, et.coDes);
	DrawPrint(sz, gi.kiLite, FALSE);
	sprintf(sz, Lang(sANG_SUC_CAD), et.coModeH[0], et.coModeH[1], et.coModeH[2]);
	DrawPrint(sz, gi.kiLite, FALSE);
	sprintf(sz, Lang(sLEARN_SHARE), et.coLearn, et.coShare);
	DrawPrint(sz, gi.kiLite, FALSE);
	us.fAnsiChar = a;
}


/* This is a subprocedure of XChartWheel() and XChartWheelRelation(). Draw  */
/* the outer sign and house rings for a wheel chart at the specified zodiac */
/* locations and at the given radius values.                                */

void DrawWheel(double *xsign, double *xhouse, int cx, int cy, double unitx, double unity, double r1, double r2, double r3, double r4, double r5) {
	int i, sc;
	double px, py, temp;

	/* Draw Ascendant/Descendant and Midheaven/Nadir lines across whole chart. */

	DrawColor(gi.kiLite);
	DrawDash(cx + POINT1(unitx, 0.99, PX(xhouse[sAri])),
			 cy + POINT1(unity, 0.99, PY(xhouse[sAri])),
			 cx + POINT1(unitx, 0.99, PX(xhouse[sLib])), 
			 cy + POINT1(unity, 0.99, PY(xhouse[sLib])), !gs.fColor);
	DrawDash(cx + POINT1(unitx, 0.99, PX(xhouse[sCap])),
			 cy + POINT1(unity, 0.99, PY(xhouse[sCap])),
			 cx + POINT1(unitx, 0.99, PX(xhouse[sCan])), 
			 cy + POINT1(unity, 0.99, PY(xhouse[sCan])), !gs.fColor);

	/* Draw small five or one degree increments around the zodiac sign ring. */

	for (i = 0; i < 360; i++)
	{
		temp = PZ(HousePlaceInX((double) i));
		px = PX(temp);
		py = PY(temp);
		DrawColor(i % 5 ? gi.kiGray : gi.kiOn);
		DrawDash(cx + POINT1(unitx, r3, px), cy + POINT1(unity, r3, py),
				 cx + POINT2(unitx, r4, px), cy + POINT2(unity, r4, py),
				 ((!gs.fColor || gs.fPS || gs.fMeta) && i % 5) * 2);
	}

	/* Draw circles for the zodiac sign and house rings. */

	DrawColor(gi.kiOn);
	DrawCircle(cx, cy, (int) (unitx * 0.95 + 0.5), (int) (unity * 0.95 + 0.5));
	DrawCircle(cx, cy, (int) (unitx * r4 + 0.5), (int) (unity * r4 + 0.5));
	DrawCircle(cx, cy, (int) (unitx * r3 + 0.5), (int) (unity * r3 + 0.5));
	DrawCircle(cx, cy, (int) (unitx * r1 + 0.5), (int) (unity * r1 + 0.5));

	/* Draw the glyphs for the signs and houses themselves. */

	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		temp = xsign[i];
		DrawColor(gi.kiOn);
		DrawLine(cx + POINT2(unitx, 0.95, PX(temp)),	/* Draw lines separating */
				 cy + POINT2(unity, 0.95, PY(temp)),	/* each sign and house   */
				 cx + POINT1(unitx, r4, PX(temp)),		/* from each other.      */
				 cy + POINT1(unity, r4, PY(temp)));
		DrawLine(cx + POINT2(unitx, r3, PX(xhouse[i])),
				 cy + POINT2(unity, r3, PY(xhouse[i])),
				 cx + POINT1(unitx, r1, PX(xhouse[i])), cy + POINT1(unity, r1, PY(xhouse[i])));

		if (gs.fColor && i % 3 != 1)
		{															/* Lines from */
			DrawColor(gi.kiGray);									/* each house */
			DrawDash(cx, cy, cx + POINT2(unitx, r1, PX(xhouse[i])),	/* to center  */
					 cy + POINT2(unity, r1, PY(xhouse[i])), 1);		/* of wheel.  */
		}

		temp = Midpoint(temp, xsign[Mod12(i + 1)]);
		DrawColor(kSignB(i));
		DrawSign(i, cx + POINT1(unitx, r5, PX(temp)), cy + POINT1(unity, r5, PY(temp)));

		if (fDisp)
		{						/* Draw dispositor's glyph. VA. */
			sc = gi.nScale;
			temp = Midpoint(xsign[i], temp);
			if (gi.nScale > 1)
				gi.nScale -= 1;
			DrawObject(rules[i], cx + POINT1(unitx, r5, PX(temp)), cy + POINT1(unity, r5, PY(temp)));
			gi.nScale = sc;
		}

		DrawColor(kSignB(i));
		temp = Midpoint(xhouse[i], xhouse[Mod12(i + 1)]);
		DrawHouse(i, cx + POINT1(unitx, r2, PX(temp)), cy + POINT1(unity, r2, PY(temp)));

		if (fDisp)
		{
			sc = gi.nScale;
			temp = Midpoint(xhouse[i], temp);
			if (gi.nScale > 1)
				gi.nScale -= 1;
			DrawObject(rules[i], cx + POINT1(unitx, r2, PX(temp)), cy + POINT1(unity, r2, PY(temp)));
			gi.nScale = sc;
		}
	}
}


/* Another subprocedure of XChartWheel() and XChartWheelRelation(). Draw */
/* a set of planets in a wheel chart, drawing each glyph and a line from */
/* it to a dot indicating the planet's actual location.                  */

void DrawSymbolRing(double *symbol, double *xplanet, double *dir, int cx, int cy, double unitx, double unity, double r1, double r2, double r3, double r4) {
	int i;
	double temp;

	for (i = cObj; i >= 0; i--)
		if (FProper(i))
		{
			if (gs.fLabel)
			{
				temp = symbol[i];
				DrawColor(dir[i] < 0.0 ? gi.kiGray : gi.kiOn);
				DrawDash(cx + POINT1(unitx, r2, PX(xplanet[i])),
						 cy + POINT1(unity, r2, PY(xplanet[i])),
						 cx + POINT1(unitx, r3, PX(temp)),
						 cy + POINT1(unity, r3, PY(temp)), (cp0.vel_longitude[i] < 0.0 ? 1 : 0) - gs.fColor);
				DrawObject(i, cx + POINT1(unitx, r4, PX(temp)), cy + POINT1(unity, r4, PY(temp)));
			}
			else
				DrawColor(kObjB[i]);
			DrawSpot(cx + POINT1(unitx, r1, PX(xplanet[i])), cy + POINT1(unity, r1, PY(xplanet[i])));
		}
}


/*
******************************************************************************
** Map Chart Routines.
******************************************************************************
*/

/* Another stream reader, this one is used by the globe drawing routine: */
/* for the next body of land/water, return its name (and color), its     */
/* longitude and latitude, and a vector description of its outline.      */

BOOL FReadWorldData(char **nam, char ** loc, char ** lin) {
	static char **psz = (char **) szWorldData;
	int i;

	*loc = *psz++;
	*lin = *psz++;
	*nam = *psz++;
	if (*loc[0])
	{
		if (gs.fPrintMap && gi.fFile)
		{
			i = **nam - '0';
			AnsiColor(i ? kRainbowA[i] : kMainA[7]);
			PrintSz(*nam + 1);
			PrintL();
		}
		return TRUE;
	}
	psz = (char **) szWorldData;	/* Reset stream when no data left. */
	return FALSE;
}


/* Given longitude and latitude values on a globe, return the window        */
/* coordinates corresponding to them. In other words, project the globe     */
/* onto the view plane, and return where our coordinates got projected to,  */
/* as well as whether our location is hidden on the back side of the globe. */

BOOL FGlobeCalc(double x1, double y1, int *u, int *v, int cx, int cy, int rx, int ry, int deg) {
	double j, siny1;

	/* Compute coordinates for a general globe invoked with -XG switch. */

	if (gi.nCurrChart == gGlobe)
	{
		x1 = Mod(x1 + (double) deg);	/* Shift by current globe rotation value. */
		if (gs.rTilt != 0.0)
		{
			/* Do another coordinate shift if the globe's equator is tilted any. */
			x1 = Deg2Rad(x1);
			y1 = Deg2Rad(90.0 - y1);
			CoorXform(&x1, &y1, Deg2Rad(gs.rTilt));
			x1 = Mod(Rad2Deg(x1));
			y1 = 90.0 - Rad2Deg(y1);
		}
		*v = cy + (int) ((double) ry * -RCosD(y1) - 0.5);
		*u = cx + (int) ((double) rx * -RCosD(x1) * RSinD(y1) - 0.5);
		return x1 > 180.0;
	}

	/* Compute coordinates for a polar globe invoked with -XP switch. */

	siny1 = RSinD(y1);
	j = gs.fBonusMode ? 90.0 + x1 + deg : 270.0 - x1 - deg;
	*v = cy + (int) (siny1 * (double) ry * RSinD(j) - 0.5);
	*u = cx + (int) (siny1 * (double) rx * RCosD(j) - 0.5);
	return gs.fBonusMode ? y1 < 90.0 : y1 > 90.0;
}


/* Draw one "Ley line" on the world map, based coordinates given in terms of */
/* longitude and vertical fractional distance from the center of the earth.  */

void DrawLeyLine(double l1, double f1, double l2, double f2) {
	l1 = Mod(l1);
	l2 = Mod(l2);

	/* Convert vertical fractional distance to a corresponding coordinate. */

	f1 = 90.0 - asin(f1) / rPiHalf * 90.0;
	f2 = 90.0 - asin(f2) / rPiHalf * 90.0;
	DrawWrap((int) (l1 * (double) gi.nScale + 0.5),
			 (int) (f1 * (double) gi.nScale + 0.5),
			 (int) (l2 * (double) gi.nScale + 0.5), (int) (f2 * (double) gi.nScale + 0.5), 0, gs.xWin - 1);
}


/* Draw the main set of planetary Ley lines on the map of the world. This */
/* consists of drawing an icosahedron and then a dodecahedron lattice.    */

void DrawLeyLines(int deg) {
	double off = (double) deg, phi, h, h1, h2, r, i;

	phi = (sqrt(5.0) + 1.0) / 2.0;	/* Icosahedron constants. */
	h = 1.0 / (phi * 2.0 - 1.0);
	DrawColor(kMainB[6]);
	for (i = off; i < 360.0 + off; i += 72.0)
	{							/* Draw icosahedron edges. */
		DrawLeyLine(i, h, i + 72.0, h);
		DrawLeyLine(i - 36.0, -h, i + 36.0, -h);
		DrawLeyLine(i, h, i, 1.0);
		DrawLeyLine(i + 36.0, -h, i + 36.0, -1.0);
		DrawLeyLine(i, h, i + 36.0, -h);
		DrawLeyLine(i, h, i - 36.0, -h);
	}
	r = 1.0 / sqrt(3.0) / phi / cos(Deg2Rad(54.0));	/* Dodecahedron constants. */
	h2 = sqrt(1.0 - r * r);
	h1 = h2 / (phi * 2.0 + 1.0);
	DrawColor(kMainB[4]);
	for (i = off; i < 360.0 + off; i += 72.0)
	{							/* Draw docecahedron edges. */
		DrawLeyLine(i - 36.0, h2, i + 36.0, h2);
		DrawLeyLine(i, -h2, i + 72.0, -h2);
		DrawLeyLine(i + 36.0, h2, i + 36.0, h1);
		DrawLeyLine(i, -h2, i, -h1);
		DrawLeyLine(i + 36.0, h1, i + 72.0, -h1);
		DrawLeyLine(i + 36.0, h1, i, -h1);
	}
}


/* This major routine draws all of Astrolog's map charts. This means       */
/* either the world map or the constellations, in either rectangular or    */
/* globe hemisphere form. The rectangular chart may also be done in a      */
/* Mollewide projection, for six total combinations. We shift the chart by */
/* specified rotational and tilt values, and may draw on the chart each    */
/* planet at its zenith position on Earth or location in constellations.   */

void DrawMap(BOOL fSky, BOOL fGlobe, int deg) {
	char *nam, *loc, *lin, chCmd;
	int X[NUMBER_OBJECTS], Y[NUMBER_OBJECTS], M[NUMBER_OBJECTS], N[NUMBER_OBJECTS],
		cx = gs.xWin / 2, cy = gs.yWin / 2, rx, ry, lon, lat, unit =
		12 * gi.nScale, x, y, xold, yold, m, n, u, v, i, j, k, l, nScl = gi.nScale;
	BOOL fNext = TRUE, fCan;
	double planet1[NUMBER_OBJECTS], planet2[NUMBER_OBJECTS], x1, y1, rT;

#ifdef CONSTEL
	char *pch;
	BOOL fBlank;
	int isz = 0, nC, xT, yT, xDelta, yDelta, xLo, xHi, yLo, yHi;
#endif

	/* Set up some variables. */
	rx = cx - 1;
	ry = cy - 1;
	if (fGlobe)
		fCan = (gs.rTilt == 0.0 && gi.nCurrChart != gPolar);

#ifdef CONSTEL
	/* Draw a dot grid for large rectangular constellation charts. */
	if (fSky && !fGlobe && !gs.fMollewide && gi.nScale / gi.nScaleT > 2)
		for (yT = 5; yT < 180; yT += 5)
			for (xT = 5; xT <= 360; xT += 5)
			{
				DrawColor(xT % 15 == 0 && yT % 10 == 0 ? gi.kiOn : gi.kiGray);
				x = xT + deg;
				if (x > 360)
					x -= 360;
				DrawPoint(x * nScl, yT * nScl);
			}
#endif

	for(;;)
	{

		/* Get the next chunk of data to process. Get the starting position, */
		/* map it to the screen, and set the drawing color appropriately.    */

		if (fNext)
		{
			fNext = FALSE;

			/* For constellations, get data for the next constellation shape. */

			if (fSky)
			{
#ifdef CONSTEL
				isz++;
				if (isz > cCnstl)
					break;
				DrawColor(gs.fBonusMode && gi.nCurrChart != gPolar
						  && (gi.nCurrChart != gWorldMap || !gs.fMollewide) ? kMainB[7] : kRainbowB[6]);
				pch = (char *) szDrawConstel[isz];
				lon = 360 - (((pch[2] - '0') * 10 + (pch[3] - '0')) * 15 + (pch[4] - '0') * 10 + (pch[5] - '0'));
				lat = 90 - ((pch[6] == '-' ? -1 : 1) * ((pch[7] - '0') * 10 + (pch[8] - '0')));
				pch += 9;
				xLo = xHi = xT = xold = x = lon;
				yLo = yHi = yT = yold = y = lat;
				nC = 0;
				if (fGlobe)
				{
					FGlobeCalc((double) x, (double) y, &m, &n, cx, cy, rx, ry, deg);
					k = l = TRUE;
				}
				else
				{
					xold += deg;
					x += deg;
				}
#else
				;
#endif

				/* For world maps, get data for the next coastline piece. */

			}
			else
			{
				if (!FReadWorldData(&nam, &loc, &lin))
					break;
				i = nam[0] - '0';
				DrawColor((!fGlobe && gi.nCurrChart == gAstroGraph) ? gi.kiOn :
						  (gi.nCurrChart == gGlobe && gs.fBonusMode) ? gi.kiGray : (i ? kRainbowB[i] : kMainB[7]));
				lon = (loc[0] == '+' ? 1 : -1) * ((loc[1] - '0') * 100 + (loc[2] - '0') * 10 + (loc[3] - '0'));
				lat = (loc[4] == '+' ? 1 : -1) * ((loc[5] - '0') * 10 + (loc[6] - '0'));
				if (fGlobe)
				{
					x = 180 - lon;
					y = 90 - lat;
					FGlobeCalc((double) x, (double) y, &m, &n, cx, cy, rx, ry, deg);
					k = l = TRUE;
				}
				else
				{
					xold = x = 181 - lon + deg;
					yold = y = 90 - lat;
				}
			}
		}

		/* Get the next unit from the string to draw on the screen as a line. */

		if (fSky)
		{

			/* For constellations we have a cache of how long we should keep    */
			/* going in the previous direction, as say "u5" for up five should  */
			/* move our pointer up five times without advancing string pointer. */

#ifdef CONSTEL
			if (nC <= 0)
			{
				if (!(chCmd = *pch))
				{
					fNext = TRUE;
					if (gs.fText)
					{

						/* If we've reached the end of current constellation, compute */
						/* the center location in it based on lower and upper bounds  */
						/* we've maintained, and print the name of the constel there. */

						xT = xLo + (xHi - xLo) * (szDrawConstel[isz][0] - '1') / 8;
						yT = yLo + (yHi - yLo) * (szDrawConstel[isz][1] - '1') / 8;
						if (xT < 0)
							xT += 360;
						else if (xT > 360)
							xT -= 360;
						if (fGlobe)
						{
							if (FGlobeCalc((double) xT, (double) yT, &x, &y, cx, cy, rx, ry, deg))
								continue;
						}
						else
						{
							xT += deg;
							if (xT > 360)
								xT -= 360;
							if (gs.fMollewide)
								x = 180 * nScl + NMultDiv(xT - 180, NMollewide(yT - 91), 180L);
							else
								x = xT * nScl;
							y = yT * nScl;
						}
						DrawColor(gs.fBonusMode && gi.nCurrChart != gPolar
								  && (gi.nCurrChart != gWorldMap || !gs.fMollewide) ? gi.kiGray : kMainB[5]);
						DrawSz(szCnstlAbbrev[isz], x, y, dtCent);
					}
					continue;
				}
				pch++;

				/* Get the next direction and distance from constellation string. */

				if (fBlank = (chCmd == 'b'))
					chCmd = *pch++;
				xDelta = yDelta = 0;
				switch (chCmd)
				{
				case 'u':
					yDelta = -1;
					break;		/* Up    */
				case 'd':
					yDelta = 1;
					break;		/* Down  */
				case 'l':
					xDelta = -1;
					break;		/* Left  */
				case 'r':
					xDelta = 1;
					break;		/* Right */
				case 'U':
					yDelta = -1;
					nC = (yT - 1) % 10 + 1;
					break;		/* Up until    */
				case 'D':
					yDelta = 1;
					nC = 10 - yT % 10;
					break;		/* Down until  */
				case 'L':
					xDelta = -1;
					nC = (xT + 599) % 15 + 1;
					break;		/* Left until  */
				case 'R':
					xDelta = 1;
					nC = 15 - (xT + 600) % 15;
					break;		/* Right until */
				default:
					PrintError("Bad draw.");	/* Shouldn't happen. */
				}
				if (chCmd >= 'a')
					nC = NFromPch((const char**)&pch);	/* Figure out how far to draw. */
			}
			nC--;
			xT += xDelta;
			x += xDelta;
			yT += yDelta;
			y += yDelta;
			if (fBlank)
			{
				xold = x;
				yold = y;		/* We occasionally want to move the pointer */
				l = FALSE;		/* without drawing the line on the screen.  */
				continue;
			}
			if (xT < xLo)		/* Maintain our bounding rectangle for this */
				xLo = xT;		/* constellation if we crossed over it any. */
			else if (xT > xHi)
				xHi = xT;
			if (yT < yLo)
				yLo = yT;
			else if (yT > yHi)
				yHi = yT;
#else
			;
#endif

		}
		else
		{

			/* Get the next unit from the much simpler world map strings. */

			if (!(chCmd = *lin))
			{
				fNext = TRUE;
				continue;
			}
			lin++;

			/* Each unit is exactly one character in the coastline string. */

			if (chCmd == 'L' || chCmd == 'H' || chCmd == 'G')
				x--;
			else if (chCmd == 'R' || chCmd == 'E' || chCmd == 'F')
				x++;
			if (chCmd == 'U' || chCmd == 'H' || chCmd == 'E')
				y--;
			else if (chCmd == 'D' || chCmd == 'G' || chCmd == 'F')
				y++;
		}

		/* Transform map coordinates to screen coordinates and draw a line. */

		while (x >= 360)	/* Take care of coordinate wrap around. */
			x -= 360;
		while (x < 0)
			x += 360;
		if (abs(x - xold) > 180)
			xold = x;

		if (fGlobe)
		{

			/* For globes, we have to go do a complicated transformation, and not */
			/* draw when we're hidden on the back side of the sphere. We're smart */
			/* and try to only do the slow stuff when we know we'll be visible.   */

			if (fCan)
			{
				k = x + deg;
				if (k >= 360)
					k -= 360;
				k = (k <= 180);
			}
			if (k && !FGlobeCalc((double) x, (double) y, &u, &v, cx, cy, rx, ry, deg))
			{
				if (l)
					DrawLine(m, n, u, v);
				m = u;
				n = v;
				l = TRUE;
			}
			else
				l = FALSE;
		}
		else
		{

			/* Rectangular maps are much simpler, with screen coordinates      */
			/* proportional to internal coords. For the Mollewide projection   */
			/* we have to apply a factor to the horizontal positioning though. */

			if (gs.fMollewide && gi.nCurrChart != gAstroGraph)
				DrawLine(180 * nScl + NMultDiv(xold - 180,
											   NMollewide(yold - 91), 180L),
						 (yold - 1) * nScl, 180 * nScl + NMultDiv(x - 180, NMollewide(y - 91), 180L), (y - 1) * nScl);
			else
				DrawLine(xold * nScl, (yold - 1) * nScl, x * nScl, (y - 1) * nScl);
			xold = x;
			yold = y;
		}
	}

	/* Draw the outline of the map, either a circle around globes or a */
	/* Mollewide type ellipse for that type of rectangular chart.      */

	DrawColor(gi.kiOn);
	if (!fGlobe)
	{
		if (gs.fMollewide && gi.nCurrChart != gAstroGraph)
			if (!gs.fBonusMode)
				for (j = -1; j <= 1; j += 2)
					for (xold = 0, y = 89; y >= 0; y--, xold = x)
						for (x = NMollewide(y), i = -1; i <= 1; i += 2)
						{
							DrawLine(180 * nScl + i * xold - (i == 1),
									 (90 + j * (y + 1)) * nScl - (j == 1),
									 180 * nScl + i * x - (i == 1), (90 + j * y) * nScl - (j == 1));
						}
	}
	else
		DrawEllipse(0, 0, gs.xWin - 1, gs.yWin - 1);

	/* Now, if we are in an appropriate bonus chart mode, draw each planet at */
	/* its zenith or visible location on the globe or map, if not hidden.     */

	if (!gs.fBonusMode || (gi.nCurrChart != gGlobe && (!fSky || gi.nCurrChart != gWorldMap || gs.fMollewide)))
		return;
	rT = gs.fConstel ? 180.0 - (fGlobe ? 0.0 : (double) deg) : ciMain.lon;
	if (rT < 0.0)
		rT += 360.0;
	for (i = 1; i <= cObj; i++)
	{
		planet1[i] = Deg2Rad(Tropical(cp0.longitude[i]));
		planet2[i] = Deg2Rad(cp0.latitude[i]);
		EclToEqu(&planet1[i], &planet2[i]);	/* Calculate zenith long. & lat. */
	}

	/* Compute screen coordinates of each object, if it's even visible. */

	for (i = 1; i <= cObj; i++)
		if (FProper(i))
		{
			if (fSky)
				x1 = planet1[i];
			else
				x1 = planet1[oMC] - planet1[i];
			if (x1 < 0.0)
				x1 += 2.0 * rPi;
			if (x1 > rPi)
				x1 -= 2.0 * rPi;
			x1 = Mod(180.0 - rT - Rad2Deg(x1));
			y1 = 90.0 - Rad2Deg(planet2[i]);
			if (fGlobe)
			{
				X[i] = FGlobeCalc(x1, y1, &u, &v, cx, cy, rx, ry, deg) ? -1000 : u;
				Y[i] = v;
			}
			else
			{
				X[i] = (int) (x1 * (double) nScl);
				Y[i] = (int) (y1 * (double) nScl);
			}
			M[i] = X[i];
			N[i] = Y[i] + unit / 2;
		}

	/* Now that we have the coordinates of each object, figure out where to   */
	/* draw the glyphs. Again we try not to draw glyphs on top of each other. */

	for (i = 1; i <= cObj; i++)
	{
		if (FProper(i))
		{
			k = l = gs.xWin + gs.yWin;

			/* For each planet, we draw the glyph either right over or right under */
			/* the actual zenith location point. So, find out the closest distance */
			/* of any other planet assuming we place ours at both possibilities.   */

			for (j = 1; j < i; j++)
				if (FProper(j))
				{
					k = Min(k, abs(M[i] - M[j]) + abs(N[i] - N[j]));
					l = Min(l, abs(M[i] - M[j]) + abs(N[i] - unit - N[j]));
				}

			/* Normally, we put the glyph right below the actual point. If however  */
			/* another planet is close enough to have their glyphs overlap, and the */
			/* above location is better, then we'll draw the glyph above instead.   */

			if (k < unit || l < unit)
				if (k < l)
					N[i] -= unit;
		}
	}

	for (i = cObj; i >= 1; i--)
		if (X[i] >= 0 && FProper(i))	/* Draw the */
			DrawObject(i, M[i], N[i]);	/* glyphs.  */

	for (i = cObj; i >= 1; i--)
	{
		if (X[i] >= 0 && FProper(i))
		{
			DrawColor(kObjB[i]);
			DrawSpot(X[i], Y[i]);
		}
	}
}


/* Create a chart in the window based on the current graphics chart mode. */
/* This is the main dispatch routine for all of the program's graphics.   */

void DrawChartX()
{
	char sz[cchSzDef];
	int i;
	BOOL fT;

	gi.nScale = gs.nScale / 100;

	DrawClearScreen();
#ifdef CONSTEL
	fT = gs.fConstel;
#else
	fT = FALSE;
#endif

	switch (gi.nCurrChart)
	{
	case gDebugFont:
		XDebugFont();
		break;
	case gWheel:
	case gHouse:
		if (us.nRel > rcDual)
			XChartWheel();

		else if (us.nRel == rcTriWheel || us.nRel == rcQuadWheel)
			XChartWheelThreeFour();

		else
			XChartWheelRelation();
		break;
	case gGrid:
		if (us.nRel > rcDual)
			XChartGrid();
		else
			XChartGridRelation();
		break;
	case gHorizon:
		if (us.fPrimeVert)
			XChartHorizonSky();
		else
			XChartHorizon();
		break;
	case gOrbit:
		XChartOrbit();
		break;
	case gSector:
		XChartSector();
		break;
	case gDisposit:
		XChartDispositor();
		break;
	case gAstroGraph:
		DrawMap(FALSE, FALSE, gs.nRot);	/* First draw map of world.           */
		XChartAstroGraph();		/* Then draw astro-graph lines on it. */
		break;
	case gCalendar:
		XChartCalendar();
		break;
	case gEphemeris:
		XChartEphemeris();
		break;
	case gWorldMap:
		DrawMap(fT, FALSE, gs.nRot);			/* First draw map of world. */
		if (!fT && gs.fBonusMode && !gs.fMollewide)	/* Then maybe Ley lines.    */
			DrawLeyLines(gs.nRot);
		break;
	case gGlobe:
	case gPolar:
		DrawMap(fT, TRUE, gs.nRot);
		break;
#ifdef BIORHYTHM
	case gBiorhythm:
		XChartBiorhythm();
		break;
#endif
	}

	/* Print text showing chart information at bottom of window. */

	DrawColor(gi.kiLite);

	if (fDrawText)
	{
		if (ciMain.mon == -1)
		{
			sprintf(sz, "(No time or space)");
		}
		else if (us.nRel == rcComposite)
		{
			sprintf(sz, "(Composite)");
		}
		else
		{
			fT = us.fAnsiChar;
			us.fAnsiChar = (!gs.fFont || (!gs.fMeta && !gs.fPS)) << 1;
			i = DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea);
			sprintf(sz, "%c%c%c %s %s (%s %s GMT) %s", chDay3(i),
				SzDate(ciMain.mon, ciMain.day, ciMain.yea, 2), 
				SzTim(ciMain.tim), ChDst(ciMain.dst), SzZone(ciMain.zon), 
				SzLocation(ciMain.lon, ciMain.lat));
			us.fAnsiChar = fT;
		}
		DrawSz(sz, gs.xWin / 2, gs.yWin - 3 * gi.nScaleT, dtBottom | dtErase);
	}

	/* Draw a border around the chart if the mode is set and appropriate. */

	if (fDrawBorder)
		DrawEdgeAll();
}

/* xcharts0.c */
