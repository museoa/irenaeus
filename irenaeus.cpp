/****************************************************************************
 * Copyright (c) 2000 Free Software Foundation, Inc.                        *
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

/*
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
#endif // NCURSES_MOUSE_VERSION 
*/

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
    else 
      if (mtype=="2") compstr="Commentaries";
      else 
	if (mtype=="3") compstr="Lexicons / Dictionaries";

    for (it=_moduleList->begin(); it!=_moduleList->end(); it++)
      {
	if (it->type==compstr)
	  namelst += it->name+" ";
                //text = md->name + " (" + md->description + ")";
      }
    return namelst;
}


string editfile(string fname)
{
  string retstr="";
  string callstr;
  char buffer[80];
  retstr="Initial value of string";
  //def_prog_mode();
  //endwin();
  callstr="vi "+fname;
  //callstr="emacs "+fname;
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



void parsepc(MainWindow *mw, string str)
{

}


/*+-------------------------------------------------------------------------
	main(argc,argv)
--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  
  int c=0,my_e_param = 1,inputlen;
  unsigned int pos=0;
  VerseKey vk;
  string inputstring,historystring,swstr,commstr,tmpstr; 
    string response;
    tmpstr="Ex2:3                           ";
    string helpstr="Irenaeus keybindings ";
    helpstr+='\n';
    helpstr+="help screen         ";helpstr+=PRINTHELP;helpstr+="     ";
    helpstr+="lookup ref.         ";helpstr+=LOOKUP_STRING;helpstr+='\n';
    helpstr+="search for string   ";helpstr+=SEARCH_STRING;helpstr+="     ";
    helpstr+="lookup from search  ";helpstr+=LOOKUP_SEARCHR;helpstr+='\n';
    helpstr+="write personal com  ";helpstr+=WRITE_PC;helpstr+="     ";
    helpstr+="change module       ";helpstr+=SET_MODULE;helpstr+='\n';
    helpstr+="list modules        ";helpstr+=LIST_MODULES;helpstr+="     ";
    helpstr+="change parameters   ";helpstr+=PARAM_CHANGE;helpstr+='\n';
    helpstr+="previous book       ";helpstr+=PREVIOUS_BOOK;helpstr+="     ";
    helpstr+="next book           ";helpstr+=NEXT_BOOK;helpstr+='\n';
    helpstr+="previous chapter    ";helpstr+=PREVIOUS_CHAP;helpstr+="     ";
    helpstr+="next chapter        ";helpstr+=NEXT_CHAP;helpstr+='\n';
    helpstr+="change module(menu) ";helpstr+=MENU_DISPLAY;helpstr+="     ";

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
	mw->switchvirwin(9);
	mw->lookupTextChanged("Mat1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(8);
	mw->lookupTextChanged("Rev1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(7);
	mw->lookupTextChanged("Luke1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(6);
	mw->lookupTextChanged("John1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(5);
	mw->lookupTextChanged("Psalm1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(4);
	mw->lookupTextChanged("Romans1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(3);
	mw->lookupTextChanged("Phili1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(2);
	mw->lookupTextChanged("galat1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(1);
	mw->lookupTextChanged("1John1:1");mw->panner(0,0);mw->drawscreen();
	mw->switchvirwin(0);
	mw->lookupTextChanged("Jude1:1");mw->panner(0,0);mw->drawscreen();
      }  

  c = mw->getnextchar(inputstring,pos);
    do {
      pos+=2;
        switch(c)
        {	
	case LOOKUP_STRING: //'l'
          response=mw->querystr("Lookup reference",inputstring,pos);
          pos+=response.size()+1;
	  mw->lookupTextChanged(response);
	  mw->panner(0,0); mw->drawscreen(); 
	  break;
	case LOOKUP_SEARCHR: //'L'
	  vk=mw->gettopsearch();
	  mw->lookupTextChanged((const char *) vk);
	  mw->panner(0,0); mw->drawscreen(); 
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
	  //mw->PersonalCommentAdd("-+*Personal*+-","Matthew1:1","Matthew1:5","This is an inline test");
	  break;
	case SET_MODULE: // 'M' enter module choice
	  response=mw->querystr("Module name",inputstring,pos);
	  mw->viewModActivate(response.c_str());
	  pos+=response.size()+1;
	  break;
	case PARAM_CHANGE: // 'D' //change global variables
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
/* this should be obsolete now
	case SEARSCROLL_UP:	// 'P' prev search result 
	  mw->panner(1,KEY_UP);
	  break;
	case SEARSCROLL_DN:	// 'N' next search result 
	  mw->panner(1,KEY_DOWN);
	  break; */
	case MENU_DISPLAY:	// 'm' menu 
	  //mw->drawmenu(mw->getModuleList()); // one to represent text type
	  mw->viewModActivate((mw->drawmenu(mw->getModuleList())).c_str());
	  //drawmenu(mw,1); // one to represent text type 
	  break;
	case PRINT_HELP:      // 'h' help screen
	  mw->popwin(helpstr);
	  break;
	case '0':
	  mw->switchvirmod(0);
	  break;
	case '1':
	  mw->switchvirmod(1);
	  break;
	case '2':
	  mw->switchvirmod(2);
	  break;
	case '3':
	  mw->switchvirmod(3);
	  break;
	case '4':
	  mw->switchvirmod(4);
	  break;
	case '5':
	  mw->switchvirmod(5);
	  break;
	case '6':
	  mw->switchvirmod(6);
	  break;
	case '7':
	  mw->switchvirmod(7);
	  break;
	case '8':
	  mw->switchvirmod(8);
	  break;
	case '9':
	  mw->switchvirmod(9);
	  break;

	default:
	  break;
	}
	historystring+=(char)c+" "+response+" ";
	response="";
    } while
        ((c = mw->getnextchar(inputstring,pos)) != 'q');

//    mw->~MainWindow();
    endwin();

    return (EXIT_SUCCESS);
}


void MainWindow::PersonalCommentAdd(const string &modName, const string &startVerse, const string &stopVerse, const string &comment)
{
        ModMap::iterator        it;
        SWModule             *commentModule = 0;
        VerseKey             startKey=startVerse.c_str();;
        VerseKey             stopKey=stopVerse.c_str();

        it = mainMgr->Modules.find(modName.data());
        if (it != mainMgr->Modules.end())
        {
                commentModule = (*it).second;

                // should return Personal Commentary (or other RawFiles module)
                cout<< "Module is %s" << commentModule->Name(); //debug

	//startKey = startVerse.data();
        //        stopKey = stopVerse.data();

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




