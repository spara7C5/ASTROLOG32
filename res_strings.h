
//---------------------------------------------------------
//						Tables
//---------------------------------------------------------

#define tFirst								1000
#define tSignName							(tFirst + 0)
#define tSignAbbrev							(tFirst + 1)
#define tObjName							(tFirst + 2)
#define tObjShortName						(tFirst + 3)
#define tAspectName							(tFirst + 4)
#define tAspectAbbrev						(tFirst + 5)
#define tAspectConfig						(tFirst + 6)
#define tHouseSystem						(tFirst + 7)
#define tMode								(tFirst + 8)
#define tElement							(tFirst + 9)
#define tAyanamshaAbbrev					(tFirst + 10)
#define tAyanamshaName						(tFirst + 11)

#define tMonth								(tFirst + 12)
#define tDay								(tFirst + 13)
#define tSuffix								(tFirst + 14)
#define tColor								(tFirst + 15)
#define tSignMeaning						(tFirst + 16)
#define tHouseMeaning						(tFirst + 17)
#define tAspectGlyph						(tFirst + 18)
#define tDir								(tFirst + 19)
#define tArabicPart							(tFirst + 20)
#define tDirNames							(tFirst + 21)
#define tDirPaths							(tFirst + 22)

#define tLast								tDirPaths


//---------------------------------------------------------
//						Strings
//---------------------------------------------------------

#define sFirst								100
#define sYES								(sFirst +  0)
#define sNO									(sFirst +  1)
#define sAM									(sFirst +  2)
#define sPM									(sFirst +  3)
#define sST									(sFirst +  4)
#define sDT									(sFirst +  5)
#define sDegSign							(sFirst +  6)


// DrawInfo() strings
#define sFirstDrawInfo						(sDegSign + 1)
#define sHOUSE1								(sFirstDrawInfo +  0)
#define sHOUSES1							(sFirstDrawInfo +  1)
#define sHouseCusp							(sFirstDrawInfo +  2)

#define sTROPICAL							(sFirstDrawInfo +  3)
#define sSIDEREAL							(sFirstDrawInfo +  4)
#define sSIDEREAL_TIME						(sFirstDrawInfo +  5)
#define sHELIOCENTRIC						(sFirstDrawInfo +  6)
#define sGEOCENTRIC							(sFirstDrawInfo +  7)
#define sJULIAN_DAY							(sFirstDrawInfo +  8)
#define sHARMONIC							(sFirstDrawInfo +  9)
#define sOBLIQUITY							(sFirstDrawInfo + 10)
#define sBONUS_MODE							(sFirstDrawInfo + 11)
#define sINNER_RING							(sFirstDrawInfo + 12)
#define sOUTER_RING							(sFirstDrawInfo + 13)
#define sNO_TIME_OR_SPACE					(sFirstDrawInfo + 14)
#define sTIME_SPACE_MIDPOINT				(sFirstDrawInfo + 15)
#define sCOMPOSITE_CHART					(sFirstDrawInfo + 16)
#define sSYNASTRY_HOUSES					(sFirstDrawInfo + 17)
#define sOUTER_RING_TRANSIT					(sFirstDrawInfo + 18)

#define sDEGREE_PER_YEAR_MONTH				(sFirstDrawInfo + 19)
#define sSOLAR_ARC							(sFirstDrawInfo + 20)

// the following two lines continue the two lines above 

#define sOUTER_RING_DIRECTED_TO				(sFirstDrawInfo + 21)
#define sCHART_DIRECTED_TO					(sFirstDrawInfo + 22)

#define sOUTER_RING_PROGRESSED_TO			(sFirstDrawInfo + 23)
#define sSECOND_RING						(sFirstDrawInfo + 24)
#define sCHART_PROGRESSED_TO				(sFirstDrawInfo + 25)
#define sPLANETS							(sFirstDrawInfo + 26)
#define sBRUSSELS							(sFirstDrawInfo + 27)

// aspect info
#define sHARM								(sFirstDrawInfo + 28)
#define sCONJ								(sFirstDrawInfo + 29)
#define sTENS								(sFirstDrawInfo + 30)
#define sTOTAL								(sFirstDrawInfo + 31)
#define sSUMM								(sFirstDrawInfo + 32)
#define sAspectAbbrevCon					(sFirstDrawInfo + 33)
#define sAspectAbbrevOpp					(sFirstDrawInfo + 34)

#define sFIRE_EARTH							(sFirstDrawInfo + 35)
#define sAIR_WATER							(sFirstDrawInfo + 36)
#define sCAR_FIX_MUT						(sFirstDrawInfo + 37)
#define sYANG_YIN							(sFirstDrawInfo + 38)
#define sM_N_A_D							(sFirstDrawInfo + 39)
#define sANG_SUC_CAD						(sFirstDrawInfo + 40)
#define sLEARN_SHARE						(sFirstDrawInfo + 41)

#define	sHouseDispositorWheel				(sFirstDrawInfo + 42)
#define	sHouseDispositorHierarchy			(sFirstDrawInfo + 43)
#define	sSignDispositorWheel				(sFirstDrawInfo + 44)
#define	sSignDispositorHierarchy			(sFirstDrawInfo + 45)

// Dialogs

// Common dialogs
#define sFirstComDlg						(sSignDispositorHierarchy +  1)
#define sOPEN_MAIN_CHART					(sFirstComDlg +  0)
#define sOPEN_CHART							(sFirstComDlg +  1)
#define sSAVE_MAIN_CHART_DATA				(sFirstComDlg +  2)
#define sSAVE_CHART_POSITIONS				(sFirstComDlg +  3)
#define sSAVE_CHART_AS_TEXT_FILE			(sFirstComDlg +  4)
#define sSAVE_CHART_AS_BITMAP				(sFirstComDlg +  5)
#define sSAVE_CHART_AS_WMF					(sFirstComDlg +  6)
#define sSAVE_CHART_AS_POSTSCRIPT			(sFirstComDlg +  7)
#define sSAVE_CURRENT_SETTINGS				(sFirstComDlg +  8)

#define sWINDOW_NAME						(sFirstComDlg +  9)
#define sAPP_VER							(sFirstComDlg + 10)
#define sTRANSIT_OBJECT_RESTRICTIONS		(sFirstComDlg + 11)
#define sPROGRESSED_OBJECT_RESTRICTIONS		(sFirstComDlg + 12)
#define sENTER_CHART_INFO					(sFirstComDlg + 13)
#define sRFED								(sFirstComDlg + 14)
#define sABOUT								(sFirstComDlg + 15)
#define sOPEN_COMMAND_FILE					(sFirstComDlg + 16)
#define sDAT_MASK							(sFirstComDlg + 17)
#define sTXT_MASK							(sFirstComDlg + 18)
#define sBMP_MASK							(sFirstComDlg + 19)
#define sWMF_MASK							(sFirstComDlg + 20)
#define sPS_MASK							(sFirstComDlg + 21)

//Messages
#define sFirstMsg							(sPS_MASK +  1)
#define sINCOMPATIBLE_CONFIG_FILE			(sFirstMsg +  0)
#define sINCOMPATIBLE_FILE					(sFirstMsg +  1)
#define sINVALID_FILE						(sFirstMsg +  2)
#define sFILE_NOT_FOUND						(sFirstMsg +  3)
#define sDIRECTORIES_NOT_FOUND_OR_CREATED	(sFirstMsg +  4)
#define sCONFIGURATION_FILE_NOT_FOUND		(sFirstMsg +  5)
#define sINT_VALUE_NOT_VALID_SETTING		(sFirstMsg +  6)
#define sFLOAT_VALUE_NOT_VALID_SETTING		(sFirstMsg +  7)
#define sEXIT								(sFirstMsg +  8)
#define sPRINT_BACKROUND					(sFirstMsg +  9)
#define sHelpForumInternationalUrl			(sFirstMsg + 10)
#define sHelpForumUrl						(sFirstMsg + 11)
#define sReadMe								(sFirstMsg + 12)
#define sHelp								(sFirstMsg + 13)
#define sUsersGuide							(sFirstMsg + 14)
#define sWAtlas								(sFirstMsg + 15)


// Text headings
#define sFirstHeading						(sWAtlas +  1)
#define sChart								(sFirstHeading +  0)
#define sDualChart							(sFirstHeading +  1)
#define sSynastryChart						(sFirstHeading +  2)
#define sComposite							(sFirstHeading +  3)
#define sTimeSpaceMidpoint					(sFirstHeading +  4)
#define sFor								(sFirstHeading +  5)
#define sTransits							(sFirstHeading +  6)
#define sChartHdrSeconds					(sFirstHeading +  7)
#define sChartHdr							(sFirstHeading +  8)
#define sCFMT								(sFirstHeading +  9)
#define sHorizonHdr							(sFirstHeading + 10)
#define sHorizonHdr2						(sFirstHeading + 11)
#define sArabicHdr							(sFirstHeading + 12)
#define sArabicHdr2							(sFirstHeading + 13)
#define sAspectHdr							(sFirstHeading + 14)
#define sInfluenceHdr						(sFirstHeading + 15)

#define sFirstOwnDlg						(sInfluenceHdr +  1)

//Wizard
#define sWELCOME_WIZ_TITLE1					(sFirstOwnDlg +  0)
#define sWELCOME_WIZ_SUBTITLE1				(sFirstOwnDlg +  1)
#define sWELCOME_WIZ_TITLE2					(sFirstOwnDlg +  2)
#define sWELCOME_WIZ_SUBTITLE2				(sFirstOwnDlg +  3)
#define sWELCOME_WIZ_TITLE3					(sFirstOwnDlg +  4)
#define sWELCOME_WIZ_SUBTITLE3				(sFirstOwnDlg +  5)
#define sWELCOME_WIZ_TITLE4					(sFirstOwnDlg +  6)
#define sWELCOME_WIZ_SUBTITLE4				(sFirstOwnDlg +  7)

//Atlas
#define sWARNING_NOT_ALL_PLACES_SHOWN		(sFirstOwnDlg +  8)
#define sNO_PLACE_MATCHING_REQUIREMENTS		(sFirstOwnDlg +  9)
#define sNEED_CHOOSE_ATLAS_BEFORE_SEARCH	(sFirstOwnDlg + 10)
#define sCOUNTRY_LIST						(sFirstOwnDlg + 11)
#define sSTATE_LIST							(sFirstOwnDlg + 12)


//Search
#define sIN_FILE							(sFirstOwnDlg + 13)
#define sCHOOSE_DIRECTORY					(sFirstOwnDlg + 14)
#define sANY_ASPECT							(sFirstOwnDlg + 15)
#define sANY_OBJECT							(sFirstOwnDlg + 16)
#define sANY_SIGN							(sFirstOwnDlg + 17)
#define sSPEED_ABBREVIATION					(sFirstOwnDlg + 18)
#define sGAUQUELIN_SECTOR					(sFirstOwnDlg + 19)

#define sLast								sGAUQUELIN_SECTOR


