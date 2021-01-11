//---------------------------------------------------------------------------
#define _CRT_SECURE_NO_DEPRECATE

#include "CharStrings.h"

// Adds a copy of string <str>
bool CharStrings::Add(char* str, int count) {
  if(!str) return false;
  int len = (count == -1) ? strlen(str) : count;
  char* t = new char[len + 1];
  if(!t) return false;
  memcpy(t, str, len);
  t[len] = 0;
  push_back(t);
  return true;
}

// Sets string at position <index> to a copy of <str>. Previous string is deleted
bool CharStrings::Set(unsigned int index, char* str) {

  CHECK_LIST_BOUNDS(index);
  if(!at(index) || !str) return false;
  char* t = new char[strlen(str) + 1];
  if(!t) return false;
  strcpy(t, str);
  delete at(index);
  at(index) = t;
  return true;
}

// Adds strings from <str> separated by <separator>. '|' is default separator
bool CharStrings::ParseAdd(char* str, char separator) {
  if(!str) return false;

  char* pStart =str;
  char* pEnd;
  while(pStart) {
    pEnd = strchr(pStart, separator);
    if(pEnd) {
      char* t = new char [pEnd - pStart + 1];
      if(!t) return false;
      strncpy(t, pStart, pEnd - pStart);
      t[pEnd - pStart] = 0;
      if(!Add(t)) {
        delete t;
        return false;
      }
      delete t;
      pStart = pEnd + 1;
    }
    else {
      if(!Add(pStart)) return false;
      pStart = NULL;
    }
  }
  return true;

}

char* CharStrings::GetItem(unsigned int index) {
  CHECK_LIST_BOUNDS(index);
  return at(index);
};

// Constructor
CharStrings::CharStrings() {};

//Copy Constructor
CharStrings::CharStrings(const CharStrings& chs) {
  for(unsigned int i = 0; i < chs.size(); i++) {
    Add(chs[i]);
  }
};

// Destructor deallocates all strings
CharStrings::~CharStrings() {
  for(unsigned int i = 0; i < size(); i++) {
    if(at(i))
      delete at(i);
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


bool CharTables::Add(CharStrings* chS) {
 CharStrings* strings = new CharStrings(*chS);
 if(!strings) return false;
 push_back(strings);
 return true;
}

bool CharTables::Set(unsigned int index, CharStrings* chS) {
  CharStrings* strings = new CharStrings(*chS);
  if(!strings) return false;
  delete at(index);
  at(index) = strings;
  return true;
}


bool CharTables::ParseAdd(char* str, char separator) {
  CharStrings* strings = new CharStrings();
  if(!strings->ParseAdd(str, separator)) {
    delete strings;
    return false;
  }
  push_back(strings);
  return true;
}

bool CharTables::AddString(unsigned int table, char* str) {
	return GetItem(table)->Add(str);
}

CharStrings* CharTables::GetItem(unsigned int index) {
  CHECK_LIST_BOUNDS(index);
  return at(index);
};

char* CharTables::operator () (int table, int index) {
	return GetItem(table)->GetItem(index);
}

CharTables::~CharTables() {
  for(unsigned int i = 0; i < size(); i++) {
    if(at(i))
      delete at(i);
  }

};