/*
** Astrolog File: process_switches.c
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


// process a switch of the obscure -Y type
static int process_switches_Y(int, char **, int, BOOL, BOOL, BOOL);

// process a switch dealing with the Windows features of the -W type
static int process_switches_W(int, char **, int, BOOL, BOOL, BOOL);

// process a switch dealing with the graphics features of the -X type
static int process_switches_X(int, char **, int, BOOL, BOOL, BOOL);

// process a switch dealing with the obscure graphics features of the -YX type
static int process_switches_YX(int, char **, int);

// Get a switch name string assigned to a numeric code
const char* GetSwitchName(int switchCode) {
	if(switchCode < 0 || switchCode >= scTerminator) return NULL;
	return szSwitchName[switchCode];
}

// Get numeric code of a switch assigned to a name string
int GetSwitchCode(const char* switchName){
	for(int i = 0; szSwitchName[i][0]; i++) {
		if(!_strcmpi(switchName, szSwitchName[i])) return i;
	}
	return scError;
}

/*
******************************************************************************
** Command Switch Procedures.
******************************************************************************
*/


// process a switch of the obscure -Y type
static int process_switches_Y(int argc, char** argv, int pos, BOOL fOr, BOOL fAnd, BOOL fNot)
{
	int darg = 0, i, j, k;
	double r;
	char ch1, ch2;
	OE oe;
	pbyte lpb;
	int *lpn;
	double *lpr;

#ifdef INTERPRET
	char *sz;
#endif

	ch1 = argv[0][pos + 1];
	ch2 = argv[0][pos + 2];
	switch (argv[0][pos])
	{
	case chNull:
		SwitchF(us.fSwitchRare);
		break;

	case 'Y':
		switch(ch1) {
			case 'f':
				us.fGraphics = TRUE;
				SwitchF(us.fDebugFont);
				break;
			default:
				ErrorArgc("YY?");
				return tcError;
		}
		break;

	case 'C':
		SwitchF(us.fSmartAspects);
		break;

	case 'd':
		SwitchF(us.fEuroDate);
		break;

	case 'D':
		SwitchF(fDisp);
		break;

	case 'H':
		SwitchF(PolarMCflip);
		break;

	case 'L':
		SwitchF(oscLilith);
		break;

	case 'm':
		SwitchF(MonthFormat);
		break;

	case 'n':
		SwitchF(us.fTrueNode);
		break;

	case 'N':
		// used to be SwitchF(forceSN);
		break;

	case 't':
		SwitchF(us.fEuroTime);
		break;

	case 'Z':
		SwitchF(fNESW);
		break;

	case 'Q':
		if (argc <= 1)
		{
			ErrorArgc("YQ");
			return tcError;
		}
		i = atoi(argv[1]);
		if (i < 0)
		{
			ErrorValN("YQ", i);
			return tcError;
		}
		us.nScrollRow = i;
		darg++;
		break;

	case 'o':
		// used to be SwitchF(us.fWriteOld), to write chart files in old format
		break;

	case 'c':
		SwitchF(us.fHouseAngle);
		break;

	case 'z':
		if (argc <= 1)
		{
			ErrorArgc("Yz");
			return tcError;
		}

		// This used to be the time minute addition to be used when NOW 
		// charts are off. This is not needed anymore so we will ignore
		// this setting for backward compatibility.
		darg++;
		break;

	case 'l':
		if (argc <= 1)
		{
			ErrorArgc("Yl");
			return tcError;
		}
		i = atoi(argv[1]);
		if (!FSector(i))
		{
			ErrorValN("Yl", i);
			return tcError;
		}
		SwitchF(pluszone[i]);
		darg++;
		break;

#ifdef ARABIC
	case 'P':
		if (argc <= 1)
		{
			ErrorArgc("YP");
			return tcError;
		}
		if (ch1 == 'a')
		{
			i = atoi(argv[1]);
			if (i < 0 || i > cAspect)
			{
				ErrorValN("YPa", i);
				return tcError;
			}
			PartAspects = i;
			darg++;
			break;
		}
		if (ch1 == 's')
		{
			i = atoi(argv[1]);
			if (i < 0 || i > cAspect)
			{
				ErrorValN("YPs", i);
				return tcError;
			}
			PartAspectsShow = i;
			darg++;
			break;
		}
		if (ch1 == 'p')
		{
			r = atof(argv[1]);
			if (r < 0.0 || r > 500.0)
			{
				ErrorValR("YPp", r);
				return tcError;
			}
			SolidAspect = r;
			darg++;
			break;
		}
		if (ch1 == 'o')
		{
			r = atof(argv[1]);
			if (r < 0.0 || r > 15.0)
			{
				ErrorValR("YPo", r);
				return tcError;
			}
			PartOrb = r;
			darg++;
			break;
		}
		i = atoi(argv[1]);
		if (!FBetween(i, -1, 1))
		{
			ErrorValN("YP", i);
			return tcError;
		}
		us.nArabicNight = i;
		darg++;
		break;
#endif

	case 'b':
		if (argc <= 1)
		{
			ErrorArgc("Yb");
			return tcError;
		}
		i = atoi(argv[1]);
		if (!FValidBioday(i))
		{
			ErrorValN("Yb", i);
			return tcError;
		}
		us.nBioday = i;
		darg++;
		break;

	case 'E':
		if (argc <= 17)
		{
			ErrorArgc("YE");
			return tcError;
		}
		i = NParseSz(argv[1], pmObjectEn);
		if (!FHelio(i))
		{
			ErrorValN("YE", i);
			return tcError;
		}
		oe.sma = atof(argv[2]);
		oe.ec0 = atof(argv[3]);
		oe.ec1 = atof(argv[4]);
		oe.ec2 = atof(argv[5]);
		oe.in0 = atof(argv[6]);
		oe.in1 = atof(argv[7]);
		oe.in2 = atof(argv[8]);
		oe.ap0 = atof(argv[9]);
		oe.ap1 = atof(argv[10]);
		oe.ap2 = atof(argv[11]);
		oe.an0 = atof(argv[12]);
		oe.an1 = atof(argv[13]);
		oe.an2 = atof(argv[14]);
		oe.ma0 = atof(argv[15]);
		oe.ma1 = atof(argv[16]);
		oe.ma2 = atof(argv[17]);
		rgoe[IoeFromObj(i)] = oe;
		darg += 17;
		break;

	case 'R':
		if (argc <= 2 + (ch1 == 'Z') * 2)
		{
			ErrorArgc("YR");
			return tcError;
		}
		i = NParseSz(argv[1], pmObjectEn);
		j = NParseSz(argv[2], pmObjectEn);
		if (ch1 == '0')
		{
			us.fIgnoreSign = i != 0;
			us.fIgnoreDir = j != 0;
			darg += 2;
			break;
		}
		else if (ch1 == 'Z')
		{
			ignorez[0] = i != 0;
			ignorez[1] = j != 0;
			ignorez[2] = atoi(argv[3]) != 0;
			ignorez[3] = atoi(argv[4]) != 0;
			darg += 4;
			break;
		}
		if (!FItem(i))
		{
			ErrorValN("YR", i);
			return tcError;
		}
		if (!FItem(j) || j < i)
		{
			ErrorValN("YR", j);
			return tcError;
		}
		if (argc <= 3 + j - i)
		{
			ErrorArgc("YR");
			return tcError;
		}

		if (ch1 == 'T')
		{
			lpb = ignore2;
		}
		else if (ch1 == 'P')
		{
			lpb = ignore3;
		}
		else
		{
			lpb = ignore;
		}

		for (k = i; k <= j; k++)
		{
			lpb[k] = atoi(argv[3 + k - i]) != 0;
		}
		darg += 3 + j - i;
		break;

	case 'A':
		if (argc <= 2)
		{
			ErrorArgc("YA");
			return tcError;
		}
		k = ch1 == 'm' || ch1 == 'd' ? pmObjectEn : pmAspect;
		i = NParseSz(argv[1], k);
		j = NParseSz(argv[2], k);
		k = ch1 == 'm' || ch1 == 'd' ? cObj : cAspect;
		if (!FBetween(i, 1, k))
		{
			ErrorValN("YA", i);
			return tcError;
		}
		if (!FBetween(j, 1, k) || j < i)
		{
			ErrorValN("YA", j);
			return tcError;
		}
		if (argc <= 3 + j - i)
		{
			ErrorArgc("YA");
			return tcError;
		}

		switch(ch1)
		{
		double		x;

		case 'o':
			for (k = i; k <= j; k++)
			{
				x = atof(argv[3 + k - i]);

				if (x < 0.0)
				{
					// For backward compatibility, we accept negative 
					// orbs, but then ignore the aspect, which is the  
					// old meaning. This is to ease transition to the  
					// new version.
					x = 0.0;
					ignoreA[k] = TRUE;
				}

				rAspOrb[k] = x;
			}
			break;

		case 'm':
			for (k = i; k <= j; k++)
			{
				rObjOrb[k] = atof(argv[3 + k - i]);
			}
			break;

		case 'd':
			for (k = i; k <= j; k++)
			{
				rObjAdd[k] = atof(argv[3 + k - i]);
			}
			break;

		case 'a':
			for (k = i; k <= j; k++)
			{
				rAspAngle[k] = atof(argv[3 + k - i]);
			}
			break;

		case 'R':
			for (k = i; k <= j; k++)
			{
				ignoreA[k] = atoi(argv[3 + k - i]) != 0;
			}
			break;

		default:
			ErrorArgc("YA");
			return tcError;
			break;
		}

		darg += 3 + j - i;
		break;

	case 'j':
		if (argc <= 2 + 2 * (ch1 == '0'))
		{
			ErrorArgc("Yj");
			return tcError;
		}
		if (ch1 == '0')
		{
			rObjInf[cLastMoving + 1] = atof(argv[1]);
			rObjInf[cLastMoving + 2] = atof(argv[2]);
			rHouseInf[NUMBER_OF_SIGNS + 1] = atof(argv[3]);
			rHouseInf[NUMBER_OF_SIGNS + 2] = atof(argv[4]);
			darg += 4;
			break;
		}
		k = ch1 == 'C' ? pmSignEn : (ch1 == 'A' ? pmAspect : pmObjectEn);
		i = NParseSz(argv[1], k);
		j = NParseSz(argv[2], k);
		k = ch1 == 'C' ? NUMBER_OF_SIGNS : (ch1 == 'A' ? cAspect : cObj);
		if (!FBetween(i, 1, k))
		{
			ErrorValN("Yj", i);
			return tcError;
		}
		if (!FBetween(j, 1, k) || j < i)
		{
			ErrorValN("Yj", j);
			return tcError;
		}
		if (argc <= 3 + j - i)
		{
			ErrorArgc("Yj");
			return tcError;
		}
		lpr = ch1 == 'C' ? rHouseInf : (ch1 == 'A' ? rAspInf : (ch1 == 'T' ? rTransitInf : rObjInf));
		for (k = i; k <= j; k++)
			lpr[k] = atof(argv[3 + k - i]);
		darg += 3 + j - i;
		break;

	case 'J':
		if (argc <= 3 - (ch1 == '0'))
		{
			ErrorArgc("YJ");
			return tcError;
		}
		i = NParseSz(argv[1], pmObjectEn);
		if (!FNorm(i))
		{
			ErrorValN("YJ", i);
			return tcError;
		}
		j = NParseSz(argv[2], pmSignEn);
		if (!FBetween(j, 0, NUMBER_OF_SIGNS))
		{
			ErrorValN("YJ", j);
			return tcError;
		}
		if (ch1 != '0')
		{
			k = NParseSz(argv[3], pmSignEn);
			if (!FBetween(k, 0, NUMBER_OF_SIGNS))
			{
				ErrorValN("YJ", k);
				return tcError;
			}
			ruler1[i] = j;
			ruler2[i] = k;
			if (FBetween(i, 1, cLastMainPlanet) && j != 0)
				rules[j] = i;
		}
		else
			exalt[i] = j;
		darg += 3 - (ch1 == '0');
		break;

	case 'r':
		if (argc <= 2)
		{
			ErrorArgc("Yr");
			return tcError;
		}
		i = NParseSz(argv[1], pmSignEn);
		if (!FBetween(i, 0, NUMBER_OF_SIGNS))
		{
			ErrorValN("Yr", i);
			return tcError;
		}
		j = NParseSz(argv[2], pmObjectEn);
		if (!FNorm(j))
		{
			ErrorValN("Yr", j);
			return tcError;
		}
		rules[i] = j;
		ruler1[j] = i;
		darg += 2;
		break;

	case 'q':
		if (argc <= 2)
		{
			ErrorArgc("Yq");
			return tcError;
		}
		i = NParseSz(argv[1], pmObjectEn);
		if (!FNorm(i))
		{
			ErrorValN("Yq", i);
			return tcError;
		}
		j = NParseSz(argv[2], pmColor);
		if (!FBetween(j, 0, cColor))
		{
			ErrorValN("Yq", j);
			return tcError;
		}
		kObjA1[i] = j;
		darg += 2;
		break;


#ifdef INTERPRET
	case 'I':
		if (argc <= 2)
		{
			ErrorArgc("YI");
			return tcError;
		}
		i = NParseSz(argv[1], ch1 == 'A' ? pmAspect : (ch1 == chNull ? pmObjectEn : pmSignEn));
		j = ch1 == 'A' ? cAspect : (ch1 == chNull ? cLastMoving : NUMBER_OF_SIGNS);
		if (!FBetween(i, 1, j))
		{
			ErrorValN("YI", i);
			return tcError;
		}
		if (ch1 == 'A' && ch2 == '0')
			ch1 = '0';
		sz = SzPersist(argv[2]);
		switch (ch1)
		{
		case 'A':
			szInteract[i] = sz;
			break;
		case '0':
			szTherefore[i] = sz;
			break;
		case chNull:
			szMindPart[i] = sz;
			break;
		case 'C':
			szLifeArea[i] = sz;
			break;
		case 'v':
			szDesire[i] = sz;
			break;
		default:
			szDesc[i] = sz;
		}
		darg += 2;
		break;
#endif

	case 'k':
		if (ch1 == 'c')
		{
			if (argc <= 1)
			{
				ErrorArgc("Ykc");
				return tcError;
			}
			
			i = NParseSz(argv[1], pmColor);

			if(!FValidColor(i))
			{
				ErrorValN("Ykc", i);
				return tcError;
			}

			kCornerDecoration = i;
			darg++;
			break;
		}
		if (argc <= 2 + 2 * (ch1 == 'C'))
		{
			ErrorArgc("Yk");
			return tcError;
		}
		if (ch1 == 'C')
		{
			kElemA[eFir] = NParseSz(argv[1], pmColor) & 15;
			kElemA[eEar] = NParseSz(argv[2], pmColor) & 15;
			kElemA[eAir] = NParseSz(argv[3], pmColor) & 15;
			kElemA[eWat] = NParseSz(argv[4], pmColor) & 15;
			darg += 4;
			break;
		}
		k = ch1 == 'A' ? pmAspect : 0;
		i = NParseSz(argv[1], k);
		j = NParseSz(argv[2], k);
		k = ch1 == 'A' ? cAspect : (ch1 == '0' ? 7 : 8);
		if (!FBetween(i, ch1 != chNull, k))
		{
			ErrorValN("Yk", i);
			return tcError;
		}
		if (!FBetween(j, ch1 != chNull, k) || j < i)
		{
			ErrorValN("Yk", j);
			return tcError;
		}
		if (argc <= 3 + j - i)
		{
			ErrorArgc("Yk");
			return tcError;
		}
		lpn = ch1 == 'A' ? kAspA : (ch1 == '0' ? kRainbowA : kMainA);
		for (k = i; k <= j; k++)
			lpn[k] = NParseSz(argv[3 + k - i], pmColor) & 15;
		darg += 3 + j - i;
		break;

	case '8':
		SwitchF(us.fClip80);
		break;

	case 'F':
		if (argc <= 8)
		{
			ErrorArgc("YF");
			return tcError;
		}
		i = NParseSz(argv[1], pmObjectEn);
		if (!FItem(i))
		{
			ErrorValN("YF", i);
			return tcError;
		}
		r = Mod((double) (atoi(argv[2]) + (NParseSz(argv[3], pmSignEn) - 1) * 30) + atof(argv[4]) / 60.0);
		if (!FCusp(i))
			cp0.longitude[i] = r;
		else
		{
			j = Mod12(i - (cuspLo - 1) + 6);
			if (FBetween(i, cuspLo - 1 + 4, cuspLo - 1 + 9))
			{
				cp0.cusp_pos[i - (cuspLo - 1)] = r;
				cp0.cusp_pos[j] = Mod(r + 180.0);
			}
			else
			{
				cp0.longitude[i] = r;
				cp0.longitude[cuspLo - 1 + j] = Mod(r + 180.0);
			}
		}
		j = atoi(argv[5]);
		r = (j < 0 ? -1.0 : 1.0) * ((double) abs(j) + atof(argv[6]) / 60.0);
		cp0.latitude[i] = Mod((r + 90.0) * 2.0) / 2.0 - 90.0;
		cp0.vel_longitude[i] = Deg2Rad(atof(argv[7]));
		if (i <= cLastMoving)
			SphToRec(atof(argv[8]), cp0.longitude[i], cp0.latitude[i], &spacex[i], &spacey[i], &spacez[i]);
		ciCore.mon = -1;
		darg += 8;
		break;

	case 'U':
		if (ch1 != 'a' && ch1 != 'o' && ch1 != 'b' && ch1 != 'O' && ch1 != 'P')
		{
			SwitchF(StarRest);
			break;
		}
		if (argc <= 1)
		{
			ErrorArgc("YU");
			return tcError;
		}
		if (ch1 == 'a')
		{
			i = atoi(argv[1]);
			if (i < 0 || i > cAspect)
			{
				ErrorValN("YUa", i);
				return tcError;
			}
			StarAspects = i;
			darg++;
			break;
		}
		if (ch1 == 'o')
		{
			r = atof(argv[1]);
			if (r < 0.0 || r > 15.0)
			{
				ErrorValR("YUo", r);
				return tcError;
			}
			StarOrb = r;
			darg++;
			break;
		}
		if (ch1 == 'P')
		{
			i = atoi(argv[1]);
			if (i < 0 || i > 3)
			{
				ErrorValN("YUP", i);
				return tcError;
			}
			PrimeRest = i;
			darg++;
			break;
		}
		if (ch1 == 'b')
		{
			if (ch2 == '1')
			{
				i = atoi(argv[1]);
				if (i < -2 || i > 16)
				{
					ErrorValN("YUb1", i);
					return tcError;
				}
				kStar1 = i;
				darg++;
				break;
			}
			if (ch2 == '2')
			{
				i = atoi(argv[1]);
				if (i < -2 || i > 16)
				{
					ErrorValN("YUb2", i);
					return tcError;
				}
				kStar2 = i;
				darg++;
				break;
			}
			if (ch2 == '3')
			{
				i = atoi(argv[1]);
				if (i < -2 || i > 16)
				{
					ErrorValN("YUb3", i);
					return tcError;
				}
				kStar3 = i;
				darg++;
				break;
			}
		}
		if (ch1 == 'O')
		{
			if (ch2 == '1')
			{
				r = atof(argv[1]);
				if (r < 0.0 || r > 16.0)
				{
					ErrorValR("YUO1", r);
					return tcError;
				}
				PrimeOrb1 = r;
				darg++;
				break;
			}
			if (ch2 == '2')
			{
				r = atof(argv[1]);
				if (r < 0.0 || r > 16.0)
				{
					ErrorValR("YUO2", r);
					return tcError;
				}
				PrimeOrb2 = r;
				darg++;
				break;
			}
		}

	case 'O':
		if (argc <= 1)
		{
			ErrorArgc("YO");
			return tcError;
		}
		if (ch1 == 'c')
		{
			r = atof(argv[1]);
			if (r <= 0)
			{
				ErrorValR("YOc", r);
				return tcError;
			}
			CoeffComp = r;
			darg++;
			break;
		}
		if (ch1 == 'p')
		{
			r = atof(argv[1]);
			if (r <= 0.0)
			{
				ErrorValR("YOp", r);
				return tcError;
			}
			CoeffPar = r;
			darg++;
			break;
		}
		if (ch1 == 'P')
		{
			r = atof(argv[1]);
			if (r < 0.0 || r > 100.0)
			{
				ErrorValR("YOP", r);
				return tcError;
			}
			PowerPar = r;
			darg++;
			break;
		}
		break;

	case 'S':
		if (ch1 == 'a')
		{
			SwitchF(us.fAppSep);
			break;
		}
		if (ch1 == 'j')
		{
			SwitchF(us.fInfluenceSign);
			break;
		}
		if (ch1 == 's')
		{
			SwitchF(us.fAspSummary);
			break;
		}
		if (ch1 == 'o')
		{
			SwitchF(fSortAspectsByOrbs);
			break;
		}
		if (ch1 == 'p')
		{
			SwitchF(us.fParallel);
			break;
		}
		break;

	case 'X':
		return process_switches_YX(argc, argv, pos + 1);

	default:
		ErrorSwitch(argv[0]);
		return tcError;
	}

	return darg; // 'darg' contains the value to be added to argc when we return
}


// process a switch dealing with the Windows features of the -W type
static int process_switches_W(int argc, char** argv, int pos, BOOL fOr, BOOL fAnd, BOOL fNot)
{
	int darg = 0, i;
	char sz[cchSzDef], ch1;

	ch1 = argv[0][pos + 1];
	switch (argv[0][pos])
	{
	case chNull:
		if (argc <= 1)
		{
			ErrorArgc("W");
			return tcError;
		}
		i = atoi(argv[1]);
		PostMessage(wi.hwnd, WM_COMMAND, i, 0L);
		darg++;
		break;

	case 'N':
		if (argc <= 1)
		{
			ErrorArgc("WN");
			return tcError;
		}
		i = atoi(argv[1]);
		if (!FValidTimer(i))
		{
			ErrorValN("WN", i);
			return tcError;
		}
		wi.nTimerDelay = i;
		darg++;
		break;

	case 'M':
		if (argc <= 2)
		{
			ErrorArgc("WM");
			return tcError;
		}
		i = atoi(argv[1]);
		if (!FValidMacro(i))
		{
			ErrorValN("WM", i);
			return tcError;
		}

		// macros with shortcuts are below 48, except 40 and 42 which haven't one
		if(i <= 48 && i != 40 && i != 42 && !wi.fDisableMacroShortcuts)	
		{
			sprintf(sz, "%s\t%sF%d", argv[2], 
				i <= 12 ? "" : (i <= 24 ? "Shift+" : (i <= 36 ? "Ctrl+" : "Alt+")), (i - 1) % 12 + 1);
		}
		else
		{
			strcpy(sz, argv[2]);
		}

		i--;

		ModifyMenu(wi.hmenu, (WORD) (cmdMacro01 + i), MF_BYCOMMAND | MF_STRING, 
			(WORD) (cmdMacro01 + i), sz);
		strcpy_s(szMacroCaption[i], 255, sz);
		darg += 2;
		break;

	case 'n':
		SwitchF(wi.fNoUpdate);
		break;

	default:
		ErrorSwitch(argv[0]);
		return tcError;
	}

	
	return darg; // 'darg' contains the value to be added to argc when we return
}

// process a switch dealing with the graphics features of the -X type
static int process_switches_X(int argc, char** argv, int pos, BOOL fOr, BOOL fAnd, BOOL fNot)

{
	int darg = 0, i, j;
	double rT;

	char ch1;

	ch1 = argv[0][pos + 1];
	switch (argv[0][pos])
	{
	case chNull:
		break;

	case 'b':
		if (us.fNoWrite || is.fSzInteract)
		{
			ErrorArgv("Xb");
			return tcError;
		}
		ch1 = ChCap(ch1);
		if (FValidBmpmode(ch1))
			gs.chBmpMode = ch1;
		SwitchF2(gs.fBitmap);
		gs.fPS = gs.fMeta = FALSE;
		break;

#ifdef PS
	case 'p':
		if (us.fNoWrite || is.fSzInteract)
		{
			ErrorArgv("Xp");
			return tcError;
		}
		gs.fPS = TRUE + (ch1 != '0');
		gs.fBitmap = gs.fMeta = FALSE;
		break;
#endif

#ifdef META
	case 'M':
		if (us.fNoWrite || is.fSzInteract)
		{
			ErrorArgv("XM");
			return tcError;
		}
		if (ch1 == '0')
			SwitchF(gs.fFont);
		SwitchF2(gs.fMeta);
		gs.fBitmap = gs.fPS = FALSE;
		break;
#endif

	case 'o':
		if (us.fNoWrite || is.fSzInteract)
		{
			ErrorArgv("Xo");
			return tcError;
		}
		if (argc <= 1)
		{
			ErrorArgc("Xo");
			return tcError;
		}
		if (!gs.fBitmap && !gs.fPS && !gs.fMeta)
			gs.fBitmap = TRUE;
		gi.szFileOut = SzPersist(argv[1]);
		darg++;
		break;

	case 'a':
		if (argc <= 1)
		{
			ErrorArgc("Xa");
			return tcError;
		}
		i = atoi(argv[1]);
		if (i < -1 || i > 2)
		{
			ErrorValN("Xa", i);
			return tcError;
		}
		TensChart = i;
		darg++;
		break;

	case 'm':
		SwitchF(gs.fColor);
		break;

	case 'r':
		SwitchF(gs.fInverse);
		break;

	case 'w':
		if (argc <= 1)
		{
			ErrorArgc("Xw");
			return tcError;
		}
		i = atoi(argv[1]);
		if (argc > 2 && ((j = atoi(argv[2])) || argv[2][0] == '0'))
		{
			argc--;
			argv++;
			darg++;
		}
		else
			j = i;
		if (!FValidGraphx(i))
		{
			ErrorValN("Xw", i);
			return tcError;
		}
		if (!FValidGraphy(j))
		{
			ErrorValN("Xw", j);
			return tcError;
		}
		gs.xWin = i;
		gs.yWin = j;
		darg++;
		break;

	case 's':
		if (argc <= 1)
		{
			ErrorArgc("Xs");
			return tcError;
		}
		i = atoi(argv[1]);
		if (i < 100)
			i *= 100;
		if (!FValidScale(i))
		{
			ErrorValN("Xs", i);
			return tcError;
		}
		gs.nScale = i;
		darg++;
		break;

	case 'i':
		SwitchF(gs.fBonusMode);
		break;

	case 'I':
		SwitchF(InfoFlag);
		break;

	case 't':
		SwitchF(gs.fText);
		break;

	case 'u':
		SwitchF(gs.fBorder);
		break;

	case 'l':
		SwitchF(gs.fLabel);
		break;

	case 'j':
		SwitchF(gs.fJetTrail);
		break;

	case '1':
		if (argc <= 1)
		{
			ErrorArgc("X1");
			return tcError;
		}
		i = NParseSz(argv[1], pmObjectEn);
		if (!FItem(i))
		{
			ErrorValN("X1", i);
			return tcError;
		}
		gs.objLeft = i;
		darg++;
		break;

	case '2':
		if (argc <= 1)
		{
			ErrorArgc("X2");
			return tcError;
		}
		i = NParseSz(argv[1], pmObjectEn);
		if (!FItem(i))
		{
			ErrorValN("X2", i);
			return tcError;
		}
		gs.objLeft = -i;
		darg++;
		break;

	case 'W':
		if (argc > 1 && ((i = atoi(argv[1])) || argv[1][0] == '0'))
		{
			darg++;
			if (!FValidRotation(i))
			{
				ErrorValN("XW", i);
				return tcError;
			}
			gs.nRot = i;
		}
		gi.nCurrChart = gWorldMap;
		if (ch1 == '0')
			gs.fMollewide = TRUE;
		break;

	case 'G':
		if (argc > 1 && ((i = atoi(argv[1])) || argv[1][0] == '0'))
		{
			darg++;
			if (!FValidRotation(i))
			{
				ErrorValN("XG", i);
				return tcError;
			}
			gs.nRot = i;
			if (argc > 2 && ((rT = atof(argv[2])) || argv[2][0] == '0'))
			{
				darg++;
				if (!FValidTilt(rT))
				{
					ErrorValR("XG", rT);
					return tcError;
				}
				gs.rTilt = rT;
			}
		}
		gi.nCurrChart = gGlobe;
		break;

	case 'P':
		if (argc > 1 && ((i = atoi(argv[1])) || argv[1][0] == '0'))
		{
			darg++;
			if (!FValidRotation(i))
			{
				ErrorValN("XP", i);
				return tcError;
			}
		}
		else
			i = 0;
		gs.nRot = i;
		gi.nCurrChart = gPolar;
		if (ch1 == '0')
			gs.fPrintMap = TRUE;
		break;

#ifdef CONSTEL
	case 'F':
		if (!fMap && gi.nCurrChart != gGlobe && gi.nCurrChart != gPolar)
			gi.nCurrChart = gWorldMap;
		not(gs.fConstel);
		break;
#endif

	case 'n':
		if (argc > 1 && (i = atoi(argv[1])))
			darg++;
		else
			i = 10;
		if (i < 1 || i > 10)
		{
			ErrorValN("Xn", i);
			return tcError;
		}
		gs.nAnimationJumpRate = i;
		break;

	default:
		ErrorSwitch(argv[0]);
		return tcError;
	}

	return darg; // 'darg' contains the value to be added to argc when we return
}


// process a switch dealing with the obscure graphics features of the -YX type
static int process_switches_YX(int argc, char** argv, int pos)
{
	int darg = 0, i, j, k;
	char ch1;
	double r;

#ifdef PS
	ch1 = argv[0][pos + 1];
#endif
	switch (argv[0][pos])
	{
	case chNull:
		if (argc <= 2)
		{
			ErrorArgc("YX");
			return tcError;
		}
		darg += 2;
		break;

	case 'C':
		if (argc <= 3)
		{
			ErrorArgc("YXC");
			return tcError;
		}
		i = atoi(argv[1]);
		if (i < 0 || i > 2)
		{
			ErrorValN("YXC", i);
			return tcError;
		}
		j = atoi(argv[2]);
		if (j < 1 || j > 100)
		{
			ErrorValN("YXC", j);
			return tcError;
		}
		k = atoi(argv[3]);
		if (k < 1 || k > 100)
		{
			ErrorValN("YXC", k);
			return tcError;
		}
		CornerDecorationType = i;
		CornerDecorationSize = j;
		CornerDecorationNumberLines = k;
		darg += 3;
		break;

	case 'c':
		if (argc <= 1)
		{
			ErrorArgc("YXc");
			return tcError;
		}
		r = atof(argv[1]);
		if (r < 0.2 || r > 4.0)
		{
			ErrorValR("YXc", r);
			return tcError;
		}
		CoeffTens = r;
		darg++;
		break;

	case 'G':
		if (argc <= 1)
		{
			ErrorArgc("YXG");
			return tcError;
		}
		i = atoi(argv[1]);
		if (!FValidGlyphs(i))
		{
			ErrorValN("YXg", i);
			return tcError;
		}
		gs.nGlyphs = i;
		darg++;
		break;

	case 'g':
		if (argc <= 1)
		{
			ErrorArgc("YXg");
			return tcError;
		}
		i = atoi(argv[1]);
		if (!FValidGrid(i))
		{
			ErrorValN("YXg", i);
			return tcError;
		}
		gs.nGridCell = i;
		darg++;
		break;

	case 'f':
		if (argc <= 1)
		{
			ErrorArgc("YXf");
			return tcError;
		}
		gs.fFont = atoi(argv[1]);
		darg++;
		break;

#ifdef PS
	case 'p':
		if (ch1 == '0')
		{
			if (argc <= 2)
			{
				ErrorArgc("YXp0");
				return tcError;
			}
			gs.xInch = atof(argv[1]);
			gs.yInch = atof(argv[2]);
			darg += 2;
			break;
		}
		if (argc <= 1)
		{
			ErrorArgc("YXp");
			return tcError;
		}
		gs.nOrient = atoi(argv[1]);
		darg++;
		break;
#endif

	default:
		ErrorSwitch(argv[0]);
		return tcError;
	}

	return darg; // 'darg' contains the value to be added to argc when we return
}





// process a switch with long name started by # sign
int ProcessExtendedSwitches(int argc, char** argv, int pos, BOOL fOr, BOOL fAnd, BOOL fNot) {
	int switchCode = GetSwitchCode(argv[0] + pos);
	if(scError == switchCode) {
		ErrorSwitch(argv[0]);
		return -1;
	}

	switch(switchCode) {
		case scGlyph:
		{
			if(argc < 4) {
				ErrorArgc(argv[0]);
				return -1;
			}
			int glyphSet = atoi(argv[1]);
			int glyphNum = atoi(argv[2]);
			if(glyphSet != 0) {
				ErrorValN(argv[0]+1, glyphSet);
				return -1;
			}
			if((glyphNum < 32) || (glyphNum > 256)) {
				ErrorValN(argv[0]+1, glyphNum);
				return -1;
			}
			Glyph.GetItem(glyphSet)->Set(glyphNum - 32, argv[3]);
			return 3;
			break;
		}
		case scSimplifiedMode:
		{
			us.advancedMode = FALSE;
			HMENU newMenu = LoadMenu(Lang.resource, MAKEINTRESOURCE(secondaryMenu));
			if(newMenu) {
				HMENU oldMenu = wi.hmenu;
				if(SetMenu(wi.hwnd, newMenu)) {
					DestroyMenu(oldMenu);
					wi.hmenu = newMenu;
				}
			}
			HACCEL newAccel = LoadAccelerators(Lang.resource, MAKEINTRESOURCE(secondaryAccelerator));
			if(newAccel) {
				HACCEL oldAccel = wi.haccel;
				wi.haccel = newAccel;
				DestroyAcceleratorTable(oldAccel);
			}
			return 0;
			break;
		}
		case scAdvancedMode:
		{
			us.advancedMode = TRUE;
			HMENU newMenu = LoadMenu(Lang.resource, MAKEINTRESOURCE(menu));
			if(newMenu) {
				HMENU oldMenu = wi.hmenu;
				wi.hmenu = newMenu;
				if(SetMenu(wi.hwnd, newMenu)) {
					DestroyMenu(oldMenu);
					// restore macro menu captions
					for(unsigned int i = 0; i < NUMBER_OF_MACROS; i++) {
						if(szMacro[i]) {
							ModifyMenu(newMenu, (WORD) (cmdMacro01 + i), MF_BYCOMMAND | MF_STRING, 
								(WORD) (cmdMacro01 + i), szMacroCaption[i]);
						}
					}
				}
			}
			HACCEL newAccel = LoadAccelerators(Lang.resource, MAKEINTRESOURCE(accelerator));
			if(newAccel) {
				HACCEL oldAccel = wi.haccel;
				wi.haccel = newAccel;
				DestroyAcceleratorTable(oldAccel);
			}
			return 0;
			break;
		}
		case scDebugFont:
			us.fGraphics = TRUE;
			SwitchF(us.fDebugFont);
			gi.nCurrChart = gDebugFont;
			return 0;
			break;
		
		case scCharSet:
			if(argc < 2) {
				ErrorArgc(argv[0]);
				return -1;
			}
			gs.charSet = atoi(argv[1]);
			return 1;
			break;

		case scUseInternationalAtlas:
			us.useInternationalAtlas = fNot ? (1 - us.useInternationalAtlas) : fOr;
			return 0;
			break;

		case scOmitWizard:
			us.omitWizard = TRUE;
			return 0;
			break;

		case scOmitRegionalWizard:
			us.omitRegionalWizard = TRUE;
			return 0;
			break;

		case scDegCharacter:
			if(argc < 2) {
				ErrorArgc(argv[0]);
				return -1;
			}
			Lang.SetString(sDegSign, argv[1]);
			return 1;
			break;

		case scDisableMacroShortcuts:
			wi.fDisableMacroShortcuts = true;
			return 0;
			break;

		default:
			;
	}
	
	ErrorSwitch(argv[0]);
	return -1;

}
/* Process a command switch line passed to the program. Read each entry in */
/* the argument list and set all the program modes and charts to display.  */
BOOL FProcessSwitchesMain(int argc, char **argv) {
	int ich, i, j;
	BOOL fNot, fOr, fAnd;
	double rT;
	char ch1, ch2, *pch;
	CI ci;
	double r;

	argc--;
	argv++;
	while (argc)
	{
		ch1 = argv[0][0];
		fNot = fOr = fAnd = FALSE;
		switch (ch1)
		{
		case '=':
			fOr = TRUE;
			break;
		case '_':
			fAnd = TRUE;
			break;
		case ':':
			break;
		default:
			fNot = TRUE;
			break;
		}
		ich = 1 + FChSwitch(argv[0][0]);	/* Leading dash? */
		ch1 = argv[0][ich];
		ch2 = (ch1 == chNull) ? chNull : argv[0][ich + 1];

		switch (argv[0][ich - 1])
		{

		case '#':
			i = ProcessExtendedSwitches(argc, argv, ich, fOr, fAnd, fNot);
			if (i < 0)
				return FALSE;
			argc -= i;
			argv += i;
			break;

		case 'H':
			if (ch1 == 'Y')
				SwitchF(us.fSwitchRare);
			else if (ch1 == 'X')
				SwitchF(us.fKeyGraph);
			else if (ch1 == 'C')
				SwitchF(us.fSign);
			else if (ch1 == 'O')
				SwitchF(us.fObject);
			else if (ch1 == 'A')
				SwitchF(us.fAspect);
			else if (ch1 == 'F')
				SwitchF(us.fConstel);
			else if (ch1 == 'S')
				SwitchF(us.fOrbitData);
			else if (ch1 == 'I')
				SwitchF(us.fMeaning);
			else if (ch1 == 'e')
			{
				SwitchF(us.fSwitch);
				SwitchF(us.fSwitchRare);
				SwitchF(us.fKeyGraph);
				SwitchF(us.fSign);
				SwitchF(us.fObject);
				SwitchF(us.fAspect);
				SwitchF(us.fConstel);
				SwitchF(us.fOrbitData);
				SwitchF(us.fMeaning);
			}
			else
				SwitchF(us.fSwitch);
			break;

		case 'M':
			i = (ch1 == '0');
			if (argc <= 1 + i)
			{
				ErrorArgc("M");
				return FALSE;
			}
			j = atoi(argv[1]);
			if (!FValidMacro(j))
			{
				ErrorValN("M", j);
				return FALSE;
			}
			j--;
			if (i)
				szMacro[j] = SzPersist(argv[2]);
			else
				FProcessCommandLine(szMacro[j]);
			argc -= 1 + i;
			argv += 1 + i;
			break;

		case 'Y':
			i = process_switches_Y(argc, argv, ich, fOr, fAnd, fNot);
			if (i < 0)
				return FALSE;
			argc -= i;
			argv += i;
			break;

			/* Switches which determine the type of chart to display: */

		case 'v':
			if (ch1 == '0')
				SwitchF(us.fVelocity);
			SwitchF(us.fListing);
			break;

		case 'w':
			if (ch1 == '0')
				SwitchF(us.fWheelReverse);
			if (argc > 1 && (i = atoi(argv[1])))
			{
				argc--;
				argv++;
				if (!FValidWheel(i))
				{
					ErrorValN("w", i);
					return FALSE;
				}
				us.nWheelRows = i;
			}
			SwitchF(us.fWheel);
			break;

		case 'g':
			if (ch1 == '0' || ch2 == '0')
				SwitchF(us.fGridConfig);
			if (ch1 == 'a')
				SwitchF(us.fAppSep);
			else if (ch1 == 'p')
				SwitchF(us.fParallel);
			SwitchF(us.fGrid);
			break;

		case 'a':
			SwitchF(us.fAspList);
			if (ch1 == '0')
			{
				SwitchF(us.fAspSummary);
				ch1 = ch2;
			}
			if (ch1 == 'a')
				SwitchF(us.fAppSep);
			else if (ch1 == 'p')
				SwitchF(us.fParallel);
			break;

		case 'm':
			if (ch1 == '0' || ch2 == '0')
				SwitchF(us.fMidSummary);
			if (ch1 == 'a')
				SwitchF(us.fMidAspect);
			SwitchF(us.fMidpoint);
			break;

		case 'Z':
			if (ch1 == '0' || ch2 == '0')
				SwitchF(us.fPrimeVert);
			else if (ch1 == 'd' && ch2 != 'e')
				SwitchF(us.fHorizonSearch);
			if (ch1 == 'e' || ch2 == 'e')
				SwitchF(fEquator);
			SwitchF(us.fHorizon);
			break;

		case 'S':
			SwitchF(us.fOrbit);
			break;

		case 'l':
			if (ch1 == '0')
				SwitchF(us.fSectorApprox);
			SwitchF(us.fSector);
			break;

		case 'j':
			if (ch1 == '0')
				SwitchF(us.fInfluenceSign);
			SwitchF(us.fInfluence);
			break;

		case 'L':
			if (ch1 == '0')
				SwitchF(us.fLatitudeCross);
			if (argc > 1 && (i = atoi(argv[1])))
			{
				argc--;
				argv++;
				if (!FValidAstrograph(i))
				{
					ErrorValN("L", i);
					return FALSE;
				}
				us.nAstroGraphStep = i;
			}
			SwitchF(us.fAstroGraph);
			break;

		case 'K':
			SwitchF(us.fCalendarYear);
			SwitchF(us.fCalendar);
			break;

		case 'd':
			if (ch1 == 'p')
			{
				us.fSolarArc = 0;
				if (ch2 == '0')
					us.fSolarArc = 1;
				if (ch2 == '1')
					us.fSolarArc = 2;
				if (us.fSolarArc)
					ch2 = argv[0][ich++ + 1];
				i = (ch2 == 'y') + 2 * (ch2 == 'Y');
				j = i < 2 && (argv[0][ich + i + 1] == 'n');
				if (!j && argc <= 2 - (i & 1))
				{
					ErrorArgc("dp");
					return FALSE;
				}
				is.fProgressIS = us.fInDayMonth = TRUE;
				SetToHere(&ciTran);

				if (j)
				{
					SetToNow(&ciTran);
				}

				if (i)
				{
					ciTran.mon = 0;
					if (!j)
						ciTran.yea = NParseSz(argv[1], pmYea);
					us.nEphemYears = i == 2 ? atoi(argv[2]) : 1;
				}
				else
				{
					if (!j)
					{
						ciTran.mon = NParseSz(argv[1], pmMon);
						ciTran.yea = NParseSz(argv[2], pmYea);
						if (!FValidMon(ciTran.mon))
						{
							ErrorValN("dp", ciTran.mon);
							return FALSE;
						}
					}
				}
				if (!FValidYea(ciTran.yea))
				{
					ErrorValN("dp", ciTran.yea);
					return FALSE;
				}
				if (!j)
				{
					i = 2 - (i & 1);
					argc -= i;
					argv += i;
				}
			}
			else if (ch1 == 'm' || ch1 == 'y' || ch1 == 'Y')
			{
				if (ch1 == 'y')
					us.nEphemYears = 1;
				else if (ch1 == 'Y')
				{
					if (argc <= 1)
					{
						ErrorArgc("dY");
						return FALSE;
					}
					i = atoi(argv[1]);
					if (i < 1)
					{
						ErrorValN("dY", i);
						return FALSE;
					}
					us.nEphemYears = i;
					argc--;
					argv++;
				}
				SwitchF(us.fInDayMonth);
				ciTran.mon = (ch1 == 'm');
			}
			else if (argc > 1 && (i = atoi(argv[1])))
			{
				if (!FValidDivision(i))
				{
					ErrorValN("d", i);
					return FALSE;
				}
				us.nDivision = i;
				argc--;
				argv++;
			}
			SwitchF(us.fInDay);
			break;

		case 'D':
			if (ch1 == 'I')
			{
				if (argc <= 1)
				{
					ErrorArgc("DI%c", ch2);
					return FALSE;
				}

				switch(ch2)
				{
				case 'M':
					strcpy(dirs.main.dir, argv[1]);
					break;

				case 'E':
					strcpy(dirs.ephemeris.dir, argv[1]);
					// we set the path even if it has not changed (it has low overhead)
					SetEphemerisPath();
					break;

				case 'C':
					strcpy(dirs.charts.dir, argv[1]);
					break;

				case 'N':
					strcpy(dirs.interpretation.dir, argv[1]);
					break;

				case 'L':
					strcpy(dirs.miscel.dir, argv[1]);
					break;

				case 'A':
					strcpy(dirs.american_atlas.dir, argv[1]);
					break;

				case 'I':
					strcpy(dirs.international_atlas.dir, argv[1]);
					break;

				default:
					ErrorSwitch("DI");
					return FALSE;
					break;
				}

				argc -= 1;
				argv += 1;
			}
			else
			{
				SwitchF(us.fInDayInf);
			}
			break;

		case 'E':
			if (ch1 == 'Y' && argc <= 1)
			{
				ErrorArgc("E");
				return FALSE;
			}
			SwitchF(us.fEphemeris);
			if (ch1 == 'y')
				us.nEphemYears = us.fEphemeris ? 1 : 0;
			else if (ch1 == 'Y')
			{
				i = atoi(argv[1]);
				if (i < 1)
				{
					ErrorValN("EY", i);
					return FALSE;
				}
				us.nEphemYears = i;
			}
			break;

		case 'e':
			SwitchF(us.fListing);
			SwitchF(us.fWheel);
			SwitchF(us.fGrid);
			SwitchF(us.fAspList);
			SwitchF(us.fMidpoint);
			SwitchF(us.fHorizon);
			SwitchF(us.fOrbit);
			SwitchF(us.fSector);
			SwitchF(us.fCalendar);
			SwitchF(us.fInfluence);
			SwitchF(us.fAstroGraph);
			SwitchF(us.fInDay);
			SwitchF(us.fInDayInf);
			SwitchF(us.fEphemeris);
			SwitchF(us.fGridConfig);
			SwitchF(us.fInfluenceSign);
			SwitchF(us.fLatitudeCross);
			break;

		case 't':
			SwitchF(us.fTransit);
			SetToHere(&ciTran);

			if (ch1 == 'p')
			{
				us.fSolarArc = 0;
				if (ch2 == '0')
					us.fSolarArc = 1;
				if (ch2 == '1')
					us.fSolarArc = 2;
				if (us.fSolarArc)
					ich++;
				is.fProgressIS = TRUE;
				ch1 = argv[0][++ich];
			}
			if (ch1 == 'r')
			{
				is.fReturn = TRUE;
				ch1 = argv[0][++ich];
			}
			if (i = (ch1 == 'y') + 2 * (ch1 == 'Y'))
				ch1 = argv[0][++ich];
			if (ch1 == 'n')
			{
				SetToNow(&ciTran);

				if (i == 1)
					ciTran.mon = 0;
				else if (i > 1)
				{
					ciTran.mon = -1;
					ciTran.day = NParseSz(argv[1], pmDay);
				}
				break;
			}
			if (argc <= 2 - (i & 1))
			{
				ErrorArgc("t");
				return FALSE;
			}
			if (i)
			{
				if (i == 1)
					ciTran.mon = 0;
				else
				{
					ciTran.mon = -1;
					ciTran.day = NParseSz(argv[2], pmDay);
				}
			}
			else
			{
				ciTran.mon = NParseSz(argv[1], pmMon);
				if (!FValidMon(ciTran.mon))
				{
					ErrorValN("t", ciTran.mon);
					return FALSE;
				}
			}
			ciTran.yea = NParseSz(argv[2 - (i > 0)], pmYea);
			argc -= 2 - (i & 1);
			argv += 2 - (i & 1);
			break;

		case 'T':
			SwitchF(us.fTransitInf);
			SetToHere(&ciTran);

			if (ch1 == 'p')
			{
				is.fProgressIS = TRUE;
				ch1 = argv[0][++ich];
			}
			if (ch1 == 'r')
			{
				is.fReturn = TRUE;
				ch1 = argv[0][++ich];
			}
			if (ch1 == 'n')
			{
				SetToNow(&ciTran);
				break;
			}
			if (argc <= 3)
			{
				ErrorArgc("T");
				return FALSE;
			}
			ciTran.mon = NParseSz(argv[1], pmMon);
			ciTran.day = NParseSz(argv[2], pmDay);
			ciTran.yea = NParseSz(argv[3], pmYea);
			if (!FValidMon(ciTran.mon))
			{
				ErrorValN("T", ciTran.mon);
				return FALSE;
			}
			else if (!FValidDay(ciTran.day, ciTran.mon, ciTran.yea))
			{
				ErrorValN("T", ciTran.day);
				return FALSE;
			}
			else if (!FValidYea(ciTran.yea))
			{
				ErrorValN("T", ciTran.yea);
				return FALSE;
			}
			argc -= 3;
			argv += 3;
			break;

#ifdef ARABIC
		case 'P':
			if (argc > 1 && (i = atoi(argv[1])))
			{
				argc--;
				argv++;
				if (!FValidPart(i))
				{
					ErrorValN("P", i);
					return FALSE;
				}
				us.nArabicParts = i;
			}
			if (ch1 == 'z' || ch1 == 'n' || ch1 == 'f')
			{
				us.nArabic = ch1;
				ch1 = ch2;
			}
			else
				SwitchF(us.nArabic);
			if (ch1 == '0')
				SwitchF(us.fArabicFlip);
			break;
#endif

#ifdef INTERPRET
		case 'I':
#ifdef INTERPRETALT
			InterpretAlt = 0;
#endif
			if (argc > 1 && (i = atoi(argv[1])))
			{
				argc--;
				argv++;
				if (!FValidScreen(i))
				{
					ErrorValN("I", i);
					return FALSE;
				}
				us.nScreenWidth = i;
			}
#ifdef INTERPRETALT
			if (ch1 == 'a')
			{
				InterpretAlt = 1;
				us.fInterpret = 1;
				break;
			}
			if (ch1 == 'b')
			{
				InterpretAlt = 2;
				us.fInterpret = 1;
				break;
			}
			if (ch1 == 'p')
			{
				InterpretAlt = 3;
				us.fInterpret = 1;
				break;
			}
			if (ch1 == 'h')
			{
				InterpretAlt = 4;
				us.fInterpret = 1;
				break;
			}
			if (ch1 == 'o')
			{
				InterpretAlt = 5;
				us.fInterpret = 1;
				break;
			}
			if (ch1 == 'd')
			{
				InterpretAlt = 6;
				us.fInterpret = 1;
				break;
			}
#endif
			SwitchF(us.fInterpret);
			break;
#endif

			/* Switches which affect how the chart parameters are obtained: */

		case 'n':
			SetHereAndNow(&ciCore);
			if (ch1 == 'd')
				ciCore.tim = 0.0;
			else if (ch1 == 'm')
			{
				ciCore.day = 1;
				ciCore.tim = 0.0;
			}
			else if (ch1 == 'y')
			{
				ciCore.mon = ciCore.day = 1;
				ciCore.tim = 0.0;
			}
			break;

		case 'z':
			if (ch1 == '0')
			{
				if (argc <= 1 || RParseSz(argv[1], pmZon) == rLarge)
				{
					i = us.dstDef != 0.0;
					SwitchF(i);
					ciCore.dst = us.dstDef = i ? 1.0 : 0.0;
				}
				else
				{
					ciCore.dst = us.dstDef = RParseSz(argv[1], pmZon);
					if (!FValidDst(us.dstDef))
					{
						ErrorValR("z0", us.dstDef);
						return FALSE;
					}
					argc--;
					argv++;
				}
				break;
			}
			else if (ch1 == 'l')
			{
				if (argc <= 2)
				{
					ErrorArgc("zl");
					return FALSE;
				}
				ciCore.lon = us.lonDef = RParseSz(argv[1], pmLon);
				ciCore.lat = us.latDef = RParseSz(argv[2], pmLat);
				if (!FValidLon(us.lonDef))
				{
					ErrorValR("zl", us.lonDef);
					return FALSE;
				}
				else if (!FValidLat(us.latDef))
				{
					ErrorValR("zl", us.latDef);
					return FALSE;
				}
				argc -= 2;
				argv += 2;
				break;
			}
			else if (ch1 == 't')
			{
				if (argc <= 1)
				{
					ErrorArgc("zt");
					return FALSE;
				}
				rT = RParseSz(argv[1], pmTim);
				if (!FValidTim(rT))
				{
					ErrorValR("zt", rT);
					return FALSE;
				}
				ciCore.tim = rT;
				argc--;
				argv++;
				break;
			}
			else if (ch1 == 'd')
			{
				if (argc <= 1)
				{
					ErrorArgc("zd");
					return FALSE;
				}
				i = NParseSz(argv[1], pmDay);
				if (!FValidDay(i, ciCore.mon, ciCore.yea))
				{
					ErrorValN("zd", i);
					return FALSE;
				}
				ciCore.day = i;
				argc--;
				argv++;
				break;
			}
			else if (ch1 == 'm')
			{
				if (argc <= 1)
				{
					ErrorArgc("zm");
					return FALSE;
				}
				i = NParseSz(argv[1], pmMon);
				if (!FValidMon(i))
				{
					ErrorValN("zm", i);
					return FALSE;
				}
				ciCore.mon = i;
				argc--;
				argv++;
				break;
			}
			else if (ch1 == 'y')
			{
				if (argc <= 1)
				{
					ErrorArgc("zy");
					return FALSE;
				}
				i = NParseSz(argv[1], pmYea);
				if (!FValidYea(i))
				{
					ErrorValN("zy", i);
					return FALSE;
				}
				ciCore.yea = i;
				argc--;
				argv++;
				break;
			}
			else if (ch1 == 'i')
			{
				if (argc <= 2)
				{
					ErrorArgc("zi");
					return FALSE;
				}
				strcpy(ciCore.nam, argv[1]);
				strcpy(ciCore.loc, argv[2]);
				argc -= 2;
				argv += 2;
				break;
			}
			else if (ch1 == 'n')
			{
				if (argc <= 1)
				{
					ErrorArgc("zn");
					return FALSE;
				}
				us.szLocNameDef = SzPersist(argv[1]);
				argc -= 1;
				argv += 1;
				break;
			}

			if (argc <= 1 || RParseSz(argv[1], pmZon) == rLarge)
				ciCore.zon -= 1.0;
			else
			{
				ciCore.zon = us.zonDef = RParseSz(argv[1], pmZon);
				if (!FValidZon(us.zonDef))
				{
					ErrorValR("z", us.zonDef);
					return FALSE;
				}
				argc--;
				argv++;
			}
			break;

		case 'q':
			i = (ch1 == 'y'
				 || ch1 == 'j') + 2 * (ch1 == 'm') + 3 * (ch1 == 'd') +
				4 * (ch1 == chNull) + 7 * (ch1 == 'a') + 8 * (ch1 == 'b');
			if (argc <= i)
			{
				ErrorArgc("q");
				return FALSE;
			}
			if (ch1 == 'j')
			{
				is.JD = atof(argv[1]) + 0.5;
				ciCore.tim = RFract(is.JD);
				JulianToMdy(is.JD - ciCore.tim, &ciCore.mon, &ciCore.day, &ciCore.yea);
				ciCore.tim = DecDeg2DegMin(ciCore.tim * 24.0);
				ciCore.dst = ciCore.zon = 0.0;
				ciCore.lon = us.lonDef;
				ciCore.lat = us.latDef;
				strcpy(ciCore.loc, us.szLocNameDef);
			}
			else
			{
				ciCore.mon = i > 1 ? NParseSz(argv[1], pmMonEn) : 1;
				ciCore.day = i > 2 ? NParseSz(argv[2], pmDay) : 1;
				ciCore.yea = NParseSz(argv[3 - (i < 3) - (i < 2)], pmYea);
				ciCore.tim = i > 3 ? RParseSz(argv[4], pmTim) : (i < 3 ? 0.0 : 12.0);
				ciCore.dst = i > 7 ? RParseSz(argv[5], pmDst) : (i > 6 ? 0.0 : us.dstDef);
				ciCore.zon = i > 6 ? RParseSz(argv[5 + (i > 7)], pmZon) : us.zonDef;
				ciCore.lon = i > 6 ? RParseSz(argv[6 + (i > 7)], pmLon) : us.lonDef;
				ciCore.lat = i > 6 ? RParseSz(argv[7 + (i > 7)], pmLat) : us.latDef;
				if(i <= 6)
				{
					strcpy(ciCore.loc, us.szLocNameDef);
				}

				if (!FValidMon(ciCore.mon))
				{
					ErrorValN("q", ciCore.mon);
					return FALSE;
				}
				else if (!FValidDay(ciCore.day, ciCore.mon, ciCore.yea))
				{
					ErrorValN("q", ciCore.day);
					return FALSE;
				}
				else if (!FValidYea(ciCore.yea))
				{
					ErrorValN("q", ciCore.yea);
					return FALSE;
				}
				else if (!FValidTim(ciCore.tim))
				{
					ErrorValR("q", ciCore.tim);
					return FALSE;
				}
				else if (!FValidDst(ciCore.dst))
				{
					ErrorValR("q", ciCore.dst);
					return FALSE;
				}
				else if (!FValidZon(ciCore.zon))
				{
					ErrorValR("a", ciCore.zon);
					return FALSE;
				}
				else if (!FValidLon(ciCore.lon))
				{
					ErrorValR("a", ciCore.lon);
					return FALSE;
				}
				else if (!FValidLat(ciCore.lat))
				{
					ErrorValR("a", ciCore.lat);
					return FALSE;
				}
			}
			argc -= i;
			argv += i;
			break;

		case 'i':
			if (us.fNoRead)
			{
				ErrorArgv("i");
				return tcError;
			}
			if (argc <= 1)
			{
				ErrorArgc("i");
				return FALSE;
			}

			ci = ciCore;
			if (!FInputData(argv[1], ch1 == 'c'? dirs.miscel.dir : dirs.charts.dir))
				return FALSE;
			if (ch1 == '2')
			{
				ciTwin = ciCore;
				ciCore = ci;
			}
			else if (ch1 == '3')
			{
				ciThre = ciCore;
				ciCore = ci;
			}
			else if (ch1 == '4')
			{
				ciFour = ciCore;
				ciCore = ci;
			}
			argc--;
			argv++;
			break;

		case '>':
			ch1 = 's';
			/* Fall through */

		case 'o':
			if (us.fNoWrite)
			{
				ErrorArgv("o");
				return tcError;
			}
			if (argc <= 1)
			{
				ErrorArgc("o");
				return FALSE;
			}
			if (ch1 == 's')
			{
				is.szFileScreen = SzPersist(argv[1]);
				argc--;
				argv++;
				break;
			}
			else if (ch1 == '0')
				SwitchF(us.fWritePos);
			SwitchF(us.fWriteFile);
			is.szFileOut = SzPersist(argv[1]);
			break;

			/* Switches which affect what information is used in a chart: */

		case 'R':
			if (ch1 == 'A')
			{
				// This used to be the -RA switch (restrict aspects by giving 
				// them negative orbs). It still restricts aspects, but does
				// it by setting the ignore byte.
				while (argc > 1 && (i = NParseSz(argv[1], pmAspect)))
					if (!FBetween(i, 1, cAspect))
					{
						ErrorValN("RA", i);
						return FALSE;
					}
					else
					{
						ignoreA[i] = TRUE;
						argc--;
						argv++;
					}
				break;
			}
			if (ch1 == 'T')
			{
				pch = (char *) ignore2;  
				ch1 = argv[0][++ich];
			}
			else
				pch = (char *) ignore;
			if (ch1 == '0')
				for (i = 1; i <= cObj; i++)
					pch[i] = TRUE;
			else if (ch1 == '1')
			{
				for (i = 1; i <= cObj; i++)
					pch[i] = FALSE;
				us.nStar = TRUE;
			}
			else if (ch1 == 'C')
				for (i = cuspLo; i <= cuspHi; i++)
					SwitchF(pch[i]);
			else if (ch1 == 'u')
				for (i = uranLo; i <= uranHi; i++)
					SwitchF(pch[i]);
			else if (ch1 == 'U')
				for (i = starLo; i <= starHi; i++)
					SwitchF(pch[i]);
			else if (argc <= 1 || (!NParseSz(argv[1], pmObjectEn)))
			{
				for (i = oChi; i <= oVesta; i++)
					SwitchF(pch[i]);
				for (i = oLil; i <= oEP; i++)
					SwitchF(pch[i]);
			}
			while (argc > 1 && (i = NParseSz(argv[1], pmObjectEn)))
				if (!FItem(i))
				{
					ErrorValN("R", i);
					return FALSE;
				}
				else
				{
					SwitchF(pch[i]);
					argc--;
					argv++;
				}
			break;

		case 'C':
			// used to set us.fCusp, kept for backward compatibility
			break;

		case 'u':
			// used to set us.fUranian, kept for backward compatibility
			break;

		case 'U':
			if (ch1 == 'z' || ch1 == 'l' || ch1 == 'n' || ch1 == 'b' || ch1 == 'r' || ch1 == 'p')
				us.nStar = ch1;
			else
				SwitchF(us.nStar);
			break;

		case 'A':
			if (ch1 != 'o' && ch1 != 'm' && ch1 != 'd' && ch1 != 'a' && ch1 != 'n' && ch1 != 'A')
			{
				// The -A switch (specify the number of aspects to use in charts) was disabled, 
				// but the program does not complain about it for backward compatibility.

				if (argc <= 1)
				{
					ErrorArgc("A");
					return FALSE;
				}
				i = NParseSz(argv[1], pmAspect);
				if (!FValidAspect(i))
				{
					ErrorValN("A", i);
					return FALSE;
				}
				argc--;
				argv++;
			}
			else
			{
				if (argc <= 2)
				{
					ErrorArgc("A");
					return FALSE;
				}
				if (ch1 == 'n' || ch1 == 'A')
				{
					i = NParseSz(argv[1], pmAspect);
					if (i < 1 || i > cAspect)
					{
						ErrorValN("A", i);
						return FALSE;
					}
					if (ch1 == 'n')
						Lang.SetTableItem(tAspectName, i, argv[2]);
					else
					{
						Lang.SetTableItem(tAspectAbbrev, i, argv[2]);
						if (i == 1)
							Lang.SetString(sAspectAbbrevCon,argv[2]);
						if (i == 2)
							Lang.SetString(sAspectAbbrevOpp,argv[2]);
					}
				}
				else
				{
					i = NParseSz(argv[1], ch1 == 'o' || ch1 == 'a' ? pmAspect : pmObjectEn);

					if (i < 1 || i > (ch1 == 'o' || ch1 == 'a' ? cAspect : cLastMoving))
					{
						ErrorValN("A", i);
						return FALSE;
					}
					rT = RParseSz(argv[2], 0);
					if (rT < -360.0 || rT > 360.0)
					{
						ErrorValR("A", rT);
						return FALSE;
					}
					switch(ch1)
					{
					case 'o':
						if (rT < 0.0)
						{
							// For backward compatibility, we accept negative 
							// orbs, but then ignore the aspect, which is the  
							// old meaning. This is to ease transition to the  
							// new version.
							rT = 0.0;
							ignoreA[i] = TRUE;
						}
						rAspOrb[i] = rT;
						break;

					case 'm':
						rObjOrb[i] = rT;
						break;

					case 'd':
						rObjAdd[i] = rT;
						break;

					default:
						rAspAngle[i] = rT;
						break;
					}
				}
				argc -= 2;
				argv += 2;
			}
			break;

			/* Switches which affect how a chart is computed: */

		case 'b':
			if (ch1 == '0')
				SwitchF(us.fSeconds);
			is.fSeconds = us.fSeconds;
			break;

		case 'c':
			if (argc <= 1)
			{
				ErrorArgc("c");
				return FALSE;
			}
			i = NParseSz(argv[1], pmHouseSystem);
			if (!FValidSystem(i))
			{
				ErrorValN("c", i);
				return FALSE;
			}
			us.nHouseSystem = i;
			argc--;
			argv++;
			break;

		case 's':
			if (argc > 1 && ch1 != 'm' && ((rT = atof(argv[1])) != 0.0 || FNumCh(argv[1][0])))
			{
				if (!FValidOffset(rT))
				{
					ErrorValR("s", rT);
					return FALSE;
				}
				argc--;
				argv++;
				us.rSiderealCorrection = rT;
			}
			if (ch1 == 'r')
				SwitchF(us.fEquator);
			else if (ch1 == 'h')
				us.nDegForm = 1;
			else if (ch1 == 'd')
				us.nDegForm = 2;
			else if (ch1 == 'z')
				us.nDegForm = 0;
			else if (ch1 == 'm')
			{
				if (argc > 1)
				{
					j = atoi(argv[1]);

					if (j < FIRST_SIDEREAL_MODE || j > LAST_SIDEREAL_MODE)
					{
						ErrorValR("sm", j);
						return FALSE;
					}

					if(us.nSiderealMode != j)
					{
						us.nSiderealMode = j;
						swe_set_sid_mode(us.nSiderealMode, 0, 0);
					}
					argc--;
					argv++;
				}
				else
				{
					ErrorArgc("sm");
					return FALSE;
				}
			}
			else
			{
				SwitchF(us.fSidereal);
				wi.fMenuAll = TRUE;
				wi.fCast = TRUE;
			}
			break;

		case 'h':
			if (argc > 1 && (i = NParseSz(argv[1], pmObjectEn)))
			{
				argc--;
				argv++;
			}
			else
				i = FSwitchF(us.objCenter != 0);
			if (!FValidCenter(i))
			{
				ErrorValN("h", i);
				return FALSE;
			}
			us.objCenter = i;
			break;

		case 'p':
			if (fAnd)
			{
				us.fProgressUS = FALSE;
				break;
			}
			us.fSolarArc = 0;
			if (ch1 == '0')
				us.fSolarArc = 1;
			if (ch1 == '1')
				us.fSolarArc = 2;
			if (us.fSolarArc)
				ch1 = (argv[0][++ich]);

			// There seems to be a bug here, inherited from 5.41g but does not exist in 5.40.
			if (ch1 == 'n')
			{
				us.fProgressUS = TRUE;
				wi.fInitProgression = TRUE;
				SetHereAndNow(&ciMain);
				is.JDp = MdytszToJulian(ciMain.mon, ciMain.day, ciMain.yea, ciMain.tim, us.dstDef, us.zonDef);
				break;
			}
			if (ch1 == 'd')
			{
				if (argc <= 1)
				{
					ErrorArgc("pd");
					return FALSE;
				}
				us.rProgDay = atof(argv[1]);
				if (us.rProgDay == 0.0)
				{
					ErrorValR("pd", us.rProgDay);
					return FALSE;
				}
				us.fProgressUS = TRUE;
				wi.fInitProgression = TRUE;
				argc--;
				argv++;
				break;
			}
			if (argc <= 3)
			{
				ErrorArgc("p");
				return FALSE;
			}
			ciMain.mon = NParseSz(argv[1], pmMon);
			ciMain.day = NParseSz(argv[2], pmDay);
			ciMain.yea = NParseSz(argv[3], pmYea);
			if (!FValidMon(ciMain.mon))
			{
				ErrorValN("p", ciMain.mon);
				return FALSE;
			}
			else if (!FValidDay(ciMain.day, ciMain.mon, ciMain.yea))
			{
				ErrorValN("p", ciMain.day);
				return FALSE;
			}
			else if (!FValidYea(ciMain.yea))
			{
				ErrorValN("p", ciMain.yea);
				return FALSE;
			}
			SetToHere(&ciMain);
			is.JDp = MdytszToJulian(ciMain.mon, ciMain.day, ciMain.yea, 0.0, us.dstDef, us.zonDef);
			us.fProgressUS = TRUE;
			wi.fInitProgression = TRUE;
			argc -= 3;
			argv += 3;
			break;

		case 'x':
			if (argc <= 1)
			{
				ErrorArgc("x");
				return FALSE;
			}
			r = atof(argv[1]);
			if (!FValidHarmonic(r))
			{
				ErrorValN("x", r);
				return FALSE;
			}
			us.nHarmonic = r;
			argc--;
			argv++;
			break;

		case '1':
			if (argc > 1 && (i = NParseSz(argv[1], pmObjectEn)))
			{
				argc--;
				argv++;
			}
			else
				i = oSun;
			if (!FItem(i))
			{
				ErrorValN("1", i);
				return FALSE;
			}
			us.objOnAsc = fAnd ? 0 : i;
			break;

		case '2':
			if (argc > 1 && (i = NParseSz(argv[1], pmObjectEn)))
			{
				argc--;
				argv++;
			}
			else
				i = oSun;
			if (!FItem(i))
			{
				ErrorValN("2", i);
				return FALSE;
			}
			us.objOnAsc = fAnd ? 0 : -i;
			break;

		case '3':
			SwitchF(us.fDecan);
			break;

		case 'f':
			SwitchF(us.fFlip);
			break;

		case 'G':
			SwitchF(us.fGeodetic);
			break;

		case 'J':
			SwitchF(us.fVedic);
			break;

		case '9':
			SwitchF(us.fNavamsa);
			break;

		case 'F':
			if (argc <= 3)
			{
				ErrorArgc("F");
				return FALSE;
			}
			i = NParseSz(argv[1], pmObjectEn);
			if (!FItem(i))
			{
				ErrorValN("F", i);
				return FALSE;
			}
			force[i] = (NParseSz(argv[2], pmSignEn) - 1.0) * 30.0 + DegMin2DecDeg(atof(argv[3]));
			if (force[i] < 0.0 || force[i] >= 360.0)
			{
				ErrorValR("F", force[i]);
				return FALSE;
			}
			else
				force[i] += 360.0;
			argc -= 3;
			argv += 3;
			break;

		case '+':
			if (argc > 1 && (i = atoi(argv[1])) != 0)
			{
				argc--;
				argv++;
			}
			else
				i = 1;
			us.dayDelta += i * (ch1 == 'y' ? 365 : (ch1 == 'm' ? 30 : 1));
			break;

		case chNull:
			if (ich <= 1)
				break;
			/* Fall thorugh */

		case '-':
			if (argc > 1 && (i = atoi(argv[1])) != 0)
			{
				argc--;
				argv++;
			}
			else
				i = 1;
			us.dayDelta -= i * (ch1 == 'y' ? 365 : (ch1 == 'm' ? 30 : 1));
			break;

			/* Switches for relationship and comparison charts: */

		case 'r':
			if (fAnd)
			{
				us.nRel = 0;
				break;
			}
			else if (FBetween(ch1, '1', '4'))
			{
				us.nRel = -(int) (ch1 - '1');
				break;
			}
			i = 2 + 2 * ((ch1 == 'c' || ch1 == 'm') && ch2 == '0');
			if (argc <= i)
			{
				ErrorArgc("r");
				return FALSE;
			}
			if (ch1 == 'c')
				us.nRel = rcComposite;
			else if (ch1 == 'm')
				us.nRel = rcMidpoint;
			else if (ch1 == 'd')
				us.nRel = rcDifference;
#ifdef BIORHYTHM
			else if (ch1 == 'b')
				us.nRel = rcBiorhythm;
#endif
			else if (ch1 == '0')
				us.nRel = rcDual;
			else if (ch1 == 't')
				us.nRel = rcTransit;
			else if (ch1 == 'p')
			{
				us.nRel = rcProgress;
				us.fSolarArc = 0;
				if (ch2 == '0')
					us.fSolarArc = 1;
				if (ch2 == '1')
					us.fSolarArc = 2;
			}
			else
				us.nRel = rcSynastry;
			ci = ciCore;
			ciCore = ciTwin;
			if (!FInputData(argv[2], dirs.charts.dir))
				return FALSE;
			ciTwin = ciCore;
			ciCore = ci;
			if (!FInputData(argv[1], dirs.charts.dir))
				return FALSE;
			if (i > 2)
			{
				us.nRatio1 = atoi(argv[3]);
				us.nRatio2 = atoi(argv[4]);
				if (us.nRatio1 == us.nRatio2)
					us.nRatio1 = us.nRatio2 = 1;
			}
			argc -= i;
			argv += i;
			break;

		case 'y':
			if (argc <= 1)
			{
				ErrorArgc("y");
				return FALSE;
			}
			if (ch1 == 'd')
				us.nRel = rcDifference;
#ifdef BIORHYTHM
			else if (ch1 == 'b')
				us.nRel = rcBiorhythm;
#endif
			else if (ch1 == 't')
				us.nRel = rcTransit;
			else if (ch1 == 'p')
			{
				us.nRel = rcProgress;
				us.fSolarArc = 0;
				if (ch2 == '0')
					us.fSolarArc = 1;
				if (ch2 == '1')
					us.fSolarArc = 2;
			}
			else
				us.nRel = rcDual;
			SetHereAndNow(&ciCore);
			ciTwin = ciCore;
			if (!FInputData(argv[1], dirs.charts.dir))
				return FALSE;
			argc--;
			argv++;
			break;

			/* Switches to access graphics options: */

		case 'k':
			if (ch1 == '1')
			{
				us.fAnsiColor = 2;
				us.fAnsiChar = 1;
			}
			else
			{
				if (ch1 != '0')
					SwitchF(us.fAnsiColor);
				SwitchF(us.fAnsiChar);
			}
			break;

		case 'X':
			if (us.fNoGraphics)
			{
				ErrorArgv("X");
				return FALSE;
			}
			i = process_switches_X(argc, argv, ich, fOr, fAnd, fNot);
			if (i < 0)
				return FALSE;
			SwitchF2(us.fGraphics);
			argc -= i;
			argv += i;
			break;

		case 'W':
			i = process_switches_W(argc, argv, ich, fOr, fAnd, fNot);
			if (i < 0)
				return FALSE;
			argc -= i;
			argv += i;
			break;

		case '0':
			while (ch1 != chNull)
			{
				switch (ch1)
				{
				case 'o':
					us.fNoWrite = TRUE;
					break;
				case 'i':
					us.fNoRead = TRUE;
					break;
				case 'q':
					us.fNoQuit = TRUE;
					break;
				case 'X':
					us.fNoGraphics = TRUE;
					break;
				}
				ch1 = (argv[0][++ich]);
			}
			break;

		case ';':				/* The -; switch means don't process the rest of the line. */
			return TRUE;

		case '@':				/* The -@ switch is just a system flag indicator no-op. */
			{
				int file_type = 0, file_version = 0;

				sscanf(argv[0] + 1, "%2d%2d", &file_type, &file_version);

				switch(file_type)
				{
				case FILE_TYPE_CONFIGURATION:
					if(file_version != FILE_VERSION_CONFIGURATION)
					{
						PrintWarning(Lang(sINCOMPATIBLE_CONFIG_FILE), 
							file_version, FILE_VERSION_CONFIGURATION);
						return FALSE;
					}
					break;

				default: 
					break;
				}
			}
			break;

		case '.':				/* "-." is usually used to exit the -Q loop. */
			Terminate(tcForce);
			break;

		case 'B':				/* For no useful reason, -B sounds a beep. */
			// PrintWarning("beep");
			MessageBeep(-1);	
			// On Windows 98 this does not beep for some unknown reason, but it beeps if 
			// PrintWarning is called before! It works fine on Windows XP.
			break;

		default:
			ErrorSwitch(argv[0]);
			return FALSE;
		}

		argc--;
		argv++;
	}

	return TRUE;
}

/* process_switches.c */
