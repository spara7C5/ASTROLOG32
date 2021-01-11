/*
** Astrolog  File: charts0.c
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
#include "Language.h"


/*
******************************************************************************
** Table Display Routines.
******************************************************************************
*/

/* A subprocedure of the credit displayed below, this prints out one line */
/* of credit information on the screen. Given a string, it's displayed    */
/* centered with left and right borders around it, in the given color.    */

void PrintW(char *sz, int col) {
	int i;

	if (!sz)
	{

		/* Null string means print the top, bottom, or a separator row. */

		if (col < 0)
			AnsiColor(kRed);
		PrintCh((byte) (col ? (col > 0 ? chSW : chNW) : chJE));
		PrintTab(chH, CREDITWIDTH);
		PrintCh((byte) (col ? (col > 0 ? chSE : chNE) : chJW));
	}
	else
	{
		i = strlen(sz);
		PrintCh(chV);
		PrintTab(' ', (CREDITWIDTH - i) / 2 + (i & 1));
		AnsiColor(col);
		PrintSz(sz);
		PrintTab(' ', (CREDITWIDTH - i) / 2);
		AnsiColor(kRed);
		PrintCh(chV);
	}
	PrintL();
}

/* Print out a command switch or keypress info line to the screen, as done  */
/* with the -H switch or 'H' key in a graphic window. This is just printing */
/* out the string, except in Ansi mode we set the proper colors: Red for    */
/* header lines, Green for individual switches or keys, and White for the   */
/* rest of the line telling what it does. We also prefix each switch with   */
/* either Unix's '-' or PC's '/', whichever is appropriate for the system.  */

void PrintS(char *sz) {
	int chDash;
	char ch;

	chDash = sz[1];
	if (*sz != ' ')
		AnsiColor(kRed);
	else if (chDash != ' ')
		AnsiColor(chDash == 'P' || sz[3] == ' ' || sz[3] == ':' ? kGreen : kDkGreen);
	else
		AnsiColor(kDefault);
	while ((ch = *sz) && ch != ':' && (chDash != 'P' || (ch != ' ' || *(sz + 1) != 't')))
	{
		if (ch != '_')
			PrintCh(ch);
		else
			PrintCh(chSwitch);
		sz++;
	}
	if (*sz)
		PrintCh(*sz++);
	AnsiColor(kDefault);

	while (ch = *sz)
	{
		if (ch != '_')
			PrintCh(ch);
		else
			PrintCh(chSwitch);
		sz++;
	}
	PrintL();
}

/* Print out a list of the various objects - planets, asteroids, house     */
/* cusps, stars - recognized by the program, and their index values. This  */
/* is displayed when the -O switch is invoked. For some objects, display   */
/* additional information, e.g. ruling signs for planets, brightnesses and */
/* positions in the sky for fixed stars, etc.                              */

void PrintObjects()
{
	char sz[cchSzDef];
	CI ci;
	int i, j, k;
	double Off;

	sprintf(sz, "\n                      PLANETS AND OBJECTS\n\n");
	PrintSz(sz);
	PrintSz("No. Name           Rule Co-Rule Fall Co-Fall Exalt Debilitate\n\n");

	for (i = 1; i <= cLastMoving; i++)
	{
		AnsiColor(kObjA[i]);
		sprintf(sz, "%3d %-15s", i, Lang(tObjName, i));
		PrintSz(sz);

		/* Print rulerships and exaltations for the planets. */

		j = ruler1[i];
		k = ruler2[i];
		if (j)
		{
			sprintf(sz, "%c%c%c", chSig3(j));
			PrintSz(sz);
		}
		else
			PrintSz("   ");
		PrintSz("  ");
		if (k)
		{
			sprintf(sz, "%c%c%c", chSig3(k));
			PrintSz(sz);
		}
		else
			PrintSz("   ");
		PrintTab(' ', 5);
		if (j)
		{
			sprintf(sz, "%c%c%c", chSig3(Mod12(j + 6)));
			PrintSz(sz);
		}
		else
			PrintSz("   ");
		PrintSz("  ");
		if (k)
		{
			sprintf(sz, "%c%c%c", chSig3(Mod12(k + 6)));
			PrintSz(sz);
		}
		else
			PrintSz("   ");
		PrintTab(' ', 5);
		j = exalt[i];
		if (j)
		{
			sprintf(sz, "%c%c%c", chSig3(j));
			PrintSz(sz);
		}
		else
			PrintSz("   ");
		PrintSz("   ");
		if (j)
		{
			sprintf(sz, "%c%c%c", chSig3(Mod12(j + 6)));
			PrintSz(sz);
		}
		else
			PrintSz("   ");
		if (FCusp(i))
		{
			sprintf(sz, "  House Cusp #%d", i - cuspLo + 1);
			PrintSz(sz);
		}
		else if (FUranian(i))
		{
			sprintf(sz, "  Uranian #%d", i - uranLo + 1);
			PrintSz(sz);
		}
		PrintL();
	}

	/* Now read in and display stars in specified order. */

	ci = ciCore;
	Off = ProcessInput(TRUE);
	ciCore = ci;
	ComputeStars(us.fSidereal ? 0.0 : -Off);

	for (i = starLo; i <= starHi; i++)
	{
		j = cLastMoving + starname[i - cLastMoving];
		AnsiColor(kObjA[j]);
		sprintf(sz, "%3d %-15s", i, Lang(tObjName, j));
		PrintSz(sz);
		sprintf(sz, "Star #%2d   ", i - cLastMoving);
		PrintSz(sz);
		PrintZodiac(cp0.longitude[j]);
		PrintTab(' ', 4);
		PrintAltitude(cp0.latitude[j]);
		sprintf(sz, " %5.2f\n", rStarBright[j - cLastMoving]);
		PrintSz(sz);
	}
	AnsiColor(kDefault);
}


/* Print out a list of all the aspects recognized by the program, and info  */
/* about them: their names, index numbers, degree angles, present orbs, and */
/* the description of their glyph. This gets displayed when the -A switch   */
/* is invoked (without any argument).                                       */

void PrintAspects(void)
{
	char sz[cchSzDef];
	int i;

	sprintf(sz, "Aspects:\n\nNo. Name         Abbrev. ");
	PrintSz(sz);
	PrintSz("Angle    Orb          Description of glyph\n\n");
	for (i = 1; i <= cAspect; i++)
	{
		AnsiColor(kAspA[i]);
		sprintf(sz, "%2d %-15s(%s) %6.2f +/- %1.0f degrees - %s\n",
				i, Lang(tAspectName, i), Lang(tAspectAbbrev, i), rAspAngle[i], 
				rAspOrb[i], Lang(tAspectGlyph, i));
		PrintSz(sz);
	}
	AnsiColor(kDefault);
}


/* Print out a list of the 12 signs and houses of the zodiac, and their    */
/* standard and traditional names, as done when the -H0 switch is invoked. */

void PrintSigns()
{
	char sz[cchSzDef];
	int i;

	sprintf(sz, "Signs and houses:\n\n");
	PrintSz(sz);
	PrintSz("Sign        English name      House Traditional name");
	PrintTab(' ', 19);
	PrintSz("Ruler\n\n");
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		AnsiColor(kSignA(i));
		sprintf(sz, "%-15s%-14s%2d%s  %-33s  %s\n",
				Lang(tSignName,i), Lang(tSignMeaning, i), i, Lang(tSuffix, i), Lang(tHouseMeaning, i), Lang(tObjName, rules[i]));
		PrintSz(sz);
	}
	AnsiColor(kDefault);
}


#ifdef CONSTEL
/* Given the standard 'noun' form of a constellation string, convert it  */
/* to its genitive or posessive form. Some standard rules are used but a */
/* special instructions string is passed for special cases.              */

char *GetSzGenitive(char *szGen, char *szInst) {
	char *pch, ch1, ch2;
	int cch;

	pch = szGen + strlen(szGen);
	if (*szInst == ' ')			/* Instructions starting with a space or */
		szInst++;				/* that are empty means no special case. */
	else if (*szInst)
	{
		cch = *szInst - '0';
		if (cch < 10)
		{						/* Instructions starting with a number means */
			szInst++;			/* hack off that many characters of string.  */
			pch -= cch;
		}
		while (*szInst > '9')	/* Instructions end with characters to append. */
			*pch++ = *szInst++;
		*pch = chNull;
		return szInst;
	}
	ch1 = *(pch - 1);
	ch2 = *(pch - 2);
	if (ch1 == 'a')
	{							/* Standard rule: 'a' ending -> 'ae'. */
		*pch++ = 'e';
		*pch = chNull;
	}
	else if (ch1 == 's' && ch2 == 'u')
	{							/* 'us' ending -> 'i'. */
		*(pch - 2) = 'i';
		*(pch - 1) = chNull;
	}
	else if (ch1 == 'm' && ch2 == 'u')
	{							/* 'um' ending -> 'i'. */
		*(pch - 2) = 'i';
		*(pch - 1) = chNull;
	}
	else if (ch1 == 'x')		/* Standard rule: 'x' ending -> 'cis'. */
		sprintf(pch - 1, "cis");
	return szInst;
}


/* Given a constellation index, fill out a string with the genitive or   */
/* posessive form of its name. This basically just calls GetSzGenitive() */
/* above, however it has to be called twice for names having two words.  */

void GetSzConstel(char *szGen, int i) {
	char sz1[cchSzDef], sz2[cchSzDef], *pchSpace, *szInst;

	sprintf(szGen, "%s", szCnstlName[i]);
	for (pchSpace = szGen; *pchSpace && *pchSpace != ' '; pchSpace++)
		;
	szInst = (char*)szCnstlGenitive[i];
	if (*pchSpace == chNull)
	{
		GetSzGenitive(szGen, szInst);
		return;
	}
	*pchSpace = chNull;
	if (szInst[0] == '!')
	{
		GetSzGenitive(szGen, szInst + 1);
		return;
	}
	sprintf(sz1, "%s", szGen);
	sprintf(sz2, "%s", pchSpace + 1);
	szInst = GetSzGenitive(sz1, szInst);
	GetSzGenitive(sz2, szInst);
	sprintf(szGen, "%s %s", sz1, sz2);
}


/* Print out a list of the 88 constellations used in astronomy, in their  */
/* standard, English, and genitive forms, as invoked with the -HF switch. */

void PrintConstellations()
{
	int i, j = eWat;
	char szGen[cchSzDef], sz[cchSzDef], chLast = chNull;

	sprintf(sz, "Constellations:\n\n");
	PrintSz(sz);
	PrintSz("No. Name                Abbrev.   ");
	PrintSz("Meaning            Genitive form\n\n");
	for (i = 1; i <= cCnstl; i++)
	{
		if (szCnstlName[i][0] != chLast)
		{
			chLast = szCnstlName[i][0];
			j = j + 1 & 3;
			AnsiColor(kElemA[j]);
		}
		sprintf(sz, "%2d: %-19s (%s) ", i, szCnstlName[i], szCnstlAbbrev[i]);
		PrintSz(sz);
		if (szCnstlMeaning[i][0] == ' ')
			sprintf(sz, "%-22s", szCnstlMeaning[i] + 1);
		else
			sprintf(sz, "the %-18s", szCnstlMeaning[i]);
		PrintSz(sz);
		GetSzConstel(szGen, i);
		sprintf(sz, " (%s)\n", szGen);
		PrintSz(sz);
	}
	AnsiColor(kDefault);
}
#endif /* CONSTEL */


/* Print out a list of the planets in the solar system (and our Moon), */
/* listing astronomical info on them, as invoked with the -HS switch.  */

void PrintOrbit()
{
	int i;
	double r;

	PrintTab(' ', 28); 
	PrintSz("PLANETS INFORMATION\n\n");
	PrintSz("   Name: Distance   Year Diameter     Day       Mass Density  Axis Satellites\n\n");

	for (i = 0; i <= oVesta; i++)
	{
		AnsiColor(kObjA[i]);
		PrintSz("%7s:%9.3f%7.2f%9.3f", Lang(tObjShortName, i),
				rObjDist[i] / rObjDist[0], rObjYear[i], rObjDiam[i] / rObjDiam[0]);

		if (i <= oPlu)
		{
			r = rObjDiam[i] * 1000.0 / 2.0;
			PrintSz("%8.2f%11.3f%8.2f%6.2f%11d",
				rObjDay[i], rObjMass[i],
				(rObjMass[i] * 5.974E24 / 1000.0) / ((4.0 / 3.0) * (r * r * r) * rPi), 
				rObjAxis[i], cSatellite[i]);
		}

		PrintL();
	}

	AnsiColor(kDefault);
}

/* Print out the contents of a chart structure */

static void PrintChartInfoSingle(char *title, CI *ci)
{
	PrintSz("\n------- %s -------\n\n", title);

	PrintSz("%-13s: %s\n", "Name", ci->nam);
	PrintSz("%-13s: %s\n", "Location", ci->loc);
	PrintSz("%-13s: %s\n", "Coordinates", SzLocation(ci->lon, ci->lat));
	PrintSz("%-13s: %f %f\n", "Long/Latitude", ci->lon, ci->lat);
	PrintSz("%-13s: %s\n", "Date", SzDate(ci->mon, ci->day, ci->yea, 1));
	PrintSz("%-13s: %s\n", "Time", SzTim(ci->tim));
	PrintSz("%-13s: %.2f\n", "Time Zone", ci->zon);
	PrintSz("%-13s: %.2f\n", "DST", ci->dst);
}

static void PrintHouses(CI *chart)
{
	double		jd, cusps[13], ascmc[10];
	int			i, house_sys;


	// calculate julian time
	jd = MdytszToJulian(chart->mon, chart->day, chart->yea, chart->tim, chart->dst, chart->zon)
		- 0.5;

	house_sys = AstrologHouse2SeHouse(us.nHouseSystem);
	if(house_sys < 0)
	{
		PrintSz("Invalid house system");
		return;
	}

	if(ERR == swe_houses(jd, DegMin2DecDeg(chart->lat), -DegMin2DecDeg(chart->lon), house_sys, cusps, ascmc))
	{
		PrintSz("Error in swe_houses");
		return;
	}

	PrintSz("\nVertex = %f\nEast Point = %f\nAscendant = %f\nMideheaven = %f\n", 
		ascmc[SE_VERTEX], ascmc[SE_EQUASC], ascmc[SE_ASC], ascmc[SE_MC]);

	for (i = 1; i <= 12; i++)
	{
		PrintSz("House %d: %f\n", i, cusps[i]);
	}
}

 void PrintProgramStatus(void)
{
	double jd = JulianDayFromTime(is.T);
	double aya = swe_get_ayanamsa(jd + swe_deltat(jd));

	PrintSz("\nCHART INFORMATION\n");

	PrintChartInfoSingle("Core Chart", &ciCore);
	PrintChartInfoSingle("Main Chart", &ciMain);
	PrintChartInfoSingle("Second Chart", &ciTwin);
	PrintChartInfoSingle("Third Chart", &ciThre);
	PrintChartInfoSingle("Fourth Chart", &ciFour);
	PrintChartInfoSingle("Transits and Progressions Chart", &ciTran);
	PrintChartInfoSingle("Saved Chart", &ciSave);

	PrintSz("\nMISCELLANEOUS INFORMATION\n\n");

	PrintSz("Julian Time (is.T): %f\n", is.T);
	PrintSz("%s %11.4f\n", Lang(sJULIAN_DAY), jd);
	PrintSz("Ayanamsha (is.rSid): %f\n", -is.rSid);
	PrintSz("Swiss Ephemeris Ayanamsha: %f\n", aya);
	PrintSz("Sidereal Mode: %d\n", us.nSiderealMode);
	PrintSz("Sidereal Correction: %f\n", us.rSiderealCorrection);
	PrintSz("PolarMCflip: %d\n", PolarMCflip);

	PrintSz("DegMin2DecDeg 39.5 = %f\n", DegMin2DecDeg(39.5));
	PrintSz("DegMin2DecDeg 39.5 = %f\n", DegMin2DecDeg(39.5));

	PrintHouses(&ciCore);
}


#ifdef ARABIC
/* Compare the strings corresponding to two Arabic part formulas. Like   */
/* NCompareSz, this returns 0 if they are equal, a positive value if the */
/* first is greater, and negative if the second is greater.              */

int NCompareSzPart(int ap1, int ap2) {
	char *pch1, *pch2;
	int ich;

	pch1 = ai[ap1].form;
	pch2 = ai[ap2].form;
	for (ich = 0; pch1[ich] && pch1[ich] == pch2[ich]; ich++)
	{
		if (!us.fArabicFlip)
		{

			/* If formulas are being displayed in alternate form, we need to */
			/* effectively swap two sections in the string and then compare. */

			if (ich == 2)
				ich = 5;
			else if (ich == 8)
				ich = 2;
			else if (ich == 5)
				ich = 8;
		}
	}
	return pch1[ich] - pch2[ich];
}


/* Print out a list of all the Arabic parts in the current chart, computing */
/* their positions first, as brought up with the -P switch.                 */

void DisplayArabic()
{
	double rPart[cPart], rBit[3], rCur, lDis, mDis;
	char sz[cchSzDef], *pch, ch;
	int iPart[cPart], h, i, j, k, l, ll, lll, nn;

	PrintSz("\n");
	PrintTab(' ', 30);
	PrintSz(Lang(sArabicHdr));
	PrintTab(' ', 1 + 4 * is.fSeconds);
	PrintSz(Lang(sArabicHdr2));

	/* Calculate the zodiac positions of all the parts. */

	for (i = 0; i < cPart; i++)
	{
		rPart[i] = -360.0;
		if (i >= us.nArabicParts)
			goto LNextPart;
		for (j = 0; j < 3; j++)
		{
			pch = &ai[i].form[j * 3];
			ch = pch[1];
			if (ch == ' ')
				k = oAsc;
			else if (ch == 'F')
				k = -apFor;
			else if (ch == 'S')
				k = -apSpi;
			else
				k = (ch - '0') * 10 + (pch[2] - '0');
			ch = *pch;
			if (ch == 'h')		/* House cusp */
				rCur = cp0.cusp_pos[k];
			else if (ch == 'r')	/* Ruler of house cusp */
				rCur = cp0.longitude[rules[Z2Sign(cp0.cusp_pos[k])]];
			else if (ch == 'j')	/* 10 degrees of house cusp */
				rCur = cp0.cusp_pos[k] + 10.0;
			else if (ch == 'H')	/* Planet's house */
				rCur = cp0.cusp_pos[cp0.house_no[k]];
			else if (ch == 'R')	/* Ruler of planet's house */
				rCur = cp0.longitude[rules[Z2Sign(cp0.cusp_pos[cp0.house_no[k]])]];
			else if (ch == 'D')	/* Dispositor / ruler of planet's position */
				rCur = cp0.longitude[rules[Z2Sign(cp0.longitude[k])]];
			else if (FBetween(ch, '0', '3'))
				rCur = (double) ((ch - '0') * 100 + k);
			else
			{
				if (k < 1)
				{
					rCur = rPart[-k];
					if (rCur < 0.0)
						goto LNextPart;
				}
				else
				{
					if (ignore[k] && !FCusp(k))
						goto LNextPart;
					else
						rCur = cp0.longitude[k];
				}
			}
			rBit[j] = rCur;
		}
		rCur = rBit[1] - rBit[2];
		if (us.nArabicNight < 0 || (ai[i].form[9] == 'F' && cp0.house_no[oSun] < sLib && us.nArabicNight == 0))
			rCur = -rCur;
		rCur = Mod(rCur + rBit[0]);
		rPart[i] = rCur;
	  LNextPart:
		iPart[i] = i;
	}

	/* Sort parts to figure out what order to display them in. */

	if (us.nArabic > 1)
	{
		for (i = 1; i < cPart; i++)
		{
			j = i - 1;

			/* Compare part zodiac locations for -Pz switch. */

			if (us.nArabic == 'z')
				while (j >= 0 && rPart[iPart[j]] > rPart[iPart[j + 1]])
				{
					SwapN(iPart[j], iPart[j + 1]);
					j--;

					/* Compare part names for -Pn switch. */

				}
			else if (us.nArabic == 'n')
				while (j >= 0 && NCompareSz(ai[iPart[j]].name, ai[iPart[j + 1]].name) > 0)
				{
					SwapN(iPart[j], iPart[j + 1]);
					j--;

					/* Compare part formulas for -Pf switch. */

				}
			else if (us.nArabic == 'f')
				while (j >= 0 && NCompareSzPart(iPart[j], iPart[j + 1]) > 0)
				{
					SwapN(iPart[j], iPart[j + 1]);
					j--;
				}
		}
	}

	/* Display the positions and formulas of the parts. */

	for (h = i = 0; i < cPart; i++)
	{
		l = iPart[i];
		if (rPart[l] < 0.0)
			continue;

		lll = 0;
		if (PartAspects)
		{
			for (ll = 1; ll < cObj; ll++)
			{
				if (!ignore[ll])
				{
					lDis = MinDistance(rPart[l], cp0.longitude[ll]);
					for (nn = 1; nn <= PartAspects; nn++)
					{
						mDis = lDis - rAspAngle[nn];
						if (fabs(mDis) < PartOrb)
						{
							lll = 1;
							continue;
						}
					}
				}
			}
		}
		else
			lll = 1;

		if (!lll)
			continue;

		sprintf(sz, "%3d: %23.23s ", ++h, Lang(tArabicPart, l));
		PrintSz(sz);
		PrintZodiac(rPart[l]);
		j = HousePlaceIn(rPart[l]);
		sprintf(sz, " [%2d%s] ", j, Lang(tSuffix, j));
		AnsiColor(kSignA(j));
		PrintSz(sz);
		AnsiColor(kDefault);
		PrintCh('(');
		for (j = 0; j < 3; j++)
		{
			k = j < 1 || us.fArabicFlip ? j : 3 - j;
			pch = &ai[l].form[k * 3];
			ch = pch[1];
			if (ch == ' ')
				k = oAsc;
			else if (ch == 'F')
				k = -apFor;
			else if (ch == 'S')
				k = -apSpi;
			else
				k = (ch - '0') * 10 + (pch[2] - '0');
			ch = *pch;
			if (k < 1)
			{
				AnsiColor(kObjA[oFor]);
				sprintf(sz, "%3.3s", ai[-k].name);
				PrintSz(sz);
			}
			else
			{
				if (ch == ' ' || ch == 'H' || ch == 'R' || ch == 'D')
				{
					AnsiColor(kSignA(ruler1[k]));
					sprintf(sz, "%c%c%c", chObj3(k));
				}
				else if (FBetween(ch, '0', '3'))
				{
					k = (ch - '0') * 100 + k;
					AnsiColor(kSignA(k / 30 + 1));
					sprintf(sz, "%2d%c%c%c", k % 30, chSig3(k / 30 + 1));
				}
				else
				{
					AnsiColor(kSignA(k));
					sprintf(sz, "%3d", k);
				}
				PrintSz(sz);
			}
			AnsiColor(kDefault);
			if (sz[3] == chNull)
			{
				PrintCh(' ');
				switch (ch)
				{
				case 'h':
					ch = ' ';
					break;
				case 'r':
					ch = 'R';
					break;
				case 'j':
					ch = '&';
					break;
				default:
					ch = *pch;
				}
				PrintCh(ch);
			}
			if (j < 2)
			{
				sprintf(sz, " %c ", (j < 1 == us.fArabicFlip) ? '+' : '-');
				PrintSz(sz);
			}
		}
		PrintCh(' ');
		ch = ai[l].form[9];
		switch (ch)
		{
		case 'F':
			PrintSz("Y");
			break;
		default:
			AnsiColor(kWhite);
			PrintSz("N");
			AnsiColor(kDefault);
			break;
		}
		PrintSz(") ");
		ch = ai[l].form[10];
		switch (ch)
		{
		case 'C':
			AnsiColor(kElemA[eWat]);
			PrintSz("Comm.");
			break;
		case 'E':
			AnsiColor(kElemA[eFir]);
			PrintSz("Event");
			break;
		case 'H':
			AnsiColor(kElemA[eEar]);
			PrintSz("Hora.");
			break;
		}
		AnsiColor(kDefault);
		PrintL();


		if (PartAspectsShow)
		{
			for (ll = 1; ll < cObj; ll++)
			{
				if (!ignore[ll])
				{
					lDis = MinDistance(cp0.longitude[ll], rPart[l]);
					for (nn = 1; nn <= PartAspectsShow; nn++)
					{
						if (ll == oFor && nn == 1 && l == 0)
							continue;
						mDis = lDis - rAspAngle[nn];
						if (fabs(mDis) < PartOrb)
						{
							lll = (int) (60 * mDis);
							PrintTab(' ', 28);
							sprintf(sz, " %s  %-4.4s", Lang(tAspectAbbrev, nn), Lang(tObjShortName, ll));
							PrintSz(sz);
							sprintf(sz, "   orb: %c%d%c%02d'\n",
									(mDis < 0 ? '-' : '+'), abs(lll) / 60, chDeg1, abs(lll) % 60);
							PrintSz(sz);
						}
					}
				}
			}
		}
	}
}
#endif /* ARABIC */

/* This is the dispatch procedure for all the generic table information      */
/* routines, such as those displaying the -H switch list, the list of signs, */
/* objects, default interpretations, and so on not requiring a date or time. */

int FPrintTables(void)
{
	if (us.fGraphics)
		return FALSE;

	if (us.fSwitch)
	{
		if (is.fMult)
			PrintL2();
		DisplaySwitches();
		is.fMult = TRUE;
	}
	if (us.fSwitchRare)
	{
		if (is.fMult)
			PrintL2();
		DisplaySwitchesRare();
		is.fMult = TRUE;
	}
	if (us.fKeyGraph)
	{
		if (is.fMult)
			PrintL2();
		DisplayKeysX();
		is.fMult = TRUE;
	}
	if (us.fPrintProgramStatus)
	{
		if (is.fMult)
			PrintL2();
		PrintProgramStatus();
		is.fMult = TRUE;
	}
	if (us.fSign)
	{
		if (is.fMult)
			PrintL2();
		PrintSigns();
		is.fMult = TRUE;
	}
	if (us.fObject)
	{
		if (is.fMult)
			PrintL2();
		PrintObjects();
		is.fMult = TRUE;
	}
	if (us.fAspect)
	{
		if (is.fMult)
			PrintL2();
		PrintAspects();
		is.fMult = TRUE;
	}
#ifdef CONSTEL
	if (us.fConstel)
	{
		if (is.fMult)
			PrintL2();
		PrintConstellations();
		is.fMult = TRUE;
	}
#endif
	if (us.fOrbitData)
	{
		if (is.fMult)
			PrintL2();
		PrintOrbit();
		is.fMult = TRUE;
	}
#ifdef INTERPRET
	if (us.fMeaning)
	{
		if (is.fMult)
			PrintL2();
		InterpretGeneral();
		InterpretAspectGeneral();
		is.fMult = TRUE;
	}
#endif

	/* If we also already have enough information to generate a chart,    */
	/* then go on and do so, else exit. (So things like "-H -i file" will */
	/* work, but things like just "-H" will print and exit right away.)   */

	return is.fMult;
}

/* charts0.c */
