/*
** Astrolog File: xscreen.c
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
*/


#include "astrolog.h"


/*
******************************************************************************
** Interactive Screen Graphics Routines.
******************************************************************************
*/

/* Set up all the colors used by the program, i.e. the foreground and   */
/* background colors, and all the colors in the object arrays, based on */
/* whether or not we are in monochrome and/or reverse video mode.       */

void InitColorsX()
{
	int i;

	gi.kiOn = kMainA[!gs.fInverse];
	gi.kiOff = kMainA[gs.fInverse];
	gi.kiLite = gs.fColor ? kMainA[2 + gs.fInverse] : gi.kiOn;
	gi.kiGray = gs.fColor ? kMainA[3 - gs.fInverse] : gi.kiOn;
	for (i = 0; i <= 8; i++)
		kMainB[i] = gs.fColor ? kMainA[i] : gi.kiOn;
	for (i = 0; i <= 7; i++)
		kRainbowB[i] = gs.fColor ? kRainbowA[i] : gi.kiOn;
	for (i = 0; i < 4; i++)
		kElemB[i] = gs.fColor ? kElemA[i] : gi.kiOn;
	for (i = 0; i <= cAspect; i++)
		kAspB[i] = gs.fColor ? kAspA[i] : gi.kiOn;
	for (i = 0; i <= cObj; i++)
		kObjB[i] = gs.fColor ? kObjA[i] : gi.kiOn;
}


/* This routine opens up and initializes a window and prepares it to be */
/* drawn upon, and gets various information about the display, too.     */

void BeginX(void)
{
	if (wi.fChartWindow && (wi.xClient != gs.xWin || wi.yClient != gs.yWin) && wi.hdcPrint == hdcNil)
		ResizeWindowToChart();
	gi.xOffset = NMultDiv(wi.xClient - gs.xWin, wi.xScroll, nScrollDiv);
	gi.yOffset = NMultDiv(wi.yClient - gs.yWin, wi.yScroll, nScrollDiv);
	SetWindowOrgEx(wi.hdc, -gi.xOffset, -gi.yOffset, NULL);
	SetWindowExtEx(wi.hdc, wi.xClient, wi.yClient, NULL);
	SetMapMode(wi.hdc, MM_ANISOTROPIC);
	SelectObject(wi.hdc, GetStockObject(NULL_PEN));
	SelectObject(wi.hdc, GetStockObject(NULL_BRUSH));

	if (!gs.fJetTrail || wi.hdcPrint != hdcNil)
	{
		PatBlt(wi.hdc, -gi.xOffset, -gi.yOffset, wi.xClient, wi.yClient, gs.fInverse ? WHITENESS : BLACKNESS);
	}
	InitColorsX();
}

/* Add a certain amount of time to the current hour/day/month/year quantity */
/* defining the present chart. This is used by the chart animation feature. */
/* We can add or subtract anywhere from 1 to 9 seconds, minutes, hours,     */
/* days, months, years, decades, centuries, or millenia in any one call.    */
/* This is mainly just addition to the appropriate quantity, but we have    */
/* to check for overflows, e.g. Dec 30 + 3 days = Jan 2 of Current year + 1 */

void AddTime(CI *ci, int mode, int toadd)
{
	int d;
	double h, m;


	h = floor(ci->tim);
	m = RFract(ci->tim) * 100.0;

	if (mode == ANIMATION_FACTOR_SECONDS)
	{
		m += 1.0 / 60.0 * (double) toadd;	/* Add seconds. */
	}
	else if (mode == ANIMATION_FACTOR_MINUTES)
	{
		m += (double) toadd;		/* add minutes. */
	}

	/* Add hours, either naturally or if minute value overflowed. */

	if (m < 0.0 || m >= 60.0 || mode == ANIMATION_FACTOR_HOURS)
	{
		if (m >= 60.0)
		{
			m -= 60.0;
			toadd = NSgn(toadd);
		}
		else if (m < 0.0)
		{
			m += 60.0;
			toadd = NSgn(toadd);
		}
		h += (double) toadd;
	}

	/* Add days, either naturally or if hour value overflowed. */

	if (h >= 24.0 || h < 0.0 || mode == ANIMATION_FACTOR_DAYS)
	{
		if (h >= 24.0)
		{
			h -= 24.0;
			toadd = NSgn(toadd);
		}
		else if (h < 0.0)
		{
			h += 24.0;
			toadd = NSgn(toadd);
		}
		ci->day = AddDay(ci->mon, ci->day, ci->yea, toadd);
	}

	/* Add months, either naturally or if day value overflowed. */

	if (ci->day > (d = DayInMonth(ci->mon, ci->yea)) || ci->day < 1 || mode == ANIMATION_FACTOR_MONTHS)
	{
		if (ci->day > d)
		{
			ci->day -= d;
			toadd = NSgn(toadd);
		}
		else if (ci->day < 1)
		{
			ci->day += DayInMonth(Mod12(ci->mon - 1), ci->yea);
			toadd = NSgn(toadd);
		}
		ci->mon += toadd;
	}

	/* Add years, either naturally or if month value overflowed. */

	if (ci->mon > 12 || ci->mon < 1 || mode == ANIMATION_FACTOR_YEARS)
	{
		if (ci->mon > 12)
		{
			ci->mon -= 12;
			toadd = NSgn(toadd);
		}
		else if (ci->mon < 1)
		{
			ci->mon += 12;
			toadd = NSgn(toadd);
		}
		ci->yea += toadd;
	}

	if (mode == ANIMATION_FACTOR_DECADES)
	{
		ci->yea += 10 * toadd;		/* Add decades.   */
	}
	else if (mode == ANIMATION_FACTOR_CENTURIES)
	{
		ci->yea += 100 * toadd;		/* Add centuries. */
	}
	else if (mode == ANIMATION_FACTOR_MILLENNIA)
	{
		ci->yea += 1000 * toadd;		/* Add millenia.  */
	}

	ci->tim = h + m / 100.0;			/* Recalibrate hour time. */
}


/* Animate the current chart based on the given values indicating how much  */
/* to update by. We update and recast the current chart info appropriately. */
/* Note animation mode for comparison charts will update the second chart.  */

void Animate(int mode, int toadd)
{
	CI ciT;

	if (us.fProgressUS && !us.nRel)
	{
		ciT = ciCore;
	}

	if (gi.nCurrChart == gWorldMap || gi.nCurrChart == gGlobe || gi.nCurrChart == gPolar)
	{
		gs.nRot += toadd;

		// For animating globe display, add in appropriate degree value
		if (gs.nRot >= 360)
		{
			gs.nRot -= 360;	
		}
		else if (gs.nRot < 0)
		{
			gs.nRot += 360;
		}
	}
	else
	{
		if (mode == ANIMATION_FACTOR_HERE_AND_NOW)
		{
			/* For the continuous chart update to present moment */
			/* animation mode, go get whatever time it is now.   */
			SetHereAndNow(&ciCore);
		}
		else
		{	/* Otherwise add on appropriate time vector to chart info. */
			if (us.nRel)
			{
				ciCore = ciTwin;
			}
			else
			{
				ciCore = ciMain;
			}

			AddTime(&ciCore, mode, toadd);
		}

		if (us.nRel)
		{
			ciTwin = ciCore;
			ciCore = ciMain;
		}
		else
		{
			ciMain = ciCore;
		}

		if (us.fProgressUS && !us.nRel)
		{
			ciCore = ciT;
		}

		if (us.nRel)
		{
			CastRelation();
		}
		else
		{
			CastChart(TRUE);
		}
	}
}

/* Given two chart size values, adjust them such that the chart will look */
/* "square". We round the higher value down and check certain conditions. */

void SquareX(int *x, int *y, int force) {
	if (!force && !fSquare)
	{
		/* Unless we want to force a square, realize */
		/* that some charts look better rectangular. */
		return;	
	}

	if (*x > *y)
	{
		*x = *y;
	}
	else
	{
		*y = *x;
	}

	if (fSidebar)
	{
		/* Take into account chart's sidebar, if any. */
		*x += (SIDESIZE*gi.nScaleT);
	}
}

/*
******************************************************************************
** Main Graphics Processing.
******************************************************************************
*/


/* This is the main interface to all the graphics features. This routine     */
/* is called from the main program if any of the -X switches were specified, */
/* and it sets up for and goes and generates the appropriate graphics chart. */
/* We return TRUE if successfull, FALSE if some non-fatal error occurred.  */

BOOL FActionX()
{
	gi.fFile = (gs.fBitmap || gs.fPS || gs.fMeta);
#ifdef PS
	gi.fEps = gs.fPS > TRUE;
#endif

	/* First figure out what graphic mode to generate the chart in, based on */
	/* various non-X command switches, e.g. -L combined with -X, -g combined */
	/* with -X, and so on, and determine the size the window is to be, too.  */

	if (gi.nCurrChart == 0)
	{
		if (us.fDebugFont == TRUE)
			gi.nCurrChart = gDebugFont;
		else if (us.fWheel)
			gi.nCurrChart = gHouse;
		else if (us.fGrid || us.fMidpoint)
			gi.nCurrChart = gGrid;
		else if (us.fHorizon)
			gi.nCurrChart = gHorizon;
		else if (us.fOrbit)
			gi.nCurrChart = gOrbit;
		else if (us.fSector)
			gi.nCurrChart = gSector;
		else if (us.fInfluence)
			gi.nCurrChart = gDisposit;
		else if (us.fAstroGraph)
			gi.nCurrChart = gAstroGraph;
		else if (us.fCalendar)
			gi.nCurrChart = gCalendar;
		else if (us.fEphemeris)
			gi.nCurrChart = gEphemeris;
		else if (us.nRel == rcBiorhythm)
			gi.nCurrChart = gBiorhythm;
		else
			gi.nCurrChart = gWheel;
	}
	if (gi.nCurrChart == gGrid)
	{
		if (us.nRel <= rcDual && us.fMidpoint && !us.fAspList)
			us.fGridConfig = TRUE;
		gs.xWin = gs.yWin = (gs.nGridCell + (us.nRel <= rcDual)) * CELLSIZE * gi.nScale + 1;
	}
	else if (fMap)
	{
		gs.xWin = 360 * gi.nScale;
		gs.yWin = 180 * gi.nScale;
	}

	if (fSidebar || us.fInfluence)
		gs.xWin -= SIDESIZE;

	gi.nScaleT = gs.fPS ? PSMUL : (gs.fMeta ? METAMUL : 1);

	if (wi.hdcPrint != hdcNil)
		gi.nScaleT = METAMUL;


	if (gi.fFile)
	{
		if (gs.xWin == 0)
			gs.xWin = DEFAULTX;
		if (gs.yWin == 0)
			gs.yWin = DEFAULTY;
		if (fSidebar || us.fInfluence)
			gs.xWin += SIDESIZE;
		if (gs.xWin > BITMAPX)
			gs.xWin = BITMAPX;
		if (gs.yWin > BITMAPY)
			gs.yWin = BITMAPY;
		BeginFileX();
		if (gs.fBitmap)
		{
			gi.cbBmpRow = (gs.xWin + 1) >> 1;
			gi.yBand = gs.yWin;
			if (!FEnsureGrid())
				return FALSE;
			while ((gi.bm = (byte*)allocate((long) gi.cbBmpRow * gi.yBand, NULL)) == NULL)
			{
				PrintWarning("The bitmap must be generated in multiple stages.");
				gi.yBand = (gi.yBand + 1) / 2;
				if (gi.yBand < 1 || gs.chBmpMode != 'B')
					return FALSE;
			}
			if (gi.yBand == gs.yWin)
				gi.yBand = 0;
			else
			{
				gi.yOffset = gs.yWin - gs.yWin % gi.yBand;
				if (gi.yOffset == gs.yWin)
					gi.yOffset -= gi.yBand;
			}
		}
#ifdef PS
		else if (gs.fPS)
			PsBegin();
#endif
#ifdef META
		else
		{
			if (!FEnsureGrid())
				return FALSE;
			for (gi.cbMeta = MAXMETA; gi.cbMeta > 0 &&
				 (gi.bm = (byte*)allocate(gi.cbMeta, NULL)) == NULL; gi.cbMeta -= MAXMETA / 8)
				PrintWarning("Attempting to get maximum memory for metafile.");
			if (gi.cbMeta == 0)
				return FALSE;
			gs.xWin *= METAMUL;	/* Increase chart sizes and scales behind the */
			gs.yWin *= METAMUL;	/* scenes to make graphics look smoother.     */
			gs.nScale *= METAMUL;
		}
#endif
		InitColorsX();
	}
	else
	{
		if (gs.xWin == 0 || gs.yWin == 0)
		{
			if (gs.xWin == 0)
				gs.xWin = DEFAULTX;
			if (gs.yWin == 0)
				gs.yWin = DEFAULTY;
			SquareX(&gs.xWin, &gs.yWin, FALSE);
		}
		else if (fSidebar || us.fInfluence)
			gs.xWin += SIDESIZE;
		BeginX();
	}

	if (gi.fFile || gs.fRoot)	/* Go draw the graphic chart. */
		DrawChartX();
	if (gi.fFile)
	{							/* Write bitmap to file if in that mode. */
		EndFileX();
		while (gi.yBand)
		{
			gi.yOffset -= gi.yBand;
			DrawChartX();
			EndFileX();
		}
		if (!gs.fPS)
			free(gi.bm);
	}
	else
	{
		DrawChartX();
	}

	return TRUE;
}

/* xscreen.c */
