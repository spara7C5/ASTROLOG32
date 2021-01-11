/*
** Astrolog File: wdialog.c
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
#include "gui.h"



/*
******************************************************************************
** Local Definitions
******************************************************************************
*/


/*
******************************************************************************
** Dialog Utility Functions
******************************************************************************
*/

/* Set the contents of the given edit control in a dialog to a string. */

void SetEditSz(HWND hdlg, int id, char *sz) {
	while (*sz == ' ')			/* Strip off any extra leading spaces. */
		sz++;
	SetEdit(id, sz);
}


/* Set the contents of the given edit control in a dialog to a floating */
/* point value, with at most 'n' significant fractional digits.         */

void SetEditR(HWND hdlg, int id, double r, int n) {
	char sz[cchSzDef], szT[8], *pch;

	sprintf(szT, "%%.%df", n);
	sprintf(sz, szT, r);
	for (pch = sz; *pch; pch++)
		;
	while (*(--pch) == '0')		/* Drop off any trailing zero digits. */
		;
	pch[1 + (*pch == '.')] = chNull;	/* Ensure at least one fractional digit. */
	SetEdit(id, sz);
}


/* Set the contents of four combo controls and their dropdowns in a dialog */
/* indicating month, day, year, and time fields to the given values.       */

void SetEditMDYT(HWND hdlg, int idMon, int idDay, int idYea, int idTim, int mon, int day, int yea, double tim) {
	char sz[cchSzDef];
	int i;

	ClearCombo(idMon);
	ClearCombo(idDay);
	ClearCombo(idYea);
	ClearCombo(idTim);
	if (!FValidMon(mon))
		mon = 1;
	sprintf(sz, "%s", Lang(tMonth, mon));
	SetEdit(idMon, sz);
	for (i = 1; i <= NUMBER_OF_MONTHS; i++)
		SetCombo(idMon, Lang(tMonth, i));
	if (!FValidDay(day, mon, yea))
		day = 1;
	SetEditN(idDay, day);
	for (i = 1; i <= 31; i++)
	{
		sprintf(sz, "%d", i);
		SetCombo(idDay, sz);
	}

	SetEditN(idYea, yea);
	for (i = 1920; i <= 2020; i++)
	{
		sprintf(sz, "%d", i);
		SetCombo(idYea, sz);
	}

	SetEdit(idTim, SzTim(tim));
	for (i = 0; i <= 23; i++)
	{
		SetCombo(idTim, SzTime(i, 0, 0));
		SetCombo(idTim, SzTime(i, 15, 0));
		SetCombo(idTim, SzTime(i, 30, 0));
		SetCombo(idTim, SzTime(i, 45, 0));
	}
}


/* Set the contents of five controls in a dialog box indicating daylight,	*/
/* time zone, longitude, latitude and location fields to the given values.	*/

void SetEditSZOA(HWND hdlg, int idDst, int idZon, int idLon, int idLat, int idLoc, 
				 double dst, double zon, double lon, double lat, char *loc)
{
	char sz[cchSzDef];
	int i;
	BOOL fT;

	if (dst == 0.0)
	{
		strcpy(sz, Lang(sNO));
	}
	else if (dst == 1.0)
	{
		strcpy(sz, Lang(sYES));
	}
	else
	{
		sprintf(sz, "%.2f", dst);
	}

	SetEdit(idDst, sz);
	SetCombo(idDst, Lang(sNO));
	SetCombo(idDst, Lang(sYES));
	SetEdit(idLoc, loc);
	sprintf(sz, "%s", SzZone(-zon));
	SetEdit(idZon, (char *) (sz[0] == '+' ? &sz[1] : sz));

	/* For the time zone dropdown, fill it out with all abbreviations of */
	/* three letters that don't reference daylight or war time.          */
	for (i = 0; i < cZone; i++)
	{
		if (szZon[i][1] && szZon[i][1] != 'D' && szZon[i][1] != 'W' && szZon[i][2] && szZon[i][2] != 'D')
			SetCombo(idZon, szZon[i]);
	}

	fT = us.fAnsiChar;
	us.fAnsiChar = FALSE;
	sprintf(sz, "%s", SzLocation(lon, lat));
	us.fAnsiChar = fT;
	sz[10] = chNull;
	SetEditSz(hdlg, idLon, &sz[0]);
	SetEditSz(hdlg, idLat, &sz[11]);
}


/* Set the contents of a combo control in a dialog indicating a color   */
/* field to the given value, and fill its dropdown with the color list. */

void SetEditColor(HWND hdlg, int id, KI ki) {
	int i;

	SetEdit(id, Lang(tColor, ki));
	for (i = 0; i < cColor; i++)
		SetCombo(id, Lang(tColor, i));
}


/* Return the contents of a dialog edit control as a floating point value. */

double GetEditR(HWND hdlg, int id) {
	char sz[cchSzDef];

	GetEdit(id, sz);
	return atof(sz);
}


void WarnWrongValue(int n, char *string)
{
	PrintWarning(Lang(sINT_VALUE_NOT_VALID_SETTING), n, string);
}


/*
******************************************************************************
** Windows Dialogue Boxes
******************************************************************************
*/

/* Bring up the Windows standard file open dialog, and process the        */
/* command file specified if any. This is called from the File Open Chart */
/* menu commands.							                              */

BOOL OpenChartDialog()
{
	char sz[cchSzDef];
	CI ciT;

	/* Setup dialog title and settings and get the name of a file from it. */
	if (us.fNoRead)
	{
		PrintWarning("File input is not allowed now.");
		return FALSE;
	}

	if (wi.nDlgChart == 1)
	{
		sprintf(sz, Lang(sOPEN_MAIN_CHART));
	}
	else
	{
		sprintf(sz, Lang(sOPEN_CHART), wi.nDlgChart);
	}

	ofn.lpstrTitle = sz;
	ofn.lpstrInitialDir = dirs.charts.dir;
	ofn.lpstrFilter = "Data Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";
	*ofn.lpstrFile = chNull;

	if (!GetOpenFileName((LPOPENFILENAME) &ofn))
	{
		return FALSE;
	}

	/* Process the given file based on what open command is being run. */
	ciT = ciCore;
	FInputData(ofn.lpstrFileTitle, dirs.charts.dir);

	if (wi.nDlgChart > 1)
	{
		if (wi.nDlgChart == 2)
			ciTwin = ciCore;
		else if (wi.nDlgChart == 3)
			ciThre = ciCore;
		else
			ciFour = ciCore;
		ciCore = ciT;
	}

	wi.fCast = TRUE;
	return TRUE;
}

// Open and load a command file

BOOL OpenCommandFile()
{
	char sz[cchSzDef];

	/* Setup dialog title and settings and get the name of a file from it. */
	if (us.fNoRead)
	{
		PrintWarning("File input is not allowed now.");
		return FALSE;
	}
	sprintf(sz, Lang(sOPEN_COMMAND_FILE));

	ofn.lpstrTitle = sz;
	ofn.lpstrInitialDir = dirs.miscel.dir;
	ofn.lpstrFilter = Lang(sDAT_MASK);
	*ofn.lpstrFile = chNull;

	if (!GetOpenFileName((LPOPENFILENAME) &ofn))
	{
		return FALSE;
	}
		
	if(FileFind(ofn.lpstrFile, dirs.main.dir, NULL))
	{
		FInputData(ofn.lpstrFile, dirs.main.dir);
	}
	return TRUE;
}

/* Bring up the Windows standard file save dialog, get the name of a file   */
/* from the user, and save to it either right away or set variables to      */
/* ensure it will be done later. This is called from all seven File Save    */
/* menu commands: Save Info, Positions, Text, Bitmap, Picture, PostScript,  */
/* and Settings.     */

BOOL SaveChartDialog(WORD wCmd)
{
	/* Setup dialog title and settings and get the name of a file from it. */
	if (us.fNoWrite)
	{
		PrintWarning("File output is not allowed now.");
		return FALSE;
	}

	switch (wCmd)
	{
	case cmdSaveChart:
		ofn.lpstrInitialDir = dirs.charts.dir;
		ofn.lpstrTitle = Lang(sSAVE_MAIN_CHART_DATA);
		ofn.lpstrFilter = Lang(sDAT_MASK);
		strcpy(ofn.lpstrFile, ciMain.nam);
		strcat(ofn.lpstrFile, ".dat");
		break;
	case cmdSavePositions:
		ofn.lpstrInitialDir = dirs.charts.dir;
		ofn.lpstrTitle = Lang(sSAVE_CHART_POSITIONS);
		ofn.lpstrFilter = Lang(sDAT_MASK);
		strcpy(ofn.lpstrFile, ciMain.nam);
		strcat(ofn.lpstrFile, "_pos.dat");
		break;
	case cmdSaveText:
		ofn.lpstrInitialDir = dirs.miscel.dir;
		ofn.lpstrTitle = Lang(sSAVE_CHART_AS_TEXT_FILE);
		ofn.lpstrFilter = Lang(sTXT_MASK);
		strcpy(ofn.lpstrFile, ciMain.nam);
		strcat(ofn.lpstrFile, ".txt");
		break;
	case cmdSaveBitmap:
		ofn.lpstrInitialDir = dirs.miscel.dir;
		ofn.lpstrTitle = Lang(sSAVE_CHART_AS_BITMAP);
		ofn.lpstrFilter = Lang(sBMP_MASK);
		strcpy(ofn.lpstrFile, ciMain.nam);
		strcat(ofn.lpstrFile, ".bmp");
		break;
	case cmdSavePicture:
		ofn.lpstrInitialDir = dirs.miscel.dir;
		ofn.lpstrTitle = Lang(sSAVE_CHART_AS_WMF);
		ofn.lpstrFilter = Lang(sWMF_MASK);
		strcpy(ofn.lpstrFile, ciMain.nam);
		strcat(ofn.lpstrFile, ".wmf");
		break;
	case cmdSavePS:
		ofn.lpstrInitialDir = dirs.miscel.dir;
		ofn.lpstrTitle = Lang(sSAVE_CHART_AS_POSTSCRIPT);
		ofn.lpstrFilter = Lang(sPS_MASK);
		strcpy(ofn.lpstrFile, ciMain.nam);
		strcat(ofn.lpstrFile, ".eps");
		break;
	case cmdSaveSettings:
		ofn.lpstrInitialDir = dirs.main.dir;
		ofn.lpstrTitle = Lang(sSAVE_CURRENT_SETTINGS);
		ofn.lpstrFilter = Lang(sDAT_MASK);
		strcpy(ofn.lpstrFile, CONFIGURATION_FILE);
		break;

	default:
		// is this needed ??
		ofn.lpstrInitialDir = dirs.miscel.dir;
		*ofn.lpstrFile = chNull;
		break;
	}

	switch(wCmd)
	{
	case cmdSaveWallTile:
	case cmdSaveWallCenter:
	case cmdSaveWallStretch:
		ofn.lpstrFilter = Lang(sBMP_MASK);
		GetWindowsDirectory(ofn.lpstrFile, MAX_FILE_NAME);
		strcat(ofn.lpstrFile, "\\" szAppNameCore " .bmp");
		break;

	default:
		if (!GetSaveFileName(&ofn))
			return FALSE;
		break;
	}

	/* Saving chart info, position, or setting command files can be done  */
	/* here. Saving actual chart output isn't done until the next redraw. */
	is.szFileOut = gi.szFileOut = ofn.lpstrFile;

	switch (wCmd)
	{
	case cmdSaveChart:
		us.fWritePos = FALSE;
		return FOutputData();
		break;
	case cmdSavePositions:
		us.fWritePos = TRUE;
		return FOutputData();
		break;
	case cmdSaveSettings:
		return FOutputSettings();
		break;
	case cmdSaveText:
		is.szFileScreen = ofn.lpstrFile;
		us.fGraphics = FALSE;
		wi.fRedraw = TRUE;
		break;
	case cmdSaveBitmap:
	case cmdSaveWallTile:
	case cmdSaveWallCenter:
	case cmdSaveWallStretch:
		gs.fBitmap = TRUE;
		gs.fMeta = gs.fPS = FALSE;
		us.fGraphics = wi.fRedraw = TRUE;
		break;
	case cmdSavePicture:
		gs.fMeta = TRUE;
		gs.fBitmap = gs.fPS = FALSE;
		us.fGraphics = wi.fRedraw = TRUE;
		break;
	case cmdSavePS:
		gs.fPS = TRUE;
		gs.fBitmap = gs.fMeta = FALSE;
		us.fGraphics = wi.fRedraw = TRUE;
		break;
	default:
		break;
	}
	return TRUE;
}


/* Bring up the Windows standard print dialog, receive any printing       */
/* settings from the user, and proceed to print the current graphics or   */
/* text chart as displayed in the window. Called from File Print command. */

BOOL PrintDialog()
{
	HDC hdc;
	LPDEVMODE lpDevMode = NULL;
	LPDEVNAMES lpDevNames;
	LPSTR lpszDriverName;
	LPSTR lpszDeviceName;
	LPSTR lpszPortName;
	DOCINFO doci;
	int xPrint, yPrint;
	int xClient, yClient;
	int result;

	/* Bring up the Windows print dialog. */
	wi.fNoUpdate = FALSE;

	if (!PrintDlg(&prd))
	{
		return TRUE;
	}

	/* Get the printer DC. */
	if (prd.hDC)
		hdc = prd.hDC;
	else
	{
		/* If the dialog didn't just return the DC, try to make one manually. */
		if (!prd.hDevNames)
		{
			return FALSE;
		}

		lpDevNames = (LPDEVNAMES) GlobalLock(prd.hDevNames);
		lpszDriverName = (LPSTR) lpDevNames + lpDevNames->wDriverOffset;
		lpszDeviceName = (LPSTR) lpDevNames + lpDevNames->wDeviceOffset;
		lpszPortName = (LPSTR) lpDevNames + lpDevNames->wOutputOffset;
		GlobalUnlock(prd.hDevNames);
		if (prd.hDevMode)
			lpDevMode = (LPDEVMODE) GlobalLock(prd.hDevMode);
		hdc = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, lpDevMode);
		if (prd.hDevMode && lpDevMode)
			GlobalUnlock(prd.hDevMode);
	}

	if (prd.hDevNames)
	{
		GlobalFree(prd.hDevNames);
		prd.hDevNames = (HGLOBAL) NULL;
	}
	if (prd.hDevMode)
	{
		GlobalFree(prd.hDevMode);
		prd.hDevMode = (HGLOBAL) NULL;
	}

	/* Setup the abort dialog and start the print job. */
	SetAbortProc(hdc, DlgAbortProc);

	memset(&doci, 0, sizeof(DOCINFO));
	doci.cbSize = sizeof(DOCINFO);
	doci.lpszDocName = szAppName;

	if (StartDoc(hdc, &doci) <= 0)
	{
		DeleteDC(hdc);
		return FALSE;
	}

	wi.fAbort = FALSE;
	wi.hwndAbort = CreateDialog(wi.hinst, MAKEINTRESOURCE(dlgAbort), wi.hwnd, DlgAbort);
	if (!wi.hwndAbort)
	{
		return FALSE;
	}
	ShowWindow(wi.hwndAbort, SW_NORMAL);
	EnableWindow(wi.hwnd, FALSE);
	wi.fPrinting = TRUE;

	result = StartPage(hdc);
	if (result <= 0)
	{
		PrintWarning("StartPage failed with error %d", result);
		return FALSE;
	}

	/* Scale the chart to the page. */
	if (us.fGraphics)
	{
		gs.xWin *= METAMUL;
		gs.yWin *= METAMUL;
		gs.nScale *= METAMUL;
	}

	SetMapMode(hdc, MM_ANISOTROPIC);	/* So SetViewportExtEx can be called */
	xPrint = GetDeviceCaps(hdc, HORZRES);
	yPrint = GetDeviceCaps(hdc, VERTRES);

	// save settings for next page
	wi.xPrint = xPrint;
	wi.yPrint = yPrint;

	SetViewportOrgEx(hdc, 0, 0, NULL);
	SetViewportExtEx(hdc, xPrint, yPrint, NULL);
	xClient = wi.xClient;
	yClient = wi.yClient;
	wi.xClient = gs.xWin;
	wi.yClient = NMultDiv(wi.xClient, yPrint, xPrint);
	if (gs.yWin > wi.yClient)
	{
		wi.yClient = gs.yWin;
		wi.xClient = NMultDiv(wi.yClient, xPrint, yPrint);
	}
	wi.hdcPrint = hdc;

	FRedraw();					/* Actually go "draw" the chart to the printer. */

	/* Restore globals that were temporarily changed to print above. */
	wi.hdcPrint = hdcNil;
	wi.xClient = xClient;
	wi.yClient = yClient;
	if (us.fGraphics)
	{
		gs.xWin /= METAMUL;
		gs.yWin /= METAMUL;
		gs.nScale /= METAMUL;
	}

	/* Finalize and cleanup everything. */
	if (!wi.fAbort)
	{
		result = EndPage(hdc);
		if (result <= 0)
		{
			PrintWarning("EndPage failed with error %d", result);
		}
		else
		{
			result = EndDoc(hdc);
			if (result <= 0)
			{
				PrintWarning("EndDoc failed with error %d", result);
			}
		}
	}

	EnableWindow(wi.hwnd, TRUE);
	wi.fPrinting = FALSE;
	wi.fRedraw = TRUE;
	DestroyWindow(wi.hwndAbort);
	DeleteDC(hdc);

	return TRUE;
}


/* Message loop function for the printing abort dialog. Loops until       */
/* printing is completed or the user hits cancel, returning which result. */

int _stdcall DlgAbortProc(HDC hdc, int nCode)
{
	MSG msg;

	if (wi.hwndAbort == (HWND) NULL)
	{
		return TRUE;
	}

	while (!wi.fAbort && PeekMessage(&msg, (HWND) NULL, 0, 0, PM_REMOVE))
	{
		if (!IsDialogMessage(wi.hwndAbort, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	wi.fPrinting = FALSE;
	return !wi.fAbort;
}


/* Processing function for the printing abort modeless dialog, as brought */
/* up temporarily when printing via the File Print menu command.          */
int CALLBACK DlgAbort(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hdlg, IDCANCEL));
		return FALSE;

	case WM_COMMAND:
		if (wid == IDOK || wid == IDCANCEL)
		{
			wi.fAbort = TRUE;
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}



/* Processing function for the command switch entry dialog, as brought up */
/* with the Edit Enter Command Line menu command.                         */

int CALLBACK DlgCommand(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char sz[cchSzDef];
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hdlg, deCo));
		return FALSE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			GetDlgItemText(hdlg, deCo, sz, cchSzDef);
			FProcessCommandLine(sz);
			wi.fCast = wi.fMenuAll = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the color customization dialog, as brought up */
/* with the View Set Colors menu command.                                */

int CALLBACK DlgColor(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char sz[cchSzDef];
	int i, j, k, l;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		for (i = 0; i < 4; i++)
			SetEditColor(hdlg, dce0 + i, kElemA[i]);
		for (i = 1; i <= cAspect; i++)
			SetEditColor(hdlg, dca01 + i - 1, kAspA[i]);
		for (i = 0; i < cColor; i++)
		{
			j = ikPalette[i];
			SetEditColor(hdlg, dck00 + i, j <= 0 ? kMainA[-j] : kRainbowA[j]);
		}
		SetEditColor(hdlg, dcs, wi.kiPen);
		SetEditColor(hdlg, dei01, kCornerDecoration);
		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			for (k = 0; k <= 1; k++)
			{
				for (i = 0; i < 4; i++)
				{
					GetEdit(dce0 + i, sz);
					l = NParseSz(sz, pmColor);
					if (k)
						kElemA[i] = l;
					else
						EnsureN(l, FValidColor(l), "element color");
				}
				for (i = 1; i <= cAspect; i++)
				{
					GetEdit(dca01 + i - 1, sz);
					l = NParseSz(sz, pmColor);
					if (k)
						kAspA[i] = l;
					else
						EnsureN(l, FValidColor(l), "aspect color");
				}
				for (i = 0; i < cColor; i++)
				{
					GetEdit(dck00 + i, sz);
					l = NParseSz(sz, pmColor);
					if (k)
					{
						j = ikPalette[i];
						if (j <= 0)
							kMainA[-j] = l;
						else
							kRainbowA[j] = l;
					}
					else
						EnsureN(l, FValidColor(l), "palette color");
				}

				GetEdit(dcs, sz);
				l = NParseSz(sz, pmColor);
				if (k)
					wi.kiPen = l;
				else
					EnsureN(l, FValidColor(l), "scribble color");

				GetEdit(dei01, sz);
				l = NParseSz(sz, pmColor);
				if (k)
					kCornerDecoration = l;
				else
					EnsureN(l, FValidColor(l), "corner decoration color");
			}
			wi.fRedraw = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the chart data entry dialog, as brought up by  */
/* both the Chart Data commands in the Edit menu. */

int CALLBACK DlgChartData(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CI		ci;
	char	sz[cchSzDef];
	WORD	wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hdlg, pctChartData, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
			(LPARAM)LoadBitmap(wi.hinst, MAKEINTRESOURCE(IDB_BITMAP6)));
		if (wi.nDlgChart < 2)
		{
			ci = ciMain;
		}
		else
		{
			sprintf(sz, Lang(sENTER_CHART_INFO), wi.nDlgChart);
			SetWindowText(hdlg, sz);
			if (wi.nDlgChart == 2)
				ci = ciTwin;
			else if (wi.nDlgChart == 3)
				ci = ciThre;
			else
				ci = ciFour;
		}

		SetEditMDYT(hdlg, dcInMon, dcInDay, dcInYea, dcInTim, ci.mon, ci.day, ci.yea, ci.tim);
		SetEditSZOA(hdlg, dcInDst, dcInZon, dcInLon, dcInLat, deInLoc, 
			ci.dst, ci.zon, ci.lon, ci.lat, ci.loc);
		SetEditSz(hdlg, deInNam, ci.nam);
		SetFocus(GetDlgItem(hdlg, dcInMon));
		return FALSE;

	case WM_COMMAND:
		switch(wid)
		{
		case dbAtlas:
			if(DialogBoxParam(Lang.resource, MAKEINTRESOURCE(dlgAtlas), 
				wi.hwnd, DlgAtlas, (LPARAM) &ci))	// success and OK
			{
				BOOL	fT;

				SetEdit(deInLoc, ci.loc);
				if(*ci.nam)
				{
					SetEdit(deInNam, ci.nam);
				}
				fT = us.fAnsiChar;
				us.fAnsiChar = FALSE;
				sprintf(sz, "%s", SzLocation(ci.lon, ci.lat));
				us.fAnsiChar = fT;
				sz[10] = chNull;
				SetEditSz(hdlg, dcInLon, &sz[0]);
				SetEditSz(hdlg, dcInLat, &sz[11]);
			}
			break;

		case dbInNow:
		case dbInSet:
			if (wid == dbInNow)
			{
				// we set every parameter except the name (the current one will be kept)
				SetHereAndNow(&ci);
			}
			else
			{
				ci = ciSave;

				// we write to the dialogue box the saved name, the remaining 
				// parameters are written below
				SetEditSz(hdlg, deInNam, ci.nam);
			}

			SetEditMDYT(hdlg, dcInMon, dcInDay, dcInYea, dcInTim, ci.mon, ci.day, ci.yea, ci.tim);
			SetEditSZOA(hdlg, dcInDst, dcInZon, dcInLon, dcInLat, deInLoc, 
				ci.dst, ci.zon, ci.lon, ci.lat, ci.loc);
			SetFocus(GetDlgItem(hdlg, IDOK));
			break;

		case IDOK:
			GetEdit(dcInMon, sz);
			ci.mon = NParseSz(sz, pmMon);
			GetEdit(dcInDay, sz);
			ci.day = NParseSz(sz, pmDay);
			GetEdit(dcInYea, sz);
			ci.yea = NParseSz(sz, pmYea);
			GetEdit(dcInTim, sz);
			ci.tim = RParseSz(sz, pmTim);
			GetEdit(dcInDst, sz);
			ci.dst = RParseSz(sz, pmDst);
			GetEdit(dcInZon, sz);
			ci.zon = RParseSz(sz, pmZon);
			GetEdit(dcInLon, sz);
			ci.lon = RParseSz(sz, pmLon);
			GetEdit(dcInLat, sz);
			ci.lat = RParseSz(sz, pmLat);
			EnsureN(ci.mon, FValidMon(ci.mon), "month");
			EnsureN(ci.yea, FValidYea(ci.yea), "year");
			EnsureN(ci.day, FValidDay(ci.day, ci.mon, ci.yea), "day");
			EnsureR(ci.tim, FValidTim(ci.tim), "time");
			EnsureR(ci.dst, FValidZon(ci.dst), "daylight");
			EnsureR(ci.zon, FValidZon(ci.zon), "zone");
			EnsureR(ci.lon, FValidLon(ci.lon), "longitude");
			EnsureR(ci.lat, FValidLat(ci.lat), "latitude");
			GetEdit(deInNam, sz);
			strcpy(ci.nam, sz);
			GetEdit(deInLoc, sz);
			strcpy(ci.loc, sz);

			switch (wi.nDlgChart)
			{
			case 1:
				ciMain = ciCore = ci;
				break;
			case 2:
				ciTwin = ci;
				break;
			case 3:
				ciThre = ci;
				break;
			default:
				ciFour = ci;
				break;
			}

			wi.fCast = TRUE;
			break;

		default:
			break;
		}

		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;

	default:
		break;
	}

	return FALSE;
}


/* Processing function for the default chart info dialog, as brought up */
/* with the Info Default Chart Info menu command.                       */

int CALLBACK DlgDefault(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CI ci;
	char sz[cchSzDef];
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetEditSZOA(hdlg, dcDeDst, dcDeZon, dcDeLon, dcDeLat, dcDeLocName,
			us.dstDef, us.zonDef, us.lonDef, us.latDef, us.szLocNameDef);
		SetFocus(GetDlgItem(hdlg, dcDeDst));
		return FALSE;

	case WM_COMMAND:
		if (wid == dbAtlas)
		{
			if(DialogBoxParam(Lang.resource, MAKEINTRESOURCE(dlgAtlas), 
				wi.hwnd, DlgAtlas, (LPARAM) &ci))	// success and OK
			{
				BOOL	fT;

				SetEdit(dcDeLocName, ci.loc);
				fT = us.fAnsiChar;
				us.fAnsiChar = FALSE;
				sprintf(sz, "%s", SzLocation(ci.lon, ci.lat));
				us.fAnsiChar = fT;
				sz[10] = chNull;
				SetEditSz(hdlg, dcDeLon, &sz[0]);
				SetEditSz(hdlg, dcDeLat, &sz[11]);
			}
		}
		if (wid == IDOK)
		{
			GetEdit(dcDeDst, sz);
			ci.dst = RParseSz(sz, pmDst);
			GetEdit(dcDeZon, sz);
			ci.zon = RParseSz(sz, pmZon);
			GetEdit(dcDeLon, sz);
			ci.lon = RParseSz(sz, pmLon);
			GetEdit(dcDeLat, sz);
			ci.lat = RParseSz(sz, pmLat);
			EnsureR(ci.dst, FValidZon(ci.dst), "daylight");
			EnsureR(ci.zon, FValidZon(ci.zon), "zone");
			EnsureR(ci.lon, FValidLon(ci.lon), "longitude");
			EnsureR(ci.lat, FValidLat(ci.lat), "latitude");
			us.dstDef = ci.dst;
			us.zonDef = ci.zon;
			us.lonDef = ci.lon;
			us.latDef = ci.lat;
			GetEdit(dcDeLocName, sz);
			us.szLocNameDef = SzPersist(sz);
			wi.fCast = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

/* Processing function for the aspect settings dialog, as brought up with */
/* the Setting Aspect Settings menu command.                              */

int CALLBACK DlgAspect(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	double r, r1, r2, r3, r4;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		for (i = 1; i <= cAspect; i++)
		{
			SetCheck(dxa01 - 1 + i, !ignoreA[i]);
			SetEdit(dxa01 - 1 + i, Lang(tAspectName, i));
			SetEditR(hdlg, deo01 - 1 + i, rAspOrb[i], 2);
			SetEditR(hdlg, dea01 - 1 + i, rAspAngle[i], 6);
			SetEditR(hdlg, dei01 - 1 + i, rAspInf[i], 3);
		}

		SetCheck(dx01, us.fAppSep);
		SetCheck(dxCh_a0, us.fAspSummary);
		SetCheck(dxOb_YC, us.fSmartAspects);

		SetEditR(hdlg, deCh_YOc, 100.0 / CoeffComp, 5);
		SetEditR(hdlg, deCh_YOp, 100.0 / CoeffPar, 5);
		SetEditR(hdlg, deYOP, PowerPar * 100.0, 2);
		SetEditR(hdlg, deYPp, SolidAspect, 2);
		return TRUE;

	case WM_COMMAND:
		switch (wid)
		{
		case dbAs_RA0: // select all
			for (i = 1; i <= cAspect; i++)
				SetCheck(dxa01 - 1 + i, TRUE);
			break;
		case dbAs_RA1: // unselect all
			for (i = 1; i <= cAspect; i++)
				SetCheck(dxa01 - 1 + i, FALSE);
			break;
		}

		if (wid == IDOK)
		{
			r1 = GetEditR(hdlg, deCh_YOc);
			r2 = GetEditR(hdlg, deCh_YOp);
			r3 = GetEditR(hdlg, deYOP);
			r4 = GetEditR(hdlg, deYPp);
			EnsureR(r1, FValidCoeffComp(r1), "Comparison orbs reduction");
			EnsureR(r2, FValidCoeffComp(r2), "Parallels orbs reduction");
			EnsureR(r3, FValidCoeffComp(r3), "Parallels power");
			EnsureR(r4, FValidCoeffComp(r4), "Power to be solid");
			CoeffComp = 100.0 / r1;
			CoeffPar = 100.0 / r2;
			PowerPar = r3 / 100.0;
			SolidAspect = r4;

			for (j = 0; j <= 1; j++)
			{
				// validate data on first iteration, set variables on second

				for (i = 1; i <= cAspect; i++)
				{
					r = GetEditR(hdlg, deo01 - 1 + i);
					if (j)
						rAspOrb[i] = r;
					else
						EnsureR(r, r >= 0 && r <= 360.0, "orb");
				}
				for (i = 1; i <= cAspect; i++)
				{
					r = GetEditR(hdlg, dea01 - 1 + i);
					if (j)
						rAspAngle[i] = r;
					else
						EnsureR(r, r >= 0 && r <= 360.0, "angle");
				}
				for (i = 1; i <= cAspect; i++)
				{
					r = GetEditR(hdlg, dei01 - 1 + i);
					if (j)
						rAspInf[i] = r;
				}
			}

			for (i = 1; i <= cAspect; i++)
			{
				ignoreA[i] = !GetCheck(dxa01 - 1 + i);
			}

			us.fAppSep = GetCheck(dx01);
			us.fAspSummary = GetCheck(dxCh_a0);
			us.fSmartAspects = GetCheck(dxOb_YC);
			wi.fRedraw = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

/* Processing function for the object settings dialog, as brought up with */
/* the Setting Object Settings menu command.                              */

int CALLBACK DlgObject(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	double r;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		for (i = 1; i <= oCore; i++)
		{
			SetEditR(hdlg, deo01 - 1 + i, rObjOrb[i], 2);
			SetEditR(hdlg, dea01 - 1 + i, rObjAdd[i], 1);
			SetEditR(hdlg, dei01 - 1 + i, rObjInf[i], 2);
		}
		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			for (j = 0; j <= 1; j++)
			{
				for (i = 1; i <= oCore; i++)
				{
					r = GetEditR(hdlg, deo01 - 1 + i);
					if (j)
						rObjOrb[i] = r;
					else
						EnsureR(r, r >= -360.0 && r <= 360.0, "max orb");
				}
				for (i = 1; i <= oCore; i++)
				{
					r = GetEditR(hdlg, dea01 - 1 + i);
					if (j)
						rObjAdd[i] = r;
					else
						EnsureR(r, r >= -360.0 && r <= 360.0, "orb addition");
				}
				for (i = 1; i <= oCore; i++)
				{
					r = GetEditR(hdlg, dei01 - 1 + i);
					if (j)
						rObjInf[i] = r;
				}
			}
			wi.fRedraw = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the cuap and uranian object settings dialog, as */
/* brought up with the Setting More Object Settings menu command.          */
int CALLBACK DlgObject2(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	double r;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		for (i = oAsc; i <= uranHi; i++)
		{
			SetEditR(hdlg, deo01 - oAsc + i, rObjOrb[i], 2);
			SetEditR(hdlg, dea01 - oAsc + i, rObjAdd[i], 1);
			SetEditR(hdlg, dei01 - oAsc + i, rObjInf[i], 2);
		}
		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			for (j = 0; j <= 1; j++)
			{
				for (i = oAsc; i <= uranHi; i++)
				{
					r = GetEditR(hdlg, deo01 - oAsc + i);
					if (j)
						rObjOrb[i] = r;
					else
						EnsureR(r, r >= -360.0 && r <= 360.0, "max orb");
				}
				for (i = oAsc; i <= uranHi; i++)
				{
					r = GetEditR(hdlg, dea01 - oAsc + i);
					if (j)
						rObjAdd[i] = r;
					else
						EnsureR(r, r >= -360.0 && r <= 360.0, "orb addition");
				}
				for (i = oAsc; i <= uranHi; i++)
				{
					r = GetEditR(hdlg, dei01 - oAsc + i);
					if (j)
						rObjInf[i] = r;
				}
			}
			wi.fRedraw = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the object restrictions dialog, as invoked with */
/* both the Setting Restrictions and Transit Restrictions menu commands.   */

int CALLBACK DlgObjSelections(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	byte *lpb;
	int i;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		if (wi.wCmd == cmdRes)
			lpb = ignore;
		else if (wi.wCmd == cmdResTransit)
		{
			SetWindowText(hdlg, Lang(sTRANSIT_OBJECT_RESTRICTIONS));
			lpb = ignore2;
		}
		else
		{
			SetWindowText(hdlg, Lang(sPROGRESSED_OBJECT_RESTRICTIONS));
			lpb = ignore3;
		}

		for (i = oEar; i <= cLastMoving; i++)
		{
			SetCheck(dx00 + i, !lpb[i]);
		}
		return TRUE;

	case WM_COMMAND:
		switch (wid)
		{
		case dca01:
			for (i = oEar; i <= cLastMainPlanet; i++)
			{
				SetCheck(dx00 + i, TRUE);
			}
			break;
		case dca02:
			for (i = cLastMainPlanet + 1; i <= oCore; i++)
			{
				SetCheck(dx01 - 1 + i, TRUE);
			}
			break;
		case dca03:
			for (i = cuspLo; i <= cuspHi; i++)
			{
				SetCheck(dx01 - 1 + i, TRUE);
			}
			break;
		case dca04:
			for (i = uranLo; i <= uranHi; i++)
			{
				SetCheck(dx01 - 1 + i, TRUE);
			}
			break;
		case dck01:
			for (i = oEar; i <= cLastMainPlanet; i++)
			{
				SetCheck(dx00 + i, FALSE);
			}
			break;
		case dck02:
			for (i = cLastMainPlanet + 1; i <= oCore; i++)
			{
				SetCheck(dx01 - 1 + i, FALSE);
			}
			break;
		case dck03:
			for (i = cuspLo; i <= cuspHi; i++)
			{
				SetCheck(dx01 - 1 + i, FALSE);
			}
			break;
		case dck04:
			for (i = uranLo; i <= uranHi; i++)
			{
				SetCheck(dx01 - 1 + i, FALSE);
			}
			break;
		}

		if (wid == IDOK)
		{
			if (wi.wCmd == cmdRes)
				lpb = ignore;
			else if (wi.wCmd == cmdResTransit)
				lpb = ignore2;
			else
				lpb = ignore3;

			for (i = oEar; i <= cLastMoving; i++)
				lpb[i] = !GetCheck(dx00 + i);

			wi.fCast = TRUE;
		}

		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the star restrictions dialog, as brought up with */
/* the Setting Star Restrictions menu command.                              */

int CALLBACK DlgStar(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		for (i = 1; i <= cStar; i++)
		{
			SetCheck(dx01 - 1 + i, !ignore[cLastMoving + i]);
			SetEdit(dx01 - 1 + i, Lang(tObjName, starLo - 1 + i));
		}
		return TRUE;

	case WM_COMMAND:
		switch (wid)
		{
		case dbSt_RU0:
			for (i = 1; i <= cStar; i++)
				SetCheck(dx01 - 1 + i, TRUE);
			break;
		case dbSt_RU1:
			for (i = 1; i <= cStar; i++)
				SetCheck(dx01 - 1 + i, FALSE);
			break;
		}

		if (wid == IDOK)
		{
			for (i = 1; i <= cStar; i++)
			{
				ignore[cLastMoving + i] = !GetCheck(dx01 - 1 + i);
			}

			if (!us.nStar)
			{
				for (i = starLo; i <= starHi; i++)
					if (!ignore[i])
					{
						us.nStar = TRUE;
						WiCheckMenu(cmdResStar, TRUE);
						break;
					}
			}
			else
			{
				for (i = starLo; i <= starHi; i++)
					if (!ignore[i])
						break;
				if (i > starHi)
				{
					us.nStar = FALSE;
					WiCheckMenu(cmdResStar, FALSE);
				}
			}
			wi.fCast = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the standard settings dialog, as brought up with */
/* the Setting Calculation Settings menu command.                           */

int CALLBACK DlgCalculationSettings(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char	sz[cchSzDef];
	double	r1, r3;
	int		n4, n5, sid_mode, index;
	WORD	wid = LOWORD(wParam);
	WORD	message_type = HIWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetCheck(dxOb_Yn, us.fTrueNode);
		SetEditR(hdlg, dcSe_s, us.rSiderealCorrection, 6);
		SetEditR(hdlg, deSe_x, us.nHarmonic, 8);
		SetEdit(deSe_h, Lang(tObjName, us.objCenter));
		SetRadio(us.objOnAsc == 0 ? dr01 : (us.objOnAsc > 0 ? dr02 : dr03), dr01, dr03);
		SetEdit(deSe_1, Lang(tObjName, us.objOnAsc == 0 ? oSun : abs(us.objOnAsc)));
		SetCheck(dxSe_sr, us.fEquator);
		SetCheck(dx75, oscLilith);

		for(sid_mode = FIRST_SIDEREAL_MODE; sid_mode <= LAST_SIDEREAL_MODE; sid_mode++)
		{
			index = SendDlgItemMessage(hdlg, dcSe_sm, LB_ADDSTRING, 0, 
				(LPARAM)(LPCSTR) Lang(tAyanamshaName, sid_mode));
			SendDlgItemMessage(hdlg, dcSe_sm, LB_SETITEMDATA, (WPARAM) index, 
				(LPARAM) sid_mode);
		}

		SendDlgItemMessage(hdlg, dcSe_sm, LB_SELECTSTRING, -1, 
			(LPARAM)(LPCSTR) Lang(tAyanamshaName, us.nSiderealMode));
		return TRUE;

	case WM_COMMAND:
		// a double click inside the sidereal mode list box will be equivalent
		// to selecting the clicked mode followed by OK
		if (wid == IDOK || wid == dcSe_sm && message_type == LBN_DBLCLK)
		{
			r1 = GetEditR(hdlg, dcSe_s);
			r3 = GetEditR(hdlg, deSe_x);
			GetEdit(deSe_h, sz);
			n4 = NParseSz(sz, pmObject);
			GetEdit(deSe_1, sz);
			n5 = NParseSz(sz, pmObject);
			EnsureR(r1, FValidOffset(r1), "Sidereal Correction");
			EnsureR(r3, FValidHarmonic(r3), "harmonic factor");
			EnsureN(n4, FValidCenter(n4), "central planet");
			EnsureN(n5, FItem(n5), "Solar chart planet");
			us.rSiderealCorrection = r1;
			us.nHarmonic = r3;
			us.objCenter = n4;
			us.objOnAsc = GetCheck(dr01) ? 0 : (GetCheck(dr02) ? n5 : -n5);
			us.fEquator = GetCheck(dxSe_sr);
			oscLilith = GetCheck(dx75);
			us.fTrueNode = GetCheck(dxOb_Yn);
			WiCheckMenu(cmdHeliocentric, us.objCenter == oSun);

			index = SendDlgItemMessage(hdlg, dcSe_sm, LB_GETCURSEL, 0, 0);
			if(index != LB_ERR)
			{
				us.nSiderealMode = SendDlgItemMessage(hdlg, dcSe_sm, 
					LB_GETITEMDATA, (WPARAM) index, 0);
				swe_set_sid_mode(us.nSiderealMode, 0, 0);
			}

			wi.fCast = TRUE;
		}

		if (wid == IDOK || wid == IDCANCEL || wid == dcSe_sm && message_type == LBN_DBLCLK)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the obscure settings dialog, as brought up with */
/* the Setting Obscure Settings menu command.                              */

int CALLBACK DlgMiscellaneousSettings(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, n1, n2;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hdlg, pctMiscellaneous, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
			(LPARAM)LoadBitmap(wi.hinst, MAKEINTRESOURCE(IDB_BITMAP4)));
		SetCheck(dxOb_Yd, us.fEuroDate);
		SetCheck(dxOb_Yt, us.fEuroTime);
		SetCheck(dxOb_Y8, us.fClip80);
		SetCheck(dxOb_Yc0, us.fHouseAngle);
		SetCheck(dxOb_YR0_s, us.fIgnoreSign);
		SetCheck(dxOb_YR0_d, us.fIgnoreDir);
		SetEditN(deOb_YXg, gs.nGridCell);
		SetCheck(dxOb_YXf, gs.fFont);
		SetEditR(hdlg, deOb_YXp0_x, gs.xInch, 2);
		SetEditR(hdlg, deOb_YXp0_y, gs.yInch, 2);
		SetRadio(gs.nOrient == 0 ? dr11 : (gs.nOrient > 0 ? dr09 : dr10), dr09, dr11);
		for (i = 0; i < 4; i++)
			SetCheck(dx01 + i, ignorez[i]);
		SetCheck(dxOb_Ze, fEquator);

#ifdef INTERPRETALT
		if (InterpretAlt == 0)
			n2 = dr12;
		else if (InterpretAlt == 1)
			n2 = dr13;
		else if (InterpretAlt == 3)
			n2 = dr14;
		else if (InterpretAlt == 4)
			n2 = dr15;
		else if (InterpretAlt == 6)
			n2 = dr18;
		else if (InterpretAlt == 2)
			n2 = dr16;
		else if (InterpretAlt == 5)
			n2 = dr17;
		SetRadio(n2, dr12, dr18);
#endif /* INTERPRETALT */

		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			n1 = GetEditN(deOb_YXg);
			EnsureN(n1, FValidGrid(n1), "grid cell");
			us.fEuroDate = GetCheck(dxOb_Yd);
			us.fEuroTime = GetCheck(dxOb_Yt);
			us.fClip80 = GetCheck(dxOb_Y8);
			us.fHouseAngle = GetCheck(dxOb_Yc0);
			us.fIgnoreSign = GetCheck(dxOb_YR0_s);
			us.fIgnoreDir = GetCheck(dxOb_YR0_d);
			gs.nGridCell = n1;
			gs.fFont = GetCheck(dxOb_YXf);
			gs.xInch = GetEditR(hdlg, deOb_YXp0_x);
			gs.yInch = GetEditR(hdlg, deOb_YXp0_y);
			gs.nOrient = GetCheck(dr11) ? 0 : (GetCheck(dr09) ? 1 : -1);

#ifdef INTERPRETALT
			if (GetCheck(dr12))
				InterpretAlt = 0;
			if (GetCheck(dr13))
				InterpretAlt = 1;
			if (GetCheck(dr14))
				InterpretAlt = 3;
			if (GetCheck(dr15))
				InterpretAlt = 4;
			if (GetCheck(dr16))
				InterpretAlt = 2;
			if (GetCheck(dr17))
				InterpretAlt = 5;
			if (GetCheck(dr18))
				InterpretAlt = 6;
#endif /* INTERPRETALT */

			for (i = 0; i < 4; i++)
				ignorez[i] = GetCheck(dx01 + i);
			fEquator = GetCheck(dxOb_Ze);
			wi.fCast = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the transit chart dialog, as brought up with the */
/* Chart Transits menu command.                                             */

int _stdcall DlgTransit(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char sz[cchSzDef];
	int mon, day, yea, n1, n2;
	double tim;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		if (us.fInDay)
			n1 = 1;
		else if (us.fInDayInf)
			n1 = 2;
		else if (us.fTransit)
			n1 = 3;
		else if (us.fTransitInf)
			n1 = 4;
		else
			n1 = 0;
		SetRadio(dr01 + n1, dr01, dr05);
		SetCheck(dxTr_p, is.fProgressIS);
		SetCheck(dxTr_r, is.fReturn);
		SetEditMDYT(hdlg, dcTrMon, dcTrDay, dcTrYea, dcTrTim, 
			ciTran.mon, ciTran.day, ciTran.yea, ciTran.tim);
		if (n1 == 1)
		{
			n2 = is.fProgressIS || us.fInDayMonth;
			if (n2 == 1 && ciTran.mon == 0)
				n2 += 1 + (us.nEphemYears > 1);
		}
		else if (n1 == 3)
			n2 = 1 + (ciTran.mon <= 0) + (ciTran.mon < 0);
		else
			n2 = 0;
		SetRadio(dr06 + n2, dr06, dr09);
		SetEditN(deTr_tY, us.nEphemYears);
		SetEditN(deTr_d, us.nDivision);
		SetFocus(GetDlgItem(hdlg, dcTrMon));
		return FALSE;

	case WM_COMMAND:
		if (wid == dbTr_tn)
		{
			GetTimeNow(&mon, &day, &yea, &tim, us.zonDef - us.dstDef);
			SetEditMDYT(hdlg, dcTrMon, dcTrDay, dcTrYea, dcTrTim, mon, day, yea, tim);
		}

		if (wid == IDOK)
		{
			GetEdit(dcTrMon, sz);
			mon = NParseSz(sz, pmMon);
			GetEdit(dcTrDay, sz);
			day = NParseSz(sz, pmDay);
			GetEdit(dcTrYea, sz);
			yea = NParseSz(sz, pmYea);
			GetEdit(dcTrTim, sz);
			tim = RParseSz(sz, pmTim);
			n1 = GetEditN(deTr_tY);
			n2 = GetEditN(deTr_d);
			EnsureN(mon, FValidMon(mon), "month");
			EnsureN(yea, FValidYea(yea), "year");
			EnsureN(day, FValidDay(day, mon, yea), "day");
			EnsureR(tim, FValidTim(tim), "time");
			EnsureN(n2, FValidDivision(n2), "searching divisions");
			SetCI(&ciTran, mon, day, yea, tim, us.dstDef, us.zonDef, us.lonDef, us.latDef, us.szLocNameDef);
			us.nEphemYears = n1;
			us.nDivision = n2;
			is.fProgressIS = GetCheck(dxTr_p);
			is.fReturn = GetCheck(dxTr_r);
			n1 = GetCheck(dr01) ? 0 : (GetCheck(dr02) ? 1 : (GetCheck(dr03) ? 2 : (GetCheck(dr04) ? 3 : 4)));
			switch (n1)
			{
			case 1:
				wi.nMode = gTraTraHit;
				break;
			case 2:
				wi.nMode = gTraTraInf;
				break;
			case 3:
				wi.nMode = gTraNatHit;
				break;
			case 4:
				wi.nMode = gTraNatInf;
				break;
			default:
				wi.nMode = gWheel;
			}
			n2 = GetCheck(dr06) ? 0 : (GetCheck(dr07) ? 1 : (GetCheck(dr08) ? 2 : 3));
			if (n1 == 1)
			{
				us.fInDayMonth = (is.fProgressIS || n2 >= 1);
				if (n2 >= 2)
				{
					ciTran.mon = 0;
					if (n2 == 2)
						us.nEphemYears = 1;
				}
			}
			else if (n1 == 3)
			{
				if (n2 == 2)
					ciTran.mon = 0;
				else if (n2 == 3)
				{
					ciTran.mon = -1;
					ciTran.day = us.nEphemYears;
				}
			}
			else if (n1 == 2)
			{
				us.fProgressUS = is.fProgressIS;
				if (us.fProgressUS)
				{
					wi.fInitProgression = TRUE;
				}
				wi.fCast = TRUE;
			}
			us.fGraphics = FALSE;
			wi.fRedraw = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the progression settings dialog, as brought up */
/* with the Chart Progressions menu command.                              */

int CALLBACK DlgProgress(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char sz[cchSzDef];
	int mon, day, yea;
	double tim, r1;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hdlg, pctProgressions, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
			(LPARAM)LoadBitmap(wi.hinst, MAKEINTRESOURCE(IDB_BITMAP7)));
		SetCheck(dxPr_p, us.fProgressUS);
		SetEditMDYT(hdlg, dcPrMon, dcPrDay, dcPrYea, dcPrTim, 
			ciTran.mon, ciTran.day, ciTran.yea, ciTran.tim);
		SetRadio(dr01 + us.fSolarArc, dr01, dr03);
		SetEditR(hdlg, dcPr_pd, us.rProgDay, 5);
		SetCombo(dcPr_pd, "365.2422");
		SetCombo(dcPr_pd, "365.25636");
		SetCombo(dcPr_pd, "27.321582");
		SetCombo(dcPr_pd, "27.321661");
		SetCombo(dcPr_pd, "29.530588");
		SetFocus(GetDlgItem(hdlg, dcPrMon));
		return FALSE;

	case WM_COMMAND:
		if (wid == dbPr_pn)
		{
			GetTimeNow(&mon, &day, &yea, &tim, us.zonDef - us.dstDef);
			SetEditMDYT(hdlg, dcPrMon, dcPrDay, dcPrYea, dcPrTim, mon, day, yea, tim);
		}

		if (wid == IDOK)
		{
			GetEdit(dcPrMon, sz);
			mon = NParseSz(sz, pmMon);
			GetEdit(dcPrDay, sz);
			day = NParseSz(sz, pmDay);
			GetEdit(dcPrYea, sz);
			yea = NParseSz(sz, pmYea);
			GetEdit(dcPrTim, sz);
			tim = RParseSz(sz, pmTim);
			r1 = GetEditR(hdlg, dcPr_pd);
			EnsureN(mon, FValidMon(mon), "month");
			EnsureN(yea, FValidYea(yea), "year");
			EnsureN(day, FValidDay(day, mon, yea), "day");
			EnsureR(tim, FValidTim(tim), "time");
			EnsureR(r1, r1 != 0.0, "degree per day");
			SetCI(&ciTran, mon, day, yea, tim, us.dstDef, us.zonDef, us.lonDef, 
				us.latDef, us.szLocNameDef);
			us.rProgDay = r1;
			us.fProgressUS = GetCheck(dxPr_p);
			CheckMenu(cmdProgress, us.fProgressUS);
			if (us.fProgressUS)
			{
				wi.fInitProgression = TRUE;
				SetRel(rcNone);
			}
			us.fSolarArc = GetCheck(dr02);
			if (us.fSolarArc == FALSE)
			{
				us.fSolarArc = GetCheck(dr03);
				if (us.fSolarArc)
					us.fSolarArc = 2;
			}
			is.JDp = MdytszToJulian(ciTran.mon, ciTran.day, ciTran.yea, ciTran.tim, us.dstDef, us.zonDef);
			wi.fCast = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

int CALLBACK DlgArabicPartsSettings(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int n2, n3, n5, n6;
	double r1;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetCheck(dxCh_P0, us.fArabicFlip);
		SetEditN(deCh_P, us.nArabicParts);
		SetEditN(deCh_YPa, PartAspects);
		SetEditN(deCh_YPs, PartAspectsShow);
		SetEditR(hdlg, deCh_YPo, PartOrb, 5);

		switch (us.nArabic)
		{
		case 'z':
			n2 = dr08;
			break;
		case 'n':
			n2 = dr09;
			break;
		case 'f':
			n2 = dr10;
			break;
		default:
			n2 = dr07;
		}

		SetRadio(n2, dr07, dr10);
		SetRadio(dr15 + us.nArabicNight + 1, dr15, dr17);
		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			n3 = GetEditN(deCh_P);
			n5 = GetEditN(deCh_YPa);
			n6 = GetEditN(deCh_YPs);
			r1 = GetEditR(hdlg, deCh_YPo);
			EnsureN(n3, FValidPart(n3), "Arabic part");
			EnsureN(n5, FValidStarAspects(n5), "Arabic Parts aspects number");
			EnsureN(n6, FValidStarAspects(n6), "Arabic Parts aspects to show");
			EnsureR(r1, FValidStarOrb(r1), "Orbs of Arabic Parts");
			us.nArabicParts = n3;
			PartAspects = n5;
			PartAspectsShow = n6;
			PartOrb = r1;

			us.fArabicFlip = GetCheck(dxCh_P0);

			if (us.nArabic)
				us.nArabic = GetCheck(dr08) ? 'z' : (GetCheck(dr09) ? 'n' : (GetCheck(dr10) ? 'f' : TRUE));
			us.nArabicNight = GetCheck(dr15) ? -1 : (GetCheck(dr16) ? 0 : 1);
			wi.fCast = TRUE;
		}

		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the chart subsettings dialog, as brought up with */
/* the Chart Chart Settings menu command.                                   */

int CALLBACK DlgChartSettings(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int n1, n2, n3, n4, n6;
	double r2, r3;
	BOOL f;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetEditN(deSe_I, us.nScreenWidth);
		SetRadio(us.nDegForm == 0 ? dr24 : (us.nDegForm == 1 ? dr25 : dr26), dr24, dr26);
		SetCheck(dxCh_v0, us.fVelocity);
		SetEditN(deCh_w, us.nWheelRows);
		SetCheck(dxCh_w0, us.fWheelReverse);
		SetCheck(dxCh_g0, us.fGridConfig);
		SetCheck(dxCh_m0, us.fMidSummary);
		SetCheck(dxCh_ma, us.fMidAspect);
		SetCheck(dxCh_Z0, us.fPrimeVert);
		SetCheck(dxCh_l, us.fSectorApprox);
		SetCheck(dxCh_j0, us.fInfluenceSign);
		SetEditN(deCh_L, us.nAstroGraphStep);
		SetCheck(dxCh_L0, us.fLatitudeCross);
		SetCheck(dxCh_YU, StarRest);
		SetCheck(dx01, fDisp);
		SetEditN(deCh_YUa, StarAspects);
		SetEditR(hdlg, deCh_YUo, StarOrb, 5);
		SetEditR(hdlg, deCh_YXc, CoeffTens, 5);

		if (TensChart == 1)
			n3 = dr12;
		else if (TensChart == -1)
			n3 = dr13;
		else if (TensChart == 2)
			n3 = dr14;
		else
			n3 = dr11;
		SetRadio(n3, dr11, dr14);

		switch (us.nStar)
		{
		case 'z':
			n1 = dr02;
			break;
		case 'l':
			n1 = dr03;
			break;
		case 'n':
			n1 = dr04;
			break;
		case 'b':
			n1 = dr05;
			break;
		case 'p':
			n1 = dr06;
			break;
		default:
			n1 = dr01;
		}
		SetRadio(n1, dr01, dr06);
		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			us.nDegForm = GetCheck(dr24) ? 0 : (GetCheck(dr25) ? 1 : 2);
			n1 = GetEditN(deCh_w);
			n2 = GetEditN(deCh_L);
			n4 = GetEditN(deCh_YUa);
			EnsureN(n1, FValidWheel(n1), "wheel row");
			EnsureN(n2, FValidAstrograph(n2), "astro-graph step");
			EnsureN(n4, FValidStarAspects(n4), "star aspects number");
			f = GetCheck(dxCh_v0);
			if (us.fVelocity != f)
			{
				us.fVelocity = f;
				WiCheckMenu(cmdGraphicsSidebar, !f);
			}
			us.nWheelRows = n1;
			us.fWheelReverse = GetCheck(dxCh_w0);
			us.fGridConfig = GetCheck(dxCh_g0);
			us.fMidSummary = GetCheck(dxCh_m0);
			us.fMidAspect = GetCheck(dxCh_ma);
			us.fPrimeVert = GetCheck(dxCh_Z0);
			us.fSectorApprox = GetCheck(dxCh_l);
			us.fInfluenceSign = GetCheck(dxCh_j0);
			us.nAstroGraphStep = n2;
			us.fLatitudeCross = GetCheck(dxCh_L0);
			fDisp = GetCheck(dx01);
			StarRest = GetCheck(dxCh_YU);
			StarAspects = n4;
			n6 = GetEditN(deSe_I);
			EnsureN(n6, FValidScreen(n6), "text columns");
			us.nScreenWidth = n6;
			r2 = GetEditR(hdlg, deCh_YUo);
			r3 = GetEditR(hdlg, deCh_YXc);
			EnsureR(r2, FValidStarOrb(r2), "Orbs of stars");
			EnsureR(r3, FValidCoeffTens(r3), "Flower's scale");
			StarOrb = r2;
			CoeffTens = r3;
			if (GetCheck(dr11))
				TensChart = 0;
			if (GetCheck(dr12))
				TensChart = 1;
			if (GetCheck(dr13))
				TensChart = -1;
			if (GetCheck(dr14))
				TensChart = 2;

			if (us.nStar)
				us.nStar = GetCheck(dr02) ? 'z' : (GetCheck(dr03) ? 'l' :
					   (GetCheck(dr04) ? 'n' : (GetCheck(dr05) ? 'b' : (GetCheck(dr06) ? 'p' : TRUE))));
			wi.fCast = TRUE;
		}
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the graphic settings dialog, as brought up with */
/* the Graphics Graphics Settings menu command.                            */

int CALLBACK DlgGraphics(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char sz[cchSzDef];
	int n1, n2, n3, n5, n6, n7, n8;
	double r4;
	WORD wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetEditN(deGr_Xw_x, gs.xWin);
		SetEditN(deGr_Xw_y, gs.yWin);
		SetEditN(deGr_XW, gs.nRot);
		SetEditR(hdlg, deGr_XG, gs.rTilt, 2);
		SetCheck(dxGr_XW0, gs.fMollewide);
		SetEditN(deGr_WN, wi.nTimerDelay);
		SetRadio(gs.objLeft > 0 ? dr02 : (gs.objLeft < 0 ? dr03 : dr01), dr01, dr03);
		SetRadio(dr07 + CornerDecorationType, dr07, dr09);
		SetEditN(dea01, CornerDecorationSize);
		SetEditN(dea02, CornerDecorationNumberLines);
		SetRadio(dr11 + (gs.nGlyphs / 1000 == 2), dr11, dr12);
		SetRadio(dr13 + ((gs.nGlyphs / 100) % 10 == 2), dr13, dr14);
		SetRadio(dr15 + ((gs.nGlyphs / 10) % 10 == 2), dr15, dr16);
		SetRadio(dr20 - 1 + gs.nGlyphs % 10, dr20, dr22);
		SetEdit(deGr_X1, Lang(tObjName, gs.objLeft == 0 ? oSun : abs(gs.objLeft)));
		SetCheck(dxGr_Wn, wi.fNoUpdate);
		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			n1 = GetEditN(deGr_Xw_x);
			n2 = GetEditN(deGr_Xw_y);
			n3 = GetEditN(deGr_XW);
			r4 = GetEditR(hdlg, deGr_XG);
			n5 = GetEditN(deGr_WN);
			GetEdit(deGr_X1, sz);
			n6 = NParseSz(sz, pmObject);
			n7 = GetEditN(dea01);
			n8 = GetEditN(dea02);
			EnsureN(n1, FValidGraphx(n1), "horizontal size");
			EnsureN(n2, FValidGraphy(n2), "vertical size");
			EnsureN(n3, FValidRotation(n3), "horizontal rotation");
			EnsureR(r4, FValidTilt(r4), "vertical tilt");
			EnsureN(n5, FValidTimer(n5), "animation delay");
			EnsureN(n7, n7 > 0 && n7 <= 100, "Corner Decoration Size");
			EnsureN(n8, n8 > 0 && n8 <= 500, "Corner Decoration Number of Lines");

			if (gs.xWin != n1 || gs.yWin != n2)
			{
				gs.xWin = n1;
				gs.yWin = n2;
				if (wi.fWindowChart)
					ResizeWindowToChart();
			}

			gs.nRot = n3;
			gs.rTilt = r4;

			if (wi.nTimerDelay != (UINT) n5)
			{
				wi.nTimerDelay = n5;
				if (wi.nTimer != 0)
					KillTimer(wi.hwnd, 1);
				wi.nTimer = SetTimer(wi.hwnd, 1, wi.nTimerDelay, NULL);
			}

			gs.objLeft = GetCheck(dr01) ? 0 : (GetCheck(dr02) ? n6 : -n6);
			CornerDecorationSize = n7;
			CornerDecorationNumberLines = n8;
			gs.fMollewide = GetCheck(dxGr_XW0);
			wi.fNoUpdate = GetCheck(dxGr_Wn);
			us.fGraphics = wi.fRedraw = TRUE;

			gs.nGlyphs = (GetCheck(dr11) ? 1 : 2) * 1000 +
				(GetCheck(dr13) ? 1 : 2) * 100 + (GetCheck(dr15) ? 1 : 2) * 10;

			if(GetCheck(dr20))
				gs.nGlyphs += 1;
			else if(GetCheck(dr21))
				gs.nGlyphs += 2;
			else
				gs.nGlyphs += 3;

			if(GetCheck(dr07))
			{
				CornerDecorationType = 0;
			}
			else if(GetCheck(dr08))
			{
				CornerDecorationType = 1;
			}
			else
			{
				CornerDecorationType = 2;
			}
		}

		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/* Processing function for the about dialog, showing copyrights and  */
/* credits, as brought up with the Help About Astrolog menu command. */

int CALLBACK DlgAbout(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD	wid = LOWORD(wParam);
	int		day, month, year;
	char	build_date[200], credits[5000];
	char	szAppVer[1024];

	sprintf(szAppVer, Lang(sAPP_VER), szAppNameCore, szVersionCore);
	switch (message)
	{
	case WM_INITDIALOG:

		SendDlgItemMessage(hdlg, pctAbout, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
			(LPARAM)LoadBitmap(wi.hinst, MAKEINTRESOURCE(IDB_BITMAP2)));

		SetEdit(dbIa_i1, szAppVer);

		// find which month we are in at build time
		for(month = 1; month <= NUMBER_OF_MONTHS; month++)
		{
			if(!strncmp(szMonthEnglish[month], __DATE__, 3))
			{
				break;
			}
		}

		// create the build date string
		sscanf(__DATE__, "%*s %d %d", &day, &year);  
		strcpy(build_date, SzDate(month, day, year, 1));

		sprintf(credits, Lang(sABOUT), build_date);
		// write out the text in the dialogue box
		SetEdit(dea07, credits);
		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK || wid == IDCANCEL)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

/* wdialog.c */
