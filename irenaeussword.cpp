/*  irenaeus
 *  Copyright (C) 1999, CrossWire Bible Society
 *			P. O. Box 2528
 *			Tempe, AZ  85280-2528
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

#include <curses.h>
#include <menu.h>
#include <panel.h>
#include "irenaeus.h"
#include <swmgr.h>
#include <versekey.h>
#include <regex.h>
#include <stream.h>
#include <swmgr.h>
#include <string>
#include "filters/plainfootnotes.h"
//#include "filters/thmlhtml.h"

#include <gbfhtml.h>
#include <rwphtml.h>
#include <plainhtml.h>
#include <rawgbf.h>

// wnat to get rid of this.  How do you do atoi in C++?
#include <stdlib.h>

MainWindow::MainWindow(int rmode):ioMgr(rmode)  /* XXX ioMgr(rmode) */ {
	mainMgr          = new irenaeusMgr();
		curMod           = NULL;
		//chapDisplay      = 0;	// set in create
		//entryDisplay     = 0;	// set in create
	initSWORD();
	//chapDisplay = new ChapDisp("");
	//entryDisplay = new EntryDisp("");
	_moduleList = new moduledeflist;
	getModuleList(); 
	runmode=rmode;
}


MainWindow::~MainWindow() {
  /*	if (chapDisplay)
		delete chapDisplay;
	if (entryDisplay)
	delete entryDisplay; */
}


void MainWindow::initSWORD() {

  //  int viewNumber = 1;
  //  char menuName[64];
  ModMap::iterator it;
  SectionMap::iterator sit;
  ConfigEntMap::iterator eit;
  char *font;
  SWModule *curMod;
  VerseKey vkey();
  
  for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
    curMod = (*it).second;
    if (!strcmp((*it).second->Type(), "Biblical Texts")) {      
      font = 0;
      if ((sit = mainMgr->config->Sections.find((*it).second->Name())) != mainMgr->config->Sections.end()) {
	if ((eit = (*sit).second.find("Font")) != (*sit).second.end()) {
	  font = (char *)(*eit).second.c_str();
	}
      }
      //curMod->Disp(chapDisplay);	// set our ChapDisp object up for the diplayer of each Biblical Text module
      if (!this->curMod) 	// set currently selected module for app to first module from SWMgr (Bible Texts get first preference
	this->curMod = curMod;
    }
		else	{
		  //curMod->Disp(entryDisplay);	// set our EntryDisp object up for the diplayer of each module other than Biblical Texts
		  if (!strcmp((*it).second->Type(), "Commentaries")) { }
		  if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")) { }
		}
  }
  if (!this->curMod) 	// set currently selected module for app to first module from SWMgr
    this->curMod = curMod;
}


string getverse(SWModule &imodule)
{
return (string)(const char *)imodule;
}


//rely on fact that not found implies gen1:1 which is 
//the only nonvalid postkey
VerseKey * gettextrange(string prehyp,string posthyp,SWModule &imodule)
{
  int endv,endc;
  VerseKey *key;
  if (posthyp.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz")!=string::npos) //XXX find a better way to check for bookname
    {
      imodule.SetKey(posthyp.c_str());
      key = (VerseKey *)(SWKey *)imodule;
    }
  else
    {
      imodule.SetKey(prehyp.c_str());
      key = (VerseKey *)(SWKey *)imodule;
      unsigned int tmpind=posthyp.find(":");
      if (tmpind==string::npos) //only single number in post
	{
	  string vnum(posthyp);
	  endv=atoi(vnum.c_str());
	  key->Verse(endv);
	}
      else
	{
	  string vnum(posthyp.substr(tmpind+1,posthyp.size()));
	  string cnum(posthyp.substr(0,tmpind));
	  endv=atoi(vnum.c_str());   
	  endc=atoi(cnum.c_str());
	  key->Chapter(endc);
	  key->Verse(endv);
	}
    }
  return key;
}


void MainWindow::lookupTextChanged(string keyText) {
	if (curMod) {
	  unsigned int idx=keyText.find("-"); // a - indicates verse range
	  if (idx == string::npos)
	    {
	  curMod->SetKey(keyText.c_str());
	  Display(*curMod);
	    }
	  else
	    {
	      string pretext=keyText.substr(0,idx-1);
	      VerseKey *vk=gettextrange(pretext,
			   keyText.substr(idx+1,keyText.size()),*curMod);
	      VerseKey copykey=vk->clone();
	      copykey.Chapter(vk->Chapter()); //XXX Why is this necessary
	      copykey.Verse(vk->Verse());
	      curMod->SetKey(pretext.c_str());
	      Display(*curMod,&copykey);
	    }
	}
	vkey=new VerseKey(keyText.c_str());
	
}

void MainWindow::viewModActivate(const char *modName) {
	ModMap::iterator it;
	string text;
	VerseKey vKey;

	vKey = (*(SWKey *)*curMod);		      
	text=((const char *) vKey);
	it = mainMgr->Modules.find(modName);
	if (it != mainMgr->Modules.end()) {
		curMod = (*it).second;
		curMod->SetKey(text.c_str());
		//lookupTextChanged(text);
	}
	updateDisplay(*curMod);
	modchanged();
}

string MainWindow::getcurverse()
{ 
  string text;
  VerseKey vKey; 
  vKey = (*(SWKey *)*curMod);
  text=((const char *) vKey); 
  return (text);
}

void MainWindow::navigateButtonClicked(int direction) {
  string text;
  VerseKey vKey;
  if (curMod) {
    if (!strcmp(curMod->Type(), "Biblical Texts")) {
      vKey = (*(SWKey *)*curMod);
      
      if (direction==1)		    
	vKey.Book(vKey.Book()-1);			      
      if (direction==2)
	vKey.Chapter(vKey.Chapter()-1);		
      if (direction==3)	
	vKey.Chapter(vKey.Chapter()+1);
      if (direction==4)		    
	vKey.Book(vKey.Book()+1);			      
      text=((const char *) vKey);
      lookupTextChanged(text);
    }
  }
  panner(0,KEY_UP);
}


void MainWindow::searchButtonClicked(string srchText) { 
	const char *resultText;
	clearsearch();
	if (curMod) 
	  {
	    int searchType = 1,
	      searchParams = 1;
	    ListKey &searchResults = 
		   curMod->Search(srchText.c_str(),searchType, searchParams);
	    
	    for (; !searchResults.Error(); searchResults++) {
	    resultText = (const char *)searchResults;
	    displaysearch(resultText);
	    };
	    panner(1,KEY_UP);
	  }
}

/*
char EntryDisp::Display(WINDOW *win,SWModule &imodule) {

	(const char *)imodule;	// snap to entry
	wprintw(win,"[%s] ", imodule.KeyText());
	}*/


moduledeflist * MainWindow::getModuleList()
{
  moduledef *moDef;
  SWModule         *mo;
  _moduleList->clear();
  
  // use SWMgr to traverse installed modules
  ModMap::iterator modIterator;
  
  for (modIterator = mainMgr->Modules.begin(); 
       modIterator != mainMgr->Modules.end(); modIterator++)
    {
      mo = (*modIterator).second;
      moDef = new moduledef;
      moDef->name=((mo->Name() != 0 ? mo->Name() : ""));
      moDef->description=((mo->Description() != 0 ? mo->Description() : ""));
      moDef->type=((mo->Type() != 0 ? mo->Type() : ""));
      _moduleList->push_back(*moDef);
    }
  moduledeflist *rmoddef=_moduleList;
  return rmoddef;
}


irenaeusMgr::irenaeusMgr() : SWMgr()
{
//These lines are commented because you have to change some SWORD things to usw this code.
//This enables the filter for footnotes in plain text
  //SWFilter *tmpFilter = 0;
        //      tmpFilter = new PLAINFootnotes();
        //optionFilters.insert(FilterMap::value_type("PLAINFootnotes", tmpFilter));
        //cleanupFilters.push_back(tmpFilter);

  //       gbftohtml       = new GBFHTML();
  //      rwptohtml       = new RWPHTML();
        //plaintohtml = new PLAINHTML();
//      thmltohtml      = new ThMLHTML();

        Load();
}


irenaeusMgr::~irenaeusMgr()
{
  //       if (gbftohtml!=0)
  //              delete gbftohtml;

  //        if (rwptohtml!=0)
  //              delete rwptohtml;

	//  if (plaintohtml!=0)
        //        delete plaintohtml;
}

#if 0
void irenaeusMgr::AddRenderFilters(SWModule *module, ConfigEntMap &section)
{
	string sourceformat;
	string moduleDriver;
	ConfigEntMap::iterator entry;
	bool noDriver = true;

	sourceformat = ((entry = section.find("SourceType")) != section.end()) ? (*entry).second : (string) "";
	moduleDriver = ((entry = section.find("ModDrv")) != section.end()) ? (*entry).second : (string) "";
	// Temporary: To support old module types
	/*	if (sourceformat.empty())	{
		if (dynamic_cast<RawGBF *>(module))
			sourceformat = "GBF";
	}
	if (!stricmp(sourceformat.c_str(), "GBF")) {
		module->AddRenderFilter(gbftohtml);
		noDriver = false;
	}
	if (!stricmp(sourceformat.c_str(), "PLAIN")) {
		module->AddRenderFilter(plaintohtml);
		noDriver = false;
		} */

/*	if (!stricmp(sourceformat.c_str(), "ThML")) {
		module->AddRenderFilter(thmltohtml);
		noDriver = false;
	}    eliminated i's in stricmp's below */	
	//	if (!strcmp(module->Name(), "-+*personal*+-")) {
	//	module->AddRenderFilter(plaintohtml);
	//	noDriver = false;
	//}
	if (!strcmp(module->Name(), "RWP")) {
		module->AddRenderFilter(rwptohtml);
		noDriver = false;
	}

	if (noDriver){
	  //		if (!strcmp(moduleDriver.c_str(), "RawCom")) {
	  //		module->AddRenderFilter(plaintohtml);
	  //		noDriver = false;
	  //	}
	  //	if (!strcmp(moduleDriver.c_str(), "RawLD")) {
	  //		module->AddRenderFilter(plaintohtml);
	  //		noDriver = false;
	  //	}
	}
	
	/** Use PLAINHTML as standard filter, if no is specified use plaintohtml
		* I used a profiler and saw this function eats more than 80% of the program power!
		* We have to optimize the PLAINTOHTML filter.
	*/
	if (noDriver){
	  //module->AddRenderFilter(plaintohtml);
		noDriver = false;
	};
}

#endif









