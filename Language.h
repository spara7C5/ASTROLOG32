#pragma once
#ifndef LanguageH
#define LanguageH
#include <windows.h>
#include "General/CharStrings.h"

enum langErrors {
	leOk, leVersion, leNotFound, leCustom
};

class Language {
public:
	int LoadLanguage(HMODULE resInst);
	int LoadLanguage(char* resFile, char* iD, int ver, int oldestVer);

	// Load strings from the resource with IDs firstId to lastId
	int LoadStrings(unsigned int firstId, unsigned int lastId);
	int LoadTables(unsigned int firstId, unsigned int lastId);

	bool SetTableItem(unsigned int tableId, unsigned int index, char* value);
	bool SetString(unsigned int stringId, char* value);

	char* operator () (int stringId);
	char* operator () (int tableId, int index);

	Language();
	~Language();

public:
	CharStrings strings;
	CharTables tables;
	HMODULE resource;
	// Difference between resource ID and actual possition in the list
	int stringsOffset;
	int tablesOffset;

protected:
	static const int RC_STRING_BUFF = 4096;

	};

extern Language Lang;

#endif