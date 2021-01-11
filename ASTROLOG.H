/*
** Astrolog File: astrolog.h
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


/*
******************************************************************************
** Warning Level Switches For Debugging.
******************************************************************************
*/

/* Compiler warnings switches needed for MS Visual C++ when warning level
is set to 4. Used for linting. Don't remove. */

//VC 6
//#pragma warning( disable : 4068)
//#define DWORD_PTR DWORD*
//#define bpc (byte*)
//
#if 0
/*
warning 4706: assignment within conditional expression
warning 4057: 'function' : 'unsigned char *' differs in indirection 
     to slightly different base types from 'char [3]'
warning 4131: 'HousePlaceIn' : uses old-style declarator
warning 4244: '=' : conversion from 'int ' to 'unsigned char ', 
     possible loss of data
warning 4701: local variable 't_plac' may be used without 
     having been initialized
warning 4100: 'lParam' : unreferenced formal parameter
*/
#pragma warning( disable : 4706 4057 4131 4244 4701 4100)
#endif

/*
******************************************************************************
** Configuration Constants.
******************************************************************************
*/


/* FEATURES SECTION: */

#define _CRT_SECURE_NO_DEPRECATE

#define PS			/* Comment out this #define if you don't want the ability to */
					/* generate charts in the PostScript graphics format.        */

#define META		/* Comment out this #define if you don't want the ability to  */
					/* generate charts in the MS Windows metafile picture format. */

#define INTERPRET	/* Comment out this #define if you don't want the ability */
					/* to display interpretations of the various chart types. */

#define INTERPRETALT

#define ARABIC		/* Comment out this #define if you don't want any chart     */
					/* lists that include Arabic parts included in the program. */

#define CONSTEL		/* Comment out this #define if you don't want any of the */
					/* astronomical constellation charts in the program.     */

#define BIORHYTHM	/* Comment out this #define if you don't want the    */
					/* non-astrological biorhythm charts in the program. */

/*
** DATA CONFIGURATION SECTION: These settings describe particulars of
** your own location and where the program looks for certain info. It is
** recommended that these values be changed appropriately, although the
** program will still run if they are left alone.
*/

#define DEFAULT_LONG 0.0	/* Change these values to the longitude west    */
#define DEFAULT_LAT  0.0	/* and latitude north of your current location. */
							/* Use negative values for east/southern areas. */

#define DEFAULT_ZONE 0.0	/* Change this number to the time zone of your */
							/* current location in hours before (west of)  */
							/* GMT. Use negative values for eastern zones. */

/*
** OPTIONAL CONFIGURATION SECTION: Although not necessary, one may like
** to change some of the values below: These constants affect some of
** the default parameters and other such things.
*/

#define DEFAULT_SYSTEM 0	/* Normally, Placidus houses are used (unless the */
							/* user specifies otherwise). If you want a       */
							/* different default system, change this number   */
							/* to a value from 0..9 (values same as in -c).   */

#define DIVISIONS 48	/* Greater numbers means more accuracy but slower  */
						/* calculation, of exact aspect and transit times. */

/* Name of file to look in for default program parameters (which will */
/* override the compile time values here, if the file exists).        */
#define CONFIGURATION_FILE "astrolog32.dat"
#define AUTORUN_FILE "autorun.dat"

#define WHEELCOLS		16	/* Affects width of each house in wheel display.    */
#define WHEELROWS		11	/* Max no. of objects that can be in a wheel house. */
#define SCREENWIDTH		80	/* Number of columns to print interpretations in.   */
#define MONTHSPACE		3	/* Number of spaces between each calendar column.   */
#define MAXINDAY		150	/* Max number of aspects or transits displayable.   */
#define MAXCROSS		750	/* Max number of latitude crossings displayable.    */
#define CREDITWIDTH		74	/* Number of text columns in the -Hc credit screen. */
#define MAXSWITCHES		32	/* Max number of switch parameters per input line.  */
#define PSGUTTER		9	/* Points of white space on PostScript page edge.   */

/* For graphics, this char affects how bitmaps are written. 'N' is written 
like with the 'bitmap' program, 'C' is compacted somewhat (files have  
less spaces), and 'V' is compacted even more. 'A' means write as rectangular 
Ascii text file. 'B' means write as Windows bitmap (.bmp) file. */
#define BITMAPMODE 'B'	

/*
** By the time you reach here and the above values are customized as
** desired, Astrolog is ready to be compiled! Be sure to similarly
** change the values in the astrolog.dat file, which will override any
** corresponding compile time values here. Don't change any of the
** values in the section below unless you know what you're doing.
*/

#define BITMAPX    2730			/* Maximum window size allowed */
#define BITMAPY    2730
#define BITMAPX1    180			/* Minimum window size allowed */
#define BITMAPY1    180
#define DEFAULTX    600			/* Default window size */
#define DEFAULTY    600
#define SIDESIZE    160			/* Size of wheel chart information sidebar.    */
#define MAXMETA 200000L			/* Max bytes allowed in a metafile.            */
#define METAMUL      12			/* Metafile coordinate to chart pixel ratio.   */
#define PSMUL        11			/* PostScript coordinate to chart pixel ratio. */
#define CELLSIZE     14			/* Size for each cell in the aspect grid.      */
#define DEGINC        2			/* Number of degrees per segment for circles.  */
#define DEFORB      7.0			/* Min distance glyphs can be from each other. */
#define MAXSCALE    400			/* Max scale factor as passed to -Xs swtich.   */
#define TILTSTEP  11.25			/* Degrees to change when pressing '[' or ']'. */

#define BIODAYS      14			/* Days to include in graphic biorhythms.      */
//
//#define chH    (char)(us.fAnsiChar ? 196 : '-')	// ANSI and ASCII       
//#define chV    (char)(us.fAnsiChar ? 179 : '|')	// characters used to   
//#define chC    (char)(us.fAnsiChar ? 197 : '|')	// display text charts. 
//#define chNW   (char)(us.fAnsiChar ? 218 : '+')
//#define chNE   (char)(us.fAnsiChar ? 191 : '+')
//#define chSW   (char)(us.fAnsiChar ? 192 : '+')
//#define chSE   (char)(us.fAnsiChar ? 217 : '+')
//#define chJN   (char)(us.fAnsiChar ? 193 : '-')
//#define chJS   (char)(us.fAnsiChar ? 194 : '-')
//#define chJW   (char)(us.fAnsiChar ? 180 : '|')
//#define chJE   (char)(us.fAnsiChar ? 195 : '|')
//#define chDeg0 (char)(us.fAnsiChar ? /*0xFE*/248 : ' ')
//#define chDeg1 (char)(us.fAnsiChar ? /*0xB0*/248 : ':')
//#define chDeg2 248		// 0xFE replaced
//#define chDeg3 248		// where 248 was hardwired
//
//#define chMin1 (char)(us.fAnsiChar ?  39 : ':')
//#define chSec1 (char)(us.fAnsiChar ?  34 : ' ')


#define chH    '-'
#define chV    '|'
#define chC    '|'
#define chNW   '+'
#define chNE   '+'
#define chSW   '+'
#define chSE   '+'
#define chJN   '-'
#define chJS   '-'
#define chJW   '|'
#define chJE   '|'
//#define chDeg0 (char)(us.fAnsiChar ? /*0xB0*/'°' : ' ') 
//#define chDeg1 (char)(us.fAnsiChar ? /*0xB0*/'°' : ':') 
#define chDeg0 (char)(us.fAnsiChar ? /*0xB0*/ Lang(sDegSign)[2] : Lang(sDegSign)[3]) 
#define chDeg1 (char)(us.fAnsiChar ? /*0xB0*/Lang(sDegSign)[0] : Lang(sDegSign)[1]) 
#define chDeg2 Lang(sDegSign)[4]		// 0xFE replaced
#define chDeg3 Lang(sDegSign)[4]		// where 248 was hardwired

#define chMin1 '\''
#define chSec1 '"'

/*
******************************************************************************
** External Include Files.
******************************************************************************
*/

#define _WIN32_IE 0x0500	// minimum version is Windows 98 with Internet Explorer 5.
							// the default is 0x0501 with Platform SDK
// #define _WIN32_WINNT		// leave undefined for now

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <ctype.h>
#include "windows.h"
#include "swephexp.h"
#include "General/CharStrings.h"


/*
******************************************************************************
** Program Constants.
******************************************************************************
*/

#ifdef PS
#define STROKE
#endif

#ifdef META
#define STROKE
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#define nScrollDiv		24
#define nScrollPage		6
#define szFileTempCore	"ASTROLOG.TMP"

#define szAddressCore \
  "Astara@msn.com - http://www.magitech.com/~cruiser1/astrolog.htm"
#define szTtyCore  "tty"
#define cchSzDef   200
#define cchSzMax   255
#define yeaJ2G     1582
#define monJ2G     mOct
#define dayJ2G1    4
#define dayJ2G2    15
#define rPi        3.14159265358979323846
#define rPi2       (rPi*2.0)
#define rPiHalf    (rPi/2.0)
#define rDegRad    (180.0/rPi)
#define rEpoch2000 (-24.736467)
#define rSmall     (1.7453E-09)
#define rLarge     10000.0
#define chNull     '\0'
#define chEscape   '\33'
#define chBell     '\7'
#define chReturn   '\r'
#define chTab      '\t'
#define chDelete   '\b'
#define chBreak    '\3'
#define chRet      'R'
#define starMaxName 12
#define starMaxNName 8
#define cStar		75

/* Array index limits */
#define cCnstl     88
#define cZone      69
#define cSector    36
#define cPart      177
#define cAspConfig 8
#define cWeek      7
#define cColor     16
#define xFont      6
#define yFont      10

#define NUMBER_OF_MONTHS	12
#define NUMBER_OF_SIGNS		12
#define NUMBER_OF_HOUSES	12
#define NUMBER_OF_MACROS	120

#define FIRST_SIDEREAL_MODE		SE_SIDM_FAGAN_BRADLEY	// NOTE: this must always be zero!!
#define LAST_SIDEREAL_MODE		SE_SIDM_GALCENT_0SAG

/* Month index values */
typedef enum
{
	mJan = 1,
	mFeb,
	mMar,
	mApr,
	mMay,
	mJun,
	mJul,
	mAug,
	mSep,
	mOct,
	mNov,
	mDec
} month_t;

/* Elements */
typedef enum
{
	eFir,
	eEar,
	eAir,
	eWat
} elements_t;

/* Zodiac signs */
typedef enum
{
	sAri = 1,
	sTau,
	sGem,
	sCan,
	sLeo,
	sVir,
	sLib,
	sSco,
	sSag,
	sCap,
	sAqu,
	sPis
} zodiac_signs_t;

/* Objects */
typedef enum
{
	oEar,						// 0
	oSun,
	oMoo,
	oMer,
	oVen,
	oMar,
	oJup,
	oSat,
	oUra,
	oNep,
	oPlu,
	cLastMainPlanet = oPlu,		// 10

	oChi,						// 11
	oCeres,
	oPallas,
	oJuno,
	oVesta,
	cLastPlanet = oVesta,		// 15

	oNoNode,					// 16
	oSoNode, 
	oLil,
	cThing = oLil,				// 18

	oFor,						// 19
	oVtx,
	oEP, 
	oCore = oEP,				// 21

	cuspLo,						// 22
	oAsc = cuspLo,
	oCusp2nd,
	oCusp3rd,
	oNad,
	oCusp5th,
	oCusp6th,
	oDes,
	oCusp8th,
	oCusp9th,
	oMC,
	oCusp11th,
	oCusp12th,
	cuspHi = oCusp12th,			// 33

	uranLo,						// 34
	oCupido = uranLo,     
	oHades,      
	oZeus,       
	oKronos,     
	oApollon,    
	oAdmetos,    
	oVulkanus,   
	oPoseidon,   
	oProserpina,
	uranHi = oProserpina,		// 42
	cLastMoving = uranHi,		// 42	

	starLo,						// 43
	starHi = starLo + cStar - 1,// 117
	cObj = starHi,				// 117

	NUMBER_OBJECTS				// 118
} objects_t;

#define cUran (uranHi - uranLo + 1)	// 9
#define oOri (starLo - 1 + 10)
#define oAnd (starLo - 1 + 47)

/* Aspects */
typedef enum
{
	aCon = 1,
	aOpp,
	aSqu,
	aTri,
	aSex,
	aInc,				// 6
	aSSx,
	aSSq,
	aSes,
	aQui,
	aBQn,
	cAspectInt = aBQn,	// 11
	aSQn,
	aSep, 
	aNov, 
	aBNv, 
	aBSp, 
	aTSp, 
	aQNv,
	cAspect = aQNv		// 18
} aspects_t;

typedef enum
{
	cHousePlacidus,
	cHouseKoch,
	cHouseEqual,
	cHouseCampanus,
	cHouseMeridian,				// axial rotation system
	cHouseRegiomontanus,
	cHousePorphyry,
	cHouseMorinus,
	cHouseTopocentric,			// Polich/Page 
	cHouseAlcabitius,
	cHouseEqualMidheaven,		
	cHouseNeoPorphyry,
	cHouseWhole,
	cHouseVedic,				// Vehlow
	cHouseNull,					// each sign is a house, starting from Aries
	cHouseShripati,				// Sripati, Shripati, Bhava Chakra, Hindu Bhava, Hindu Unequal
	NUMBER_OF_HOUSE_SYSTEMS,	// 16
} house_system_t;

enum SwitchCode { scError = -1, scGlyph, scCharSet, scDegCharacter, 
	scUseInternationalAtlas, scOmitWizard, scOmitRegionalWizard, scDebugFont, scSimplifiedMode, 
	scAdvancedMode, scDisableMacroShortcuts, scTerminator 
};

#define aDir -2
#define aSig -1

/* Biorhythm cycle constants */
#define brPhy 23.0
#define brEmo 28.0
#define brInt 33.0

/* Relationship chart modes */
typedef enum
{
	rcProgress = -5,
	rcTransit,   
	rcQuadWheel, 
	rcTriWheel,  
	rcDual,      
	rcNone,			// 0
	rcSynastry,  
	rcComposite, 
	rcMidpoint,  
	rcDifference,
	rcBiorhythm,	// 5
} relat_chart_ty;

/* Aspect configurations */
#define acS  1
#define acGT 2
#define acTS 3
#define acY  4
#define acGC 5
#define acC  6
#define acMR 7
#define acK  8

/* chart modes */
#define gWheel      1
#define gHouse      2
#define gGrid       3
#define gHorizon    4
#define gOrbit      5
#define gSector     6
#define gCalendar   7
#define gDisposit   8
#define gAstroGraph 9
#define gEphemeris 10
#define gWorldMap  11
#define gGlobe     12
#define gPolar     13
#define gBiorhythm 14
#define gAspect    15
#define gMidpoint  16
#define gArabic    17
#define gSign      18
#define gObject    19
#define gHelpAsp   20
#define gConstel   21
#define gPlanet    22
#define gMeaning   23
#define gSwitch    24
#define gObscure   25
#define gKeystroke 26
#define gRising    28
#define gTraTraHit 29
#define gTraTraInf 30
#define gTraNatHit 31
#define gTraNatInf 32
#define gProgramStatus 33
#define gDebugFont 34


/* Colors */
#define kReverse -2
#define kDefault -1
#define kBlack   0
#define kMaroon  1
#define kDkGreen 2
#define kOrange  3
#define kDkBlue  4
#define kPurple  5
#define kDkCyan  6
#define kLtGray  7
#define kDkGray  8
#define kRed     9
#define kGreen   10
#define kYellow  11
#define kBlue    12
#define kMagenta 13
#define kCyan    14
#define kWhite   15
#define kNull    16

/* Arabic parts */
#define apFor 0
#define apSpi 1

/* Draw text formatting flags */
#define dtCent   0x0
#define dtLeft   0x1
#define dtBottom 0x2
#define dtErase  0x4
#define dtScale  0x8
#define dtTop    0x10

/* User string parse modes */
#define pmMon			1
#define pmDay			2
#define pmYea			3
#define pmTim			4
#define pmDst			5
#define pmZon			6
#define pmLon			7
#define pmLat			8
#define pmObject		9
#define pmAspect		10
#define pmHouseSystem	11
#define pmSign			12
#define pmColor			13
#define pmMonEn			14
#define pmSignEn		15
#define pmObjectEn		16

/* Termination codes */
#define tcError -1
#define tcOK    0
#define tcFatal 1
#define tcForce 2

#define chDirSep '\\'
#define chSwitch '/'
#define hdcNil ((HDC)NULL)

// File type and version numbers appear at the head of files, with the 
// form @xxyy, where xx is the file type and yy the version number. 
// These numbers are used for compatibility checks.

#define FILE_TYPE_CHART					"01"
#define FILE_VERSION_CHART				"02"

#define FILE_TYPE_PLANET_POSITION		02
#define FILE_VERSION_PLANET_POSITION	3

#define FILE_TYPE_CONFIGURATION			3
#define FILE_VERSION_CONFIGURATION		9

#define FILE_TYPE_COUNTRY_LIST			"04"	// for international and american atlas
#define FILE_VERSION_COUNTRY_LIST		"00"

#define FILE_TYPE_COUNTRY_FILE			"05"	// for international atlas
#define FILE_VERSION_COUNTRY_FILE		"00"

#define FILE_TYPE_USA_STATE_FILE		"06"	// for american atlas
#define FILE_VERSION_USA_STATE_FILE		"00"

#define FILE_TYPE_ALT_INTERPRET			"09"
#define FILE_VERSION_ALT_INTERPRET		"01"

#define BUILD_DATE "BUILD_DATE"

/*
******************************************************************************
** Type Definitions.
******************************************************************************
*/

typedef unsigned char byte;
typedef unsigned char *pbyte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned int uint;
typedef unsigned long KV;
typedef int KI;

// Windows accepts names up to 256 characters, which can be 512 bytes for 
// Unicode, however Astrolog does not do Unicode, so we use a size of 256. Note 
// that the Windows defile MAX_PATH actually has 260 bytes. For our purposes 
// using that would cause us some difficulties when comparing sizes, so we 
// create our own definition MAX_FILE_NAME. 
#define MAX_FILE_NAME	256
typedef char file_name_t[MAX_FILE_NAME + 1];	// File name or directory or a combination

typedef struct 
{
	char		*name;	// name used in user's interface
	file_name_t dir;	// actual directory
}
directory_t;

typedef struct 
{
	// This is the drive plus directory where astrolog32.exe resides. This 
	// directory is one of the places Astrolog looks for files.
	directory_t executable; 

	// we have several directories where we put different sorts of files.

	// Astrolog32.exe, astrolog32.dat, icons, URLs, documentation files, 
	// fixed stars catalogue.
	directory_t main; 

	// This directory will store Swiss Ephemeris files. Note that while a list 
	// of search directories is passed by Astolog32 to Swiss Ephemeris, Swiss 
	// Ephemeris has its own file search routine. For example, one of the places 
	// it looks for is the directory specified by the environment variable 
	// SE_EPHE_PATH. This is the current behaviour and will not be changed, as 
	// Astolog (currently) uses Swiss Ephemeris in its unchanged form. 
	directory_t ephemeris; 

	// Charts and planet position files.
	directory_t charts; 

	// Interpretation files (pl01.dat, house10.dat, etc).
	directory_t interpretation; 

	// Miscellaneous files, such as chart saved as either text, bitmap, Windows 
	// Meta File, or Postscript.
	directory_t miscel; 

	// Atlas files. 
	directory_t american_atlas; 
	directory_t international_atlas; 
}
global_directories_t;

typedef struct _GridInfo
{
	byte n[NUMBER_OBJECTS][NUMBER_OBJECTS];
	short v[NUMBER_OBJECTS][NUMBER_OBJECTS];
}
GridInfo;

typedef struct _CrossInfo
{
	double lat[MAXCROSS];
	double lon[MAXCROSS];
	int obj1[MAXCROSS];
	int obj2[MAXCROSS];
}
CrossInfo;

typedef struct _UserSettings
{
	/* Chart types */
	BOOL fListing;				/* -v */
	BOOL fWheel;				/* -w */
	BOOL fGrid;					/* -g */
	BOOL fAspList;				/* -a */
	BOOL fMidpoint;				/* -m */
	BOOL fHorizon;				/* -Z */
	BOOL fOrbit;				/* -S */
	BOOL fSector;				/* -l */
	BOOL fInfluence;			/* -j */
	BOOL fAstroGraph;			/* -L */
	BOOL fCalendar;				/* -K */
	BOOL fInDay;				/* -d */
	BOOL fInDayInf;				/* -D */
	BOOL fEphemeris;			/* -E */
	BOOL fTransit;				/* -t */
	BOOL fTransitInf;			/* -T */

	/* Chart suboptions */
	BOOL fVelocity;				/* -v0 */
	BOOL fWheelReverse;			/* -w0 */
	BOOL fGridConfig;			/* -g0 */
	BOOL fAppSep;				/* -ga */
	BOOL fParallel;				/* -gp */
	BOOL fAspSummary;			/* -a0 */
	BOOL fMidSummary;			/* -m0 */
	BOOL fMidAspect;			/* -ma */
	BOOL fPrimeVert;			/* -Z0 */
	BOOL fHorizonSearch;		/* -Zd */
	BOOL fSectorApprox;			/* -l0 */
	BOOL fInfluenceSign;		/* -j0 */
	BOOL fLatitudeCross;		/* -L0 */
	BOOL fCalendarYear;			/* -K  */
	BOOL fInDayMonth;			/* -dm */
	BOOL fArabicFlip;			/* -P0 */

	/* Table chart types */
	BOOL fSwitch;				/* -H  */
	BOOL fSwitchRare;			/* -Y  */
	BOOL fKeyGraph;				/* -HX */
	BOOL fSign;					/* -HC */
	BOOL fObject;				/* -HO */
	BOOL fAspect;				/* -HA */
	BOOL fConstel;				/* -HF */
	BOOL fOrbitData;			/* -HS */
	BOOL fMeaning;				/* -HI */
	BOOL fPrintProgramStatus;
	BOOL fDebugFont;

	/* Main flags */
	BOOL fSidereal;			/* -s */
	BOOL fProgressUS;		/* Are we doing a -p progressed chart?           */
	BOOL fInterpret;		/* Is -I interpretation switch in effect?        */
	BOOL fDecan;			/* -3 */
	BOOL fFlip;				/* -f */
	BOOL fGeodetic;			/* -G */
	BOOL fVedic;			/* -J */
	BOOL fNavamsa;			/* -9 */
	BOOL fWriteFile;		/* -o */
	BOOL fAnsiColor;		/* -k */
	BOOL fGraphics;			/* -X */

	/* Main subflags */
	BOOL fNoSwitches;
	BOOL fSeconds;			/* -b0 */
	BOOL fEquator;			/* -sr */
	BOOL fSolarArc;			/* -p0, etc */
	BOOL fWritePos;			/* -o0 */
	BOOL fAnsiChar;			/* -k0 */

	/* Rare flags */
	BOOL fTrueNode;			/* -Yn */
	BOOL fEuroDate;			/* -Yd */
	BOOL fEuroTime;			/* -Yt */
	BOOL fSmartAspects;		/* -YC */
	BOOL fClip80;			/* -Y8 */
	BOOL fHouseAngle;		/* -Yc */
	BOOL fIgnoreSign;		/* -YR0 */
	BOOL fIgnoreDir;		/* -YR0 */
	BOOL fNoWrite;			/* -0o */
	BOOL fNoRead;			/* -0i */
	BOOL fNoQuit;			/* -0q */
	BOOL fNoGraphics;		/* -0X */

	/* Value settings */
	int nEphemYears;			/* -Ey */
	int nArabic;				/* -P */
	int nRel;					/* What relationship chart are we doing, if any? */
	int nHouseSystem;			/* -c */
	int objCenter;				/* -h */
	int nStar;					/* -U */
	double nHarmonic;				/* Harmonic chart value passed to -x switch.     */
	int objOnAsc;				/* House value passed to -1 or -2 switch.        */
	int dayDelta;				/* -+, -- */
	int nDegForm;				/* -s */
	int nDivision;				/* -d */
	int nScreenWidth;			/* -I */
	int nSiderealMode;			/* -sm */
	double dstDef;				/* -z0 */
	double zonDef;				/* -z  */
	double lonDef;				/* -zl  */
	double latDef;				/* -zl  */
	char *szLocNameDef;			/* -zn */
	BOOL useInternationalAtlas;	/*#UseInternationalAtlas*/
	BOOL advancedMode;			/* #AdvancedMode or #SimplifiedMode*/
	BOOL omitWizard;			/* #OmitWizard */
	BOOL omitRegionalWizard;	/* #OmitRegionalWizard */

	/* Value subsettings */
	int nWheelRows;				/* Number of rows per house to use for -w wheel. */
	int nAstroGraphStep;		/* Latitude step rate passed to -L switch.       */
	int nArabicParts;			/* Arabic parts to include value passed to -P.   */
	double rSiderealCorrection;	/* Position shifting value passed to -s switch
									used in sidereal charts only.				 */
	double rProgDay;				/* Progression day value passed to -pd switch.   */
	int nRatio1;				/* Chart ratio factors passed to -rc or -rm.     */
	int nRatio2;
	int nScrollRow;				/* -YQ */
	int nArabicNight;			/* -YP */
	int nBioday;				/* -Yb */
}
US;

typedef struct _InternalSettings
{
	BOOL fProgressIS;			/* Are we doing a chart involving progression?       */
	BOOL fReturn;				/* Are we doing a transit chart for returns?         */
	int fMult;					/* Have we already printed at least one text chart?  */
	BOOL fSeconds;				/* Do we print locations to nearest second?          */
	BOOL fSzInteract;			/* Are we in middle of chart so some setting fixed?  */
	BOOL fNoEphFile;			/* Have we already had a ephem file not found error? */
	char *szProgName;			/* The name and path of the executable running.      */
	char *szFileScreen;			/* The file to send text output to as passed to -os. */
	char *szFileOut;			/* The output chart filename string as passed to -o. */
	char **rgszComment;			/* Points to any comment strings after -o filename.  */
	int cszComment;				/* The number of strings after -o that are comments. */
	int cchCol;					/* The current column text charts are printing at.   */
	int cchRow;					/* The current row text charts have scrolled to.     */
	double rSid;					/* Sidereal offset degrees to be added to locations. */
	double JD;					/* Fractional Julian day for current chart.          */
	double JDp;					/* Julian day that a progressed chart indicates.     */
	FILE *S;					/* File to write text to.   */
	double T;						/* Julian time for chart (expressed as number of	 */
								/* Julian centuries since noon GMT 1/Jan/1900)		 */
	double MC;					/* Midheaven at chart time. */
	double Asc;					/* Ascendant at chart time. */
	double RA;					/* Right ascension at time. */
	double rObliquity;					/* Obliquity of ecliptic.   */
}
IS;

typedef struct _ChartInfo
{
	int mon;					/* Month            */
	int day;					/* Day              */
	int yea;					/* Year             */
	double tim;					/* Time in hours    */
	double dst;					/* Daylight offset  */
	double zon;					/* Time zone        */
	double lon;					/* Longitude        */
	double lat;					/* Latitude         */
	char nam[256];					/* Name for chart   */
	char loc[256];					/* Name of location */
}
CI;

typedef struct _ChartPositions
{
	double longitude[NUMBER_OBJECTS];			/* The zodiac positions.    */
	double latitude[NUMBER_OBJECTS];			/* Ecliptic declination.    */
	double vel_longitude[NUMBER_OBJECTS];			/* Retrogradation velocity. */
	double vel_latitude[NUMBER_OBJECTS];		/* Speed in declination  */
	double cusp_pos[NUMBER_OF_HOUSES + 1];	/* House cusp positions.    */
	byte house_no[NUMBER_OBJECTS];			/* House each object is in. */
	double deltaT;
}
CP;

typedef struct _GraphicsSettings
{
	BOOL fBitmap;				/* Are we creating a bitmap file (-Xb set).         */
	BOOL fPS;					/* Are we generating a PostScript file (-Xp set).   */
	BOOL fMeta;					/* Are we generating a metafile graphic (-XM set).  */
	BOOL fColor;				/* Are we drawing a color chart (-Xm not set).      */
	BOOL fInverse;				/* Are we drawing in reverse video (-Xr set).       */
	BOOL fRoot;					/* Are we drawing on the X11 background (-XB set).  */
	BOOL fText;					/* Are we printing chart info on bottom of graphics chart (-Xt set).   */
	BOOL fFont;					/* Are we simulating fonts in charts (-XM0 set).    */
	BOOL fBonusMode;			/* Are we drawing in alternate mode (-Xi set).      */
	BOOL fBorder;				/* Are we drawing borders around charts (-Xu set).  */
	BOOL fLabel;				/* Are we labeling objects in charts (-Xl not set). */
	BOOL fJetTrail;				/* Are we not clearing screen on updates (-Xj set). */
	BOOL fMouse;				/* Are we not considering PC mouse inputs.          */
	BOOL fConstel;				/* Are we drawing maps as constellations (-XF set). */
	BOOL fMollewide;			/* Are we drawing maps scaled correctly (-XW0 set). */
	BOOL fPrintMap;				/* Are we printing globe names on draw (-XP0 set).  */
	int xWin;					/* Current size of graphic chart (-Xw).      */
	int yWin;
	int nAnimationJumpRate;		/* Current animation rate (-Xn).			*/
	int nScale;					/* Current character scale factor (-Xs).     */
	int objLeft;				/* Current object to place on Asc (-X1).     */
	int nTextRows;				/* Numb. of rows to set text screen to (-V). */
	int nRot;					/* Current rotation degree of globe.         */
	double rTilt;					/* Current vertical tilt of rotating globe.  */
	char chBmpMode;				/* Current bitmap file type (-Xb).           */
	int nOrient;				/* PostScript paper orientation indicator.   */
	double xInch;					/* PostScript horizontal paper size inches.  */
	double yInch;					/* PostScript vertical paper size inches.    */
	char *szDisplay;			/* Current X11 display name (-Xd).           */
	int nGridCell;				/* Number of cells in -g grids (-Yg).        */
	int nGlyphs;				/* Settings for what gylphs to use (-YXG).    */
	int charSet;					/* character set for TTF fonts */
}
GS;

typedef struct _GraphicsInternal
{
	int nCurrChart;				/* Current type of chart to create.           */
	BOOL fMono;					/* Is this a monochrome monitor.              */
	int kiCur;					/* Current color drawing with.                */
	pbyte bm;					/* Pointer to allocated memory.               */
	int cbBmpRow;				/* Horizontal size of bitmap array in memory. */
	char *szFileOut;			/* Current name of bitmap file (-Xo).         */
	FILE *file;					/* Actual file handle writing graphics to.    */
	int yBand;					/* Vertical offset to current bitmap band.    */
	double rAsc;					/* Degree to be at left edge in wheel charts. */
	BOOL fFile;					/* Are we making a graphics file.             */
	int nScale;					/* Scale ratio, i.e. percentage / 100.        */
	int nScaleT;				/* Relative scale to draw chart text at.      */
	int nPenWid;				/* Pen width to use when creating metafiles.  */
	KI kiOn;					/* Foreground color. */
	KI kiOff;					/* Background color. */
	KI kiLite;					/* Hilight color.    */
	KI kiGray;					/* A "dim" color.    */
	int xOffset;				/* Viewport origin.                           */
	int yOffset;
	int xTurtle;				/* Current coordinates of drawing pen.        */
	int yTurtle;
	int xPen;					/* Cached coordinates where last line ended.  */
	int yPen;
#ifdef PS						/* Variables used by the PostScript generator. */
	BOOL fEps;					/* Are we doing Encapsulated PostScript.    */
	int cStroke;				/* Number of items drawn without fluahing.  */
	BOOL fLineCap;				/* Are line ends rounded instead of square. */
	int nDash;					/* How much long are dashes in lines drawn. */
	int nFont;					/* What system font are we drawing text in. */
	double rLineWid;				/* How wide are lines, et al, drawn with.   */
#endif
#ifdef META						/* Variables used by the metafile generator.  */
	word *pwMetaCur;			/* Current mem position when making metafile. */
	word *pwPoly;				/* Position for start of current polyline.    */
	long cbMeta;
	KI kiLineAct;				/* Desired and actual line color. */
	KI kiLineDes;
	KI kiFillAct;				/* Desired and actual fill color. */
	KI kiFillDes;
	int nFontAct;				/* Desired and actual text font.  */
	int nFontDes;
	KI kiTextAct;				/* Desired and actual text color. */
	KI kiTextDes;
	int nAlignAct;				/* Desired/actual text alignment. */
	int nAlignDes;
#endif
}
GI;

typedef struct _ArabicInfo
{
	char *form;					/* The formula to calculate it. */
	char *name;					/* The name of the Arabic part. */
}
AI;

typedef struct _ElementTable
{
	int coSum;					/* Total objects considered.          */
	int coHemi;					/* Number that can be in hemispheres. */
	int coSign[NUMBER_OF_SIGNS];			/* Number of objects in each sign.    */
	int coHouse[NUMBER_OF_HOUSES];			/* Number of objects in each house.   */
	int coElemMode[4][3];		/* Objects in each elem/mode combo.   */
	int coElem[4];				/* Object in each element.            */
	int coMode[3];				/* Objects in each sign mode.         */
	int coModeH[3];				/* Objects in each house mode.        */
	int coYang;					/* Objects in Fire/Air signs.         */
	int coYin;					/* Objects in Earth/Water signs.      */
	int coLearn;				/* Objects in first six signs.        */
	int coShare;				/* Objects in last six signs.         */
	int coAsc;					/* Objects in Eastern houses.         */
	int coDes;					/* Objects in Western houses.         */
	int coMC;					/* Objects in Southern houses.        */
	int coIC;					/* Objects in Northern houses.        */
}
ET;

typedef struct _OrbitalElements
{
	double ma0, ma1, ma2;			/* Mean anomaly.           */
	double ec0, ec1, ec2;			/* Eccentricity.           */
	double sma;					/* Semi-major axis.        */
	double ap0, ap1, ap2;			/* Argument of perihelion. */
	double an0, an1, an2;			/* Ascending node.         */
	double in0, in1, in2;			/* Inclination.            */
}
OE;

typedef struct _WindowInternal
{
	HINSTANCE hinst;			/* Instance of the Astrolog window class.    */
	HWND hwndMain;				/* The outer created frame window.           */
	HWND hwnd;					/* The current window being dealt with.      */
	HMENU hmenu;				/* The Astrolog main menu bar.               */
	HACCEL haccel;				/* Keyboard accelerator or shortcut table.   */
	HDC hdc;					/* The current DC bring drawn upon.          */
	HDC hdcPrint;				/* The current DC being printed upon.        */
	HWND hwndAbort;				/* Window of the printing abort dialog.      */
	HPEN hpen;					/* Pen with the current line color.          */
	HBRUSH hbrush;				/* Fill if any with the current color.       */
	HFONT hfont;				/* Font of current text size being printed.  */
	UINT nTimer;				/* Identifier for the animation timer.       */
	int xScroll;				/* Horizontal & vertical scrollbar position. */
	int yScroll;
	int xClient;				/* Horizontal & vertical window size.		 */
	int yClient;
	int xChar;					/* Horizontal & vertical font character size.*/
	int yChar;
	int xMouse;					/* Horitontal & vertical mouse position.	 */
	int yMouse;
	WORD wCmd;					/* The currently invoked menu command.        */
	int nMode;					/* New chart type to switch to if any.        */
	BOOL fMenu;					/* Do we need to repaint the menu bar?        */
	BOOL fMenuAll;				/* Do we need to redetermine all menu checks? */
	BOOL fRedraw;				/* Do we need to redraw the screen?           */
	BOOL fCast;					/* Do we need to recast the chart positions?  */
	BOOL fAbort;				/* Did the user cancel printing in progress?  */
	int nDlgChart;				/* Which chart to set in Open or Info dialog. */
	BOOL fInitProgression;		/* hack to set chart data at beginning of progression, 
									to make animation possible */

	/* Window User settings. */
	BOOL fAnimate;				/* Animation runs if true */
	BOOL fDelayScreenRedraw;	/* Are we drawing updates off screen?     */
	BOOL fHourglass;			/* Bring up hourglass cursor on redraws?  */
	BOOL fChartWindow;			/* Does chart change cause window resize? */
	BOOL fWindowChart;			/* Does window resize cause chart change? */
	BOOL fNoUpdate;				/* Do we not automatically update screen? */
	KI kiPen;					/* The current pen scribble color.        */
	int nAnimationJumpFactor;	/* Time increases if positive, decreases if negative  */
	UINT nTimerDelay;			/* Milliseconds between animation draws.  */
	int xPrint;					/* Hor. and ver. resolution for printing  */
	int yPrint;
	BOOL fPrinting;				/* TRUE if we are printing				  */
	bool fDisableMacroShortcuts; /*true if menu shortcuts for macros should be hidden on update*/
}
WI;

//typedef enum
//{
//	cEnglish,
//	cFrench,
//} language_t;

typedef enum
{
	ANIMATION_FACTOR_SECONDS = 1,
	ANIMATION_FACTOR_MINUTES,
	ANIMATION_FACTOR_HOURS,
	ANIMATION_FACTOR_DAYS,
	ANIMATION_FACTOR_MONTHS,
	ANIMATION_FACTOR_YEARS,
	ANIMATION_FACTOR_DECADES,
	ANIMATION_FACTOR_CENTURIES,
	ANIMATION_FACTOR_MILLENNIA,
	ANIMATION_FACTOR_HERE_AND_NOW,
} animation_factor_t;


// operations on a chart stack
typedef enum		
{
	CHART_CREATE,
	CHART_DESTROY,
	CHART_PREVIOUS,	
	CHART_NEXT,	
} chart_ops_t;


typedef struct _chart_unit	
{
	CI					user_chart;	// must always be the first element in structure
	struct _chart_unit	*previous;
	struct _chart_unit	*next;
	int					magic_number;
} chart_unit_t;

typedef enum 
{
	CHART_HEAP_SEARCH,
	CHART_HEAP_SORTED,
	NO_CHART_HEAPS,
};


/*
******************************************************************************
** Astrolog Include Files.
******************************************************************************
*/

#include "version.h"
#include "extern.h"
#include "macro_functions.h"

// language files and definitions
// ----- English -----
#if defined(LANGUAGE_ENGLISH)

#include "resources_en.h"
#include "strings_en.h"

#include "gui.h"

#define LANGUAGE cEnglish
//#define COUNTRY_LIST "country_list.atl"
//#define USA_STATE_LIST "states_list.atl"

// ----- French -----
#elif defined(LANGUAGE_FRENCH)

#include "resources_fr.h"
#include "strings_fr.h"

//#define LANGUAGE cFrench
//#define COUNTRY_LIST "liste_pays.atl"
//#define USA_STATE_LIST "liste_etats.atl"

#else
Error: unsupported language
#endif


// astrolog.h 
