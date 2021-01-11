/*
** Astrolog File: wdriver.c
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
#include "shellapi.h"
#include "objbase.h"		// for CoInitializeEx
#include "gui.h"
#include <iostream>
#include <strstream>
Language Lang;


/*
******************************************************************************
** Windows Command Processing.
******************************************************************************
*/


bool LoadLanguagePack(void) {

	// Obtain the DLL path
	char buff[MAX_PATH + 1];
	GetModuleFileName(NULL, buff, MAX_PATH);
	char* lastBS = strrchr(buff, '\\');
	if(lastBS) {
		strcpy(lastBS, "\\language.dll"); 
	}
	else return false;

	// Load the DLL
	switch(Lang.LoadLanguage(buff, "ASTRLG32", 1, 1)) {
		case leOk: 
			break;
		case leNotFound:
			PrintError("Language resource not found or valid. Expected: %s", buff);
			return false;
		default:
			PrintError("Language resource is not compatible with this version of Astrolog.");
			return false;
	}

	// Load strings
	Lang.stringsOffset = sFirst;
	int res = Lang.LoadStrings(sFirst, sLast);
	if(res) {
		PrintError("Missing (plain) resource string %d", res);
		return false;
	}

	// Load tables
	Lang.tablesOffset = tFirst;
	res = Lang.LoadTables(tFirst, tLast);
	if(res) {
		PrintError("Missing table resource string %d", res);
		return false;
	}

	// Test sizes of tables to provide extra safety
	#define TABLE_CHECK(TAB, COUNT) if((Lang.tables.GetItem((TAB) - Lang.tablesOffset))->size() != (COUNT)) {PrintError("Invalid number of items in table: %s. Expected %d, obtained %d", #TAB, COUNT, (Lang.tables.GetItem((TAB) - Lang.tablesOffset))->size()); return false;};
	
	TABLE_CHECK(tSignName, NUMBER_OF_SIGNS + 1);
	TABLE_CHECK(tSignAbbrev, NUMBER_OF_SIGNS + 1);
	TABLE_CHECK(tObjName, NUMBER_OBJECTS);
	TABLE_CHECK(tObjShortName, NUMBER_OBJECTS);
	TABLE_CHECK(tAspectName, cAspect + 1);
	TABLE_CHECK(tAspectAbbrev, cAspect + 1);
	TABLE_CHECK(tAspectConfig, cAspConfig + 1);
	TABLE_CHECK(tHouseSystem, NUMBER_OF_HOUSE_SYSTEMS);
	TABLE_CHECK(tElement, 4);
	TABLE_CHECK(tMode, 3);
	TABLE_CHECK(tAyanamshaName, LAST_SIDEREAL_MODE + 1);
	TABLE_CHECK(tAyanamshaAbbrev, LAST_SIDEREAL_MODE + 1);

	TABLE_CHECK(tMonth, NUMBER_OF_SIGNS + 1);
	TABLE_CHECK(tSuffix, NUMBER_OF_SIGNS + 1);
	TABLE_CHECK(tDay, cWeek);
	TABLE_CHECK(tColor, cColor);
	
	TABLE_CHECK(tSignMeaning, NUMBER_OF_SIGNS + 1);
	TABLE_CHECK(tHouseMeaning, NUMBER_OF_SIGNS + 1);
	TABLE_CHECK(tAspectGlyph, cAspect + 1);
	TABLE_CHECK(tDir, 4);
	TABLE_CHECK(tArabicPart, cPart);
	TABLE_CHECK(tDirNames, 7);
	TABLE_CHECK(tDirPaths, 7);

	// Initialize dir names and the paths
	GetModuleFileName(NULL, buff, MAX_PATH);
	lastBS = strrchr(buff, '\\');
	*lastBS = 0;
	lastBS = strrchr(buff, '\\');
	if(lastBS) {
		*(lastBS + 1) = 0; 
	}
	else {
		// the executable is in the root directory
		strcat(buff, "\\");
	}

	// Set the names
	dirs.main.name = Lang(tDirNames, 0);
	dirs.ephemeris.name = Lang(tDirNames, 1);
	dirs.charts.name = Lang(tDirNames, 2);
	dirs.interpretation.name = Lang(tDirNames, 3);
	dirs.miscel.name = Lang(tDirNames, 4);
	dirs.american_atlas.name = Lang(tDirNames, 5);
	dirs.international_atlas.name = Lang(tDirNames, 6);

	// Set the default paths
	sprintf(dirs.main.dir, "%s%s",  buff, Lang(tDirPaths, 0));
	sprintf(dirs.ephemeris.dir, "%s%s",  buff, Lang(tDirPaths, 1));
	sprintf(dirs.charts.dir, "%s%s",  buff, Lang(tDirPaths, 2));
	sprintf(dirs.interpretation.dir, "%s%s",  buff, Lang(tDirPaths, 3));
	sprintf(dirs.miscel.dir, "%s%s",  buff, Lang(tDirPaths, 4));
	sprintf(dirs.american_atlas.dir, "%s%s",  buff, Lang(tDirPaths, 5));
	sprintf(dirs.international_atlas.dir, "%s%s",  buff, Lang(tDirPaths, 6));
	return	true;
}

/* Change the pixel size of the window so its internal drawable area is the */
/* dimensions of the current graphics chart. Both the upper left and lower  */
/* right corners of the window may change depending on the scroll position. */

void ResizeWindowToChart()
{
	RECT rcOld, rcNew;
	int xScr, yScr;
	HDC hdc;

	if (!us.fGraphics || gs.xWin == 0 || gs.yWin == 0)
		return;
	GetWindowRect(wi.hwnd, &rcOld);
	hdc = GetDC(wi.hwnd);
	xScr = GetDeviceCaps(hdc, HORZRES);
	yScr = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(wi.hwnd, hdc);
	rcNew.left = rcOld.left + gi.xOffset;
	rcNew.top = rcOld.top + gi.yOffset;
	rcNew.right = rcNew.left + gs.xWin + (gi.nCurrChart == 0 ? SIDESIZE : 0) + 24;
	rcNew.bottom = rcNew.top + gs.yWin + 62;
	if (rcNew.right > xScr)
		OffsetRect(&rcNew, xScr - rcNew.right, 0);
	if (rcNew.bottom > yScr)
		OffsetRect(&rcNew, 0, yScr - rcNew.bottom);
	if (rcNew.left < 0)
		OffsetRect(&rcNew, -rcNew.left, 0);
	if (rcNew.top < 0)
		OffsetRect(&rcNew, 0, -rcNew.top);
	MoveWindow(wi.hwnd, rcNew.left, rcNew.top, rcNew.right - rcNew.left, rcNew.bottom - rcNew.top, TRUE);
}

void SetRel(int rc)
{
	CheckMenuRadioItem(wi.hmenu, cmdRelProgressed, cmdRelBiorhythm, 
		rc - rcProgress + cmdRelProgressed, MF_BYCOMMAND);
	us.nRel = rc;
	wi.fCast = TRUE;
	if (us.nRel)
		us.fProgressUS = FALSE;
	CheckMenu(cmdProgress, us.fProgressUS);
}

static void set_os_dependant_parameters(void)
{
	OSVERSIONINFO	osvi;

	// This function was developed originally to set the variable command_interpreter, which
	// is not used anymore. This function will be kept as it may be useful in the future.

	memset(&osvi, 0, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if (GetVersionEx(&osvi)) 
	{
		// "cmd" does not work on W98, etc, and "start" does not work for W2000, 
		// etc, that is why we need this switch.
		switch (osvi.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:	// Windows NT 3.51, NT 4.0, 2000, XP, Server 2003
			command_interpreter = "cmd /c";
			break;
			
		case VER_PLATFORM_WIN32_WINDOWS:	// Windows 95, 98, ME
			command_interpreter = "start";
			break;
			
		default:
			PrintWarning("Unsupported OS, some functionality may not operate properly");
			break;
		}
	}
	else
	{
		PrintWarning("GetVersionEx failed");
	}
}

// save the directory where astrolog32.exe is
static void save_exec_dir(char *lpCmdLine)
{
	char			szFullCommandLine[500];
	char			*pch;
	int				i, last_separator;


	strcpy(szFullCommandLine, GetCommandLine());

	// GetCommandLine() returns the full path plus the parameters. We first
	// discard the parameters. We do that by cutting the string at the point
	// the parameters start.

	szFullCommandLine[strlen(szFullCommandLine) - strlen(lpCmdLine)] = chNull;

	// GetCommandLine returns to us a string enclosed in quotes, we need to 
	// remove them. We also make a note of where the last path separator is.

	for(pch = szFullCommandLine + 1, i = 0; *pch; pch++, i++) 
		// + 1 to discard the first double-quote
	{
		if(*pch == '\\' || *pch == '/')
		{
			// '\' is the usual separator, but in some cases '/' may be used 
			// (I am not completely sure about this).
			last_separator = i;
		}
		dirs.executable.dir[i] = *pch;
	}

	// now discard the executable name (astrolog32.exe) plus the terminating
	// double-quote and space, but keep the last separator
	dirs.executable.dir[last_separator + 1] = chNull;
}

/* The main program, the starting point of Astrolog for Windows, follows.   */
/* This is like the "main" function in standard C. The program initializes  */
/* here, then spins in a tight message processing loop until it terminates. */

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow)
{
	MSG				msg;
	WNDCLASS		wndclass;
	HRESULT			rc;


	// Set up the window class shared by all instances of Astrolog

	wi.hinst = hInstance;
	if (!hPrevInstance)
	{
		memset(&wndclass, 0, sizeof(WNDCLASS));

		wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = wi.hinst;
		wndclass.hIcon = LoadIcon(wi.hinst, MAKEINTRESOURCE(icon));
		wndclass.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		wndclass.lpszMenuName = MAKEINTRESOURCE(menu);
		wndclass.lpszClassName = szAppName;

		if (!RegisterClass(&wndclass))
		{
			PrintError("The window class could not be registered.");
			return -1L;
		}
	}

	// Create the window to be used and drawn on by this instance
	
	//TK
	InitTurtleFont();
	if(!LoadLanguagePack()) return -1;
	char szWindowName[1024];
	sprintf(szWindowName, Lang(sWINDOW_NAME), szAppNameCore, szVersionCore, szSwissEphemerisVersion);

	wi.hmenu = LoadMenu(Lang.resource, MAKEINTRESOURCE(menu));

	wi.hwndMain = CreateWindow(
		szAppName,	// pointer to registered class name
		szWindowName, // pointer to window name
		WS_CAPTION | WS_CLIPCHILDREN | WS_HSCROLL | WS_MAXIMIZE | 
			WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_OVERLAPPED | 
			WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE | 
			WS_VSCROLL,	// window style
		0,	// horizontal position of window
		0,	// vertical position of window
		CW_USEDEFAULT,	// window width
		0,	// window height
		(HWND) NULL,	// handle to parent or owner window
		wi.hmenu,	// handle to menu or child-window identifier
		wi.hinst,	// handle to application instance
		(LPSTR) NULL	// pointer to window-creation data
		);

	if (wi.hwndMain == (HWND) NULL)
	{
		PrintError("The window could not be created.");
		return -1;
	}

	// initialize COM module for SHBrowseForFolder
	if((rc = CoInitialize(NULL)) != S_OK) 
	{
		PrintError("CoInitializeEx failed, returned %d", rc);
		return -1;
	}

	set_os_dependant_parameters(); // the different OSes behave differently
	save_exec_dir(lpCmdLine);	// save the directory where astrolog32.exe is
	SetEphemerisPath();

	// Set up some globals that can't be initialized at compile time

	is.S = stdout;
	ofn.hwndOwner = wi.hwndMain;
	prd.hwndOwner = wi.hwndMain;
//	wi.haccel = LoadAccelerators(wi.hinst, MAKEINTRESOURCE(accelerator));
	wi.haccel = LoadAccelerators(Lang.resource, MAKEINTRESOURCE(accelerator));
	wi.kiPen = gi.kiLite;

	// Start Astrolog32 with a square chart for now at default location and timezone

//	SetHereAndNow(&ciCore);
	wi.fCast = TRUE;
	us.fGraphics = TRUE;
	wi.fRedraw = TRUE;
	RedoMenu();
	SquareX(&gs.xWin, &gs.yWin, TRUE);
	ResizeWindowToChart();
	ShowWindow(wi.hwndMain, nCmdShow);

	// Load language specific configuration from the resource
	HRSRC hIni = FindResource(Lang.resource, MAKEINTRESOURCE(configResource), "TEXT");
	if(hIni) {
		HGLOBAL ini = LoadResource(Lang.resource, hIni);
		if(ini) {
			char* pIni = (char*)LockResource(ini);

			// Now read and execute each line in the ini
			istrstream iniStream(pIni, GlobalSize(pIni));
			char buff[1024];
			iniStream.getline(buff, 1023, '\r');
			while(!iniStream.eof()) {
				//getline( iniStream.getline(buff, 1023, '\r');
				FProcessCommandLine(buff);
				iniStream.read(buff, 1);		// skip \r\n
				iniStream.getline(buff, 1023, '\r');
			}
		}
	}


	// Look for the configuration file. If it can't be found, create a dialogue
	// for initial configuration.

	if(FileFind(CONFIGURATION_FILE, dirs.main.dir, NULL))
	{
		FInputData(CONFIGURATION_FILE, dirs.main.dir);
	}
	else
	{
		if(!us.omitWizard) WelcomeWizard(wi.hinst, wi.hwndMain);
	}

	SetHereAndNow(&ciCore);

	// load default macro file
	if(FileFind(AUTORUN_FILE, dirs.miscel.dir, NULL))
	{
		FInputData(AUTORUN_FILE, dirs.miscel.dir);
	}
	
	// Process the Windows command line parameters

	FProcessCommandLine(lpCmdLine);

	// Initialise chart info structures

	ciSave = ciTran = ciFour = ciThre = ciTwin = ciCore;

	// Redo window 

	RedoMenu();
	wi.fCast = TRUE;
	wi.fRedraw = TRUE;
	SquareX(&gs.xWin, &gs.yWin, TRUE);
	ResizeWindowToChart();
	RedrawWindow(wi.hwnd, NULL, (HRGN) NULL, RDW_INVALIDATE);

	// Set timer for animation

	wi.nTimer = SetTimer(wi.hwnd, 1, wi.nTimerDelay, NULL);


	// Process window messages until the program is told to terminate

	while (GetMessage(&msg, (HWND) NULL, 0, 0))
	{
		if (!TranslateAccelerator(wi.hwndMain, wi.haccel, &msg))
			TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Cleanup and exit Astrolog32

	UnregisterClass(szAppName, wi.hinst);
	CoUninitialize(); 
	return msg.wParam;
}


/* This is the main message processor for the Astrolog window. Given a */
/* user input or other message, do the appropriate action and updates. */

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HPEN hpen, hpenOld;
	HBRUSH hbr;
	RECT rc;
	int x, y;

	wi.hwnd = hwnd;
	switch (msg)
	{
		/* A command, most likely a menu option, was given. */

	case WM_COMMAND:
		switch (NWmCommand(LOWORD(wParam)))
		{
		case 1:
			goto LWM_CLOSE;
		case -1:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		default:
			ProcessState();
		}
		break;

		/* When a part of a window is uncovered, Windows quickly blanks it out, */
		/* usually with white, before having the app redraw that section. Most  */
		/* apps don't do anything here, however Astrolog can quickly draw with  */
		/* a more appropriate color since we know our chart's background color. */

	case WM_ERASEBKGND:
		if (!(wi.fNoUpdate & 1))
		{
			GetClipBox((HDC) wParam, &rc);
			hbr = CreateSolidBrush(rgbbmp[gs.fInverse ? kWhite : kBlack]);
			FillRect((HDC) wParam, &rc, hbr);
			DeleteObject(hbr);
		}
		return TRUE;

		/* The window was just created. Setup the scrollbars. */

	case WM_CREATE:
		DlgAtlas(0, 0, 0, 0);	// pre-initialise dialogue data
		SetScrollRange(hwnd, SB_HORZ, 0, nScrollDiv, FALSE);
		SetScrollRange(hwnd, SB_VERT, 0, 4 * nScrollDiv, FALSE);
		break;

		/* The window has been resized. Change the chart size if need be. */

	case WM_SIZE:
		wi.xClient = LOWORD(lParam);
		wi.yClient = HIWORD(lParam);
		if (wi.fWindowChart || wi.fChartWindow)
		{
			gs.xWin = wi.xClient;
			gs.yWin = wi.yClient;
		}
		break;

		/* All or part of the window needs to be redrawn. Go do so. */

	case WM_PAINT:
		if(wi.fPrinting)
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		else if (!(wi.fNoUpdate & 1))
		{
			FRedraw();
		}
		break;

		/* The mouse has been left clicked or dragged over the window. */

	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:

		/* Treat dragging with the mouse down as a Shift+left click. */
		if (msg == WM_MOUSEMOVE)
		{
			if ((wParam & MK_LBUTTON) == 0 || (wParam & MK_SHIFT) || (wParam & MK_CONTROL))
				break;
			wParam = MK_SHIFT;
		}
		x = WLo(lParam);
		y = WHi(lParam);

		/* Alt+click on a world map chart means relocate the chart there. */
		if (msg == WM_LBUTTONDOWN && GetKeyState(VK_MENU) < 0)
		{
			if (fMap && gs.nRot == 0 && !gs.fConstel && !gs.fMollewide)
			{
				ciMain.lon = DecDeg2DegMin(180.0 - (double) (x - gi.xOffset) / (double) gs.xWin * 360.0);
				if (ciMain.lon < -180.0)
					ciMain.lon = -180.0;
				else if (ciMain.lon > 180.0)
					ciMain.lon = 180.0;
				ciMain.lat = DecDeg2DegMin(90.0 - (double) (y - gi.yOffset) / (double) gs.yWin * 180.0);
				if (ciMain.lat < -90.0)
					ciMain.lat = -90.0;
				else if (ciMain.lat > 90.0)
					ciMain.lat = 90.0;
				wi.xMouse = -1;
				wi.fCast = TRUE;
			}
			break;
		}
		hdc = GetDC(hwnd);
		hpen = CreatePen(PS_SOLID, 0, (COLORREF) rgbbmp[wi.kiPen]);
		hpenOld = (HPEN__ *)SelectObject(hdc, hpen);

		/* Ctrl+click means draw a rectangle. Ctrl+Shift+click does ellipse. */
		if (wParam & MK_CONTROL)
		{
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			if (wParam & MK_SHIFT)
				Ellipse(hdc, wi.xMouse, wi.yMouse, x, y);
			else
				Rectangle(hdc, wi.xMouse, wi.yMouse, x, y);

			/* Shift+click means draw a line from the last to current position. */
		}
		else if (wParam & MK_SHIFT)
		{
			if (wi.xMouse >= 0)
			{
				MoveToEx(hdc, wi.xMouse, wi.yMouse, NULL);
				LineTo(hdc, x, y);
				if (msg == WM_MOUSEMOVE)
				{
					wi.xMouse = x;
					wi.yMouse = y;
				}
			}

			/* A simple click means set a pixel and remember that location. */
		}
		else
		{
			SetPixel(hdc, x, y, (COLORREF) rgbbmp[wi.kiPen]);
			wi.xMouse = x;
			wi.yMouse = y;
		}
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);
		ReleaseDC(hwnd, hdc);
		break;

		/* The mouse has been right clicked on the window. If on a world map  */
		/* or astro-graph chart, relocate the chart to the mouse coordinates. */

	case WM_RBUTTONDOWN:
		x = WLo(lParam);
		y = WHi(lParam);
		if (us.fGraphics && (us.fAstroGraph || gi.nCurrChart == gWorldMap) && gs.nRot == 0 && !gs.fConstel && !gs.fMollewide)
		{
			ciMain.lon = DecDeg2DegMin(180.0 - (double) (x - gi.xOffset) / (double) gs.xWin * 360.0);
			if (ciMain.lon < -180.0)
				ciMain.lon = -180.0;
			else if (ciMain.lon > 180.0)
				ciMain.lon = 180.0;
			ciMain.lat = DecDeg2DegMin(90.0 - (double) (y - gi.yOffset) / (double) gs.yWin * 181.0);
			if (ciMain.lat < -90.0)
				ciMain.lat = -90.0;
			else if (ciMain.lat > 90.0)
				ciMain.lat = 90.0;
			ciCore = ciMain;
			wi.fCast = TRUE;
			ProcessState();
		}
		break;

		/* A timer message is received at a defined regular interval. */

	case WM_TIMER:
		if (wi.fAnimate)
		{
			Animate(gs.nAnimationJumpRate, wi.nAnimationJumpFactor);
			wi.fRedraw = TRUE;
			ProcessState();
		}
		break;

		/* Define the minimum and maximum size the window may be resized to. */

	case WM_GETMINMAXINFO:
		((MINMAXINFO *) lParam)->ptMinTrackSize.x = BITMAPX1;
		((MINMAXINFO *) lParam)->ptMinTrackSize.y = BITMAPY1;
		((MINMAXINFO *) lParam)->ptMaxTrackSize.x = BITMAPX;
		((MINMAXINFO *) lParam)->ptMaxTrackSize.x = BITMAPY;
		break;

		/* The horizontal scrollbar was clicked on or used in some way. */

	case WM_HSCROLL:
		x = wi.xScroll;
		switch (wParam)
		{
		case SB_LINEUP:
			wi.xScroll--;
			break;
		case SB_LINEDOWN:
			wi.xScroll++;
			break;
		case SB_PAGEUP:
			wi.xScroll -= nScrollPage;
			break;
		case SB_PAGEDOWN:
			wi.xScroll += nScrollPage;
			break;
		case SB_THUMBPOSITION:
			wi.xScroll = LOWORD(lParam);
			break;
		default:
			return FALSE;
		}
		wi.xScroll = max(0, min(wi.xScroll, nScrollDiv));
		if (wi.xScroll == x)
			break;
		SetScrollPos(hwnd, SB_HORZ, wi.xScroll, TRUE);
		wi.fRedraw = TRUE;
		ProcessState();
		break;

		/* The vertical scrollbar was clicked on or used in some way. */

	case WM_VSCROLL:
		y = wi.yScroll;
		switch (wParam)
		{
		case SB_LINEUP:
			wi.yScroll--;
			break;
		case SB_LINEDOWN:
			wi.yScroll++;
			break;
		case SB_PAGEUP:
			wi.yScroll -= nScrollPage;
			break;
		case SB_PAGEDOWN:
			wi.yScroll += nScrollPage;
			break;
		case SB_THUMBPOSITION:
			wi.yScroll = LOWORD(lParam);
			break;
		default:
			return FALSE;
		}
		wi.yScroll = max(0, min(wi.yScroll, 4 * nScrollDiv));
		if (wi.yScroll == y)
			break;
		SetScrollPos(hwnd, SB_VERT, wi.yScroll, TRUE);
		wi.fRedraw = TRUE;
		ProcessState();
		break;

		/* The window is being terminated. Clean up and free program data. */

	case WM_CLOSE:
	  LWM_CLOSE:
		if (us.fNoQuit)
		{
			PrintWarning("Program exiting is not allowed now.");
			break;
		}
		if (wi.hpen != (HPEN) NULL)
			DeleteObject(wi.hpen);
		if (wi.nTimer != 0)
			KillTimer(hwnd, 1);
		DestroyWindow(hwnd);
		if (hwnd == wi.hwndMain)
			PostQuitMessage(0);
		break;

		/* Messages not processed here are handled by Windows in a default way. */

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return FALSE;
}

void ClearUserSettings(void)
{
	/* Chart types */

	us.fDebugFont = FALSE;
	us.fListing = FALSE;			
	us.fWheel = FALSE;				
	us.fGrid = FALSE;				
	us.fAspList = FALSE;			
	us.fMidpoint = FALSE;			
	us.fHorizon = FALSE;			
	us.fOrbit = FALSE;				
	us.fSector = FALSE;				
	us.fInfluence = FALSE;			
	us.fAstroGraph = FALSE;			
	us.fCalendar = FALSE;			
	us.fInDay = FALSE;				
	us.fInDayInf = FALSE;			
	us.fEphemeris = FALSE;			
	us.fTransit = FALSE;			
	us.fTransitInf = FALSE;			

	/* Table chart types */

	us.fSwitch = FALSE;				
	us.fSwitchRare = FALSE;			
	us.fKeyGraph = FALSE;			
	us.fSign = FALSE;				
	us.fObject = FALSE;				
	us.fAspect = FALSE;				
	us.fConstel = FALSE;			
	us.fOrbitData = FALSE;			
	us.fMeaning = FALSE;
	us.fPrintProgramStatus = FALSE;
	
	us.nArabic = FALSE;
}

/* This is called after some action has been done that probably changed the */
/* chart state, such as a menu command was run. Update anything needing it. */

void ProcessState()
{
	/* If the chart type was changed, clear all the setting flags, then set */
	/* the appropriate core switch settings based on the new chart type.    */

	if (wi.nMode)
	{
		ClearUserSettings();

		gi.nCurrChart = 0;

		switch (wi.nMode)
		{
		case gBiorhythm:
		case gWheel:
			us.fListing = TRUE;
			break;
		case gHouse:
			us.fWheel = TRUE;
			break;
		case gGrid:
			us.fGrid = TRUE;
			break;
		case gHorizon:
			us.fHorizon = TRUE;
			us.fHorizonSearch = FALSE;
			break;
		case gOrbit:
			us.fOrbit = TRUE;
			break;
		case gSector:
			us.fSector = TRUE;
			break;
		case gAstroGraph:
			us.fAstroGraph = TRUE;
			break;
		case gEphemeris:
			us.fEphemeris = TRUE;
			break;
		case gWorldMap:
			gi.nCurrChart = gWorldMap;
			break;
		case gGlobe:
			gi.nCurrChart = gGlobe;
			break;
		case gPolar:
			gi.nCurrChart = gPolar;
			break;
		case gCalendar:
			us.fCalendar = TRUE;
			break;
		case gDisposit:
			us.fInfluence = TRUE;
			break;
		case gAspect:
			us.fAspList = TRUE;
			break;
		case gMidpoint:
			us.fMidpoint = TRUE;
			break;
		case gArabic:
			us.nArabic = 1;
			break;
		case gSign:
			us.fSign = TRUE;
			break;
		case gObject:
			us.fObject = TRUE;
			break;
		case gHelpAsp:
			us.fAspect = TRUE;
			break;
		case gConstel:
			us.fConstel = TRUE;
			break;
		case gPlanet:
			us.fOrbitData = TRUE;
			break;
		case gMeaning:
			us.fMeaning = TRUE;
			break;
		case gSwitch:
			us.fSwitch = TRUE;
			break;
		case gObscure:
			us.fSwitchRare = TRUE;
			break;
		case gKeystroke:
			us.fKeyGraph = TRUE;
			break;
		case gRising:
			us.fHorizon = us.fHorizonSearch = TRUE;
			break;
		case gTraTraHit:
			us.fInDay = TRUE;
			break;
		case gTraTraInf:
			us.fInDayInf = TRUE;
			break;
		case gTraNatHit:
			us.fTransit = TRUE;
			break;
		case gTraNatInf:
			us.fTransitInf = TRUE;
			break;
		case gProgramStatus:
			us.fPrintProgramStatus = TRUE;
			break;

		case gDebugFont:
			us.fDebugFont = TRUE;
			break;
		}
		wi.nMode = 0;
		wi.fRedraw = TRUE;
		wi.fMenuAll = TRUE;
	}

	if (wi.fMenuAll)
	{							/* Redetermine all menu checks if need be. */
		RedoMenu();
		wi.fMenu = TRUE;
	}
	if (wi.fMenu)
	{							/* Update menu if we added/removed check marks. */
		DrawMenuBar(wi.hwnd);
		wi.fMenu = FALSE;
	}
	if (wi.fCast)				/* Recasting a chart implies redrawing it too. */
		wi.fRedraw = TRUE;

	if (wi.fRedraw)				/* Send the window a redraw message if need be. */
		RedrawWindow(wi.hwnd, NULL, (HRGN) NULL, RDW_INVALIDATE);
}

void create_process(char *command)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;


    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    memset(&pi, 0, sizeof(pi));

    // Start the child process. 
    if(CreateProcess( 
		NULL,				// No module name (use command line). 
        command,			// Command line. 
        NULL,				// Process handle not inheritable. 
        NULL,				// Thread handle not inheritable. 
        FALSE,				// Set handle inheritance to FALSE. 
        DETACHED_PROCESS,  	// Creation flags. 
        NULL,				// Use parent's environment block. 
        NULL,				// Use parent's starting directory. 
        &si,				// Pointer to STARTUPINFO structure.
        &pi)				// Pointer to PROCESS_INFORMATION structure.
    ) 
	{
		// success, no need for the handles, release them
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
    {
		PrintWarning("CreateProcess failed for \"%s\" with error %d.", 
			command, GetLastError()); 
    }
}

void run_command(char *command, char *file)
{
	char command_buffer[300];
	file_name_t file_name;

	if(FileFind(file, dirs.main.dir, file_name))
	{
		if(command)
		{
			sprintf(command_buffer, "%s %s", command, file_name);
			create_process(command_buffer);
		}
		else
		{
			ShellExecute(wi.hwnd, "open", file_name, NULL, NULL, SW_SHOWNORMAL);
		}
	}
	else
	{
		PrintWarning("Cannot find %s.", file);
	}
}

// This function is incomplete. It existed in 5.40 and 5.41g but did not work.
// It is left here for future expansion. It is not called from anywhere.
#if 0
void copy_wmf_to_clipboard(void)
{
	HDC				hdc;
	HMETAFILE		hmf;
	HGLOBAL			hglobal;
	METAFILEPICT	*mfpp;


	hdc = CreateMetaFile(NULL);

	// need to add code here to create picture!!

	hmf = CloseMetaFile(hdc);

	hglobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(METAFILEPICT));
	if (!hglobal)
	{
		return;
	}

	mfpp = GlobalLock(hglobal);
	if (!mfpp)
	{
		GlobalFree(hglobal);
		return;
	}

	mfpp->mm = MM_ANISOTROPIC;	/* For metafiles a special structure  */
	mfpp->xExt = -gs.xWin;		/* with a pointer to the picture data */
	mfpp->yExt = -gs.yWin;		/* needs to be allocated and used.    */
	mfpp->hMF = hglobal;

	GlobalUnlock(hglobal);

	if (!OpenClipboard(wi.hwnd))
	{
		return;
	}

	EmptyClipboard();

	SetClipboardData(CF_METAFILEPICT, hglobal);
	CloseClipboard();
}
#endif

/* Given a command, process it, changing any appropriate program settings. */
/* Return values are 0, meaning it was one of Astrolog's menu commands and */
/* action was taken; 1, meaning it's the special end program command; or   */
/* -1, meaning it's not a custom command and Windows can deal with it.     */

int NWmCommand(WORD wCmd)
{
	int		i;
	long	l;
	BOOL	fGraphics, fT;
	CI		ciTemp;


	wi.wCmd = wCmd;
	fGraphics = us.fGraphics;

	switch (wCmd)
	{
		/* File Menu */

	case cmdFileExit:
		if(PrintQuestion(Lang(sEXIT))) return 1;
		else return 0;

	case cmdOpenChart1:
	case cmdOpenChart2:
	case cmdOpenChart3:
	case cmdOpenChart4:
		wi.nDlgChart = wCmd - cmdOpenChart1 + 1;
		OpenChartDialog();
		break;

	case cmdSearchCharts:
		WiDoDialog(DlgSearchCharts, dlgSearchCharts);
		break;

	case cmdSaveChart:
	case cmdSavePositions:
	case cmdSaveText:
	case cmdSaveBitmap:
#ifdef META
	case cmdSavePicture:
#endif
#ifdef PS
	case cmdSavePS:
#endif
	case cmdSaveSettings:
	case cmdSaveWallTile:
	case cmdSaveWallCenter:
	case cmdSaveWallStretch:
		SaveChartDialog(wCmd);
		break;

	case cmdPrint:
		if(us.fGraphics && gi.kiOff != kWhite) {
			if(!PrintQuestion(Lang(sPRINT_BACKROUND))){ 
				break;
			}
		}
		if (!PrintDialog())
		{
			PrintWarning("The printing was not completed successfully.");
		}
		break;

	case cmdPrintSetup:
		l = prd.Flags;
		prd.Flags |= PD_PRINTSETUP;
		PrintDlg((LPPRINTDLG) & prd);
		prd.Flags = l;
		break;

		/* Edit Menu */

	case cmdCommand:
		WiDoDialog(DlgCommand, dlgCommand);
		break;

	case cmdColor:
		WiDoDialog(DlgColor, dlgColor);
		break;

	case cmdCopyText:
		is.szFileScreen = szFileTemp;
		us.fGraphics = FALSE;
		wi.fRedraw = TRUE;
		break;

	case cmdCopyBitmap:
#ifdef PS
	case cmdCopyPS:
#endif
		if (us.fNoWrite)
			break;
		gi.szFileOut = szFileTemp;
		gs.fBitmap = wi.wCmd == cmdCopyBitmap;
		gs.fPS = wi.wCmd == cmdCopyPS;
		if (wCmd == cmdCopyBitmap)
			gs.chBmpMode = 'B';
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdExchangeCharts1and2:
		ciTemp = ciMain;
		ciMain = ciTwin;
		ciCore = ciTwin;
		ciTwin = ciTemp;
		wi.fCast = TRUE;
		break;

	case cmdPreviousSearchedChart:
	case cmdNextSearchedChart:
		{
			CI		*new_chart;

			if(wCmd == cmdPreviousSearchedChart)
			{
				new_chart = ChartList(CHART_PREVIOUS, CHART_HEAP_SORTED, LastChartShown);
			}
			else
			{
				new_chart = ChartList(CHART_NEXT, CHART_HEAP_SORTED, LastChartShown);
			}

			if(new_chart)
			{
				LastChartShown = new_chart;
				ciMain = *new_chart;
				ciCore = *new_chart;
				wi.fCast = TRUE;
			}
		}
		break;


		/* View Menu */

	case cmdGraphicsToggle:
		not(us.fGraphics);
		wi.fRedraw = TRUE;
		wi.fMenuAll = TRUE;
		break;

	case cmdGraphicsMode:
		us.fGraphics = TRUE;
		wi.fRedraw = TRUE;
		wi.fMenuAll = TRUE;
		break;

	case cmdTextMode:
		us.fGraphics = FALSE;
		wi.fRedraw = TRUE;
		wi.fMenuAll = TRUE;
		break;

	case cmdColoredText:
		not(us.fAnsiColor);
		not(us.fAnsiChar);
		WiCheckMenu(cmdColoredText, us.fAnsiColor);
		us.fGraphics = FALSE;
		wi.fRedraw = TRUE;
		break;

	case cmdDelayScreenRedraw:
		not(wi.fDelayScreenRedraw);
		WiCheckMenu(cmdDelayScreenRedraw, wi.fDelayScreenRedraw);
		break;

	case cmdWinRedraw:
		if (wi.fNoUpdate)
			wi.fNoUpdate = 2;
		wi.fMenuAll = wi.fRedraw = TRUE;
		break;

	case cmdWinClear:
		fT = gs.fJetTrail;
		gs.fJetTrail = FALSE;
		wi.hdc = GetDC(wi.hwnd);
		if (us.fGraphics)
			DrawClearScreen();
		else
			TextClearScreen();
		ReleaseDC(wi.hwnd, wi.hdc);
		gs.fJetTrail = fT;
		break;

	case cmdWinHourglass:
		not(wi.fHourglass);
		WiCheckMenu(cmdWinHourglass, wi.fHourglass);
		break;

	case cmdChartResizesWindow:
		not(wi.fChartWindow);
		WiCheckMenu(cmdChartResizesWindow, wi.fChartWindow);
		wi.fRedraw = TRUE;
		break;

	case cmdWindowResizesChart:
		not(wi.fWindowChart);
		WiCheckMenu(cmdWindowResizesChart, wi.fWindowChart);
		wi.fRedraw = TRUE;
		break;

	case cmdSizeChartToWindow:
		gs.xWin = wi.xClient;
		gs.yWin = wi.yClient;
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdSizeWindowToChart:
		ResizeWindowToChart();
		break;

	case cmdScrollPageUp:
		PostMessage(wi.hwnd, WM_VSCROLL, SB_PAGEUP, 0);
		break;

	case cmdScrollPageDown:
		PostMessage(wi.hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
		break;

	case cmdScrollHome:
		PostMessage(wi.hwnd, WM_HSCROLL, SB_THUMBPOSITION, 0);
		PostMessage(wi.hwnd, WM_VSCROLL, SB_THUMBPOSITION, 0);
		break;

	case cmdScrollEnd:
		PostMessage(wi.hwnd, WM_HSCROLL, SB_THUMBPOSITION, nScrollDiv);
		PostMessage(wi.hwnd, WM_VSCROLL, SB_THUMBPOSITION, 4 * nScrollDiv);
		break;

#ifdef INTERPRET
	case cmdInterpret:
		not(us.fInterpret);
		WiCheckMenu(cmdInterpret, us.fInterpret);
		us.fGraphics = FALSE;
		wi.fRedraw = TRUE;
		break;
#endif

	case cmdSecond:
		not(us.fSeconds);
		is.fSeconds = us.fSeconds;
		WiCheckMenu(cmdSecond, us.fSeconds);
		wi.fRedraw = TRUE;
		break;

	case cmdApplying:
		not(us.fAppSep);
		wi.fRedraw = TRUE;
		break;

	case cmdParallel:
		not(us.fParallel);
		WiCheckMenu(cmdParallel, us.fParallel);
		wi.fRedraw = TRUE;
		break;

		/* Info Menu */

	case cmdHereAndNow:
		SetHereAndNow(&ciCore);
		wi.fCast = TRUE;
		break;

	case cmdDefaultInfo:
		WiDoDialog(DlgDefault, dlgDefault);
		break;

	case cmdEnterData1:
	case cmdEnterData2:
	case cmdEnterData3:
	case cmdEnterData4:
		wi.nDlgChart = wCmd - cmdEnterData1 + 1;
		WiDoDialog(DlgChartData, dlgChartData);
		break;

	case cmdRelNo:
	case cmdRelDualWheel:
	case cmdRelSynastry:
	case cmdRelComposite:
	case cmdRelMidpoint:
	case cmdRelDate:
	case cmdRelTransit:
	case cmdRelProgressed:
	case cmdRelQuadWheel:
	case cmdRelTriWheel:
#ifdef BIORHYTHM
	case cmdRelBiorhythm:
#endif

		SetRel(wCmd - cmdRelProgressed + rcProgress);

		switch (wCmd)
		{
		case cmdRelDate:
			gi.nCurrChart = gWheel;
			us.fGraphics = FALSE;
			break;

#ifdef BIORHYTHM
		case cmdRelBiorhythm:
			wi.nMode = gBiorhythm;
			break;
#endif
		default:
			break;
		}
		break;


		/* Settings Menu */

	case cmdSiderealToggle:
		not(us.fSidereal);
		CheckMenuRadioItem(wi.hmenu, cmdSidereal, cmdTropical, us.fSidereal ? cmdSidereal : cmdTropical, MF_BYCOMMAND);
		wi.fMenu = TRUE;
		wi.fCast = TRUE;
		break;

	case cmdSidereal:
		us.fSidereal = TRUE;
		CheckMenuRadioItem(wi.hmenu, cmdSidereal, cmdTropical, cmdSidereal, MF_BYCOMMAND);
		wi.fMenu = TRUE;
		wi.fCast = TRUE;
		break;

	case cmdTropical:
		us.fSidereal = FALSE;
		CheckMenuRadioItem(wi.hmenu, cmdSidereal, cmdTropical, cmdTropical, MF_BYCOMMAND);
		wi.fMenu = TRUE;
		wi.fCast = TRUE;
		break;

	case cmdAspectsSortToggle:
		not(fSortAspectsByOrbs);
		CheckMenuRadioItem(wi.hmenu, cmdAspectsSortPower, cmdAspectsSortOrbs, fSortAspectsByOrbs ? cmdAspectsSortOrbs : cmdAspectsSortPower, MF_BYCOMMAND);
		wi.fMenu = TRUE;
		wi.fCast = TRUE;
		break;

	case cmdAspectsSortPower:
		fSortAspectsByOrbs = FALSE;
		CheckMenuRadioItem(wi.hmenu, cmdAspectsSortPower, cmdAspectsSortOrbs, cmdAspectsSortPower, MF_BYCOMMAND);
		wi.fMenu = TRUE;
		wi.fCast = TRUE;
		break;

	case cmdAspectsSortOrbs:
		fSortAspectsByOrbs = TRUE;
		CheckMenuRadioItem(wi.hmenu, cmdAspectsSortPower, cmdAspectsSortOrbs, cmdAspectsSortOrbs, MF_BYCOMMAND);
		wi.fMenu = TRUE;
		wi.fCast = TRUE;
		break;

	case cmdHeliocentric:
		us.objCenter = (us.objCenter == oSun) ? oEar : oSun;
		WiCheckMenu(cmdHeliocentric, us.objCenter == oSun);
		wi.fCast = TRUE;
		break;

	case cmdHouse00:
	case cmdHouse01:
	case cmdHouse02:
	case cmdHouse03:
	case cmdHouse04:
	case cmdHouse05:
	case cmdHouse06:
	case cmdHouse07:
	case cmdHouse08:
	case cmdHouse09:
	case cmdHouse10:
	case cmdHouse11:
	case cmdHouse12:
	case cmdHouse13:
	case cmdHouse14:
	case cmdHouse15:
		CheckMenuRadioItem(wi.hmenu, cmdHouse00, cmdHouse15, wCmd, MF_BYCOMMAND);
		us.nHouseSystem = (int) (wCmd - cmdHouse00);
		wi.fCast = TRUE;
		wi.fMenu = TRUE;
		break;

	case cmdHouseSetSolar:
		not(us.objOnAsc);
		WiCheckMenu(cmdHouseSetSolar, us.objOnAsc);
		wi.fCast = TRUE;
		break;

	case cmdHouseSetDecan:
		not(us.fDecan);
		WiCheckMenu(cmdHouseSetDecan, us.fDecan);
		wi.fCast = TRUE;
		break;

	case cmdHouseSetFlip:
		not(us.fFlip);
		WiCheckMenu(cmdHouseSetFlip, us.fFlip);
		wi.fCast = TRUE;
		break;

	case cmdHouseSetGeodetic:
		not(us.fGeodetic);
		WiCheckMenu(cmdHouseSetGeodetic, us.fGeodetic);
		wi.fCast = TRUE;
		break;

	case cmdHouseSetVedic:
		not(us.fVedic);
		WiCheckMenu(cmdHouseSetVedic, us.fVedic);
		wi.fRedraw = TRUE;
		break;

	case cmdHouseSetNavamsa:
		not(us.fNavamsa);
		WiCheckMenu(cmdHouseSetNavamsa, us.fNavamsa);
		wi.fCast = TRUE;
		break;

	case cmdAspect:
		WiDoDialog(DlgAspect, dlgAspect);
		break;

	case cmdObject:
		WiDoDialog(DlgObject, dlgObject);
		break;

	case cmdObject2:
		WiDoDialog(DlgObject2, dlgObject2);
		break;

	case cmdRes:
	case cmdResTransit:
	case cmdResProgress:
		WiDoDialog(DlgObjSelections, dlgObjSelections);
		break;

	case cmdResStar:
		us.nStar = !us.nStar;
		for (i = starLo; i <= starHi; i++)
			ignore[i] = !us.nStar || !ignore[i];
		WiCheckMenu(cmdResStar, us.nStar);
		wi.fCast = TRUE;
		break;

	case cmdStar:
		WiDoDialog(DlgStar, dlgStar);
		break;

	case cmdCalculationSettings:
		WiDoDialog(DlgCalculationSettings, dlgCalculationSettings);
		break;

	case cmdMiscellaneousSettings:
		WiDoDialog(DlgMiscellaneousSettings, dlgMiscellaneousSettings);
		break;

		/* Chart Menu */

	case cmdChartList:
		wi.nMode = gWheel;
		break;

	case cmdChartWheel:
		wi.nMode = gHouse;
		break;

	case cmdChartGrid:
		wi.nMode = gGrid;
		break;

	case cmdChartAspect:
		wi.nMode = gAspect;
		us.fGraphics = FALSE;
		break;

	case cmdChartMidpoint:
		wi.nMode = gMidpoint;
		us.fGraphics = FALSE;
		break;

	case cmdChartHorizon:
		wi.nMode = gHorizon;
		break;

	case cmdChartOrbit:
		wi.nMode = gOrbit;
		break;

	case cmdChartSector:
		wi.nMode = gSector;
		break;

	case cmdChartCalendar:
		not(us.fCalendarYear);
		wi.nMode = gCalendar;
		break;

	case cmdChartCalendarYear:
		us.fCalendarYear = TRUE;
		wi.nMode = gCalendar;
		break;

	case cmdChartCalendarMonth:
		us.fCalendarYear = FALSE;
		wi.nMode = gCalendar;
		break;

	case cmdChartDispositors:
		us.fGraphics = TRUE;
		wi.nMode = gDisposit;
		break;

	case cmdChartPower:
		us.fGraphics = FALSE;
		wi.nMode = gDisposit;
		break;

	case cmdChartAstroGraph:
		wi.nMode = gAstroGraph;
		break;

	case cmdChartEphemeris:
		wi.nMode = gEphemeris;
		break;

#ifdef ARABIC
	case cmdChartArabic:
		wi.nMode = gArabic;
		us.fGraphics = FALSE;
		break;
#endif

	case cmdChartRising:
		wi.nMode = gRising;
		us.fGraphics = FALSE;
		break;

	case cmdProgress:
		WiDoDialog(DlgProgress, dlgProgress);
		break;

	case cmdTransit:
		WiDoDialog(DlgTransit, dlgTransit);
		break;

	case cmdChartSettings:
		WiDoDialog(DlgChartSettings, dlgChartSettings);
		break;

	case cmdArabicPartsSettings:
		WiDoDialog(DlgArabicPartsSettings, dlgArabicPartsSettings);
		break;

		/* Graphics Menu */

	case cmdChartMap:
		wi.nMode = gWorldMap;
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdChartGlobe:
		wi.nMode = gGlobe;
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdChartPolar:
		wi.nMode = gPolar;
		us.fGraphics = wi.fRedraw = TRUE;
		break;

#ifdef CONSTEL
	case cmdConstellation:
		if (!fMap && gi.nCurrChart != gGlobe && gi.nCurrChart != gPolar)
			wi.nMode = gWorldMap;
		not(gs.fConstel);
		WiCheckMenu(cmdConstellation, gs.fConstel);
		us.fGraphics = wi.fRedraw = TRUE;
		break;
#endif

	case cmdGraphicsReverse:
		not(gs.fInverse);
		WiCheckMenu(cmdGraphicsReverse, gs.fInverse);
		wi.fRedraw = TRUE;
		break;

	case cmdGraphicsMonochrome:
		not(gs.fColor);
		WiCheckMenu(cmdGraphicsMonochrome, !gs.fColor);
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdGraphicsBorder:
		not(gs.fBorder);
		WiCheckMenu(cmdGraphicsBorder, gs.fBorder);
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdGraphicsText:
		not(gs.fText);
		WiCheckMenu(cmdGraphicsText, gs.fText);
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdGraphicsSidebar:
		not(us.fVelocity);
		if (!us.fVelocity)
		{
			gs.fText = TRUE;
			WiCheckMenu(cmdGraphicsText, TRUE);
		}
		WiCheckMenu(cmdGraphicsSidebar, !us.fVelocity);
		wi.nMode = gWheel;
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdGraphicsLabel:
		not(gs.fLabel);
		WiCheckMenu(cmdGraphicsLabel, gs.fLabel);
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdGraphicsSquare:
		SquareX(&gs.xWin, &gs.yWin, TRUE);
		ResizeWindowToChart();
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdScale1:
	case cmdScale2:
	case cmdScale3:
	case cmdScale4:
		WiCheckMenu(cmdScale1 + gs.nScale / 100 - 1, FALSE);
		gs.nScale = (int) (wCmd - cmdScale1) + 1;
		gs.nScale *= 100;
		WiCheckMenu(wCmd, TRUE);
		wi.fRedraw = TRUE;
		break;

	case cmdScaleDecrease:
		if (gs.nScale > 100)
		{
			WiCheckMenu(cmdScale1 + gs.nScale / 100 - 1, FALSE);
			gs.nScale -= 100;
		  LScale:
			WiCheckMenu(cmdScale1 + gs.nScale / 100 - 1, TRUE);
			wi.fRedraw = TRUE;
		}
		break;

	case cmdScaleIncrease:
		if (gs.nScale < MAXSCALE)
		{
			WiCheckMenu(cmdScale1 + gs.nScale / 100 - 1, FALSE);
			gs.nScale += 100;
			goto LScale;
		}
		break;

	case cmdTiltZero:
		if (gi.nCurrChart != gGlobe)
			wi.nMode = gGlobe;
		if (gs.rTilt != 0.0)
		{
			gs.rTilt = 0.0;
			wi.fRedraw = TRUE;
		}
		us.fGraphics = TRUE;
		break;

	case cmdTiltDecrease:
		if (gi.nCurrChart != gGlobe)
			wi.nMode = gGlobe;
		if (gs.rTilt > -90.0)
		{
			gs.rTilt = gs.rTilt > -90.0 ? gs.rTilt - TILTSTEP : -90.0;
			wi.fRedraw = TRUE;
		}
		us.fGraphics = TRUE;
		break;

	case cmdTiltIncrease:
		if (gi.nCurrChart != gGlobe)
			wi.nMode = gGlobe;
		if (gs.rTilt < 90.0)
		{
			gs.rTilt = gs.rTilt < 90.0 ? gs.rTilt + TILTSTEP : 90.0;
			wi.fRedraw = TRUE;
		}
		us.fGraphics = TRUE;
		break;

	case cmdBonusMode:
		not(gs.fBonusMode);
		WiCheckMenu(cmdBonusMode, gs.fBonusMode);
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdAddAspectInfo:
		not(InfoFlag);
		WiCheckMenu(cmdAddAspectInfo, InfoFlag);
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdChartModify:
		not(us.fPrimeVert);
		not(us.nEphemYears);
		not(gs.fMollewide);
		gi.nCurrChart = (gi.nCurrChart == gWheel ? gHouse : (gi.nCurrChart == gHouse ? gWheel : gi.nCurrChart));
		us.fGraphics = wi.fRedraw = TRUE;
		break;

	case cmdGraphicsSettings:
		WiDoDialog(DlgGraphics, dlgGraphics);
		break;

	case cmdDirectorySettings:
		WiDoDialog(DlgDirectories, dlgDirectories);
		break;

		/* Animate Menu */

		// jump rate: seconds, minutes,... update to now
	case cmdAnimateS1:
	case cmdAnimateS2:
	case cmdAnimateS3:
	case cmdAnimateS4:
	case cmdAnimateS5:
	case cmdAnimateS6:
	case cmdAnimateS7:
	case cmdAnimateS8:
	case cmdAnimateS9:
	case cmdAnimateNow:
		WiCheckMenu(cmdAnimateS1 + gs.nAnimationJumpRate - 1, FALSE);
		gs.nAnimationJumpRate = (int) (wCmd - cmdAnimateS1 + 1);
		WiCheckMenu(wCmd, TRUE);
		break;

		// jump factors: 1, 2, ...9
	case cmdAnimateF1:
	case cmdAnimateF2:
	case cmdAnimateF3:
	case cmdAnimateF4:
	case cmdAnimateF5:
	case cmdAnimateF6:
	case cmdAnimateF7:
	case cmdAnimateF8:
	case cmdAnimateF9:
		WiCheckMenu(cmdAnimateF1 + abs(wi.nAnimationJumpFactor) - 1, FALSE);
		wi.nAnimationJumpFactor = (wi.nAnimationJumpFactor > 0 ? 1 : -1) * (int) (wCmd - cmdAnimateF1 + 1);
		WiCheckMenu(wCmd, TRUE);
		break;

	case cmdAnimateReverse:
		neg(wi.nAnimationJumpFactor);
		WiCheckMenu(cmdAnimateReverse, wi.nAnimationJumpFactor < 0);
		break;

	case cmdAnimate:
		not(wi.fAnimate);
		WiCheckMenu(cmdAnimate, wi.fAnimate);
		WiCheckMenu(cmdDelayScreenRedraw, TRUE);
		wi.fDelayScreenRedraw = wi.fRedraw = TRUE;
		break;

	case cmdStepForward:
		Animate(gs.nAnimationJumpRate, abs(wi.nAnimationJumpFactor));
		wi.fCast = TRUE;
		break;

	case cmdStepBackward:
		Animate(gs.nAnimationJumpRate, -abs(wi.nAnimationJumpFactor));
		wi.fCast = TRUE;
		break;

	case cmdTimedExposure:
		not(gs.fJetTrail);
		WiCheckMenu(cmdTimedExposure, gs.fJetTrail);
		break;

	case cmdStore:
		ciSave = ciMain;
		break;

	case cmdRecall:
		ciMain = ciSave;
		wi.fCast = TRUE;
		break;

		/* Help Menu */

		

	case cmdHelpForumInternational:
		ShellExecute(wi.hwnd, "open", Lang(sHelpForumInternationalUrl), 
				NULL, NULL, SW_SHOWNORMAL);
		break;

	case cmdHelpForum:
		ShellExecute(wi.hwnd, "open", Lang(sHelpForumUrl), 
				NULL, NULL, SW_SHOWNORMAL);
		break;

	case cmdDocDefault:
		run_command("notepad", CONFIGURATION_FILE);
		break;

	case cmdDocReadme:
		run_command(NULL, Lang(sReadMe));
		break;

	case cmdDocHelpfile:
		run_command(NULL, Lang(sHelp));
		break;

	case cmdUsersGuide:
		run_command(NULL, Lang(sUsersGuide));
		break;

	case cmdDocHomepage:
		run_command(NULL, "astrolog.url");
		break;

	case cmdBrowseAstrolog32:
		run_command(NULL, "astrolog32.url");
		break;

	case cmdDocWAtlas:
		run_command(NULL, Lang(sWAtlas));
		break;

	case cmdDocStars:
		run_command(NULL, "stars.url");
		break;

	case cmdHelpSign:
		wi.nMode = gSign;
		us.fGraphics = FALSE;
		break;

	case cmdHelpObject:
		wi.nMode = gObject;
		us.fGraphics = FALSE;
		break;

	case cmdHelpAspect:
		wi.nMode = gHelpAsp;
		us.fGraphics = FALSE;
		break;

#ifdef CONSTEL
	case cmdHelpConstellation:
		wi.nMode = gConstel;
		us.fGraphics = FALSE;
		break;
#endif

	case cmdHelpPlanetInfo:
		wi.nMode = gPlanet;
		us.fGraphics = FALSE;
		break;

#ifdef INTERPRET
	case cmdHelpMeaning:
		wi.nMode = gMeaning;
		us.fGraphics = FALSE;
		break;
#endif

	case cmdHelpSwitch:
		wi.nMode = gSwitch;
		us.fGraphics = FALSE;
		break;

	case cmdHelpObscure:
		wi.nMode = gObscure;
		us.fGraphics = FALSE;
		break;

	case cmdHelpKeystroke:
		wi.nMode = gKeystroke;
		us.fGraphics = FALSE;
		break;

	case cmdHelpAbout:
		WiDoDialog(DlgAbout, dlgAbout);
		break;

	case cmdProgramStatus:
		wi.nMode = gProgramStatus;
		us.fGraphics = FALSE;
		break;

	case cmdShowSecondChartInfoBar:
		fShowSecondChartInfoBar = !fShowSecondChartInfoBar;
		break;
	
	case cmdSimplifiedMode:
		{
		char* t = "#SimplifiedMode";
		ProcessExtendedSwitches(1, &t, 1, 0,0,0);
		break;
		}
	
	case cmdAdvancedMode:
		{
		char* t = "#AdvancedMode";
		ProcessExtendedSwitches(1, &t, 1, 0,0,0);
		break;
		}	

	case cmdOpenCommandFile:
		OpenCommandFile();
		break;
	
	default:
		if (FBetween(wCmd, cmdMacro01, cmdMacro01 + NUMBER_OF_MACROS - 1))
		{
			i = (int) (wCmd - cmdMacro01);

			if (szMacro[i])
			{
				FProcessCommandLine(szMacro[i]);
				wi.fCast = wi.fMenuAll = TRUE;
			}
			else
			{
				PrintWarning("Macro number %d is not defined.", i + 1);
			}
			break;
		}
		return -1;
	}

	if (us.fNoGraphics)
		us.fGraphics = FALSE;

	if (us.fGraphics != fGraphics)
	{
		CheckMenuRadioItem(wi.hmenu, cmdGraphicsMode, cmdTextMode, 
			us.fGraphics ? cmdGraphicsMode : cmdTextMode, MF_BYCOMMAND);
		wi.fRedraw = TRUE;
		wi.fMenu = TRUE;
	}

	return 0;
}


/* For each menu command that can have a check mark by it, determine its   */
/* state and set or clear appropriately. This is called when the program   */
/* is first started, and after commands that may change many settings that */
/* can't be kept track of, such as running macro commands or script files. */


void RedoMenu()
{
	WORD			cmd;


	if(us.fGraphics)
	{
		CheckMenu(cmdChartDispositors, us.fInfluence);
		CheckMenu(cmdChartPower, FALSE);
	}
	else
	{
		CheckMenu(cmdChartDispositors, FALSE);
		CheckMenu(cmdChartPower, us.fInfluence);
	}

	CheckMenu(cmdChartAspect, us.fAspList);
	CheckMenu(cmdChartArabic, us.nArabic);
	CheckMenu(cmdChartRising, us.fHorizon && us.fHorizonSearch);
	CheckMenu(cmdChartMidpoint, us.fMidpoint);
	CheckMenu(cmdChartOrbit, us.fOrbit);
	CheckMenu(cmdChartEphemeris, us.fEphemeris);
	CheckMenu(cmdChartGrid, us.fGrid);
	CheckMenu(cmdChartSector, us.fSector);
	CheckMenu(cmdChartHorizon, us.fHorizon);
	CheckMenu(cmdChartAstroGraph, us.fAstroGraph);
	CheckMenu(cmdChartWheel, us.fWheel);
	CheckMenu(cmdChartList, us.fListing);
	CheckMenu(cmdColoredText, us.fAnsiColor);
	CheckMenu(cmdDelayScreenRedraw, wi.fDelayScreenRedraw);
	CheckMenu(cmdWinHourglass, wi.fHourglass);
	CheckMenu(cmdChartResizesWindow, wi.fChartWindow);
	CheckMenu(cmdWindowResizesChart, wi.fWindowChart);
#ifdef INTERPRET
	CheckMenu(cmdInterpret, us.fInterpret);
#endif
	CheckMenu(cmdSecond, us.fSeconds);
	CheckMenu(cmdParallel, us.fParallel);
	CheckMenu(cmdHeliocentric, us.objCenter == oSun);
	CheckMenu(cmdHouseSetSolar, us.objOnAsc);
	CheckMenu(cmdHouseSetDecan, us.fDecan);
	CheckMenu(cmdHouseSetFlip, us.fFlip);
	CheckMenu(cmdHouseSetGeodetic, us.fGeodetic);
	CheckMenu(cmdHouseSetVedic, us.fVedic);
	CheckMenu(cmdHouseSetNavamsa, us.fNavamsa);
	CheckMenu(cmdResStar, us.nStar);
#ifdef CONSTEL
	CheckMenu(cmdConstellation, gs.fConstel);
#endif
	CheckMenu(cmdGraphicsReverse, gs.fInverse);
	CheckMenu(cmdGraphicsMonochrome, !gs.fColor);
	CheckMenu(cmdGraphicsBorder, gs.fBorder);
	CheckMenu(cmdGraphicsText, gs.fText);
	CheckMenu(cmdGraphicsSidebar, !us.fVelocity);
	CheckMenu(cmdGraphicsLabel, gs.fLabel);
	for (cmd = cmdScale1; cmd <= cmdScale4; cmd++)
		CheckMenu(cmd, FALSE);
	CheckMenu(cmdScale1 + gs.nScale / 100 - 1, TRUE);
	CheckMenu(cmdBonusMode, gs.fBonusMode);
	CheckMenu(cmdAddAspectInfo, InfoFlag);
	CheckMenu(cmdAnimateS1 + gs.nAnimationJumpRate - 1, TRUE);
	CheckMenu(cmdAnimateF1 + abs(wi.nAnimationJumpFactor) - 1, TRUE);
	CheckMenu(cmdAnimateReverse, wi.nAnimationJumpFactor < 0);
	CheckMenu(cmdAnimate, wi.fAnimate);
	CheckMenu(cmdTimedExposure, gs.fJetTrail);
	CheckMenu(cmdProgress, us.fProgressUS);

	// menu radio buttons
	CheckMenuRadioItem(wi.hmenu, cmdSidereal, cmdTropical, 
		us.fSidereal ? cmdSidereal : cmdTropical, MF_BYCOMMAND);
	CheckMenuRadioItem(wi.hmenu, cmdAspectsSortPower, cmdAspectsSortOrbs, 
		fSortAspectsByOrbs ? cmdAspectsSortOrbs : cmdAspectsSortPower, MF_BYCOMMAND);
	CheckMenuRadioItem(wi.hmenu, cmdRelProgressed, cmdRelBiorhythm, 
		us.nRel - rcProgress + cmdRelProgressed, MF_BYCOMMAND);
	CheckMenuRadioItem(wi.hmenu, cmdHouse00, cmdHouse15, 
		cmdHouse00 + us.nHouseSystem, MF_BYCOMMAND);
	CheckMenuRadioItem(wi.hmenu, cmdGraphicsMode, cmdTextMode, 
		us.fGraphics ? cmdGraphicsMode : cmdTextMode, MF_BYCOMMAND);

	wi.fMenuAll = FALSE;
}


// for graphics: FRedraw -> Action -> FActionX -> BeginX

/* BOOL FRedraw(void) */
/* This important routine is the bottleneck to redraw the window and call */
/* into the program to draw or do action with a particular chart type.    */

/* void Action() */
/* This is the dispatch procedure for the entire program. After all the   */
/* command switches have been processed, this routine is called to        */
/* actually call the various routines to generate and display the charts. */

/* BOOL FActionX() */
/* This is the main interface to all the graphics features. This routine     */
/* is called from the main program if any of the -X switches were specified, */
/* and it sets up for and goes and generates the appropriate graphics chart. */
/* We return TRUE if successfull, FALSE if some non-fatal error occurred.  */

/* BeginX */
/* this routine opens up and initializes a window and prepares it to be */
/* drawn upon, and gets various information about the display, too.     */

//-----------------

/* This important routine is the bottleneck to redraw the window and call */
/* into the program to draw or do action with a particular chart type.    */

BOOL FRedraw(void)
{
	/* Local variables used in drawing on the screen. */
	PAINTSTRUCT ps;
	HDC hdcWin;
	HCURSOR hcurOld;
	HBITMAP hbmp, hbmpOld;
	HFONT hfontOld;
	int nScrollRow, i;

	/* Local variables used for copying to the Windows clipboard. */
	HFILE hfile;
	LONG lSize, l;
	HGLOBAL hglobal;
	byte *hpb;

	if (wi.fHourglass)
		hcurOld = SetCursor(LoadCursor((HINSTANCE) NULL, IDC_WAIT));
	memset(&ps, 0, sizeof(ps));
	if (wi.hdcPrint != hdcNil)
		wi.hdc = wi.hdcPrint;
	else
	{
		hdcWin = BeginPaint(wi.hwnd, &ps);
		if (wi.fDelayScreenRedraw)
		{
			wi.hdc = CreateCompatibleDC(hdcWin);
			hbmp = CreateCompatibleBitmap(hdcWin, wi.xClient, wi.yClient);
			hbmpOld = (HBITMAP) SelectObject(wi.hdc, hbmp);
			if (gs.fJetTrail)
				BitBlt(wi.hdc, 0, 0, wi.xClient, wi.yClient, hdcWin, 0, 0, SRCCOPY);
		}
		else
			wi.hdc = hdcWin;
	}

	if (us.fGraphics)
	{
		/* Set up a graphics chart. */
		if (wi.fWindowChart && wi.hdcPrint == hdcNil)
		{
			gs.xWin = wi.xClient;
			gs.yWin = wi.yClient;
		}
		gi.nScale = gs.nScale / 100;
		gi.kiCur = -1;
	}
	else
	{
		/* Set up a text chart. */
		SetWindowOrgEx(wi.hdc, 0, 0, NULL);
		SetWindowExtEx(wi.hdc, wi.xClient, wi.yClient, NULL);
		SetBkMode(wi.hdc, TRANSPARENT);
		if (wi.hdcPrint == hdcNil)
			TextClearScreen();
		i = gs.nScale / 100;
		wi.xChar = i < 2 ? 6 : (i < 3 ? 8 : (i < 4 ? 10 : 12));
//		wi.yChar = i < 2 ? 8 : (i < 3 ? 12 : (i < 4 ? 18 : 16));
		wi.yChar = i < 2 ? 12 : (i < 3 ? 14 : (i < 4 ? 16 : 18));

//		wi.hfont = CreateFont(wi.yChar /*nHeight */ ,wi.xChar /*nWidth */ ,
		wi.hfont = CreateFont(wi.yChar /*nHeight */ ,0 /*nWidth auto*/ ,
							  0 /*nEscapement */ , 0 /*nOrientation */ ,
							  FW_DONTCARE , 0 /*fbItalic */ , 0 /*fbUnderline */ ,
							  0 /*fbStrikeOut */ ,
							  gs.charSet, OUT_CHARACTER_PRECIS,
							  CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FIXED_PITCH | FF_DONTCARE, "Lucida Console");
		hfontOld = (HFONT)SelectObject(wi.hdc, wi.hfont);
		TEXTMETRIC tm;
		GetTextMetrics(wi.hdc, &tm);
		wi.yChar = tm.tmHeight;
		wi.xChar = tm.tmAveCharWidth;
		/* If printing, set the number of text rows per page. */
		if (wi.hdcPrint != hdcNil)
		{
			nScrollRow = us.nScrollRow;
			us.nScrollRow = wi.yClient / wi.yChar;
		}
	}

	Action();					/* Actually go and create the chart here. */

	/* Cleanup and copy from the buffer to the screen if need be. */

	if (!us.fGraphics)
	{
		if (wi.hdcPrint != hdcNil)
			us.nScrollRow = nScrollRow;
		SelectObject(wi.hdc, hfontOld);
		DeleteObject(wi.hfont);
	}
	if (wi.hdcPrint == hdcNil)
	{
		if (wi.fDelayScreenRedraw)
		{
			BitBlt(hdcWin, 0, 0, wi.xClient, wi.yClient, wi.hdc, 0, 0, SRCCOPY);
			SelectObject(wi.hdc, hbmpOld);
			DeleteObject(hbmp);
			DeleteDC(wi.hdc);
		}
		EndPaint(wi.hwnd, &ps);
	}
	if (wi.fHourglass)
		SetCursor(hcurOld);

	/* If all text was scrolled off the top of the screen, scroll up. */

	if (!us.fGraphics && is.S == stdout && is.cchRow - wi.yScroll * 10 < 0)
		PostMessage(wi.hwnd, WM_VSCROLL, SB_THUMBPOSITION, is.cchRow / 10 - 2);

	/* Sometimes creating a chart means saving it to a file instead of     */
	/* drawing it on screen. If we were in file mode, cleanup things here. */

	if (is.szFileScreen != NULL || gs.fBitmap || gs.fMeta || gs.fPS)
	{
		is.szFileScreen = NULL;
		if (gs.fMeta)
		{
			gs.xWin /= METAMUL;
			gs.yWin /= METAMUL;
			gs.nScale /= METAMUL;
		}
		else if (gs.fPS)
		{
			gs.xWin /= PSMUL;
			gs.yWin /= PSMUL;
			gs.nScale /= PSMUL;
		}
		gs.fBitmap = gs.fMeta = gs.fPS = FALSE;

		/* To copy charts to the clipboard, Astrolog saves the chart to a temp */
		/* file, then copies the contents of that file to the clipboard.       */

		if (wi.wCmd == cmdCopyText || wi.wCmd == cmdCopyBitmap || wi.wCmd == cmdCopyPS)
		{
			hfile = _lopen(szFileTemp, OF_READ);
			if (hfile == HFILE_ERROR)
			{
				return FALSE;
			}

			lSize = _llseek(hfile, 0, 2);
			/* For bitmap skip over the file header bytes. */
			l = (wi.wCmd == cmdCopyBitmap) ? sizeof(BITMAPFILEHEADER) : 0;

			hglobal = GlobalAlloc(GMEM_MOVEABLE, lSize - l);
			if (!hglobal)
			{
				return FALSE;
			}

			hpb = (byte*)GlobalLock(hglobal);
			_llseek(hfile, l, 0);
			_hread(hfile, hpb, lSize - l);
			_lclose(hfile);
			GlobalUnlock(hglobal);

			if (!OpenClipboard(wi.hwnd))
			{
				return FALSE;
			}

			EmptyClipboard();

			if (wi.wCmd == cmdCopyText || wi.wCmd == cmdCopyPS)
			{
				SetClipboardData(CF_TEXT, hglobal);
			}
			else
			{
				SetClipboardData(CF_DIB, hglobal);
			}

			CloseClipboard();
			_unlink(szFileTemp);	// delete file
			wi.wCmd = 0;
		}
		ProcessState();
	}

	wi.fRedraw = FALSE;
	if (wi.fNoUpdate)
		wi.fNoUpdate = TRUE;
	return TRUE;
}

/* wdriver.c */
