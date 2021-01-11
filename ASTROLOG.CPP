/*
** Astrolog File: astrolog.c
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
** Program Dispatch Procedures.
******************************************************************************
*/

/* Initialize the Ansi color arrays with the color to print each object in. */

void InitColors()
{
	int i;

	kObjA[oEar] = kElemA[eEar];
	for (i = oSun; i <= oPlu; i++)
		kObjA[i] = kSignA(ruler1[i]);
	for (i = oChi; i <= oVesta; i++)
		kObjA[i] = kMainA[8];
	for (i = oNoNode; i <= oEP; i++)
		kObjA[i] = kMainA[6];
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		kObjA[cuspLo + i - 1] = kSignA(i);
	for (i = uranLo; i <= uranHi; i++)
		kObjA[i] = kRainbowA[7];
	for (i = 0; i <= cLastMoving; i++)
	{
		if (kObjA1[i] != 0)
			kObjA[i] = kObjA1[i];
	}
	for (i = starLo; i <= starHi; i++)
	{
		if (rStarBright[i - cLastMoving] < 1.0)
			kObjA[i] = kStar1;
		else if (rStarBright[i - cLastMoving] < 2.0)
			kObjA[i] = kStar2;
		else
			kObjA[i] = kStar3;
	}
}


/* This is the dispatch procedure for the entire program. After all the   */
/* command switches have been processed, this routine is called to        */
/* actually call the various routines to generate and display the charts. */

void Action()
{
	char sz[cchSzDef];
	int i;

	NoPrintHeader = 0;
	is.fMult = FALSE;			/* Cleared here because no InitVariables routine. */
	is.fNoEphFile = FALSE;
	is.cchRow = 0;
	AnsiColor(kDefault);
	InitColors();

	//TK not nice

	Lang.SetTableItem(tAspectAbbrev, aCon,  us.fParallel ? "Par" : Lang(sAspectAbbrevCon));
	Lang.SetTableItem(tAspectAbbrev, aOpp,  us.fParallel ? "CPr" : Lang(sAspectAbbrevOpp));

	/* First let's adjust the restriction status of the cusps, uranians, and */
	/* fixed stars based on whether -C, -u, and -U switches are in effect.   */

	if (!us.nStar)
		for (i = starLo; i <= starHi; i++)
			ignore[i] = ignore2[i] = TRUE;

	/* If the -os switch is in effect, open a file and set a global to */
	/* internally 'redirect' all screen output to.                     */

	if (is.szFileScreen)
	{
		is.S = fopen(is.szFileScreen, "wb+");
		if (is.S == NULL)
		{
			sprintf(sz, "File %s can not be created.", is.szFileScreen);
			PrintError(sz);
			is.S = stdout;
		}
	}
	else if (wi.fAnimate==0){
		is.S = stdout;
	}
	/* Print out any generic tables specified.        */
	/* If nothing else to do, we can exit right away. */
	if (!FPrintTables())			
	{
		if (is.fMult)
		{
			PrintL2();
			is.fMult = FALSE;
		}

		/* If -+ or -- switches in effect, then add the specified delta value to */
		/* the date and use that as a new date before proceeding to make chart.  */

		if (us.dayDelta != 0)
		{
			is.JD = (double) MdyToJulian(ciCore.mon, ciCore.day + us.dayDelta, ciCore.yea);
			JulianToMdy(is.JD, &ciCore.mon, &ciCore.day, &ciCore.yea);
		}

		/* Here we either do a normal chart or some kind of relationship chart. */

		if (us.fProgressUS && !us.nRel && wi.fInitProgression && !us.fInDay)
		{
			// going through here means same date as initial
			wi.fInitProgression = FALSE;

			ciMain = ciTran;
		}
 
		if (!us.nRel)
		{
			if (wi.fCast)
			{
				wi.fCast = FALSE;
				CastChart(TRUE);
			}
 
			if (!us.fProgressUS || us.nRel) 
				ciMain = ciCore;
		} 
		else
			CastRelation();

		if (us.fGraphics)			/* If any of the X window switches in effect, */
			FActionX();				/* then go make a graphics chart...           */
		else
			PrintChart(is.fProgressIS);	/* Otherwise print chart on text screen. */

		if (us.fWriteFile)			/* If -o switch in effect, then write */
			FOutputData();			/* the chart information to a file.   */
	}

	if (is.S != stdout && wi.fAnimate==0)			/* If we were internally directing chart display to a */
		fclose(is.S);			/* file as with the -os switch, close it here.        */

	if (grid)
	{
		free(grid);
		grid = NULL;
	}
}

/*
******************************************************************************
** Command Switch Procedures.
******************************************************************************
*/

/* Given a string representing a command line (e.g. a macro string), go    */
/* parse it into its various switches and parameters, then go process them */
/* and change program settings. Basically a wrapper for other functions.   */

BOOL FProcessCommandLine(char *szLine) {
	char szCommandLine[cchSzMax], *rgsz[MAXSWITCHES];
	int argc, cb, fT;

	if (szLine == NULL)
	{
		return TRUE;
	}
	if (ciMain.mon != -1)
	{
		ciCore = ciMain;
	}
	cb = strlen(szLine) + 1;
	CopyRgb((byte *) szLine, (byte *) szCommandLine, cb);
	argc = NParseCommandLine(szCommandLine, rgsz);
	fT = FProcessSwitchesMain(argc, rgsz);
	ciMain = ciCore;
	return fT;
}


/* Given string representing a command line, convert it to an "argv" format */
/* of an array of strings, one for each switch or parameter, i.e. exactly   */
/* like the format of the command line as given when the program starts.    */

// Each pointer in the argv pointer array, will point to the beginning of
// a string in szLine when this function exits.

int NParseCommandLine(char *szLine, char **argv)
{
	int argc = 1, fSpace = TRUE, fQuote = FALSE;
	byte *pch = (byte*)szLine;

	/* Split the entered line up into its individual switch strings. */
	while (*pch >= ' ' || *pch == chTab)
	{
		if (*pch == ' ' || *pch == chTab)
		{
			/* Skip over the current run of spaces between strings. */

			if (!fSpace)
			{
				/* First space after a string, end parameter here. */
				if (!fQuote)
				{
					*pch = chNull;
					fSpace = TRUE;
				}
			}
		}
		else
		{
			if (fSpace)
			{
				/* First character after run of spaces, begin parameter here. */
				if (argc >= MAXSWITCHES - 1)
				{
					PrintWarning("Too many items - rest of line ignored.");
					break;
				}

				fQuote = (*pch == '"');
				argv[argc++] = (char*)(pch + fQuote);
				fSpace = FALSE;
			}
			else if (fQuote && *pch == '"')	/* Skip over the current string. */
			{
				*pch = chNull;
				fSpace = TRUE;
			}
		}

		pch++;
	}

	argv[0] = szAppNameCore;
	argv[argc] = NULL;			/* Set last string in switch array to Null. */

	return argc;
}



/*
******************************************************************************
** Configuration Procedures.
******************************************************************************
*/


/* Take many of the user visible settings, and write them out to a new     */
/* command switch file, which may be read in to restore those settings.    */
/* Most often this would be used to create a new astrolog.dat default      */
/* settings file. This is called from the File Save Settings menu command. */

BOOL FOutputSettings()
{
	char sz[cchSzDef];
	char app_name[cchSzDef];
	FILE *file;
	int i;
	BOOL fT;

	if (us.fNoWrite)
		return FALSE;

	file = fopen(is.szFileOut, "w");	/* Create and open the file for output. */
	if (file == NULL)
	{
		fprintf(file, "Settings file %s can not be created.", is.szFileOut);
		PrintError(sz);
		return FALSE;
	}

	for(i = 0; *(szAppName + i); i++)
	{
		app_name[i] = toupper(*(szAppName + i));
	}
	app_name[i] = 0; // terminate string

	fprintf(file, "@%02d%02d\n\n; %s DEFAULT SETTINGS\n\n", 
		FILE_TYPE_CONFIGURATION, FILE_VERSION_CONFIGURATION, app_name);
	fprintf(file, "; %s Version: %s\n; File Name: %s\n\n", szAppName, 
		szVersionCore, CONFIGURATION_FILE);
	fprintf(file, "; NOTE: Everything from a semicolon (;) until the end of the same line\n");
	fprintf(file, ";       is considered a comment, and is disregarded by the program.\n\n");
	fprintf(file, "-z0 %d             ", (int) us.dstDef);
	fprintf(file, "; Default Daylight time setting   [0 standard, 1 daylight]\n");
	fprintf(file, "-z %s          ", SzZone(-us.zonDef));
	fprintf(file, "; Default time zone               [hours before GMT      ]\n");
	fT = us.fAnsiChar;
	us.fAnsiChar = FALSE;
	fprintf(file, "-zl %s  ", SzLocation(us.lonDef, us.latDef));
	us.fAnsiChar = fT;
	fprintf(file, "; Default longitude and latitude\n");

	if(*us.szLocNameDef) // do not write default location if not set
	{
		fprintf(file, "-zn \"%s\" ; Default location name\n", us.szLocNameDef);
	}

	fprintf(file, "\n%cs      ", ChDashF(us.fSidereal));
	fprintf(file, "; Zodiac selection          [\"_s\" is tropical, \"=s\" is sidereal]\n");
	fprintf(file, ":sm %d   ", us.nSiderealMode);
	fprintf(file, "; Sidereal Mode             [Change \"0\" to desired mode        ]\n");
	fprintf(file, ":s %.0f    ", us.rSiderealCorrection);
	fprintf(file, "; Sidereal Correction value [Change \"0\" to desired offset      ]\n");
	fprintf(file, "-c %d    ", us.nHouseSystem);
	fprintf(file, "; House system              [Use number, or name with minimum 3 letters (ex. Koch)]\n");
	fprintf(file, "%ck      ", ChDashF(us.fAnsiColor));
	fprintf(file, "; ANSI color text           [\"=k\" is color, \"_k\" is normal     ]\n");
	fprintf(file, ":d %d   ", us.nDivision);
	fprintf(file, "; Searching divisions       [Change \"%d\" to desired divisions  ]\n", us.nDivision);
	fprintf(file, "%cb0     ", ChDashF(us.fSeconds));
	fprintf(file, "; Print zodiac seconds      [\"_b0\" to minute, \"=b0\" to second  ]\n");
	fprintf(file, ":w %d    ", us.nWheelRows);
	fprintf(file, "; Wheel chart text rows     [Change \"4\" to desired wheel rows  ]\n");
	fprintf(file, ":I %d   ", us.nScreenWidth);
	fprintf(file, "; Text screen columns       [Change \"80\" to desired columns    ]\n");
	fprintf(file, "-YQ %d  ", us.nScrollRow);
	fprintf(file, "; Text screen scroll limit  [Change \"24\" or set to \"0\" for none]\n");
	fprintf(file, "%cYd     ", ChDashF(us.fEuroDate));
	fprintf(file, "; European date format      [\"_Yd\" is MDY, \"=Yd\" is DMY        ]\n");
	fprintf(file, "%cYt     ", ChDashF(us.fEuroTime));
	fprintf(file, "; European time format      [\"_Yt\" is AM/PM, \"=Yt\" is 24 hour  ]\n");
	fprintf(file, "%cYC     ", ChDashF(us.fSmartAspects));
	fprintf(file, "; Aspects display           [\"=YC\" smart, \"_YC\" show all       ]\n");
	fprintf(file, "%cY8     ", ChDashF(us.fClip80));
	fprintf(file, "; Clip text to end of line  [\"=Y8\" clips, \"_Y8\" doesn't clip   ]\n");
	fprintf(file, "-YP %d   ", us.nArabicNight);
	fprintf(file, "; Arabic Parts in night charts. (see Helpfile)\n\n\n");


	// SWITCHES NOT SUPPORTED BY ORIGINAL ASTROLOG

	fprintf(file, "; SWITCHES NOT SUPPORTED BY ORIGINAL ASTROLOG:\n\n");
	if(us.advancedMode) 
		fprintf(file, "=#AdvancedMode  ; Full mode\n");
	else
		fprintf(file, "=#SimplifiedMode  ; Simplified mode for beginners\n");

	if (StarRest)
		fprintf(file, "=");
	else
		fprintf(file, "_");

	fprintf(file, "YU       ; Stars alternative restriction. \"=YU\" alternative, \"_YU\" original.\n");
	fprintf(file, "-YUo %.1f  ; Orbs of fixed stars. Default 1.2 degrees\n", StarOrb);
	fprintf(file, "-YUa %d    ; Number of star aspects taken into account. Default 1.\n", StarAspects);
	fprintf(file, "-YUb1 %d   ; Stars colors (magnitude less than 1.0). Default 9 (red).\n", kStar1);
	fprintf(file, "-YUb2 %d   ; Stars colors (magnitude between 1.0 and 2.0). Default 3 (orange).\n", kStar2);
	fprintf(file, "-YUb3 %d   ; Stars colors (magnitude over .02). Default 1 (maroon).\n", kStar3);
	fprintf(file, "-YPo %.1f  ; Orbs of Arabic Parts. Default 1.0 degree\n", PartOrb);
	if (PartAspects)
	{
		fprintf(file, "-YPa %d    ; Number of Arabic Parts aspects taken into account. Default 1.\n", PartAspects);
		fprintf(file, "-YPs %d    ; Number of Arabic Parts aspects showed. Default 5.\n", PartAspectsShow);
	}
	fprintf(file, "%cYSa      ; Aspects display: applying-separating \"=YSa\", difference \"_YSa\".\n", ChDashF(us.fAppSep));
	if (us.fInfluenceSign)
		fprintf(file, "=");
	else
		fprintf(file, "_");
	fprintf(file, "YSj      ; Influence text chart shows by default more info.\n");
	if (us.fAspSummary)
		fprintf(file, "=");
	else
		fprintf(file, "_");
	fprintf(file, "YSs      ; Aspect listing shows by default also aspect summary.\n");
	fprintf(file, "-YOc %f ; Temporary decreasing of orbs in comparison charts.\n", CoeffComp);
	fprintf(file, "          ; Default 2.5 times.\n");
	fprintf(file, "-YOp %f ; Temporary decreasing of orbs with parallel aspects.\n", CoeffPar);
	fprintf(file, "          ; Default 6.0 times.\n");
	fprintf(file, "-YOP %.2f ; Power of parallels relatively to conjunctions.\n", PowerPar);
	fprintf(file, "          ; Default 0.25\n");
	if (PolarMCflip)
		fprintf(file, "=");
	else
		fprintf(file, "_");
	fprintf(file, "YH       ; Circumpolar flip of MC. \"=YH\" flips, \"_YH\" doesn't.\n");
	if (MonthFormat)
		fprintf(file, "=");
	else
		fprintf(file, "_");
	fprintf(file, "Ym       ; Month alternative format. \"=Ym\" alternative, \"_Ym\" original.\n");
	if (fNESW)
		fprintf(file, "=");
	else
		fprintf(file, "_");
	fprintf(file, "YZ       ; NESW local horizon. \"_YZ\" original ENWS.\n");
	if (fDisp)
		fprintf(file, "=");
	else
		fprintf(file, "_");
	fprintf(file, "YD       ; Dispositor's glyphs on the wheel: \"=YD\" on, \"_YD\" off.\n");
	fprintf(file, "%cYn	      ; Moon Nodes: True \"=Yn\", Mean \"_Yn\"\n", ChDashF(us.fTrueNode));
	fprintf(file, "%cYL	      ; Lilith: Osculating \"=YL\", Mean \"_YL\"\n", ChDashF(oscLilith));
	fprintf(file, "%cYSo      ; Aspects sorting: by orbs \"=YSo\", by power \"_YSo\"\n\n\n", ChDashF(fSortAspectsByOrbs));


	// DEFAULT RESTRICTIONS

	fprintf(file, "; DEFAULT RESTRICTIONS:\n\n-YR 0 10     ");
	for (i = 0; i <= 10; i++)
		fprintf(file, SzNumF(ignore[i]));
	fprintf(file, "     ; Planets\n-YR 11 21    ");
	for (i = 11; i <= 21; i++)
		fprintf(file, SzNumF(ignore[i]));
	fprintf(file, "   ; Minor planets\n-YR 22 33    ");
	for (i = 22; i <= 33; i++)
		fprintf(file, SzNumF(ignore[i]));
	fprintf(file, " ; House cusps\n-YR 34 42    ");
	for (i = 34; i <= 42; i++)
		fprintf(file, SzNumF(ignore[i]));
	fprintf(file, "       ; Uranians\n\n");

	// DEFAULT TRANSIT RESTRICTIONS

	fprintf(file, "; DEFAULT TRANSIT RESTRICTIONS:\n\n-YRT 0 10    ");
	for (i = 0; i <= 10; i++)
		fprintf(file, SzNumF(ignore2[i]));
	fprintf(file, "     ; Planets\n-YRT 11 21    ");
	for (i = 11; i <= 21; i++)
		fprintf(file, SzNumF(ignore2[i]));
	fprintf(file, "   ; Minor planets\n-YRT 22 33    ");
	for (i = 22; i <= 33; i++)
		fprintf(file, SzNumF(ignore2[i]));
	fprintf(file, " ; House cusps\n-YRT 34 42    ");
	for (i = 34; i <= 42; i++)
		fprintf(file, SzNumF(ignore2[i]));
	fprintf(file, "       ; Uranians\n\n");

	// DEFAULT PROGRESSION RESTRICTIONS (FOR DUAL CHARTS ONLY)

	fprintf(file, "; DEFAULT PROGRESSION RESTRICTIONS (FOR DUAL CHARTS ONLY):\n\n-YRP 0 10    ");
	for (i = 0; i <= 10; i++)
		fprintf(file, SzNumF(ignore3[i]));
	fprintf(file, "     ; Planets\n-YRP 11 21    ");
	for (i = 11; i <= 21; i++)
		fprintf(file, SzNumF(ignore3[i]));
	fprintf(file, "   ; Minor planets\n-YRP 22 33    ");
	for (i = 22; i <= 33; i++)
		fprintf(file, SzNumF(ignore3[i]));
	fprintf(file, " ; House cusps\n-YRP 34 42    ");
	for (i = 34; i <= 42; i++)
		fprintf(file, SzNumF(ignore3[i]));
	fprintf(file, "       ; Uranians\n\n");

	fprintf(file, "-YR0 %s%s ; Restrict sign, direction changes\n\n", 
		SzNumF(us.fIgnoreSign), SzNumF(us.fIgnoreDir));

	// DEFAULT ASPECT RESTRICTIONS

	fprintf(file, "; DEFAULT ASPECT RESTRICTIONS:\n\n-YAR 1 5    ");
	for (i = 1; i <= 5; i++)
	{
		fprintf(file, SzNumF(ignoreA[i]));
	}
	fprintf(file, "      ; Major aspects\n-YAR 6 11   ");
	for (i = 6; i <= 11; i++)
	{
		fprintf(file, SzNumF(ignoreA[i]));
	}
	fprintf(file, "    ; Minor aspects\n-YAR 12 18  ");
	for (i = 12; i <= 18; i++)
	{
		fprintf(file, SzNumF(ignoreA[i]));
	}
	fprintf(file, "  ; Obscure aspects\n\n");

	// DEFAULT ASPECT ORBS

	fprintf(file, "; DEFAULT ASPECT ORBS:\n\n-YAo 1 5    ");
	for (i = 1; i <= 5; i++)
	{
		fprintf(file, " %.1f", rAspOrb[i]);
	}
	fprintf(file, "          ; Major aspects\n-YAo 6 11   ");
	for (i = 6; i <= 11; i++)
	{
		fprintf(file, " %.1f", rAspOrb[i]);
	}
	fprintf(file, "      ; Minor aspects\n-YAo 12 18  ");
	for (i = 12; i <= 18; i++)
	{
		fprintf(file, " %.1f", rAspOrb[i]);
	}
	fprintf(file, "  ; Obscure aspects\n\n");

	// DEFAULT MAX PLANET ASPECT ORBS

	fprintf(file, "; DEFAULT MAX PLANET ASPECT ORBS:\n\n-YAm 1 10   ");
	for (i = 1; i <= 10; i++)
	{
		fprintf(file, "%4.0f", rObjOrb[i]);
	}
	fprintf(file, "\n-YAm 11 21  ");
	for (i = 11; i <= 21; i++)
	{
		fprintf(file, "%4.0f", rObjOrb[i]);
	}
	fprintf(file, "\n-YAm 22 33  ");
	for (i = 22; i <= 33; i++)
	{
		fprintf(file, "%4.0f", rObjOrb[i]);
	}
	fprintf(file, "\n-YAm 34 42  ");
	for (i = 34; i <= 42; i++)
	{
		fprintf(file, "%4.0f", rObjOrb[i]);
	}

	// DEFAULT PLANET ASPECT ORB ADDITIONS

	fprintf(file, "\n\n; DEFAULT PLANET ASPECT ORB ADDITIONS:\n\n-YAd 1 10   ");
	for (i = 1; i <= 10; i++)
	{
		fprintf(file, " %.1f", rObjAdd[i]);
	}
	fprintf(file, "\n-YAd 11 21  ");
	for (i = 11; i <= 21; i++)
	{
		fprintf(file, " %.1f", rObjAdd[i]);
	}
	fprintf(file, "\n-YAd 22 33  ");
	for (i = 22; i <= 33; i++)
	{
		fprintf(file, " %.1f", rObjAdd[i]);
	}

	// DEFAULT INFLUENCES

	fprintf(file, "\n\n\n; DEFAULT INFLUENCES:\n\n-Yj 1 10   ");
	for (i = 1; i <= 10; i++)
	{
		fprintf(file, " %.0f", rObjInf[i]);
	}
	fprintf(file, "        ; Planets\n-Yj 11 21  ");
	for (i = 11; i <= 21; i++)
	{
		fprintf(file, " %.0f", rObjInf[i]);
	}
	fprintf(file, "                  ; Minor planets\n-Yj 22 33  ");
	for (i = 22; i <= 33; i++)
	{
		fprintf(file, " %.0f", rObjInf[i]);
	}
	fprintf(file, "  ; Cusp objects\n-Yj 34 42  ");
	for (i = 34; i <= 42; i++)
	{
		fprintf(file, " %.0f", rObjInf[i]);
	}
	fprintf(file, "                    ; Uranians\n\n-YjC 1 12  ");

	for (i = 1; i <= 12; i++)
	{
		fprintf(file, " %.0f", rHouseInf[i]);
	}
	fprintf(file, "  ; Houses\n\n-YjA 1 5   ");

	for (i = 1; i <= 5; i++)
	{
		fprintf(file, "%6.3f", rAspInf[i]);
	}
	fprintf(file, "              ; Major aspects\n-YjA 6 11  ");
	for (i = 6; i <= 11; i++)
	{
		fprintf(file, "%6.3f", rAspInf[i]);
	}
	fprintf(file, "        ; Minor aspects\n-YjA 12 18 ");
	for (i = 12; i <= 18; i++)
	{
		fprintf(file, "%6.3f", rAspInf[i]);
	}
	fprintf(file, "  ; Obscure aspects\n\n");

	// DEFAULT TRANSIT INFLUENCES

	fprintf(file, "; DEFAULT TRANSIT INFLUENCES:\n\n-YjT 1 10   ");
	for (i = 1; i <= 10; i++)
	{
		fprintf(file, " %.0f", rTransitInf[i]);
	}
	fprintf(file, "  ; Planets\n-YjT 11 21  ");
	for (i = 11; i <= 21; i++)
	{
		fprintf(file, " %.0f", rTransitInf[i]);
	}
	fprintf(file, "   ; Minor planets\n-YjT 34 42  ");
	for (i = 34; i <= 42; i++)
	{
		fprintf(file, " %.0f", rTransitInf[i]);
	}

	fprintf(file, "  ; Uranians\n\n-Yj0 %.0f %.0f %.0f %.0f ",
			rObjInf[cLastMoving + 1], rObjInf[cLastMoving + 2], rHouseInf[NUMBER_OF_SIGNS + 1], rHouseInf[NUMBER_OF_SIGNS + 2]);
	fprintf(file, "; In ruling sign, exalted sign, ruling house, exalted house.\n\n\n");

	// DEFAULT COLORS

	fprintf(file, "; DEFAULT COLORS:\n\n-YkC");
	for (i = eFir; i <= eWat; i++)
	{
		fprintf(file, " %d", kElemA[i]);
	}
	fprintf(file, "                 ; Element colors\n-YkA 1 18 ");
	for (i = 1; i <= cAspect; i++)
	{
		fprintf(file, " %d", kAspA[i]);
	}
	fprintf(file, "  ; Aspect colors\n-Yk0 1 7  ");
	for (i = 1; i <= 7; i++)
	{
		fprintf(file, " %d", kRainbowA[i]);
	}
	fprintf(file, "    ; Rainbow colors\n-Yk  0 8  ");
	for (i = 0; i <= 8; i++)
	{
		fprintf(file, " %d", kMainA[i]);
	}
	fprintf(file, "  ; Main colors\n");
	fprintf(file, "-Ykc %d                         ; Corner decoration color\n\n", kCornerDecoration);

	// GRAPHICS DEFAULTS

	fprintf(file, "; GRAPHICS DEFAULTS:\n\n");
	fprintf(file, "%cX              ", ChDashF(us.fGraphics));
	fprintf(file, "; Graphics chart flag [\"_X\" is text, \"=X\" is graphics]\n");
	i = gs.xWin;
	if (fSidebar)
		i -= SIDESIZE;
	fprintf(file, ":Xw %d %d     ", i, gs.yWin);
	fprintf(file, "; Default X and Y resolution\n");
	fprintf(file, ":Xb%c            ", ChUncap(gs.chBmpMode));
	fprintf(file, "; Bitmap file type\n");
	fprintf(file, ":YXG %d       ", gs.nGlyphs);
	fprintf(file, "; Glyph selections\n");
	fprintf(file, ":YXg %d         ", gs.nGridCell);
	fprintf(file, "; Aspect grid cells\n");
	fprintf(file, ":YXf %d          ", gs.fFont);
	fprintf(file, "; Use actual fonts\n");
	fprintf(file, ":YXp %d          ", gs.nOrient);
	fprintf(file, "; PostScript paper orientation\n");
	fprintf(file, ":YXp0 %.1f %.1f  ", gs.xInch, gs.yInch);
	fprintf(file, "; PostScript paper X and Y inch sizes\n");
	fprintf(file, ":YXC %d %d %d    ", CornerDecorationType, CornerDecorationSize, CornerDecorationNumberLines);
	fprintf(file, "; Corner Decoration <0 = none, 1 = moire, 2 = web> <size> <no. lines>\n\n");


	// DIRECTORIES

	fprintf(file, "; DIRECTORIES:\n\n");
	fprintf(file, ":DIM \"%s\"                ; %s\n", dirs.main.dir, dirs.main.name);
	fprintf(file, ":DIE \"%s\"           ; %s\n", dirs.ephemeris.dir, dirs.ephemeris.name);
	fprintf(file, ":DIC \"%s\"              ; %s\n", dirs.charts.dir, dirs.charts.name);
	fprintf(file, ":DIN \"%s\"      ; %s\n", dirs.interpretation.dir, dirs.interpretation.name);
	fprintf(file, ":DIL \"%s\"       ; %s\n", dirs.miscel.dir, dirs.miscel.name);
	fprintf(file, ":DIA \"%s\"      ; %s\n", dirs.american_atlas.dir, dirs.american_atlas.name);
	fprintf(file, ":DII \"%s\" ; %s\n", dirs.international_atlas.dir, dirs.international_atlas.name);


	fprintf(file, "\n; %s\n", CONFIGURATION_FILE);
	fclose(file);
	return TRUE;
}

/* astrolog.c */
