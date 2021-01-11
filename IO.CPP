/*
** Astrolog File: io.c
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
#include <string.h>
/*
******************************************************************************
** File IO Routines.
******************************************************************************
*/

/* Take the current chart information, and write it out to the file   */
/* as indicated by the -o switch. This is executed at the end of */
/* program execution if the -o switch is in effect.                   */

BOOL FOutputData()
{
	char sz[cchSzDef];
	FILE *file;
	int i, j;
	double rT;

	if (us.fNoWrite)
		return FALSE;

	file = fopen(is.szFileOut, "w");	/* Create and open the file for output. */

	if (file == NULL)
	{
		sprintf(sz, "File %s can not be created.", is.szFileOut);
		PrintError(sz);
		return FALSE;
	}

	if (!us.fWritePos)
	{

		/* Write the chart information to the file. */

		if (ciMain.mon < 1)
		{
			fclose(file);
			PrintError("Can't output chart with no time/space to file.");
			return FALSE;
		}

		fprintf(file, "@0102  ; %s chart info.\n\n", szAppName);
		fprintf(file, "; Date is in American format: month day year.\n\n", szAppName);
		i = us.fAnsiChar;
		us.fAnsiChar = FALSE;
		
		// Force time to 24 hour mode
		BOOL oldTimeFormat = us.fEuroTime;
		us.fEuroTime = 1;
		fprintf(file, "%cqb %d %d %d %s %s %s %s\n", chSwitch, ciMain.mon, ciMain.day, ciMain.yea, SzTim(ciMain.tim),
				ciMain.dst == 0.0 ? "ST" : (ciMain.dst == 1.0 ? "DT" : SzZone(ciMain.dst)), SzZone(-ciMain.zon), SzLocation(ciMain.lon, ciMain.lat));
		us.fEuroTime =	oldTimeFormat;
		fprintf(file, "%czi \"%s\" \"%s\"\n", chSwitch, ciMain.nam, ciMain.loc);
		us.fAnsiChar = i;
	}
	else
	{

		/* However, if the -o0 switch is in effect, then write the actual */
		/* positions of the planets and houses to the file instead.       */

		fprintf(file, "@0203  ; %s chart positions.\n", szAppName);
		fprintf(file, "%czi \"%s\" \"%s\"\n", chSwitch, ciMain.nam, ciMain.loc);

		for (i = 1; i <= cObj; i++)
		{
			if (!ignore[i] || FCusp(i))
			{
				fprintf(file, "%cYF ", chSwitch);
				if (i <= cLastMoving)
				{
					fprintf(file, "%c%c%c", szObjShortNameEnglish[i][0],szObjShortNameEnglish[i][1],szObjShortNameEnglish[i][2]);
				}
				else
				{
					fprintf(file, "%3d", i);
				}

				rT = FBetween(i, cuspLo - 1 + 4, cuspLo - 1 + 9) ? cp0.cusp_pos[i - (cuspLo - 1)] : cp0.longitude[i];
				j = (int) rT;

				// removed the ':' after the planet name in the following statement just in case we want in 
				// the future increase the number of characters, also it is not needed for anything
				fprintf(file, "%3d %s%13.9f,%4d%13.9f,",
						j % 30, szSignAbbrevEnglish[j / 30 + 1], RFract(rT) * 60.0,
						(int) cp0.latitude[i], RFract(fabs(cp0.latitude[i])) * 60.0);
				rT = i > cLastMoving ? 999.0 : sqrt(spacex[i] * spacex[i] +
									spacey[i] * spacey[i] + spacez[i] * spacez[i]);
				fprintf(file, "%14.9f%14.9f\n", Rad2Deg(cp0.vel_longitude[i]), rT);
			}
		}
	}

	/* Now write any extra strings that were on the command line after the -o */
	/* specification but before the next switch, to the file as comments.     */

	for (i = 1; i < is.cszComment; i++)
	{
		is.rgszComment++;
		fprintf(file, "; %s\n", is.rgszComment[1]);
	}
	fclose(file);
	return TRUE;
}


/*
******************************************************************************
** User Input Routines.
******************************************************************************
*/

/* Given a string, return an index number corresponding to what the string */
/* indicates, based on a given parsing mode. In most cases this is mainly  */
/* looking up a string in the appropriate array and returning the index.   */

int NParseSz(char *szEntry, int pm) {
	char szLocal[cchSzMax], *sz;
	int cch, n, i;

	/* First strip off any leading or trailing spaces. */
	for (cch = 0; szLocal[cch] = szEntry[cch]; cch++)
		;
	szLocal[cch] = chNull;
	while (cch && (byte)szLocal[cch - 1] <= ' ')
		szLocal[--cch] = chNull;
	for (sz = szLocal; *sz && (byte)(*sz) <= ' '; sz++, cch--)
		;
	// fix IC name to be "IC "
	if( sz[0] == 'I' && sz[1] == 'C') {
		sz[2] = ' ';
		sz[3] = 0;
		cch++;
	}
	if (cch >= 3) // ask for a minimum of 3 characters, but why? sometimes less is enough!
	{
		switch (pm)
		{
			/* Parse months, e.g. "February" or "Feb" -> 2 for February. */
		case pmMon:
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			{
				if(fSstringsMatch(sz, Lang(tMonth, i), 0))
				{
					return i;
				}
			}
			break;

			/* Parse months in English (e.g. for data files), e.g. "February" or "Feb" -> 2 for February. */
		case pmMonEn:
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			{
				if(fSstringsMatch(sz, szMonthEnglish[i], 0))
				{
					return i;
				}
			}
			break;

			/* Parse planets, e.g. "Jupiter" or "Jup" -> 6 for Jupiter. */
		case pmObject:

			for (i = 0; i <= cObj; i++)
			{
				if(fSstringsMatch(sz, Lang(tObjShortName, i), 3))	
				// we can match maximum 3 characters because of planet position files that have three chars for planet names
				{
					return i;
				}
			}
			break;
		case pmObjectEn:

			for (i = 0; i <= cObj; i++)
			{
				if(fSstringsMatch(sz, (char*)szObjShortNameEnglish[i], 3))	
				// we can match maximum 3 characters because of planet position files that have three chars for planet names
				{
					return i;
				}
			}
			break;

			/* Parse aspects, e.g. "Conjunct" or "Con" -> 1 for the Conjunction. */
		case pmAspect:
			for (i = 1; i <= cAspect; i++)
			{
				if(fSstringsMatch(sz, Lang(tAspectAbbrev, i), 0))
				{
					return i;
				}
			}
			break;

			/* Parse house systems, e.g. "Koch" or "Koc" -> 1 for Koch houses. */
		case pmHouseSystem:
			for (i = 0; i < NUMBER_OF_HOUSE_SYSTEMS; i++)
			{
				if(fSstringsMatch(sz, Lang(tHouseSystem, i), 0))
				{
					return i;
				}
			}
			break;

			/* Parse zodiac signs, e.g. "Scorpio" or "Sco" -> 8 for Scorpio. */
		case pmSign:
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			{
				if(fSstringsMatch(sz, Lang(tSignName, i), 0))
				{
					return i;
				}
			}
			break;

		case pmSignEn:
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			{
				if(fSstringsMatch(sz, (char*)szSignAbbrevEnglish[i], 0))
				{
					return i;
				}
			}
			break;
			/* Parse colors, e.g. "White" or "Whi" -> 15 for White. */
		case pmColor:
			for (i = 0; i < cColor; i++)
			{
				if(fSstringsMatch(sz, Lang(tColor, i), 0))
				{
					return i;
				}
			}
			break;
		}
	}

	n = atoi(sz);

	if (pm == pmYea)
	{
		/* For years, process any "BC" (or "B.C.", "b.c", and variations) and   */
		/* convert an example such as "5BC" to -4. For negative years, note the */
		/* difference of one, as 1AD was preceeded by 1BC, with no year zero.   */
		i = Max(cch - 1, 0);
		if (i && sz[i] == '.')
			i--;
		if (i && ChCap(sz[i]) == 'C')
			i--;
		if (i && sz[i] == '.')
			i--;
		if (i && ChCap(sz[i]) == 'B')
			n = 1 - n;
	}
	return n;
}


/* Given a string, return a floating point number corresponding to what the  */
/* string indicates, based on a given parsing mode, like above for integers. */

double RParseSz(char *szEntry, int pm) {
	char szLocal[cchSzMax], *sz, *pch, ch, chdot, minutes[12];
	int cch, havedot, dot1, dot2, newstyle = FALSE, i, j, f = FALSE;
	double r, rMinutes, rSeconds;

	/* First strip off any leading or trailing spaces. */
	for (cch = 0; szLocal[cch] = szEntry[cch]; cch++)
		;
	while (cch && szLocal[cch - 1] <= ' ')
		szLocal[--cch] = chNull;
	for (sz = szLocal; *sz && *sz <= ' '; sz++, cch--);
	;
	/* Capitalize all letters and make colons be periods to be like numbers. */
	for (pch = sz; *pch; pch++)
	{
		ch = *pch;
		if (ch == ':' || ch == '\'' || ch == '\"')
			ch = '.';
		else
			ch = ChCap(ch);
		*pch = ch;
	}
	ch = sz[0];

	if (pm == pmTim)
	{
		/* For times, process "Noon" and "Midnight" (or just "N" and "M"). */
		if (ch == 'N')
			return 12.0;
		else if (ch == 'M')
			return 0.0;
	}
	else if (pm == pmDst)
	{
		/* For the Daylight time flag, "Daylight", "Yes", and "True" (or just */
		/* their first characters) are all indications to be ahead one hour.  */
		/* For French language we add "Oui" (yes) */
		//if (ch == 'D' || ch == 'Y' || ch == 'T' || ch == 'O')
		//	return 1.0;
		/* "Standard", "No", and "False" mean the normal zero offset. */
		/* For French language we add "Non" */
	//	else if (ch == 'S' || ch == 'N' || ch == 'F')
	//		return 0.0;
		if(	(_strcmpi(sz, Lang(sYES)) == 0) ||				  // test "Yes"
			(sz[0] == Lang(sYES)[0] && sz[1] == 0) ||		  // test "Y"
			(_strcmpi(sz, "DT") == 0)) return 1.0;			  // test "DT"
		else return 0.0;
	}
	else if (pm == pmZon)
	{
		/* For time zones, see if the abbrev is in a table, e.g. "EST" -> 5. */
		for (i = 0; i < cZone; i++)
			if (NCompareSz(sz, szZon[i]) == 0)
				return rZon[i];
	}
	else if (pm == pmLon || pm == pmLat)
	{
		/* For locations, negate the value for an "E" or "S" in the middle    */
		/* somewhere (e.g. "105E30" or "27:40S") for eastern/southern values. */
		for (i = 0; i < cch; i++)
		{
			ch = sz[i];
			if (FCapCh(ch))
			{
				if (ch == 'E' || ch == 'S')
					f = TRUE;
				sz[i] = '.';
				i = cch;
			}
		}
		ch = sz[0];
	}

	/* In new time and locations strings there are at least two dots now,    */
	/* so we have to locate, if this string has second dot. If not, it's old */
	/* style string. But some old style strings can have second dot at end,  */
	/* so we have to distinguish them from new strings with two dots. VA.    */

	newstyle = TRUE;
	if (pm == pmTim || pm == pmLon || pm == pmLat)
	{
		havedot = 0;
		dot1 = 0;
		dot2 = 0;
		for (i = 0; i < cch; i++)
		{
			chdot = sz[i];
			if (chdot == '.')
			{
				if (havedot)
				{
					dot2 = i;
					i = cch;
				}
				else
				{
					dot1 = i;
					havedot = 1;
				}
			}
		}
		if (dot2 == 0 || dot2 == cch - 1)
			newstyle = FALSE;

		/* Now, when we know that it is new style string, we have to convert */
		/* it to the old style.                                              */

		if (newstyle)
		{
			for (i = dot1 + 1; i < cch; i++)
			{
				j = i - (dot1 + 1);
				if (sz[i] >= '.' && sz[i] <= '9' && sz[i] != '/')
					minutes[j] = sz[i];
				/*        if (minutes[j] == chNull || j > 9)  */
				else
					i = cch;
			}
			rMinutes = atof(minutes);
			rSeconds = RFract(rMinutes) / 0.6;
			if (rSeconds >= 1.0)
				return rLarge;
			sprintf(minutes, "%6.4f", rSeconds);
			for (i = 2; i < 5; i++)
			{
				j = i - 2 + dot2;
				sz[j] = minutes[i];
			}
		}
	}

	/* Anything still at this point should be in a numeric format. */
	if (!FNumCh(ch) && ch != '+' && ch != '-' && ch != '.')
		return rLarge;
	r = (f ? -1.0 : 1.0) * atof(sz);

	if (pm == pmTim)
	{
		/* Backtrack over any time suffix, e.g. "AM", "p.m." and variations. */
		i = Max(cch - 1, 0);
		if (i && sz[i] == '.')
			i--;
		if (i && sz[i] == 'M')
			i--;
		if (i && sz[i] == '.')
			i--;
		bool suffixSet = false;
		if (i && sz[i + 1] == 'M')
		{
			ch = sz[i];
			if (ch == 'A') {		/* Adjust value appropriately */
				r = r >= 12.0 ? r - 12.0 : r;	/* if AM or PM suffix.        */
				suffixSet = true;
			}
			else if (ch == 'P') {
				r = r >= 12.0 ? r : r + 12.0;
				suffixSet = true;
			}
		}
		// check localized AM/PM suffix
		if(!suffixSet && strlen(sz) > strlen(Lang(sAM)) && !_strcmpi(sz + strlen(sz) - strlen(Lang(sAM)), Lang(sAM))) {
				r = r >= 12.0 ? r - 12.0 : r;	
		}
		if(!suffixSet && strlen(sz) > strlen(Lang(sPM)) && !_strcmpi(sz + strlen(sz) - strlen(Lang(sPM)), Lang(sPM))) {
				r = r >= 12.0 ? r : r + 12.0;
		}
	}
	return r;
}

// Set location parameters in chart info to default location
void SetToHere(CI *ci)
{
	ci->dst = us.dstDef;
	ci->zon = us.zonDef;
	ci->lon = us.lonDef;
	ci->lat = us.latDef;
	strcpy(ci->loc, us.szLocNameDef);
}

// Set time parameters to now
void SetToNow(CI *ci)
{
	GetTimeNow(&ci->mon, &ci->day, &ci->yea, &ci->tim, us.zonDef - us.dstDef);
}

// Set location parameters in chart info to default location, and time parameters to now
void SetHereAndNow(CI *ci)
{
	SetToHere(ci);
	SetToNow(ci);
}

void SetCI(CI *ci, int mon, int day, int yea, double tim, double dst, double zon, double lon, 
		   double lat, char *loc)
{
	ci->mon = mon;
	ci->day = day;
	ci->yea = yea;
	ci->tim = tim;
	ci->dst = dst;
	ci->zon = zon;
	ci->lon = lon;
	ci->lat = lat;
	strcpy(ci->loc, loc);
}


/* This procedure gets all the parameters defining the chart that  
 * will be worked with later. Given a "filename", it gets from it all the    
 * pertinent chart information. This is more than just reading from a file - 
 * the procedure also handles special cases, such as calling the time 
 * functions to determine the current time - the  
 * program considers these cases "virtual" files. Furthermore, when reading  
 * from a real file, we have to check if it was written in the -o0 format.   
 */

BOOL FInputData(char *szFile, char *szDir)
{
	FILE *file;
	char ch, szLine[cchSzMax], *argv[MAXSWITCHES];
	BOOL rc = FALSE;	// failure
	int argc, i;


	if (NCompareSz(szFile, "nul") == 0)
	{
		// Reading from the virtual file "nul" means leaving the 
		// chart information alone with whatever settings it may already have 
		return TRUE;
	}
	else if (NCompareSz(szFile, "set") == 0)
	{
		// Reading from the virtual file "set" means use a particular set of 
		// chart information generated earlier in the program
		ciCore = ciSave;
		return TRUE;
	}
	else if (NCompareSz(szFile, "now") == 0)
	{
		// Reading from the file "now" means use the time functions to 
		// calculate the present date and time and set the other 
		// parameters to default
		SetHereAndNow(&ciCore);
		return TRUE;
	}

	// The special cases have been taken care of, we read from a real file

	file = FileOpen(szFile, szDir, TRUE);
	if (!file)
	{
		return FALSE;
	}

	ch = getc(file);
	ungetc(ch, file);

	if (ch == '@')
	{

		// Here we handle chart info files, position files, and config files. 
		// Run through each line as a series of command switches.

		for(;;)
		{
			while (!feof(file) && (ch = getc(file)) < ' ');	// discard control characters
				
			if (feof(file))
			{
				rc = TRUE;		// normal end of file
				break;
			}

			for (szLine[0] = ch, i = 1; 
				i < cchSzMax && !feof(file) && (unsigned char)(szLine[i] = getc(file)) >= ' '; i++);	// read one line

			szLine[i] = chNull;	// terminate line

			argc = NParseCommandLine(szLine, argv);

			if (!FProcessSwitchesMain(argc, argv))
			{
				break;
			}
		}
	}
	else
	{
		PrintWarning("The command info file is not in any valid format (character %d).", (int) ch);
	}

	fclose(file);
	return rc;
}

// Search for a file in three directories: the current directory, 
// the directory where Astrolog32 is, and finally the directory passed
// as parameter. If the file exists, open it and return the file pointer,
// otherwise return NULL.
FILE *FileOpen(char *szFile, char *szDir, BOOL warn_if_file_not_found)
{
	FILE *file;
	file_name_t file_name;
	char *mode = "r";

	// First look for the file in the current directory
	
	if (file = fopen(szFile, mode))
	{
		return file;
	}

	// Next look in the executable directory
	sprintf(file_name, "%s\\%s", dirs.executable.dir, szFile);
	
	if (file = fopen(file_name, mode))
	{
		return file;
	}

	// Finally look in the directory passed as parameter
	sprintf(file_name, "%s\\%s", szDir, szFile);

	if (file = fopen(file_name, mode))
	{
		return file;
	}

	// If we got here, that means the file was not found

	if (warn_if_file_not_found)
	{
		PrintWarning(Lang(sFILE_NOT_FOUND), szFile, szDir);
	}

	return NULL;
}

// See if a file exists in one of three directories: the current directory, 
// the directory where Astrolog32 is, and finally the directory passed
// as parameter. Return TRUE if file found, false otherwise. Parameter
// path_found returns the full path of the file if it was found,
// but only if path_found is not NULL.
BOOL FileFind(char *szFile, char *szDir, char *path_found)
{
	file_name_t file_name;
	char *mode = "r";
	FILE *file;


	// First look for file in the current directory
	strcpy(file_name, szFile);
	file = fopen(file_name, mode);

	if (!file)
	{
		// Next look in the executable directory
		sprintf(file_name, "%s\\%s", dirs.executable.dir, szFile);
		file = fopen(file_name, mode);

		if (!file)
		{
			// Finally look in the directory passed as parameter
			sprintf(file_name, "%s\\%s", szDir, szFile);
			file = fopen(file_name, mode);

			if (!file)
			{
				return FALSE; // no luck finding file
			}
		}
	}

	fclose(file);
	if(path_found)
	{
		strcpy(path_found, file_name);
	}

	return TRUE;
}

/* io.c */
