/*  Note: You are free to use whatever license you want.
    Eventually you will be able to edit it within Glade. */

/*  irenaeus
 *  Copyright (C) 2000 Richard Holcombe
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef IRENAEUS_H
#define IRENAEUS_H


#include <string>
#include <swmgr.h>
#include <swdisp.h>
#include <versekey.h>
#include <menu.h>
#include <stream.h>

#define NUMVWIN 10
#define MENUROWS 9

//object types
#define BIBLICALTEXT 0
#define SEARCHRESULT 1

struct moduledef
{
  string custom; // unused now, but available field for others to use
  string name;
  string description;
  string type;
};

typedef list<moduledef> moduledeflist;
typedef list<string> stringlist;

struct windat
{
  WINDOW *textWindow;
  string modname;
  string keyname;
  stringlist *_searchresult;
  int top_x;
  int top_y;
  int porty;
  int portx;
  int basex;
  int basey;
  int objecttype;
};


class ioMgr
{
 private:
  int vnum;
  int inputtype;
  int outputtype;
  windat wd[NUMVWIN];
  int vistab[NUMVWIN][NUMVWIN];
  MENU *menuwin;
  int outlevel;
  int hascolor;

 public:
  ioMgr(int t);
  int getnextchar(string inputstring,unsigned int pos);
  string querystr(string str1,string inputstring,unsigned int pos);
  int queryint(string str1,string inputstring,unsigned int pos);
  void displaymessage(string mess);
  void printwindow(string text,int winnum);
  void panner(int num,int c);
  void panner(int c);
  void drawscreen();
  void modchanged();
  string drawmenu(moduledeflist *_moduleList);
  void Display(SWModule &imodule,VerseKey *endvkey);
  void Display(SWModule &imodule);
  void updateDisplay(SWModule &imodule);
  void displayverse(string Text);
  void displaysearch(string Text);
  VerseKey gettopsearch();
  void clearsearch();
  void switchvirwin(int wn);
  void popwin(string msg);
  int getvnum();
  void setvnum(int vn);
  void setobjecttype(int mnum,int objtype);
  void setinputtype(int inptyp);
  int getinputtype();
  void changewindowtop(int winnum,int value);
  void changewindowbottom(int winnum,int value);
  void changewindowleft(int winnum,int value);
  void changewindowright(int winnum,int value);

};


class irenaeusMgr : public SWMgr
{
protected:
  //SWFilter *gbftohtml;
  //     SWFilter *rwptohtml;
        //SWFilter *plaintohtml;
  //     SWFilter *thmltohtml;
  //      virtual void AddRenderFilters(SWModule *module, ConfigEntMap &section);

public:
        irenaeusMgr();
        virtual ~irenaeusMgr();
};


class MainWindow : public ioMgr {
	int mrows;
	int mcols;
	int c;
	int runmode;
	irenaeusMgr *mainMgr;
	SWModule *curMod;
	VerseKey verkey[NUMVWIN];
	string  modulename[NUMVWIN];
	int searchType;
	int searchParams;
	moduledeflist *_moduleList;
	int linktable[NUMVWIN][NUMVWIN];
public:
	MainWindow(int rmode);
	~MainWindow();
	void initSWORD();
	void lookupTextChanged(int vnum,string text);
	void lookupTextChanged(string text);
	void searchButtonClicked(int vnum,string response);
	void searchButtonClicked(string response);
	void viewModActivate(int vnum,const char *modName);
	void viewModActivate(const char *modName);
	void navigateButtonClicked(int direction);
	void resultListSelectionChanged(char *clist, int row, int column);
	moduledeflist *getModuleList();
	string listmodules(string mtype);
	string getcurverse();
	int linkscr(int l1,int l2,int val);
	int getlink(int l1,int l2);
	void clearlink(int l1);
	void switchvirmod(int mn);
	void AddRenderFilters(SWModule *module, ConfigEntMap &section);
	void PersonalCommentAdd(const string &modName, const string &startVerse, const string &stopVerse, const string &comment);
	void PersonalCommentRemove(const string &modName, const string &startVerse, const string &stopVerse);
	void setsearchType(int st);
	void setsearchParams(int sp);
	int getsearchType();
	int getsearchParams();
};

#define BLANK		' '	

#define GRIDSIZE	3
#define MENU_Y	8
#define MENU_X	8



#define BIBLICAL_TEXTS 1
#define COMMENTARIES 2
#define LEXICON_DICT 3

#define PRINT_HELP     '?'
#define LOOKUP_STRING  'l'
#define SEARCH_STRING  's'
#define LOOKUP_SEARCHR 'L'
#define WRITE_PC       'W'
#define SET_MODULE     'M'
#define LIST_MODULES   'R'
#define PARAM_CHANGE   'D'
#define PREVIOUS_BOOK  'p'
#define NEXT_BOOK      'n'
#define PREVIOUS_CHAP  '-'
#define NEXT_CHAP      '+'
#define PREVIOUS_VERSE '_'
#define NEXT_VERSE     '='
#define SEARSCROLL_UP  'P'
#define SEARSCROLL_DN  'N'
#define MENU_DISPLAY   'm'
#define READ_FILE      'r'

// The way I have implemented things, any use of strings on the command
// line needs to be inside of [ ... ]



#endif
