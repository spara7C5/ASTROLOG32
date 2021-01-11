/*
** Astrolog File: wdialog2.c
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
#include "shlobj.h"		// for SHBrowseForFolder
#include "shlobj.h"		// for SHBrowseForFolder
#include "gui.h"



/*
******************************************************************************
** Local Definitions
******************************************************************************
*/


/*
******************************************************************************
** Dialog Utility Functions.
******************************************************************************
*/

// See if directory entered by the user exists. If it does not exist attempt
// to create it if the create_dir flag is TRUE. If the directory could be found
// or created return TRUE and set parameter validated_directory to the full 
// directory name of the found or created directory, otherwise return FALSE.

static BOOL directory_found_or_created(HWND hdlg, int nIDDlgItem, BOOL create_dir, 
							  file_name_t validated_directory)
{
	file_name_t			full_path, temp_path;
	char				*dummy;
	int					i;


	GetEdit(nIDDlgItem, temp_path);

	if(!GetFullPathName(temp_path, sizeof(full_path), full_path, &dummy))
	{
		// user entered something that cannot be a directory, quit
		return FALSE;
	}

	if(GetFileAttributes(full_path) != FILE_ATTRIBUTE_DIRECTORY)
	{
		// the directory couldn't be found

		if(!create_dir)
		{
			// the user does not want to create the directory, quit
			return FALSE;
		}

		// we attempt to create the directory, starting at the root
		
		// copy drive, colon and path separator
		for(i = 0; i < 3; i++)
		{
			temp_path[i] = full_path[i];
		}

		while(TRUE)
		{
			// copy until next path separator or end of path
			for(; full_path[i] && full_path[i] != '\\' && full_path[i] != '/'; i++)
			{
				temp_path[i] = full_path[i];
			}

			temp_path[i] = 0;

			if(GetFileAttributes(temp_path) != FILE_ATTRIBUTE_DIRECTORY)
			{
				if(!CreateDirectory(temp_path, NULL))
				{
					// the directory couldn't be created, quit
					return FALSE;
				}
			}

			if(!full_path[i])
			{
				break; // end of directory string
			}

			temp_path[i] = full_path[i]; // copy the path separator
			i++;
		}
	}

	// if we got here, that means success
	strcpy(validated_directory, full_path);
	return TRUE;
}


/*
******************************************************************************
** Windows Dialogs.
******************************************************************************
*/


void ProcessDirectorySettings(HWND hdlg)
{
	char	invalid_dirs_list[300];
	char	*p = invalid_dirs_list;		// pointer to end of string
	char	*separator = ", ";
	BOOL	create_dir = GetCheck(dx01);

	*p = 0;	// empty string

	if(!directory_found_or_created(hdlg, dimain, create_dir, 
		dirs.main.dir))
	{
		
		p = StrCpy(StrCpy(p, dirs.main.name), separator);
	}
	
	if(!directory_found_or_created(hdlg, diephemeris, create_dir, 
		dirs.ephemeris.dir))
	{
		
		p = StrCpy(StrCpy(p, dirs.ephemeris.name), separator);
	}
	else
	{
		// we set the path even if it has not changed (it has low overhead)
		SetEphemerisPath();
	}
	
	if(!directory_found_or_created(hdlg, dicharts, create_dir, 
		dirs.charts.dir))
	{
		
		p = StrCpy(StrCpy(p, dirs.charts.name), separator);
	}
	
	if(!directory_found_or_created(hdlg, diinterpretation, create_dir, 
		dirs.interpretation.dir))
	{
		
		p = StrCpy(StrCpy(p, dirs.interpretation.name), separator);
	}
	
	if(!directory_found_or_created(hdlg, dimiscel, create_dir, 
		dirs.miscel.dir))
	{
		
		p = StrCpy(StrCpy(p, dirs.miscel.name), separator);
	}
	
	if(!directory_found_or_created(hdlg, diAmericanAtlas, create_dir, 
		dirs.american_atlas.dir))
	{
		
		p = StrCpy(StrCpy(p, dirs.american_atlas.name), separator);
	}
	
	if(!directory_found_or_created(hdlg, diInternationalAtlas, create_dir, 
		dirs.international_atlas.dir))
	{
		
		p = StrCpy(StrCpy(p, dirs.international_atlas.name), separator);
	}
	
	if(invalid_dirs_list[0])	// error?
	{
		*(p - 2) = '.';	// replace the last comma (separator) with a period
		PrintWarning(Lang(sDIRECTORIES_NOT_FOUND_OR_CREATED), invalid_dirs_list);
	}
}

int CALLBACK DlgDirectories(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD	wid = LOWORD(wParam);

	switch (message)
	{
	case WM_INITDIALOG:
		SetEdit(dimain,					dirs.main.dir);
		SetEdit(diephemeris,			dirs.ephemeris.dir);
		SetEdit(dicharts,				dirs.charts.dir);
		SetEdit(diinterpretation,		dirs.interpretation.dir);
		SetEdit(dimiscel,				dirs.miscel.dir);
		SetEdit(diAmericanAtlas,		dirs.american_atlas.dir);
		SetEdit(diInternationalAtlas,	dirs.international_atlas.dir);
		SetCheck(dx01, TRUE);			// create directories by default
		return TRUE;

	case WM_COMMAND:
		if (wid == IDOK)
		{
			ProcessDirectorySettings(hdlg);
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

// returns TRUE if derectory chosen
BOOL ChooseDirectory(HWND hwnd, LPSTR lpszTitle, BOOL ShowNewFolderButton, LPSTR pszPath)
{
   
	LPITEMIDLIST	pidlSelected = NULL;
    BROWSEINFO		bi = {0};
    LPMALLOC		pMalloc = NULL;
	BOOL			rc = FALSE;
	char			buffer[400];


	// this function will get confused with shortcuts, but I won't worry
	// about that because it is too much work for too little benefit
 
    if(SHGetMalloc(&pMalloc) == NOERROR)
	{
		bi.hwndOwner = hwnd;
		bi.pszDisplayName = buffer;	// returned string not used, as does not show full path
		bi.lpszTitle = lpszTitle;

		if (ShowNewFolderButton)
		{
			bi.ulFlags = 0;
		}
		else
		{
			bi.ulFlags = BIF_NONEWFOLDERBUTTON;
		}
		bi.ulFlags |= BIF_NEWDIALOGSTYLE;

		pidlSelected = SHBrowseForFolder(&bi);
		if(pidlSelected)
		{
			SHGetPathFromIDList(pidlSelected, pszPath);
			pMalloc->Free(pidlSelected);
			rc = TRUE;
		}

		pMalloc->Release();
	}

    return rc;
	
}

/* wdialog2.c */
