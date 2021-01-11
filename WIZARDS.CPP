/*
** Astrolog File: wizards.c
**
** Author: Antonio Ramos.
**
** Permission is granted to freely use and distribute these routines
** provided one doesn't sell, restrict, or profit from them in any way.
** Modification is allowed provided these notices remain with any
** altered or edited versions of the program.
**
** This copyright notice must not be changed or removed 
** by any user of this program.
**
** This software is provided "as is", without warranty of any kind, either 
** express or implied. In no event shall the authors or copyright holders be 
** liable for any claim, damages or other liability, arising from the use or 
** performance of this software.
**
*/


#include "astrolog.h"
#include "windowsx.h"
#include "prsht.h"
#include "gui.h"


/*
******************************************************************************
** Local Definitions
******************************************************************************
*/

//typedef DWORD* DWORD_PTR; //TK

typedef struct SHAREDWIZDATA 
{
	HFONT			hTitleFont; // title font for the Welcome and Completion pages
	BOOL			user_completed_ok;
} SHAREDWIZDATA, *LPSHAREDWIZDATA;

// wizard page ids
enum
{
	PAGE_INTRO,
	PAGE_REGIONAL_PREF,
	PAGE_DEF_LOC_TIMEZONE,
	PAGE_DIRECTORIES,
	PAGE_MODE,
	PAGE_COMPLETION,
	PAGE_COUNT,
};
         
#define ValidateR(r, f, sz) if (!(f)) {PrintWarning(Lang(sFLOAT_VALUE_NOT_VALID_SETTING), r, sz); \
                    SetWindowLong(hdlg, DWL_MSGRESULT, -1); return TRUE; }


/*
******************************************************************************
** Dialog Utility Functions.
******************************************************************************
*/



/*
******************************************************************************
** Windows Dialogs.
******************************************************************************
*/

   
static BOOL CALLBACK DlgWelcomeWizardIntro(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPSHAREDWIZDATA		pdata;
	HWND				hwndControl;
	LPNMHDR				lpnm;
	
	
    switch (message)
    {
    case WM_INITDIALOG :
        pdata = (LPSHAREDWIZDATA) ((LPPROPSHEETPAGE) lParam) -> lParam;
        SetWindowLong(hdlg, GWL_USERDATA, (DWORD_PTR) pdata);
        hwndControl = GetDlgItem(hdlg, IDC_TITLE);
        SetWindowFont(hwndControl,pdata->hTitleFont, TRUE);
        break;
		
    case WM_NOTIFY :
		lpnm = (LPNMHDR) lParam;
		pdata = (LPSHAREDWIZDATA) GetWindowLong(hdlg, GWL_USERDATA);
		
        switch (lpnm->code)
		{
		case PSN_SETACTIVE : //Enable the Next button    
			PropSheet_SetWizButtons(GetParent(hdlg), PSWIZB_NEXT);
			break;
			
		case PSN_WIZNEXT :
			break;
			
		case PSN_RESET :
			break;
			
		default :
			break;
		}
        break;
		
	default:
		break;
    }
	
    return 0;
}

static BOOL CALLBACK DlgWelcomeWizardRegionalPref(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPSHAREDWIZDATA		pdata;
	HWND				hwndControl;
	LPNMHDR				lpnm;

	
    switch (message)
    {
    case WM_INITDIALOG :
        pdata = (LPSHAREDWIZDATA) ((LPPROPSHEETPAGE) lParam) -> lParam;
        SetWindowLong(hdlg, GWL_USERDATA, (DWORD_PTR) pdata);
        hwndControl = GetDlgItem(hdlg, IDC_TITLE);
        SetWindowFont(hwndControl,pdata->hTitleFont, TRUE);
		
		SetRadio(us.fEuroDate ? dx02 : dx01, dx01, dx03);	// default is American
        break;
		
    case WM_NOTIFY :
		lpnm = (LPNMHDR) lParam;
		pdata = (LPSHAREDWIZDATA) GetWindowLong(hdlg, GWL_USERDATA);
		
		switch (lpnm->code)
		{
			
		case PSN_SETACTIVE : //Enable the Next and Back button
			PropSheet_SetWizButtons(GetParent(hdlg), PSWIZB_BACK | PSWIZB_NEXT);
			break;
			
		case PSN_WIZNEXT :
			if(GetCheck(dx01))
			{
				// American - only set location, other settings are default
				us.fEuroDate  = FALSE;
				us.fEuroTime = FALSE;
				us.dstDef = 0;
				if(!us.zonDef) us.zonDef = 5;
				if(!us.lonDef) us.lonDef = 77.01;
				if(!us.latDef) us.latDef = 38.54;
				if(!us.szLocNameDef[0]) us.szLocNameDef = "Washington, DC";
			}
			else if(GetCheck(dx02))
			{
				// European
				gs.nGlyphs = 2113;
				us.fEuroDate  = TRUE;
				us.fEuroTime = TRUE;

				us.dstDef = 0;
				if(!us.zonDef) us.zonDef = -1;
				if(!us.lonDef) us.lonDef = -4.20;
				if(!us.latDef) us.latDef = 50.50;
				if(!us.szLocNameDef[0]) us.szLocNameDef = Lang(sBRUSSELS);
			}
			else 
			{
				// Indian
				us.fEuroDate  = TRUE;
				us.fEuroTime = TRUE;
				us.fSidereal = TRUE;
				us.nSiderealMode = SE_SIDM_LAHIRI;
				swe_set_sid_mode(us.nSiderealMode, 0, 0);
				us.nHouseSystem = cHouseVedic;

				us.dstDef = 0;
				us.zonDef = -5.30;
				us.lonDef = -77.12;
				us.latDef = 28.36;
				us.szLocNameDef = "New Delhi, India";
			}
			break;
			
		case PSN_RESET :
			break;
			
		default :
			break;
		}
        break;
		
	default:
		break;
    }
	
    return 0;
}

static BOOL CALLBACK DlgWelcomeWizardDefLocTimezone(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPSHAREDWIZDATA		pdata;
	HWND				hwndControl;
	LPNMHDR				lpnm;
	CI					ci;
	char				sz[cchSzDef];
	
    switch (message)
    {
    case WM_INITDIALOG :
        pdata = (LPSHAREDWIZDATA) ((LPPROPSHEETPAGE) lParam) -> lParam;
        SetWindowLong(hdlg, GWL_USERDATA, (DWORD_PTR) pdata);
        hwndControl = GetDlgItem(hdlg, IDC_TITLE);
        SetWindowFont(hwndControl,pdata->hTitleFont, TRUE);
        break;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == dbAtlas)
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
        break;

    case WM_NOTIFY :
		lpnm = (LPNMHDR) lParam;
		pdata = (LPSHAREDWIZDATA) GetWindowLong(hdlg, GWL_USERDATA);
		
		switch (lpnm->code)
		{
			
		case PSN_SETACTIVE : 
			SetEditSZOA(hdlg, dcDeDst, dcDeZon, dcDeLon, dcDeLat, dcDeLocName,
				us.dstDef, us.zonDef, us.lonDef, us.latDef, us.szLocNameDef);
			PropSheet_SetWizButtons(GetParent(hdlg), PSWIZB_BACK | PSWIZB_NEXT);
			break;
			
		case PSN_WIZNEXT :
			GetEdit(dcDeDst, sz);
			ci.dst = RParseSz(sz, pmDst);
			GetEdit(dcDeZon, sz);
			ci.zon = RParseSz(sz, pmZon);
			GetEdit(dcDeLon, sz);
			ci.lon = RParseSz(sz, pmLon);
			GetEdit(dcDeLat, sz);
			ci.lat = RParseSz(sz, pmLat);
			ValidateR(ci.dst, FValidZon(ci.dst), "daylight");
			ValidateR(ci.zon, FValidZon(ci.zon), "zone");
			ValidateR(ci.lon, FValidLon(ci.lon), "longitude");
			ValidateR(ci.lat, FValidLat(ci.lat), "latitude");
			us.dstDef = ci.dst;
			us.zonDef = ci.zon;
			us.lonDef = ci.lon;
			us.latDef = ci.lat;
			GetEdit(dcDeLocName, sz);
			us.szLocNameDef = SzPersist(sz);
			break;
			
		case PSN_RESET :
			break;
			
		default :
			break;
		}
        break;
		
	default:
		break;
    }
	
    return 0;
}

static BOOL CALLBACK DlgWelcomeWizardDirectories(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPSHAREDWIZDATA		pdata;
	HWND				hwndControl;
	LPNMHDR				lpnm;
	
    switch (message)
    {
    case WM_INITDIALOG :
        pdata = (LPSHAREDWIZDATA) ((LPPROPSHEETPAGE) lParam) -> lParam;
        SetWindowLong(hdlg, GWL_USERDATA, (DWORD_PTR) pdata);
        hwndControl = GetDlgItem(hdlg, IDC_TITLE);
        SetWindowFont(hwndControl,pdata->hTitleFont, TRUE);
		SetCheck(dx01, TRUE);			// create directories by default
        break;
		
    case WM_NOTIFY :
		lpnm = (LPNMHDR) lParam;
		pdata = (LPSHAREDWIZDATA) GetWindowLong(hdlg, GWL_USERDATA);
		
		switch (lpnm->code)
		{
			
		case PSN_SETACTIVE : 
			PropSheet_SetWizButtons(GetParent(hdlg), PSWIZB_BACK | PSWIZB_NEXT);
			SetEdit(dimain,					dirs.main.dir);
			SetEdit(diephemeris,			dirs.ephemeris.dir);
			SetEdit(dicharts,				dirs.charts.dir);
			SetEdit(diinterpretation,		dirs.interpretation.dir);
			SetEdit(dimiscel,				dirs.miscel.dir);
			SetEdit(diAmericanAtlas,		dirs.american_atlas.dir);
			SetEdit(diInternationalAtlas,	dirs.international_atlas.dir);
			break;
			
		case PSN_WIZNEXT :
			ProcessDirectorySettings(hdlg);
			break;
			
		case PSN_RESET :
			break;
			
		default :
			break;
		}
        break;
		
	default:
		break;
    }
	
    return 0;
}

static BOOL CALLBACK DlgWelcomeWizardMode(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPSHAREDWIZDATA		pdata;
	HWND				hwndControl;
	LPNMHDR				lpnm;

	
    switch (message)
    {
    case WM_INITDIALOG :
        pdata = (LPSHAREDWIZDATA) ((LPPROPSHEETPAGE) lParam) -> lParam;
        SetWindowLong(hdlg, GWL_USERDATA, (DWORD_PTR) pdata);
        hwndControl = GetDlgItem(hdlg, IDC_TITLE);
        SetWindowFont(hwndControl,pdata->hTitleFont, TRUE);
		
		SetRadio(dx02, dx01, dx02);
        break;
		
    case WM_NOTIFY :
		lpnm = (LPNMHDR) lParam;
		pdata = (LPSHAREDWIZDATA) GetWindowLong(hdlg, GWL_USERDATA);
		
		switch (lpnm->code)
		{
			
		case PSN_SETACTIVE : //Enable the Next and Back button
			PropSheet_SetWizButtons(GetParent(hdlg), PSWIZB_BACK | PSWIZB_NEXT);
			break;
			
		case PSN_WIZNEXT :
			if(GetCheck(dx01))
			{
				// Simplified
				us.advancedMode = FALSE;
			}
			else
			{
				// Advanced
				us.advancedMode = TRUE;
			}
			break;
			
		case PSN_RESET :
			break;
			
		default :
			break;
		}
        break;
		
	default:
		break;
    }
	
    return 0;
}

static BOOL CALLBACK DlgWelcomeWizardCompletion(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPSHAREDWIZDATA		pdata;
	HWND				hwndControl;
	LPNMHDR				lpnm;
	

    switch (message)
    {
    case WM_INITDIALOG :
        pdata = (LPSHAREDWIZDATA) ((LPPROPSHEETPAGE) lParam) -> lParam;
        SetWindowLong(hdlg, GWL_USERDATA, (DWORD_PTR) pdata);
        hwndControl = GetDlgItem(hdlg, IDC_TITLE);
        SetWindowFont(hwndControl,pdata->hTitleFont, TRUE);
		// PropSheet_SetFinishText(hdlg, "Save"); - this does not work for some reason
        break;
		
    case WM_NOTIFY :
		lpnm = (LPNMHDR) lParam;
		pdata = (LPSHAREDWIZDATA) GetWindowLong(hdlg, GWL_USERDATA);
		
        switch (lpnm->code)
		{
		case PSN_SETACTIVE : 
			PropSheet_SetWizButtons(GetParent(hdlg), PSWIZB_BACK | PSWIZB_FINISH);
			break;
			
		case PSN_WIZBACK :
			break;
			
		case PSN_WIZFINISH :
			pdata->user_completed_ok = TRUE;
			break;
			
		case PSN_RESET :
			break;
			
		default :
			break;
		}
		break;
		
	default:
		break;
    }
	
    return 0;
}	


void WelcomeWizard(HINSTANCE hInstance, HWND hwnd)
{
    HPROPSHEETPAGE		ahpsp[PAGE_COUNT] = {0};// an array to hold the page's HPROPSHEETPAGE handles
    PROPSHEETPAGE		psp =       {0};		// defines the property sheet pages
    PROPSHEETHEADER		psh =       {0};		// defines the property sheet
    SHAREDWIZDATA		wizdata =	{0};		// the shared data structure
    NONCLIENTMETRICS	ncm = {0};
	HDC					hdc;
    INT					FontSize = 12;
	LOGFONT				TitleLogFont;

    //
    //Create the Wizard pages
    //

    //Opening page

    psp.dwSize =			sizeof(psp);
    psp.dwFlags =			PSP_DEFAULT|PSP_HIDEHEADER;
	psp.hInstance =			Lang.resource;
    psp.lParam =			(LPARAM) &wizdata; //The shared data structure
    psp.pszTemplate =		MAKEINTRESOURCE(dlgWelcomeWizardIntro);
    psp.pfnDlgProc =		DlgWelcomeWizardIntro;

    ahpsp[PAGE_INTRO] =		CreatePropertySheetPage(&psp);

	if(!us.omitRegionalWizard) {
		//First interior page

		psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
		psp.pszHeaderTitle =    Lang(sWELCOME_WIZ_TITLE1);
		psp.pszHeaderSubTitle = Lang(sWELCOME_WIZ_SUBTITLE1);
		psp.pszTemplate =       MAKEINTRESOURCE(dlgWelcomeWizardRegionalPref);
		psp.pfnDlgProc =        DlgWelcomeWizardRegionalPref;

		ahpsp[PAGE_REGIONAL_PREF] = CreatePropertySheetPage(&psp);

		//Second interior page

		psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
		psp.pszHeaderTitle =    Lang(sWELCOME_WIZ_TITLE2);
		psp.pszHeaderSubTitle = Lang(sWELCOME_WIZ_SUBTITLE2);
		psp.pszTemplate =       MAKEINTRESOURCE(dlgWelcomeWizardDefLocTimezone);
		psp.pfnDlgProc =        DlgWelcomeWizardDefLocTimezone;

		ahpsp[PAGE_DEF_LOC_TIMEZONE] = CreatePropertySheetPage(&psp);
	}
    //Third interior page

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    Lang(sWELCOME_WIZ_TITLE3);
    psp.pszHeaderSubTitle = Lang(sWELCOME_WIZ_SUBTITLE3);
    psp.pszTemplate =       MAKEINTRESOURCE(dlgWelcomeWizardDirectories);
    psp.pfnDlgProc =        DlgWelcomeWizardDirectories;

    ahpsp[PAGE_DIRECTORIES] = CreatePropertySheetPage(&psp);

    //FOURTH interior page

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    Lang(sWELCOME_WIZ_TITLE4);
    psp.pszHeaderSubTitle = Lang(sWELCOME_WIZ_SUBTITLE4);
    psp.pszTemplate =       MAKEINTRESOURCE(dlgWelcomeWizardMode);
    psp.pfnDlgProc =        DlgWelcomeWizardMode;

    ahpsp[PAGE_MODE] = CreatePropertySheetPage(&psp);

	//Final page

    psp.dwFlags =			PSP_DEFAULT|PSP_HIDEHEADER;
    psp.pszTemplate =		MAKEINTRESOURCE(dlgWelcomeWizardCompletion);
    psp.pfnDlgProc =		DlgWelcomeWizardCompletion;

    ahpsp[PAGE_COMPLETION] = CreatePropertySheetPage(&psp);


    //Create the property sheet

    psh.dwSize =            sizeof(psh);
    psh.hInstance =         hInstance;
    psh.hwndParent =        hwnd;
    psh.phpage =            ahpsp;
    psh.dwFlags =           PSH_WIZARD97|PSH_WATERMARK|PSH_HEADER;
    psh.pszbmWatermark =    MAKEINTRESOURCE(IDB_WATERMARK);
    psh.pszbmHeader =       MAKEINTRESOURCE(IDB_BANNER);
    psh.nStartPage =        PAGE_INTRO;
    psh.nPages =            PAGE_COUNT;


    //Set up the font for the titles on the intro and ending pages

    ncm.cbSize = sizeof(ncm);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

    //Create the intro/end title font
    TitleLogFont = ncm.lfMessageFont;
    TitleLogFont.lfWeight = FW_BOLD;
	strcpy(TitleLogFont.lfFaceName, "Verdana Bold");

    hdc = GetDC(NULL); //gets the screen DC
    TitleLogFont.lfHeight = 0 - GetDeviceCaps(hdc, LOGPIXELSY) * FontSize / 72;
    wizdata.hTitleFont = CreateFontIndirect(&TitleLogFont);
    ReleaseDC(NULL, hdc);

    //Display the wizard
    PropertySheet(&psh);
    
    // Destroy the fonts
    DeleteObject(wizdata.hTitleFont);

	wi.fCast = TRUE;
	if(wizdata.user_completed_ok)
	{
		SaveChartDialog(cmdSaveSettings);
		if(!us.advancedMode) {
			char* t = "#SimplifiedMode";
			ProcessExtendedSwitches(1, &t, 1, 0,0,0);
		}
	}
}
