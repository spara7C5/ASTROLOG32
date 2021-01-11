/*
** Astrolog File: general.c
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

double rOne = 0.999999999;
double rFractal = 0.0;

/*
******************************************************************************
** General Procedures.
******************************************************************************
*/



/* Determine the sign of a number: -1 if value negative, +1 if value */
/* positive, and 0 if it's zero.                                     */

double RSgn(double r) {
	return r == 0.0 ? 0.0 : RSgn2(r);
}


/* Given an x and y coordinate, return the angle formed by a line from the */
/* origin to this coordinate. This is just converting from rectangular to  */
/* polar coordinates; however, we don't determine the radius here.         */

double Angle(double x, double y) {
	double a;

	if (x != 0.0)
	{
		if (y != 0.0)
			a = atan(y / x);
		else
			a = x < 0.0 ? rPi : 0.0;
	}
	else
		a = y < 0.0 ? -rPiHalf : rPiHalf;
	if (a < 0.0)
		a += rPi;
	if (y < 0.0)
		a += rPi;
	return a;
}


/* Modulus function for floating point values, where we bring the given */
/* parameter to within the range of 0 to 360.                           */

double Mod(double d) {
	if (d >= 360.0)			/* In most cases, our value is only slightly */
		d -= 360.0;			/* out of range, so we can test for it and   */
	else if (d < 0.0)		/* avoid the more complicated arithmetic.    */
		d += 360.0;

	if (d >= 0 && d < 360.0)
		return d;
	return (d - floor(d / 360.0) * 360.0);
}


/* Another modulus function, this time for the range of 0 to 2 Pi. */

double ModRad(double r) {
	while (r >= rPi2)			/* We assume our value is only slightly out of       */
		r -= rPi2;				/* range, so test and never do any complicated math. */
	while (r < 0.0)
		r += rPi2;
	return r;
}


/* Integer division - like the "/" operator but always rounds result down. */

long Dvd(long x, long y) {
	long z;

	if (y == 0)
		return x;
	z = x / y;
	if (((x >= 0) == (y >= 0)) || x - z * y == 0)
		return z;
	return z - 1;
}


/* A similar modulus function: convert an integer to value from 1..12. */

int Mod12(int i) {
	while (i > 12)
		i -= 12;
	while (i < 1)
		i += 12;
	return i;
}


/* Convert an inputed fractional degrees/minutes value to a true decimal   */
/* degree quantity. For example, the user enters the decimal value "10.30" */
/* to mean 10 degrees and 30 minutes; this will return 10.5, i.e. 10       */
/* degrees and 30 minutes expressed as a floating point degree value.      */

double DegMin2DecDeg(double d) {
	return RSgn(d) * (floor(fabs(d)) + RFract(fabs(d)) * 100.0 / 60.0);
}


/* This is the inverse of the above function. Given a true decimal value */
/* for a zodiac degree, adjust it so the degrees are in the integer part */
/* and the minute expressed as hundredths, e.g. 10.5 degrees -> 10.30    */

double DecDeg2DegMin(double d) {
	return RSgn(d) * (floor(fabs(d)) + RFract(fabs(d)) * 60.0 / 100.0);
}


/* Return the shortest distance between two degrees in the zodiac. This is  */
/* normally their difference, but we have to check if near the Aries point. */

double MinDistance(double deg1, double deg2) {
	double i;

	i = fabs(deg1 - deg2);
	return i < 180.0 ? i : 360.0 - i;
}


/* This is just like the above routine, except the min distance value  */
/* returned will either be positive or negative based on whether the   */
/* second value is ahead or behind the first one in a circular zodiac. */

double MinDifference(double deg1, double deg2) {
	double i;

	i = deg2 - deg1;
	if (fabs(i) < 180.0)
		return i;
	return RSgn(i) * (fabs(i) - 360.0);
}


/* Return the degree of the midpoint between two zodiac positions, making */
/* sure we return the true midpoint closest to the positions in question. */

double Midpoint(double deg1, double deg2) {
	double mid;

	mid = (deg1 + deg2) / 2.0;
	return MinDistance(deg1, mid) < 90.0 ? mid : Mod(mid + 180.0);
}


/* Given a planet and sign, determine whether: The planet rules the sign, */
/* the planet has its fall in the sign, the planet exalts in the sign, or */
/* is debilitated in the sign; and return an appropriate character.       */

char Dignify(int obj, int sign) {
	if (obj > cLastMoving)
		return ' ';
	if (ruler1[obj] == sign || ruler2[obj] == sign)
		return Lang(sRFED)[0];       // R
	if (ruler1[obj] == Mod12(sign + 6) || ruler2[obj] == Mod12(sign + 6))
		return Lang(sRFED)[1];       // F
	if (exalt[obj] == sign)
		return Lang(sRFED)[2];       // e
	if (exalt[obj] == Mod12(sign + 6))
		return Lang(sRFED)[3];       // d
	return '-';
}


/* Determine the number of days in a particular month. The year is needed, */
/* too, because we have to check for leap years in the case of February.   */

int DayInMonth(int month, int year) {
	int d;

	if (month == mSep || month == mApr || month == mJun || month == mNov)
		d = 30;
	else if (month != mFeb)
		d = 31;
	else
	{
		d = 28;
		if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0 || year <= yeaJ2G))
			d++;
	}
	return d;
}


/* Return the actual number of days in a particular month. Normally, this  */
/* is the same as the above routine which determines the index of the last */
/* day of the month, but the values can differ when changing between       */
/* calendar systems (Julian to Gregorian) in which one can jump over days. */

int DaysInMonth(int month, int year) {
	int d;

	d = DayInMonth(month, year);
	if (year == yeaJ2G && month == monJ2G)
		d -= (dayJ2G2 - dayJ2G1 - 1);
	return d;
}


/* Return the day of the week (Sunday is 0) of the specified given date. */

int DayOfWeek(int month, int day, int year) {
	int d;

	d = (int) ((MdyToJulian(month, day, year) + 1) % 7);
	return d < 0 ? d + 7 : d;
}


/* Given a day, and the month and year it falls in, add a number of days    */
/* to it and return the new day index. As month changes are not checked for */
/* here, this is mostly just adding the offset to the day; however we need  */
/* to check for calendar changes for when days in a month may be skipped.   */

int AddDay(int month, int day, int year, int delta) {
	int d;

	d = day + delta;
	if (year == yeaJ2G && month == monJ2G)
	{							/* Check for Julian to  */
		if (d > dayJ2G1 && d < dayJ2G2)	/* Gregorian crossover. */
			d += NSgn(delta) * (dayJ2G2 - dayJ2G1 - 1);
	}
	return d;
}


/* Given an aspect and two objects making that aspect with each other,   */
/* return the maximum orb allowed for such an aspect. Normally this only */
/* depends on the aspect itself, but some objects require narrow orbs,   */
/* and some allow wider orbs, so check for these cases.                  */

double GetOrb(int obj1, int obj2, int asp) {
	double orb, r;

	orb = rAspOrb[asp];
	r = obj1 > cLastMoving ? StarOrb : rObjOrb[obj1];
	orb = Min(orb, r);
	r = obj2 > cLastMoving ? StarOrb : rObjOrb[obj2];
	orb = Min(orb, r);
	if (obj1 <= cLastMoving && obj2 <= cLastMoving)
	{
		orb += rObjAdd[obj1];
		orb += rObjAdd[obj2];
		if (us.fParallel)
			orb = orb / CoeffPar;
		else if (us.nRel <= rcDual || us.fTransitInf)
			orb = orb / CoeffComp;
	}
	return orb;
}

// this is like function GetOrb, except that it does not consider
// stars, parallel aspects or relationship charts
double GetOrbEx(int obj1, int obj2, int asp)
{
	double orb;

	if (obj1 <= cLastMoving && obj2 <= cLastMoving)
	{
		orb = Min(Min(rAspOrb[asp], rObjOrb[obj1]), rObjOrb[obj2]);
		orb += rObjAdd[obj1];
		orb += rObjAdd[obj2];

		return orb;
	}
	else
	{
		return 0.0;
	}
}


/*
******************************************************************************
** String Procedures.
******************************************************************************
*/

// just like strcpy, except return the end of destination string instead of the
// source string
char *StrCpy(char *dst, const char *src)
{
	while(*src)
	{
		*dst++ = *src++;
	}

	*dst = 0;

	return dst;
}


/* Compare two strings. Return 0 if they are equal, a positive value if  */
/* the first string is greater, and a negative if the second is greater. */

int NCompareSz(const char *s1, const char *s2) {
	while (*s1 && *s1 == *s2)
		s1++, s2++;
	return *s1 - *s2;
}


/* Copy a given number of bytes from one location to another. */

void CopyRgb(byte *pbSrc, byte *pbDst, int cb) {
	while (cb-- > 0)
		*pbDst++ = *pbSrc++;
}

// Compares two NULL-terminated strings, byte by byte, until the end of 
// the shorter string. Case is disregarded. Third parameter is the maximum
// number of characters to compare, if greater than zero, otherwise it 
// is disregarded.
// Returns true if both strings are the same, false otherwise. 

BOOL fSstringsMatch(char *s1, char *s2, int n)
{
	for(; *s1 && *s2; s1++, s2++)
	{
		if(tolower(*s1) != tolower(*s2))
		{
			return FALSE;
		}

		if(--n == 0)
		{
			break;
		}
	}

	return TRUE;
}

// This is just like the standard library function strstr, 
// except that case is disregarded during the search.
// It returns a pointer to the first occurrence of string ct in cs, or NULL if not present.
char *StrStr(char *cs, char *ct)
{
	char cs_buf[5000];
	char ct_buf[5000];
	char *s, *d;
	char *ret = NULL;


	if(cs && ct)
	{
		// make lower case copies of the strings
		for(s = cs, d = cs_buf; *s; s++, d++)
		{
			*d  = tolower(*s);
		}
		*d = 0;

		for(s = ct, d = ct_buf; *s; s++, d++)
		{
			*d  = tolower(*s);
		}
		*d = 0;

		// now search
		d = strstr(cs_buf, ct_buf);
		if(d)
		{
			ret = d - cs_buf + cs;
		}
	}

	return ret;
}

/* Given a string, return a pointer to a persistent version of it, where  */
/* 'persistent' means its contents won't be invalidated when the stack    */
/* frame changes.  */

char *SzPersist(char *string)
{
	char	*copy;
	int		len = strlen(string) + 1;

	// make a copy of the string in the local heap and use it
	if(copy = (char*)malloc(len))
	{
		memcpy(copy, string, len);
	}

	return copy;
}

/* Exit the program, and do any cleanup necessary. */

void Terminate(int tc) {
	char sz[cchSzDef];

	if (us.fNoQuit)
		return;
	if (tc == tcForce)
	{
		is.S = stdout;
		AnsiColor(kWhite);
		sprintf(sz, "\n%s v. %s exited.\n", szAppName, szVersionCore);
		PrintSz(sz);
	}

	if (us.fAnsiColor)
	{
		sprintf(sz, "%c[0m", chEscape);	/* Get out of any Ansi color mode. */
		PrintSz(sz);
	}
	exit(abs(tc));
}


/* Print a string on the screen. A seemingly simple operation, however we */
/* keep track of what column we are printing at after each newline so we  */
/* can automatically clip at the appropriate point, and we keep track of  */
/* the row we are printing at, so we may prompt before screen scrolling.  */

void PrintSz(char *sz, ...)
{
	va_list	args;
	char	buffer[1000];
	char	szInput[2], *pch;

	va_start(args, sz);
	vsprintf(buffer, sz, args);

	for (pch =  buffer; *pch; pch++)
	{
		if (*pch != '\n')
		{
			is.cchCol++;
			if (us.fClip80 && is.cchCol >= us.nScreenWidth)	/* Clip if need be. */
			{
				continue;
			}
		}
		else
		{
			is.cchRow++;
			is.cchCol = 0;
		}

		if (is.S == stdout)
		{
			if ((byte) * pch >= ' ')
			{
				szInput[0] = *pch;
				szInput[1] = chNull;
				TextOut(wi.hdc,
						(is.cchCol - 1 - wi.xScroll * 10) * wi.xChar + 4,
						(is.cchRow - wi.yScroll * 10) * wi.yChar, szInput, 1);
			}
		}
		else
		{
			putc(*pch, is.S);
		}

		if (*pch == '\n' && is.S == stdout && wi.hdcPrint != hdcNil && is.cchRow >= us.nScrollRow)
		{

			/* If writing to the printer, start a new page when appropriate. */

			is.cchRow = 0;
			EndPage(wi.hdcPrint);
			StartPage(wi.hdcPrint);

			/* StartPage clobbers all the DC settings for Windows 95, 98, etc,   */
			/* however for NT, 2000 and later it doesn't. Here we cater for both */
			SetMapMode(wi.hdcPrint, MM_ANISOTROPIC);	/* For SetViewportExtEx */
			SetViewportOrgEx(wi.hdcPrint, 0, 0, NULL);
			SetViewportExtEx(wi.hdcPrint, wi.xPrint, wi.yPrint, NULL);
			SetWindowOrgEx(wi.hdcPrint, 0, 0, NULL);
			SetWindowExtEx(wi.hdcPrint, wi.xClient, wi.yClient, NULL);
			SetBkMode(wi.hdcPrint, TRANSPARENT);
			SelectObject(wi.hdcPrint, wi.hfont);
		}
	}

	va_end(args);
}


/* Print a single character on the screen. */

void PrintCh(char ch) {
	char sz[2];

	sz[0] = ch;
	sz[1] = chNull;				/* Treat char as a string of length one. */
	PrintSz(sz);				/* Then call above to print the string.  */
}


/* Print a single character on the screen 'n' times */

void PrintTab(char ch, int cch) {
	int		i;
	char	sz[2];

	sz[0] = ch;
	sz[1] = chNull;				

	for (i = 0; i < cch; i++)
		PrintSz(sz);				
}


/* Print a string on the screen. Unlike the normal PrintSz(), here we still */
/* go to the standard output even if text is being sent to a file with -os. */

void PrintSzScreen(char *sz) {
	FILE *fileT;

	fileT = is.S;
	is.S = stdout;
	PrintSz(sz);
	is.S = fileT;
}


/* Print a warning message given a string. This is called in non-fatal  */
/* cases where we return to normal execution after printing the string. */

void PrintWarning(char *error_string, ...)
{
	va_list args;
	char title[100];
	char text[1000];

	va_start(args, error_string);
	vsprintf(text, error_string, args);
	strcpy(title, szAppName);
	strcat(title, " Warning");
	MessageBeep(MB_ICONEXCLAMATION);
	MessageBoxEx(wi.hwndMain, text, title, MB_ICONEXCLAMATION, 0);
	va_end(args);
}


/* Print an error message. This is called in more serious cases which halt */
/* running of the current chart sequence, which can terminate the program. */

void PrintError(char *error_string, ...)
{
	va_list args;
	char title[100];
	char text[1000];

	va_start(args, error_string);
	vsprintf(text, error_string, args);
	strcpy(title, szAppName);
	strcat(title, " Error");
	MessageBeep(-1);
	MessageBoxEx(wi.hwndMain, text, title, MB_ICONSTOP, 0);
	va_end(args);
}


// Request answer from the user - yes or no.
bool PrintQuestion(char *error_string, ...)
{
	va_list args;
	char title[100];
	char text[1000];

	va_start(args, error_string);
	vsprintf(text, error_string, args);
	strcpy(title, szAppName);
	va_end(args);
	if(IDYES == MessageBoxEx(wi.hwndMain, text, title, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2, 0))
		return true;
	else
		return false;
}


/* Simplification for a commonly printed error message. */

void ErrorArgc(char *error_string, ...)
{
	va_list args;
	char text[200];

	va_start(args, error_string);
	vsprintf(text, error_string, args);
	PrintError("Too few options to switch %c%s", chSwitch, text);
	va_end(args);
}


/* Another simplification for a commonly printed error message. */

void ErrorValN(char *szOpt, int nVal) {
	char sz[cchSzDef];

	sprintf(sz, "Value %d passed to switch %c%s out of range.\n", nVal, chSwitch, szOpt);
	PrintError(sz);
}


/* Yet another place to print a type of error message. */

void ErrorArgv(char *szOpt) {
	char sz[cchSzDef];

	sprintf(sz, "The switch %c%s is not allowed now.\n", chSwitch, szOpt);
	PrintError(sz);
}


/* Still another place to print a type of error message. */

void ErrorSwitch(char *szOpt) {
	char sz[cchSzDef];

	sprintf(sz, "Unknown switch '%s'", szOpt);
	PrintError(sz);
}


/* Print error messages dealing with ephemeris file access. */

void ErrorEphem(char *sz, long l) {
	char szT[cchSzDef];

	if (l < 0)
		sprintf(szT, "Ephemeris file %s not found.\n", sz);
	else
		sprintf(szT, "Ephemeris file %s is damaged at position %ld.\n", sz, l);
	is.fNoEphFile = TRUE;
	PrintWarning(szT);
}

/* Set an Ansi or MS Windows text color. */

void AnsiColor(int k)
{
	char sz[cchSzDef];
	int cchSav;

	if (is.S == stdout)
	{
		if (k < 0)
			k = kLtGray;
		SetTextColor(wi.hdc, (COLORREF) rgbbmp[us.fAnsiColor ? k : (gs.fInverse ? kBlack : kLtGray)]);
		return;
	}

	/* Special case: If we are passed the value Reverse, and Ansi color is */
	/* not only on but set to a value > 1, then enter reverse video mode.  */

	if (!us.fAnsiColor || (k == kReverse && us.fAnsiColor < 2))
		return;
	cchSav = is.cchCol;
	is.cchCol = 0;
	sprintf(sz, "%c[", chEscape);
	PrintSz(sz);
	if (k == kDefault)
		PrintCh('0');
	else if (k == kReverse)
	{
		PrintCh('7');
	}
	else
	{
		sprintf(sz, "%c;%d", k > 7 ? '1' : '0', 30 + (k & 7));
		PrintSz(sz);
	}
	PrintCh('m');
	is.cchCol = cchSav;
}


/* Print a zodiac position on the screen. This basically just prints the */
/* string returned from SzZodiac() below, except we take care of color.  */

void PrintZodiac(double deg) {
	AnsiColor(kElemA[(int) (deg / 30.0) & 3]);
	PrintSz(SzZodiac(deg));
	AnsiColor(kDefault);
}


/* Given a zodiac position, return a string containing it as it's */
/* formatted for display to the user.                             */

char *SzZodiac(double deg)
{
	static char szZod[12];
	int sign, d, m;
	double s;

	switch (us.nDegForm)
	{
	case 0:

		/* Normally, we format the position in degrees/sign/minutes format: */

		deg = Mod(deg + (is.fSeconds ? 0.5 / 60.0 / 60.0 : 0.5 / 60.0));
		sign = (int) deg / 30;
		d = (int) deg - sign * 30;
		m = (int) (RFract(deg) * 60.0);
		sprintf(szZod, "%2d%c%c%c%02d", d, chSig3(sign + 1), m);
		if (is.fSeconds)
		{
			s = RFract(deg) * 60.0;
			s = RFract(s) * 60.0;
			sprintf(&szZod[7], "'%02d\"", (int) s);
		}
		break;

	case 1:
		/* However, if -sh switch in effect, get position in hours/minutes: */

		deg = Mod(deg + (is.fSeconds ? 0.5 / 4.0 / 60.0 : 0.5 / 4.0));
		d = (int) deg / 15;
		m = (int) ((deg - (double) d * 15.0) * 4.0);
		sprintf(szZod, "%2dh,%02dm", d, m);
		if (is.fSeconds)
		{
			s = RFract(deg) * 4.0;
			s = RFract(s) * 60.0;
			sprintf(&szZod[7], ",%02ds", (int) s);
		}
		break;

	default:
		/* Otherwise, if -sd in effect, format position as decimal degrees */

		sprintf(szZod, is.fSeconds ? "%11.7f" : "%7.3f", deg);
		break;
	}
	return szZod;
}


/* This is similar to formatting a zodiac degree, but here we return a */
/* string of a (signed) declination value in degrees and minutes.      */

char *SzAltitude(double deg)
{
	static char szAlt[12];
	int d, m, f;
	double s;
	char ch;

	if(us.nDegForm == 2)
	{
		// use decimal degrees

		sprintf(szAlt, is.fSeconds ? "%c%9.6f" : "%c%6.3f", 
			deg < 0.0 ? '-' : '+', fabs(deg));
	}
	else
	{
		// degrees, minutes, and possibly seconds

		f = deg < 0.0;
		deg = fabs(deg) + (is.fSeconds ? 0.5 / 60.0 / 60.0 : 0.5 / 60.0);
		d = (int) deg;
		m = (int) (RFract(deg) * 60.0);
		ch = us.fAnsiChar > 1 ? chDeg3 : chDeg1;
		sprintf(szAlt, "%c%2d%c%02d'", f ? '-' : '+', d, ch, m);

		if (is.fSeconds)
		{
			s = RFract(deg) * 60.0;
			s = RFract(s) * 60.0;
			sprintf(&szAlt[7], "%02d\"", (int) s);
		}
	}

	return szAlt;
}


/* Here we return a string simply expressing the given value as degrees */
/* and minutes (and sometimes seconds) in the 0 to 360 degree circle.   */

char *SzDegree(double deg)
{
	static char szPos[11];
	int d, m;
	double s;

	deg = fabs(deg) + (is.fSeconds ? 0.5 / 60.0 / 60.0 : 0.5 / 60.0);
	d = (int) deg;
	m = (int) (RFract(deg) * 60.0);
	sprintf(szPos, "%3d%c%02d'", d, chDeg1, m);
	if (is.fSeconds)
	{
		s = RFract(deg) * 60.0;
		s = RFract(s) * 60.0;
		sprintf(&szPos[7], "%02d\"", (int) s);
	}
	return szPos;
}


/* Another string formatter, here we return a date string given a month,    */
/* day, and year. We format with the day or month first based on whether    */
/* the "European" date variable is set or not. The routine also takes a     */
/* parameter to indicate how much the string should be abbreviated, if any. */

char *SzDate(int mon, int day, int yea, int nFormat)
{
	static char szDat[20];

	if (us.fEuroDate)
	{
		switch (nFormat)
		{
		case 2:
			sprintf(szDat, "%2d %c%c%c%5d", day, chMon3(mon), yea);
			break;
		case 1:
			sprintf(szDat, "%d %s %d", day, Lang(tMonth, mon), yea);
			break;
		case -1:
			sprintf(szDat, "%2d-%2d-%2d", day, mon, abs(yea) % 100);
			break;
		default:
			sprintf(szDat, "%2d-%2d-%4d", day, mon, yea);
			break;
		}
	}
	else
	{
		switch (nFormat)
		{
		case 3:
			sprintf(szDat, "%c%c%c %2d, %d", chMon3(mon), day, yea);
			break;
		case 2:
			sprintf(szDat, "%c%c%c %2d%5d", chMon3(mon), day, yea);
			break;
		case 1:
			sprintf(szDat, "%s %d, %d", Lang(tMonth, mon), day, yea);
			break;
		case -1:
			sprintf(szDat, "%2d/%2d/%2d", mon, day, abs(yea) % 100);
			break;
		default:
			sprintf(szDat, "%2d/%2d/%4d", mon, day, yea);
			break;
		}
	}
	return szDat;
}


/* Return a string containing the given time expressed as an hour and */
/* minute quantity. This is formatted in 24 hour or am/pm time based  */
/* on whether the "European" time format flag is set or not.          */

char *SzTime(int hr, int min, int sec)
{
	static char szTim[12];

	while (min >= 60)
	{
		min -= 60;
		hr++;
	}
	while (hr < 0)
		hr += 24;
	while (hr >= 24)
		hr -= 24;

	if (us.fEuroTime)
	{
		if (sec == -1)
			sprintf(szTim, "%2d:%02d", hr, min);
		else
			sprintf(szTim, "%2d:%02d:%02d", hr, min, sec);
	}
	else
	{
		if (sec == -1)
			sprintf(szTim, "%2d:%02d%s", Mod12(hr), min, hr < 12 ? Lang(sAM) : Lang(sPM));
		else
			sprintf(szTim, "%2d:%02d:%02d%s", Mod12(hr), min, sec, hr < 12 ? Lang(sAM) : Lang(sPM));
	}
	return szTim;
}


/* This just determines the correct hour and minute and calls the above. */

char *SzTim(double tim)
{
	/* Corrected by Michael Rideout on February 15, 2004: */
	int hr, min, sec;
	double rMin;

	hr = NFloor(tim);
	rMin = (tim - hr) * 100.0;
	min = (int) (rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	sec = (int) (60.0 * rFractal + 0.5);
	return SzTime(hr, min, sec);
}

/* Return a string containing the given time zone, given as a real value     */
/* having the hours before GMT in the integer part and minutes fractionally. */

char *SzZone(double zon)
{
	static char szZon[7];

	sprintf(szZon, "%c%d:%02d", zon > 0.0 ? '-' : '+', (int) fabs(zon),
			(int) (RFract(fabs(zon)) * 100.0 + 0.5 / 60.0));
	return szZon;
}


/* Nicely format the given longitude and latitude locations and return    */
/* them in a string. Various parts of the program display a chart header, */
/* and this allows the similar computations to be coded only once.        */

char *SzLocation(double lon, double lat)
{
	static char szLoc[32];
	int mini, minj, seci, secj;
	double rMin;
	char ch1, ch2, ch3;

	rMin = RFract(fabs(lon)) * 100.0;
	mini = (int) (rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	seci = (int) (60.0 * rFractal + 0.5);

	rMin = RFract(fabs(lat)) * 100.0;
	minj = (int) (rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	secj = (int) (60.0 * rFractal + 0.5);

	ch1 = us.fAnsiChar > 1 ? chDeg3 : chDeg1;
	ch2 = us.fAnsiChar > 1 ? 39 : chMin1;
	ch3 = us.fAnsiChar > 1 ? 34 : chSec1;
	if (us.fAnsiChar)
	{
		if (us.fAnsiChar != 3)
		{
			sprintf(szLoc, "%3.0f%c%02d%c%02d%c%c%3.0f%c%02d%c%02d%c%c",
					floor(fabs(lon)), ch1, mini, ch2, seci, ch3,
					lon < 0.0 ? 'E' : 'W', floor(fabs(lat)), ch1, minj, ch2, secj, ch3, lat < 0.0 ? 'S' : 'N');
		}
		else
		{
			sprintf(szLoc, "%3.0f%c%02d%c%02d%3.0f%c%02d%c%02d",
					floor(fabs(lon)), lon < 0.0 ? 'E' : 'W', mini, ch2, seci,
					floor(fabs(lat)), lat < 0.0 ? 'S' : 'N', minj, ch2, secj);
		}
	}
	else
	{
		sprintf(szLoc, "%3.0f%c%02d%c%02d%c%3.0f%c%02d%c%02d%c",
				floor(fabs(lon)), ch1, mini, ch2, seci,
				lon < 0.0 ? 'E' : 'W', floor(fabs(lat)), ch1, minj, ch2, secj, lat < 0.0 ? 'S' : 'N');
	}
	return szLoc;
}

/* Compute the date and time it is right now as the program is running      */
/* using the computer's internal clock. The time return value filled is		*/
/* expressed in the given zone parameter.									*/

void GetTimeNow(int *pmonth, int *pday, int *pyear, double *ptime, double zon)
{
	SYSTEMTIME	st;
	double		hour;
	int			month, day, year;
	
	GetSystemTime(&st);

	month = st.wMonth;
	day = st.wDay;
	year = st.wYear;
	hour = (double) st.wHour - zon;

	while (hour < 0.0) 
	{
		--day;
		hour += 24.0;
	}

	while (hour >= 24.0) 
	{
		++day;
		hour -= 24.0;
	}

	if(day < 1)
	{
		if(--month < 1)
		{
			month = 12;
			year--;
		}

		day += DayInMonth(month, year);
	}
	else if(day > DayInMonth(month, year))
	{
		day -= DayInMonth(month, year);

		if(++month > 12)
		{
			month = 1;
			year++;
		}
	}

	*pmonth = month;
	*pday = day;
	*pyear = year;
	*ptime = hour + (double) st.wMinute / 100.0 + (double) st.wSecond / 6000.0;
}

/* Given a string representing the complete pathname to a file, strip off    */
/* all the path information leaving just the filename itself. This is called */
/* by the main program to determine the name of the Astrolog executable.     */

char *ProcessProgname(char* szPath)
{
	char *b, *c, *e;

	b = c = szPath;
	while (*c)
	{
		*c = ChUncap(*c);		/* Because DOS filenames are case insensitive. */
		c++;
	}
	e = c;
	while (c > b && *c != '.')
		c--;
	if (c > b)
		*c = 0;
	else
		c = e;
	while (c > b && *c != chDirSep)
		c--;
	if (c > b)
		szPath = c + 1;
	return szPath;
}



/* This is Astrolog's memory allocation routine, returning a pointer given  */
/* a size, and if the allocation fails prints a warning, including the user */
/* passed as parameter.														*/

void *allocate(long length, char *user)
{
	void *p;

	p = malloc(length);

	if (p == NULL && user)
	{
		PrintWarning("Can't allocate memory for %s (%d bytes)", user, length);
	}

	return p;
}

// return true if file is valid
BOOL CheckFileTypeAndVersion(FILE *file, char *file_name, char *file_type, 
							 char *file_version)
{
	char	buffer[500];


	// read input line, check the beginning of the line, disregard the rest

	if(fgets(buffer, sizeof(buffer), file) && *buffer == '@' 
		&& !strncmp(file_type, buffer + 1, 2))
	{
		if(strncmp(file_version, buffer + 3, 2))
		{
			PrintWarning(Lang(sINCOMPATIBLE_FILE), file_name, file_version);
		}
		else
		{
			return TRUE;	// success
		}
	}
	else
	{
		PrintWarning(Lang(sINVALID_FILE), file_name, file_type);
	}

	return FALSE;
}

/* general.c */
