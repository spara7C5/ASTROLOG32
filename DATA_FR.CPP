/*
** Astrolog File: data_fr.c
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
** Global Tables.
******************************************************************************
*/

byte *szSignName[NUMBER_OF_SIGNS + 1] = { "",
	"Belier", "Taureau", "Gemeaux", "Cancer", "Lion",
	"Vierge", "Balance", "Scorpion",
	"Sagittaire", "Capricorne", "Verseau", "Poissons"
};

const char *szSignAbbrev[NUMBER_OF_SIGNS + 1] = { "",
	"Be", "Ta", "Ge", "Cn", "Li", "Vi", "Ba", "Sc", "Sg", "Cp", "Ve", "Po"
};

const char *szSignEnglish[NUMBER_OF_SIGNS + 1] = { "",
	"Ram", "Bull", "Twins", "Crab", "Lion", "Virgin",
	"Scales", "Scorpion", "Archer", "Sea Goat", "Water Bearer", "Fishes"
};

const char *szHouseTradition[NUMBER_OF_SIGNS + 1] = { "",
	"Personnalite", "Possession", "Communication", "Racines",
	"Creativite", "Service", "Relation", "Transformation",
	"Expansion", "Vocation", "Projets", "Cloture"
};

byte *szObjName[NUMBER_OBJECTS] = {
	"Terre", "Soleil", "Lune", "Mercure", "Venus", "Mars",	/* Planets   */
	"Jupiter", "Saturne", "Uranus", "Neptune", "Pluton",
	"Chiron", "Ceres", "Pallas Athena", "Junon", "Vesta",	/* Asteroids */
	"Noeud Nord", "Noeud Sud", "Lilith", "Part Fortune", "Vertex", "Point Est",	/* Others    */
	// the extra space after IC is necessary for house wheels in text
	// mode to be correctly displayed
	"Ascendant", "2e Cusp", "3e Cusp", "FC ",	/* Cusps     */
	"5e Cusp", "6e Cusp", "Descendant", "8e Cusp",
	"9e Cusp", "Milieu Ciel", "11e Cusp", "12e Cusp",
	"Cupidon", "Hades", "Zeus", "Kronos",	/* Uranians  */
	"Apollon", "Admetos", "Vulkanus", "Poseidon", "Proserpine",

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

// This short name is used in the info bar and other places where only three 
// or four letters are printed. Make sure that these 3-4 letters are 
// meaningful and unique. The remaining letters are ignored, they are kept 
// here just for help.  
byte *szObjShortName[NUMBER_OBJECTS] = {
	"Terre", "Soleil", "Lune", "Mercure", "Venus", "Mars",	/* Planets   */
	"Jupiter", "Saturne", "Uranus", "Neptune", "Pluton",
	"Chiron", "Ceres", "Pallas", "Junon", "Vesta",	/* Asteroids */
	"NoNord", "NoSud", "Lilith", "Fortune", "Vertex", "PoEst",	/* Others    */
	// the extra space after IC is necessary for house wheels in text
	// mode to be correctly displayed
	"Ascendant", "2e Cusp", "3e Cusp", "FC ",	/* Cusps     */
	"5e Cusp", "6e Cusp", "Descendant", "8e Cusp",
	"9e Cusp", "Milieu Ciel", "11e Cusp", "12e Cusp",
	"Cupidon", "Hades", "Zeus", "Kronos",	/* Uranians  */
	"Apollon", "Admetos", "Vulkanus", "Poseidon", "Proserpine",

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

byte *szHouseSystem[NUMBER_OF_HOUSE_SYSTEMS] = {
	"Placidus", "Koch", "Egales(Asc)", "Campanus", "Meridian",
	"Regiomontanus", "Porphyry", "Morinus", "Topocentrique", "Alcabitius",
	"Egales(MC)", "Neo-Porphyry", "Entiere", "Vedique", "Aucune", "Indien inegal"
};

byte *szAspectName[cAspect + 1] = { "",
	"Conjonction", "Opposition", "Carre", "Trigone", "Sextile",
	"Quinconce", "Semisextile", "Semicarre", "Sesquicarre",
	"Quintile", "Biquintile",
	"Semiquintile", "Septile", "Novile",
	"Binovile", "Biseptile", "Triseptile", "Quatronovile"
};

byte *szAspectAbbrev[cAspect + 1] = { "",
	"Con", "Opp", "Car", "Tri", "Sex",
	"Qco", "SSx", "SCa", "Ses", "Qti", "BQt",
	"SQt", "Sep", "Nov", "BNv", "BSp", "TSp", "QNv"
};

byte *szAspectAbbrevCon = "Con";
byte *szAspectAbbrevOpp = "Opp";

const char *szAspectGlyph[cAspect + 1] = { "",
	"Cercle avec ligne tendue", "2 cercles joints par 1 ligne",
	"Carre", "Triangle", "Asterisque",
	"'K' tourne a droite", "'K' tourne a gauche", "Angle aigu",
	"Carre avec ligne tendue", "Lettre 'Q'", "'+' sur '-'",
	"'-' sur '+'", "Nombre '7'", "Nombre '9'", "'9' sous le chiffre Romain 'II'",
	"'7' sous le chiffre Romain 'II'", "'7' sous le chiffre Romain 'III'", 
	"'9' sous le chiffre Romain 'IV'"
};

const char *szAspectConfig[cAspConfig + 1] = { "",
	"Stellium", "Grand Trigone", "T-Carre", "Yod", "Grand Carre",
	"Baquet", "Rect Mystique", "Cerf-volant"
};

char *szElem[4] = { "Feu", "Terre", "Air", "Eau" };

char *szMode[3] = { "Cardinal", "Fixe", "Mutable" };

char *szMonth[NUMBER_OF_SIGNS + 1] = { "",
	"Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin",
	"Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre"
};

const char *szDay[cWeek] = { "Dimanche", "Lundi", "Mardi", "Mercredi",
	"Jeudi", "Vendredi", "Samedi"
};

const char * szZon[cZone] = {
	"HST", "HT", "H", "CAT", "AHS", "HDT", "AHD", "YST", "YT", "Y", "YDT",
	"PST", "PT", "P", "PDT", "PWT", "MST", "MT", "M", "MDT", "MWT",
	"CST", "CT", "C", "CDT", "CWT", "EST", "ET", "E", "EDT", "EWT",
	"AST", "AT", "A", "ADT", "AWT", "BST", "BT", "B", "BDT", "WAT",
	"GMT", "GT", "G", "WET", "CET", "EET", "UZ3", "UZ4",
	"IST", "IT", "I", "UZ5", "NST", "SST", "CCT", "JST", "JT", "J",
	"SAS", "GST", "UZ1", "NZT", "ZT", "Z", "IDL", "LMT", "LT", "L"
};

const char *szDir[4] = { "Nord", "Est", "Sud", "Ouest" };

const char *szSuffix[NUMBER_OF_SIGNS + 1] = { "",
	"e ", "e ", "e ", "e ", "e ", "e ", "e ", "e ", "e ", "e ", "e ", "e "
};

// colour names must be unambiguous to the extent of the length 
// of the shorter colour name
byte * szColor[cColor] = { "Noir",
	"Marron", "VertFc", "Orange", "BleuFc", "Violet", "CyanFc", "GrisCl",
	"GrisFc", "Rouge", "VertCl", "Jaune", "BleuCl", "Magenta", "CyanCl", "Blanc"
};

char *ayanamsha_short_name[LAST_SIDEREAL_MODE + 1] = 
{
	"Fagan/Bradley",
	"Lahiri",
	"De Luce",
	"Raman",
	"Ushashashi",
	"Krishnamurti",
	"Djwhal Khul",
	"Yukteshwar",
	"J. N. Bhasin",
	"Babyl. (Kugler 1)",
	"Babyl. (Kugler 2)",
	"Babyl. (Kugler 3)",
	"Babyl. (Huber)",
	"Babyl. (Eta Pisc.)",
	"Babyl. (Ald. 15 Tau)",
	"Hipparchos",
	"Sassanian",
	"Centre Galaxie 0 Sag",
};

char *ayanamsha_name[LAST_SIDEREAL_MODE + 1] = 
{
	"Fagan/Bradley",
	"Lahiri",
	"De Luce",
	"Raman",
	"Ushashashi",
	"Krishnamurti",
	"Djwhal Khul",
	"Yukteshwar",
	"J. N. Bhasin",
	"Babylonien (Kugler 1)",
	"Babylonien (Kugler 2)",
	"Babylonien (Kugler 3)",
	"Babylonien (Huber)",
	"Babylonien (Eta Piscium)",
	"Babylonien (Aldebaran � 15� Taureau)",
	"Hipparchos",
	"Sassanian",
	"Centre Galaxie � 0� Sagittaire",
};

global_directories_t dirs = 
{
	{ "",						"" },
	{ "Principal",				"C:\\astrolog32\\principal" },
	{ "�ph�m�rides",			"C:\\astrolog32\\�ph�m�rides" },
	{ "Th�mes",					"C:\\astrolog32\\th�mes" },
	{ "Interpr�tations",		"C:\\astrolog32\\interpr�tations" },
	{ "Divers",					"C:\\astrolog32\\divers" },
	{ "Atlas Am�ricain",		"C:\\astrolog32\\atlas\\am�ricain" },
	{ "Atlas International",	"C:\\astrolog32\\atlas\\international" },
};

// text that will be displayed in the About dialogue box
// Note: we need carriage return line feed sequences ("\r\n") for Windows to display
// correctly a new line (just '\n' is not enough).
char about_dialog_text[] =
{
	"By Walter D. Pullen (Astara@msn.com)\r\n"
	"Astrolog Homepage: http://www.magitech.com/~cruiser1/astrolog.htm\r\n"
	"Astrolog Homepage #2: http://www.astrolog.org/astrolog.htm\r\n"
	"\r\n"
	"Main planetary calculation formulas were converted from routines by "
	"James Neely, as listed in \"Manual of Computer Programming for "
	"Astrologers\" by Michael Erlewine, available from Matrix Software.\r\n"
	"\r\n"
	"PostScript graphics routines by Brian D. Willoughby.\r\n"
	"\r\n"
	"Extended ephemeris calculation and formulas are by Astrodienst AG, as "
	"in the library SWISS EPHEMERIS, available from Astrodienst AG.\r\n"
	"\r\n"
	"IMPORTANT: Astrolog is \"freeware\", but is copyrighted and not in "
	"public domain. Permission is granted to freely use and distribute these "
	"routines provided one does not sell, restrict, or profit from the program "
	"or its output in any way. Modification is allowed provided these exact "
	"notices remain with any altered or edited versions of the program. "
	"These conditions are true of both the program in whole and of all "
	"parts by any individual author. Violators are subject to copyright law "
	"penalties, and negative karmic debts to aforementioned contributors.\r\n"
	"\r\n"
	"Special thanks to all those unmentioned, seen and unseen, who have "
	"pointed out problems, suggested features, && sent many positive vibes! :)\r\n"
	"\r\n"
	"   ______________________________________________________\r\n"
	"\r\n"
	"Walter Pullen finished development of Astrolog 5.40 for Windows 16-bits "
	"on 20th December 1998.\r\n"
	"\r\n"
	"Alois Treindl added support for Swiss Ephemeris to version 5.41.\r\n"
	"\r\n"
	"Valentin Abramov made many improvements to Windows 16-bits versions "
	"from 5.30a through 5.31g and 5.41a through 5.41g, up to 26th April 2002.\r\n"
	"\r\n"
	"Ant�nio Ramos created Astrolog32 (Windows 32-bits version) based on "
	"5.41g. Version 1.0 was released on 5th March 2004. Later versions "
	"included improvements to user interface, language support, atlas, etc.\r\n"
	"\r\n"

	"Traduction fran�aise par Philippe Epaud et Ant�nio Ramos.\r\n"
	"\r\n"

	// the following line will contain the build date, set at a later stage;
	// note the form feed (\f), which will be replaced by the build date
	"\f\r\n"
	"\r\n"
	"   ______________________________________________________\r\n"
	"\r\n"
	"Page d'accueil d'Astrolog32: http://uk.geocities.com/astrolog32/\r\n"
	"\r\n"
	"Ce logiciel est fourni en l'�tat \"tel quel\", sans aucune sorte de garantie, "
	"explicite ou implicite. Les auteurs ou d�tenteurs de droits ne pourront �tre "
	"tenus pour responsables de tout dommage direct ou indirect, r�sultant de "
	"l'utilisation ou de l'ex�cution de ce logiciel."
};

#ifdef ARABIC
const AI ai[cPart] = {
	{"    02 01F ", "Fortune"},
	{"    01 02F ", "Esprit"},
	{"    06 S F ", "Victory"},
	{"    F  05F ", "Valor & Bravery"},
	{"    05 03Fh", "Mind & Administrators"},
	{"   h02r02Fh", "Property & Goods"},
	{"    06 07 h", "Siblings"},
	{"   j03 01Fh", "Death of Siblings"},
	{"    06 07Fh", "Death of Parents"},
	{"    07h02Fh", "Grandparents"},
	{"    06 03Fh", "Real Estate"},
	{"    07 06Fh", "Children & Life"},
	{"   R02 02 h", "Expected Birth"},
	{"    05 03 h", "Disease & Defects (1)"},
	{"    05 07 h", "Disease & Defects (2)"},
	{"   R07 07 h", "Captivity"},
	{"    02 03 h", "Servants"},
	{"   h07 04 h", "Partners"},
	{"   h08 02 h", "Death"},
	{"    05 07Fh", "Sickness & Murder"},
	{"    03 07Fh", "Danger, Violence & Debt"},
	{"   h09r09 h", "Journeys"},
	{"   105 07Fh", "Travel by Water"},
	{"    03 02Fh", "Faith, Trust & Belief"},
	{"    02 07Fh", "Deep Reflection"},
	{"    01 07Fh", "Understanding & Wisdom"},
	{"    06 01Fh", "Fame & Recognition"},
	{"    02 05Fh", "Rulers & Disassociation"},
	{"    07 01Fh", "Father, Fate & Karma"},	/* Combust */
	{"    F  07Fh", "Sudden Advancement"},
	{"    01 07 h", "Celebrity of Rank"},
	{"    07 05Fh", "Surgery & Accident"},
	{"    04 03Fh", "Merchants & Their Work"},
	{"    F  S Fh", "Merchandise (Exchange)"},	/* Moon */
	{"    02 04Fh", "Mother"},
	{"    S  F Fh", "Glory & Constancy"},
	{"    01 F Fh", "Honorable Acquaintances"},
	{"    06 F Fh", "Success"},
	{"    04 F Fh", "Worldliness"},
	{"    03 02 h", "Acquaintances"},
	{"    03 S  h", "Violence"},
	{"    01 03Fh", "Liberty of Person"},
	{"    04 06Fh", "Praise & Acceptance"},
	{"   h12r12 h", "Enmity"},
	{"    F  S  h", "Bad Luck"},
	{"    05 F F ", "Debilitated Bodies"},
	{"    02D  F ", "Boldness & Violence"},
	{"    S  03F ", "Trickery & Deceit"},
	{"   h03 05  ", "Necessities"},
	{"    03 F   ", "Realization of Needs"},
	{"    01 05F ", "Retribution"},
	{"    06 02  ", "Children (Male)"},
	{"    04 02  ", "Children (Female)"},
	{"    05 04  ", "Play & Variety"},	/* Change */
	{"    07 03 h", "Stability"},
	{"   h05 06Fh", "Speculation"},
	{"    03 04Fh", "Art"},
	{"   h05r05 h", "Sexual Attraction"},
	{"    10 04 h", "Sex Drive & Stimulation"},
	{"    05 01 h", "Passion"},
	{"    05 04 h", "Emotion & Affection"},
	{"   r08 07 h", "Most Perilous Year"},
	{"   h08 07 h", "Peril"},
	{"    09 08 h", "Occultism"},
	{"    03 01 h", "Commerce"},
	{"h09h03 04 h", "Marriage Contracts"},
	{"   h09r09 h", "Travel by Land"},
	{"    08H08 h", "Travel by Air"},
	{" 30 01 02Fh", "Destiny"},
	{" 30 02 01Fh", "Vocation & Status"},
	{"   019 01 h", "Honor, Nobility (Day)"},
	{"   033 02 h", "Honor, Nobility (Night)"},
	{"    10 01 h", "Organization"},
	{"    04h07 h", "Divorce"},
	{"    08 01 h", "Ostracism & Loss"},
	{"    02 08Fh", "Friends"},
	{"    07 01 h", "Tragedy & Brethren"},
	{"    02 10Fh", "Race (Consciousness)"},
	{"    02D02Fh", "Bondage & Slavery"},
	{"    F  09 h", "Imprisonment & Sorrow"},
	{"    04 08 h", "Perversion"},
	{"   h12 09 h", "Self-Undoing"},
	{"    09 01 h", "Treachery & Entrapment"},
	{"h12r12 09 h", "Bereavement"},
	{"    06h12 h", "Suicide (Yang)"},
	{"   h08 09 h", "Suicide (Yin)"},
	{"    06 09 h", "Depression"},
	{" 05 09 08 h", "Assassination (Yang)"},
	{"   r12 09 h", "Assassination (Yin)"},
	{"    09 06  ", "Cancer (Disease)"},
	{"    08 07  ", "Catastrophe"},
	{"    07 08  ", "Foolhardiness"},
	{"    03 05  ", "Release & Luck"},
	{"    06 03  ", "Benevolence & Assurance"},
	{"    03 06  ", "Hope & Sensitivity"},
	{"    03 07  ", "Aptness & Aloofness"},
	{"    08 09  ", "Charm & Personality"},
	{"    02 03F ", "Faith & Good Manners"},
	{"    01 03  ", "Temperament"},
	{"    04 03  ", "Security & Treasure"},
	{"    08 03  ", "Originality"},
	{"    03 08  ", "Eccentricity, Astrology"},
	{"    09 03  ", "Divination"},
	{"    03 09  ", "Intrusion"},
	{"    05 06  ", "Negotiation"},
	{"    06 05  ", "Discord & Controversy"},
	{"    05 08  ", "Coincidence"},
	{"    08 05  ", "Unpreparedness"},
	{"    05 09  ", "Popularity"},
	{"    09 05  ", "Misunderstanding"},
	{"    04 06  ", "Sentiment & Marriage"},
	{"    06 04  ", "Loneliness"},
	{"    04 07  ", "Success in Investment"},
	{"    07 04  ", "Frugality & Labor"},
	{"    08 04  ", "Wastefulness"},
	{"    04 09  ", "Vanity"},
	{"    09 04  ", "Corruptness"},
	{"    01 05  ", "Initiative"},
	{"    05 02F ", "Memory"},
	{"    04 01  ", "Love, Beauty & Peace"},
	{"    01 04  ", "Disinterest & Boredom"},
	{"    01 06  ", "Accomplishment"},
	{"    07 02F ", "Influence"},
	{"    06 01  ", "Increase & Impression"},
	{"    09 07  ", "Caution"},
	{"    07 09  ", "Timidity"},
	{"    08 06  ", "Entertainment"},
	{"    06 08  ", "Bequest"},
	{"    01 09  ", "Genie"},
	{"    02 09F ", "Revelation"},
	{"    09 02F ", "Delusion"},
	{"    08 02F ", "Misinterpretation"},
	{"    01 08  ", "Intellectuality"},
	{"    06 07 E", "Earth"},
	{"    04 02 E", "Water"},
	{"   r04 03 E", "Air & Wind"},
	{"    05 01 E", "Fire"},
	{"    07 05FE", "Clouds"},
	{"    04 02FE", "Rains"},
	{"    07 03FE", "Cold"},
	{"    06 01FC", "Wheat"},
	{"    06 02FC", "Barley & Meats"},
	{"    04 06FC", "Rice & Millet"},
	{"    07 06FC", "Corn"},
	{"    07 05FC", "Lentils, Iron, Pungents"},
	{"    05 07FC", "Beans & Onions"},
	{"    01 03FC", "Chick Peas"},
	{"    04 07FC", "Sesame & Grapes"},
	{"    03 04FC", "Sugar & Legumes"},
	{"    01 02FC", "Honey"},
	{"    02 05FC", "Oils"},
	{"    04 05FC", "Nuts & Flax"},
	{"    02 03FC", "Olives"},
	{"    05 07FC", "Apricots & Peaches"},
	{"    03 06FC", "Melons"},
	{"    05 02FC", "Salt"},
	{"    03 01FC", "Sweets"},
	{"    07 03FC", "Astrigents & Purgatives"},
	{"    04 03FC", "Silk & Cotton"},
	{"    05 07FC", "Purgatives (Bitter)"},
	{"    06 07FC", "Purgatives (Acid)"},
	{"    30D   H", "Secrets"},
	{"    02 03FH", "Information True/False"},
	{"    F D  FH", "Injury to Business"},
	{" 03 07 06 H", "Freedmen & Servants"},
	{" 02 07 06 H", "Employers"},
	{"   h07 04 H", "Marriage"},
	{"    06 01 H", "Time for Action/Success"},
	{"    07 01 H", "Time Occupied in Action"},
	{" 07 06 01 H", "Dismissal & Resignation"},
	{"    05 02 H", "Life/Death of Absentee"},
	{"    05 01 H", "Lost Animal (Light)"},
	{"    05 07 H", "Lost Animal (Dark)"},
	{"    03 05 H", "Lawsuit"},
	{"h08 05 02 H", "Decapitation"},
	{" 30 07 02 H", "Torture"},
	{" 02h04D   H", "Lost Objects"}
};
#endif /* ARABIC */

/*
******************************************************************************
** Constellation Tables.
******************************************************************************
*/

#ifdef CONSTEL
const byte * szCnstlName[cCnstl + 1] = { "",
	"Andromeda", "Antilia", "Apus", "Aquarius",
	"Aquila", "Ara", "Aries", "Auriga",
	"Bootes", "Caelum", "Camelopardalis", "Cancer",
	"Canes Venatici", "Canis Major", "Canis Minor", "Capricornus",
	"Carina", "Cassiopeia", "Centaurus", "Cepheus",
	"Cetus", "Chamaeleon", "Circinus", "Columba",
	"Coma Berenices", "Corona Australis", "Corona Borealis", "Corvus",
	"Crater", "Crux", "Cygnus", "Delphinus",
	"Dorado", "Draco", "Equuleus", "Eridanus",
	"Fornax", "Gemini", "Grus", "Hercules",
	"Horologium", "Hydra", "Hydrus", "Indus",
	"Lacerta", "Leo", "Leo Minor", "Lepus",
	"Libra", "Lupus", "Lynx", "Lyra",
	"Mensa", "Microscopium", "Monoceros", "Musca",
	"Norma", "Octans", "Ophiuchus", "Orion",
	"Pavo", "Pegasus", "Perseus", "Phoenix",
	"Pictor", "Pisces", "Piscis Austrinus", "Puppis",
	"Pyxis", "Reticulum", "Sagitta", "Sagittarius",
	"Scorpius", "Sculptor", "Scutum", "Serpens Caput/Cauda",
	"Sextans", "Taurus", "Telescopium", "Triangulum",
	"Triangulum Australe", "Tucana", "Ursa Major", "Ursa Minor",
	"Vela", "Virgo", "Volans", "Vulpecula"
};

const byte * szCnstlAbbrev[cCnstl + 1] = { "",
	"And", "Ant", "Aps", "Aqu", "Aql", "Ara", "Ari", "Aur",
	"Boo", "Cae", "Cam", "Cnc", "CVn", "CMa", "CMi", "Cap",
	"Car", "Cas", "Cen", "Cep", "Cet", "Cha", "Cir", "Col",
	"Com", "CrA", "CrB", "Crv", "Crt", "Cru", "Cyg", "Del",
	"Dor", "Dra", "Equ", "Eri", "For", "Gem", "Gru", "Her",
	"Hor", "Hya", "Hyi", "Ind", "Lac", "Leo", "LMi", "Lep",
	"Lib", "Lup", "Lyn", "Lyr", "Men", "Mic", "Mon", "Mus",
	"Nor", "Oct", "Oph", "Ori", "Pav", "Peg", "Per", "Phe",
	"Pic", "Psc", "PsA", "Pup", "Pyx", "Ret", "Sge", "Sgr",
	"Sco", "Scl", "Sct", "Ser", "Sex", "Tau", "Tel", "Tri",
	"TrA", "Tuc", "UMa", "UMi", "Vel", "Vir", "Vol", "Vul"
};

const byte * szCnstlMeaning[cCnstl + 1] = { "",
	"Chained Maiden", "Air Pump", "Bird of Paradise", "Water Bearer",
	"Eagle", "Altar", "Ram", "Charioteer",
	"Herdsman", "Chisel", "Giraffe", "Crab",
	"Hunting Dogs", "Great Dog", "Little Dog", "Sea Goat",
	"Keel", "Queen", "Centaur", "King",
	"Sea Monster", "Chameleon", "Drawing Compass", "Dove",
	" Berenice's Hair", "Southern Crown", "Northern Crown", "Crow",
	"Cup", "Southern Cross", "Swan", "Dolphin",
	"Dolphinfish", "Dragon", "Little Horse", "River",
	"Furnace", "Twins", "Crane", "Strongman",
	"Clock", "Sea Serpent", "Water Snake", "Indian",
	"Lizard", "Lion", "Little Lion", "Hare",
	"Scales", "Wolf", "Lynx", "Lyre",
	"Table Mountain", "Microscope", "Unicorn", "Fly",
	"Level", "Octant", "Serpent Bearer", "Hunter",
	"Peacock", "Winged Horse", "Hero", "Phoenix",
	"Painter", "Fishes", "Southern Fish", "Stern",
	"Compass", "Net", "Arrow", "Archer",
	"Scorpion", "Sculptor", "Shield", " Head/Tail of the Snake",
	"Sextant", "Bull", "Telescope", "Triangle",
	"Southern Triangle", "Toucan", "Great Bear", "Little Bear",
	"Sail", "Virgin", "Flying Fish", "Fox"
};

const byte * szCnstlGenitive[cCnstl + 1] = { "",
	"", "", "2odis", "", "", "", "1tis", "",
	"2is", "", "", "2ri", "2um1orum", " is", " is", "",
	"", "", "", "", "", "tis", "", "",
	"", "", "", "", "is", "", "", "",
	"1us", "nis", "", "", "", "1orum", "1is", "2is",
	"", "", "", "", "", "nis", "nis0is", "2oris",
	"", "", "", "", "", "", "1tis", "",
	"", "1is", "", "is", "nis", "", "", "",
	"is", "2ium", "", "", "1dis", "", "", "",
	"", "is", "", "!1is", "1tis", "", "", "",
	"2i1is", "", " is", " is", "1orum", "1inis", "1tis", ""
};
#endif /* CONSTEL */

#ifdef INTERPRET
/*
******************************************************************************
** Interpretation Tables.
******************************************************************************
*/

byte *szMindPart[cLastMoving + 1] = { "earthy nature",			// Earth
	"spirit, ego, image of self, and sense of aliveness",
	"emotional nature, moods, feelings, and caring tendencies",
	"thoughts, intellect, and communicative activity",
	"creativity, tendencies for affection and calmness, and relationship needs",
	"will, energy, activity, and aggressive, assertive tendencies",
	"enthusiastic, faithful, wise, expansive, spontaneous nature",
	"part of mind that is disciplined, respectful, and solitary",
	"individuality, desires for change, and tendency to go against social norms",
	"intuitive, spiritual, compassionate, psychic nature",
	"destiny, and capacity to transform the self and the outer world",	// Pluto
	"ability to help, heal, and teach others, and where one has much experience", // Chiron
	"tendency to direct energy to instinctive, creative, reproductive goals",
	"tendency to direct energy away from emotional and into mental pursuits",
	"tendency to give away individual power for the benefit of a relationship",
	"capacity to direct creative energy into personal or devotional goals",
	"karmic goals, and best direction of evolutionary growth", // North Node
	"karmic past, and area of experience but little growth",
	"capacity to let go and accept their natural darker side",
	"place where opportunity and success can be found",
	"karmic fate and method of reception of energies",
	"generalized projected personality and outward appearance",
	"personality and outward appearance, as projected to the world", // ascendant
	"", "", "", "", "", "", "", "",
	"view as seen in the eyes of others, reputation, and social standing", // midheaven
	"", "",
	"group associations and matters relating to art", // Uranians
	"dark, secretive, past, shameful side",
	"fiery, creative, leadership tendencies",
	"lofty, extraordinary parts",
	"progressive, abundant energies",
	"intense, focused nature",
	"greatly forceful energies",
	"sudden recovery ability",
	"idealistic, honorable side"
};

byte *szDesc[NUMBER_OF_SIGNS + 1] = { "",
	"forceful, energetic, direct, courageous",
	"practical, often skeptical and stubborn",
	"inquisitive, witty, perceptive, adaptable",
	"introspective, emotional, protective",
	"proud, gregarious, dramatic, dignified",
	"analytical, critical, modest, helpful",
	"affectionate, tolerant, often indecisive, appreciates beauty",
	"penetrating, suspicious, introspective",
	"jovial, open minded, loves freedom",
	"industrious, practical, disciplined",
	"progressive, erratic, revolutionary, idealistic, humanitarian, inventive",
	"imaginative, other worldly, impressionable"
};

byte *szDesire[NUMBER_OF_SIGNS + 1] = { "",
	"seeks adventure and challenge",
	"loves serenity and inner peace",
	"seeks out information",
	"wants inner and outer security",
	"desires self expression",
	"works toward perfection",
	"seeks balance, relationship, and calmness",
	"desires to transform and remove outer masks",
	"seeks meaning and new experience",
	"works for solitude and personal integrity",
	"desires individuality",
	"seeks transcendence of self"
};

byte *szLifeArea[NUMBER_OF_SIGNS + 1] = { "",
	"establishment of personal identity",
	"self image, self worth, and material security",
	"communicating to and receiving from the environment",
	"imagination, fantasies, inner feelings, and domestic life",
	"finding joy, pleasure, and creative expression",
	"work and feeling talented and useful",
	"personal relationships and intimacy",
	"sex, death, the occult, and other hard to grasp topics",
	"changes, exploration, and the breaking of routine",
	"career, social stature, and destiny",
	"the future, life goals, and association with friends and groups",
	"things that disrupt or cause disassociation with the personality"
};

byte *szInteract[cAspectInt + 1] = { "",
	"is %sconnected and fused together with",
	"%sopposes and creates tension with",
	"is %sin conflict with",
	"is %sin harmony with",
	"has %sopportunity for growth, exchange, and harmony in relation with",
	"is %sdifferent from",
	"%sgets new perspectives in relation with",
	"%screates internal friction with",
	"%screates internal agitation with",
	"%screatively relates externally with",
	"%screatively relates internally with"
};

byte *szTherefore[cAspectInt + 1] = { "",
	"Both parts are prominent in their psyche", "Balance is needed",
	"Adaptation is required by both sides", "", "",
	"They can often relate in a discordant way", "", "", "", "", ""
};

/* Modifier array makes the interpretation stronger for narrower orbs. */

byte * szModify[3][cAspectInt] = { {"always ", "always ", "irreconcilably ", "always ", "much ",
									   "completely ", "often ", "often ", "often ", "often ", "often "}
,
{"", "", "", "", "", "", "", "", "", "", ""}
,
{"somewhat ", "somewhat ", "somewhat ", "somewhat ", "some ", "somewhat ",
 "sometimes ", "sometimes ", "sometimes ", "sometimes ", "sometimes "}
};
#endif /* INTERPRET */

/* data_fr.c */
