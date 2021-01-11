/*
** Astrolog File: strings_en.h
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

// =======================================================
// STRINGS USED IN THE WINDOWS INTERFACE - MAY USE ACCENTS
// =======================================================

//#define ST_WINDOW_NAME						szAppNameCore " version " szVersionCore \
	//" for MS Windows (32 bits)"
//#define ST_WITH_SWISS_EPHEMERIS				" with Swiss Ephemeris v. " szSwissEphemerisVersion
//#define ST_TRANSIT_OBJECT_RESTRICTIONS		"Transit Object Selections"
//#define ST_PROGRESSED_OBJECT_RESTRICTIONS	"Progressed Object Selections"
//#define ST_INCOMPATIBLE_CONFIG_FILE			"Incompatible configuration file:\nfile is v. %d, but v. %d is required."
//#define ST_INCOMPATIBLE_FILE				"Incompatible file '%s' (version '%s' required)"
//#define ST_INVALID_FILE						"Invalid file '%s' (does not start with '@%s')"
//#define ST_FILE_NOT_FOUND					"File '%s' not found in directory '%s'"
//#define ST_BUILT_ON							szAppNameCore " version " szVersionCore  	" was built on %s by Tomas Kubec."
//#define ST_ENTER_CHART_INFO					"Enter Chart #%d Data" 

//#define ST_SORT_ASPECTS_POWER_MENU_ITEM		"Sort aspects by power\tCtrl+x"
//#define ST_SORT_ASPECTS_ORBS_MENU_ITEM		"Sort aspects by orbs\tCtrl+x"

//#define ST_OPEN_MAIN_CHART					"Open Main Chart"
//#define ST_OPEN_CHART						"Open Chart #%d"
//#define ST_SAVE_MAIN_CHART_DATA				"Save Main Chart Data"
//#define ST_SAVE_CHART_POSITIONS				"Save Chart Positions"
//#define ST_SAVE_CHART_AS_TEXT_FILE			"Save Chart as Text File"
//#define ST_SAVE_CHART_AS_BITMAP				"Save Chart as Bitmap"
//#define ST_SAVE_CHART_AS_WMF				"Save Chart as WMF"
//#define ST_SAVE_CHART_AS_POSTSCRIPT			"Save Chart as PostScript"
//#define ST_SAVE_CURRENT_SETTINGS			"Save Current Settings"
//#define ST_YES								"Yes"
//#define ST_NO								"No"
//#define ST_DIRECTORIES_NOT_FOUND_OR_CREATED "The following directories could not be found or created:\n\n%s"
//#define ST_CONFIGURATION_FILE_NOT_FOUND		"The configuration file could not be found.\nWould you like to configure " szAppNameCore " now?"
//#define ST_WELCOME							"                Welcome to "
//#define ST_WARNING_NOT_ALL_PLACES_SHOWN		">> WARNING: NOT ALL PLACES WERE SHOWN. WE SUGGEST YOU NARROW YOUR CHOICE. <<" 
//#define ST_NO_PLACE_MATCHING_REQUIREMENTS	">> No place was found matching your requirements. <<"
//#define ST_NEED_CHOOSE_ATLAS_BEFORE_SEARCH	"You need to choose an atlas before searching"
//#define ST_INT_VALUE_NOT_VALID_SETTING		"Value %d is not a valid setting for %s"
//#define ST_FLOAT_VALUE_NOT_VALID_SETTING	"Value %f is not a valid setting for %s"
//#define ST_IN_FILE							" in file %s"
//#define ST_CHOOSE_DIRECTORY					"Choose directory for chart files"
//#define ST_ANY_ASPECT						"Any aspect"
//#define ST_ANY_OBJECT						"Any object"
//#define ST_ANY_SIGN							"Any sign"
//#define ST_SPEED_ABBREVIATION				"Speed"
//#define ST_GAUQUELIN_SECTOR					"Gauq. Sec."
//#define ST_WELCOME_WIZ_TITLE1				"Regional Choices"
//#define ST_WELCOME_WIZ_SUBTITLE1			"Choosing a region here just sets a few parameters in one go for your convenience. You can later modify your choices in the Settings menu."
//#define ST_WELCOME_WIZ_TITLE2				"Default Location and Timezone"
//#define ST_WELCOME_WIZ_SUBTITLE2			"Choose now a reference place and time zone. We suggest you choose the town where you live."
//#define ST_WELCOME_WIZ_TITLE3				"Directories"
//#define ST_WELCOME_WIZ_SUBTITLE3			"Astrolog32 has many files. Directories are used to keep them tidy. We suggest you keep the default directories for now, and you can change them later if necessary."



// ============================================
// STRINGS USED ELSEWHERE - CANNOT USE ACCENTS
// ============================================

//#define ST_POWER							"Power"
//#define ST_ORB								"Orb"
//#define ST_ASPECT_TITLE						"  Orb         Power\n\n"

// DrawInfo() strings

//#define ST_HOUSE1							"house"
//#define ST_HOUSES1							"%s houses."
//#define ST_TROPICAL							"Tropical"
//#define ST_SIDEREAL							"Sidereal"
//#define ST_SIDEREAL_TIME					"Sidereal time: %s"
//#define ST_HELIOCENTRIC						"Heliocentric"
//#define ST_GEOCENTRIC						"Geocentric"
//#define ST_JULIAN_DAY						"Julian Day: "
//#define ST_HARMONIC							"Harmonic"
//#define ST_OBLIQUITY						"Obliquity"
//#define ST_BONUS_MODE						"Bonus Mode"
//#define ST_INNER_RING						"Inner ring:"
//#define ST_OUTER_RING						"Outer ring:"
//#define ST_NO_TIME_OR_SPACE					"No time or space"
//#define ST_TIME_SPACE_MIDPOINT				"Time and space Midpoint"
//#define ST_COMPOSITE_CHART					"Composite chart"
//#define ST_SYNASTRY_HOUSES					"Synastry.  Houses:"
//#define ST_OUTER_RING_TRANSIT				"Outer ring - Transit:"

//#define ST_DEGREE_PER_YEAR_MONTH			"Degree per Year/Month -"
//#define ST_SOLAR_ARC						"Solar Arc -"
// the following two lines continue the two lines above 
//#define ST_OUTER_RING_DIRECTED_TO			"outer ring directed to:"	
//#define ST_CHART_DIRECTED_TO				"chart directed to:"

//#define ST_OUTER_RING_PROGRESSED_TO			"Outer ring progressed to:"
//#define ST_SECOND_RING						"Second ring:"
//#define ST_CHART_PROGRESSED_TO				"Chart progressed to:"
//#define ST_PLANETS							"Planets:"
//#define ST_BRUSSELS							"Brussels, Belgium"

// aspect info
//#define ST_HARM								"Harm = "
//#define ST_CONJ								"Conj = "
//#define ST_TENS								"Tens = "
//#define ST_TOTAL							"Total = "
//#define ST_SUMM								"Summ = "

//#define ST_FIRE_EARTH						"Fire: %d, Earth: %d,"
//#define ST_AIR_WATER						"Air : %d, Water: %d"
//#define ST_CAR_FIX_MUT						"Car: %d, Fix: %d, Mut: %d"
//#define ST_YANG_YIN							"Yang: %d, Yin: %d"
//#define ST_M_N_A_D							"M: %d, N: %d, A: %d, D: %d"
//#define ST_ANG_SUC_CAD						"Ang: %d, Suc: %d, Cad: %d"
//#define ST_LEARN_SHARE						"Learn: %d, Share: %d"



/*

*/

// strings_en.h


