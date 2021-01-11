/*
** Astrolog File: atlas.c
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
#include "gui.h"


/*
******************************************************************************
** Local Definitions
******************************************************************************
*/




typedef struct
{
	// NOTE: THE ORDER OF FIELDS IN THIS STRUCTURE MUST MATCH THE ORDER USED
	// IN THE GRAPHICAL INTERFACE. THIS IS TO SIMPLIFY CODING BY MEANS OF A
	// UNION. SEE BELOW FOR DETAILS.

	// name type
	struct
	{
		BOOL N;				// native
		BOOL V;				// variant or alternate
		BOOL D;				// not verified
		BOOL C;				// conventional (foreign)
	} name_type;

	// Population classification - a graduated numerical scale denoting the 
	// relative importance of a populated place. The scale ranges from 1, 
	// relatively high, to 5, relatively low.
	struct
	{
		BOOL unclassified;	// unknown or undetermined classification (- in atlas files)
		BOOL c1;			// highest importance
		BOOL c2;
		BOOL c3;
		BOOL c4;
		BOOL c5;			// lowest importance
	} pop_class;

	// type of populated place
	struct
	{
		BOOL unclassified;	// PPL	populated place (- in atlas files)
		BOOL A;				// PPLA	seat of a first-order administrative division
		BOOL C;				// PPLC	capital of a political entity
		BOOL L;				// PPLL	populated locality
		BOOL Q;				// PPLQ	abandoned populated place
		BOOL R;				// PPLR	religious populated place
		BOOL S;				// PPLS	populated place
		BOOL W;				// PPLW	destroyed populated place	
		BOOL X;				// PPLX	section of populated place	
		BOOL other;			// Israeli settlementS, farms, etc  (# in atlas files)
	} place_type;

} int_atlas_individual_search_fields_t;

#define NUMBER_INT_ATLAS_SEARCH_FIELDS	(sizeof(int_atlas_individual_search_fields_t)/sizeof(BOOL))

// this union makes it possible to access the search fields either individually 
// or in bulk, with a for loop, to simplify coding
typedef union
{
	int_atlas_individual_search_fields_t	indiv;
	BOOL									bulk[NUMBER_INT_ATLAS_SEARCH_FIELDS];
} int_atlas_search_fields_t;

typedef struct
{
	// NOTE: THE ORDER OF FIELDS IN THIS STRUCTURE MUST MATCH THE ORDER USED
	// IN THE GRAPHICAL INTERFACE. THIS IS TO SIMPLIFY CODING BY MEANS OF A
	// UNION. SEE BELOW FOR DETAILS.

	BOOL ppl;		// populated places
	BOOL civ;		// administrative regions
	BOOL air;		// airports
	BOOL har;		// harbors
	BOOL prk;		// parks
	BOOL loc;		// locales
	BOOL mil;		// military places
	BOOL cem;		// cemetaries
	BOOL chu;		// churchs
	BOOL sch;		// schools 
	BOOL hos;		// hospitals
	BOOL pof;		// post offices
	BOOL bdg;		// buildings
	BOOL bch;		// beaches
	BOOL isl;		// islands

} usa_atlas_individual_search_fields_t;

#define NUMBER_USA_ATLAS_SEARCH_FIELDS	(sizeof(usa_atlas_individual_search_fields_t)/sizeof(BOOL))

// this union makes it possible to access the search fields either individually 
// or in bulk, with a for loop, to simplify coding
typedef union
{
	usa_atlas_individual_search_fields_t	indiv;
	BOOL									bulk[NUMBER_INT_ATLAS_SEARCH_FIELDS];
} usa_atlas_search_fields_t;

typedef struct
{
	usa_atlas_search_fields_t	usa_sf;		// american atlas search fields
	int_atlas_search_fields_t	int_sf;		// international atlas search fields
} atlas_adv_settings_t;						// atlas advanced settings

typedef enum
{
	ATLAS_TYPE_NOT_SET,
	ATLAS_TYPE_AMERICAN,
	ATLAS_TYPE_INTERNATIONAL,
} atlas_type_t;			

#define MAXIMUM_NUMBER_PLACES_DISPLAY	30000
#define MAX_NUMBER_REGIONS	400	// current maximum is 228 regions for UK



/*
******************************************************************************
** Auxiliary Functions
******************************************************************************
*/


// NOTE: REFERENCES TO "COUNTRY" ALSO APPLY TO USA STATES IN MOST CASES

// return true if could create list
static BOOL setup_country_list(HWND hdlg, atlas_type_t atlas_type, 
										int last_selected_country)
{
	FILE	*file;
	char	buffer[500];
	int		index;


	switch(atlas_type)
	{
	case ATLAS_TYPE_AMERICAN:
		file = FileOpen(Lang(sSTATE_LIST), dirs.american_atlas.dir, TRUE);
		if(!file)
		{
			return FALSE;
		}

		if(!CheckFileTypeAndVersion(file, Lang(sSTATE_LIST), 
			FILE_TYPE_COUNTRY_LIST, FILE_VERSION_COUNTRY_LIST))
		{
			fclose(file);	// failure - close file
			return FALSE;
		}
		break;

	case ATLAS_TYPE_INTERNATIONAL:
		file = FileOpen(Lang(sCOUNTRY_LIST), dirs.international_atlas.dir, TRUE);
		if(!file)
		{
			return FALSE;
		}

		if(!CheckFileTypeAndVersion(file, Lang(sCOUNTRY_LIST), 
			FILE_TYPE_COUNTRY_LIST, FILE_VERSION_COUNTRY_LIST))
		{
			fclose(file);	// failure - close file
			return FALSE;
		}
		break;

	default:
		return FALSE;
		break;
	}

	// erase previous list, if any
	SendDlgItemMessage(hdlg, lbCountry, LB_RESETCONTENT, 0, 0);

	while(fgets(buffer, sizeof(buffer), file))
	{
		if(*buffer == ';' || *buffer == '\n' || *buffer == ' ')
		{
			// skip comment and blank lines and lines starting with a blank
			continue;
		}

		buffer[strlen(buffer) - 1] = 0;			// remove terminating newline

		// Include existing files only
		char filename[16];	 
		sprintf(filename, "%c%c.atl", buffer[0], buffer[1]);
		FILE* countryFile = FileOpen(filename, (atlas_type == ATLAS_TYPE_INTERNATIONAL) ? dirs.international_atlas.dir : dirs.american_atlas.dir, false);
		if(countryFile) {
			fclose(countryFile);
			// display name of country, starting at fourth byte
			index = SendDlgItemMessage(hdlg, lbCountry, LB_ADDSTRING, 0, 
				(LPARAM)(LPCSTR) (buffer + 3));

			// save the first four characters of the line read - we are only 
			// interested in the first two that have the country code
			SendDlgItemMessage(hdlg, lbCountry, LB_SETITEMDATA, (WPARAM) index, 
				(LPARAM) *(int *) buffer);
		}
	}

	SendDlgItemMessage(hdlg, lbCountry, LB_SETCURSEL, 
		(WPARAM) (last_selected_country >= 0 ? last_selected_country : 0), 0);

	fclose(file);
	return TRUE;
}



// parse input buffer, and build output string into parameter outp
static void build_international_atlas_name_string(char *buffer, char *regions[], 
									int regions_count, char *outp)
{
	char	*p, *regp;
	short	region_code;
	int		i, n;

	// Skip search fields, population, longitude, latitude, and an extra unused 
	// field, to be used perhaps as altitude at a later date. Skip by counting 
	// five blanks.
	for(p = buffer, n = 0; ; p++)
	{
		if(*p == ' ')
		{
			if(++n == 5)
			{
				p++;
				break;
			}
		}
	}

	// save region code for later
	region_code = *p++;
	region_code |= (*p++) << 8;
	p++;								// skip blank

	// we are now ready to start building the string to put in the list box

	// copy name until end of line	
	for(; *p != '\n'; *outp++ = *p++);
	*outp++ = ' ';

	// Copy the name of the administrative region.
	// Some places may not have a valid region code, in that case 
	// we simply don't display it.
	// Region 00 is generic, usually the country name, so we don't 
	// bother with it.
	if(region_code != 0x3030) // 0x3030 is 00 in ASCII
	{
		// now search for the region
		for(i = 0; i < regions_count ; i++)
		{
			// We do the comparison with a short (two bytes at a time)
			// because it is faster than two individual bytes. 
			if(region_code == *(short *)(regions[i]))
			{
				*outp++ = '|';
				*outp++ = ' ';

				// copy name of region, skipping the region code 
				// at the beginning (2 bytes code + 1 blank = 3 bytes)
				for(regp = regions[i] + 3; *regp; *outp++ = *regp++);
				break;
			}
		}

		*outp++ = ' ';
	}

	*outp++ = '%';	// '%' marks the beginning of the extra info
	*outp++ = ' ';

	for(p = buffer; *p != ' '; *outp++ = *p++);	// copy search fields
	*outp++ = ' ';
	p++;								// skip blank

	// print population if available
	if(*p == '-')
	{
		p += 2;								// skip '-' plus blank
	}
	else
	{
		for(; *p != ' '; *outp++ = *p++);	// copy population
		*outp++ = ' ';
		p++;								// skip blank
	}
		
	*outp++ = '@';	// @ marks the beginning of the coordinates
	*outp++ = ' ';
	
	for(; *p != ' '; *outp++ = *p++);				// copy longitude
	p++;											// skip blank
	*outp++ = ' ';
	for(; *p != ' '; *outp++ = *p++);				// copy latitude

	*outp = 0;	// finally, terminate string
}

// parse input buffer, and build output string into parameter outp
static void build_american_atlas_name_string(char *buffer, char *outp)
{
	char	*p;

	// copy name from buffer + 4 (to skip search fields)
	for(p = buffer + 4; *p != '|'; *outp++ = *p++);
	*outp++ = ' ';

	if(*++p != '|')
	{
		// we have a subdivision of a county, copy it
		*outp++ = '|';
		*outp++ = ' ';
		for(; *p != '|'; *outp++ = *p++);
		*outp++ = ' ';
	}

	if(*++p != '|')
	{
		// we have a county name, copy it
		*outp++ = '|';
		*outp++ = ' ';
		for(; *p != '|'; *outp++ = *p++);
		*outp++ = ' ';
	}

	*outp++ = '%';	// '%' marks the beginning of the extra info
	*outp++ = ' ';

	for(; *buffer != '|'; *outp++ = *buffer++);	// copy search field

	*outp++ = ' ';
	*outp++ = '@';	// @ marks the beginning of the coordinates
	*outp++ = ' ';
	
	for(p++; *p != '|'; *outp++ = *p++);				// copy longitude
	for(p++, *outp++ = ' '; *p != '|'; *outp++ = *p++);	// copy latitude

	*outp = 0;	// finally, terminate string
}

// return TRUE if place satisfies search criteria, FALSE otherwise
static BOOL show_place_international_atlas(char *search_fields,
				atlas_adv_settings_t *atlas_adv_set)
{
	 register char search_field = *search_fields;

	// for each field, we check most frequent first, for efficiency

	// first check name type
	if(	search_field == 'N' && atlas_adv_set->int_sf.indiv.name_type.N ||
		search_field == 'V' && atlas_adv_set->int_sf.indiv.name_type.V ||
		search_field == 'D' && atlas_adv_set->int_sf.indiv.name_type.D ||
		search_field == 'C' && atlas_adv_set->int_sf.indiv.name_type.C)
	{
		search_field = *(search_fields + 1);

		// now check type of populated place
		if(	search_field == '-' && atlas_adv_set->int_sf.indiv.place_type.unclassified ||
			search_field == 'X' && atlas_adv_set->int_sf.indiv.place_type.X ||
			search_field == 'L' && atlas_adv_set->int_sf.indiv.place_type.L ||
			search_field == 'Q' && atlas_adv_set->int_sf.indiv.place_type.Q ||
			search_field == 'A' && atlas_adv_set->int_sf.indiv.place_type.A ||
			search_field == 'W' && atlas_adv_set->int_sf.indiv.place_type.W ||
			search_field == 'C' && atlas_adv_set->int_sf.indiv.place_type.C ||
			search_field == '#' && atlas_adv_set->int_sf.indiv.place_type.other ||
			search_field == 'S' && atlas_adv_set->int_sf.indiv.place_type.S ||
			search_field == 'R' && atlas_adv_set->int_sf.indiv.place_type.R)
		{
			search_field = *(search_fields + 2);

			// finally check population class
			if(	search_field == '-' && atlas_adv_set->int_sf.indiv.pop_class.unclassified ||
				search_field == '5' && atlas_adv_set->int_sf.indiv.pop_class.c5 ||
				search_field == '4' && atlas_adv_set->int_sf.indiv.pop_class.c4 ||
				search_field == '3' && atlas_adv_set->int_sf.indiv.pop_class.c3 ||
				search_field == '2' && atlas_adv_set->int_sf.indiv.pop_class.c2 ||
				search_field == '1' && atlas_adv_set->int_sf.indiv.pop_class.c1)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// return TRUE if place satisfies search criteria, FALSE otherwise
static BOOL show_place_american_atlas(char *search_fields,
				atlas_adv_settings_t *atlas_adv_set)
{
	 register int feature_type = *(int *) search_fields;

	// for efficiency we check most frequent features first, and we do the
	// comparisons as an integer - note: this is compiler dependent code
	if(	
		feature_type == *(int *) "ppl|" && atlas_adv_set->usa_sf.indiv.ppl ||
		feature_type == *(int *) "chu|" && atlas_adv_set->usa_sf.indiv.chu ||
		feature_type == *(int *) "sch|" && atlas_adv_set->usa_sf.indiv.sch ||
		feature_type == *(int *) "loc|" && atlas_adv_set->usa_sf.indiv.loc ||
		feature_type == *(int *) "cem|" && atlas_adv_set->usa_sf.indiv.cem ||
		feature_type == *(int *) "prk|" && atlas_adv_set->usa_sf.indiv.prk ||
		feature_type == *(int *) "civ|" && atlas_adv_set->usa_sf.indiv.civ ||
		feature_type == *(int *) "bdg|" && atlas_adv_set->usa_sf.indiv.bdg ||
		feature_type == *(int *) "pof|" && atlas_adv_set->usa_sf.indiv.pof ||
		feature_type == *(int *) "isl|" && atlas_adv_set->usa_sf.indiv.isl ||
		feature_type == *(int *) "air|" && atlas_adv_set->usa_sf.indiv.air ||
		feature_type == *(int *) "hos|" && atlas_adv_set->usa_sf.indiv.hos ||
		feature_type == *(int *) "bch|" && atlas_adv_set->usa_sf.indiv.bch ||
		feature_type == *(int *) "mil|" && atlas_adv_set->usa_sf.indiv.mil ||
		feature_type == *(int *) "har|" && atlas_adv_set->usa_sf.indiv.har 
		)
	{
		return TRUE;
	}

	return FALSE;
}

// returns true if could create list and is not empty
static BOOL create_places_list_international_atlas(HWND hdlg, FILE *country_file,
				int	initial_place, int max_display, char *search_string,
				atlas_adv_settings_t *atlas_adv_set)
{
	char		buffer[500], outbuffer[1000], *p, *s;
	char		*regions[MAX_NUMBER_REGIONS];
	int			i, n, length, places_count = 0, regions_count = 0;
	HCURSOR		hcursor;


	rewind(country_file);

	// show hourglass cursor
	hcursor = GetCursor();
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 

	// don't redraw list box while we create the list
	SendDlgItemMessage(hdlg, lbAtlasNames, WM_SETREDRAW, FALSE, 0); 

	// erase previous list, if any
	SendDlgItemMessage(hdlg, lbAtlasNames, LB_RESETCONTENT, 0, 0);

	// pre-allocate memory, 100 octets for each item, for efficiency
	SendDlgItemMessage(hdlg, lbAtlasNames, LB_INITSTORAGE, 
		(WPARAM) (int) max_display, (LPARAM) (DWORD) 100);

	// skip file version
	fgets(buffer, sizeof(buffer), country_file);

	// read list of administrative regions
	while(fgets(buffer, sizeof(buffer), country_file))
	{
		if(*buffer == ';' || *buffer == '\n' || *buffer == ' ')
		{
			// skip comment and blank lines and lines starting with a blank
			continue;
		}

		if(buffer[0] == '%' && buffer[1] == '%')
		{
			break;	// %% marks the end of the list
		}

		length = strlen(buffer);
		buffer[--length] = 0;		// remove terminating newline

		// save region code plus name
		regions[regions_count++] = 
			strcpy((char*)malloc(length + 1), buffer); // allocate space for terminating NULL
	}

	// Now read list of places. This needs to be very efficient code as we may
	// have to read 30000 places, depending on what the user wants. We don't 
	// want the user to wait long. We sacrifice safety for speed, if there is
	// bad input the program may well crash.

	while(fgets(buffer, sizeof(buffer), country_file))
	{
		if(*buffer == ';' || *buffer == '\n' || *buffer == ' ')
		{
			// skip comment and blank lines and lines starting with a blank
			continue;
		}

		if(initial_place >= 0)	// do block browse
		{
			if(places_count++ < initial_place)
			{
				continue;
			}

			if(places_count > (max_display + initial_place))
			{
				break;
			}
		}
		else	// do search
		{
			if(show_place_international_atlas(buffer, atlas_adv_set))
			{
				// Skip population, longitude, latitude, an extra unused field,
				// to be used perhaps as altitude at a later date, and the 
				// region code. Skip by counting five blanks.

				for(p = buffer + 4, n = 0; ; p++)	// buffer + 4 to skip search fields
				{
					if(*p == ' ')
					{
						if(++n == 5)
						{
							p++;
							break;
						}
					}
				}

				for(s = search_string; *s && *p; p++)
				{
					if(*p == *s || *p + 0x20 == *s)
					{
						s++;	// match
					}
					else if(*p >= 'A' && *p <= 'Z' || *p >= 'a' && *p <= 'z')
					{
						break;	
					}
					// else skip (for example, apostrophes, blanks, etc)
				}

				if(*s)
				{
					// disregard as it does not match the complete search string
					continue;	
				}

				if(places_count++ == max_display)
				{
					SendDlgItemMessage(hdlg, lbAtlasNames, LB_INSERTSTRING, 0, 
							(LPARAM)(LPCSTR) Lang(sWARNING_NOT_ALL_PLACES_SHOWN));
					break;
				}
			}
			else
			{
				continue;	// does not satisfy advanced search criteria - skip
			}
		}

		build_international_atlas_name_string(buffer, regions, regions_count, outbuffer);

		SendDlgItemMessage(hdlg, lbAtlasNames, LB_ADDSTRING, 0, 
			(LPARAM)(LPCSTR) outbuffer);		// show name string
	}

	// list finished, redraw box now
	SendDlgItemMessage(hdlg, lbAtlasNames, WM_SETREDRAW, TRUE, 0); 

	// restore cursor
	SetCursor(hcursor);

	for(i = 0; i < regions_count ; i++)
	{
		free(regions[i]);
	}

	if(places_count)
	{
		SendDlgItemMessage(hdlg, lbAtlasNames, LB_SETCURSEL, 0, 0); // select first place
		return TRUE;
	}
	else
	{
		SendDlgItemMessage(hdlg, lbAtlasNames, LB_INSERTSTRING, 0, 
				(LPARAM)(LPCSTR) Lang(sNO_PLACE_MATCHING_REQUIREMENTS));
		return FALSE;
	}
}

// returns true if could create list and is not empty
static BOOL create_places_list_american_atlas(HWND hdlg, FILE *country_file,
				int	initial_place, int max_display, char *search_string, 
				atlas_adv_settings_t *atlas_adv_set)
{
	char		buffer[500], outbuffer[1000], *p, *s;
	int			places_count = 0;
	HCURSOR		hcursor;


	rewind(country_file);

	// show hourglass cursor
	hcursor = GetCursor();
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 

	// don't redraw list box while we create the list
	SendDlgItemMessage(hdlg, lbAtlasNames, WM_SETREDRAW, FALSE, 0); 

	// erase previous list, if any
	SendDlgItemMessage(hdlg, lbAtlasNames, LB_RESETCONTENT, 0, 0);

	// Pre-allocate memory, 160 octets for each item, for efficiency.
	// The maximum name currently has 121 bytes, for the some of its three parts.
	// To this we need to add 6 bytes for the separators within the name, plus
	// 27 bytes for the feature type, longitude and latitude and their separators.
	// We get a grand total of 154, to which we add 6 for future expansion.
	SendDlgItemMessage(hdlg, lbAtlasNames, LB_INITSTORAGE, 
		(WPARAM) (int) max_display, (LPARAM) (DWORD) 160);

	// skip file versioin
	fgets(buffer, sizeof(buffer), country_file);

	// Now read list of places. This needs to be very efficient code as we may
	// have to read 30000 places, depending on what the user wants. We don't 
	// want the user to wait long. We sacrifice safety for speed, if there is
	// bad input the program may well crash.

	while(fgets(buffer, sizeof(buffer), country_file))
	{
		if(*buffer == ';' || *buffer == '\n' || *buffer == ' ')
		{
			// skip comment and blank lines and lines starting with a blank
			continue;
		}

		if(initial_place >= 0)	// do block browse
		{
			if(places_count++ < initial_place)
			{
				continue;
			}

			if(places_count > (max_display + initial_place))
			{
				break;
			}
		}
		else	// do search
		{
			if(show_place_american_atlas(buffer, atlas_adv_set))
			{
				for(s = search_string, p = buffer + 4; 
						*s && *p; p++) // buffer + 4 to skip search fields
				{
					if(*p == *s || *p + 0x20 == *s)
					{
						s++;	// match
					}
					else if(*p >= 'A' && *p <= 'Z' || *p >= 'a' && *p <= 'z')
					{
						break;	
					}
					// else skip (for example, apostrophes, blanks, etc)
				}

				if(*s)
				{
					// disregard as it does not match the complete search string
					continue;	
				}

				if(places_count++ == max_display)
				{
					SendDlgItemMessage(hdlg, lbAtlasNames, LB_INSERTSTRING, 0, 
							(LPARAM)(LPCSTR) Lang(sWARNING_NOT_ALL_PLACES_SHOWN));
					break;
				}
			}
			else
			{
				continue;	// does not satisfy advanced search criteria - skip
			}
		}

		build_american_atlas_name_string(buffer, outbuffer);

		SendDlgItemMessage(hdlg, lbAtlasNames, LB_ADDSTRING, 0, 
			(LPARAM)(LPCSTR) outbuffer);		// show name string
	}

	// list finished, redraw box now
	SendDlgItemMessage(hdlg, lbAtlasNames, WM_SETREDRAW, TRUE, 0); 

	// restore cursor
	SetCursor(hcursor);

	if(places_count)
	{
		SendDlgItemMessage(hdlg, lbAtlasNames, LB_SETCURSEL, 0, 0); // select first place
		return TRUE;
	}
	else
	{
		SendDlgItemMessage(hdlg, lbAtlasNames, LB_INSERTSTRING, 0, 
				(LPARAM)(LPCSTR) Lang(sNO_PLACE_MATCHING_REQUIREMENTS));
		return FALSE;
	}
}

// returns number of places in file
static int count_places_in_file(atlas_type_t atlas_type, FILE *country_file)
{
	char	buffer[500];
	int		places_count = 0;
	HCURSOR	hcursor;


	hcursor = GetCursor();
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 	// show hourglass cursor

	rewind(country_file);

	// skip file versioin
	fgets(buffer, sizeof(buffer), country_file);

	if(atlas_type == ATLAS_TYPE_INTERNATIONAL)
	{
		// skip the list of administrative regions

		while(fgets(buffer, sizeof(buffer), country_file))
		{
			if(*buffer == ';' || *buffer == '\n' || *buffer == ' ')
			{
				// skip comment and blank lines and lines starting with a blank
				continue;
			}

			if(buffer[0] == '%' && buffer[1] == '%')
			{
				break;	// %% marks the end of the list of administrative regions
			}
		}
	}

	while(fgets(buffer, sizeof(buffer), country_file))
	{
		if(*buffer == ';' || *buffer == '\n' || *buffer == ' ')
		{
			// skip comment and blank lines and lines starting with a blank
			continue;
		}

		places_count++;	// every other line contains a place name
	}

	SetCursor(hcursor);	// restore cursor

	return places_count;
}

// parse what we got from the atlas, and store in ci structure
static void parse_atlas_data(char *buffer, BOOL use_location_name_as_chart_name,
				BOOL append_co_to_county_name, BOOL append_usa_to_location_name, 
				char *country_name, CI *ci)
{
	int		n;
	BOOL	found_comma = FALSE;
	char	*p;

 
	if(use_location_name_as_chart_name)
	{
		// 	we take the first type of location name and use it as chart name

		for(p = ci->nam; *buffer != '%' && *buffer != '|'; buffer++, p++)
		{
			*p = *buffer;
		}

		*p = 0;			// terminate name string overwriting trailing blank

		if(*buffer == '|')
		{
			buffer += 2;	// skip '|' and following blank
		}
	}
	else
	{
		*ci->nam = 0;
	}

	// first parse name of place - we need to know the length of the name
	// as well as if it has a comma in it
	for(n = 0; *(buffer + n) != '%'; n++)
	{
		if(*(buffer + n) == ',')
		{
			found_comma = TRUE;
		}
	}

	if(!found_comma)
	{
		for(p = country_name; *p; p++)
		{
			if(*p == ',')
			{
				found_comma = TRUE;
			}
		}
	}

	// if we found a comma in the place or country name we use the vertical bar
	// as separator, otherwise a comma
	if(found_comma)
	{
		if(n)
		{
			strncpy(ci->loc, buffer, n - 1);
			ci->loc[n - 1] = 0;

			if(append_co_to_county_name)
			{
				strcat(ci->loc, " Co.");
			}

			strcat(ci->loc, " | ");
		}
		else
		{
			ci->loc[0] = 0;
		}

		strcat(ci->loc, country_name);

		if(append_usa_to_location_name)
		{
			strcat(ci->loc, " | USA");
		}
	}
	else
	{
		char	*dst, *src;


		if(n)
		{
			// we use the comma as separator, but we have to get rid of the 
			// extra blanks before the bars that we used in the list box
			for(src = buffer, dst = ci->loc; *src; src++)
			{
				if(*src == '%')
				{
					dst--;	// skip blank before '%'
					break;
				}
				else if(*src == '|')
				{
					// add comma separator between name and administrative region
					// skipping blank before '|'
					*(dst - 1) = ',';
				}
				else
				{
					*dst++ = *src;	// copy name
				}
			}

			*dst = 0; // terminate string

			if(append_co_to_county_name)
			{
				strcat(ci->loc, " Co.");
			}

			strcat(ci->loc, ", ");
		}
		else
		{
			ci->loc[0] = 0;
		}

		strcat(ci->loc, country_name);

		if(append_usa_to_location_name)
		{
			strcat(ci->loc, ", USA");
		}
	}

	for(buffer += n; *buffer != '@'; buffer++);	// skip extra info
	buffer += 2;								// skip '@' plus blank

	// longitude
	n = 0;
	for(; *(buffer + n) != ' '; n++);
	*(buffer + n) = 0;
	ci->lon = RParseSz(buffer, pmLon);

	// finally latitude
	ci->lat = RParseSz(buffer + n + 1, pmLat);	// string already terminated
}

// return true if file found and has valid type
static BOOL open_atlas_file_and_check_file_type(atlas_type_t atlas_type, 
						char *file_name, FILE **country_file)
{
	if(*country_file)
	{
		fclose(*country_file);
	}

	if(atlas_type == ATLAS_TYPE_AMERICAN)
	{
		*country_file = FileOpen(file_name, dirs.american_atlas.dir, TRUE);
		if(*country_file)
		{
			if(CheckFileTypeAndVersion(*country_file, file_name, 
				FILE_TYPE_USA_STATE_FILE, FILE_VERSION_USA_STATE_FILE))
			{
				return TRUE;
			}
			else
			{
				fclose(*country_file);
				*country_file = NULL;
			}
		}
	}
	else
	{
		*country_file = FileOpen(file_name, dirs.international_atlas.dir, TRUE);
		if(*country_file)
		{
			if(CheckFileTypeAndVersion(*country_file, file_name, 
				FILE_TYPE_COUNTRY_FILE, FILE_VERSION_COUNTRY_FILE))
			{
				return TRUE;
			}
			else
			{
				fclose(*country_file);
				*country_file = NULL;
			}
		}
	}


	return FALSE;
}


/*
******************************************************************************
** Windows Dialogs.
******************************************************************************
*/


// return FALSE through EndDialog if error or user cancelled, 
// otherwise TRUE (user sucessfully chose a place)
int CALLBACK DlgAtlas(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	// NOTE: REFERENCES TO "COUNTRY" ALSO APPLY TO USA STATES IN MOST CASES

	static atlas_adv_settings_t		atlas_adv_set;
	static atlas_type_t				atlas_type = ATLAS_TYPE_NOT_SET;
	static int						block_number = 0, last_block = -1;
	static int						last_selected_country = -1;	// -1 means not selected
	static int						max_names_display = 5000;
	static BOOL						list_created = FALSE;
	static BOOL						append_co_to_county_name = TRUE;
	static BOOL						state_abbrev_instead_state_name = TRUE;
	static BOOL						append_usa_to_location_name = FALSE;
	static CI						*ci;
	static char						country_name[80];
	static char						country_abbreviation[3];
	static FILE						*country_file = NULL;
	WORD							wid = LOWORD(wParam);
	WORD							message_type = HIWORD(wParam);
	int								i, index, new_max_display;
	int								newly_selected_country, places_count;
	char							file_name[100], buffer[300], *s, *d;


	if(!hdlg)	// called from WndProc as a result of WM_CREATE
	{
		// pre-initialisation, by default select everything

		for (i = 0; i < NUMBER_INT_ATLAS_SEARCH_FIELDS; i++)
		{
			atlas_adv_set.int_sf.bulk[i] = TRUE;
		}

		for (i = 0; i < NUMBER_USA_ATLAS_SEARCH_FIELDS; i++)
		{
			atlas_adv_set.usa_sf.bulk[i] = TRUE;
		}

		return FALSE;
	}

	switch (message)
	{
	case WM_INITDIALOG:

		SendDlgItemMessage(hdlg, pctAtlas, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
			(LPARAM)LoadBitmap(wi.hinst, MAKEINTRESOURCE(IDB_BITMAP3)));


		ci = (CI *) lParam;				// save return parameter
		SetEditN(deAtlasNDisplay, max_names_display);
		list_created = FALSE;

		// alow horizontal scrolling
		SendDlgItemMessage(hdlg, lbAtlasNames, LB_SETHORIZONTALEXTENT, 
			(WPARAM) 2000, 0);

		// Select atlas according to settings
		if(atlas_type == ATLAS_TYPE_NOT_SET) {
			if(us.useInternationalAtlas) {
				SetRadio(dxInternationalAtlas, dxAmericanAtlas, dxInternationalAtlas);
				atlas_type= ATLAS_TYPE_INTERNATIONAL;
			}
			else {
				SetRadio(dxAmericanAtlas, dxAmericanAtlas, dxInternationalAtlas);
				atlas_type= ATLAS_TYPE_AMERICAN;
			}
		}

		if(atlas_type != ATLAS_TYPE_NOT_SET)
		{
			if(setup_country_list(hdlg, atlas_type, 
				last_selected_country))
			{
				if(atlas_type == ATLAS_TYPE_AMERICAN)
				{
					SetRadio(dxAmericanAtlas, dxAmericanAtlas, 
						dxInternationalAtlas);
					EnableWindow(GetDlgItem(hdlg, dx02), TRUE);
					EnableWindow(GetDlgItem(hdlg, dx03), TRUE);
					EnableWindow(GetDlgItem(hdlg, dx04), TRUE);
					SetCheck(dx02, append_co_to_county_name);
					SetCheck(dx03, state_abbrev_instead_state_name);
					SetCheck(dx04, append_usa_to_location_name);
				}
				else
				{
					SetRadio(dxInternationalAtlas, dxAmericanAtlas, 
						dxInternationalAtlas);
				}
				SetFocus(GetDlgItem(hdlg, lbCountry));
				return FALSE;
			}
			else
			{
				EndDialog(hdlg, FALSE); // error
			}
		}
		return TRUE;
		break;
		
	case WM_COMMAND:
		switch(wid)
		{
		case dxAmericanAtlas:
			if(setup_country_list(hdlg, ATLAS_TYPE_AMERICAN, 
				last_selected_country))
			{
				if(atlas_type == ATLAS_TYPE_INTERNATIONAL)
				{
					last_selected_country = -1;
				}
				atlas_type = ATLAS_TYPE_AMERICAN;
				EnableWindow(GetDlgItem(hdlg, dx02), TRUE);
				EnableWindow(GetDlgItem(hdlg, dx03), TRUE);
				EnableWindow(GetDlgItem(hdlg, dx04), TRUE);
				SetCheck(dx02, append_co_to_county_name);
				SetCheck(dx03, state_abbrev_instead_state_name);
				SetCheck(dx04, append_usa_to_location_name);
				SetRadio(dxAmericanAtlas, dxAmericanAtlas, dxInternationalAtlas);
				SetFocus(GetDlgItem(hdlg, lbCountry));
			}
			else
			{
				if(country_file)
				{
					fclose(country_file);
					country_file = NULL;
				}
				EndDialog(hdlg, FALSE);	// error
			}
			break;

		case dxInternationalAtlas:
			if(setup_country_list(hdlg, ATLAS_TYPE_INTERNATIONAL, 
				last_selected_country))
			{
				if(atlas_type == ATLAS_TYPE_AMERICAN)
				{
					last_selected_country = -1;
				}

				atlas_type = ATLAS_TYPE_INTERNATIONAL;
				EnableWindow(GetDlgItem(hdlg, dx02), FALSE);
				EnableWindow(GetDlgItem(hdlg, dx03), FALSE);
				EnableWindow(GetDlgItem(hdlg, dx04), FALSE);
				SetRadio(dxInternationalAtlas, dxAmericanAtlas, dxInternationalAtlas);
				SetFocus(GetDlgItem(hdlg, lbCountry));
			}
			else
			{
				if(country_file)
				{
					fclose(country_file);
					country_file = NULL;
				}
				EndDialog(hdlg, FALSE);	// error
			}
			break;
			
		case dbAtlasAdvancedSettings:
			switch(atlas_type)
			{
			case ATLAS_TYPE_AMERICAN:
				DialogBoxParam(Lang.resource, MAKEINTRESOURCE(dlgAmericanAtlasAdvSettings), 
					wi.hwnd, DlgAmericanAtlasAdvSettings, (LPARAM) &atlas_adv_set);
				break;

			case ATLAS_TYPE_INTERNATIONAL:
				DialogBoxParam(Lang.resource, MAKEINTRESOURCE(dlgInterAtlasAdvSettings), 
					wi.hwnd, DlgInterAtlasAdvSettings, (LPARAM) &atlas_adv_set);
				break;

			case ATLAS_TYPE_NOT_SET:
			default:
				PrintWarning(Lang(sNEED_CHOOSE_ATLAS_BEFORE_SEARCH));
				break;
			}

			break;
						
		case lbCountry:
			if(message_type != LBN_DBLCLK)
			{
				return FALSE;	
			}
			// Double click will be equivalent to dbAtlasFirstBlock. 
			// Drop through.
						
		case dbAtlasFirstBlock:
		case dbAtlasPreviousBlock:
		case dbAtlasMiddleBlock:
		case dbAtlasNextBlock:
		case dbAtlasLastBlock:
		case dbSearchAtlas:
			if(atlas_type == ATLAS_TYPE_NOT_SET)
			{
				PrintWarning(Lang(sNEED_CHOOSE_ATLAS_BEFORE_SEARCH));
				break;
			}
			
			new_max_display = GetEditN(deAtlasNDisplay);
			if(new_max_display < 1 || new_max_display > MAXIMUM_NUMBER_PLACES_DISPLAY)
			{
				GetDlgItemText(hdlg, dstAtlasNDisplay, buffer, sizeof(buffer));
				WarnWrongValue(new_max_display, buffer);
				break;
			}

			if(new_max_display != max_names_display)
			{
				max_names_display = new_max_display;
				block_number = 0;
				last_block = -1;
			}
			
			// find out the country the user selected, and build the file name
			newly_selected_country = SendDlgItemMessage(hdlg, lbCountry, 
				LB_GETCURSEL, 0, 0);
			if(!country_file || newly_selected_country != last_selected_country)
			{
				int country_code;

				last_selected_country = newly_selected_country;
				block_number = 0;
				last_block = -1;

				country_code = SendDlgItemMessage(hdlg, lbCountry, 
					LB_GETITEMDATA, (WPARAM) last_selected_country, 0);
				
				// the first two characters have the country code, add the 
				// extension ".atl" to that
				strncpy(file_name, (char *) &country_code, 2); 
				strcpy(file_name + 2, ".atl");

				if(!open_atlas_file_and_check_file_type(atlas_type, 
					file_name, &country_file))
				{
					EndDialog(hdlg, FALSE);	// error
					break;
				}

				// save country name to store in chart data later
				SendDlgItemMessage(hdlg, lbCountry, LB_GETTEXT, 
					(WPARAM) newly_selected_country, (LPARAM) (LPCTSTR) country_name);

				strncpy(country_abbreviation, file_name, 2); 
				country_abbreviation[0] -= 0x20;		// convert to uppercase
				country_abbreviation[1] -= 0x20;
				country_abbreviation[2] = 0;			// terminate string
			}

			if(last_block == -1)
			{
				places_count = count_places_in_file(atlas_type, country_file);
				if(places_count)
				{
					// If places_count is an exact multiple of
					// max_names_display we need one less block.
					// Subtracting one to places_count accomplishes that.
					places_count--;
				}
				last_block = places_count / max_names_display;
			}

			switch(wid)
			{
			case lbCountry:	// double-click means choose first block
			case dbAtlasFirstBlock:
				block_number = 0;
				break;

			case dbAtlasPreviousBlock:
				if(block_number)
				{
					--block_number;
				}
				break;

			case dbAtlasMiddleBlock:
				block_number = last_block / 2;
				break;

			case dbAtlasNextBlock:
				if(block_number < last_block)
				{
					block_number++;
				}
				break;

			case dbAtlasLastBlock:
				block_number = last_block;
				break;

			case dbSearchAtlas:
				GetEdit(deAtlasSearchLetters, buffer);
				for(s = buffer, d = buffer; *s; s++)
				{
					if(*s >= 'A' && *s <= 'Z')
					{
						*d++ = *s + 0x20;	// convert upper to lower case
					}
					else if(*s >= 'a' && *s <= 'z')
					{
						*d++ = *s;			// just copy if lower case
					}
					// else skip (for example, apostrophes, blanks, etc)
				}
				*d = 0;	// terminate string

				block_number = -1;	// -1 means do block search instead of block browse
				break;
			}

			if(atlas_type == ATLAS_TYPE_AMERICAN)
			{
				list_created = create_places_list_american_atlas(hdlg, 
					country_file, block_number * max_names_display, 
					max_names_display, buffer, &atlas_adv_set);
			}
			else
			{
				list_created = create_places_list_international_atlas(hdlg, 
					country_file, block_number * max_names_display, 
					max_names_display, buffer, &atlas_adv_set);
			}

			SetFocus(GetDlgItem(hdlg, lbAtlasNames));
			break;
			
		case lbAtlasNames:
			if(message_type != LBN_DBLCLK)
			{
				return FALSE;
			}
			// Double clicks will be equivalent to OK. 
			// Drop through.

		case IDOK:
			if(country_file)
			{
				fclose(country_file);
				country_file = NULL;
			}

			// we update max_names_display in case the user did not press
			// any of the search buttons
			new_max_display = GetEditN(deAtlasNDisplay);
			if(new_max_display < 1 || new_max_display > MAXIMUM_NUMBER_PLACES_DISPLAY)
			{
				GetDlgItemText(hdlg, dstAtlasNDisplay, buffer, sizeof(buffer));
				WarnWrongValue(new_max_display, buffer);
				break;
			}
			max_names_display = new_max_display;

			if(atlas_type == ATLAS_TYPE_AMERICAN)
			{
				append_co_to_county_name = GetCheck(dx02);
				state_abbrev_instead_state_name = GetCheck(dx03);
				append_usa_to_location_name = GetCheck(dx04);
			}

			if(list_created)
			{
				index = SendDlgItemMessage(hdlg, lbAtlasNames, LB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hdlg, lbAtlasNames, LB_GETTEXT, 
					(WPARAM) index, (LPARAM) (LPCTSTR) buffer);

				if(*buffer != '>')	// disregard warnings that start with '>'
				{
					if(atlas_type == ATLAS_TYPE_AMERICAN) 
					{
						parse_atlas_data(buffer, GetCheck(dx01), 
							append_co_to_county_name, append_usa_to_location_name, 
							state_abbrev_instead_state_name ? country_abbreviation : 
							country_name, ci);
					}
					else
					{
						parse_atlas_data(buffer, GetCheck(dx01), FALSE, FALSE,
							country_name, ci);
					}
					
					EndDialog(hdlg, TRUE);	// success (user chose a place)
					return TRUE;
				}
			}

			EndDialog(hdlg, FALSE); // no place chosen
			break;
			
		case IDCANCEL:
			if(country_file)
			{
				fclose(country_file);
				country_file = NULL;
			}
			EndDialog(hdlg, FALSE); // user cancelled
			break;

		default:	// ID for WM_COMMAND
			return FALSE;
			break;

		}	// end switch(wid) for case WM_COMMAND
		
		return TRUE;	// end WM_COMMAND
		break;
		
	default:	// message type
		break;

	}	// end switch (message)

	return FALSE;
}


// return FALSE through EndDialog if error or user cancelled, 
// otherwise TRUE (user chose new settings)
int CALLBACK DlgInterAtlasAdvSettings(HWND hdlg, UINT message, 
										  WPARAM wParam, LPARAM lParam)
{
	static atlas_adv_settings_t	*atlas_adv_set;
	WORD							wid = LOWORD(wParam);
	int								i;

	
	switch (message)
	{
	case WM_INITDIALOG:
		atlas_adv_set = (atlas_adv_settings_t *) lParam; // save return parameter

		for (i = 0; i < NUMBER_INT_ATLAS_SEARCH_FIELDS; i++)
		{
			SetCheck(dx01 + i, atlas_adv_set->int_sf.bulk[i]);
		}
		return TRUE;
		break;
		
	case WM_COMMAND:
		switch(wid)
		{
		case dbSelectAll:	// type of names
			for (i = 0; i < 4; i++)
			{
				SetCheck(dx01 + i, TRUE);
			}
			break;

		case dbNoSelection:
			for (i = 0; i < 4; i++)
			{
				SetCheck(dx01 + i, FALSE);
			}
			break;

		case dbSelectAll2:	 // population class
			for (i = 4; i < 10; i++)
			{
				SetCheck(dx01 + i, TRUE);
			}
			break;

		case dbNoSelection2:
			for (i = 4; i < 10; i++)
			{
				SetCheck(dx01 + i, FALSE);
			}
			break;

		case dbSelectAll3:	// type of populated place
			for (i = 10; i < NUMBER_INT_ATLAS_SEARCH_FIELDS; i++)
			{
				SetCheck(dx01 + i, TRUE);
			}
			break;

		case dbNoSelection3:
			for (i = 10; i < NUMBER_INT_ATLAS_SEARCH_FIELDS; i++)
			{
				SetCheck(dx01 + i, FALSE);
			}
			break;

		case IDOK:
			for (i = 0; i < NUMBER_INT_ATLAS_SEARCH_FIELDS; i++)
			{
				atlas_adv_set->int_sf.bulk[i] = GetCheck(dx01 + i);
			}

			EndDialog(hdlg, TRUE);	// success 
			break;
			
		case IDCANCEL:
			EndDialog(hdlg, FALSE); // user cancelled
			break;
		}
		return TRUE;
		break;
		
	default:
		break;
	}

	return FALSE;
}

// return FALSE through EndDialog if error or user cancelled, 
// otherwise TRUE (user chose new settings)
int CALLBACK DlgAmericanAtlasAdvSettings(HWND hdlg, UINT message, 
											 WPARAM wParam, LPARAM lParam)
{
	static atlas_adv_settings_t	*atlas_adv_set;
	WORD						wid = LOWORD(wParam);
	int							i;

	
	switch (message)
	{
	case WM_INITDIALOG:
		atlas_adv_set = (atlas_adv_settings_t *) lParam; // save return parameter

		for (i = 0; i < NUMBER_USA_ATLAS_SEARCH_FIELDS; i++)
		{
			SetCheck(dx01 + i, atlas_adv_set->usa_sf.bulk[i]);
		}
		return TRUE;
		break;
		
	case WM_COMMAND:
		switch(wid)
		{
		case dbSelectAll:	// type of names
			for (i = 0; i < NUMBER_USA_ATLAS_SEARCH_FIELDS; i++)
			{
				SetCheck(dx01 + i, TRUE);
			}
			break;

		case dbNoSelection:
			for (i = 0; i < NUMBER_USA_ATLAS_SEARCH_FIELDS; i++)
			{
				SetCheck(dx01 + i, FALSE);
			}
			break;

		case IDOK:
			for (i = 0; i < NUMBER_USA_ATLAS_SEARCH_FIELDS; i++)
			{
				atlas_adv_set->usa_sf.bulk[i] = GetCheck(dx01 + i);
			}

			EndDialog(hdlg, TRUE);	// success 
			break;
			
		case IDCANCEL:
			EndDialog(hdlg, FALSE); // user cancelled
			break;
		}
		return TRUE;
		break;
		
	default:
		break;
	}

	return FALSE;
}

/* atlas.c */
