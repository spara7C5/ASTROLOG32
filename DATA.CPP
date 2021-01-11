/*
** Astrolog File: data.c
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
** calculation routines in the library SWISS EPHEMERIS and are programmed and
** copyright 1998 by Astrodienst AG.
** The use of that source code is subject to
** the Swiss Ephemeris Public License, available at 
** http://www.astro.ch/swisseph. This copyright notice must not be 
** changed or removed by any user of this program.
**
** Initial programming 8/28,30, 9/10,13,16,20,23, 10/3,6,7, 11/7,10,21/1991.
** X Window graphics initially programmed 10/23-29/1991.
** PostScript graphics initially programmed 11/29-30/1992.
** Last code change made 12/20/1998.
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
** Global Variables.
******************************************************************************
*/

US us = {

	/* Chart types */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	/* Chart suboptions */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TRUE, 0, 0,

	/* Table chart types */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, /*fDebug*/

	/* Main flags */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TRUE,

	/* Main subflags */
	FALSE, 0, 0, 0, 0, 0,

	/* Rare flags */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	/* Value settings */
	0,
	0,
	0,
	DEFAULT_SYSTEM,
	oEar,
	0,
	1.0,
	0,
	0,
	0,
	DIVISIONS,
	SCREENWIDTH,
	SE_SIDM_FAGAN_BRADLEY,
	0.0,
	DEFAULT_ZONE,
	DEFAULT_LONG,
	DEFAULT_LAT,
	"",
	false,			// useInternationalAtlas
	1,				//Advanced mode by default
	0, 0,			// ommit (regional) wizard		

	/* Value subsettings */

	4, 5, cPart, 0.0, 365.2421988, 1, 1, 24, 0, BIODAYS
};

IS is = {
	FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
	NULL, NULL, NULL, NULL, 0, 0, 0, 0.0, 0.0, 0.0,
	NULL, 0.0, 0.0, 0.0, 0.0, 0.0
};

// chart info
// month == -1 means "no time or space"

// Longitude and latitude in ciCore may be in degrees or radians, depending on location!
// In CastChart ciCore value is saved and later restored, but in the mean time, in 
// ProcessInput value is changed to radians! Mad!
CI ciCore = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", "" };
CI ciMain = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", "" };
CI ciTwin = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", "" };
CI ciThre = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", "" };
CI ciFour = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", "" };
CI ciTran = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", "" };
CI ciSave = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", "" };

// planet positions
CP cp0, cp1, cp2, cp3;

int fCP3 = 0; // this variable appears to be related to the usage of cp3, but how?


/*
******************************************************************************
** Global Arrays.
******************************************************************************
*/

double spacex[cLastMoving + 1], spacey[cLastMoving + 1];
double spacez[cLastMoving + 1], force[NUMBER_OBJECTS];
GridInfo *grid = NULL;
int starname[cStar + 1], kObjA[NUMBER_OBJECTS];
char *szMacro[NUMBER_OF_MACROS] = {0};
char szMacroCaption[NUMBER_OF_MACROS][256] = {0};

int kObjA1[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		/* Planets  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* Minors   */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* Cusps    */
	0, 0, 0, 0, 0, 0, 0, 0, 0			/* Uranians */
};								

/* Restriction status of each object, as specified with -YR switch. */

byte ignore[NUMBER_OBJECTS] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					/* Planets  */
	1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 				/* Minors   */
	0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,				/* Cusps    */
	1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Stars    */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1
};

/* Restriction of objects when transiting, as specified with -YRT switch. */

byte ignore2[NUMBER_OBJECTS] = { 0,
	0, 1, 0, 0, 0, 0, 0, 0, 0, 0,					/* Planets  */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 				/* Minors   */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,				/* Cusps    */
	1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Stars    */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1
};

/* Restriction of objects when progressing, as specified with -YRP switch.  */
/* For comparison charts only, else normal ignore used.						*/

byte ignore3[NUMBER_OBJECTS] = { 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1,					/* Planets  */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 				/* Minors   */
	0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,				/* Cusps    */
	1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Stars    */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1
};

/* Restriction of aspects as specified with -YRA switch. */
byte ignoreA[cAspect + 1] = { 0, 
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

byte ignorez[4] = { 0, 0, 0, 0 };	/* Restrictions for -Zd chart events. */

/* Gauquelin sector plus zones, as specified with -Yl switch. */

byte pluszone[cSector + 1] = { 0,
	1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
	1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1
};


/*
******************************************************************************
** Global Tables.
******************************************************************************
*/

const char *szAppName = szAppNameCore;


const double  rZon[cZone] = {
	10.30, 10.30, 10.30, 10.0, 10.0, 9.30, 9.0, 9.0, 9.0, 9.0, 8.0,
	8.0, 8.0, 8.0, 7.0, 7.0, 7.0, 7.0, 7.0, 6.0, 6.0,
	6.0, 6.0, 6.0, 5.0, 5.0, 5.0, 5.0, 5.0, 4.0, 4.0,
	4.0, 4.0, 4.0, 3.0, 3.0, 3.0, 3.0, 3.0, 2.0, 1.0,
	0.0, 0.0, 0.0, 0.0, -1.0, -2.0, -4.0, -5.0,
	-5.30, -5.30, -5.30, -6.0, -6.30, -7.0, -8.0, -9.0, -9.0, -9.0,
	-9.30, -10.0, -11.0, -11.30, -11.30, -11.30, -12.0, 24.0, 24.0, 24.0
};

double rAspAngle[cAspect + 1] = { 0,
	0.0, 180.0, 90.0, 120.0, 60.0, 150.0, 30.0, 45.0, 135.0, 72.0, 144.0,
	36.0, 360.0 / 7.0, 40.0, 80.0, 720.0 / 7.0, 1080.0 / 7.0, 160.0
};

double rAspOrb[cAspect + 1] = { 0,
	7.0, 7.0, 7.0, 7.0, 6.0, 3.0, 3.0, 3.0, 3.0, 2.0, 2.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
};

double rObjOrb[cLastMoving + 1] = { 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0
};

double rObjAdd[cLastMoving + 1] = { 0.0,
	1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

int ruler1[cLastMoving + 1] = { 0,
	5,  4, 3, 2, 1,  9,10,11, 12, 8,
	12, 2, 6, 7, 8, 11, 5, 8, 12, 7, 1,
	1,  2, 3, 4, 5,  6, 7, 8, 9, 10, 11, 12,
	7,  8, 5,10, 9,  6, 1, 6, 9
};
int ruler2[cLastMoving + 1] = { 0,
	0, 0, 6, 7, 8,12,11, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};
int exalt[cLastMoving + 1] = { 0,
	1, 2, 11, 12, 10, 4, 7, 8, 9, 6,
	4, 4, 10, 5, 11, 6, 12, 12, 9, 12, 10,
	5, 6, 7, 8, 9, 10, 11, 12, 1, 2, 3, 4,
	3, 6, 1, 9, 11, 8, 5, 3, 12
};

  /* This array is the reverse of the ruler arrays:   */
  /* Here, given a sign, return what planet rules it. */
int rules[NUMBER_OF_SIGNS + 1] = { 0,
	5, 4, 3, 2, 1, 3, 4, 10, 6, 7, 8, 9
};

// colour defaults

int kMainA[9] = {0, 15, 7, 8, 1, 2, 6, 4, 13};

int kRainbowA[8] = {15, 9, 3, 11, 10, 14, 12, 5};

int kElemA[4] = {kRed, kMaroon, kGreen, kBlue};

int kAspA[cAspect + 1] = {15,
	3, 12, 9, 10, 14, 13,
	13, 3, 3, 11, 6, 6, 
	1, 5, 5, 1, 1, 5
};

int kStar1 = kRed;
int kStar2 = kOrange;
int kStar3 = kMaroon;
int kCornerDecoration = kRed;

#ifdef INTERPRETALT
byte NoPrint = 0;
byte InterpretAlt = 0;
#endif

byte NoPrintHeader = 0;

/* Influence information used by ChartInfluence() follows. The influence of */
/* a planet in its ruling or exalting sign or house is tacked onto the last */
/* two positions of the object and house influence array, respectively.     */


  /* The inherent strength of each planet - */
double rObjInf[cLastMoving + 3] = { 20,
	30, 25, 10, 10, 10, 10, 10, 10, 10, 10,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	20, 10, 10, 10, 10, 10, 10, 10, 10, 15, 10, 10,
	3, 3, 3, 3, 3, 3, 3, 3, 3,
	20, 10
};

double ppower1[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

double ppower2[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

double ppowerTotal[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

  /* The inherent strength of each house - */
double rHouseInf[NUMBER_OF_SIGNS + 3] = { 0,
	20, 0, 0, 10, 0, 0, 5, 0, 0, 15, 0, 0,
	15, 5
};

  /* The inherent strength of each aspect - */
double rAspInf[cAspect + 1] = { 0.0,
	1.0, 0.8, 0.8, 0.6, 0.6, 0.4, 0.4, 0.2, 0.2,
	0.2, 0.2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
};

  /* The inherent strength of each planet when transiting - */
double rTransitInf[cLastMoving + 3] = { 10,
	10, 4, 8, 9, 20, 30, 35, 40, 45, 50,
	30, 15, 15, 15, 15, 30, 30, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	50, 50, 50, 50, 50, 50, 50, 50, 50
};

const double rObjDist[oVesta + 1] = { 149.59787, 0.0, 0.3844,
	57.91, 108.2, 227.94, 778.33, 1426.98, 2870.99, 4497.07, 5913.52,
	13.670 * 149.59787, 2.767 * 149.59787, 2.770 * 149.59787,
	2.669 * 149.59787,
	2.361 * 149.59787
};
const double rObjYear[oVesta + 1] = { 1, 0, 27.32166 / 365.25,
	87.969 / 365.25, 224.701 / 365.25, 686.98 / 365.25, 11.8623,
	29.458, 84.01, 164.79, 248.54,
	51.0, 4.60, 4.61, 4.36, 3.63
};
const double rObjDiam[oVesta + 1] = { 12756.0, 1392000.0, 3476.0,
	4878.0, 12102.0, 6786.0, 142984.0, 120536.0, 51118.0, 49528.0, 2300.0,
	320.0, 955.0, 538.0, 226.0, 503.0
};
const double rObjDay[oPlu + 1] = { 24.0, 30.0 * 24.0, 27.322 * 24.0,
	58.65 * 24.0, 243.01 * 24.0, 24.6229, 9.841, 10.233, 17.9, 19.2,
	6.3872 * 24.0
};
const double rObjMass[oPlu + 1] = { 1.0, 322946.0, 0.0123,
	0.0553, 0.8149, 0.1074, 317.938, 95.181, 14.531, 17.135, 0.0022
};					   
const double rObjAxis[oPlu + 1] = { 23.5, 0.0, 6.7,
	2.0, 2.7, 25.19, 3.12, 26.73, 82.14, 29.6, 57.54
};
const byte cSatellite[oPlu + 1] = { 1, 9, 0,
	0, 0, 2, 16, 18, 15, 8, 1
};

char *szStarConName[cStar + 1] = { "",
	"alEri\0   ", "alUMi\0   ", "zeRet\0   ", "etTau\0   ", "alTau\0   ",
	"alAur\0   ",
	"beOri\0   ", "gaOri\0   ", "beTau\0   ", "epOri\0   ", "alOri\0   ",
	"beAur\0   ",
	"beCMa\0   ", "alCar\0   ", "gaGem\0   ", "alCMa\0   ", "epCMa\0   ",
	"deCMa\0   ",
	"alGem\0   ", "alCMi\0   ", "beGem\0   ", "ga-2Vel\0 ", "epCar\0   ",
	"beCar\0   ",
	"alHya\0   ", "alLeo\0   ", "alUMa\0   ", "al-1Cru\0 ", "gaCru\0   ",
	"beCru\0   ",
	"epUMa\0   ", "alVir\0   ", "etUMa\0   ", "beCen\0   ", "alBoo\0   ",
	"alCen\0   ",
	"alSco\0   ", "laSco\0   ", "thSco\0   ", "epSgr\0   ", "alLyr\0   ",
	"alAql\0   ",
	"alPav\0   ", "alCyg\0   ", "alGru\0   ", "alPsA\0   ", "M31\0     ",
	"alAnd\0   ",
	"gaPeg\0   ", "alCas\0   ", "beAnd\0   ", "alPis\0   ", "ga-1And\0 ",
	"bePer\0   ",
	"deOri\0   ", "deGem\0   ", "alCnc\0   ", "beUMa\0   ", "epVir\0   ",
	"zeUMa\0   ",
	"beUMi\0   ", "al-2Lib\0 ", "beLib\0   ", "alCrB\0   ", "alSer\0   ",
	"alOph\0   ",
	"be-1Cyg\0 ", "alCep\0   ", "gaCap\0   ", "deAqr\0   ", "bePeg\0   ",
	"alPeg     ", 
	"SgrA*\0   ", "HerA*\0   ", "alAqr\0   "
};

double StarOrb = 1.2;
double PartOrb = 1.0;
double CoeffComp = 2.5;
double CoeffPar = 6.0;
double CoeffTens = 1.0;
double PowerPar = 0.25;
double SolidAspect = 8.0;
double SConjunct = 0.0;
double SHarmon = 0.0;
double STension = 0.0;
double SSumma = 0.0;
double SChart = 0.0;
double PrimeOrb1 = 1.16666667;
double PrimeOrb2 = 1.16666667;
int StarAspects = 1;
int PartAspects = 1;
int PartAspectsShow = 5;
int TensChart = 0;
int hRevers = 0;
int InfoFlag = 0;
int PrimeRest = 0;
int MonthFormat = FALSE;
byte StarRest = 0;
byte PolarMCflip = 0;
byte fEquator = 0;
byte oscLilith = 0;
byte fNESW = 0;
BOOL fSortAspectsByOrbs = 0; // if false, sorting is done by power
byte fDisp = FALSE;
//language_t language = LANGUAGE;
int CornerDecorationType = 0; // 0 = none, 1 = moire, 2 = spider
int CornerDecorationSize = 60;
int CornerDecorationNumberLines = 80;	// used only for spider web
#ifdef _DEBUG
BOOL fShowSecondChartInfoBar = TRUE;
#else
BOOL fShowSecondChartInfoBar = FALSE;
#endif

CI *LastChartShown = NULL;

// This must be in English, do not translate!!
char *szMonthEnglish[NUMBER_OF_MONTHS + 1] = { "",
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
};

const char* szSignAbbrevEnglish[NUMBER_OF_SIGNS + 1 ]  = { "",
	"Ari", "Tau", "Gem", "Can", "Leo", "Vir", "Lib", "Sco",
	"Sag","Cap", "Aqu", "Pis"
};

const char *szObjShortNameEnglish[NUMBER_OBJECTS] = {
	"Earth", "Sun ", "Moon", "Mercury", "Venus", "Mars",	/* Planets   */
	"Jupiter", "Saturn", "Uranus", "Neptune", "Pluto",
	"Chiron", "Ceres", "Pallas", "Juno", "Vesta",	/* Asteroids */
	"NoNode", "SoNode", "Lilith", "Fortune", "Vertex", "EaPoint",	/* Others    */
	// the extra space after IC is necessary for house wheels in text
	// mode to be correctly displayed
	"Ascendant", "2nd Cusp", "3rd Cusp", "IC ",	/* Cusps     */
	"5th Cusp", "6th Cusp", "Descendant", "8th Cusp",
	"9th Cusp", "Midheaven", "11th Cusp", "12th Cusp",
	"Cupido", "Hades", "Zeus", "Kronos",	/* Uranians  */
	"Apollon", "Admetos", "Vulkanus", "Poseidon", "Proserpina",

	// Stars 

	// Star names should not be translated, because they may be overwritten
	// by names from the fixed stars catalogue, that are in English.

	"Achernar\0    ", "Polaris\0     ", "Zeta Retic.\0 ", "Alcyone\0     ",
	"Aldebaran\0   ", "Capella\0     ", "Rigel\0       ", "Bellatrix\0   ",
	"Elnath\0      ", "Alnilam\0     ", "Betelgeuse\0  ", "Menkalinan\0  ",
	"Mirzam\0      ", "Canopus\0     ", "Alhena\0      ", "Sirius\0      ",
	"Adhara\0      ", "Wezen\0       ", "Castor\0      ", "Procyon\0     ",
	"Pollux\0      ", "Suhail\0      ", "Avior\0       ", "Miaplacidus\0 ",
	"Alphard\0     ", "Regulus\0     ", "Dubhe\0       ", "Acrux\0       ",
	"Gacrux\0      ", "Mimosa\0      ", "Alioth\0      ", "Spica\0       ",
	"Alkaid\0      ", "Agena\0       ", "Arcturus\0    ", "Toliman\0     ",
	"Antares\0     ", "Shaula\0      ", "Sargas\0      ", "Kaus Austr.\0 ",
	"Vega\0        ", "Altair\0      ", "Peacock\0     ", "Deneb\0       ",
	"Alnair\0      ", "Fomalhaut\0   ", "Andromeda\0   ", "Alpheratz\0   ",
	"Algenib\0     ", "Schedar\0     ", "Mirach\0      ", "Alrischa\0    ",
	"Almac\0       ", "Algol\0       ", "Mintaka\0     ", "Wasat\0       ",
	"Acubens\0     ", "Merak\0       ", "Vindemiatrix\0", "Mizar\0       ",
	"Kochab\0      ", "Zuben Elgen.\0", "Zuben Escha.\0", "Alphecca\0    ",
	"Unuk Alhai\0  ", "Ras Alhague\0 ", "Albireo\0     ", "Alderamin\0   ",
	"Nashira\0     ", "Skat\0        ", "Scheat\0      ", "Markab\0      ",
	"Gal.Center\0  ", "Apex\0        ", "Sadalmelik\0  "
};
const char *szSwitchName[] = {
	"Glyph", "CharSet", "DegCharacter", "UseInternationalAtlas", "OmitWizard", 
	"OmitRegionalWizard", "DebugFont", "SimplifiedMode", "AdvancedMode", "DisableMacroShortcuts",
	""
};

/*
******************************************************************************
** Object Calculation Tables.
******************************************************************************
*/

const byte rErrorCount[oPlu - oJup + 1] = { 11, 5, 4, 4, 4 };
const byte rErrorOffset[oPlu - oJup + 1] = { 0, 72, 72 + 51, 72 + 51 + 42, 72 + 51 + 42 + 42 };

const double  rErrorData[72 + 51 + 42 * 3] = {
	-.001, -.0005, .0045, .0051, 581.7, -9.7, -.0005, 2510.7, -12.5, -.0026,
	1313.7, -61.4,
	0.0013, 2370.79, -24.6, -.0013, 3599.3, 37.7, -.001, 2574.7, 31.4,
	-.00096, 6708.2,
	-114.5, -.0006, 5499.4, -74.97, -.0013, 1419, 54.2, .0006, 6339.3, -109,
	.0007, 4824.5,
	-50.9, .0020, -.0134, .0127, -.0023, 676.2, .9, .00045, 2361.4, 174.9,
	.0015, 1427.5,
	-188.8, .0006, 2110.1, 153.6, .0014, 3606.8, -57.7, -.0017, 2540.2, 121.7,
	-.00099,
	6704.8, -22.3, -.0006, 5480.2, 24.5, .00096, 1651.3, -118.3, .0006,
	6310.8, -4.8, .0007,
	4826.6, 36.2,				/* Jupiter error */

	-.0009, .0037, 0, .0134, 1238.9, -16.4, -.00426, 3040.9, -25.2, .0064,
	1835.3, 36.1,
	-.0153, 610.8, -44.2, -.0015, 2480.5, -69.4, -.0014, .0026, 0, .0111,
	1242.2, 78.3,
	-.0045, 3034.96, 62.8, -.0066, 1829.2, -51.5, -.0078, 640.6, 24.2, -.0016,
	2363.4,
	-141.4, .0006, -.0002, 0, -.0005, 1251.1, 43.7, .0005, 622.8, 13.7, .0003,
	1824.7, -71.1,
	.0001, 2997.1, 78.2,		/* Saturn error */

	-.0021, -.0159, 0, .0299, 422.3, -17.7, -.0049, 3035.1, -31.3, -.0038,
	945.3, 60.1,
	-.0023, 1227, -4.99, .0134, -.02186, 0, .0317, 404.3, 81.9, -.00495,
	3037.9, 57.3, .004,
	993.5, -54.4, -.0018, 1249.4, 79.2, -.0003, .0005, 0, .0005, 352.5,
	-54.99, .0001, 3027.5,
	54.2, -.0001, 1150.3, -88,	/* Uranus error */

	0.1832, -.6718, .2726, -.1923, 175.7, 31.8, .0122, 542.1, 189.6, .0027,
	1219.4, 178.1,
	-.00496, 3035.6, -31.3, -.1122, .166, -.0544, -.00496, 3035.3, 58.7,
	.0961, 177.1, -68.8,
	-.0073, 630.9, 51, -.0025, 1236.6, 78, .00196, -.0119, .0111, .0001,
	3049.3, 44.2, -.0002,
	893.9, 48.5, .00007, 1416.5, -25.2,	/* Neptune error */

	-.0426, .073, -.029, .0371, 372, -331.3, -.0049, 3049.6, -39.2, -.0108,
	566.2, 318.3,
	0.0003, 1746.5, -238.3, -.0603, .5002, -.6126, .049, 273.97, 89.97,
	-.0049, 3030.6, 61.3,
	0.0027, 1075.3, -28.1, -.0007, 1402.3, 20.3, .0145, -.0928, .1195, .0117,
	302.6, -77.3,
	0.00198, 528.1, 48.6, -.0002, 1000.4, -46.1	/* Pluto error */
};

OE rgoe[oVesta - 1 + cUran] = {
	{358.4758, 35999.0498, -.0002, .01675, -.4E-4, 0, 1, 101.2208, 1.7192,
	 .00045, 0, 0, 0, 0, 0, 0}
	,							/* Earth/Sun */
	{102.2794, 149472.515, 0, .205614, .2E-4, 0, .3871, 28.7538, .3703, .0001,
	 47.1459, 1.1852, 0.0002, 7.009, .00186, 0}
	,							/* Mercury */
	{212.6032, 58517.8039, .0013, .00682, -.5E-4, 0, .7233, 54.3842, .5082,
	 -.14E-2, 75.7796, 0.8999, .4E-3, 3.3936, .1E-2, 0}
	,							/* Venus */
	{319.5294, 19139.8585, .2E-3, .09331, .9E-4, 0, 1.5237, 285.4318, 1.0698,
	 .1E-3, 48.7864, 0.77099, 0, 1.8503, -.7E-3, 0}
	,							/* Mars */
	{225.4928, 3033.6879, 0, .04838, -.2E-4, 0, 5.2029, 273.393, 1.3383, 0,
	 99.4198, 1.0583, 0, 1.3097, -.52E-2, 0}
	,							/* Jupiter */
	{174.2153, 1223.50796, 0, .05423, -.2E-3, 0, 9.5525, 338.9117, -.3167, 0,
	 112.8261, .8259, 0, 2.4908, -.0047, 0}
	,							/* Saturn */
	{74.1757, 427.2742, 0, .04682, .00042, 0, 19.2215, 95.6863, 2.0508, 0,
	 73.5222, .5242, 0, 0.7726, .1E-3, 0}
	,							/* Uranus */
	{30.13294, 240.45516, 0, .00913, -.00127, 0, 30.11375, 284.1683, -21.6329,
	 0, 130.68415,
	 1.1005, 0, 1.7794, -.0098, 0}
	,							/* Neptune */
	{229.781, 145.1781, 0, .24797, .002898, 0, 39.539, 113.5366, .2086, 0,
	 108.944, 1.3739, 0, 17.1514, -.0161, 0}
	,							/* Pluto */
	{34.6127752, 713.5756219, 0, .382270369, -.004694073, 0, 13.66975144,
	 337.407213, 2.163306646, 0, 208.1482658, 1.247724355, 0, 6.911179715, .011236955, 0}
	,							/* Chiron */
	{108.2925, 7820.36556, 0, .0794314, 0, 0, 2.7672273, 71.0794444, 0, 0,
	 80.23555556, 1.3960111, 0, 10.59694444, 0, 0}
	,							/* Ceres */
	{106.6641667, 7806.531667, 0, .2347096, 0, 0, 2.7704955, 310.166111, 0, 0,
	 172.497222, 1.39601111, 0, 34.81416667, 0, 0}
	,							/* Pallas Athena */
	{267.685, 8256.081111, 0, .2562318, 0, 0, 2.6689897, 245.3752778, 0, 0,
	 170.137777, 1.396011111, .0003083333, 13.01694444, 0, 0}
	,							/* Juno */
	{138.7733333, 9924.931111, 0, .0902807, 0, 0, 2.360723, 149.6386111, 0, 0,
	 103.2197222, 1.396011111, .000308333, 7.139444444, 0, 0}
	,							/* Vesta */
	{104.5959, 138.5369, 0, 0, 0, 0, 40.99837, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Cupido   */
	{337.4517, 101.2176, 0, 0, 0, 0, 50.667443, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Hades    */
	{104.0904, 80.4057, 0, 0, 0, 0, 59.214362, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Zeus     */
	{17.7346, 70.3863, 0, 0, 0, 0, 64.816896, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Kronos   */
	{138.0354, 62.5, 0, 0, 0, 0, 70.361652, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Apollon  */
	{-8.678, 58.3468, 0, 0, 0, 0, 73.736476, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Admetos  */
	{55.9826, 54.2986, 0, 0, 0, 0, 77.445895, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Vulkanus */
	{165.3595, 48.6486, 0, 0, 0, 0, 83.493733, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Poseidon */
	{170.73, 51.05, 0, 0, 0, 0, 79.22663, 0, 0, 0, 0, 0, 0, 0, 0, 0}	
								/* Proserpina */
};

double  rStarBright[cStar + 1] = { 0,
	0.46, 2.02, 5.24, 5.09, 0.85, 0.08, 0.12, 1.64, 1.65, 1.70,
	0.50, 1.90, 1.98, -0.72, 1.93, -1.46, 1.50, 1.86, 1.58, 0.38,
	1.14, 1.82, 1.86, 1.68, 1.98, 1.35, 1.79, 1.58, 1.63, 1.25,
	1.77, 0.97, 1.86, 0.61, -0.04, -0.01, 0.96, 1.63, 1.87, 1.85,
	0.03, 0.77, 1.94, 1.25, 1.74, 1.16, 4.61,
	2.06, 2.83, 2.23, 2.29, 4.33, 2.25, 2.12, 2.23, 3.53, 4.25,
	2.37, 2.83, 2.27, 2.08, 2.75, 2.61, 2.23, 2.65, 2.08, 3.24,
	2.44, 3.68, 3.27, 2.42, 2.49, 9.99, 9.99, 2.96
};

const double  rStarData[cStar * 6] = {
	1, 37, 42.9, -57, 14, 12, 2, 31, 50.5, 89, 15, 51, 3, 17, 46.1, -62, 34,
	32,
	3, 49, 11.1, 24, 8, 12, 4, 35, 55.2, 16, 30, 33, 5, 16, 41.4, 45, 59, 53,
	5, 14, 32.3, -8, 12, 6, 5, 25, 7.9, 6, 20, 59, 5, 26, 17.5, 28, 36, 27,
	5, 36, 12.8, -1, 12, 7, 5, 55, 10.3, 7, 24, 25, 5, 59, 31.7, 44, 56, 51,
	6, 22, 42.0, -17, 57, 21, 6, 23, 57.1, -52, 41, 45, 6, 37, 42.7, 16, 23,
	57,
	6, 45, 8.9, -16, 42, 58, 6, 58, 37.6, -28, 58, 20, 7, 8, 23.5, -26, 23,
	36,
	7, 34, 36.0, 31, 53, 19, 7, 39, 18.1, 5, 13, 30, 7, 45, 18.9, 28, 1, 34,
	8, 9, 32.0, -47, 20, 12, 8, 22, 30.8, -59, 30, 35, 9, 13, 12.0, -69, 43,
	2,
	9, 27, 35.2, -8, 39, 31, 10, 8, 22.3, 11, 58, 2, 11, 3, 43.7, 61, 45, 3,
	12, 26, 35.9, -63, 5, 57, 12, 31, 9.9, -57, 6, 48, 12, 47, 43.2, -59, 41,
	19,
	12, 54, 1.7, 55, 57, 35, 13, 25, 11.6, -11, 9, 41, 13, 47, 32.4, 49, 18,
	48,
	14, 3, 49.4, -60, 22, 23, 14, 15, 39.7, 19, 10, 57, 14, 39, 35.9, -60, 50,
	7,
	16, 29, 24.4, -26, 25, 55, 17, 33, 36.5, -37, 6, 14, 17, 37, 19.2, -42,
	59, 52,
	18, 24, 10.3, -34, 23, 5, 18, 36, 56.3, 38, 47, 1, 19, 50, 47.0, 8, 52, 6,
	20, 25, 38.9, -56, 44, 6, 20, 41, 25.9, 45, 16, 49, 22, 8, 14.0, -46, 57,
	40,
	22, 57, 39.0, -29, 37, 20, 0, 42, 7.0, 41, 16, 0, 0, 8, 23.3, 29, 05, 26,
	0, 13, 14.0, 15, 11, 1, 0, 40, 30.4, 56, 32, 15, 1, 9, 43.9, 35, 37, 14,
	2, 2, 2.7, 2, 45, 51, 2, 3, 54.0, 42, 19, 47, 3, 8, 10.1, 40, 57, 20,
	5, 32, 0.4, -0, -17, 57, 7, 20, 7.4, 21, 58, 56, 8, 58, 29.2, 11, 51, 28,
	11, 1, 50.5, 56, 22, 57, 13, 2, 10.6, 10, 57, 33, 13, 23, 55.5, 54, 55,
	32,
	14, 50, 42.3, 74, 9, 20, 14, 50, 52.7, -16, 2, 30, 15, 17, 0.4, -9, 22,
	59,
	15, 34, 41.3, 26, 42, 53, 15, 44, 16.0, 6, 25, 32, 17, 34, 56.0, 12, 33,
	36,
	19, 30, 43.3, 27, 57, 35, 21, 18, 34.8, 62, 35, 8, 21, 40, 5.5, -16, 39,
	44,
	22, 54, 39.0, -15, 49, 15, 23, 3, 46.5, 28, 4, 58, 23, 4, 45.7, 15, 12,
	19,
	17, 45, 37.42, -29, 0, 21, 18, 4, 0.0,  30, 0,  0, 22, 5, 47.0, -0,19, 11.5 
};

/* data.c */
