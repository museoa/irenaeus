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


#define FRAME struct frame
FRAME
{
	FRAME		*next, *last;
	bool		do_scroll;
	bool		do_keypad;
	WINDOW		*wind;
};

static string helpstr;

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

string getfile(string fname)
{
  string retstr="";
  string callstr;
  char buffer[800];
  retstr="";
  // now read in file, and return it
  ifstream in1(fname.c_str());
  while (!in1.eof())
    {
      in1.getline(buffer,800);
      retstr+=buffer;
    };
  return(retstr);
}

void usage()
{
  cout<<"Usage: irenaeus \n or: irenaeus -f\n or: irenaeus -c input string ";
}


/* Global variables */
int runmode=0;


/* change parameters.  currently very ad hoc. */
void parsepc(MainWindow *mw, string  varnum, string varval)
{
  int num,val,snum;
  div_t dt;
  num=atoi(varnum.c_str());
  val=atoi(varval.c_str());

  if (num<100)
    {
      switch(val)
	{
	case 0: mw->setsearchType(val);break;
	case 1: mw->setsearchParams(val);break;
	}
    }
  else if (num<200)
    {
      dt=div(num-100,10);
      snum=dt.quot;
      switch(dt.rem)
	{
	case 0: mw->changewindowtop(snum,val);break;
	case 1: mw->changewindowbottom(snum,val);break;
	case 2: mw->changewindowleft(snum,val);break;
	case 3: mw->changewindowright(snum,val);break;
	}
    }

  else
    {
      dt=div(num-200,10);
      mw->linkscr(dt.quot,dt.rem,val);
    }

}

void commandloop(MainWindow *mw,int runmode,string inputstring, int pos)
{
  int c=0;
  int tmpint;
  string response,response2,tmpstr,historystring;
  VerseKey vk;
  c = mw->getnextchar(inputstring,pos);
  do {
    pos+=2;
    switch(c)
      {	
      case LOOKUP_STRING: //'l'
	response=mw->querystr("Lookup reference",inputstring,pos);
          pos+=response.size()+1;
	  mw->lookupTextChanged(response);
	  mw->panner(0);  
	  break;
      case LOOKUP_SEARCHR: //'L'
	vk=mw->gettopsearch();
	mw->lookupTextChanged((const char *) vk);
	mw->panner(0);  
	break;
      case SEARCH_STRING: //'s'
	response=mw->querystr("Search string",inputstring,pos);
	mw->searchButtonClicked(response);
	pos+=response.size()+1;
	
	  // if (runmode==0) 
	break;
	case WRITE_PC: //'W'
	  tmpstr=editfile("/tmp/irenaeuspc");
	  mw->PersonalCommentAdd("-+*Personal*+-" /*const string &modName*/,
				 mw->getcurverse() /*const string &startV*/, 
				 mw->getcurverse() /*const string &stopVer*/, 
				 tmpstr);
	  //mw->PersonalCommentAdd("-+*Personal*+-","Matthew1:1","Matthew1:5","This is an inline test");
	  break;
	case SET_MODULE: // 'M' enter module choice
	  response=mw->querystr("Module name",inputstring,pos);
	  mw->viewModActivate(response.c_str());
	  pos+=response.size()+1;
	  break;
	case PARAM_CHANGE: // 'D' //change global variables
	  response=mw->querystr("change which variable",inputstring,pos);
	  pos+=response.size()+1;
	  response2=mw->querystr("to what value",inputstring,pos);
	  parsepc(mw,response,response2);
	  pos+=response2.size()+1;
	  break;
	case LIST_MODULES: // 'R'
	  response=mw->querystr("Module type",inputstring,pos);
	  mw->listmodules(response);
	  pos+=response.size()+1;
	  break;
        case KEY_UP:    // pan upwards 
	  mw->panner(KEY_UP);	  
	  break;
	case KEY_DOWN:  // pan downwards 
	  mw->panner(KEY_DOWN);	  
	  break;
        case KEY_LEFT:    // pan left 
	  mw->panner(KEY_LEFT);	  
	  break;
	case KEY_RIGHT:  // pan right 
	  mw->panner(KEY_RIGHT);	  
	  break;
	case PREVIOUS_BOOK:	// 'p' prev book 
	  mw->navigateButtonClicked(1);
	  break;
	case PREVIOUS_CHAP:	// '-'prev chapter 
	  mw->navigateButtonClicked(2);
	  break;
	case PREVIOUS_VERSE:	// '_'prev verse 
	  mw->navigateButtonClicked(3);
	  break;
	case NEXT_VERSE:	// '=' next chapter 
	  mw->navigateButtonClicked(4);
	  break;
	case NEXT_CHAP:	// '+' next chapter 
	  mw->navigateButtonClicked(5);
	  break;
	case NEXT_BOOK:	//  'n' next book 
	  mw->navigateButtonClicked(6);
	  break;
	case MENU_DISPLAY:	// 'm' menu 
	  //mw->drawmenu(mw->getModuleList()); // one to represent text type
	  mw->viewModActivate((mw->drawmenu(mw->getModuleList())).c_str());
	  //drawmenu(mw,1); // one to represent text type 
	  break;
	case PRINT_HELP:      // 'h' help screen
	  mw->popwin(helpstr);
	  break;
	case READ_FILE:      //'r' read file
          response=mw->querystr("file to read",inputstring,pos);
          pos+=response.size()+1;
	  tmpstr=getfile(response);
	  tmpint=mw->getinputtype();
	  mw->setinputtype(2);
	  commandloop(mw,2,tmpstr,0);
	  mw->setinputtype(tmpint);
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
}
/*+-------------------------------------------------------------------------
	main(argc,argv)
--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  
  int my_e_param = 1,inputlen;
  unsigned int pos=0;
  string inputstring,historystring,swstr; 

    helpstr="Irenaeus keybindings ";
    helpstr+='\n';
    helpstr+=" help screen       ";helpstr+=PRINT_HELP;helpstr+=   "   ";
    helpstr+=" search for string ";helpstr+=SEARCH_STRING;helpstr+="   ";
    helpstr+=" lookup ref.       ";helpstr+=LOOKUP_STRING;helpstr+='\n';

    helpstr+=" lookup search     ";helpstr+=LOOKUP_SEARCHR;helpstr+="   ";
    helpstr+=" write commentary  ";helpstr+=WRITE_PC;helpstr+=     "   ";
    helpstr+=" change module     ";helpstr+=SET_MODULE;helpstr+='\n';

    helpstr+=" list modules      ";helpstr+=LIST_MODULES;helpstr+= "   ";
    helpstr+=" change parameters ";helpstr+=PARAM_CHANGE;helpstr+= "   ";
    helpstr+=" previous book     ";helpstr+=PREVIOUS_BOOK;helpstr+='\n';
    helpstr+=" next book         ";helpstr+=NEXT_BOOK;helpstr+=    "   ";
    helpstr+=" previous chapter  ";helpstr+=PREVIOUS_CHAP;helpstr+="   ";
    helpstr+=" next chapter      ";helpstr+=NEXT_CHAP;helpstr+='\n';
    helpstr+=" previous verse    ";helpstr+=PREVIOUS_VERSE;helpstr+="   ";
    helpstr+=" next verse        ";helpstr+=NEXT_VERSE;helpstr+=    "   ";
    helpstr+=" menu of modules   ";helpstr+=MENU_DISPLAY;helpstr+='\n';
    helpstr+=" read file         ";helpstr+=READ_FILE;helpstr+=    "   ";

    inputstring.erase();pos=0;
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
      
    // tell it we're going to play with soft keys 
    slk_init(my_e_param);
    // we must initialize the curses data structure only once 
  
    if (runmode==0) //initial screen if ncurses mode
      {
	mw->setvnum(9);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("Mat1:1");mw->panner(0);
	mw->setvnum(8);	
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("Rev1:1");mw->panner(0);
	mw->setvnum(7);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("Luke1:1");mw->panner(0);
	mw->setvnum(6);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("John1:1");mw->panner(0);
	mw->setvnum(5);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("Psalm1:1");mw->panner(0);
	mw->setvnum(4);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("Romans1:1");mw->panner(0);
	mw->setvnum(3);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("Phili1:1");mw->panner(0);
	mw->setvnum(2);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("galat1:1");mw->panner(0);
	mw->setvnum(1);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("1John1:1");mw->panner(0);
	mw->setvnum(0);
	mw->viewModActivate("KJV");
	mw->lookupTextChanged("Jude1:1");mw->panner(0);
      }  
    commandloop(mw,runmode,inputstring,pos);
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




