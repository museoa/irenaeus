/****************************************************************************
 * Copyright (c) 1998,1999 Free Software Foundation, Inc.                   *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/
/****************************************************************************

NAME

irenaeus

SYNOPSIS
   ncurses front end to sword

AUTHOR
   Author: Richard A Holcombe <raholcom@eos.ncsu.edu> 2000

***************************************************************************/
#include <curses.h>
#include "irenaeus.h"
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <assert.h>
#include <signal.h>
#include <panel.h>
#include <menu.h>
#include <getopt.h>
#include <fstream>


#ifdef NCURSES_MOUSE_VERSION
static const char *mouse_decode(MEVENT const *ep)
{
	static char buf[80];

	(void) sprintf(buf, "id %2d  at (%2d, %2d, %2d) state %4lx = {",
		       ep->id, ep->x, ep->y, ep->z, ep->bstate);


	if (buf[strlen(buf)-1] == ' ')
		buf[strlen(buf)-2] = '\0';
	(void) strcat(buf, "}");
	return(buf);
}
#endif /* NCURSES_MOUSE_VERSION */


#define FRAME struct frame
FRAME
{
	FRAME		*next, *last;
	bool		do_scroll;
	bool		do_keypad;
	WINDOW		*wind;
};


string MainWindow::listmodules(string mtype)
{
    moduledeflist::iterator it;
    string namelst;
    string compstr;

    compstr=mtype;
    if (mtype=="1") compstr="Biblical Texts";
    else if (mtype=="2") compstr="Commentaries";
    else if (mtype=="3") compstr="Lexicons / Dictionaries";

for (it=_moduleList->begin(); it!=_moduleList->end(); it++)
        {
	  if (it->type==compstr)
	    namelst += it->name+" ";
                //text = md->name + " (" + md->description + ")";
	}
 cout<<namelst; //for testing only
 return namelst;
}

static void ShellOut(bool message)
{
        if (message)
                addstr("Shelling out...");
        def_prog_mode();
        endwin();
        system("sh");
        if (message)
                addstr("returned from shellout.\n");
        refresh();
}



string editfile(string fname)
{
  string retstr="";
  string callstr;
  char buffer[80];
  retstr="Initial value of string";
  //def_prog_mode();
  //endwin();
  callstr="emacs "+fname;
  system(callstr.c_str()); //call editor with specified file
  // now read in file, and return it
  ifstream in1(fname.c_str());
  while (!in1.eof())
    {
       in1.getline(buffer,80);
       retstr+=buffer;
    };
  //  in1>> retstr;
  return(retstr);
}

void usage()
{
  cout<<"Usage: irenaeus \n or: irenaeus -f\n or: irenaeus -c input string ";
}


/* Global variables */
int runmode=0;


/*
void printhelp()
{
      WINDOW *win;       
      win=newwin(20,70,0,0);
      wclear(win);
      attron(A_REVERSE);
      mvwprintw(win,2,2,"This is the help screen"); 
      mvwprintw(win,3,2,"Not very helpful yet"); 
      mvwprintw(win,4,2,"Press a key to continue"); 
      attroff(A_REVERSE);
      echo();
      wrefresh(win);
      string s;
      //getnextchar(s,1);
};
*/

void parsepc(MainWindow *mw, string str)
{

}


/*+-------------------------------------------------------------------------
	main(argc,argv)
--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  
  int  c=0,my_e_param = 1,inputlen;
  unsigned int pos=0;
  string inputstring,historystring,swstr,commstr; 
    string response;

  for (int i=1;i<argc;i++)
   { 
     swstr=argv[i];
     if (swstr[0]=='-')
       switch (swstr[1])
	 {
	 case 'f': // input from standard input
	   runmode=1;
	   break;
	   
	 case 'c': // only use cmd line strings
	   runmode=2;
	   inputstring.erase();
	   i++;
	   for (;i<argc;i++) 
	     {
	       inputstring+=argv[i];
	       inputstring+=" ";
	     };
	   break;
	      
	 default:
	   usage ();
	   break;
	 } 
   }; 
  inputlen=inputstring.size();

  MainWindow irenaeus(runmode);
  MainWindow *mw=&irenaeus;
  // mw->inmgr.inputmgr(runmode);
      
    // tell it we're going to play with soft keys 
    slk_init(my_e_param);
    // we must initialize the curses data structure only once 

    //  if (runmode==0) {mw->drawscreen();   mw->lookupTextChanged("Mat1:1");}
  
    if (runmode==0) //initial screen if ncurses mode
      {
	mw->lookupTextChanged("Mat1:1");
	mw->panner(0,0);
	mw->drawscreen();
      }  

  c = mw->getnextchar(inputstring,pos);
    do {
      pos+=2;
        switch(c)
        {
	// case PRINTHELP: //'?'printhelp();mw->drawscreen();break; 
	case LOOKUP_STRING: //'l'
          response=mw->querystr("Lookup reference",inputstring,pos);
          pos+=response.size()+1;
	  mw->lookupTextChanged(response);
	  mw->panner(0,KEY_UP); mw->drawscreen(); 
	  break;
	case LOOKUP_SEARCHR: //'L'
	  mw->lookupTextChanged(mw->gettopsearch());
	  mw->panner(0,KEY_UP); mw->drawscreen(); 
	  break;
	case SEARCH_STRING: //'s'
	  response=mw->querystr("Search string",inputstring,pos);
	  mw->searchButtonClicked(response);
          pos+=response.size()+1;

	  // if (runmode==0) mw->drawscreen();
	  break;
	case WRITE_PC: //'W'
	  commstr=editfile("/tmp/irenaeuspc");
	  mw->PersonalCommentAdd("-+*Personal*+-" /*const string &modName*/,
				 mw->getcurverse() /*const string &startV*/, 
				 mw->getcurverse() /*const string &stopVer*/, 
				 commstr);
	  break;
	case SET_MODULE: // 'M'
	  response=mw->querystr("Module name",inputstring,pos);
	  mw->viewModActivate(response.c_str());
	  pos+=response.size()+1;
	  break;
	case PARAM_CHANGE: // 'D'
	  response=mw->querystr("change which, to what",inputstring,pos);
	  parsepc(mw,response);
	  pos+=response.size()+1;
	  break;
	case LIST_MODULES: // 'R'
	  response=mw->querystr("Module type",inputstring,pos);
	  mw->listmodules(response);
	  pos+=response.size()+1;
	  break;

        case KEY_UP:    // pan upwards 
	  mw->panner(0,KEY_UP);
	  mw->drawscreen();
	  break;
	case KEY_DOWN:  // pan downwards 
	  mw->panner(0,KEY_DOWN);
	  mw->drawscreen();
	  break;
	case PREVIOUS_BOOK:	// 'p' prev book 
	  mw->navigateButtonClicked(1);
	  break;
	case PREVIOUS_CHAP:	// '-'prev chapter 
	  mw->navigateButtonClicked(2);
	  break;
	case NEXT_CHAP:	// '+' next chapter 
	  mw->navigateButtonClicked(3);
	  break;
	case NEXT_BOOK:	//  'n' next book 
	  mw->navigateButtonClicked(4);
	  break;
	case SEARSCROLL_UP:	// 'P' prev search result 
	  mw->panner(1,KEY_UP);
	  break;
	case SEARSCROLL_DN:	// 'N' next search result 
	  mw->panner(1,KEY_DOWN);
	  break;
	case MENU_DISPLAY:	// 'm' menu 
	  //mw->drawmenu(mw->getModuleList()); // one to represent text type
	  mw->viewModActivate((mw->drawmenu(mw->getModuleList())).c_str());
	  //drawmenu(mw,1); // one to represent text type 
	  break;

	default:
	  break;
	}
	historystring+=(char)c+" "+response+" ";
	response="";
    } while
        ((c = mw->getnextchar(inputstring,pos)) != 'q');

    mw->~MainWindow();
    endwin();

    return (EXIT_SUCCESS);
}


void MainWindow::PersonalCommentAdd(const string &modName, const string &startVerse, const string &stopVerse, const string &comment)
{
        ModMap::iterator        it;
        SWModule         *commentModule = 0;
        VerseKey         startKey=startVerse.c_str();
        VerseKey         stopKey=stopVerse.c_str();

        it = mainMgr->Modules.find(modName.data());
        if (it != mainMgr->Modules.end())
        {
                commentModule = (*it).second;

                // should return Personal Commentary (or other RawFiles module)
                //      debug("Module is %s",commentModule->Name());

                startKey = startVerse.data();
                stopKey = stopVerse.data();

                commentModule->SetKey(startVerse.data()); // position mod to key                (*commentModule) << comment.data();   // set entry to comment
		cout<<comment.data();
                if ((*(SWKey *)*commentModule) < stopKey)
                {
                        (*commentModule)++;

                        while ((*(SWKey *)*commentModule) <= stopKey)
                        {
                                (*commentModule) << startKey;   // link all subsequent verses to the comment at 'startKey'
                                (*commentModule)++;
                        }
                }
                if (commentModule == curMod)
                        commentModule->Display();

                cout<<"Added Comment";
        } else
                // debug("Module %s not found!", commentModule->Name());
	  cout<<"Personal Commentary Module not found!";
}


void MainWindow::PersonalCommentRemove(const string &modName, const string &startVerse, const string &stopVerse)
{
  ModMap::iterator  it;
  SWModule          *commentModule = 0;
  VerseKey          startKey;
  VerseKey          stopKey;
  
  it = mainMgr->Modules.find(modName.data());
  if (it != mainMgr->Modules.end())
    {
      commentModule = (*it).second;
      
      // should return Personal Commentary (or other RawFiles module)
      //debug("Module is %s", commentModule->Name());
      
      startKey = startVerse.data();
      stopKey = stopVerse.data();
      
      commentModule->SetKey(startVerse.data()); // position mod to key                (*commentModule).Delete();   // remove entry
      if ((*(SWKey *)*commentModule) < stopKey)
	{
	  (*commentModule)++;
	  
	  while ((*(SWKey *)*commentModule) <= stopKey)
	    {
	      (*commentModule).Delete();   // link all subsequent verses to the comment at 'startKey'
	      (*commentModule)++;
	    }
	}
                if (commentModule == curMod)
		  commentModule->Display();
    }
}




