/*
** Astrolog File: intrpret.c
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
#define FProper2I(i) \
(!(us.nRel == rcProgress ? ignore3[i] : (us.nRel == rcTransit ? ignore2[i] : ignore[i])))


#ifdef INTERPRET
/*
******************************************************************************
** Interpretation Routines.
******************************************************************************
*/

/* This function is used by the interpretation routines to print out lines  */
/* of text with newlines inserted just before the end of screen is reached. */

void FieldWord(char *sz) {
	static char line[cchSzMax];
	static int cursor = 0;
	int i, j;

	/* Hack: Dump buffer if function called with a null string. */

	if (sz == NULL)
	{
		line[cursor] = 0;
		PrintSz(line);
		PrintL();
		cursor = 0;
		return;
	}
	if (cursor)
		line[cursor++] = ' ';
	for (i = 0; (line[cursor] = sz[i]); i++, cursor++)
		;

	/* When buffer overflows 'n' columns, display one line and start over. */

	while (cursor >= us.nScreenWidth - 1)
	{
		for (i = us.nScreenWidth - 1; line[i] != ' '; i--)
			;
		line[i] = 0;
		PrintSz(line);
		PrintL();
		line[0] = line[1] = ' ';
		for (j = 2; (line[j] = line[i + j - 1]) != 0; j++)
			;
		cursor -= (i - 1);
	}
}


/* Display a general interpretation of what each sign of the zodiac, house, */
/* and planet or object means. This is called to do the -I0 switch table.   */

void InterpretGeneral()
{
	char sz[cchSzDef * 2];
	int i;

	PrintSz("Signs of the zodiac represent psychological characteristics.\n\n");
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		AnsiColor(kSignA(i));
		sprintf(sz, "%s is", Lang(tSignName, i));
		FieldWord(sz);
		sprintf(sz, "%s, and", szDesc[i]);
		FieldWord(sz);
		sprintf(sz, "%s.", szDesire[i]);
		FieldWord(sz);
		FieldWord(NULL);
	}
	AnsiColor(kDefault);
	PrintSz("\nHouses represent different areas within one's life.\n\n");
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		AnsiColor(kSignA(i));
		sprintf(sz, "The %d%s House is the area of life dealing with", i, Lang(tSuffix, i));
		FieldWord(sz);
		sprintf(sz, "%s.", szLifeArea[i]);
		FieldWord(sz);
		FieldWord(NULL);
	}
	AnsiColor(kDefault);
	PrintSz("\nPlanets represent various parts of one's mind or self.\n\n");
	for (i = 1; i <= cLastMoving; i++)
	{
		if (ignore[i] || FCusp(i))
			continue;
		AnsiColor(kObjA[i]);
		if (i <= oMoo || (FBetween(i, oNoNode, oCore)) && (i != oLil))
			FieldWord("The");
		sprintf(sz, "%s represents one's", Lang(tObjName, i));
		FieldWord(sz);
		sprintf(sz, "%s.", szMindPart[i]);
		FieldWord(sz);
		FieldWord(NULL);
	}
	AnsiColor(kDefault);
}


/* Display a general interpretation of what each aspect type means. This */
/* is called when printing the interpretation table in the -I0 switch.   */

void InterpretAspectGeneral()
{
	char sz[cchSzDef * 2];
	int i;

	PrintSz("\nAspects are different relationships between planets.\n\n");
	for (i = 1; i <= cAspectInt; i++)
	{
		if(!ignoreA[i])
		{
			AnsiColor(kAspA[i]);
			sprintf(sz, "When planets are %s, one", Lang(tAspectName, i));
			FieldWord(sz);
			sprintf(sz, szInteract[i], "");
			FieldWord(sz);
			FieldWord("another.");
			if (szTherefore[i][0])
			{
				sprintf(sz, "%s.", szTherefore[i]);
				FieldWord(sz);
			}
			FieldWord(NULL);
		}
	}
	return;
}


/* Print the interpretation of each planet in sign and house, as specified */
/* with the -I switch. This is basically array accessing combining the     */
/* meanings of each planet, sign, and house, and a couple of other things. */

void InterpretLocation()
{
	char sz[cchSzDef * 2], c;
	int i, j;

	PrintL();
	for (i = 1; i <= cLastMoving; i++)
	{
		if (ignore[i] || FCusp(i))
			continue;
		AnsiColor(kObjA[i]);
		j = Z2Sign(cp0.longitude[i]);
		c = Dignify(i, j);
		sprintf(sz, "%s%s in %s", cp0.vel_longitude[i] < 0.0 ? "Retrograde " : "",
				Lang(tObjName, i), Lang(tSignName, j));
		FieldWord(sz);
		sprintf(sz, "and %d%s House:", cp0.house_no[i], Lang(tSuffix, cp0.house_no[i]));
		FieldWord(sz);
		sprintf(sz, "%s's", szPerson);
		FieldWord(sz);
		FieldWord(szMindPart[i]);
		FieldWord("is");
		if (((int) cp0.longitude[i]) % 30 < 10)
			FieldWord("very");
		sprintf(sz, "%s, and", szDesc[j]);
		FieldWord(sz);
		sprintf(sz, "%s.", szDesire[j]);
		FieldWord(sz);
		FieldWord("Most often this manifests");
		if (cp0.vel_longitude[i] < 0.0 && i != oNoNode)
			FieldWord("in an independent, backward, introverted manner, and");
		FieldWord("in the area of life dealing with");
		sprintf(sz, "%s.", szLifeArea[cp0.house_no[i]]);
		FieldWord(sz);

		/* Extra information if planet is in its ruling, falling, etc, sign. */

		if (c == 'R')
			FieldWord("This is a major aspect of their psyche!");
		else if (c == 'F')
			FieldWord("(This bit plays only a minor part in their psyche.)");
		else if (c == 'e')
			FieldWord("It is easy for them to express this part of themself.");
		else if (c == 'd')
			FieldWord("It is difficult for them to express this part of themself.");
		FieldWord(NULL);
	}
}


/* Print an interpretation for a particular aspect in effect in a chart. */
/* This is called from the InterpretGrid and ChartAspect routines.       */

void InterpretAspect(int x, int y) {
	char sz[cchSzDef * 2];
	int n;

	n = grid->n[x][y];
	if (n < 1 || n > cAspectInt || FCusp(x) || FCusp(y) || x > cLastMoving || y > cLastMoving)
		return;
	AnsiColor(kAspA[n]);
	sprintf(sz, "%s %s %s: %s's", Lang(tObjName, x), Lang(tAspectName, n), Lang(tObjName, y), szPerson);
	FieldWord(sz);
	FieldWord(szMindPart[x]);
	sprintf(sz, szInteract[n], szModify[Min(abs(grid->v[x][y]) / 150, 2)][n - 1]);
	FieldWord(sz);
	sprintf(sz, "their %s.", szMindPart[y]);
	FieldWord(sz);
	if (szTherefore[n][0])
	{
		sprintf(sz, "%s.", szTherefore[n]);
		FieldWord(sz);
	}
	FieldWord(NULL);
}


/* Print the interpretation of each aspect in the aspect grid, as specified */
/* with the -g -I switch. Again, this is done by basically array accessing  */
/* of the meanings of the two planets in aspect and of the aspect itself.   */

void InterpretGrid()
{
	int i, j;

	for (i = 1; i < cLastMoving; i++)
		if (!ignore[i] && !FCusp(i))
			for (j = i + 1; j <= cLastMoving; j++)
				if (!ignore[j] && !FCusp(i))
					InterpretAspect(i, j);
}


/* Print an interpretation for a particular midpoint in effect in a chart. */
/* This is called from the ChartMidpoint routine.                          */

void InterpretMidpoint(int x, int y) {
	char sz[cchSzDef * 2];
	int n, i;

	if (FCusp(x) || FCusp(y) || x > cLastMoving || y > cLastMoving)
		return;
	n = grid->n[y][x];
	AnsiColor(kSignA(n));
	sprintf(sz, "%s midpoint %s in %s: The merging of %s's", Lang(tObjName, x), 
		Lang(tObjName, y), Lang(tSignName, n), szPerson0);
	FieldWord(sz);
	FieldWord(szMindPart[x]);
	FieldWord("with their");
	FieldWord(szMindPart[y]);
	FieldWord("is");
	if (grid->v[y][x] / 60 < 10)
		FieldWord("very");
	sprintf(sz, "%s, and", szDesc[n]);
	FieldWord(sz);
	sprintf(sz, "%s.", szDesire[n]);
	FieldWord(sz);
	FieldWord("Most often this manifests in");
	if (cp0.vel_longitude[x] + cp0.vel_longitude[y] < 0.0 && x != oNoNode && y != oNoNode)
		FieldWord("an independent, backward, introverted manner, and");
	FieldWord("the area of life dealing with");
	i = HousePlaceIn(Sign2Z(n) + (double) grid->v[y][x] / 60.0);
	sprintf(sz, "%s.", szLifeArea[i]);
	FieldWord(sz);
	FieldWord(NULL);
}


/* This is a subprocedure of ChartInDaySearch(). Print the interpretation    */
/* for a particular instance of the various exciting events that can happen. */

void InterpretInDay(int source, int aspect, int dest) {
	char sz[cchSzDef * 2];

	if (source > cLastMoving || dest > cLastMoving)
		return;

	/* Interpret object changing direction. */

	if (aspect == aDir)
	{
		AnsiColor(kObjA[source]);
		FieldWord("Energy representing");
		FieldWord(szMindPart[source]);
		FieldWord("will tend to manifest in");
		FieldWord(dest ? "an independent, backward, introverted" : "the standard, direct, open");
		FieldWord("manner.");
		FieldWord(NULL);

		/* Interpret object entering new sign. */

	}
	else if (aspect == aSig)
	{
		AnsiColor(kObjA[source]);
		FieldWord("Energy representing");
		FieldWord(szMindPart[source]);
		sprintf(sz, "will be %s,", szDesc[dest]);
		FieldWord(sz);
		sprintf(sz, "and it %s.", szDesire[dest]);
		FieldWord(sz);
		FieldWord(NULL);

		/* Interpret aspect between transiting planets. */

	}
	else if (aspect > 0 && aspect <= cAspectInt)
	{
		AnsiColor(kAspA[aspect]);
		FieldWord("Energy representing");
		FieldWord(szMindPart[source]);
		sprintf(sz, szInteract[aspect], szModify[1][aspect - 1]);
		FieldWord(sz);
		sprintf(sz, "energies of %s.", szMindPart[dest]);
		FieldWord(sz);
		if (szTherefore[aspect][0])
		{
			if (aspect > aCon)
			{
				sprintf(sz, "%s.", szTherefore[aspect]);
				FieldWord(sz);
			}
			else
				FieldWord("They will affect each other prominently.");
		}
		FieldWord(NULL);
	}
}


/* This is a subprocedure of ChartTransitSearch(). Print the interpretation */
/* for a particular transit of a planet to a natal object of a chart.       */

void InterpretTransit(int source, int aspect, int dest) {
	char sz[cchSzDef * 2];

	if (source <= oCore && dest <= oCore && aspect <= cAspectInt)
	{
		AnsiColor(kAspA[aspect]);
		FieldWord("Energy representing");
		FieldWord(szMindPart[source]);
		sprintf(sz, szInteract[aspect], szModify[1][aspect - 1]);
		FieldWord(sz);
		if (source != dest)
		{
			sprintf(sz, "%s's %s.", szPerson0, szMindPart[dest]);
		}
		else
		{
			sprintf(sz, "the same aspect inside %s's makeup.", szPerson0);
		}
		FieldWord(sz);
		if (szTherefore[aspect][0])
		{
			if (aspect > aCon)
			{
				sprintf(sz, "%s.", szTherefore[aspect]);
				FieldWord(sz);
			}
			else
				FieldWord("This part of their psyche will be strongly influenced.");
		}
		FieldWord(NULL);
	}
}


/* Print the interpretation of one person's planet in another's sign and    */
/* house, in a synastry chart as specified with the -r switch combined with */
/* -I. This is very similar to the interpretation of the standard -v chart  */
/* in InterpretLocation(), but we treat the chart as a relationship here.   */

void InterpretSynastry()
{
	char sz[cchSzDef * 2], c;
	int i, j;

	PrintL();
	for (i = 1; i <= cLastMoving; i++)
	{
		if (ignore[i] || FCusp(i))
			continue;
		AnsiColor(kObjA[i]);
		j = Z2Sign(cp0.longitude[i]);
		c = Dignify(i, j);
		sprintf(sz, "%s%s in %s,", cp0.vel_longitude[i] < 0.0 ? "Retrograde " : "",
				Lang(tObjName, i), Lang(tSignName, j));
		FieldWord(sz);
		sprintf(sz, "in their %d%s House:", cp0.house_no[i], Lang(tSuffix, cp0.house_no[i]));
		FieldWord(sz);
		sprintf(sz, "%s's", szPerson2);
		FieldWord(sz);
		FieldWord(szMindPart[i]);
		FieldWord("is");
		if (((int) cp0.longitude[i]) % 30 < 10)
			FieldWord("very");
		sprintf(sz, "%s, and", szDesc[j]);
		FieldWord(sz);
		sprintf(sz, "%s.", szDesire[j]);
		FieldWord(sz);
		FieldWord("This");
		if (cp0.vel_longitude[i] < 0.0 && i != oNoNode)
			FieldWord("manifests in an independent, backward, introverted manner, and");
		sprintf(sz, "affects %s in the area of life dealing with %s.", szPerson1, szLifeArea[cp0.house_no[i]]);
		FieldWord(sz);

		/* Extra information if planet is in its ruling, falling, etc, sign. */

		if (c == 'R')
		{
			sprintf(sz, "This is a major aspect of %s's psyche!", szPerson2);
			FieldWord(sz);
		}
		else if (c == 'F')
		{
			sprintf(sz, "(This bit plays only a minor part in %s's psyche.)", szPerson2);
			FieldWord(sz);
		}
		else if (c == 'e')
		{
			sprintf(sz, "%s is affected harmoniously in this way.", szPerson1);
			FieldWord(sz);
		}
		else if (c == 'd')
		{
			sprintf(sz, "%s is affected discordantly in this way.", szPerson1);
			FieldWord(sz);
		}
		FieldWord(NULL);
	}
}


/* Print an interpretation for a particular aspect in effect in a comparison */
/* relationship chart. This is called from the InterpretGridRelation and     */
/* the ChartAspectRelation routines.                                         */

void InterpretAspectRelation(int x, int y) {
	char sz[cchSzDef * 2];
	int n;

	n = grid->n[y][x];
	if (n < 1 || n > cAspectInt || FCusp(x) || FCusp(y) || x > cLastMoving || y > cLastMoving)
		return;
	AnsiColor(kAspA[n]);
	sprintf(sz, "%s %s %s: %s's", Lang(tObjName, x), Lang(tAspectName, n), Lang(tObjName, y), szPerson1);
	FieldWord(sz);
	FieldWord(szMindPart[x]);
	sprintf(sz, szInteract[n], szModify[Min(abs(grid->v[y][x]) / 150, 2)][n - 1]);
	FieldWord(sz);
	sprintf(sz, "%s's %s.", szPerson2, szMindPart[y]);
	FieldWord(sz);
	if (szTherefore[n][0])
	{
		if (n != 1)
		{
			sprintf(sz, "%s.", szTherefore[n]);
			FieldWord(sz);
		}
		else
			FieldWord("These parts affect each other prominently.");
	}
	FieldWord(NULL);
}


/* Print the interpretation of each aspect in the relationship aspect grid, */
/* as specified with the -r0 -g -I switch combination.                      */

void InterpretGridRelation()
{
	int i, j;

	for (i = 1; i <= cLastMoving; i++)
		if (!ignore[i])
			for (j = 1; j <= cLastMoving; j++)
				if (!ignore[j])
					InterpretAspectRelation(i, j);
}


/* Print the interpretation of a midpoint in the relationship grid, as */
/* specified with the -r0 -m -I switch combination.                    */

void InterpretMidpointRelation(int x, int y) {
	char sz[cchSzDef * 2];
	int n;

	if (FCusp(x) || FCusp(y) || x > cLastMoving || y > cLastMoving)
		return;
	n = grid->n[y][x];
	AnsiColor(kSignA(n));
	sprintf(sz, "%s midpoint %s in %s: The merging of %s's", Lang(tObjName, x), Lang(tObjName, y), Lang(tSignName, n), szPerson1);
	FieldWord(sz);
	FieldWord(szMindPart[x]);
	sprintf(sz, "with %s's", szPerson2);
	FieldWord(sz);
	FieldWord(szMindPart[y]);
	FieldWord("is");
	if (grid->v[y][x] / 60 < 10)
		FieldWord("very");
	sprintf(sz, "%s, and", szDesc[n]);
	FieldWord(sz);
	sprintf(sz, "%s.", szDesire[n]);
	FieldWord(sz);
	if (cp1.vel_longitude[x] + cp2.vel_longitude[y] < 0.0 && x != oNoNode && y != oNoNode)
	{
		FieldWord("Most often this manifests in");
		FieldWord("an independent, backward, introverted manner.");
	}
	FieldWord(NULL);
}
#endif /* INTERPRET */


/*
******************************************************************************
** Chart Influence Routines.
******************************************************************************
*/

/* This is a subprocedure of ChartInfluence(). Based on the values in the */
/* array parameter 'value', store numbers in array 'rank' reflecting the  */
/* relative order, e.g. value[x] 2nd greatest array value -> rank[x] = 2. */

void SortRank(double *value, int *rank, int size) {
	int h, i, j, k;

	value[0] = -1000.0;
	for (i = 1; i <= size; i++)
		rank[i] = -1;
	for (h = 1, i = 0; h <= size; h++)
	{
		if (size != NUMBER_OF_SIGNS && (!FProper2(h)))
			continue;
		i++;
		k = 0;
		for (j = 1; j <= size; j++)
		{
			if (size != NUMBER_OF_SIGNS && (!FProper2(j)))
				continue;
			if (value[j] > value[k] && rank[j] < 0)
				k = j;
		}

		/* 'k' is the current position of the 'i'th place planet. */

		rank[k] = i;
	}
}


/* This is a subprocedure of ChartInfluence(). Based on the values in the */
/* array parameter 'value', store numbers in array 'rank' reflecting the  */
/* relative order, e.g. value[x] 2nd greatest array value -> rank[x] = 2. */
/* But this time central object is other than Earth, so value[0] and      */
/* rank[0] represents Earth, and we have to swap these values temporarily */
/* with free place of central planet */

void SortRank2(double *value, int *rank, int size) {
	int h, i, j, k;
	double tempval;

	tempval = value[us.objCenter];
	value[us.objCenter] = value[0];
	value[0] = -1000.0;
	for (i = 1; i <= size; i++)
		rank[i] = -1;
	for (h = 1, i = 0; h <= size; h++)
	{
		if (size != NUMBER_OF_SIGNS && (!FProper2I(h)))
			continue;
		i++;
		k = 0;
		for (j = 1; j <= size; j++)
		{
			if (size != NUMBER_OF_SIGNS && (!FProper2I(j)))
				continue;
			if (value[j] > value[k] && rank[j] < 0)
				k = j;
		}

		/* 'k' is the current position of the 'i'th place planet. */

		rank[k] = i;
	}
	rank[0] = rank[us.objCenter];
	value[0] = value[us.objCenter];
	value[us.objCenter] = tempval;
}


/* Calculate position part of planet powers. Used by calculation of total */
/* planet powers, also by calculating of aspects powers.                  */

void PlanetPPower()
{
	double x, Hy;
	int xSub, i, j, l, Rulers = 12;
	int oDis[cLastMoving + 1], dLev[cLastMoving + 1], cLev[cLastMoving + 1];
	int dLevH[cLastMoving + 1], dLevS[cLastMoving + 1], dLevHd[cLastMoving + 1];
	byte SaveRest[cLastMoving + 1], SaveStar, c;
	CP cpT;


	cpT = cp0;

	for (i = 1; i <= NUMBER_OF_SIGNS - 1; i++)
		for (j = i + 1; j <= NUMBER_OF_SIGNS; j++)
			if ((rules[i] == rules[j]) && (i != j))
				Rulers -= 1;

	for (i = 0; i <= cLastMoving; i++)
		SaveRest[i] = ignore[i];
	for (i = 0; i <= cLastMoving; i++)
		if (RulerPlanet(i))
			ignore[i] = 0;

	SaveStar = us.nStar;
	us.nStar = FALSE;
	CastChart(TRUE);

	if (us.nHouseSystem != cHouseNull)
		Hy = 1.0;
	else
		Hy = 0.0;

	ppower1[0] = rObjInf[0];
	for (i = 0; i <= cLastMoving; i++)
	{
		if (ignore[i] || !FThing(i))
			ppower1[i] = rObjInf[i];
		else
			ppower1[i] = 0.0;
	}
	for (i = 0; i <= cLastMoving; i++)
	{
		dLevS[i] = 0;
		dLevH[i] = 0;
		dLevHd[i] = 0;
	}

	/* Looking for dispositors, look xcharts1.c, XChartDispositor  */

	for (xSub = 0; xSub <= 2; xSub++)
	{
		for (i = 0; i <= cLastMoving; i++)
		{
			dLev[i] = 1;
			if (xSub == 0)
				oDis[i] = rules[Z2Sign(cp0.longitude[i])];
			if (xSub == 1)
				oDis[i] = rules[cp0.house_no[i]];
			if (xSub == 2)
				oDis[i] = rules[Z2Sign(cp0.cusp_pos[cp0.house_no[i]])];
		}
		do
		{
			j = FALSE;
			for (i = 0; i <= cLastMoving; i++)
				cLev[i] = FALSE;
			for (i = 0; i <= cLastMoving; i++)
				if (dLev[i] && RulerPlanet(i))
					cLev[oDis[i]] = TRUE;
			for (i = 0; i <= cLastMoving; i++)	/* A planet isn't a final dispositor */
				if (dLev[i] && !cLev[i] && RulerPlanet(i))
				{				/* if nobody is pointing to it.      */
					dLev[i] = 0;
					j = TRUE;
				}
		}
		while (j);

		for (i = 0; i <= cLastMoving; i++)
			if (!RulerPlanet(i))
				dLev[i] = 0;

		do
		{
			j = FALSE;
			for (i = 0; i <= cLastMoving; i++)
			{
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
		}
		while (j);

		if (xSub == 0)
			for (i = 0; i <= cLastMoving; i++)
				dLevS[i] = dLev[i];	/* Store sign dispositors info */
		if (xSub == 1)
			for (i = 0; i <= cLastMoving; i++)
				dLevH[i] = dLev[i];	/* Store house dispositors info */
		if (xSub == 2)
			for (i = 0; i <= cLastMoving; i++)
				dLevHd[i] = dLev[i];	/* Store house cusp dispositor info */
	}

	for (i = 0; i <= cLastMoving; i++)
		ignore[i] = SaveRest[i];
	us.nStar = SaveStar;
	cp0 = cpT;

	/* For each object, find its power based on its placement alone. */

	for (i = 0; i <= cLastMoving; i++)
	{
		if (!ignore[i] && FThing(i) && i != us.objCenter)
		{
			j = Z2Sign(cp0.longitude[i]);
			ppower1[i] += rObjInf[i];	/* Influence of planet itself. */
			ppower1[i] += rHouseInf[cp0.house_no[i]];	/* Influence of house it's in. */

			c = Dignify(i, j);
			switch (c)
			{
			case 'R':
				x = rObjInf[cLastMoving + 1];
				break;			/* Planets in signs they rule or */
			case 'e':
				x = rObjInf[cLastMoving + 2];
				break;			/* exalted in have influence.    */
			case 'F':
				x = -rObjInf[i] / 3.0;
				break;
			case 'd':
				x = -rObjInf[i] / 4.0;
				break;
			default:
				x = 0.0;
			}

			c = Dignify(i, cp0.house_no[i]);
			switch (c)
			{
			case 'R':
				x += rHouseInf[NUMBER_OF_SIGNS + 1] / 2.0;
				break;			/* Item in house aligned with */
			case 'e':
				x += rHouseInf[NUMBER_OF_SIGNS + 2] / 2.0;
				break;			/* sign ruled has influence. */
			case 'F':
				x -= rObjInf[i] / 6.0;
				break;
			case 'd':
				x -= rObjInf[i] / 8.0;
				break;
			default:;
			}

			/*  Same additions by situative houses.  */

			c = Dignify(i, Z2Sign(cp0.cusp_pos[cp0.house_no[i]]));
			switch (c)
			{
			case 'R':
				x += rHouseInf[NUMBER_OF_SIGNS + 1] / 2.0;
				break;
			case 'e':
				x += rHouseInf[NUMBER_OF_SIGNS + 2] / 2.0;
				break;
			case 'F':
				x -= rObjInf[i] / 6.0;
				break;
			case 'd':
				x -= rObjInf[i] / 8.0;
				break;
			default:;
			}
			ppower1[i] += x;

			/* Sign ruler has influence, if planet is in this sign.             */
			/* Addition depends of ruling level (see graphics influence chart). */

			if (i != rules[j])
			{
				if (RulerPlanet(i))
					ppower1[rules[j]] += rObjInf[i] / (5.0 * sqrt((double) dLevS[rules[j]]));
				else
					ppower1[rules[j]] += rObjInf[i] / 5.0;
			}

			/* House cusp dispositor has influence, if planet is in this house. */
			/* Addition depends of ruling level (see graphics influence chart). */
			/* Influence has both natural and situative rulers of house.        */

			if (i != (j = rules[cp0.house_no[i]]))
			{
				if (RulerPlanet(i))
					ppower1[j] += rObjInf[i] / (10.0 * sqrt((double) dLevH[j]));
				else
					ppower1[j] += rObjInf[i] / 10.0;
			}

			l = Z2Sign(cp0.cusp_pos[cp0.house_no[i]]);
			if (i != rules[l])
			{
				if (RulerPlanet(i))
					ppower1[rules[l]] += Hy * rObjInf[i] / (10.0 * sqrt((double) dLevHd[rules[l]]));
				else
					ppower1[rules[l]] += Hy * rObjInf[i] / 10.0;
			}
		}
	}

	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{							/* Various planets get influence */
		j = Z2Sign(cp0.cusp_pos[i]);	/* if house cusps fall in signs  */
		ppower1[rules[j]] += rHouseInf[i] / 2.0;	/* they rule.                    */
	}

	/* And now, when all things have calculated positional powers, give  */
	/* additions to house cusps (as objects), if there are something in  */
	/* this house. */

	for (i = 0; i <= cLastMoving; i++)
		if (!ignore[i] && !FCusp(i) && i != us.objCenter)
			ppower1[cp0.house_no[i] + cuspLo - 1] += ppower1[i] / 10.0;
}


/* Print out a list of power values and relative rankings, based on the */
/* placements of the planets, and their aspects in the aspect grid, as  */
/* specified with the -j "find influences" switch.                      */

void ChartInfluence()
{
	double power[cLastMoving + 1], power1[cLastMoving + 1], power2[cLastMoving + 1];
	double power3[cLastMoving + 1], Harmon[cLastMoving + 1], Tension[cLastMoving + 1];
	double AspectSum[cLastMoving + 1], Conjunct[cLastMoving + 1], total, total1, total2;
	double total3, total4, total5, valt, m, ll, lll, ip, jp;
	int rank[cLastMoving + 1], rank1[cLastMoving + 1], rank2[cLastMoving + 1];
	int rank3[cLastMoving + 1], i, j, k, l, jo, kk, ssmart;
	byte saveflag;
	char sz[cchSzDef];

	PrintHeader();
	PrintL();					/* Display chart info */

	if (us.nRel == rcDual || us.nRel == rcTransit || us.nRel == rcProgress)
		goto DualInfluence;

	saveflag = us.fParallel;
	us.fParallel = 0;

	for (i = 0; i <= cLastMoving; i++)
		power2[i] = power3[i] = 0.0;
	total = total1 = total2 = total3 = 0.0;

	PlanetPPower();

	for (i = 0; i <= cLastMoving; i++)
	{
		power1[i] = ppower1[i];
		ppowerTotal[i] = ppower1[i];
	}

	/* Second, for each object, find its power based on aspects it makes. */

	if (!FCreateGrid(FALSE))
		return;
	for (j = 0; j <= cLastMoving; j++)
		if (!ignore[j] && j != us.objCenter)
			for (i = 0; i <= cLastMoving; i++)
				if (!ignore[i] && i != j && i != us.objCenter)
				{
					k = grid->n[Min(i, j)][Max(i, j)];
					if (FCusp(i) && FCusp(j))
						continue;
					if (k)
					{
						l = grid->v[Min(i, j)][Max(i, j)];
						power2[j] +=
							rAspInf[k] * (sqrt) (power1[i] * power1[j]) *
							0.5 * (1.0 - fabs((double) l) / 60.0 / GetOrb(i, j, k));

						/* Even in case of SmartCusps, give additional power to opposite */
						/* cusp. But planet has influence only from one cusp!!!          */

						if (us.fSmartAspects && FCusp(j))
						{
							us.fSmartAspects = FALSE;
							if (j >= oAsc && j < oDes)
								jo = j + 6;
							else
								jo = j - 6;

							lll = MinDistance(cp0.longitude[i], cp0.longitude[jo]);
							for (kk = cAspect; kk >= 1; kk--)
							{
								if (!FAcceptAspect(i, kk, jo))
									continue;
								m = lll - rAspAngle[kk];
								if (fabs(m) < GetOrb(i, jo, kk))
								{
									power2[jo] +=
										rAspInf[kk] * (sqrt) (power1[i] * power1[jo]) *
										0.5 * (1.0 - fabs((double) l) / 60.0 / GetOrb(i, jo, kk));
								}
							}
							us.fSmartAspects = TRUE;
						}
					}
				}

	/* Third, for each object, find its power based on parallel and  */
	/* and contraparallel aspects it makes                           */

	us.fParallel = 1;
	if (!FCreateGrid(FALSE))
		return;

	for (j = 0; j <= oCore; j++)
		if (!ignore[j] && FThing(j) && j != us.objCenter)
			for (i = 0; i <= oCore; i++)
				if (!ignore[i] && FThing(i) && i != j && i != us.objCenter)
				{
					k = grid->n[Min(i, j)][Max(i, j)];
					if (k)
					{
						l = grid->v[Min(i, j)][Max(i, j)];
						power3[j] +=
							rAspInf[k] * (sqrt) (power1[i] * power1[j]) *
							0.05 * (1.0 - fabs((double) l) / 60.0 / GetOrb(i, j, k));
					}
				}

	us.fParallel = saveflag;

	/* Calculate total power of each planet. */

	for (i = 0; i <= cLastMoving; i++)
	{
		power[i] = power1[i] + power2[i] + power3[i];
		ppowerTotal[i] = power[i];
		if (!ignore[i])
		{
			total1 += power1[i];
			total2 += power2[i];
			total3 += power3[i];
		}
	}
	total = total1 + total2 + total3;

#ifdef INTERPRETALT
	if (NoPrint)
		return;
#endif

	/* Finally, determine ranks of the arrays, then print everything out. */

	if (us.objCenter)
	{
		SortRank2(power1, rank1, cLastMoving);
		SortRank2(power2, rank2, cLastMoving);
		SortRank2(power3, rank3, cLastMoving);
		SortRank2(power, rank, cLastMoving);
	}
	else
	{
		SortRank(power1, rank1, cLastMoving);
		SortRank(power2, rank2, cLastMoving);
		SortRank(power3, rank3, cLastMoving);
		SortRank(power, rank, cLastMoving);
	}

	PrintSz(Lang(sInfluenceHdr));
	for (i = 0; i <= cLastMoving; i++)
		if (!ignore[i] && i != us.objCenter)
		{
			AnsiColor(kObjA[i]);
			sprintf(sz, "%11.11s: ", Lang(tObjName, i));
			PrintSz(sz);
			sprintf(sz,
					"%6.1f (%2d) +%6.1f (%2d) +%6.1f (%2d) =%7.1f (%2d) /%6.1f%%\n",
					power1[i], rank1[i], power2[i], rank2[i], power3[i],
					rank3[i], power[i], rank[i], power[i] / total * 100.0);
			PrintSz(sz);
		}
	AnsiColor(kDefault);
	sprintf(sz, "      Total: %6.1f      +%6.1f      +%6.1f      =%7.1f      / 100.0%%\n", 
		total1, total2, total3, total);
	PrintSz(sz);

	/* Now, print out a list of power values and relative rankings, based on  */
	/* the power of each sign of the zodiac, as indicated by the placement of */
	/* the planets above, in the chart, as specified with the -j0 switch.     */

	if (!us.fInfluenceSign)
		return;

	/* Sign power is determined by power of ruler and */
	/* powers of objects in sign                      */

	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		power1[i] = power[rules[i]];

	for (i = 1; i <= cLastMoving; i++)
	{
		if (!ignore[i])
		{
			if (FThing(i))
				power1[Z2Sign(cp0.longitude[i])] += power[i];
			else
				power1[Z2Sign(cp0.longitude[i])] += power[i] / 3.0;
		}
	}

	total1 = 0.0;
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		total1 += power1[i];
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		power1[i] *= total / total1;
	total1 = total;

	/* Again, determine ranks in the array, and print everything out. */

	SortRank(power1, rank1, NUMBER_OF_SIGNS);
	PrintSz("\n       Sign:  Power Rank  Percent  -   Element  Power  Percent\n");
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		AnsiColor(kSignA(i));
		sprintf(sz, "%11.11s: ", Lang(tSignName, i));
		PrintSz(sz);
		sprintf(sz, "%6.1f (%2d) /%6.1f%%", power1[i], rank1[i], power1[i] / total1 * 100.0);
		PrintSz(sz);
		if (i <= 4)
		{
			sprintf(sz, "  -%9.7s:", Lang(tElement, i - 1));
			PrintSz(sz);
			total2 = 0.0;
			for (j = 1; j < NUMBER_OF_SIGNS; j += 4)
				total2 += power1[i - 1 + j];
			sprintf(sz, "%7.1f /%6.1f%%", total2, total2 / total1 * 100.0);
			PrintSz(sz);
		}
		else if (i == 6)
		{
			AnsiColor(kDefault);
			PrintSz("  -      Mode  Power  Percent");
		}
		else if (i >= 7 && i <= 9)
		{
			AnsiColor(kModeA(i - 7));
			sprintf(sz, "  -%9.8s:", Lang(tMode, i - 7));
			PrintSz(sz);
			total2 = 0.0;
			for (j = 1; j < NUMBER_OF_SIGNS; j += 3)
				total2 += power1[i - 7 + j];
			sprintf(sz, "%7.1f /%6.1f%%", total2, total2 / total1 * 100.0);
			PrintSz(sz);
		}
		PrintL();
	}
	AnsiColor(kDefault);
	sprintf(sz, "      Total:%7.1f      / 100.0%%\n", total1);
	PrintSz(sz);

	/* House power is determined by internal initial power and natural  */
	/* and situative rulers, also powers of objects in house and         */
	/* house cusps.                                                       */

	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		power1[i] = rHouseInf[i];
		power1[i] += power[cuspLo + i - 1];
		power1[i] += power[rules[i]] / 2.0;
		power1[i] += power[rules[Z2Sign(cp0.cusp_pos[i])]] / 2.0;
	}

	/* For each house, determine its power based on the power of the object. */

	for (i = 1; i <= cLastMoving; i++)
		if (!ignore[i] && !FCusp(i))
			power1[cp0.house_no[i]] += power[i];

	total1 = 0.0;
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		total1 += power1[i];
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		power1[i] *= total / total1;
	total1 = total;

	/* Again, determine ranks in the array, and print everything out. */

	SortRank(power1, rank1, NUMBER_OF_SIGNS);
	PrintSz("\n      House:  Power Rank  Percent  -   Element  Power  Percent\n");

	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		AnsiColor(kSignA(i));
		sprintf(sz, "%9d%s: ", i, Lang(tSuffix, i));
		PrintSz(sz);
		sprintf(sz, "%6.1f (%2d) /%6.1f%%", power1[i], rank1[i], power1[i] / total1 * 100.0);
		PrintSz(sz);
		if (i <= 4)
		{
			sprintf(sz, "  -%9.7s:", Lang(tElement, i - 1));
			PrintSz(sz);
			total2 = 0.0;
			for (j = 1; j < NUMBER_OF_SIGNS; j += 4)
				total2 += power1[i - 1 + j];
			sprintf(sz, "%7.1f /%6.1f%%", total2, total2 / total1 * 100.0);
			PrintSz(sz);
		}
		else if (i == 6)
		{
			AnsiColor(kDefault);
			PrintSz("  -      Mode  Power  Percent");
		}
		else if (i >= 7 && i <= 9)
		{
			AnsiColor(kModeA(i - 7));
			sprintf(sz, "  -%9.8s:", Lang(tMode, i - 7));
			PrintSz(sz);
			total2 = 0.0;
			for (j = 1; j < NUMBER_OF_SIGNS; j += 3)
				total2 += power1[i - 7 + j];
			sprintf(sz, "%7.1f /%6.1f%%", total2, total2 / total1 * 100.0);
			PrintSz(sz);
		}
		PrintL();
	}
	AnsiColor(kDefault);
	sprintf(sz, "      Total:%7.1f      / 100.0%%\n", total1);
	PrintSz(sz);
	PrintL();


	/* Harmonious-tension influences by planets in single chart */

	ssmart = us.fSmartAspects;
	for (i = 0; i <= cObj; i++)
		if (!ignore[i] && i != us.objCenter)
		{
			Tension[i] = Harmon[i] = power3[i] = AspectSum[i] = Conjunct[i] = 0.0;
			for (j = 0; j <= cObj; j++)
				if (!ignore[j] && i != j && j != us.objCenter)
				{
					if (FCusp(i) && FCusp(j))
						continue;
					if (FCusp(i))
						us.fSmartAspects = FALSE;
					ll = MinDistance(cp0.longitude[i], cp0.longitude[j]);
					for (k = 1; k <= cAspect; k++)
					{
						m = fabs(ll - rAspAngle[k]);
						if (m < GetOrb(i, j, k) && FAcceptAspect(i, k, j))
						{
							valt = (sqrt) (ppower1[i] * ppower1[j]) * rAspInf[k] * (1.0 - m / GetOrb(i, j, k));
							if (k == aOpp || k == aSqu || k == aSSq || k == aSes)
								Tension[i] -= valt;
							if (k == aTri || k == aSex || k == aSSx || k == aInc)
								Harmon[i] += valt;
							if (k == aCon)
								Conjunct[i] += valt;
							AspectSum[i] += valt;
						}
					}
					us.fSmartAspects = ssmart;
				}
			power3[i] = Tension[i] + Harmon[i];
		}
	total4 = total1 = total2 = total3 = total5 = 0.0;
	for (i = 0; i <= cObj; i++)
		if (!ignore[i] && i != us.objCenter)
		{
			total1 += Harmon[i];
			total2 += Tension[i];
			total3 += power3[i];
			total4 += AspectSum[i];
			total5 += Conjunct[i];
		}

	if (us.objCenter)
	{
		SortRank2(power3, rank1, cLastMoving);
		SortRank2(Conjunct, rank2, cLastMoving);
		SortRank2(AspectSum, rank, cLastMoving);
	}
	else
	{
		SortRank(power3, rank1, cLastMoving);
		SortRank(Conjunct, rank2, cLastMoving);
		SortRank(AspectSum, rank, cLastMoving);
	}

	PrintSz("     Planet:     Harm.     Tens.      Sum.           Conj.         Total\n");
	for (i = 0; i <= cLastMoving; i++)
		if (!ignore[i] && i != us.objCenter)
		{
			AnsiColor(kObjA[i]);
			sprintf(sz, "%11.11s: ", Lang(tObjName, i));
			PrintSz(sz);
			sprintf(sz,
					"  %6.1f    %6.1f    %6.1f (%2d)    %6.1f (%2d)    %6.1f (%2d)\n",
					Harmon[i], Tension[i], power3[i], rank1[i], Conjunct[i], rank2[i], AspectSum[i], rank[i]);
			PrintSz(sz);
		}
	AnsiColor(kDefault);
	sprintf(sz, "      Total:   %6.1f    %6.1f    %6.1f         %6.1f         %6.1f\n",
			total1, total2, total3, total5, total4);
	PrintSz(sz);
	return;

	/* Influences from second chart to first one for comparison charts */
	/* (dual,  natal-transit, natal-progressed                         */

  DualInfluence:

	if (!FCreateGridRelation(FALSE))
		return;
	for (i = 0; i <= cLastMoving; i++)
	{
		if (us.nRel == rcDual)
			power2[i] = ppower2[i];
		if (us.nRel == rcTransit)
			power2[i] = rTransitInf[i];
		if (us.nRel == rcProgress)
			power2[i] = ppower1[i];
	}

	for (i = 0; i <= cObj; i++)
		if (FProper2(i))
		{
			Tension[i] = Harmon[i] = power3[i] = AspectSum[i] = Conjunct[i] = 0.0;
			for (j = 0; j <= cObj; j++)
				if (FProper(j))
				{
					if (grid->n[i][j])
					{
						k = grid->n[i][j];
						ip = i <= cLastMoving ? power2[i] : 2.5;
						jp = j <= cLastMoving ? ppower1[j] : 2.5;
						valt = 2.0 * (rAspInf[k] * (sqrt) (ip * jp) *
									  (1.0 - fabs((double) (grid->v[i][j])) / 60.0 / GetOrb(i, j, k)));
						if (us.fParallel)
							valt = valt / 4.0;
						if (k == aOpp || k == aSqu || k == aSSq || k == aSes)
							Tension[i] -= valt;
						if (k == aSex || k == aTri || k == aSSx || k == aInc)
							Harmon[i] += valt;
						if (k == aCon)
							Conjunct[i] += valt;
						AspectSum[i] += valt;
					}
				}
			power3[i] = Tension[i] + Harmon[i];
		}
	total4 = total1 = total2 = total3 = total5 = 0.0;
	for (i = 0; i <= cObj; i++)
		if (FProper2(i))
		{
			total1 += Harmon[i];
			total2 += Tension[i];
			total3 += power3[i];
			total4 += AspectSum[i];
			total5 += Conjunct[i];
		}

	if (us.objCenter)
	{
		SortRank2(power3, rank1, cLastMoving);
		SortRank2(Conjunct, rank2, cLastMoving);
		SortRank2(AspectSum, rank, cLastMoving);
	}
	else
	{
		SortRank(power3, rank1, cLastMoving);
		SortRank(Conjunct, rank2, cLastMoving);
		SortRank(AspectSum, rank, cLastMoving);
	}

	PrintSz("     Planet:     Harm.     Tens.      Sum.           Conj.         Total\n");
	for (i = 0; i <= cLastMoving; i++)
		if (FProper2(i))
		{
			AnsiColor(kObjA[i]);
			sprintf(sz, "%11.11s: ", Lang(tObjName, i));
			PrintSz(sz);
			sprintf(sz,
					"  %6.1f    %6.1f    %6.1f (%2d)    %6.1f (%2d)    %6.1f (%2d)\n",
					Harmon[i], Tension[i], power3[i], rank1[i], Conjunct[i], rank2[i], AspectSum[i], rank[i]);
			PrintSz(sz);
		}
	AnsiColor(kDefault);
	sprintf(sz, "      Total:   %6.1f    %6.1f    %6.1f         %6.1f         %6.1f\n",
			total1, total2, total3, total5, total4);
	PrintSz(sz);

}


/* Look, if planet[i] is listed as ruler planet in rules[]  */
/* Used by XChartDispositor() and by PlanetPPower()         */

int RulerPlanet(int i) {
	int j, IsRuler = 0;

	for (j = 1; j <= NUMBER_OF_SIGNS; j++)
	{
		if (i == rules[j])
			IsRuler = 1;
	}
	return IsRuler;
}

/* intrpret.c */
