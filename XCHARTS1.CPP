/*
** Astrolog File: xcharts1.c
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

void DrawCalendar(int, int, int, int, int);

/*
******************************************************************************
** Single Chart Graphics Routines.
******************************************************************************
*/

/* Draw a wheel chart, in which the 12 signs and houses are delineated, and  */
/* the planets are inserted in their proper places. This is the default      */
/* graphics chart to generate, as is done when the -v or -w (or no) switches */
/* are included with -X. Draw the aspects in the middle of chart, too.       */

void XChartWheel()
{
	double xsign[NUMBER_OF_SIGNS + 1], xhouse[NUMBER_OF_SIGNS + 1], xplanet[NUMBER_OBJECTS], symbol[NUMBER_OBJECTS];
	int cx, cy, i, j, k, first, n;
	double unitx, unity, argum, argum1, value, valt, l, m, p, ip, jp;
	double power1[cLastMoving + 1];
	double ccc = 2.0;

	/* Set up variables and temporarily automatically decrease the horizontal */
	/* chart size to leave room for the sidebar if that mode is in effect.    */

	if (gs.fText && !us.fVelocity)
		gs.xWin -= (SIDESIZE*gi.nScaleT);
	cx = gs.xWin / 2 - 1;
	cy = gs.yWin / 2 - 1;
	unitx = (double) cx;
	unity = (double) cy;
	gi.rAsc = gs.objLeft ? cp0.longitude[abs(gs.objLeft)] + 90.0 * (gs.objLeft < 0) : cp0.cusp_pos[1];
	if (us.fVedic)
		gi.rAsc = gs.objLeft ? (gs.objLeft < 0 ? 120.0 : -60.0) - gi.rAsc : 0.0;

	if (hRevers == 2)
		gi.rAsc = cp0.cusp_pos[7];

	/* Fill out arrays with the angular degree on the circle of where to */
	/* place each object, cusp, and sign glyph based on the chart mode.  */

	if (gi.nCurrChart == gWheel)
	{
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			xhouse[i] = PZ(cp0.cusp_pos[i]);
	}
	else
	{
		gi.rAsc -= cp0.cusp_pos[1];
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			xhouse[i] = PZ(Sign2Z(i));
	}
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		xsign[i] = PZ(HousePlaceInX(Sign2Z(i)));
	for (i = 0; i <= cObj; i++)
		xplanet[i] = PZ(HousePlaceInX(cp0.longitude[i]));

	/* Go draw the outer sign and house rings. */

	DrawWheel(xsign, xhouse, cx, cy, unitx, unity, 0.65, 0.70, 0.75, 0.80, 0.875);

	for (i = 0; i <= cObj; i++)	/* Figure out where to put planet glyphs. */
		symbol[i] = xplanet[i];
	FillSymbolRing(symbol, 1.0);

	/* For each planet, draw a small dot indicating where it is, and then */
	/* a line from that point to the planet's glyph.                      */

	DrawSymbolRing(symbol, xplanet, cp0.vel_longitude, cx, cy, unitx, unity, 0.50, 0.52, 0.56, 0.60);

	if (!FCreateGrid(FALSE))
		return;
	for (i = 0; i <= cLastMoving; i++)
		power1[i] = rObjInf[i];
	if (us.nRel <= 0)
	{
		PlanetPPower();
		for (i = 0; i <= cLastMoving; i++)
			power1[i] = ppower1[i];
	}
	STension = SHarmon = SConjunct = SSumma = SChart = 0.0;

/* Added by V.A. Additional graphics output - tension/harmony chart. */

	if (TensChart != 0)
	{
		DrawColor(kWhite);
		for (i = 0; i < 180; i++)
		{
			value = 0.25;
			argum = (double) (i) * 2.0;
			argum = PZ(HousePlaceInX(argum));
			DrawPoint(cx + POINT1(unitx, value, PX(argum)), cy + POINT1(unity, value, PY(argum)));
		}
		DrawColor(kWhite);
		for (i = 0; i < 3600; i++)
		{
			value = 0.0;
			if (TensChart <= 1)
				first = aOpp;
			else
				first = aCon;

			argum = (double) (i) / 10.0;
			argum1 = PZ(HousePlaceInX(argum));
			for (j = 1; j <= cObj; j++)
				if (!ignore[j])
				{
					l = MinDistance(argum, cp0.longitude[j]);
					for (k = first; k <= cAspect; k++)
					{
						if (!ignoreA[k])
						{
							m = fabs(l - rAspAngle[k]);
							if (m < GetOrb(1, j, k))
							{
								if (FCusp(j) && (k != aCon || k != aTri))
									continue;
								valt = (sqrt) (20.0 * ppower1[j]) * rAspInf[k] * (1.0 - m / GetOrb(1, j, k));
								if ((k == aOpp || k == aSqu || k == aSSq || k == aSes) && (TensChart <= 1))
									valt = -valt;
								value = value + CoeffTens * valt / 120.0;
							}
						}
					}
				}

			if (TensChart == 1)
				value = 0.25 + value;
			if (TensChart == -1)
				value = 0.25 - value;
			if (value < 0.0)
				value = 0.0;
			if (value > 0.6)
				value = 0.6;

			DrawPoint(cx + POINT1(unitx, value, PX(argum1)), cy + POINT1(unity, value, PY(argum1)));
		}
	}
	else
	{

		/* End V.A. addition */
 

		/* Draw lines connecting planets which have aspects between them. */
		/* Now in bonus mode dashenness of aspect lines depends of absolute */
		/* power of aspect, not only of orb. V.A. */

		for (j = cObj; j >= 1; j--)
			for (i = j - 1; i >= 0; i--)
				if (grid->n[i][j] && FProper(i) && FProper(j))
				{
					k = grid->n[i][j];
					ip = i <= cLastMoving ? power1[i] : 2.5;
					jp = j <= cLastMoving ? power1[j] : 2.5;
					p = (rAspInf[k] * (sqrt) (ip * jp) * (1.0 - fabs((double) (grid->v[i][j])) / 60.0 / GetOrb(i, j, k)));
					if (us.fParallel)
						p = p * PowerPar;
 
					DrawColor(kAspB[grid->n[i][j]]);
					if (!gs.fBonusMode)
					{
						DrawDash(cx + POINT1(unitx, 0.48, PX(xplanet[i])),
								 cy + POINT1(unity, 0.48, PY(xplanet[i])),
								 cx + POINT1(unitx, 0.48, PX(xplanet[j])),
								 cy + POINT1(unity, 0.48, PY(xplanet[j])),
								 (int) (fabs((double) (grid->v[i][j])) / (GetOrb(i, j, grid->n[i][j])) / 15.0));
					}
					else
					{
						if (p / SolidAspect <= 0.05)
							n = 20;
						else
							n = (int) (SolidAspect / p);
						DrawDash(cx + POINT1(unitx, 0.48, PX(xplanet[i])),
								 cy + POINT1(unity, 0.48, PY(xplanet[i])),
								 cx + POINT1(unitx, 0.48, PX(xplanet[j])), cy + POINT1(unity, 0.48, PY(xplanet[j])), n);
					}
				}
	}
	for (j = cObj; j >= 1; j--)
		for (i = j - 1; i >= 0; i--)
			if (grid->n[i][j] && FProper(i) && FProper(j))
			{
				k = grid->n[i][j];
				ip = i <= cLastMoving ? power1[i] : 2.5;
				jp = j <= cLastMoving ? power1[j] : 2.5;
				p = (rAspInf[k] * (sqrt) (ip * jp) * (1.0 - fabs((double) (grid->v[i][j])) / 60.0 / GetOrb(i, j, k)));
				if (us.fParallel)
					p = p * PowerPar;
				if (FCusp(i) && FCusp(j))
					continue;
				if (k == aOpp || k == aSqu || k == aSSq || k == aSes)
					STension -= ccc * p;
				if (k == aSex || k == aTri || k == aSSx || k == aInc)
					SHarmon += ccc * p;
				if (k == aCon)
					SConjunct += ccc * p;
				SSumma += ccc * p;
			}
	SChart = SHarmon + STension;

	/* Go draw sidebar with chart information and positions if need be. */

	DrawInfo();
}


/* Draw an astro-graph chart on a map of the world, i.e. the draw the     */
/* Ascendant, Descendant, Midheaven, and Nadir lines corresponding to the */
/* time in the chart. This chart is done when the -L switch is combined   */
/* with the -X switch.                                                    */

void XChartAstroGraph()
{
	double planet1[NUMBER_OBJECTS], planet2[NUMBER_OBJECTS],
		end1[cObj * 2 + 1], end2[cObj * 2 + 1],
		symbol1[cObj * 2 + 1], symbol2[cObj * 2 + 1], lon, longm, x, y, z, ad, oa, am, od, dm, lat;
	int unit = gi.nScale, fStroke, lat1 = -60, lat2 = 75, y1, y2, xold1, xold2, i, j, k, l;

	lon = DegMin2DecDeg(ciMain.lon);

	/* Erase top and bottom parts of map. We don't draw the astro-graph lines */
	/* above certain latitudes, and this gives us room for glyph labels, too. */

	y1 = (91 - lat1) * gi.nScale;
	y2 = (91 - lat2) * gi.nScale;
	DrawColor(gi.kiOff);
	DrawBlock(0, 1, gs.xWin - 1, y2 - 1);
	DrawBlock(0, y1 + 1, gs.xWin - 1, gs.yWin - 2);
	DrawColor(gi.kiLite);
	DrawDash(0, gs.yWin / 2, gs.xWin - 2, gs.yWin / 2, 4);	/* Draw equator. */
	DrawColor(gi.kiOn);
	DrawLine(1, y2, gs.xWin - 2, y2);
	DrawLine(1, y1, gs.xWin - 2, y1);
	for (i = 1; i <= cObj * 2; i++)
		end1[i] = end2[i] = -rLarge;

	/* Draw small hatches every 5 degrees along edges of world map. */

	DrawColor(gi.kiLite);
	for (i = lat1; i <= lat2; i += 5)
	{
		j = (91 - i) * gi.nScale;
		k = (2 + (i % 10 == 0) + 2 * (i % 30 == 0)) * gi.nScaleT;
		DrawLine(1, j, k, j);
		DrawLine(gs.xWin - 2, j, gs.xWin - 1 - k, j);
	}
	for (i = -180; i < 180; i += 5)
	{
		j = (180 - i) * gi.nScale;
		k = (2 + (i % 10 == 0) + 2 * (i % 30 == 0) + (i % 90 == 0)) * gi.nScaleT;
		DrawLine(j, y2 + 1, j, y2 + k);
		DrawLine(j, y1 - 1, j, y1 - k);
	}
	if (us.fLatitudeCross)
	{
		DrawColor(kRainbowB[7]);
		i = (int) ((91.0 - ciMain.lat) * (double) gi.nScale);
		DrawLine(0, i, gs.xWin - 1, i);
	}

	/* Calculate zenith locations of each planet. */

	for (i = 1; i <= cObj; i++)
		if (!ignore[i] || i == oMC)
		{
			planet1[i] = Deg2Rad(Tropical(i == oMC ? is.MC : cp0.longitude[i]));
			planet2[i] = Deg2Rad(cp0.latitude[i]);
			EclToEqu(&planet1[i], &planet2[i]);
		}

	/* Draw the Midheaven lines and zenith location markings. */

	if (lon < 0.0)
		lon += 360.0;
	for (i = 1; i <= cObj; i++)
		if (FProper(i))
		{
			x = planet1[oMC] - planet1[i];
			if (x < 0.0)
				x += 2.0 * rPi;
			if (x > rPi)
				x -= 2.0 * rPi;
			z = lon + Rad2Deg(x);
			if (z > 180.0)
				z -= 360.0;
			j = (int) (Mod(180.0 - z + gs.nRot) * (double) gi.nScale);
			DrawColor(kElemB[eEar]);
			DrawLine(j, y1 + unit * 4, j, y2 - unit * 1);
			end2[i * 2 - 1] = (double) j;
			y = Rad2Deg(planet2[i]);
			k = (int) ((91.0 - y) * (double) gi.nScale);
			if (FBetween((int) y, lat1, lat2))
			{
				DrawColor(gi.kiLite);
				DrawBlock(j - gi.nScaleT, k - gi.nScaleT, j + gi.nScaleT, k + gi.nScaleT);
				DrawColor(gi.kiOff);
				DrawBlock(j, k, j, k);
			}

			/* Draw Nadir lines assuming we aren't in bonus chart mode. */

			if (!gs.fBonusMode)
			{
				j += 180 * gi.nScale;
				if (j > gs.xWin - 2)
					j -= (gs.xWin - 2);
				end1[i * 2 - 1] = (double) j;
				DrawColor(kElemB[eWat]);
				DrawLine(j, y1 + unit * 2, j, y2 - unit * 2);
			}
		}

	/* Now, normally, unless we are in bonus chart mode, we will go on to draw */
	/* the Ascendant and Descendant lines here.                                */

	longm = Deg2Rad(Mod(Rad2Deg(planet1[oMC]) + lon));
	if (!gs.fBonusMode)
		for (i = 1; i <= cObj; i++)
			if (FProper(i))
			{
				xold1 = xold2 = -1000;

				/* Hack: Normally we draw the Ascendant and Descendant line segments  */
				/* simultaneously. However, for the PostScript and metafile stroke    */
				/* graphics, this will case the file to get inordinately large due to */
				/* the constant thrashing between the Asc and Desc colors. Hence for  */
				/* these charts only, we'll do two passes for Asc and Desc.           */
				fStroke = gs.fPS || gs.fMeta;
				for (l = 0; l <= fStroke; l++)

					for (lat = (double) lat1; lat <= (double) lat2; lat += 1.0 / (double) (gi.nScale / gi.nScaleT))
					{

						/* First compute and draw the current segment of Ascendant line. */

						j = (int) ((91.0 - lat) * (double) gi.nScale);
						ad = tan(planet2[i]) * tan(Deg2Rad(lat));
						if (ad * ad > 1.0)
							ad = rLarge;
						else
						{
							ad = asin(ad);
							oa = planet1[i] - ad;
							if (oa < 0.0)
								oa += 2.0 * rPi;
							am = oa - rPiHalf;
							if (am < 0.0)
								am += 2.0 * rPi;
							z = longm - am;
							if (z < 0.0)
								z += 2.0 * rPi;
							if (z > rPi)
								z -= 2.0 * rPi;
							z = Rad2Deg(z);
							k = (int) (Mod(180.0 - z + gs.nRot) * (double) gi.nScale);
							if (!fStroke || !l)
							{
								DrawColor(kElemB[eFir]);
								DrawWrap(xold1, j + gi.nScaleT, k, j, 1, gs.xWin - 2);
								if (lat == (double) lat1)
								{	/* Line segment */
									DrawLine(k, y1, k, y1 + unit * 4);	/* pointing to  */
									end2[i * 2] = (double) k;	/* Ascendant.   */
								}
							}
							else if (lat == (double) lat1)
								end2[i * 2] = (double) k;
							xold1 = k;
						}

						/* The curving Ascendant and Descendant lines actually touch at low or */
						/* high latitudes. Sometimes when we start out, a particular planet's  */
						/* lines haven't appeared yet, i.e. we are scanning at a latitude      */
						/* where our planet's lines don't exist. If this is the case, then     */
						/* when they finally do start, draw a thin horizontal line connecting  */
						/* the Ascendant and Descendant lines so they don't just start in      */
						/* space. Note that these connected lines aren't labeled with glyphs.  */

						if (ad == rLarge)
						{
							if (xold1 >= 0)
							{
								if (!fStroke || !l)
								{
									DrawColor(gi.kiGray);
									DrawWrap(xold1, j + 1, xold2, j + 1, 1, gs.xWin - 2);
								}
								lat = 90.0;
							}
						}
						else
						{

							/* Then compute and draw corresponding segment of Descendant line. */

							od = planet1[i] + ad;
							dm = od + rPiHalf;
							z = longm - dm;
							if (z < 0.0)
								z += 2.0 * rPi;
							if (z > rPi)
								z -= 2.0 * rPi;
							z = Rad2Deg(z);
							k = (int) (Mod(180.0 - z + gs.nRot) * (double) gi.nScale);
							if (xold2 < 0 && lat > (double) lat1 && (!fStroke || l))
							{
								DrawColor(gi.kiGray);
								DrawWrap(xold1, j, k, j, 1, gs.xWin - 2);
							}
							if (!fStroke || l)
							{
								DrawColor(kElemB[eAir]);
								DrawWrap(xold2, j + gi.nScaleT, k, j, 1, gs.xWin - 2);
								if (lat == (double) lat1)	/* Line segment */
									DrawLine(k, y1, k, y1 + unit * 2);	/* pointing to  */
							}	/* Descendant.  */
							xold2 = k;
						}
					}

				/* Draw segments pointing to top of Ascendant and Descendant lines. */

				if (ad != rLarge)
				{
					DrawColor(kElemB[eFir]);
					DrawLine(xold1, y2, xold1, y2 - unit * 1);
					DrawColor(kElemB[eAir]);
					DrawLine(k, y2, k, y2 - unit * 2);
					end1[i * 2] = (double) k;
				}
			}

	DrawColor(kMainB[8]);
	i = (int) ((181.0 - ciMain.lon) * (double) gi.nScale);
	j = (int) ((91.0 - ciMain.lat) * (double) gi.nScale);
	if (us.fLatitudeCross)
		DrawSpot(i, j);
	else
		DrawPoint(i, j);

	/* Determine where to draw the planet glyphs. We have four sets of each    */
	/* planet - each planet's glyph appearing in the chart up to four times -  */
	/* one for each type of line. The Midheaven and Ascendant lines are always */
	/* labeled at the bottom of the chart, while the Nadir and Midheaven lines */
	/* at the top. Therefore we need to place two sets of glyphs, twice.       */

	for (i = 1; i <= cObj * 2; i++)
	{
		symbol1[i] = end1[i];
		symbol2[i] = end2[i];
	}
	FillSymbolLine(symbol1);
	FillSymbolLine(symbol2);

	/* Now actually draw the planet glyphs. */

	for (i = 1; i <= cObj * 2; i++)
	{
		j = (i + 1) / 2;
		if (FProper(j))
		{
			if ((gi.xTurtle = (int) symbol1[i]) > 0 && gs.fLabel)
			{
				DrawColor(cp0.vel_longitude[j] < 0.0 ? gi.kiGray : gi.kiOn);
				DrawDash((int) end1[i], y2 - unit * 2, (int) symbol1[i],
						 y2 - unit * 4, (cp0.vel_longitude[i] < 0.0 ? 1 : 0) - gs.fColor);
				DrawObject(j, gi.xTurtle, y2 - unit * 10);
			}
			if ((gi.xTurtle = (int) symbol2[i]) > 0)
			{
				DrawColor(cp0.vel_longitude[j] < 0.0 ? gi.kiGray : gi.kiOn);
				DrawDash((int) end2[i], y1 + unit * 4, (int) symbol2[i],
						 y1 + unit * 8, (cp0.vel_longitude[i] < 0.0 ? 1 : 0) - gs.fColor);
				DrawObject(j, gi.xTurtle, y1 + unit * 14);
				DrawTurtle(szDrawObject[i & 1 ? oMC : oAsc], (int) symbol2[i], y1 + unit * 24 - gi.nScaleT);
			}
		}
	}
}

/************************************************************************************/

void XDebugFont() {
	int i, s;
	int yOffset = 20;
	int xOffset = 34;
	int y = 10;
	char t[256];

	for(s = 100; s < 255; s++) t[s-100] = s;
	
	t[s-32] = 0;
	DrawColor(gi.kiLite);
	s  = gi.nScale;
	gi.nScale =  1;
	DrawSz(ciMain.nam,10,y, dtLeft | dtScale);
	y += yOffset;
	gi.nScale =  2;
	DrawSz(ciMain.nam,10,y, dtLeft | dtScale);
	y += yOffset+4;
	gi.nScale =  3;
	DrawSz(ciMain.nam,10,y, dtLeft | dtScale);
	y += yOffset;
	gi.nScale =  2;

	for(i=0; i<4;i++) {
		for(s = 32+i*56; s < 32+(i+1)*56 ; s++) t[s- 32 - 56 * i] = s;
		t[56] = 0;
		DrawSz(t,10,y+(yOffset+5)*i, dtLeft | dtScale);
	}
    

	y += 35;

	for(i=1;i<=4;i++) {
		gi.nScale =  i;
		DrawColor(gi.kiLite);
		for(s = 1; s <=12;s++) DrawSign(s, 20+xOffset*s, y +i*3*yOffset);
		for(s = 1; s <=(NUMBER_OF_SIGNS);s++) DrawHouse(s, 20+xOffset*s +xOffset*12, y +i*3*yOffset);
		for(s = 0; s <(24);s++) DrawObject(s, 20+xOffset*s, y +i*3*yOffset + yOffset);
		for(s = 24; s <(cLastMoving + 5);s++) DrawObject(s, 20+xOffset*(s-24), y +i*3*yOffset + yOffset*2);
		DrawColor(gi.kiLite);
		for(s = 1; s <(cAspect+3);s++)
			DrawAspect(s, 20+xOffset*s, y +i*3*yOffset + yOffset*3);//DrawAspect(1, 10, 360);
		yOffset+=6;
	}
		gi.nScale = s;

}

/* Draw an aspect and midpoint grid in the window, with planets labeled down */
/* the diagonal. This chart is done when the -g switch is combined with the  */
/* -X switch. The chart always has a certain number of cells; hence based    */
/* how the restrictions are set up, there may be blank columns and rows,     */
/* or else only the first number of unrestricted objects will be included.   */

void XChartGrid()
{
	char sz[cchSzDef];
	int unit, siz, x, y, i, j, k;
	KI c;

	unit = CELLSIZE * gi.nScale;
	siz = gs.nGridCell * unit;
	if (!FCreateGrid(gs.fBonusMode))
		return;

	/* Loop through each cell in each row and column of grid. */

	for (y = 1, j = oEar - 1; y <= gs.nGridCell; y++)
	{
		do
		{
			j++;
		}
		while (!FProper(j) && j <= cObj);
		DrawColor(gi.kiGray);
		DrawDash(0, y * unit, siz, y * unit, !gs.fColor);
		DrawDash(y * unit, 0, y * unit, siz, !gs.fColor);
		if (j <= cObj)
			for (x = 1, i = oEar - 1; x <= gs.nGridCell; x++)
			{
				do
				{
					i++;
				}
				while (!FProper(i) && i <= cObj);
				if (i <= cObj)
				{
					gi.xTurtle = x * unit - unit / 2;
					gi.yTurtle = y * unit - unit / 2 - (gi.nScale / gi.nScaleT > 2 ? 5 * gi.nScaleT : 0);
					k = grid->n[i][j];

					/* If this is an aspect cell, draw glyph of aspect in effect. */

					if (gs.fBonusMode ? x > y : x < y)
					{
						if (k)
						{
							DrawColor(c = kAspB[k]);
							DrawAspect(k, gi.xTurtle, gi.yTurtle);
						}

						/* If this is a midpoint cell, draw glyph of sign of midpoint. */

					}
					else if (gs.fBonusMode ? x < y : x > y)
					{
						DrawColor(c = kSignB(grid->n[i][j]));
						DrawSign(grid->n[i][j], gi.xTurtle, gi.yTurtle);

						/* For cells on main diagonal, draw glyph of planet. */

					}
					else
					{
						DrawColor(gi.kiLite);
						DrawEdge((y - 1) * unit, (y - 1) * unit, y * unit, y * unit);
						DrawObject(i, gi.xTurtle, gi.yTurtle);
					}

					/* When the scale size is 300+, we can print text in each cell: */

					if (gi.nScale / gi.nScaleT > 2 && gs.fLabel)
					{
						k = abs(grid->v[i][j]);

						/* For the aspect portion, print the orb in degrees and minutes. */

						if (gs.fBonusMode ? x > y : x < y)
						{
							if (grid->n[i][j])
								sprintf(sz, "%c%d %02d'",
										k !=
										grid->v[i][j] ? (us.
														 fAppSep ? 'a' : '-')
										: (us.fAppSep ? 's' : '+'), k / 60, k % 60);
							else
								sprintf(sz, "");

							/* For the midpoint portion, print the degrees and minutes. */

						}
						else if (gs.fBonusMode ? x < y : x > y)
							sprintf(sz, "%2d %02d'", k / 60, k % 60);

						/* For the main diagonal, print degree and sign of each planet. */

						else
						{
							c = kSignB(grid->n[i][j]);
							sprintf(sz, "%c%c%c %02d", chSig3(grid->n[i][j]), k);
						}
						DrawColor(c);
						DrawSz(sz, x * unit - unit / 2, y * unit - 3 * gi.nScaleT, dtBottom);
					}
				}
			}
	}
}


/* Draw the local horizon, and draw in the planets where they are at the */
/* time in question, as done when the -Z is combined with the -X switch. */

void XChartHorizon()
{
	double lat, lonz[NUMBER_OBJECTS], latz[NUMBER_OBJECTS], azi[NUMBER_OBJECTS], alt[NUMBER_OBJECTS];
	int x[NUMBER_OBJECTS], y[NUMBER_OBJECTS], m[NUMBER_OBJECTS], n[NUMBER_OBJECTS], cx, cy, unit, x1, y1, x2, y2, xs, ys, i, j, k, l;
	char sz[2];
	double lonMC, lon1, lat1, alt1, azi1;
	int xx1, yy1;

	unit = Max(12, 6 * gi.nScale);
	x1 = unit;
	y1 = unit;
	x2 = gs.xWin - 1 - unit;
	y2 = gs.yWin - 1 - unit;
	unit = 12 * gi.nScale;
	xs = x2 - x1;
	ys = y2 - y1;
	cx = (x1 + x2) / 2;
	cy = (y1 + y2) / 2;

	/* Make a slightly smaller rectangle within the window to draw the planets */
	/* in. Make segments on all four edges marking 5 degree increments.        */

	DrawColor(gi.kiLite);
	for (i = 5; i < 180; i += 5)
	{
		j = y1 + (int) ((double) i * (double) ys / 180.0);
		k = (2 + (i % 10 == 0) + 2 * (i % 30 == 0)) * gi.nScaleT;
		DrawLine(x1 + 1, j, x1 + 1 + k, j);
		DrawLine(x2 - 1, j, x2 - 1 - k, j);
	}
	sz[1] = chNull;
	for (i = 5; i < 360; i += 5)
	{
		j = x1 + (int) ((double) i * (double) xs / 360.0);
		k = (2 + (i % 10 == 0) + 2 * (i % 30 == 0)) * gi.nScaleT;
		DrawLine(j, y1 + 1, j, y1 + 1 + k);
		DrawLine(j, y2 - 1, j, y2 - 1 - k);
		if (i % 90 == 0)
		{
			*sz = *Lang(tDir, i / 90 & 3);
			DrawSz(sz, j, y1 - 2 * gi.nScaleT, dtBottom);
		}
	}

	/* Draw vertical lines dividing our rectangle into four areas. In our     */
	/* local space chart, the middle line represents due south, the left line */
	/* due east, the right line due west, and the edges due north. A fourth   */
	/* horizontal line divides that which is above and below the horizon.     */

	DrawColor(gi.kiGray);
	DrawDash(cx, y1, cx, y2, 1);
	DrawDash((cx + x1) / 2, y1, (cx + x1) / 2, y2, 1);
	DrawDash((cx + x2) / 2, y1, (cx + x2) / 2, y2, 1);
	DrawColor(gi.kiOn);
	DrawEdge(x1, y1, x2, y2);
	DrawDash(x1, cy, x2, cy, 1);

	/* Calculate the local horizon coordinates of each planet. First convert */
	/* zodiac position and declination to zenith longitude and latitude.     */

	lat = Deg2Rad(DegMin2DecDeg(ciMain.lat));
	for (i = 0; i <= cObj; i++)
		if (!ignore[i] || i == oMC)
		{
			if (us.fEquator)
			{
				lonz[i] = Deg2Rad(cp0.longitude[i]);
				latz[i] = Deg2Rad(cp0.latitude[i]);
			}
			else
			{
				lonz[i] = Deg2Rad(Tropical(cp0.longitude[i]));
				latz[i] = Deg2Rad(cp0.latitude[i]);
				EclToEqu(&lonz[i], &latz[i]);
			}
		}
	if (PolarMCflip && hRevers)
		lonMC = Mod(lonz[oMC] + rPi);
	else
		lonMC = lonz[oMC];

	if (!fEquator)
		for (i = 0; i < 180; i++)
		{
			lon1 = Deg2Rad(2 * (double) i);
			lat1 = 0.0;
			lon1 = Deg2Rad(Mod(Rad2Deg(lonMC - lon1 + rPiHalf)));
			EquToLocal(&lon1, &lat1, rPiHalf - lat);
			azi1 = 360.0 - Rad2Deg(lon1);
			alt1 = Rad2Deg(lat1);
			xx1 = x1 + (int) ((double) xs * (Mod(90.0 - azi1)) / 360.0 + 0.5);
			yy1 = y1 + (int) ((double) ys * (90.0 - alt1) / 180.0 + 0.5);
			DrawColor(kLtGray);
			DrawPoint(xx1, yy1);
		}
	for (i = 0; i < 180; i++)
	{
		lon1 = 2 * (double) i;
		lat1 = 0.0;
		lon1 = Deg2Rad(Tropical(lon1));
		EclToEqu(&lon1, &lat1);
		lon1 = Deg2Rad(Mod(Rad2Deg(lonMC - lon1 + rPiHalf)));
		if (!fEquator)
			EquToLocal(&lon1, &lat1, rPiHalf - lat);
		azi1 = 360.0 - Rad2Deg(lon1);
		alt1 = Rad2Deg(lat1);
		xx1 = x1 + (int) ((double) xs * (Mod(90.0 - azi1)) / 360.0 + 0.5);
		yy1 = y1 + (int) ((double) ys * (90.0 - alt1) / 180.0 + 0.5);
		DrawColor(kDkGray);
		if (RFract((double) i / 15.0) <= 0.0 && i != 0)
			DrawColor(kWhite);
		if (!i)
			DrawColor(kRed);
		if (i == 90)
			DrawColor(kGreen);
		DrawPoint(xx1, yy1);
	}

	for (i = 0; i <= cObj; i++)
		if (FProper(i))
		{
			lonz[i] = Deg2Rad(Mod(Rad2Deg(lonMC - lonz[i] + rPiHalf)));
			if (!fEquator)
				EquToLocal(&lonz[i], &latz[i], rPiHalf - lat);
			azi[i] = 360.0 - Rad2Deg(lonz[i]);
			alt[i] = Rad2Deg(latz[i]);
			x[i] = x1 + (int) ((double) xs * (Mod(90.0 - azi[i])) / 360.0 + 0.5);
			y[i] = y1 + (int) ((double) ys * (90.0 - alt[i]) / 180.0 + 0.5);
			m[i] = x[i];
			n[i] = y[i] + unit / 2;
		}

	/* As in the DrawGlobe() routine, we now determine where to draw the   */
	/* glyphs in relation to the actual points, so that the glyphs aren't  */
	/* drawn on top of each other if possible. Again, we assume that we'll */
	/* put the glyph right under the point, unless there would be some     */
	/* overlap and the above position is better off.                       */

	for (i = 0; i <= cObj; i++)
		if (FProper(i))
		{
			k = l = gs.xWin + gs.yWin;
			for (j = 1; j < i; j++)
				if (FProper(j))
				{
					k = Min(k, abs(m[i] - m[j]) + abs(n[i] - n[j]));
					l = Min(l, abs(m[i] - m[j]) + abs(n[i] - unit - n[j]));
				}
			if (k < unit || l < unit)
				if (k < l)
					n[i] -= unit;
		}
	for (i = cObj; i >= 0; i--)
		if (FProper(i))			/* Draw planet's glyph. */
			DrawObject(i, m[i], n[i]);
	for (i = cObj; i >= 0; i--)
		if (FProper(i))
		{
			DrawColor(kObjB[i]);
			if (!gs.fBonusMode)
				DrawPoint(x[i], y[i]);	/* Draw small or large dot */
			else				/* near glyph indicating   */
				DrawSpot(x[i], y[i]);	/* exact local location.   */
		}
}


/* Draw the local horizon, and draw in the planets where they are at the  */
/* time in question. This chart is done when the -Z0 is combined with the */
/* -X switch. This is an identical function to XChartHorizon(); however,  */
/* that routine's chart is entered on the horizon and meridian. Here we   */
/* center the chart around the center of the sky straight up from the     */
/* local horizon, with the horizon itself being an encompassing circle.   */

void XChartHorizonSky()
{
	double lat, rx, ry, s, lonz[NUMBER_OBJECTS], latz[NUMBER_OBJECTS], azi[NUMBER_OBJECTS], alt[NUMBER_OBJECTS];
	int x[NUMBER_OBJECTS], y[NUMBER_OBJECTS], m[NUMBER_OBJECTS], n[NUMBER_OBJECTS], cx, cy, unit, x1, y1, x2, y2, xs, ys, i, j, k, l;
	double lonMC, coef;
	int ccx, ccy;

	unit = Max(12, 6 * gi.nScale);
	x1 = unit;
	y1 = unit;
	x2 = gs.xWin - 1 - unit;
	y2 = gs.yWin - 1 - unit;
	unit = 12 * gi.nScale;
	xs = x2 - x1;
	ys = y2 - y1;
	cx = (x1 + x2) / 2;
	cy = (y1 + y2) / 2;

	/* Draw a circle in window to indicate horizon line, lines dividing   */
	/* the window into quadrants to indicate n/s and w/e meridians, and   */
	/* segments on these lines marking 5 degree increments.               */

	coef = 1.8;
	DrawColor(gi.kiGray);
	DrawDash(cx, y1, cx, y2, 1);
	DrawDash(x1, cy, x2, cy, 1);
	DrawColor(gi.kiLite);
	for (i = -160; i <= 160; i += 5)
	{
		k = (2 + (i / 10 * 10 == i ? 1 : 0) + (i / 30 * 30 == i ? 2 : 0)) * gi.nScaleT;
		s = 1.0 / (90.0 * coef);
		j = cy + (int) (s * ys / 2 * i);
		DrawLine(cx - k, j, cx + k, j);
		j = cx + (int) (s * xs / 2 * i);
		DrawLine(j, cy - k, j, cy + k);
	}
	DrawSz(gs.fBonusMode ? "Z" : "N", cx, y1 - 2 * gi.nScaleT, dtBottom);
	DrawSz("E", x1 / 2, cy + 2 * gi.nScaleT, dtCent);
	DrawSz("W", (gs.xWin + x2) / 2, cy + 2 * gi.nScaleT, dtCent);
	if (!gs.fText)
		DrawSz(gs.fBonusMode ? "N" : "S", cx, gs.yWin - 3 * gi.nScaleT, dtBottom);
	rx = xs / 2 / coef;
	ry = ys / 2 / coef;
	DrawColor(gi.kiOn);
	DrawEdge(x1, y1, x2, y2);
	DrawCircle(cx, cy, (int) rx, (int) ry);
	for (i = 0; i < 360; i += 5)
	{
		k = (2 + (i / 10 * 10 == i ? 1 : 0) + (i / 30 * 30 == i ? 2 : 0)) * gi.nScaleT;
		DrawLine(cx + (int) ((rx - k) * RCosD((double) i)),
				 cy + (int) ((ry - k) * RSinD((double) i)),
				 cx + (int) ((rx + k) * RCosD((double) i)), cy + (int) ((ry + k) * RSinD((double) i)));
	}

	/* Draw outhern dashed circle, which represents nadir point, which is */
	/* now on screen in corners, because I rescaled picture. V.A.         */

	rx = xs / coef;
	ry = ys / coef;
	for (i = 0; i < 360; i++)
	{
		ccx = (int) (rx * RCosD((double) i)) + cx;
		ccy = (int) (ry * RSinD((double) i)) + cy;
		if (ccx < x1 || ccx > x2 || ccy < y1 || ccy > y2)
			continue;
		DrawPoint(ccx, ccy);
	}
	for (i = 0; i < 360; i += 5)
	{
		if (i < 27 || FBetween(i, 62, 118) || FBetween(i, 152, 208) || FBetween(i, 242, 298) || i > 333)
			continue;
		k = (1 + (i / 10 * 10 == i ? 1 : 0) + (i / 30 * 30 == i ? 1 : 0)) * gi.nScaleT;
		DrawLine(cx + (int) ((rx - k) * RCosD((double) i)),
				 cy + (int) ((ry - k) * RSinD((double) i)),
				 cx + (int) ((rx + k) * RCosD((double) i)), cy + (int) ((ry + k) * RSinD((double) i)));
	}
	rx = xs / 2 / coef;
	ry = ys / 2 / coef;

	/* Calculate the local horizon coordinates of each planet. First convert */
	/* zodiac position and declination to zenith longitude and latitude.     */

	lat = Deg2Rad(DegMin2DecDeg(ciMain.lat));
	for (i = 0; i <= cObj; i++)
		if (!ignore[i] || i == oMC)
		{
			if (us.fEquator)
			{
				lonz[i] = Deg2Rad(cp0.longitude[i]);
				latz[i] = Deg2Rad(cp0.latitude[i]);
			}
			else
			{
				lonz[i] = Deg2Rad(Tropical(cp0.longitude[i]));
				latz[i] = Deg2Rad(cp0.latitude[i]);
				EclToEqu(&lonz[i], &latz[i]);
			}
		}
	if (PolarMCflip && hRevers)
		lonMC = Mod(lonz[oMC] + rPi);
	else
		lonMC = lonz[oMC];

	for (i = 0; i <= cObj; i++)
		if (FProper(i) && i != oMC)
		{
			lonz[i] = Deg2Rad(Mod(Rad2Deg(lonMC - lonz[i] + rPiHalf)));
			if (!fEquator)
				EquToLocal(&lonz[i], &latz[i], rPiHalf - lat);
			azi[i] = 360.0 - Rad2Deg(lonz[i]);

			/* In bonus mode convert from altitude/azimuth coordinates to */
			/* prime vertical coordinates that we'll print instead.       */

			if (gs.fBonusMode)
			{
				alt[i] = Rad2Deg(latz[i]);
				azi[i] = Deg2Rad(azi[i]);
				alt[i] = Deg2Rad(alt[i]);
				CoorXform(&azi[i], &alt[i], rPiHalf);
				azi[i] = 360.0 - Rad2Deg(azi[i]);
				alt[i] = 90.0 + Rad2Deg(alt[i]);
			}
			else
			{
				alt[i] = 90.0 - Rad2Deg(latz[i]);
			}
			s = alt[i] / 90.0;
			x[i] = cx + (int) (rx * s * RCosD(180.0 + azi[i]) + 0.5);
			y[i] = cy + (int) (ry * s * RSinD(180.0 + azi[i]) + 0.5);
			if (!FOnWin(x[i], y[i]))
				x[i] = -1000;
			m[i] = x[i];
			n[i] = y[i] + unit / 2;
		}

	/* As in the DrawGlobe() routine, we now determine where to draw the   */
	/* glyphs in relation to the actual points, so that the glyphs aren't  */
	/* drawn on top of each other if possible. Again, we assume that we'll */
	/* put the glyph right under the point, unless there would be some     */
	/* overlap and the above position is better off.                       */

	for (i = 0; i <= cObj; i++)
		if (FProper(i))
		{
			k = l = gs.xWin + gs.yWin;
			for (j = 0; j < i; j++)
				if (FProper(j))
				{
					k = Min(k, abs(m[i] - m[j]) + abs(n[i] - n[j]));
					l = Min(l, abs(m[i] - m[j]) + abs(n[i] - unit - n[j]));
				}
			if (k < unit || l < unit)
				if (k < l)
					n[i] -= unit;
		}
	for (i = cObj; i >= 0; i--)
		if (m[i] >= x1 && FProper(i))	/* Draw glyph. */
			DrawObject(i, m[i], n[i]);
	for (i = cObj; i >= 0; i--)
		if (x[i] >= y1 && FProper(i))
		{
			DrawColor(kObjB[i]);
			DrawSpot(x[i], y[i]);
		}
}


/* Draw a chart depicting an aerial view of the solar system in space, with */
/* all the planets drawn around the Sun, and the specified central planet   */
/* in the middle, as done when the -S is combined with the -X switch.       */

void XChartOrbit()
{
	int x[NUMBER_OBJECTS], y[NUMBER_OBJECTS], m[NUMBER_OBJECTS], n[NUMBER_OBJECTS],
		cx = gs.xWin / 2, cy = gs.yWin / 2, unit, x1, y1, x2, y2, i, j, k, l;
	double sx, sy, sz = 30.0, xp, yp, a;

	unit = Max(gs.fText * 12, 6 * gi.nScale);
	x1 = unit;
	y1 = unit;
	x2 = gs.xWin - 1 - unit;
	y2 = gs.yWin - 1 - unit;
	unit = 12 * gi.nScale;

	/* Determine the scale of the chart. For a scale size of 400+, make the */
	/* graphic 1 AU in radius (just out to Earth's orbit). For 300, make    */
	/* the chart 6 AU in radius (enough for inner planets out to asteroid   */
	/* belt). For a scale of 200, make window 30 AU in radius (enough for   */
	/* planets out to Neptune). For scale of 100, make it 90 AU in radius   */
	/* (enough for all planets including the orbits of the uranians.)       */

	if (gi.nScale / gi.nScaleT < 2)
		sz = 90.0;
	else if (gi.nScale / gi.nScaleT == 3)
		sz = 6.0;
	else if (gi.nScale / gi.nScaleT > 3)
		sz = 1.0;
	sx = (double) (cx - x1) / sz;
	sy = (double) (cy - y1) / sz;
	for (i = 0; i <= cLastMoving; i++)
		if (FProper(i))
		{
			xp = spacex[i];
			yp = spacey[i];
			x[i] = cx - (int) (xp * sx);
			y[i] = cy + (int) (yp * sy);
			m[i] = x[i];
			n[i] = y[i] + unit / 2;
		}

	/* As in the DrawGlobe() routine, we now determine where to draw the   */
	/* glyphs in relation to the actual points, so that the glyphs aren't  */
	/* drawn on top of each other if possible. Again, we assume that we'll */
	/* put the glyph right under the point, unless there would be some     */
	/* overlap and the above position is better off.                       */

	for (i = 0; i <= cLastMoving; i++)
		if (FProper(i))
		{
			k = l = gs.xWin + gs.yWin;
			for (j = 0; j < i; j++)
				if (FProper(j))
				{
					k = Min(k, abs(m[i] - m[j]) + abs(n[i] - n[j]));
					l = Min(l, abs(m[i] - m[j]) + abs(n[i] - unit - n[j]));
				}
			if (k < unit || l < unit)
				if (k < l)
					n[i] -= unit;
		}

	/* Draw the 12 sign boundaries from the center body to edges of screen. */

	a = Mod(Rad2Deg(Angle(spacex[oJup], spacey[oJup])) - cp0.longitude[oJup]);
	DrawColor(gi.kiGray);
	for (i = 0; i < NUMBER_OF_SIGNS; i++)
	{
		k = cx + 2 * (int) ((double) cx * RCosD((double) i * 30.0 + a));
		l = cy + 2 * (int) ((double) cy * RSinD((double) i * 30.0 + a));
		DrawClip(cx, cy, k, l, x1, y1, x2, y2, 1);
	}
	DrawColor(gi.kiLite);
	DrawEdge(x1, y1, x2, y2);
	for (i = cLastMoving; i >= 0; i--)
		if (FProper(i) && FInRect(m[i], n[i], x1, y1, x2, y2))
			DrawObject(i, m[i], n[i]);
	for (i = cLastMoving; i >= 0; i--)
		if (FProper(i) && FInRect(x[i], y[i], x1, y1, x2, y2))
		{
			DrawColor(kObjB[i]);
			if (!gs.fBonusMode || i > cLastMoving)
				DrawPoint(x[i], y[i]);	/* Draw small or large dot */
			else				/* near glyph indicating   */
				DrawSpot(x[i], y[i]);	/* exact orbital location. */
		}
}


/* Draw a chart showing the 36 Gauquelin sectors, with all the planets    */
/* positioned in their appropriate sector (and at the correct fracton     */
/* across the sector) as done when the -l is combined with the -X switch. */

void XChartSector()
{
	double xplanet[NUMBER_OBJECTS], symbol[NUMBER_OBJECTS];
	char sz[3];
	int cx, cy, i, j, k;
	double unitx, unity, px, py, temp;
	byte ignoreT[NUMBER_OBJECTS + 1];

	for (i = 1; i <= NUMBER_OBJECTS; i++)
		ignoreT[i] = ignore[i];

	if (gs.fText && !us.fVelocity)
		gs.xWin -= (SIDESIZE*gi.nScaleT);
	cx = gs.xWin / 2 - 1;
	cy = gs.yWin / 2 - 1;
	unitx = (double) cx;
	unity = (double) cy;

	/* Draw lines across the whole chart at the four angles. */

	DrawColor(gi.kiLite);
	DrawDash(cx + POINT1(unitx, 0.99, PX(0.0)),
			 cy + POINT1(unity, 0.99, PY(0.0)),
			 cx + POINT1(unitx, 0.99, PX(180.0)), cy + POINT1(unity, 0.99, PY(180.0)), !gs.fColor);
	DrawDash(cx + POINT1(unitx, 0.99, PX(90.0)),
			 cy + POINT1(unity, 0.99, PY(90.0)),
			 cx + POINT1(unitx, 0.99, PX(270.0)), cy + POINT1(unity, 0.99, PY(270.0)), !gs.fColor);

	/* Draw circles and radial lines delineating the 36 sectors. */

	DrawColor(gi.kiOn);
	for (i = 0; i < 360; i += 10)
	{
		px = PX((double) i);
		py = PY((double) i);
		DrawLine(cx + POINT1(unitx, 0.81, px), cy + POINT1(unity, 0.81, py),
				 cx + POINT2(unitx, 0.95, px), cy + POINT2(unity, 0.95, py));
	}
	DrawCircle(cx, cy, (int) (unitx * 0.95 + 0.5), (int) (unity * 0.95 + 0.5));
	DrawCircle(cx, cy, (int) (unitx * 0.81 + 0.5), (int) (unity * 0.81 + 0.5));

	/* Label the 36 sectors, with plus zones in red and normal in dark green. */

	k = pluszone[cSector];
	for (i = 1; i <= cSector; i++)
	{
		j = pluszone[i];
		DrawColor(j ? kRainbowB[1] : kMainB[5]);
		sprintf(sz, "%d", i);
		DrawSz(sz, cx + POINT1(unitx, 0.88, PX((double) (i * 10 + 175))) +
			   (FBetween(i, 12, 19) ? -(gi.nScale /* *gi.nScaleT */ ) : 0),
			   cy + POINT1(unity, 0.88, PY((double) (i * 10 + 175))) + (gi.nScale /* *gi.nScaleT */ ),
			   dtCent | dtScale);
		sprintf(sz, "%c", j ? '+' : '-');
		DrawSz(sz, cx + POINT1(unitx, 0.97, PX((double) (i * 10 + 175))),
			   cy + POINT1(unity, 0.97, PY((double) (i * 10 + 175))) + gi.nScaleT * 2, dtCent);
		if (j != k)
		{
			DrawColor(gi.kiGray);
			DrawDash(cx, cy,
					 cx + POINT2(unitx, 0.81, PX((double) (i * 10 + 170))),
					 cy + POINT2(unity, 0.81, PY((double) (i * 10 + 170))), 1);
		}
		k = j;
	}

	if (!gs.fBonusMode && !FCreateGrid(FALSE))
		return;
	CastSectors();				/* Go compute the planets' sector positions. */

	for (i = 0; i <= cObj; i++)	/* Figure out where to put planet glyphs. */
		symbol[i] = xplanet[i] = Mod(180.0 - cp0.longitude[i]);
	FillSymbolRing(symbol, 1.0);

	/* For each planet, draw a small dot indicating where it is, and then */
	/* a line from that point to the planet's glyph.                      */

	for (i = cObj; i >= 0; i--)
		if (FProper(i))
		{
			if (gs.fLabel)
			{
				temp = symbol[i];
				DrawColor(cp0.vel_longitude[i] < 0.0 ? gi.kiGray : gi.kiOn);
				DrawDash(cx + POINT1(unitx, 0.67, PX(xplanet[i])),
						 cy + POINT1(unity, 0.67, PY(xplanet[i])),
						 cx + POINT1(unitx, 0.71, PX(temp)),
						 cy + POINT1(unity, 0.71, PY(temp)), (cp0.vel_longitude[i] < 0.0 ? 1 : 0) - gs.fColor);
				DrawObject(i, cx + POINT1(unitx, 0.75, PX(temp)), cy + POINT1(unity, 0.75, PY(temp)));
			}
			else
				DrawColor(kObjB[i]);
			DrawPoint(cx + POINT1(unitx, 0.65, PX(xplanet[i])), cy + POINT1(unity, 0.65, PY(xplanet[i])));
		}

	/* Draw lines connecting planets which have aspects between them. */

	if (!gs.fBonusMode)
	{							/* Don't draw aspects in bonus mode. */
		for (j = cObj; j >= 1; j--)
			for (i = j - 1; i >= 0; i--)
				if (grid->n[i][j] && FProper(i) && FProper(j))
				{
					DrawColor(kAspB[grid->n[i][j]]);
					DrawDash(cx + POINT1(unitx, 0.63, PX(xplanet[i])),
							 cy + POINT1(unity, 0.63, PY(xplanet[i])),
							 cx + POINT1(unitx, 0.63, PX(xplanet[j])),
							 cy + POINT1(unity, 0.63, PY(xplanet[j])), abs(grid->v[i][j] / 60 / 2));
				}
	}
	CastChart(TRUE);			/* restore cp0 for infoborder */
	DrawInfo();
	for (i = 1; i <= NUMBER_OBJECTS; i++)
		ignore[i] = ignoreT[i];
	CastChart(TRUE);			/* restore normal restriction */
}

/* Draw an arrow from one point to another, a line with an arrowhead at the */
/* ending point. The size of the arrowhead is based on current scale size,  */
/* and the line segment is actually shorter and doesn't touch either        */
/* endpoint by the same amount. This is used by XChartDispositor() below.   */

void DrawArrow(int x1, int y1, int x2, int y2) {
	double r, s, a;

	r = Rad2Deg(Angle((double) (x2 - x1), (double) (y2 - y1)));
	s = (double) (gi.nScale * 8);
	x1 += (int) (s * RCosD(r));
	y1 += (int) (s * RSinD(r));	/* Shrink line by    */
	x2 -= (int) (s * RCosD(r));
	y2 -= (int) (s * RSinD(r));	/* the scale amount. */
	s = (double) (gi.nScale) * 4.5;
	DrawLine(x1, y1, x2, y2);	/* Main segment. */
	for (a = -1.0; a <= 1.0; a += 2.0)
		DrawLine(x2, y2, x2 + (int) (s * RCosD(r + a * 135.0)),	/* The two arrow     */
				 y2 + (int) (s * RSinD(r + a * 135.0)));	/* head line pieces. */
}

/* Draw dispositor graphs for the 10 main planets, as done when the -j is   */
/* combined with the -X switch. Four graphs are drawn, one in each screen   */
/* quadrant. A dispositor graph may be based on the sign or house position, */
/* and the planets may be arranged in a hierarchy or a wheel format.        */

void XChartDispositor()
{
	int oDis[cLastMoving + 1], dLev[cLastMoving + 1], cLev[cLastMoving + 1];
	int xo[cLastMoving + 1], yo[cLastMoving + 1];
	double xCirc[cLastMoving + 1], yCirc[cLastMoving + 1];
	char sz[cchSzDef];
	byte SaveRest[cLastMoving + 1];
	double jj;
	int xLev, yLev, xSub, ySub, cx0, cy0, cx, cy, i, j, k;
	int Rulers = 12;
	byte SaveCentr;

	for (i = 1; i <= NUMBER_OF_SIGNS - 1; i++)
		for (j = i + 1; j <= NUMBER_OF_SIGNS; j++)
			if ((rules[i] == rules[j]) && (i != j))
				Rulers -= 1;

	for (i = 1; i <= cLastMoving; i++)
		SaveRest[i] = ignore[i];
	for (i = 1; i <= cLastMoving; i++)
		if (RulerPlanet(i))
			ignore[i] = 0;
	SaveCentr = us.objCenter;
	us.objCenter = oEar;
	CastChart(TRUE);

	/* Set up screen positions of the 10 planets for the wheel graphs. */
	/* Now number of planets can be up to 12.  (V.A.)                  */

	jj = 180.0;
	cx0 = gs.xWin / 3;
	cy0 = gs.yWin / 2;
	for (i = 1; i <= cLastMoving; i++)
		if (RulerPlanet(i))
		{
			xCirc[i] = (double) cx0 *0.4 * RCosD(jj);
			yCirc[i] = (double) cy0 *0.35 * RSinD(jj);

			jj -= 360.0 / (double) Rulers;
		}

	/* Loop over the two basic dispositor types: sign based and house based. */

	for (xSub = 0; xSub <= 2; xSub++)
	{
		cx = xSub * cx0 + cx0 / 2;

		/* For each planet, get its dispositor planet for current graph type. */

		for (i = 1; i <= cLastMoving; i++)
		{
			dLev[i] = 1;
			if (xSub == 0)
				oDis[i] = rules[Z2Sign(cp0.longitude[i])];
			if (xSub == 1)
				oDis[i] = rules[cp0.house_no[i]];
			if (xSub == 2)
				oDis[i] = rules[Z2Sign(cp0.cusp_pos[cp0.house_no[i]])];
		}

		/* Determine the final dispositors (including mutual reception loops). */

		do
		{
			j = FALSE;
			for (i = 1; i <= cLastMoving; i++)
				cLev[i] = FALSE;
			for (i = 1; i <= cLastMoving; i++)
				if (dLev[i] && RulerPlanet(i))
					cLev[oDis[i]] = TRUE;
			for (i = 1; i <= cLastMoving; i++)	/* A planet isn't a final dispositor */
				if (dLev[i] && !cLev[i] && RulerPlanet(i))
				{				/* if nobody is pointing to it.      */
					dLev[i] = 0;
					j = TRUE;
				}
		}
		while (j);

		for (i = 1; i <= cLastMoving; i++)
			if (!RulerPlanet(i))
				dLev[i] = 0;

		/* Determine the level of each planet, i.e. how many times you have to */
		/* jump to your dispositor before reaching a final, with finals == 1.  */

		do
		{
			j = FALSE;
			for (i = 1; i <= cLastMoving; i++)
				if (RulerPlanet(i))
				{
					if (!dLev[i])
					{
						if (!dLev[oDis[i]])
							j = TRUE;
						else	/* If my dispositor already has */
							dLev[i] = dLev[oDis[i]] + 1;	/* a level, mine is one more.   */
					}
				}
		}
		while (j);

		/* Count the number of planets at each dispositor level. */

		for (i = 1; i <= cLastMoving; i++)
			cLev[i] = 0;
		for (i = 1; i <= cLastMoving; i++)
			cLev[dLev[i]]++;

		/* Count the number of levels total, and max planets on any one level. */

		xLev = yLev = 0;
		for (i = 1; i <= cLastMoving; i++)
			if (cLev[i])
			{
				yLev = i;
				if (cLev[i] > xLev)
					xLev = cLev[i];
			}

		/* Loop over our two dispositor display formats: hierarchy and wheel. */

		for (ySub = 0; ySub <= 1; ySub++)
		{
			cy = ySub * cy0 + cy0 / 2;
			if(xSub) {
				if(ySub) 
					strcpy(sz, Lang(sHouseDispositorWheel));
				else 
					strcpy(sz, Lang(sHouseDispositorHierarchy));
			}
			else {
				if(ySub) 
					strcpy(sz, Lang(sSignDispositorWheel));
				else 
					strcpy(sz, Lang(sSignDispositorHierarchy));
			}

			DrawColor(gi.kiLite);
			DrawSz(sz, cx, ySub * cy0 + 3 * gi.nScaleT, dtTop);

			if (ySub)
			{

				/* Draw a graph in wheel format. */

				for (i = 1; i <= cLastMoving; i++)
					if (RulerPlanet(i))
					{
						DrawObject(i, cx + (int) xCirc[i], cy + (int) yCirc[i]);
						j = oDis[i];
						if (j != i)
						{
							if (dLev[i] < 2)
								DrawColor(gi.kiOn);
							else
								DrawColor(kObjB[i]);
							DrawArrow(cx + (int) xCirc[i],
									  cy + (int) yCirc[i], cx + (int) xCirc[j], cy + (int) yCirc[j]);
						}
						if (!gs.fBonusMode && (j == i || dLev[i] < 2))
						{
							DrawColor(j == i ? gi.kiOn : gi.kiGray);
							DrawCircle(cx + (int) xCirc[i], cy + (int) yCirc[i], 7 * gi.nScale, 7 * gi.nScale);
						}
					}
			}
			else
			{

				/* For level hierarchies, first figure out the screen coordinates    */
				/* for each planet, based on its level, total levels, and max width. */

				for (i = 1; i <= cLastMoving; i++)
					if (RulerPlanet(i))
					{
						yo[i] = cy0 * (dLev[i] * 2 - 1) / (yLev * 2);
						k = 0;
						for (j = 1; j < i; j++)
							if (RulerPlanet(j))
							{
								if (dLev[i] == dLev[j])
									k = j;
							}
						if (k)
							xo[i] = xo[k] + cx0 / xLev;	/* One right of last one on level. */
						else
							xo[i] = cx - ((cx0 / xLev) * (cLev[dLev[i]] - 1) / 2);
					}

				/* Draw graph in level hierarchy format. */

				for (i = 1; i <= cLastMoving; i++)
					if (RulerPlanet(i))
					{
						DrawObject(i, xo[i], yo[i]);
						j = oDis[i];
						if (j != i)
						{
							if (dLev[i] < 2)
							{
								if (abs(xo[i] - xo[j]) < cx0 / xLev * 3 / 2)
								{
									DrawColor(gi.kiOn);
									DrawArrow(xo[i], yo[i], xo[j], yo[j]);
								}
								DrawColor(gi.kiGray);
							}
							else
							{
								DrawColor(kObjB[i]);
								DrawArrow(xo[i], yo[i], xo[j], yo[j]);
							}
						}
						else
							DrawColor(gi.kiOn);
						if (!gs.fBonusMode && dLev[i] < 2)
							DrawCircle(xo[i], yo[i], 7 * gi.nScale, 7 * gi.nScale);
					}
			}
		}
	}

	/* Draw boundary lines between the four separate dispositor graphs. */

	if (gs.fBorder)
	{
		DrawColor(gi.kiLite);
		DrawBlock(cx0, 0, cx0, gs.yWin - 1);
		DrawBlock(2 * cx0, 0, 2 * cx0, gs.yWin - 1);
		DrawBlock(0, cy0, gs.xWin, cy0);
	}
	for (i = 1; i <= cLastMoving; i++)
		ignore[i] = SaveRest[i];
	us.objCenter = SaveCentr;
	CastChart(TRUE);
}


/* Draw a graphical calendar for a given month, with numbers in boxes,  */
/* scaled to fit within the given bounds. This is used for single month */
/* -K switch images and is called 12 times for a full year -Ky image.   */

void DrawCalendar(int mon, int X1, int Y1, int X2, int Y2) {
	char sz[cchSzDef];
	int day, cday, dayHi, cweek, xunit, yunit, xs, ys, x1, y1, x, y, s;

	xs = X2 - X1;
	ys = Y2 - Y1;
	day = DayOfWeek(mon, 1, ciMain.yea);	/* Day of week of 1st of month.     */
	cday = DaysInMonth(mon, ciMain.yea);	/* Count of days in the month.      */
	dayHi = DayInMonth(mon, ciMain.yea);	/* Number of last day in the month. */
	cweek = us.fCalendarYear ? 6 : (day + cday + 6) / 7;	/* Week rows. */
	xunit = xs / 8;				/* Hor. pixel size of each day box. */
	yunit = ys / (cweek + 2);	/* Ver. pixel size of each day box. */
	x1 = X1 + (xs - xunit * 7) / 2;	/* Blank space to left of calendar. */
	y1 = Y1 + yunit * 3 / 2;	/* Blank space to top of calendar.  */

	/* Print the month and year in big letters at top of chart. */

	DrawColor(gi.kiOn);
	sprintf(sz, "%s, %d", Lang(tMonth, mon), ciMain.yea);
	s = gi.nScale;
	gi.nScale = Min((yunit * 3 / 2 - yFont * s) / yFont, xs / 9 / xFont);
	gi.nScale = Max(gi.nScale - 3, 1);
	DrawSz(sz, X1 + xs / 2, Y1 + (yunit * 3 / 2 - yFont * s) / 2, dtCent | dtScale);
	gi.nScale = s;

	/* Draw the grid of boxes for the days. */

	for (x = 0; x <= cWeek; x++)
	{

		/* Print days of week at top of each column (abbreviated if need be). */

		if (x < cWeek)
		{
			if (xunit / (xFont * gi.nScale) < 9)
				sprintf(sz, "%c%c", chDay2(x));
			else
				sprintf(sz, "%s", Lang(tDay, x));
			DrawColor(kRainbowB[3]);
			DrawSz(sz, x1 + x * xunit + xunit / 2, y1 - s * 3, dtBottom | dtScale);
			DrawColor(kRainbowB[5]);
		}
		DrawLine(x1 + x * xunit, y1, x1 + x * xunit, y1 + cweek * yunit);
	}
	for (y = 0; y <= cweek; y++)
		DrawLine(x1, y1 + y * yunit, x1 + 7 * xunit, y1 + y * yunit);

	/* Actually draw the day numbers in their appropriate boxes. */

	x = day;
	y = 0;
	for (day = 1; day <= dayHi; day = AddDay(mon, day, ciMain.yea, 1))
	{
		sprintf(sz, gs.fText ? "%2d" : "%d", day);
		DrawColor(day == ciMain.day && mon == ciMain.mon && gs.fLabel ? kRainbowB[4] :
				  (x <= 0 || x >= cWeek - 1 ? kRainbowB[1] : gi.kiLite));
		if (!gs.fBonusMode)
			DrawSz(sz, x1 + x * xunit + s * 2, y1 + y * yunit + s * 4, dtLeft | dtTop | dtScale);
		else
			DrawSz(sz, x1 + x * xunit + xunit / 2, y1 + y * yunit + yunit / 2 + gi.nScale, dtCent | dtScale);
		if (++x >= cWeek)
		{
			x = 0;
			y++;
		}
	}
}


/* Draw a graphical calendar on the screen for the chart month or entire */
/* year, as done when the -K or -Ky is combined with the -X switch.      */

void XChartCalendar()
{
	int xs, ys, xunit, yunit, x1, y1, x, y;

	if (!us.fCalendarYear)
	{
		DrawCalendar(ciMain.mon, 0, 0, gs.xWin, gs.yWin);
		return;
	}

	/* Determine the best sized rectangle of months to draw the year in based */
	/* on the chart dimensions: Either do 6x2 months, or 4x3, 3x4, or 2x6.    */

	if (gs.xWin > gs.yWin)
	{
		if (gs.xWin > gs.yWin * 3)
		{
			xs = 6;
			ys = 2;
		}
		else
		{
			xs = 4;
			ys = 3;
		}
	}
	else
	{
		if (gs.yWin > gs.xWin * 2)
		{
			xs = 2;
			ys = 6;
		}
		else
		{
			xs = 3;
			ys = 4;
		}
	}
	xunit = gs.xWin / xs;
	yunit = gs.yWin / ys;
	x1 = (gs.xWin - xunit * xs) / 2;
	y1 = (gs.yWin - yunit * ys) / 2;
	for (y = 0; y < ys; y++)
		for (x = 0; x < xs; x++)
		{
			DrawCalendar(y * xs + x + 1, x1 + x * xunit, y1 + y * yunit, x1 + (x + 1) * xunit, y1 + (y + 1) * yunit);
		}
}

/* xcharts1.c */
