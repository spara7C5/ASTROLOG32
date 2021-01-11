/*
** Astrolog File: etc_en.c
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


/* Print a list of every key that one can press in a graphics window to do  */
/* a certain function, and a description of what it does, as displayed when */
/* one presses the 'H' or '?' key, and as shown with the -HX switch.        */


void DisplayKeysX()
{
	char sz[cchSzDef];

	sprintf(sz, "%s keyboard shortcuts:\n", szAppName);
	PrintS(sz);
	PrintS(" Press 'H' or '?' to display this list of key options.");
	PrintS(" Press 'p' to toggle pause status on or off.");
	PrintS(" Press 'x' to toggle fg/bg colors on screen.");
	PrintS(" Press 'm' to toggle color/monochrome display on screen.");
	PrintS(" Press 'i' to toggle status of the minor chart modification.");
	PrintS(" Press 't' to toggle header info on current chart on screen.");
	PrintS(" Press 'b' to toggle drawing of a border around the chart.");
	PrintS(" Press 'l' to toggle labeling of object points in chart.");
	PrintS(" Press 'j' to toggle not clearing screen between chart updates.");
	PrintS(" Press 'v' to display current chart positions on text screen.");
	PrintS(" Press 'U' to toggle restriction status of stars.");
	PrintS(" Press 'c' to toggle relationship comparison chart mode.");
	PrintS(" Press 's', 'h', 'f', 'g', 'z', 'y' to toggle status of sidereal");
	PrintS("       zodiac, heliocentric charts, domal charts, decan charts,");
	PrintS("       vedic format wheel charts, and navamsa charts.");
	PrintS(" Press 'O' and 'o' to recall/store a previous chart from memory.");
	PrintS(" Press 'B' to resize chart display to full size of screen.");
	PrintS(" Press 'Q' to resize chart display to a square.");
	PrintS(" Press '<' and '>' to decrease/increase the scale size of the");
	PrintS("       glyphs and the size of world map.");
	PrintS(" Press '[' and ']' to decrease/increase tilt in globe display.");
	PrintS(" Press '+' and '-' to add/subtract a day from current chart.");
	PrintS(" Press 'n' to set chart information to current time now.");
	PrintS(" Press 'N' to toggle animation status on or off. Charts will");
	PrintS("       be updated to current status and globe will rotate.");
	PrintS(" Press '!'-'(' to begin updating current chart by adding times.");
	PrintS("       !: seconds, @: minutes, #: hours, $: days, %: months,");
	PrintS("       ^: years, &: years*10, *: years*100, (: years*1000.");
	PrintS(" Press 'r' to reverse direction of time-lapse or animation.");
	PrintS(" Press '1'-'9' to set rate of animation to 'n' degrees, etc.");
	PrintS(" Press 'V','A','Z','S','M','K','J','L','E','W','G','P' to switch to");
	PrintS("       normal (_v), grid (_g), local (_Z), space (_S), sector (_l),");
	PrintS("       calendar (_K), dispositor (_j), astro-graph (_L), ephemeris");
	PrintS("       (_E), world map (_XW), globe (_XG), and polar (_XP) modes.");
	PrintS(" Press 'Y' to switch to biorhythm relation chart mode.");
	PrintS(" Press '0' to toggle between _Z,_Z0 & _XW,_XW0 & _E,_Ey modes.");

#ifdef CONSTEL
	PrintS(" Press 'F' to toggle between world and constellation map modes.");
#endif

	PrintS(" Press 'space' to force redraw of current graphics display.");
	PrintS(" Press 'del' to clear the graphics screen and not redraw.");
	PrintS(" Press 'page up','page down','home','end' to move up or down");
	PrintS("       the screen.");
	PrintS(" Press 'enter' to input a command line of general switches.");
	PrintS(" Press 'q' to terminate graphics and the program.");
	PrintL();
	PrintS(" Left  mouse button: Draw line strokes on chart in screen.");
	PrintS(" Right mouse button: Set coordinates to pointer on world map.");
}

/* Print a list of every command switch that can be passed to the program, */
/* and a description of what it does. This is what the -H switch prints.   */

void DisplaySwitches()
{
	char sz[cchSzDef];

	sprintf(sz, "Command switches (version %s):\n", szVersionCore);
	PrintS(sz);
	PrintS(" _H: Display this help list.");
	PrintS(" _HC: Display names of zodiac signs and houses.");
	PrintS(" _HO: Display available planets and other celestial objects.");
	PrintS(" _HA: Display available aspects, their angles, and present orbs.");
#ifdef CONSTEL
	PrintS(" _HF: Display names of astronomical constellations.");
#endif
	PrintS(" _HS: Display information about planets in the solar system.");
#ifdef INTERPRET
	PrintS(" _HI: Display meanings of signs, houses, planets, and aspects.");
#endif
	sprintf(sz, " _He: Display all info tables together (_Hc_H_Y_HX_HC_HO_HA%s_HS%s).",
#ifdef CONSTEL
			"_HF",
#else
			"",
#endif
#ifdef INTERPRET
			"_HI");
#else
			"");
#endif
	PrintS(sz);
	PrintS(" _Q: Prompt for more command switches after display finished.");
	PrintS(" _Q0: Like _Q but prompt for additional switches on startup.");
	PrintS(" _M <1-100>: Run the specified command switch macro.");
	PrintS(" _M0 <1-100> <string>: Define the specified command switch macro.");
	PrintS(" _Y: Display help list of less commonly used command switches.");
	PrintS("\nSwitches which determine the type of chart to display:\n");
	PrintS(" _v: Display list of object positions (chosen by default).");
	PrintS(" _v0: Like _v but express velocities relative to average speed.");
	PrintS(" _w [<rows>]: Display chart in a graphic house wheel format.");
	PrintS(" _w0 [..]: Like _w but reverse order of objects in houses 4..9.");
	PrintS(" _g: Display aspect and midpoint grid among planets.");
	PrintS(" _g0: Like _g but flag aspect configurations (e.g. Yod's) too.");
	PrintS(" _g0: For comparison charts, show midpoints instead of aspects.");
	PrintS(" _ga: Like _g but indicate applying instead of difference orbs.");
	PrintS(" _gp: Like _g but generate parallel and contraparallel aspects.");
	PrintS(" _a: Display list of all aspects ordered by influence.");
	PrintS(" _a0: Like _a but display aspect summary too.");
	PrintS(" _a[0]a: Like _a but indicate applying and separating orbs.");
	PrintS(" _a[0]p: Like _a but do parallel and contraparallel aspects.");
	PrintS(" _m: Display all object midpoints in sorted zodiac order.");
	PrintS(" _m0: Like _m but display midpoint summary too.");
	PrintS(" _ma: Like _m but show aspects from midpoints to planets as well.");
	PrintS(" _Z: Display planet locations with respect to the local horizon.");
	PrintS(" _Z0: Like _Z but express coordinates relative to polar center.");
	PrintS(" _Zd: Search day for object local rising and setting times.");
	PrintS(" _S: Display x,y,z coordinate positions of planets in space.");
	PrintS(" _l: Display Gauquelin sectors for each planet in chart.");
	PrintS(" _l0: Like _l but approximate sectors using Placidus cusps.");
	PrintS(" _j: Display astrological influences of each object in chart.");
	PrintS(" _j0: Like _j but include influences of each zodiac sign as well.");
	PrintS(" _L [<step>]: Display astro-graph locations of planetary angles.");
	PrintS(" _L0 [..]: Like _L but display list of latitude crossings too.");
	PrintS(" _K: Display a calendar for given month.");
	PrintS(" _Ky: Like _K but display a calendar for the entire year.");
	PrintS(" _d [<step>]: Print all aspects and changes occurring in a day.");
	PrintS(" _dm: Like _d but print all aspects for the entire month.");
	PrintS(" _dy: Like _d but print all aspects for the entire year.");
	PrintS(" _dY <years>: Like _d but search within a number of years.");
	PrintS(" _dp <month> <year>: Print aspects within progressed chart.");
	PrintS(" _dpy <year>: Like _dp but search for aspects within entire year.");
	PrintS(" _dpY <year> <years>: Like _dp but search within number of years.");
	PrintS(" _dp[y]n: Search for progressed aspects in current month/year.");
	PrintS(" _D: Like _d but display aspects by influence instead of time.");
	PrintS(" _E: Display planetary ephemeris for given month.");
	PrintS(" _Ey: Display planetary ephemeris for the entire year.");
	PrintS(" _EY <years>: Display planetary ephemeris for a number of years.");
	PrintS(" _e: Print all charts together (i.e. _v_w_g0_a_m_Z_S_l_K_j0_L0_d_D_E).");
	PrintS(" _t <month> <year>: Compute all transits to natal planets in month.");
	PrintS(" _tp <month> <year>: Compute progressions to natal in month for chart.");
	PrintS(" _tr <month> <year>: Compute all returns in month for chart.");
	PrintS(" _t[p]y: <year>: Compute transits/progressions for entire year.");
	PrintS(" _t[p]Y: <year> <years>: Compute transits for a number of years.");
	PrintS(" _t[py]n: Compute transits to natal planets for current time now.");
	PrintS(" _T <month> <day> <year>: Display transits ordered by influence.");
	PrintS(" _Tp <month> <day> <year>: Print progressions instead of transits.");
	PrintS(" _T[p]n: Display transits ordered by influence for current date.");
#ifdef ARABIC
	PrintS(" _P [<parts>]: Display list of Arabic parts and their positions.");
	PrintS(" _P0 [<parts>]: Like _P but display formulas with terms reversed.");
	PrintS(" _P[z,n,f]: Order parts by position, name, or formula.");
#endif
#ifdef INTERPRET
	PrintS(" _I [<columns>]: Print interpretation of selected charts.");
#endif

	PrintS("\nSwitches which affect how the chart parameters are obtained:\n");
	PrintS(" _n: Compute chart for this exact moment using current time.");
	PrintS(" _n[d,m,y]: Compute chart for start of current day, month, year.");
	PrintS(" _z [<zone>]: Change the default time zone (for _d_E_t_q options).");
	PrintS(" _z0 [<offset>]: Change the default daylight time setting.");
	PrintS(" _zl <long> <lat>: Change the default longitude & latitude.");
	PrintS(" _zt <time>: Set only the time of current chart.");
	PrintS(" _zd <date>: Set only the day of current chart.");
	PrintS(" _zm <month>: Set only the month of current chart.");
	PrintS(" _zy <year>: Set only the year of current chart.");
	PrintS(" _zi <name> <place>: Set name and place strings of current chart.");
	PrintS(" _q <month> <date> <year> <time>: Compute chart with defaults.");
	PrintS(" _qd <month> <date> <year>: Compute chart for noon on date.");
	PrintS(" _qm <month> <year>: Compute chart for first of month.");
	PrintS(" _qy <year>: Compute chart for first day of year.");
	PrintS(" _qa <month> <date> <year> <time> <zone> <long> <lat>:");
	PrintS("     Compute chart automatically given specified data.");
	PrintS(" _qb <month> <date> <year> <time> <daylight> <zone> <long> <lat>:");
	PrintS("     Like _qa but takes additional parameter for daylight offset.");
	PrintS(" _qj <day>: Compute chart for time of specified Julian day.");
	PrintS(" _i <file>: Compute chart based on info in file.");
	PrintS(" _i[2,3,4] <file>: Load chart info into chart slots 2, 3, or 4.");
	PrintS(" _o <file> [..]: Write parameters of current chart to file.");
	PrintS(" _o0 <file> [..]: Like _o but output planet/house positions.");
	PrintS(" _os <file>, > <file>: Redirect output of text charts to file.");

	PrintS("\nSwitches which affect what information is used in a chart:\n");
	PrintS(" _R [<obj1> [<obj2> ..]]: Restrict specific bodies from displays.");
	PrintS(" _R0 [<obj1> ..]: Like _R but restrict everything first.");
	PrintS(" _R1 [<obj1> ..]: Like _R0 but unrestrict and show all objects.");
	PrintS(" _R[C,u,U]: Restrict all minor cusps, all uranians, or stars.");
	PrintS(" _RT[0,1,C,u,U] [..]: Restrict transiting planets in _t lists.");
	PrintS(" _RA [<asp1> ..]: Restrict aspects.");
	PrintS(" _C: Include angular and non-angular house cusps in charts.");
	PrintS(" _u: Include transneptunian/uranian bodies in charts.");
	PrintS(" _U: Include locations of fixed background stars in charts.");
	PrintS(" _U[z,l,n,b]: Order by azimuth, altitude, name, or brightness.");
	PrintS(" _Ao <aspect> <orb>: Specify maximum orb for an aspect.");
	PrintS(" _Am <planet> <orb>: Specify maximum orb allowed to a planet.");
	PrintS(" _Ad <planet> <orb>: Specify orb addition given to a planet.");
	PrintS(" _Aa <aspect> <angle>: Change the actual angle of an aspect.");
	PrintS("\nSwitches which affect how a chart is computed:\n");
	PrintS(" _b: Use ephemeris files for more accurate location computations.");
	PrintS(" _b0: Like _b but display locations to the nearest second too.");
	PrintS(" _c <value>: Select a different default system of houses.");
	PrintS("     (0 = Placidus, 1 = Koch, 2 = Equal, 3 = Campanus,");
	PrintS("     4 = Meridian, 5 = Regiomontanus, 6 = Porphyry, 7 = Morinus,");
	PrintS("     8 = Topocentric, 9 = Alcabitius, 10 = Equal (MC),");
	PrintS("     11 = Neo-Porphyry, 12 = Whole, 13 = Vedic, 14 = None.)");
	PrintS(" _s [..]: Compute a sidereal chart instead of the normal tropical chart.");
	PrintS(" _sr: Compute right ascension locations relative to equator.");
	PrintS(" _s[z,h,d]: Display locations as in zodiac, hours/minutes, or degrees.");
	PrintS(" _h [<objnum>]: Compute positions centered on specified object.");
	PrintS(" _p <month> <day> <year>: Cast 2ndary progressed chart for date.");
	PrintS(" _p0 <month> <day> <year>: Cast solar arc chart for date.");
	PrintS(" _p[0]n: Cast progressed chart based on current date now.");
	PrintS(" _pd <days>: Set no. of days to progress / day (default 365.25).");
	PrintS(" _x <value>: Cast harmonic chart based on specified factor.");
	PrintS(" _1 [<objnum>]: Cast chart with specified object on Ascendant.");
	PrintS(" _2 [<objnum>]: Cast chart with specified object on Midheaven.");
	PrintS(" _3: Display objects in their zodiac decan positions.");
	PrintS(" _f: Display houses as sign positions (flip them).");
	PrintS(" _G: Compute houses based on geographic location only.");
	PrintS(" _J: Display wheel charts in Vedic format.");
	PrintS(" _9: Display objects in their zodiac navamsa positions.");
	PrintS(" _F <objnum> <sign> <deg>: Force object's position to be value.");
	PrintS(" _+ [<days>]: Cast chart for specified no. of days in the future.");
	PrintS(" _- [<days>]: Cast chart for specified no. of days in the past.");
	PrintS(" _+[m,y] [<value>]: Cast chart for no. of months/years in future.");
	PrintS("\nSwitches for relationship and comparison charts:\n");
	PrintS(" _r <file1> <file2>: Compute a relationship synastry chart.");
	PrintS(" _rc <file1> <file2>: Compute a composite chart.");
	PrintS(" _rm <file1> <file2>: Compute a time space midpoint chart.");
	PrintS(" _r[c,m]0 <file1> <file2> <ratio1> <ratio2>: Weighted chart.");
	PrintS(" _rd <file1> <file2>: Print time span between files' dates.");
#ifdef BIORHYTHM
	PrintS(" _rb <file1> <file2>: Display biorhythm for file1 at time file2.");
#endif
	PrintS(" _r0 <file1> <file2>: Keep the charts separate in comparison.");
	PrintS(" _rp[0] <file1> <file2>: Like _r0 but do file1 progr. to file2.");
	PrintS(" _rt <file1> <file2>: Like _r0 but treat file2 as transiting.");
	PrintS(" _r[3,4]: Make graphics wheel chart tri-wheel or quad-wheel.");
	PrintS(" _y <file>: Display current house transits for particular chart.");
#ifdef BIORHYTHM
	PrintS(" _y[b,d,p,t] <file>: Like _r0 but compare to current time now.");
#endif
	PrintS("\nSwitches to access graphics options:\n");
	PrintS(" _k: Display text charts using Ansi characters and color.");
	PrintS(" _k0: Like _k but only use special characters, not Ansi color.");

	PrintS(" _X: Create a graphics chart instead of displaying it as text.");
	PrintS(" _Xb: Create bitmap file instead of putting graphics on screen.");
	PrintS(" _Xb[n,c,v,a,b]: Set bitmap file output mode to X11 normal,");
	PrintS("     compacted, very compact, Ascii (bmtoa), or Windows bmp.");
#ifdef PS
	PrintS(" _Xp: Create PostScript stroke graphic instead of bitmap file.");
	PrintS(" _Xp0: Like _Xp but create complete instead of encapsulated file.");
#endif
#ifdef META
	PrintS(" _XM[0]: Create Windows metafile stroke graphic instead of bitmap.");
#endif
	PrintS(" _Xo <file>: Write output bitmap or graphic to specified file.");
	PrintS(" _Xm: Create monochrome graphic instead of one in color.");
	PrintS(" _Xr: Create chart graphic in reversed colors (white background).");
	PrintS(" _Xw <hor> [<ver>]: Change the size of chart graphic.");
	PrintS(" _Xs <100,200,300,400>: Change the size of map or characters by %.");
	PrintS(" _Xi: Create chart graphic in slightly modified form.");
	PrintS(" _Xt: Inhibit display of chart info at bottom of graphic.");
	PrintS(" _Xu: Inhibit display of a border around graphic.");
	PrintS(" _Xl: Inhibit labeling of object points in chart graphic.");
	PrintS(" _Xj: Don't clear screen between chart updates, drawing trails.");
	PrintS(" _X1 <object>: Rotate wheel charts so object is at left edge.");
	PrintS(" _X2 <object>: Rotate wheel charts so object is at top edge.");
	PrintS(" _XW: Simply create an image of the world map.");
	PrintS(" _XW0: Like _XW but do a non-rectangular Mollewide projection.");
	PrintS(" _XG [<degrees>]: Display the image of the world as a globe.");
	PrintS(" _XP: Like _XG but create the globe from a polar projection.");
#ifdef CONSTEL
	PrintS(" _XF: Display maps as constellations on the celestial sphere.");
#endif
	PrintS(" _Xn [<mode>]: Start up chart or globe display in animation mode.");
	PrintS(" _HX: Display list of key press options for screen graphics.");
	
	PrintS(" _W <value>: Run given Windows menu command internally.");
	PrintS(" _WN <1-32000>: Set animation update delay in milliseconds.");
	PrintS(" _WM <1-100> <text>: Set Windows menu text for macro command.");
	PrintS(" _Wn: Don't redraw screen until user forces update.");
}


/* Print a list of the obscure command switches that can be passed to the */
/* program and a description of them. This is what the -Y switch prints.  */

void DisplaySwitchesRare()
{
	char sz[cchSzDef];

	sprintf(sz, "Obscure command switches (version %s):\n", szVersionCore);
	PrintS(sz);
	PrintS(" _Y: Display this help list.");
	PrintS(" _Yn: Compute location of true instead of mean node.");
	PrintS(" _Yd: Display dates in D/M/Y instead of M/D/Y format.");
	PrintS(" _Yt: Display times in 24 hour instead of am/pm format.");
	PrintS(" _YC: Automatically ignore insignificant house cusp aspects.");
	PrintS(" _Y8: Clip text charts at the rightmost (e.g. 80th) column.");
	PrintS(" _YQ <rows>: Pause text scrolling after a page full has printed.");
	PrintS(" _Yo: Output chart info and position files in old style format.");
	PrintS(" _Yc: Angular cusp objects are house positions instead of angles.");
	PrintS(" _Yl <1-36>: Toggle plus zone status of sector for sector chart.");
#ifdef ARABIC
	PrintS(" _YP <-1,0,1>: Set how Arabic parts are computed for night charts.");
#endif
	PrintS(" _Yb <days>: Set number of days to span for biorhythm chart.");
	PrintS(" _YE <obj> <semi-major axis> <eccentricity (3)> <inclination (3)>");
	PrintS("     <perihelion (3)> <ascending node (3)> <time offset (3)>");
	PrintS("     Change orbit of object to be the given elements.");
	PrintS(" _YR <obj1> <obj2> <flag1>..<flag2>: Set restrictions for object range.");
	PrintS(" _YRT <obj1> <obj2> <flag1>..<flag2>: Transit restrictions for range.");
	PrintS(" _YR0 <flag1> <flag2>: Set restrictions for sign, direction changes.");
	PrintS(" _YRZ <rise> <zenith> <set> <nadir>: Set restrictions for _Zd chart.");
	PrintS(" _YAR <asp1> <asp2> <flag1>..<flag2>: Set aspect restrictions for range.");
	PrintS(" _YAo <asp1> <asp2> <orb1>..<orb2>: Set aspect orbs for range.");
	PrintS(" _YAm <obj1> <obj2> <orb1>..<orb2>: Set max planet orbs for range.");
	PrintS(" _YAd <obj1> <obj2> <orb1>..<orb2>: Set planet orb additions for range.");
	PrintS(" _YAa <asp1> <asp2> <ang1>..<ang2>: Set planet aspect angles for range.");
	PrintS(" _Yj <obj1> <obj2> <inf1>..<inf2>: Set influences for object range.");
	PrintS(" _YjC <cusp1> <cusp2> <inf1>..<inf2>: Set influences for house cusps.");
	PrintS(" _YjA <asp1> <asp2> <inf1>..<inf2>: Set influences for aspect range.");
	PrintS(" _YjT <obj1> <obj2> <inf1>..<inf2>: Set transit influences for range.");
	PrintS(" _Yj0 <inf1> <inf2> <inf3> <inf4>: Set influences given to planets");
	PrintS("     in ruling sign, exalted sign, ruling house, exalted house.");
	PrintS(" _YJ <obj> <sign> <cosign>: Set sign planet rules and co-rules.");
	PrintS(" _YJ0 <obj> <sign>: Set zodiac sign given planet exalts in.");
	PrintS(" _YI <obj> <string>: Customize interpretation for object.");
	PrintS(" _YIa <sign> <string>: Customize interpretation adjective for sign.");
	PrintS(" _YIv <sign> <string>: Customize interpretation verb for sign.");
	PrintS(" _YIC <house> <string>: Customize interpretation for house.");
	PrintS(" _YIA <asp> <string>: Customize interpretation for aspect.");
	PrintS(" _YIA0 <asp> <string>: Customize aspect interpretation statement.");
	PrintS(" _YkC <fir> <ear> <air> <wat>: Customize element colors.");
	PrintS(" _YkA <asp1> <asp2> <col1>..<col2>: Customize aspect colors.");
	PrintS(" _Yk0 <1..7> <1..7> <col1>..<col2>: Customize 'rainbow' colors.");
	PrintS(" _Yk <0..8> <0..8> <col1>..<col2>: Customize 'general' colors.");
	PrintS(" _YXC <0 (none) 1 (moire) 2 (web)> <size> <no. lines>: Corner decoration");
	PrintS(" _YXG <0-2><0-2><0-2><0-3>: Select among different graphic glyphs");
	PrintS("     for Capricorn, Uranus, Pluto, and Lilith.");
	PrintS(" _YXg <cells>: Set number of cells for graphic aspect grid.");
	PrintS(" _YXf <val>: Set usage of actual system fonts in graphic file.");
	PrintS(" _YXp <-1,0,1>: Set paper orientation for PostScript files.");
	PrintS(" _YXp0 <hor> <ver>: Set paper size for PostScript files.");
	PrintS(" _0[o,i,q,X]: Disallow file output, input, exiting, and graphics.");
	PrintS(" _;: Ignore rest of command line and treat it as a comment.");
}


/* etc_en.c */
