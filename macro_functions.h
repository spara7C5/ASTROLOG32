/*
** Astrolog File: macro_functions.h
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

/*
******************************************************************************
** Macro Functions.
******************************************************************************
*/

#define PrintAltitude(deg) PrintSz(SzAltitude(deg))
#define ErrorValR(sz, r) ErrorValN(sz, (int)r)
#define RBiorhythm(day, rate) (sin(((day)/(rate))*rPi2)*100.0)

#define BBmGet(B, X, Y) ((B)[(long)(Y)*(long)(gi.cbBmpRow) + ((X) >> 1)])
#define FBmGet(B, X, Y) (BBmGet(B, X, Y) >> (((X)&1^1) << 2) & 15)
#define BmSet(B, X, Y, O) BBmGet(B, X, Y) = BBmGet(B, X, Y) & \
  15 << (((X)&1) << 2) | (O) << (((X)&1^1) << 2)

#define PutByte(A) putc((byte) (A), file)
#define PutWord(A) PutByte(BLo(A)); PutByte(BHi(A))
#define PutLong(A) PutWord(WLo(A)); PutWord(WHi(A))

#define DrawEdge(X1, Y1, X2, Y2) \
  DrawBox(X1, Y1, X2, Y2, gi.nScaleT, gi.nScaleT)
#define DrawEdgeAll() DrawEdge(0, 0, gs.xWin-1, gs.yWin-1)
#define DrawLine(X1, Y1, X2, Y2) DrawDash(X1, Y1, X2, Y2, 0)
#define DrawCircle(X, Y, RX, RY) \
  DrawEllipse((X)-(RX), (Y)-(RY), (X)+(RX), (Y)+(RY))


// macros to output the various metafile commands used
#ifdef META
#define MetaRecord(S,R) MetaLong((long)(S)); MetaWord((word) (R))
#define MetaSelectObject(O) MetaRecord(4,0x12D); MetaWord((word) (O))
#define MetaDeleteObject(O) MetaRecord(4, 0x1F0); MetaWord((word) (O))
#define MetaSaveDc() MetaRecord(3, 0x01E)
#define MetaRestoreDc() MetaRecord(4, 0x127); MetaWord((word)-1)
#define MetaWindowOrg(X, Y) MetaRecord(5, 0x20B); MetaWord((word) (Y)); MetaWord((word) (X))
#define MetaWindowExt(X, Y) MetaRecord(5, 0x20C); MetaWord((word) (Y)); MetaWord((word) (X))
#define MetaCreatePen(S, W, C) MetaRecord(8, 0x2FA); MetaWord((word) (S)); \
  MetaWord((word) (W)); MetaWord((word) (W)); MetaLong((long) (C))
#define MetaCreateBrush(S, C) MetaRecord(7, 0x2FC); \
  MetaWord((word) (S)); MetaLong((long) (C)); MetaWord(0 /* Not used */);
#define MetaCreateFont(S, X, Y, C) MetaRecord(12+(S), 0x2FB); MetaWord((word) (Y)); \
  MetaWord((word) (X)); MetaWord(0 /* Angle */); MetaWord(0 /* Not used */); \
  MetaWord(400 /* Normal Weight */); MetaWord(0 /* Italic, Underline */); \
  MetaWord(WFromBB(0 /* Strikeout */, C)); \
  MetaWord(WFromBB(4 /* TrueType */, 0 /* Clip */))
#define MetaBkMode(M) MetaRecord(4, 0x102); MetaWord((word) (M))
#define MetaTextAlign(A) MetaRecord(4, 0x12E); MetaWord((word) (A))
#define MetaTextColor(C) MetaRecord(5, 0x209); MetaLong(C);
#define MetaTextOut(X, Y, S) MetaRecord(7+((S)+1)/2, 0xA32); \
  MetaWord((word) (Y)); MetaWord((word) (X)); MetaWord((word) (S)); MetaWord(0 /* ETO */)
#define MetaRectangle(X1, Y1, X2, Y2) MetaRecord(7, 0x41B); \
  MetaWord((word) (Y2)); MetaWord((word) (X2)); MetaWord((word) (Y1)); MetaWord((word) (X1))
#define MetaEllipse(X1, Y1, X2, Y2) MetaRecord(7, 0x418); \
  MetaWord((word) (Y2)); MetaWord((word) (X2)); MetaWord((word) (Y1)); MetaWord((word) (X1))
#define MetaEscape(S) MetaRecord((S), 0x626); \
  MetaWord(15 /* MFCOMMENT */); MetaWord(((S)-5)*2 /* Bytes in comment */);
#endif /* META */

#define TextClearScreen() PatBlt(wi.hdc, 0, 0, wi.xClient, wi.yClient, \
  us.fAnsiColor || !gs.fInverse ? BLACKNESS : WHITENESS);


// calculations
#define EclToEqu(Z, L) CoorXform(Z, L, is.rObliquity)
#define EquToEcl(Z, L) CoorXform(Z, L, -is.rObliquity)
#define EquToLocal(Z, L, T) CoorXform(Z, L, T)
#define JulianDayFromTime(t) ((t)*36525.0+2415020.0)
#define IoeFromObj(obj) \
  (obj < oMoo ? 0 : (obj <= cLastPlanet ? obj - 2 : obj - uranLo + cLastPlanet - 1))
#define Tropical(deg) (deg - is.rSid + us.rSiderealCorrection)

#define BLo(w) ((byte)(w))
#define BHi(w) ((byte)((word)(w) >> 8 & 0xFF))
#define WLo(l) ((word)(dword)(l))
#define WHi(l) ((word)((dword)(l) >> 16 & 0xFFFF))
#define WFromBB(bLo, bHi) ((word)BLo(bLo) | (word)((byte)(bHi)) << 8)
#define LFromWW(wLo, wHi) ((dword)WLo(wLo) | (dword)((word)(wHi)) << 16)
#define LFromBB(b1, b2, b3, b4) LFromWW(WFromBB(b1, b2), WFromBB(b3, b4))
#define RGBR(l) BLo(l)
#define RGBG(l) BHi(l)
#define RGBB(l) ((byte)((dword)(l) >> 16 & 0xFF))
#define ChHex(n) (char)((n) < 10 ? '0' + (n) : 'a' + (n) - 10)
#define VgaFromEga(x) NMultDiv((x), 480, 350)
#define VgaFromCga(x) NMultDiv((x), 480, 200)
#define Max(v1, v2) ((v1) > (v2) ? (v1) : (v2))
#define Min(v1, v2) ((v1) < (v2) ? (v1) : (v2))
#define NSgn(n) ((n) < 0 ? -1 : (n) > 0)
#define RSgn2(r) ((r) < 0.0 ? -1.0 : 1.0)
#define FBetween(v, v1, v2) ((v) >= (v1) && (v) <= (v2))
#define RFract(r) ((r) - floor(r))
#define ChCap(ch) ((ch) >= 'a' && (ch) <= 'z' ? (ch) - 'a' + 'A' : (ch))
#define ChUncap(ch) (FCapCh(ch) ? (ch) - 'A' + 'a' : (ch))
#define FCapCh(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define FNumCh(ch) ((ch) >= '0' && (ch) <= '9')
#define NMultDiv(n1, n2, n3) ((int)((long)(n1) * (n2) / (n3)))
#define Ratio(v1, v2, v3) ((v1) + ((v2) - (v1)) * (v3))
#define Sign2Z(s) ((double)(((s)-1)*30))
#define Z2Sign(r) (((int)(r))/30+1)
#define Deg2Rad(r) ((r)/rDegRad)
#define Rad2Deg(r) ((r)*rDegRad)
#define NFloor(r) ((int)floor(r))
#define RSinD(r) sin(Deg2Rad(r))
#define RCosD(r) cos(Deg2Rad(r))
#define NSinD(nR, nD) ((int)((double)(nR)*RSinD((double)nD)))
#define NCosD(nR, nD) ((int)((double)(nR)*RCosD((double)nD)))
#define mod12(n) ((n - 1) % 12 + 1)  // returns 1-12, n must be >= 1
#define mod36(n) ((n - 1) % 36 + 1)  // returns 1-36, n must be >= 1

// object comparison
#define FItem(obj)    FBetween(obj, 0, cObj)
#define FNorm(obj)    FBetween(obj, 0, cLastMoving)
#define FCusp(obj)    FBetween(obj, cuspLo, cuspHi)
#define FAngle(obj)   (obj == oAsc || obj == oNad  || obj == oDes  || obj == oMC)
#define FMinor(obj)   (FCusp(obj) && (obj - oAsc) % 3 != 0)
#define FUranian(obj) FBetween(obj, uranLo, uranHi)
#define FStar(obj)    FBetween(obj, starLo, starHi)
#define FObject(obj)  ((obj) <= oVesta || (obj) >= uranLo)
#define FThing(obj)   ((obj) <= cThing || (obj) >= uranLo)
#define FHelio(obj)   (FNorm(obj) && FObject(obj) && (obj) != oMoo)

#define FSector(s)    FBetween(s, 1, cSector)
#define ChDst(dst)    (dst == 0.0 ? Lang(sST) : (dst == 1.0 ? Lang(sDT) : "AT"))
#define ChDashF(f)    (f ? '=' : '_')
#define SzNumF(f)     (f ? "1 " : "0 ")
#define DayInYearHi(yea) (365-28+DayInMonth(2, yea))
#define FChSwitch(ch) \
  ((ch) == '-' || (ch) == '/' || (ch) == '_' || (ch) == '=' || (ch) == ':')

#define FValidMon(mon) FBetween(mon, 1, 12)
#define FValidDay(day, mon, yea) ((day) >= 1 && (day) <= DayInMonth(mon, yea))
#define FValidYea(yea) FBetween(yea, -20000, 20000)
#define FValidTim(tim) ((tim) > -2.0 && (tim) < 24.0 && \
  RFract(fabs(tim)) < 0.60)
#define FValidDst(dst) FValidZon(dst)
#define FValidZon(zon) FBetween(zon, -24.0, 24.0)
#define FValidLon(lon) FBetween(lon, -180.0, 180.0)
#define FValidLat(lat) FBetween(lat, -90.0, 90.0)
#define FValidAspect(asp) FBetween(asp, 0, cAspect)
#define FValidSystem(n) FBetween(n, 0, NUMBER_OF_HOUSE_SYSTEMS - 1)
#define FValidDivision(n) FBetween(n, 1, 2880)
#define FValidOffset(r) FBetween(r, -360.0, 360.0)
#define FValidCenter(obj) \
  (FBetween(obj, oEar, uranHi) && FObject(obj) && (obj) != oMoo)
#define FValidHarmonic(n) FBetween(n, 1, 30000)
#define FValidWheel(n) FBetween(n, 1, WHEELROWS)
#define FValidAstrograph(n) (n > 0 && 160%n == 0)
#define FValidPart(n) FBetween(n, 1, cPart)
#define FValidBioday(n) FBetween(n, 1, 199)
#define FValidScreen(n) FBetween(n, 20, 200)
#define FValidMacro(n) FBetween(n, 1, NUMBER_OF_MACROS)
#define FValidTextrows(n) ((n) == 25 || (n) == 43 || (n) == 50)
#define FValidGlyphs(n) FBetween(n, 0, 2223)
#define FValidGrid(n) FBetween(n, 1, cObj)
#define FValidScale(n) (FBetween(n, 100, MAXSCALE) && (n)%100 == 0)
#define FValidGraphx(x) (FBetween(x, BITMAPX1, BITMAPX) || (x) == 0)
#define FValidGraphy(y) (FBetween(y, BITMAPY1, BITMAPY) || (y) == 0)
#define FValidRotation(n) FBetween(n, 0, 360-1)
#define FValidTilt(n) FBetween(n, -90.0, 90.0)
#define FValidColor(n) FBetween(n, 0, cColor - 1)
#define FValidBmpmode(ch) \
  ((ch) == 'N' || (ch) == 'C' || (ch) == 'V' || (ch) == 'A' || (ch) == 'B')
#define FValidTimer(n) FBetween(n, 1, 32000)
#define FValidStarAspects(n) FBetween(n, 0, 18)
#define FValidStarOrb(r) FBetween(r, 0.2, 4.0)
#define FValidCoeffTens(r) FBetween(r, 0.2, 4.0)
#define FValidCoeffComp(r) FBetween(r, rSmall, 1000.0)

#define chSig3(A) Lang(tSignName,A)[0], Lang(tSignName,A)[1], Lang(tSignName,A)[2]
#define chObj3(A) Lang(tObjShortName, A)[0], Lang(tObjShortName, A)[1], Lang(tObjShortName, A)[2]
#define chMon3(A) Lang(tMonth, A)[0], Lang(tMonth, A)[1], Lang(tMonth, A)[2]
#define chDay3(A) Lang(tDay, A)[0], Lang(tDay, A)[1], Lang(tDay, A)[2]
#define chDay2(A) Lang(tDay, A)[0], Lang(tDay, A)[1]

#define kSignA(s) kElemA[(s)-1 & 3]
#define kSignB(s) kElemB[(s)-1 & 3]
#define kModeA(m) kElemA[m <= 1 ? m : eWat]

#define FIgnore(i) (ignore[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)
#define FIgnore2(i) (ignore2[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)
#define FIgnore3(i) (ignore3[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)

#define not(V) V = !(V)
#define neg(V) V = -(V)

#define PrintL() PrintSz("\n")
#define PrintL2() PrintSz("\n\n")

#define SwapN(n1, n2) {int  tmp = n1; n1 = n2; n2 = tmp;}
#define SwapR(r1, r2) {double tmp = r1; r1 = r2; r2 = tmp;}

#define FSwitchF(f) ((((f) | fOr) & !fAnd) ^ fNot)
#define SwitchF(f) f = FSwitchF(f)
#define SwitchF2(f) f = (((f) | (fOr || fNot)) & !fAnd)

// macros for chart info
// the following are unsed only in intrpret.c
#define szPerson  (ciMain.nam[0] ? ciMain.nam : "This person")
#define szPerson0 (ciMain.nam[0] ? ciMain.nam : "the person")
#define szPerson1 (ciMain.nam[0] ? ciMain.nam : "Person1")
#define szPerson2 (ciTwin.nam[0] ? ciTwin.nam : "Person2")

/* Should an object in the outer wheel be restricted? */
#define FProper2(i) \
(!(us.nRel == rcProgress ? ignore3[i] : (us.nRel == rcTransit ? ignore2[i] : ignore[i])) && i != us.objCenter)
/* Are particular coordinates on the chart? */
#define FInRect(x, y, x1, y1, x2, y2) \
  ((x) >= (x1) && (x) < (x2) && (y) >= (y1) && (y) < (y2))
#define FOnWin(X, Y) FInRect((X), (Y), 0, 0, gs.xWin, gs.yWin)
/* Get a coordinate based on chart radius, a fraction, and (co)sin value. */
#define POINT1(U, R, S) ((int)(((U)+1.4)*(R)*(S)))
#define POINT2(U, R, S) ((int)(((U)-0.3)*(R)*(S)))
/* Determine (co)sin factors based on zodiac angle and chart orientation. */
#define PX(A) RCosD(A)
#define PY(A) RSinD(A)
#define PZ(A) PlaceInX(A)
/* Compute Mollewide projection in pixel scale given latitude. */
#define NMollewide(y) \
  ((int)(sqrt((double)(180L*nScl*180L*nScl - 4L*(y)*nScl*(y)*nScl))+0.5))
/* Do settings indicate the current chart should have the info sidebar? */
#define fSidebar ((gi.nCurrChart == gWheel || gi.nCurrChart == gHouse || \
  gi.nCurrChart == gSector) && gs.fText && !us.fVelocity)
/* Is the current chart most properly displayed as a square graphic? */
#define fSquare \
  (gi.nCurrChart == gWheel || gi.nCurrChart == gHouse || gi.nCurrChart == gGrid || \
  (gi.nCurrChart == gHorizon && us.fPrimeVert) || gi.nCurrChart == gDisposit || \
  gi.nCurrChart == gOrbit || gi.nCurrChart == gGlobe || gi.nCurrChart == gPolar)
/* Does the current chart have to be displayed in a map rectangle? */
#define fMap \
  (gi.nCurrChart == gAstroGraph || gi.nCurrChart == gWorldMap)
/* Do settings indicate the current chart should have an outer border? */
#define fDrawBorder \
  ((gs.fBorder || gi.nCurrChart == gGrid) && gi.nCurrChart != gGlobe && \
  gi.nCurrChart != gPolar && (gi.nCurrChart != gWorldMap || !gs.fMollewide))
/* Do settings indicate current chart should have chart info at its bottom? */
#define fDrawText \
  (gs.fText && gi.nCurrChart != gCalendar && gi.nCurrChart != gWorldMap && \
  gi.nCurrChart != gGlobe && gi.nCurrChart != gPolar && ((gi.nCurrChart != gWheel && \
  gi.nCurrChart != gHouse && gi.nCurrChart != gSector) || us.fVelocity))



#define SIZEOF_FIELD(structure, field) (sizeof(((structure *) 0)->field))
#define NUM_ELEMENTS(array)  (sizeof(array)/sizeof(*array))


/* macro_functions.h */
