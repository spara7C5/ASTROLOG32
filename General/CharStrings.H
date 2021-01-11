//---------------------------------------------------------------------------
#ifndef CharStringsH
#define CharStringsH

#include <windows.h>
#include <vector>
using namespace std;

#define CHECK_LIST_BOUNDS(A) if(A >= size()) {MessageBox(0,"Fatal error: List bound error in " __FILE__ , 0,0);__debugbreak();return NULL;};


//---------------------------------------------------------------------------

class CharStrings : public vector<char*> {
public:
  bool Add(char* str, int count = -1);
  bool Set(unsigned int index, char* str);
  bool ParseAdd(char* str, char separator = '|');
  char* GetItem(unsigned int index);
  CharStrings();
  CharStrings(const CharStrings& chs);
  ~CharStrings();
};

//---------------------------------------------------------------------------

class CharTables : public vector<CharStrings*> {
public:
  bool Add(CharStrings* chS);
  bool Set(unsigned int index, CharStrings* chS);
  bool ParseAdd(char* str, char separator = '|');
  bool AddString(unsigned int table, char* str);
  char* operator () (int table, int index);

  CharStrings* GetItem(unsigned int index);
  ~CharTables();
} ;

//---------------------------------------------------------------------------
#endif
