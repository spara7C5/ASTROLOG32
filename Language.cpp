
//-------------------------------------------------------------------------------
// Version                When      Who                  What 
//     1.0                24.1.2006 Tomas
//

#include "Language.h"




Language::Language()  : stringsOffset(0)
{
	resource = NULL;
	stringsOffset  = tablesOffset = 0;
}

Language::~Language() {
	if(resource) FreeLibrary(resource);
}



int Language::LoadLanguage(HMODULE resInst) {
	resource = resInst;
	return leOk;
};

int Language::LoadLanguage(char* resFile, char* iD, int ver, int oldestVer){
	resource = LoadLibrary(resFile);
	
	if(!resource) {
		LPVOID myMessage;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR)&myMessage, 0, NULL);
		LocalFree(myMessage);

		return leNotFound;
	}
	char* buff = new char[RC_STRING_BUFF];

	// now check the language pack ID and compatibility
	try {
		int res  = LoadString(resource, 0, buff, RC_STRING_BUFF);
		if(!res) throw leVersion;
		if(strlen(buff) != 10) throw leVersion;
		if(strncmp(buff, iD, 8)) throw leVersion;

		int resVer = buff[8];
		int resOldestVer = buff[9];
		// is the resource too old?
		if(oldestVer > resVer) throw leVersion;
		// is the application too old?
		if(resOldestVer > ver) throw leVersion;
	}
	catch(langErrors e) {
		delete []buff;
		return e;
	}
	delete []buff;
	return leOk;
};

char* Language::operator () (int stringId) {
	return strings.GetItem(stringId - stringsOffset);
}

char* Language::operator () (int tableId, int index) {
	return tables.GetItem(tableId - tablesOffset)->GetItem(index);
}

// Load strings from the resource with IDs firstId to lastId
int Language::LoadStrings(unsigned int firstId, unsigned int lastId) {
	char* buff = new char[RC_STRING_BUFF];
	try {
		for(unsigned int i = firstId; i <= lastId; i++) {
			int read = LoadString(resource, i, buff, RC_STRING_BUFF);
			if(buff[read - 1] == 001) buff[read - 1] = 0;  // hack for terminating 0
			if(!read) throw (langErrors)i;
			if(!strings.Add(buff, read)) throw (langErrors)i;
		}
		throw leOk;
	}
	catch(langErrors e) {
		delete [] buff;
		return e;
	}
}
// Load strings from the resource with IDs firstId to lastId

int Language::LoadTables(unsigned int firstId, unsigned int lastId) {
	char* buff = new char[RC_STRING_BUFF];

	try {
		for(unsigned int i = firstId; i <= lastId; i++) {
			if(!LoadString(resource, i, buff, RC_STRING_BUFF)) throw (langErrors)i;
			if(!tables.ParseAdd(buff)) throw (langErrors)i;
		}
		throw leOk;
	}
	catch(langErrors e) {
		delete [] buff;
		return e;
	}
}

	
bool Language::SetTableItem(unsigned int tableId, unsigned int index, char* value) {
	tables.GetItem(tableId - tablesOffset)->Set(index, value);
	return true;

}
bool Language::SetString(unsigned int stringId, char* value) {
	strings.Set(stringId- stringsOffset, value);
	return true;
}

