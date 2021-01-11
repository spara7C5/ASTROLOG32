/*
** Astrolog File: intrpalt.c
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


/***********************************************************************/
/* This module is created by Valentin Abramov (valja@tartu.customs.ee) */
/* and includes routines for alternative interpretation of Astrolog    */
/* natal charts and natal-transit comparison charts.                   */
/***********************************************************************/

#include "astrolog.h"

#ifdef INTERPRET
#ifdef INTERPRETALT

#include <string.h>

char *szHouseFile(int);
char *szPlanetFile(int, BOOL);
char *szAspectFile(int, BOOL);
void FieldWordI(char *);
BOOL FProcessInterpretFile(char *szFile, int j, int k);

byte SaveStars[NUMBER_OBJECTS] = { 0,
	0, 1, 0, 0, 0, 0, 0, 0, 0, 0,	/* Planets  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* Minors   */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Cusps    */
	0, 0, 0, 0, 0, 0, 0, 0, 0,	/* Uranians */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Stars    */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};


/* Three simple routines, which builds names of interpretation files. */
/* That saves memory, wich would be needed to describe hundreds file  */
/* names. There are four different files for every planet (position   */
/* and aspects, same for transits), also house files.                 */

char *szHouseFile(int i)
{
	static char szFileName[16];
	char szFileNumber[8];

	sprintf(szFileNumber, "%i", i);
	sprintf(szFileName, "house");
	strcat(szFileName, szFileNumber);
	strcat(szFileName, ".dat");
	return szFileName;
}


char *szPlanetFile(int i, BOOL trflag)
{
	static char szFileName[16];
	char szFileNumber[8];

	sprintf(szFileNumber, "%i", i);
	if (i < 10)
		sprintf(szFileName, "pl0");
	else
		sprintf(szFileName, "pl");
	strcat(szFileName, szFileNumber);
	if (trflag)
		strcat(szFileName, "t.dat");
	else
		strcat(szFileName, ".dat");
	return szFileName;
}

char *szAspectFile(int i, BOOL trflag)
{
	static char szFileName[16];
	char szFileNumber[8];

	sprintf(szFileNumber, "%i", i);
	if (i < 10)
		sprintf(szFileName, "pl0");
	else
		sprintf(szFileName, "pl");
	strcat(szFileName, szFileNumber);
	if (trflag)
		strcat(szFileName, "ta.dat");
	else
		strcat(szFileName, "a.dat");
	return szFileName;
}

void FieldWordI(char *sz) {
	static char line[cchSzMax];
	static int cursor = 0;
	int i, j;

	if (sz == NULL)
	{
		line[cursor] = 0;
		PrintSz(line);
		PrintL();
		cursor = 0;
		return;
	}
	if (cursor)
		line[cursor++] = ' ';
	for (i = 0; (line[cursor] = sz[i]); i++, cursor++)
		;

	while (cursor >= us.nScreenWidth - 1)
	{
		for (i = us.nScreenWidth - 1; line[i] != ' '; i--)
			;
		line[i] = 0;
		PrintSz(line);
		PrintL();
		for (j = 0; (line[j] = line[i + j + 1]) != 0; j++)
			;
		cursor -= (i + 1);
	}
}

/* This is generic file processing routine, which handles chart          */
/* interpretation files. Given a file name or a file handle and two      */
/* indexes of necessary interpretation part, run through interpretation  */
/* data file and print out current part of interpretation                */

BOOL FProcessInterpretFile(char *szFile, int j, int k)
{
	char			szLine[cchSzMax], ch = 0, chLocal[3];
	static BOOL		warning_invalid_given = FALSE;
	int				file_type = 0, file_version = 0;
	int				i, iind;
	FILE			*file;


	if(warning_invalid_given)
	{
		// if there was one invalid file, there could be more than one, so
		// don't even try to open it, to avoid a cascade of warnings
		return FALSE;
	}

	file = FileOpen(szFile, dirs.interpretation.dir, FALSE);

	if (file == NULL)
	{
		return FALSE;
	}

	if(!CheckFileTypeAndVersion(file, szFile, FILE_TYPE_ALT_INTERPRET, 
		FILE_VERSION_ALT_INTERPRET))
	{
		warning_invalid_given = TRUE;
		fclose(file);	// failure - close file
		return FALSE;
	}

	// add terminating NULL to string
	chLocal[2] = 0;

	/* Look for indexes of interpretation we need.  */
	/* Every pair of indexes begins with '&'        */

	for(;;)
	{
		while (!feof(file) && (ch = (char) getc(file)) != '&')
			;
		if (feof(file))
		{
			fclose(file);
			return TRUE;
		}
		chLocal[0] = (char) getc(file);
		chLocal[1] = (char) getc(file);
		iind = atoi(chLocal);
		if (iind == j)
		{						/* Found 1st index */
			while (!feof(file) && (ch = (char) getc(file)) <= ' ')
				;
			if (feof(file))
			{
				fclose(file);
				return TRUE;
			}
			chLocal[0] = ch;
			chLocal[1] = (char) getc(file);
			iind = atoi(chLocal);
			if (iind == k)
			{					/* Found 2nd index */
				while (!feof(file) && (ch = (char) getc(file)) < ' ')
					;
				if (feof(file) || (ch == '&'))
				{
					fclose(file);
					return TRUE;
				}
				ungetc(ch, file);
				break;
			}
		}
	}

	/* Print out current inpretation part up to index of next part  "&"  */

	for(;;)
	{
		while (!feof(file) && (ch = (char) getc(file)) <= '\n')
			;
		if (feof(file))
		{
			FieldWordI(NULL);
			break;
		}
		for (szLine[0] = ch, i = 1; i < cchSzMax && !feof(file) &&
			 (szLine[i] = (char) getc(file)) > '\n' && szLine[i] != '&'; i++)
			;
		if (szLine[0] == '&')
		{
			szLine[0] = chNull;
			FieldWordI(NULL);
			break;
		}
		if (szLine[i] == '&')
		{
			szLine[i] = chNull;
			FieldWordI(szLine);
			FieldWordI(NULL);
			break;
		}
		if (szLine[i] <= '\n')
		{
			szLine[i] = chNull;
			FieldWordI(szLine);
		}
	}
	fclose(file);
	return TRUE;
}


/*
***************************************************************************
** Natal Chart interpretation routine
***************************************************************************
*/

void NatalInterpret()
{
	char sz[cchSzDef], c;
	int i, j, k, l, m, n, jj, kk, SaveStarFlag, DispSS, DispSH;
	double ip, jp, p;

	SaveStarFlag = us.nStar;
	for (i = starLo; i <= starHi; i++)
		SaveStars[i] = ignore[i];

	if (!us.fInfluence)
	{
		NoPrint = 1;
		ChartInfluence();
		NoPrint = 0;
	}

	us.fAppSep = TRUE;
	us.nStar = SaveStarFlag;
	if (!FCreateGrid(FALSE))
		return;

	SaveStarFlag = us.nStar;
	us.nStar = FALSE;

/*****************************************************************************/
/* PART 1: Planet's positions and aspects.                                   */
/* For every object print position and calculated powers: position and total */
/* and meaning of current object, aspects of current object with others and  */
/* interpretataion, also interpretation of object in sign and House.         */
/*****************************************************************************/

	if (InterpretAlt != 4 && InterpretAlt != 6)
	{
		sprintf(sz, "   ****** PLANETS AND ASPECTS ******");
		PrintSz(sz);
		PrintL2();

		for (i = 1; i <= cLastMoving; i++)
		{
			if (ignore[i])
				continue;

			AnsiColor(kObjA[i]);
			l = Z2Sign(cp0.longitude[i]);
			c = Dignify(i, l);
			sprintf(sz, "%s%s in %s", cp0.vel_longitude[i] < 0.0 ? "Retrograde " : "",
					Lang(tObjName, i), Lang(tSignName, l));
			PrintSz(sz);
			if (i < cuspLo || i > cuspHi)
			{
				sprintf(sz, " and %d%s House.", cp0.house_no[i], Lang(tSuffix, cp0.house_no[i]));
				PrintSz(sz);
			}
			PrintL();
			sprintf(sz, "Powers -   position:%6.2f   total:%6.2f", ppower1[i], ppowerTotal[i]);
			PrintSz(sz);
			PrintL();

			/*        PRINT OUT description of planet 'i'   */
			if (InterpretAlt != 1)
			{
				AnsiColor(kDefault);
				FProcessInterpretFile(szPlanetFile(i, FALSE), 0, 0);
			}

			/* Print aspects of planets with orb and aspect power      */

			for (j = 1; j <= cLastMoving; j++)
			{
				if (ignore[j] || i == j)
					continue;
				if (i > j)
				{
					k = grid->n[j][i];
					m = grid->v[j][i];
				}
				else
				{
					k = grid->n[i][j];
					m = grid->v[i][j];
				}
				if (k < 1 || k > cAspectInt)
					continue;

				ip = i <= cLastMoving ? ppower1[i] : 2.5;
				jp = j <= cLastMoving ? ppower1[j] : 2.5;
				p = (rAspInf[k] * (sqrt) (ip * jp) * (1.0 - fabs((double) (m)) / 60.0 / GetOrb(i, j, k)) * 500.0);

				PrintAspect(i, Z2Sign(cp0.longitude[i]), (int) RSgn(cp0.vel_longitude[i]), k, j, 
					Z2Sign(cp0.longitude[j]), (int) RSgn(cp0.vel_longitude[j]), 'a', 7, 10);
				sprintf(sz, " - orb: %c%d%c%02d'",
						us.fAppSep ? (m < 0 ? 'a' : 's') : (m < 0 ? '-' : '+'), abs(m) / 60, chDeg1, abs(m) % 60);
				PrintSz(sz);
				AnsiColor(kDkGreen);
				sprintf(sz, " - power:%6.2f\n", p / 500.0);
				PrintSz(sz);

				if (InterpretAlt != 1)
				{
					AnsiColor(kDefault);
					/*      PRINT INTERPRETATION OF CURRENT ASPECT           */
					/*      first planet "i", second one "j",  aspect "k"    */

					if (i < j)
						FProcessInterpretFile(szAspectFile(i, FALSE), j, k);
					else
						FProcessInterpretFile(szAspectFile(j, FALSE), i, k);
				}
			}
			if (InterpretAlt != 1)
			{
				AnsiColor(kSignA(l));
				sprintf(sz, "%s%s in %s:\n",
						cp0.vel_longitude[i] < 0.0 ? "Retrograde " : "",
						Lang(tObjName, i), Lang(tSignName, l));
				PrintSz(sz);
				/*     PRINT INTERPRETATION OF PLANET IN SIGN */
				/*     planet "i", sign "l"                   */
				AnsiColor(kDefault);
				FProcessInterpretFile(szPlanetFile(i, FALSE), 1, l);
			}

			if (i < cuspLo || i > cuspHi)
			{
				if (InterpretAlt != 1)
				{
					AnsiColor(kSignA(cp0.house_no[i]));
					sprintf(sz, "%s%s in %d%s House:\n", 
						cp0.vel_longitude[i] < 0.0 ? "Retrograde " : "", Lang(tObjName, i), cp0.house_no[i], 
						Lang(tSuffix, cp0.house_no[i]));
					PrintSz(sz);
					/*     PRINT INTERPRETATION OF PLANET IN HOUSE */
					/*     planet "i" , house "inhouse[i]"         */

					AnsiColor(kDefault);
					FProcessInterpretFile(szPlanetFile(i, FALSE), 2, cp0.house_no[i]);
				}
			}
			PrintL();
		}
		PrintL();
	}

/**************************************************************************/
/*  PART 2: Houses                                                        */
/*  For every house print out relative power, meaning of house, position  */
/*  of house cusp with interpretation, position of situative dispositor   */
/*  in sign in house with interpretation, also position of natural        */
/*  dispositor with interpretations.                                      */
/**************************************************************************/

	if (InterpretAlt != 3)
	{

		AnsiColor(kWhite);
		sprintf(sz, "   ****** HOUSES ******");
		PrintSz(sz);
		PrintL2();

		for (n = 1; n <= NUMBER_OF_SIGNS; n++)
		{
			AnsiColor(kSignA(n));
			sprintf(sz, "%d%s House:\n", n, Lang(tSuffix, n));
			PrintSz(sz);

			if (InterpretAlt >= 2)
			{
				AnsiColor(kDefault);
				FProcessInterpretFile(szHouseFile(n), 0, 0);
			}

			l = Z2Sign(cp0.cusp_pos[n]);
			AnsiColor(kSignA(l));
			sprintf(sz, "House Cusp. in %s\n", Lang(tSignName, l));
			PrintSz(sz);

			m = rules[Z2Sign(cp0.cusp_pos[n])];
			AnsiColor(kObjA[m]);
			if (m == (int) rules[n])
				sprintf(sz, "Natural and Situative Dispositor %s, total power %6.2f:\n", 
				Lang(tObjName, m), ppowerTotal[m]);
			else
				sprintf(sz, "Situative Dispositor %s, total power %6.2f:\n", Lang(tObjName, m), ppowerTotal[m]);
			PrintSz(sz);

			if (InterpretAlt >= 2)
			{
				AnsiColor(kDefault);
				FProcessInterpretFile(szHouseFile(n), 1, l);
			}

			/* Meanings of placement of dispositors in signs and houses.  */

			if ((InterpretAlt == 1) || (InterpretAlt >= 5))
			{
				l = Z2Sign(cp0.longitude[m]);
				DispSS = l;
				AnsiColor(kSignA(l));
				sprintf(sz, "%d%s House dispositor in %s:\n", n, Lang(tSuffix, n), Lang(tSignName, l));
				PrintSz(sz);

				if (InterpretAlt != 1)
				{
					AnsiColor(kDefault);
					FProcessInterpretFile(szHouseFile(n), 2, l);
				}
			}

			if ((InterpretAlt == 1) || (InterpretAlt >= 5))
			{
				l = cp0.house_no[m];
				DispSH = l;
				AnsiColor(kSignA(l));
				sprintf(sz, "%d%s House dispositor in %d%s House:\n", n, Lang(tSuffix, n), l, Lang(tSuffix, l));
				PrintSz(sz);

				if (InterpretAlt != 1)
				{
					AnsiColor(kDefault);
					FProcessInterpretFile(szHouseFile(n), 3, l);
				}
			}

			if (m != (int) rules[n])
			{
				AnsiColor(kObjA[m]);
				sprintf(sz, "Natural Dispositor %s, total power %6.2f\n", 
					Lang(tObjName, rules[n]), ppowerTotal[rules[n]]);
				PrintSz(sz);


				if ((InterpretAlt == 1) || (InterpretAlt >= 5))
				{
					l = Z2Sign(cp0.longitude[rules[n]]);
					AnsiColor(kSignA(l));
					sprintf(sz, "%d%s House dispositor in %s:\n", n, Lang(tSuffix, n), Lang(tSignName, l));
					PrintSz(sz);
				}
				if (InterpretAlt >= 5)
				{
					AnsiColor(kDefault);
					if (l == DispSS)
						FProcessInterpretFile(szHouseFile(n), 0, 1);
					else
						FProcessInterpretFile(szHouseFile(n), 2, l);
				}

				if ((InterpretAlt == 1) || (InterpretAlt >= 5))
				{
					l = cp0.house_no[rules[n]];
					AnsiColor(kSignA(l));
					sprintf(sz, "%d%s House dispositor in %d%s House:\n", n, Lang(tSuffix, n), l, Lang(tSuffix, l));
					PrintSz(sz);
				}
				if (InterpretAlt >= 5)
				{
					AnsiColor(kDefault);
					if (l == DispSH)
						FProcessInterpretFile(szHouseFile(n), 0, 1);
					else
						FProcessInterpretFile(szHouseFile(n), 3, l);
				}
			}

			for (i = 1; i <= cLastMoving; i++)
			{
				if (ignore[i] || FCusp(i))
					continue;
				if ((int) (cp0.house_no[i]) == n)
				{
					AnsiColor(kObjA[i]);
					sprintf(sz, "In %d%s House %s, total power %6.2f:\n", n, Lang(tSuffix, n), 
						Lang(tObjName, i), ppowerTotal[i]);
					PrintSz(sz);
					/*  INTERPRETATION of planet (i) in house (n)            */

					if ((InterpretAlt == 2) || (InterpretAlt >= 4))
					{
						AnsiColor(kDefault);
						FProcessInterpretFile(szPlanetFile(i, FALSE), 2, n);
					}
				}
			}
			PrintL();
		}
		PrintL();
	}

/***********************************************************/
/* PART 3: Fixed Stars                                     */
/* For every Fixed Star conjuct with some allowed thing,   */
/* print out interpretation.                               */
/***********************************************************/

	us.nStar = SaveStarFlag;

	if (!us.nStar)
		return;

	AnsiColor(kWhite);
	sprintf(sz, "   ****** FIXED STARS ******");
	PrintSz(sz);
	PrintL2();

	for (i = starLo; i <= starHi; i++)
		ignore[i] = SaveStars[i];

	for (i = starLo; i <= starHi; i++)
		if (!ignore[cLastMoving + starname[i - cLastMoving]])
		{
			j = cLastMoving + starname[i - cLastMoving];
			AnsiColor(kObjA[j]);
			sprintf(sz, "%-10.10s: ", Lang(tObjName, j));
			PrintSz(sz);
			PrintZodiac(cp0.longitude[j]);
			PrintSz("  ");
			PrintAltitude(cp0.latitude[j]);
			k = cp0.house_no[j];
			AnsiColor(kSignA(k));
			sprintf(sz, "  [%2d%s house]", k, Lang(tSuffix, k));
			PrintSz(sz);
			AnsiColor(kDefault);
			sprintf(sz, "  %s %7.7s  Star #%2d: %5.2f\n",
					us.fSeconds ? " " : "", szStarConName[j - cLastMoving], i - cLastMoving, rStarBright[j - cLastMoving]);
			PrintSz(sz);

			for (jj = 1; jj <= cLastMoving; jj++)
			{
				if (ignore[jj])
					continue;
				kk = grid->n[jj][j];
				m = grid->v[jj][j];
				if (kk < 1 || kk > 5)
					continue;
				PrintAspect(j, Z2Sign(cp0.longitude[j]), (int) RSgn(cp0.vel_longitude[j]), kk,
							jj, Z2Sign(cp0.longitude[jj]), (int) RSgn(cp0.vel_longitude[jj]), 'a', 7, 10);
				AnsiColor(kDefault);
				sprintf(sz, " - orb: %c%d%c%02d'\n",
						us.fAppSep ? (m < 0 ? 'a' : 's') : (m < 0 ? '-' : '+'), abs(m) / 60, chDeg1, abs(m) % 60);
				PrintSz(sz);
			}

			if (InterpretAlt > 1)
			{
				FProcessInterpretFile("stars.dat", 0, j - cLastMoving);
				PrintL();
			}
		}
}

/*
***************************************************************************
** Transit Chart interpretation routine
***************************************************************************
*/

void TransitInterpret()
{
	char sz[cchSzDef], c;
	int i, j, k, l, m, SaveStarFlag;
	double ip, jp, p;

	SaveStarFlag = us.nStar;
	for (i = starLo; i <= starHi; i++)
		SaveStars[i] = ignore[i];
	us.nStar = FALSE;
	us.fAppSep = TRUE;

	if (!FCreateGridRelation(FALSE))
		return;

/************************************************************************ */
/* PART 1: Transiting Planet's positions in houses and aspects.           */
/* For every transiting object print general meaning of planet transits,  */
/* position in House with interpretation, aspects with natal planets with */
/* powers and interpretataion.                                            */
/**************************************************************************/

	PrintL();

	sprintf(sz, "   ****** TRANSITING PLANETS AND THEIR ASPECTS ******");
	PrintSz(sz);
	PrintL2();

	for (i = 1; i <= cLastMoving; i++)
	{
		if (ignore2[i])
			continue;

		AnsiColor(kObjA[i]);
		l = Z2Sign(cp2.longitude[i]);
		c = Dignify(i, l);
		sprintf(sz, "%s%s in %s", cp0.vel_longitude[i] < 0.0 ? "Retrograde " : "",
				Lang(tObjName, i), Lang(tSignName, l));
		PrintSz(sz);
		if (i < cuspLo || i > cuspHi)
		{
			sprintf(sz, " and %d%s House.", cp0.house_no[i], Lang(tSuffix, cp0.house_no[i]));
			PrintSz(sz);
		}
		PrintL();
		if (InterpretAlt != 1)
		{
			AnsiColor(kDefault);
			FProcessInterpretFile(szPlanetFile(i, TRUE), 0, 0);
			FProcessInterpretFile(szPlanetFile(i, TRUE), 1, cp0.house_no[i]);
		}

		/* Print aspects of planets with orb and aspect power   */

		for (j = 1; j <= cLastMoving; j++)
		{
			if (!ignore[j])
			{
				k = grid->n[i][j];
				m = grid->v[i][j];
				if (k < 1 || k > cAspectInt)
					continue;

				ip = i <= cLastMoving ? rTransitInf[i] : 2.5;
				jp = j <= cLastMoving ? ppower1[j] : 2.5;
				p = (rAspInf[k] * (sqrt) (ip * jp) * (1.0 - fabs((double) (m)) / 60.0 / GetOrb(i, j, k)) * 500.0);

				PrintAspect(i, Z2Sign(cp2.longitude[i]), (int) RSgn(cp2.vel_longitude[i]), k,
							j, Z2Sign(cp1.longitude[j]), (int) RSgn(cp1.vel_longitude[j]), 'a', 7, 10);
				sprintf(sz, " - orb: %c%d%c%02d'",
						us.fAppSep ? (m < 0 ? 'a' : 's') : (m < 0 ? '-' : '+'), abs(m) / 60, chDeg1, abs(m) % 60);
				PrintSz(sz);
				AnsiColor(kDkGreen);
				sprintf(sz, " - power:%6.2f\n", p / 500.0);
				PrintSz(sz);

/* Print out aspect interpretation (if needed)   */

				if (InterpretAlt != 1)
				{
					AnsiColor(kDefault);
					FProcessInterpretFile(szAspectFile(i, TRUE), j, k);
				}
			}
		}

		PrintL();
	}

	us.nStar = SaveStarFlag;

	for (i = starLo; i <= starHi; i++)
		ignore[i] = SaveStars[i];
}
#endif /* INTERPRETALT */
#endif /* INTERPRET */
