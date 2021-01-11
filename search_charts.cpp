/*
** Astrolog File: search_charts.c
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
#include "gui.h"


#ifdef _DEBUG

// #define DEBUG_SEARCH

#endif

/*
******************************************************************************
** Local Definitions
******************************************************************************
*/

// validation
//FIX sIN_FILE missing!! #define ValidateN(n, f, sz, file) if (!(f)) {PrintWarning(Lang(sINT_VALUE_NOT_VALID_SETTING) Lang(sIN_FILE), n, sz, file); break; }
#define ValidateN(n, f, sz, file) if (!(f)) {PrintWarning(Lang(sINT_VALUE_NOT_VALID_SETTING), n, sz); break; }
#define ValidateR(r, f, sz, file) ValidateN((int)r, f, sz, file)

typedef enum
{
	SEARCH_MOVEMENT_DIRECT,
	SEARCH_MOVEMENT_RETROGRADE,
	SEARCH_MOVEMENT_EITHER,
} search_movement_t;

// search options
typedef struct
{
	BOOL					search_subdirs;

	// for chart name search
	struct
	{
		BOOL				search; 
		char				string[30];
	} string_in_name;

	// for object in sign search
	struct
	{
		BOOL				search; 
		objects_t			object;	// -1 means any object, otherwise it is object number
		int					sign;	// 1-12 are signs, 13-16 are elements, 17-19 are qualities, 20 means any sign
		search_movement_t	direction;
	} object_in_sign;

	// for object in aspect search
	struct
	{
		BOOL				search; 
		objects_t			object1;
		aspects_t			aspect;	// values 1-18 are aspects, value 0 means any selected aspect
		objects_t			object2;
	} object_in_aspect;

	// for object in house search
	struct
	{
		BOOL				search; 
		objects_t			object;
		int					house;
		house_system_t		house_system;
	} object_in_house;

	// for object in Gauquelin sector
	struct
	{
		BOOL				search; 
		objects_t			object;
		int					sector;
	} object_in_gauquelin_sector;
} search_options_t;





/*
******************************************************************************
** Auxiliary Functions
******************************************************************************
*/

#define MAGIC_NUMBER(chart) ((~((int) chart) << 1) + 1)

// allocate or free a chart in a heap, or get previous or last chart
// the meaning of parameters and return value depend on the chart operation
CI *ChartList(chart_ops_t operation, int heap_number, CI *ref_chart)
{
	static chart_unit_t	*last_chart[NO_CHART_HEAPS] = {NULL};	// points to the top of chart stack
	chart_unit_t		*chart = NULL; 


	if(heap_number >= 0 && heap_number < NO_CHART_HEAPS)
	{
		switch(operation)
		{
		case CHART_CREATE:
			// allocate memory for chart, zero it and store at top of private heap
			// paramater ref_chart is not used
			// return pointer to chart allocated, or NULL if could not allocate
			chart = (_chart_unit*)malloc(sizeof (chart_unit_t));
			if(chart)
			{
				memset(chart, 0, sizeof (chart_unit_t));
				chart->previous = last_chart[heap_number];
				if(last_chart[heap_number])
				{
					last_chart[heap_number]->next = chart;
				}
				last_chart[heap_number] = chart;
				chart->magic_number = MAGIC_NUMBER(chart);
			}
			break;

		case CHART_DESTROY:
			// frees memory for chart at top of heap
			// paramater ref_chart is not used
			// return pointer to previous chart in heap, or NULL if no more charts left
			if(last_chart[heap_number])
			{
				last_chart[heap_number]->magic_number = 0;
				chart = last_chart[heap_number]->previous;
				free(last_chart[heap_number]);
				last_chart[heap_number] = chart;
			}
			break;

		case CHART_PREVIOUS:
			// paramater ref_chart is the chart for which the user wants to find out a previous chart
			// return pointer to previous chart in heap, or NULL if no previous chart exist
			if(last_chart[heap_number])
			{
				chart_unit_t *ch = (chart_unit_t *) ref_chart;

				if(ch && ch->magic_number == MAGIC_NUMBER(ch))
				{
					if(ch->previous && (ch->previous)->next == ch && 
						(ch->previous)->magic_number == MAGIC_NUMBER(ch->previous))
					{
						chart = ch->previous;
					}
				}
			}
			break;

		case CHART_NEXT:
			// paramater ref_chart is the chart for which the user wants to find out a next chart
			// return pointer to next chart in heap, or NULL if no next chart exist
			if(last_chart[heap_number])
			{
				chart_unit_t *ch = (chart_unit_t *) ref_chart;

				if(ch && ch->magic_number == MAGIC_NUMBER(ch))
				{
					if(ch->next && (ch->next)->previous == ch && 
						(ch->next)->magic_number == MAGIC_NUMBER(ch->next))
					{
						chart = ch->next;
					}
				}
			}
			break;

		default :
			break;
		}
	}
	
	return (CI *) chart;
}

// open chart file, read chart data, store it in chart structure, then close file
// returns TRUE if success
static BOOL read_file_data(char *szFile, CI *chart)
{
	FILE		*file;
	char		*argv[MAXSWITCHES];
	int 		argc;
	char		*mode = "r";
	char		buffer[1000];
	BOOL		rc = FALSE;


	file = fopen(szFile, mode);
	if (!file)
	{
		PrintWarning("Couldn't open file %s", szFile);
	}
	else
	{
		// make sure the file type is correct
		if(fgets(buffer, sizeof(buffer), file) && *buffer == '@' && strlen(buffer) >= 5 &&
			!strncmp(FILE_TYPE_CHART, buffer + 1, 2) && !strncmp(FILE_VERSION_CHART, buffer + 3, 2))
		{
			// read input file, one line at a time
			while(fgets(buffer, sizeof(buffer), file))	
			{
				argc = NParseCommandLine(buffer, argv);

				if(argc > 1)
				{
					if(!strcmp(argv[1], "/zi"))
					{
						if (argc < 4)
						{
							PrintWarning("Invalid parameter for /zi in %s", szFile);
							break;
						}

						if(*argv[2])
						{
							strcpy(chart->nam, argv[2]);
						}

						if(*argv[3])
						{
							strcpy(chart->loc, argv[3]);
						}

						rc = TRUE;

						// skip rest of line, if any
					}
					else if(!strcmp(argv[1], "/qb"))
					{
						if (argc < 10)
						{
							PrintWarning("Missing parameter(s) for /qb in %s", szFile);
							break;
						}

						chart->mon = NParseSz(argv[2], pmMonEn);
						chart->day = NParseSz(argv[3], pmDay);
						chart->yea = NParseSz(argv[4], pmYea);
						chart->tim = RParseSz(argv[5], pmTim);
						chart->dst = RParseSz(argv[6], pmDst);
						chart->zon = RParseSz(argv[7], pmZon);
						chart->lon = RParseSz(argv[8], pmLon);
						chart->lat = RParseSz(argv[9], pmLat);

						// range check

						ValidateN(chart->mon, FValidMon(chart->mon), "month", szFile);
						ValidateN(chart->yea, FValidYea(chart->yea), "year", szFile);
						ValidateN(chart->day, FValidDay(chart->day, chart->mon, chart->yea), "day", szFile);
						ValidateR(chart->tim, FValidTim(chart->tim), "time", szFile);
						ValidateR(chart->dst, FValidZon(chart->dst), "daylight", szFile);
						ValidateR(chart->zon, FValidZon(chart->zon), "zone", szFile);
						ValidateR(chart->lon, FValidLon(chart->lon), "longitude", szFile);
						ValidateR(chart->lat, FValidLat(chart->lat), "latitude", szFile);

						rc = TRUE;	// if we got this far, the chart data is valid
					}
				}
			};	// while(fgets(buffer, sizeof(buffer), file))	
		}	// correct file type

#ifdef DEBUG_SEARCH
		else
		{
			PrintWarning("Incorrect file type %s", szFile);
		}
#endif


		fclose(file);	
	}	// could open file

	return rc;
}

static BOOL search_planet_in_sign_and_speed(int obj, double longitude, double speed, 
								  int chosen_sign, search_movement_t chosen_direction)
{
	BOOL	found = FALSE;
	int		sign = ((int) longitude) / 30 + 1;


	if(chosen_sign == 20)	// 20 means any sign
	{
		found = TRUE;	
	}
	else if(chosen_sign == sign)
	{
		found = TRUE;	// object is in sign
	}
	else if(chosen_sign >= 13 && chosen_sign <= 16)
	{
		int element = chosen_sign - 13; // zero based element

		if(((sign - 1) % 4) == element)
		{
			found = TRUE;	// object is in element
		}
	}
	else if(chosen_sign >= 17 && chosen_sign <= 19)
	{
		int quality = chosen_sign - 17; // zero based quality

		if(((sign - 1) % 3) == quality)
		{
			found = TRUE;	// object is in quality (cardinal, fixed, mutable)
		}
	}

	// now check speed
	if(found && chosen_direction != SEARCH_MOVEMENT_EITHER)
	{
		if(obj >= oEar && obj <= cThing || obj >= uranLo && obj <= uranHi)
		{
			if(speed >= 0.0 && chosen_direction == SEARCH_MOVEMENT_RETROGRADE ||
			   speed < 0.0 && chosen_direction == SEARCH_MOVEMENT_DIRECT)
			{
				found = FALSE;
			}
		}
	}

	return found;
}

// If chart is NULL, get longitude and speed for an object.
// If chart if not NULL, then initialize variables that will be used in later calls 
// to function, and take object as meaning the house system.
// Returns TRUE if success
static BOOL get_object_info(CI *chart, int object, double *longitude, double *speed)
{
	double			jd;
	static double	jde, cusps[13], ascmc[10];
	static BOOL		bad_houses;
	BOOL			rc = TRUE;
	double			SE_coordinates[6];
	int				house_system;

	
	if(chart)
	{
		// initialize variables that will be used in later calls 

		// calculate julian time
		jd = MdytszToJulian(chart->mon, chart->day, chart->yea, chart->tim, chart->dst, chart->zon)
			- 0.5;

		// calculate julian ephemeris time
		jde = jd + swe_deltat(jd);

		// we take the object as meaning the house system
		house_system = AstrologHouse2SeHouse(object);
		if(house_system == -1)
		{
			house_system = 'E'; // should never happen
		}

		// calculate houses, ascendant, midheaven, etc
		if(swe_houses(jd, DegMin2DecDeg(chart->lat), -DegMin2DecDeg(chart->lon), 
			house_system, cusps, ascmc) == ERR)
		{
			// Looking at SE code, we will find an error is returned for Placidus, Koch and Alcabitius at
			// polar latitudes, even if the other parameters are calculated well. In this case
			// we recalculate the ascendant, midheaven, etc, as if for equal houses.
			
			bad_houses = TRUE;
			swe_houses(jd, DegMin2DecDeg(chart->lat), -DegMin2DecDeg(chart->lon), 'E', cusps, ascmc);
		}
		else
		{
			bad_houses = FALSE;
		}
	}
	else
	{
		if(object >= 1001 && object <= 1012)	// hack until have proper object ID for house cusps
		{
			if(bad_houses)
			{
				rc = FALSE;
			}
			else
			{
				*longitude = cusps[object - 1000];
			}
		}
		else
		{
			switch(object)
			{
			case oVtx:
				*longitude = ascmc[SE_VERTEX];
				break;

			case oEP:
				*longitude = ascmc[SE_EQUASC];
				break;

			case oAsc:
				*longitude = ascmc[SE_ASC];
				break;

			case oMC:
				*longitude = ascmc[SE_MC];
				break;

			default :
				if (CalculatePlanetSE(object, jde, us.objCenter != oEar, SE_coordinates))
				{
					*longitude = Mod(SE_coordinates[0] + is.rSid);

					if(speed)
					{
						*speed = SE_coordinates[3]; // speed in longitude, degrees per day
					}
				}
				else
				{
					rc = FALSE;
				}
				break;
			}
		}
	}

	return rc; 
}

static BOOL search_planet_in_aspect(int object1, double longitude1, 
			int object2, double longitude2, int *aspect, double *orb)
{
	BOOL		found = FALSE;
	int			asp;
	double		torb;


	// to see if two planets are in aspect, take the longitudes of both,
	// calculate the minimum distance, then compare it the aspect, 
	// considering the configured orb

	if(*aspect > 0)
	{
		// search individual aspect
		asp = *aspect;

		torb = MinDistance(longitude1, longitude2) - rAspAngle[asp];

		if(fabs(torb) <= GetOrbEx(object1, object2, asp))
		{
			found = TRUE;
		}
	}
	else
	{
		// search all selected aspects
		for(asp = 1; asp <= cAspect; asp++)
		{
			if (!ignoreA[asp])
			{
				torb = MinDistance(longitude1, longitude2) - rAspAngle[asp];

				if(fabs(torb) <= GetOrbEx(object1, object2, asp))
				{
					found = TRUE;
					break;
				}
			}
		}
	}

	if(found)
	{
		*aspect = asp;
		*orb = torb;
	}

	return found;
}

// Decide if a chart satisfies the search criteria. If it does, put the name 
// and other pertinent information in buffer, to be presented to the user
// by the calling function.
// Return TRUE if the chart is good.
static BOOL filter_chart(HWND hdlg, CI *chart, search_options_t *search_options, char *buffer)
{
	int 		chars_written;
	BOOL		good_chart = FALSE;
	int			house_system;

#ifdef DEBUG_SEARCH
	static int	count = 0;
#endif


	while (TRUE)	// fake while loop, just so that we can break from the middle
	{
		// we now go through several filters one after another, if any fails we bail out

		if(!chart->nam || !*chart->nam)
		{
			break;	// we only do charts with names
		}
	
		house_system = search_options->object_in_house.house_system;
		if(house_system == -1 || !search_options->object_in_house.search)
		{
			// default to equal houses if house search not chosen, or first time around,
			// this is to avoid flipping the ascendant in polar regions if no houses are
			// calculated.
			house_system = cHouseEqual;
		}

		// initialize function
		get_object_info(chart, house_system, NULL, NULL);

		// chars_written = sprintf(buffer, "%s", chart->nam);
#ifdef DEBUG_SEARCH
		chars_written = sprintf(buffer, "%04d %s", ++count, chart->nam);
#else
		chars_written = sprintf(buffer, "%s", chart->nam);
#endif

		if(search_options->string_in_name.search)
		{
			if(!StrStr(chart->nam, search_options->string_in_name.string))
			{
				break;	// it's not the chart we want
			}
		}

		if(search_options->object_in_sign.search)
		{
			BOOL		found = FALSE;
			int			obj;
			double		longitude;
			double		speed;

			if(search_options->object_in_sign.object < 0)
			{
				for(obj = oEar; obj <= cLastMoving; obj++)  
				{
					if (!(ignore[obj] || obj == us.objCenter))
					{
						if(!get_object_info(NULL, obj, &longitude, &speed))
						{
							continue;	// couldn't get longitude
						}

						found = search_planet_in_sign_and_speed(obj, longitude, speed, 
							search_options->object_in_sign.sign, search_options->object_in_sign.direction);

						if(found)
						{
							break;
						}
					}
				}
			}
			else
			{
				obj = search_options->object_in_sign.object;

				if(!get_object_info(NULL, obj, &longitude, &speed))
				{
					break;	// couldn't get longitude
				}

				found = search_planet_in_sign_and_speed(obj, longitude, speed, 
					search_options->object_in_sign.sign, search_options->object_in_sign.direction);
			}

			if(found)
			{
				chars_written += sprintf(buffer + chars_written, " | %.4s %s", 
					Lang(tObjShortName, obj), SzZodiac(longitude));

				if(search_options->object_in_sign.direction != SEARCH_MOVEMENT_EITHER)
				{
					chars_written += sprintf(buffer + chars_written, " %s %.4f", Lang(sSPEED_ABBREVIATION), speed);
				}
				// sprintf(buffer, "%s %d %d %d %f %f %f %f %f", chart->nam, chart->day, chart->mon, chart->yea, 
				// chart->tim, chart->dst, chart->zon, chart->lon, chart->lat);
			}
			else
			{
				break;	// object not in the sign, element or quality we want
			}
		}

		if(search_options->object_in_aspect.search)
		{
			int			asp = search_options->object_in_aspect.aspect;
			int			obj1 = search_options->object_in_aspect.object1;
			int			obj2 = search_options->object_in_aspect.object2;
			BOOL		found;
			double		orb;
			double		longitude1, longitude2;


			if(!get_object_info(NULL, obj2, &longitude2, NULL))
			{
				break;	// couldn't get longitude for second object, quit
			}


			if(obj1 < 0)
			{
				for(obj1 = oEar; obj1 <= cLastMoving; obj1++)  
				{
					if (!(ignore[obj1] || obj1 == obj2 || obj1 == us.objCenter))
					{
						if(!get_object_info(NULL, obj1, &longitude1, NULL))
						{
							continue;	// couldn't get longitude
						}

						found = search_planet_in_aspect(obj1, longitude1, obj2, longitude2, &asp, &orb);

						if(found)
						{
							break;
						}
					}
				}
			}
			else
			{
				if(!get_object_info(NULL, obj1, &longitude1, NULL))
				{
					break;	// couldn't get longitude
				}

				found = search_planet_in_aspect(obj1, longitude1, obj2, longitude2, &asp, &orb);
			}

			if(found)
			{
				double degrees;
				double minutes = fabs(modf(orb, &degrees)) * 60.0;

				chars_written += sprintf(buffer + chars_written, " | %.4s %s %.4s orb %+.0f:%02.0f'",
					Lang(tObjShortName, obj1), Lang(tAspectAbbrev, asp), Lang(tObjShortName, obj2), 
					degrees, minutes);
			}
			else
			{
				break;	// object not in the aspect we want
			}
		}

		if(search_options->object_in_house.search)
		{
			int			house = search_options->object_in_house.house;
			int			obj = search_options->object_in_house.object;
			BOOL		found = FALSE;
			double		beg, end, longitude;


			if(obj < 0)
			{
				for(obj = oEar; obj <= cLastMoving; obj++)  
				{
					if (!(ignore[obj] || obj == us.objCenter))
					{
						if(!get_object_info(NULL, 1000 + house, &beg, NULL) ||
						   !get_object_info(NULL, 1000 + mod12(house + 1), &end, NULL) ||
						   !get_object_info(NULL, obj, &longitude, NULL))
						{
							continue;	// couldn't get longitude
						}

						found = (longitude >= beg && longitude < end) || 
								(beg > end && (longitude >= beg || longitude < end));

						if(found)
						{
							break;
						}
					}
				}
			}
			else
			{

				if(!get_object_info(NULL, 1000 + house, &beg, NULL) ||
				   !get_object_info(NULL, 1000 + mod12(house + 1), &end, NULL) ||
				   !get_object_info(NULL, obj, &longitude, NULL))
				{
					break;	// couldn't get longitude
				}

				found = (longitude >= beg && longitude < end) || 
						(beg > end && (longitude >= beg || longitude < end));
			}

			if(found)
			{
				chars_written += sprintf(buffer + chars_written, " | %.4s %s %d", 
					Lang(tObjShortName, obj), Lang(sHOUSE1), house);
			}
			else
			{
				break;	// object not in the house we want
			}
		}

		if(search_options->object_in_gauquelin_sector.search)
		{
			int			sector = search_options->object_in_gauquelin_sector.sector;
			int			obj = search_options->object_in_gauquelin_sector.object;
			BOOL		found = FALSE;
			double		beg, end, longitude;
			double		jd, jde, cusps[37], ascmc[10];


			// calculate julian time
			jd = MdytszToJulian(chart->mon, chart->day, chart->yea, chart->tim, chart->dst, chart->zon) - 0.5;

			// calculate julian ephemeris time
			jde = jd + swe_deltat(jd);

			// calculate Gauquelin sectors
			if(swe_houses(jd, DegMin2DecDeg(chart->lat), -DegMin2DecDeg(chart->lon), 'G', cusps, ascmc) == ERR)
			{
				// looking at SE code, we will find an error is returned polar latitudes
				break;	// couldn't get sectors
			}
			else
			{
				// sectors are numbered clockwise
				end	= cusps[sector];
				beg = cusps[mod36(sector + 1)];
			}

			if(obj < 0)
			{
				for(obj = oEar; obj <= cLastMoving; obj++)  
				{
					if (!(ignore[obj] || obj == us.objCenter))
					{
						if(!get_object_info(NULL, obj, &longitude, NULL))
						{
							continue;	// couldn't get longitude
						}

						// note that this formula is slightly different than that for the houses,
						// the difference is in the = sign.
						found = (longitude > beg && longitude <= end) || 
								(beg > end && (longitude > beg || longitude <= end));

						if(found)
						{
							break;
						}
					}
				}
			}
			else
			{

				if(!get_object_info(NULL, obj, &longitude, NULL))
				{
					break;	// couldn't get longitude
				}

				found = (longitude > beg && longitude <= end) || 
						(beg > end && (longitude > beg || longitude <= end));
			}

			if(found)
			{
				chars_written += sprintf(buffer + chars_written, " | %.4s %s %d", 
					Lang(tObjShortName, obj),  Lang(sGAUQUELIN_SECTOR), sector);
			}
			else
			{
				break;	// object not in the sector
			}
		}

		good_chart = TRUE;
		break;	// get out of this (fake) loop
	}

	return good_chart;
}

static void find_chart_files(HWND hdlg, char *search_dir, search_options_t *search_options, 
							   char *file_extension, unsigned int file_ext_length)
{
	WIN32_FIND_DATA 	FindFileData;
	HANDLE				search_file_handle;
	char				*p;
	char				file_name[1000];
	char				search_string[1000];
	char				new_search_dir[1000];


	// warning: this is a recursive function in order to traverse a directory tree

	p = StrCpy(search_string, search_dir);
	StrCpy(p, "\\*");	// search all files and directories

	search_file_handle = FindFirstFile(search_string, &FindFileData);

	if(search_file_handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!(FindFileData.dwFileAttributes & (FILE_ATTRIBUTE_COMPRESSED | 
					FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ENCRYPTED | 
					FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_OFFLINE | 
					FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY)))
			{
				// here we have a file (not a directory) and it is not encrypted, hidden, etc

				if(!file_ext_length || (  (p = StrStr(FindFileData.cFileName, file_extension)) && 
					(strlen(p) == file_ext_length) ))
				{
					CI		*chart = ChartList(CHART_CREATE, CHART_HEAP_SEARCH, NULL);
					BOOL	good_chart = FALSE;
					char	buffer[1000];


					// we are not filtering files by extensions, or else this 
					// file has the desired extension with nothing extra

					// build file name
					p = StrCpy(file_name, search_dir);
					p = StrCpy(p, "\\");
					StrCpy(p, FindFileData.cFileName);

					if(read_file_data(file_name, chart))
					{
						good_chart = filter_chart(hdlg, chart, search_options, buffer);
					}

					if(good_chart)
					{
						int 	index;

						// Put the name and other pertinent information in the list box, so that 
						// the user can have the opportunity of selecting it.
						index = SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_ADDSTRING, 0, 
									(LPARAM)(LPCSTR) buffer);

						// save pointer to chart info in list for later
						SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_SETITEMDATA, 
							(WPARAM) index, (LPARAM) chart);	
					}
					else
					{
						// something bad with this chart, throw it away
						ChartList(CHART_DESTROY, CHART_HEAP_SEARCH, NULL);

#ifdef DEBUG_SEARCH
						{
							char buffer[200];

							sprintf(buffer, "-----> Bad chart: %s", FindFileData.cFileName);
							SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_ADDSTRING, 0, 
												(LPARAM)(LPCSTR) buffer);
						}
#endif
					}
				}

#ifdef DEBUG_SEARCH
				else
				{
					char buffer[200];

					sprintf(buffer, "-----> Bad extension: %s", FindFileData.cFileName);
					SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_ADDSTRING, 0, 
										(LPARAM)(LPCSTR) buffer);
				}
#endif
			}
			else if(search_options->search_subdirs && 
				FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				strcmp(".", FindFileData.cFileName) && 
				strcmp("..", FindFileData.cFileName))
			{
				// this is a sub-directory, prepare to search it

				// build full sub-directory string
				p = StrCpy(new_search_dir, search_dir);
				p = StrCpy(p, "\\");					// add directory separator
				StrCpy(p, FindFileData.cFileName);		// add directory name

				find_chart_files(hdlg, new_search_dir, search_options, 
					file_extension, file_ext_length);	// recurse
			}
#ifdef DEBUG_SEARCH
			else
			{
				char buffer[200];

				sprintf(buffer, "-----> Bad file type: %s", FindFileData.cFileName);
				SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_ADDSTRING, 0, 
									(LPARAM)(LPCSTR) buffer);
			}
#endif

		} while (FindNextFile(search_file_handle, &FindFileData));

		FindClose(search_file_handle);
	}
}

static void set_initial_selection_planet(HWND hdlg, WORD wid, int object)
{
	int 	index;


	if(object < 0)
	{
		index = SendDlgItemMessage(hdlg, wid, CB_FINDSTRINGEXACT, (WPARAM) 0, 
			(LPARAM) (LPCSTR) Lang(sANY_OBJECT)); 
	}
	else
	{
		index = SendDlgItemMessage(hdlg, wid, CB_FINDSTRINGEXACT, (WPARAM) 0, 
			(LPARAM) (LPCSTR) Lang(tObjName, object)); 
	}

	if(index != CB_ERR)
	{
		// we need to check for error in case the user changed central planet or deselected object
		// select last saved object
		SendDlgItemMessage(hdlg, wid, CB_SETCURSEL, (WPARAM) index, 0); 
	}
	else
	{
		// select first object in list (Sun or Earth)
		SendDlgItemMessage(hdlg, wid, CB_SETCURSEL, (WPARAM) 0, 0); 
	}
}

static void set_object_list(HWND hdlg, int object)
{
	if (!ignore[object])
	{
		SetCombo(dcPlanetSelection, Lang(tObjName, object));
		SetCombo(dcPlanetSelection2, Lang(tObjName, object));
		SetCombo(dcPlanetSelection3, Lang(tObjName, object));
		SetCombo(dcPlanetSelection4, Lang(tObjName, object));
		SetCombo(dcPlanetSelection5, Lang(tObjName, object));
	}
}

#if defined(LANGUAGE_FRENCH)
#define INSIDE "en "
#else
#define INSIDE ""
#endif

// create list of names of signs, elements, and qualities
static void create_signs_list(char signs[21][20])
{
	int i;


	// 1-12 are signs, 13-16 are elements, 17-19 are qualities, 20 means any sign

	// create list of signs
	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		strcpy(&signs[i][0], INSIDE);
		strcat(&signs[i][0], Lang(tSignName, i));
	}

	// add elements to list of signs
	for (i = 0; i < 4; i++)
	{
		strcpy(&signs[13 + i][0], INSIDE);
		strcat(&signs[13 + i][0], Lang(tElement, i));
	}
	
	// add qualities to list of signs
	for (i = 0; i < 3; i++)
	{
		strcpy(&signs[17 + i][0], INSIDE);
		strcat(&signs[17 + i][0], Lang(tMode, i));
	}

	// add any sign
	strcpy(&signs[20][0], Lang(sANY_SIGN));
}

/*
******************************************************************************
** Windows Dialogs.
******************************************************************************
*/
 

int CALLBACK DlgSearchCharts(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static search_options_t search_options = 
		{FALSE, 
		{FALSE, ""}, 
		{FALSE, oSun, sAri, SEARCH_MOVEMENT_EITHER}, 
		{FALSE, oMoo, aCon, oVen}, 
		{FALSE, oMoo, 1, (house_system_t)-1},
		{FALSE, oMoo, 1}};
	WORD					wid = LOWORD(wParam);
	static char				search_dir[300] = {0};
	static char				file_extension[20] = {1};	// 1 means first time this is called
	char					buffer[200];
	char					signs[21][20];
	unsigned int			file_ext_length;
	int 					index;
	CI						*chart; 
	int 					i, asp;
	WORD					message_type = HIWORD(wParam);
	HCURSOR 				hcursor;


	switch (message)
	{
	case WM_INITDIALOG:

		SendDlgItemMessage(hdlg, pctSearchCharts, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
			(LPARAM)LoadBitmap(wi.hinst, MAKEINTRESOURCE(IDB_BITMAP1)));

		// alow horizontal scrolling
		SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_SETHORIZONTALEXTENT, 
			(WPARAM) 1200, 0);

		if(search_dir[0])
		{
			SetEdit(dSearchDirectory, search_dir);
		}
		else
		{
			// initialize with charts directory the first time around
			SetEdit(dSearchDirectory, dirs.charts.dir);
		}

		if(file_extension[0] > 1) // not first time and not empty string
		{
			SetEdit(deSearchFileExtension, file_extension);
		}
		else if(file_extension[0] == 1) //  first time around
		{
			// Set with default extension for chart files. Note that if the user had
			// previously set no extension we will set to the default, but we won't
			// bother about that.
			SetEdit(deSearchFileExtension, ".dat"); 
		}
		// else leave extension field empty

		if(us.objCenter > oSun) 
		{
			search_options.object_in_sign.search = FALSE;
			search_options.object_in_aspect.search = FALSE;

			EnableWindow(GetDlgItem(hdlg, dx03), FALSE);
			EnableWindow(GetDlgItem(hdlg, dx04), FALSE);
			EnableWindow(GetDlgItem(hdlg, dr01), FALSE);
			EnableWindow(GetDlgItem(hdlg, dr02), FALSE);
			EnableWindow(GetDlgItem(hdlg, dr03), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcPlanetSelection), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcPlanetSelection2), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcPlanetSelection3), FALSE);
			EnableWindow(GetDlgItem(hdlg, IDC_STATIC2), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcSignSelection), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcAspectSelection), FALSE);
		}

		if(us.objCenter != oEar)
		{
			search_options.object_in_house.search = FALSE;

			EnableWindow(GetDlgItem(hdlg, dx05), FALSE);
			EnableWindow(GetDlgItem(hdlg, dx06), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcPlanetSelection4), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcPlanetSelection5), FALSE);
			EnableWindow(GetDlgItem(hdlg, IDC_STATIC1), FALSE);
			EnableWindow(GetDlgItem(hdlg, IDC_STATIC3), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcHouseSelection), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcHouseSystemSelection), FALSE);
			EnableWindow(GetDlgItem(hdlg, dcGauquelinSector), FALSE);
		}

		SetEdit(dName, search_options.string_in_name.string);
		SetCheck(dx01, search_options.search_subdirs);	
		SetCheck(dx02, search_options.string_in_name.search);	
		SetCheck(dx03, search_options.object_in_sign.search);	
		SetCheck(dx04, search_options.object_in_aspect.search); 
		SetCheck(dx05, search_options.object_in_house.search); 
		SetCheck(dx06, search_options.object_in_gauquelin_sector.search); 

		if(us.objCenter == oSun)
		{
			set_object_list(hdlg, oEar);
		}
		else
		{
			set_object_list(hdlg, oSun);
		}

		// create list of planets
		for (i = oMoo; i <= cLastPlanet; i++)
		{
			set_object_list(hdlg, i);
		}

		// now some objects
		if(us.objCenter == oEar)
		{
			set_object_list(hdlg, oNoNode);
			set_object_list(hdlg, oLil);
			set_object_list(hdlg, oVtx);
			set_object_list(hdlg, oEP);
			set_object_list(hdlg, oAsc);
			set_object_list(hdlg, oMC);
		}

		// now uranians
		for (i = uranLo; i <= uranHi; i++)
		{
			set_object_list(hdlg, i);
		}

		SetCombo(dcPlanetSelection, Lang(sANY_OBJECT));
		SetCombo(dcPlanetSelection2, Lang(sANY_OBJECT));
		SetCombo(dcPlanetSelection4, Lang(sANY_OBJECT));
		SetCombo(dcPlanetSelection5, Lang(sANY_OBJECT));

		set_initial_selection_planet(hdlg, dcPlanetSelection, 
				search_options.object_in_sign.object);
		set_initial_selection_planet(hdlg, dcPlanetSelection2, 
				search_options.object_in_aspect.object1);
		set_initial_selection_planet(hdlg, dcPlanetSelection3, 
				search_options.object_in_aspect.object2);
		set_initial_selection_planet(hdlg, dcPlanetSelection4, 
				search_options.object_in_house.object);
		set_initial_selection_planet(hdlg, dcPlanetSelection5, 
				search_options.object_in_gauquelin_sector.object);

		// create list of signs
		create_signs_list(signs);
		for (i = 1; i <= 20; i++)
		{
			SetCombo(dcSignSelection, signs[i]);
		}

		//	select last sign (or element or quality), or Aries the first time around
		SendDlgItemMessage(hdlg, dcSignSelection, CB_SETCURSEL, 
			(WPARAM) (search_options.object_in_sign.sign - 1), 0); 

		SetRadio(dr01 + search_options.object_in_sign.direction, dr01, dr03);

		// create list of aspects
		for (asp = 1; asp <= cAspect; asp++)
		{
			if (!ignoreA[asp])
			{
				SetCombo(dcAspectSelection, Lang(tAspectName, asp));
			}
		}
		SetCombo(dcAspectSelection, Lang(sANY_ASPECT));

		//	select last aspect, or conjunction, the first time around
		asp = search_options.object_in_aspect.aspect;
		if(asp > 0)
		{
			index = SendDlgItemMessage(hdlg, dcAspectSelection, CB_FINDSTRINGEXACT, 
				(WPARAM) 0, (LPARAM) (LPCSTR) Lang(tAspectName, asp)); 
		}
		else
		{
			index = SendDlgItemMessage(hdlg, dcAspectSelection, CB_FINDSTRINGEXACT, 
				(WPARAM) 0, (LPARAM) (LPCSTR) Lang(sANY_ASPECT)); 
		}

		// we need to check for error in case the user deselected an aspect
		if(index == CB_ERR)
		{
			// select conjunction if error
			SendDlgItemMessage(hdlg, dcAspectSelection, CB_SETCURSEL, (WPARAM) 0, 0); 
		}
		else
		{
			//	select last aspect, or conjunction, the first time around
			SendDlgItemMessage(hdlg, dcAspectSelection, CB_SETCURSEL, (WPARAM) index, 0); 
		}

		// create list of house numbers
		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
		{
			char buffer[10];
			sprintf(buffer, "%d", i);
			SetCombo(dcHouseSelection, buffer);
		}

		//	select last house number, or first house initially
		SendDlgItemMessage(hdlg, dcHouseSelection, CB_SETCURSEL, 
			(WPARAM) search_options.object_in_house.house - 1, 0); 

		// create list of house systems
		for (i = 0; i < NUMBER_OF_HOUSE_SYSTEMS; i++)
		{
			// add to list only houses supported by Swiss Ephemeris
			if(AstrologHouse2SeHouse(i) != -1)
			{
				SetCombo(dcHouseSystemSelection, Lang(tHouseSystem, i));
			}
		}

		if(search_options.object_in_house.house_system == -1)
		{
			// the first time around, we initialize the house system for search as the
			// house system for other calculations, if supported by S. Eph.

			index = SendDlgItemMessage(hdlg, dcHouseSystemSelection, CB_FINDSTRINGEXACT, 
					(WPARAM) 0, (LPARAM) (LPCSTR) Lang(tHouseSystem, us.nHouseSystem)); 
		}
		else
		{
			index = SendDlgItemMessage(hdlg, dcHouseSystemSelection, CB_FINDSTRINGEXACT, 
					(WPARAM) 0, (LPARAM) (LPCSTR) Lang(tHouseSystem, search_options.object_in_house.house_system)); 
		}

		// we need to check for error in case the user was using an house system not supported by SE
		if(index == CB_ERR)
		{
			// select Placidus if error
			SendDlgItemMessage(hdlg, dcHouseSystemSelection, CB_SETCURSEL, (WPARAM) 0, 0); 
		}
		else
		{
			//	select last house system
			SendDlgItemMessage(hdlg, dcHouseSystemSelection, CB_SETCURSEL, (WPARAM) index, 0); 
		}
		
		// create list of Gauquelin sectors
		for (i = 1; i <= cSector; i++)
		{
			char buffer[10];
			sprintf(buffer, "%d", i);
			SetCombo(dcGauquelinSector, buffer);
		}

		//	select last sector number, or first sector initially
		SendDlgItemMessage(hdlg, dcGauquelinSector, CB_SETCURSEL, 
			(WPARAM) search_options.object_in_gauquelin_sector.sector - 1, 0); 

		return TRUE;

	case WM_COMMAND:

		if (wid == bBrowseDirectory1)
		{
			if(ChooseDirectory(wi.hwnd, Lang(sCHOOSE_DIRECTORY), FALSE, search_dir))
			{
				SetEdit(dSearchDirectory, search_dir);
			}
		}
		
		if (wid == dbSearchCharts)
		{
			GetEdit(dSearchDirectory, search_dir);
			GetEdit(deSearchFileExtension, file_extension);
			GetEdit(dName, search_options.string_in_name.string);
			GetDlgItemText(hdlg, dName, search_options.string_in_name.string, 
				sizeof(search_options.string_in_name.string));
			file_ext_length = strlen(file_extension);
			search_options.search_subdirs = GetCheck(dx01);
			search_options.string_in_name.search = GetCheck(dx02);
			search_options.object_in_sign.search = GetCheck(dx03);
			search_options.object_in_aspect.search = GetCheck(dx04);
			search_options.object_in_house.search = GetCheck(dx05);
			search_options.object_in_gauquelin_sector.search = GetCheck(dx06);

			if(search_options.object_in_sign.search)
			{
				GetEdit(dcPlanetSelection, buffer);
				for (i = oEar; i <= uranHi; i++)
				{
					if(fSstringsMatch(buffer, Lang(tObjName, i), 0))
					{
						search_options.object_in_sign.object = (objects_t)i;
						break;
					}
				}

				if(i > uranHi)
				{
					if(fSstringsMatch(buffer, Lang(sANY_OBJECT), 0))
					{
						search_options.object_in_sign.object = (objects_t)-1;
					}
				}

				GetEdit(dcSignSelection, buffer);

				// create list of signs
				create_signs_list(signs);

				// search for a sign match
				for (i = 1; i <= 20; i++)
				{
					if(fSstringsMatch(buffer, signs[i], 0))
					{
						search_options.object_in_sign.sign = i;
						break;
					}
				}

				search_options.object_in_sign.direction = 
					GetCheck(dr01) ? SEARCH_MOVEMENT_DIRECT : 
					(GetCheck(dr02) ? SEARCH_MOVEMENT_RETROGRADE : SEARCH_MOVEMENT_EITHER);
			}

			if(search_options.object_in_aspect.search)
			{
				GetEdit(dcPlanetSelection2, buffer);
				for (i = oEar; i <= uranHi; i++)
				{
					if(fSstringsMatch(buffer, Lang(tObjName, i), 0))
					{
						search_options.object_in_aspect.object1 = (objects_t)i;
						break;
					}
				}

				if(i > uranHi)
				{
					if(fSstringsMatch(buffer, Lang(sANY_OBJECT), 0))
					{
						search_options.object_in_aspect.object1 = (objects_t)-1;
					}
				}

				GetEdit(dcPlanetSelection3, buffer);
				for (i = oEar; i <= uranHi; i++)
				{
					if(fSstringsMatch(buffer, Lang(tObjName, i), 0))
					{
						search_options.object_in_aspect.object2 = (objects_t)i;
						break;
					}
				}

				GetEdit(dcAspectSelection, buffer);
				for (i = 1; i <= cAspect; i++)
				{
					if(fSstringsMatch(buffer, Lang(tAspectName, i), 0))
					{
						search_options.object_in_aspect.aspect = (aspects_t)i;
						break;
					}
				}

				if(i > cAspect)
				{
					// this test is redundant, but we leave it here for future expansion
					if(fSstringsMatch(buffer, Lang(sANY_ASPECT), 0))
					{
						search_options.object_in_aspect.aspect = (aspects_t)0;
					}
				}
			}

			if(search_options.object_in_house.search)
			{
				int house;

				GetEdit(dcPlanetSelection4, buffer);
				for (i = oEar; i <= uranHi; i++)
				{
					if(fSstringsMatch(buffer, Lang(tObjName, i), 0))
					{
						search_options.object_in_house.object =(objects_t) i;
						break;
					}
				}

				if(i > uranHi)
				{
					if(fSstringsMatch(buffer, Lang(sANY_OBJECT), 0))
					{
						search_options.object_in_house.object = (objects_t)-1;
					}
				}

				GetEdit(dcHouseSelection, buffer);

				house = atoi(buffer);
				if(house < 1 || house > 12)
				{
					PrintWarning("Invalid house number %d, using previous house %d", 
						house, search_options.object_in_house.house);
				}
				else
				{
					search_options.object_in_house.house = house;
				}

				GetEdit(dcHouseSystemSelection, buffer);
				for (i = 0; i < NUMBER_OF_HOUSE_SYSTEMS; i++)
				{
					if(fSstringsMatch(buffer, Lang(tHouseSystem, i), 0))
					{
						search_options.object_in_house.house_system = (house_system_t)i;
						break;
					}
				}
			}

			if(search_options.object_in_gauquelin_sector.search)
			{
				int sector;

				GetEdit(dcPlanetSelection5, buffer);
				for (i = oEar; i <= uranHi; i++)
				{
					if(fSstringsMatch(buffer, Lang(tObjName, i), 0))
					{
						search_options.object_in_gauquelin_sector.object = (objects_t)i;
						break;
					}
				}

				if(i > uranHi)
				{
					if(fSstringsMatch(buffer, Lang(sANY_OBJECT), 0))
					{
						search_options.object_in_gauquelin_sector.object = (objects_t)-1;
					}
				}

				GetEdit(dcGauquelinSector, buffer);

				sector = atoi(buffer);
				if(sector < 1 || sector > 36)
				{
					PrintWarning("Invalid sector number %d, using previous sector %d", 
						sector, search_options.object_in_gauquelin_sector.sector);
				}
				else
				{
					search_options.object_in_gauquelin_sector.sector = sector;
				}
			}

			// erase previous list, if any
			SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_RESETCONTENT, 0, 0);

			// show hourglass cursor
			hcursor = GetCursor();
			SetCursor(LoadCursor(NULL, IDC_WAIT)); 

			// don't redraw list box while we create the list
			SendDlgItemMessage(hdlg, dSearchChartsListBox, WM_SETREDRAW, FALSE, 0); 

			// free all charts from a previous search
			while(ChartList(CHART_DESTROY, CHART_HEAP_SEARCH, NULL)); 
			while(ChartList(CHART_DESTROY, CHART_HEAP_SORTED, NULL)); 

			// pre-allocate memory, 1000 items of 100 octets each, for efficiency
			SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_INITSTORAGE, 
				(WPARAM) 1000, (LPARAM) (DWORD) 100);

			find_chart_files(hdlg, search_dir, &search_options, file_extension, file_ext_length);

			// select first chart in list
			SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_SETCURSEL, (WPARAM) 0, 0); 

			// list finished, redraw box now
			SendDlgItemMessage(hdlg, dSearchChartsListBox, WM_SETREDRAW, TRUE, 0); 

			// restore cursor
			SetCursor(hcursor);
		}

		// show the selected chart, if user clicked OK or double-clicked name
		if (wid == IDOK || wid == dSearchChartsListBox && message_type == LBN_DBLCLK)
		{
			index = SendDlgItemMessage(hdlg, dSearchChartsListBox, LB_GETCURSEL, 0, 0);
			if(index != LB_ERR)
			{
				int count =  SendDlgItemMessage(hdlg, dSearchChartsListBox, 
						LB_GETCOUNT, (WPARAM) 0, 0);;

				chart = (CI *) SendDlgItemMessage(hdlg, dSearchChartsListBox, 
					LB_GETITEMDATA, (WPARAM) index, 0);

				ciMain = *chart;
				ciCore = *chart;
				wi.fCast = TRUE;

				// now we create another heap with charts as sorted in the list box
				for (i = 0; i < count; i++)
				{
					CI		*new_chart = ChartList(CHART_CREATE, CHART_HEAP_SORTED, NULL);

					chart = (CI *) SendDlgItemMessage(hdlg, dSearchChartsListBox, 
						LB_GETITEMDATA, (WPARAM) i, 0);

					if(i == index)
					{
						LastChartShown = new_chart;
					}

					*new_chart = *chart;
				}
			}
		}

		if (wid == IDOK || wid == IDCANCEL || 
			wid == dSearchChartsListBox && message_type == LBN_DBLCLK)
		{
			while(ChartList(CHART_DESTROY, CHART_HEAP_SEARCH, NULL));  // free all charts

			EndDialog(hdlg, TRUE);
			return TRUE;
		}
		break;
	}

	return FALSE;
}


/* search_charts.c */
