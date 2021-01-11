/*
** Astrolog File: strings_fr.h
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

#define ST_WINDOW_NAME						szAppNameCore " version " szVersionCore \
	" pour MS Windows (32 bits)"
#define ST_WITH_SWISS_EPHEMERIS				" avec Ephémérides Suisses v. " szSwissEphemerisVersion
#define ST_TRANSIT_OBJECT_RESTRICTIONS		"Sélections Transit"
#define ST_PROGRESSED_OBJECT_RESTRICTIONS	"Sélections Progression"
#define ST_INCOMPATIBLE_CONFIG_FILE			"Fichier de configuration incompatible:\nfichier a v. %d, mais v. %d nécessaire."
#define ST_INCOMPATIBLE_FILE				"Fichier incompatible '%s' (version '%d' nécessaire)"
#define ST_INVALID_FILE						"Fichier invalide '%s' (ne commence pas avec '@%02d')"
#define ST_FILE_NOT_FOUND					"Fichier '%s' abscent du dossier '%s'"
#define ST_BUILT_ON							szAppNameCore " version " szVersionCore \
	" créé le %s par António Ramos."
#define ST_ENTER_CHART_INFO					"Entrée Données du Thème n°%d" 
#define ST_SORT_ASPECTS_POWER_MENU_ITEM		"Trier aspects par influence\tCtrl+x"
#define ST_SORT_ASPECTS_ORBS_MENU_ITEM		"Trier aspects par orbe\tCtrl+x"
#define ST_OPEN_MAIN_CHART					"Ouvrir thème principal"
#define ST_OPEN_CHART						"Ouvrir thème n°%d"
#define ST_SAVE_MAIN_CHART_DATA				"Enregistrer Thème principal"
#define ST_SAVE_CHART_POSITIONS				"Enregistrer Positions"
#define ST_SAVE_CHART_AS_TEXT_FILE			"Enregistrer Texte"
#define ST_SAVE_CHART_AS_BITMAP				"Enregistrer image Bmp"
#define ST_SAVE_CHART_AS_WMF				"Enregistrer image Wmf"
#define ST_SAVE_CHART_AS_POSTSCRIPT			"Enregistrer image Eps"
#define ST_SAVE_CURRENT_SETTINGS			"Enregistrer Configuration"
#define ST_YES								"Oui"
#define ST_NO								"Non"
#define ST_DIRECTORIES_NOT_FOUND_OR_CREATED "Impossible de trouver ou créer les dossiers suivants:\n\n%s"
#define ST_CONFIGURATION_FILE_NOT_FOUND		"Le fichier-configuration n'a pas été trouvé.\nVoulez-vous configurer " szAppNameCore " maintenant?"
#define ST_WELCOME							"                  Bienvenue sur "
#define ST_WARNING_NOT_ALL_PLACES_SHOWN		">> ATTENTION: TOUS LES LIEUX NE SONT PAS AFFICHES. PRECISEZ VOTRE DEMANDE, S.V.P. <<" 
#define ST_NO_PLACE_MATCHING_REQUIREMENTS	">> Aucun lieu ne correspond à votre demande. <<"
#define ST_NEED_CHOOSE_ATLAS_BEFORE_SEARCH	"Vous devez choisir un atlas avant de lancer la recherche."
#define ST_INT_VALUE_NOT_VALID_SETTING		"La valeur %d n'est pas acceptée pour %s"
#define ST_FLOAT_VALUE_NOT_VALID_SETTING	"La valeur %f n'est pas acceptée pour %s"
#define ST_IN_FILE							" dans fichier %s"
#define ST_CHOOSE_DIRECTORY					"Choisir le dossier contenant les thèmes à rechercher"
#define ST_ANY_ASPECT						"En aspect avec"
#define ST_ANY_OBJECT						"N'importe quoi"
#define ST_ANY_SIGN							"N'importe où"
#define ST_SPEED_ABBREVIATION				"Vit."
#define ST_GAUQUELIN_SECTOR					"Sec. Gauq."


#define ST_WELCOME_WIZ_TITLE1				"Choix de la Région"
#define ST_WELCOME_WIZ_SUBTITLE1			"En choisissant ici un des styles proposés, les principaux paramètres de présentation vont s'adapter à votre situation. Vous pourrez modifier ces choix plus tard à l'aide du menu REGLAGES."
#define ST_WELCOME_WIZ_TITLE2				"Lieu et Fuseau horaire par defaut"
#define ST_WELCOME_WIZ_SUBTITLE2			"Choisissez maintenant le lieu référent et le fuseau horaire lui correspondant. Nous vous conseillons de choisir la ville où vous vivez."
#define ST_WELCOME_WIZ_TITLE3				"Dossiers"
#define ST_WELCOME_WIZ_SUBTITLE3			"Astrolog32 utilise de nombreux fichiers. Les Dossiers sont utilisés pour les trier et les ranger selon leur fonction. Nous vous suggérons de conserver les dossiers définis ici par défaut, vous pourrez les modifier plus tard si nécessaire."


// ============================================
// STRINGS USED ELSEWHERE - CANNOT USE ACCENTS
// ============================================


#define ST_POWER							"Puissance"
#define ST_ORB								"Orbe"
#define ST_ASPECT_TITLE						" Orbe     Puissance\n\n"

// DrawInfo() strings

#define ST_HOUSE1							"maison"
#define ST_HOUSES1							"Maisons: %s."
#define ST_TROPICAL							"Tropical"
#define ST_SIDEREAL							"Sideral"
#define ST_SIDEREAL_TIME					"Temps sideral: %s"
#define ST_HELIOCENTRIC						"Heliocentrique"
#define ST_GEOCENTRIC						"Geocentrique"
#define ST_JULIAN_DAY						"Jour Julien: "
#define ST_HARMONIC							"Harmonique"
#define ST_OBLIQUITY						"Obliquite"
#define ST_BONUS_MODE						"Affichage modifie"
#define ST_INNER_RING						"Interieur:"
#define ST_OUTER_RING						"Exterieur:"
#define ST_NO_TIME_OR_SPACE					"Sans date ni lieu"
#define ST_TIME_SPACE_MIDPOINT				"Theme mi-espace mi-temps"
#define ST_COMPOSITE_CHART					"Theme composite"
#define ST_SYNASTRY_HOUSES					"Synastrie dans Theme 1:"
#define ST_OUTER_RING_TRANSIT				"Exterieur: Transit du"

#define ST_DEGREE_PER_YEAR_MONTH			"Degre par An/Mois -"
#define ST_SOLAR_ARC						"Arc Solaire -"
// the following two lines continue the two lines above 
#define ST_OUTER_RING_DIRECTED_TO			"Exterieur: Direction du"	
#define ST_CHART_DIRECTED_TO				"Theme Direction:"

#define ST_OUTER_RING_PROGRESSED_TO			"Exterieur: Progression du"
#define ST_SECOND_RING						"Au milieu:"
#define ST_CHART_PROGRESSED_TO				"Theme Progression:"
#define ST_PLANETS							"Planetes Theme 2:"
#define ST_BRUSSELS							"Bruxelles, Belgique"

// aspect info
#define ST_HARM								"Harm = "
#define ST_CONJ								"Conj = "
#define ST_TENS								"Tens = "
#define ST_TOTAL							"Total = "
#define ST_SUMM								"Somm = "

#define ST_FIRE_EARTH						"Feu: %d, Terre: %d,"
#define ST_AIR_WATER						"Air: %d, Eau: %d"
#define ST_CAR_FIX_MUT						"Car: %d, Fix: %d, Mut: %d"
#define ST_YANG_YIN							"Yang: %d, Yin: %d"
#define ST_M_N_A_D							"M: %d, N: %d, A: %d, D: %d"
#define ST_ANG_SUC_CAD						"Ang: %d, Suc: %d, Cad: %d"
#define ST_LEARN_SHARE						"Apprendre:%d, Partager:%d"

// strings_fr.h

