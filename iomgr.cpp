#include <string>
#include "irenaeus.h"


/* The behavior of mvhline, mvvline for negative/zero length is unspecified,
 * though we can rely on negative x/y values to stop the macro.
 */
static void do_h_line(int y, int x, chtype c, int to)
{
	if ((to) > (x))
		mvhline(y, x, c, (to) - (x));
}


static void do_v_line(int y, int x, chtype c, int to)
{
	if ((to) > (y))
		mvvline(y, x, c, (to) - (y));
}

static void
set_terminal_modes(void)
{
    noraw();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    idlok(stdscr, TRUE);
    keypad(stdscr, TRUE);
}


ioMgr::ioMgr(int t)
{
  int i;
  vnum=0;       
  inputtype=t;
  for (i=0;i<NUMVWIN;i++) 
    {
      _searchresult[i] = new stringlist;
      objecttype[i]=BIBLICALTEXT;
    }
  // _searchresult.clear(); 
	if (inputtype==0)
	  {
	    outlevel=1;
	    initscr(); 
	    for (i=0;i<NUMVWIN;i++)
	      {
		wd[i].top_x=1;	wd[i].top_y=1;
		wd[i].porty=LINES-1;	wd[i].portx=COLS;
		wd[i].basex=0;	wd[i].basey=0;
		wd[i].textWindow  =newpad(1000,wd[i].portx-wd[i].top_x);  
		keypad(wd[i].textWindow, TRUE);
	      }

	    bkgdset(BLANK);
	    start_color();
	    set_terminal_modes();
	    def_prog_mode();
	    drawscreen();
	    start_color();
	    hascolor=has_colors();
	    if (hascolor)
	      {
		//available colors: COLOR_BLACK COLOR_RED COLOR_GREEN  
		//COLOR_YELLOW COLOR_BLUE COLOR_MAGENTA COLOR_CYAN COLOR_WHITE
		init_pair(1,COLOR_RED,COLOR_BLACK); //pnum,fg,bg
		init_pair(2,COLOR_GREEN,COLOR_BLACK); 
		init_pair(3,COLOR_BLUE,COLOR_BLACK); 
	      }
	  };	
	if (inputtype==1)
	  {
	    outlevel=0;
	    for (i=0;i<NUMVWIN;i++) wd[i].textWindow=NULL;
	  }
	if (inputtype==2)
	  {
	    outlevel=0;
	    for (i=0;i<NUMVWIN;i++) wd[i].textWindow=NULL;
	  }
}


int ioMgr::getnextchar(string inputstring,unsigned int pos)
{
  int nchar;
  if (inputtype==0) 
    nchar= getch();
  if (inputtype==1)
    {
      string st;
      cin>>st;
      nchar=st[0];
    };
  if (inputtype==2)
    {
      if ((inputstring.size()>=pos)&(inputstring.size()>0))
	{
	  nchar=inputstring[pos];
	}
      else nchar='q';
    }
  return nchar;
}

int ioMgr::queryint(string str1,string inputstring,unsigned int pos)
{
  int resp; char tmpbuf[80];
  switch(inputtype)
    {
    case 0:  // interactive mode
      WINDOW *win;       
      win=newwin(3,30,4,4);
      wclear(win);wrefresh(win);
      attron(A_REVERSE);
      strcpy(tmpbuf,str1.c_str());
      mvwprintw(win,0,0,tmpbuf); 
      attroff(A_REVERSE);
      echo();
      //mvwscanw(win,0,14,"%s",&entryText);
      noecho();
      //resp=entryText;
      cin>>resp;
      break;
    case 1: // input from standard input
      cin>>resp;
      break;
    default:
      //resp=inputstring.substr(pos);
      //int n=resp.find_first_of(" \n");
      //resp=inputstring.substr(pos,n);

      //inputstring=resp;
      //inputstring.erase(1,n);
      break;
    }
  return resp;
};

string ioMgr::querystr(string str1,string inputstring,unsigned int pos)
{
  string resp; char tmpbuf[80];
  char entryText[30]="                        ";
  switch(inputtype)
    {
    case 0:  // interactive mode
      WINDOW *win;       
      win=newwin(3,30,4,4);
      wclear(win);wrefresh(win);
      attron(A_REVERSE);
      strcpy(tmpbuf,str1.c_str());
      mvwprintw(win,0,0,tmpbuf); 
      attroff(A_REVERSE);
      echo();
      mvwgetnstr(win,0,18,entryText,30);
      //mvprintw(win,0,18,">");
      //cin>>resp;
      noecho();
      resp=entryText;
      break;
    case 1: // input from standard input
      cin>>resp;
      break;
    default:
      resp=inputstring.substr(pos);
      if (resp[0]!='[')
	{
      int n=resp.find_first_of(" \n");
      resp=inputstring.substr(pos,n);
	}
      else
	{
	  int n=resp.find_first_of("]\n");
	  resp=inputstring.substr(pos+1,n-1);
	}
      //inputstring=resp;
      //inputstring.erase(1,n);
      break;
    }
  return resp;
};


void ioMgr::panner(int num,int c) //num is pan number, c is direction
{   
  WINDOW *tempwin;
  bool scrollers = TRUE;
  int pxmax, pymax, lowend, highend;
  int top_x,top_y,porty,portx,basex,basey;
  if (inputtype==0) {
    top_x=wd[vnum].top_x;      top_y=wd[vnum].top_y;
    porty=wd[vnum].porty;      portx=wd[vnum].portx;
    basex=wd[vnum].basex;      basey=wd[vnum].basey;

    tempwin=wd[vnum].textWindow;

    getmaxyx(tempwin, pymax, pxmax);
    scrollok(stdscr, FALSE);	/* we don't want stdscr to scroll! */
	
    switch(c)
      {
      case KEY_REFRESH:
	erase();

	/* FALLTHRU */
	
      case KEY_UP:	/* pan upwards */
	if (basey > 0)
	  basey--;
	break;

      case KEY_DOWN:	/* pan downwards */
	if (basey + porty - (pxmax > portx) < pymax)
	  basey++;
	else
	  beep();
	break;

      default:
	//beep();
	break;
      }

    mvaddch(top_y - 1, top_x - 1, ACS_ULCORNER);
    do_v_line(top_y, top_x - 1, ACS_VLINE, porty);
    do_h_line(top_y - 1, top_x, ACS_HLINE, portx); 

    if (scrollers && (pxmax > portx - 1)) {
      int length  = (portx - top_x - 1);
      float ratio = ((float) length) / ((float) pxmax);

      lowend  = (int)(top_x + (basex * ratio));
      highend = (int)(top_x + ((basex + length) * ratio));

      do_h_line(porty - 1, top_x, ACS_HLINE, lowend);
      if (highend < portx) {
	attron(A_REVERSE);
	do_h_line(porty - 1, lowend, ' ', highend + 1);
	attroff(A_REVERSE);
	do_h_line(porty - 1, highend + 1, ACS_HLINE, portx);
      }
    } else
      do_h_line(porty - 1, top_x, ACS_HLINE, portx);

    if (scrollers && (pymax > porty - 1)) {
      int length  = (porty - top_y - 1);
      float ratio = ((float) length) / ((float) pymax);

      lowend  = (int)(top_y + (basey * ratio));
      highend = (int)(top_y + ((basey + length) * ratio));

      do_v_line(top_y, portx - 1, ACS_VLINE, lowend);
      if (highend < porty) {
	attron(A_REVERSE);
	do_v_line(lowend, portx - 1, ' ', highend + 1);
	attroff(A_REVERSE);
	do_v_line(highend + 1, portx - 1, ACS_VLINE, porty);
      }
    } else
      do_v_line(top_y, portx - 1, ACS_VLINE, porty);

    mvaddch(top_y - 1, portx - 1, ACS_URCORNER);
    mvaddch(porty - 1, top_x - 1, ACS_LLCORNER);
    mvaddch(porty - 1, portx - 1, ACS_LRCORNER);

    wnoutrefresh(stdscr);
    pnoutrefresh(tempwin,
		 basey, basex,
		 top_y, top_x,
		 porty - (pxmax > portx) - 1,
		 portx - (pymax > porty) - 1);

        wd[vnum].basey=basey;

    doupdate();

    scrollok(stdscr, TRUE);	/* reset to driver's default */
    drawscreen();
  }
}


void ioMgr::drawscreen()
{
  if (inputtype ==0) {
    //int vertc=1; //char tmpbuf[80];

    /* show translation */
    // strcpy(tmpbuf,curMod->Name()); 
    mvprintw(0,25,(char *)wd[vnum].modname.c_str());

    /* show verse reference on top */
    // strcpy(tmpbuf,curMod->KeyText());
    mvprintw(0,40,(char *)wd[vnum].keyname.c_str());
    mvprintw(LINES-1,1,
	     "s=Search,l=lookup,m=module menu, +,- chapter, n,p/N,P books/search,q=quit");
    move(2,15);
    refresh();
  }
}


void ioMgr::modchanged()
{
  if (inputtype==0)
    {
      panner(0,0);
    }
}


static int menu_virtualize(int c)
{
  if (c == '\n' || c == KEY_EXIT)
    return(MAX_COMMAND + 1);
  else if (c == 'u')
    return(REQ_SCR_ULINE);
  else if (c == 'd')
    return(REQ_SCR_DLINE);
  else if (c == 'b' || c == KEY_NPAGE)
    return(REQ_SCR_UPAGE);
  else if (c == 'f' || c == KEY_PPAGE)
    return(REQ_SCR_DPAGE);
  else if (c == 'n' || c == KEY_DOWN)
    return(REQ_NEXT_ITEM);
  else if (c == 'p' || c == KEY_UP)
    return(REQ_PREV_ITEM);
  else if (c == ' ')
    return(REQ_TOGGLE_ITEM);
  else {
    if (c != KEY_MOUSE)
      beep();
    return(c);
  }
}

string ioMgr::drawmenu(moduledeflist *_moduleList)
{
  MENU	*m;
  ITEM	*items[1000];
  ITEM	**ip = items;
  int		mrows, mcols, c;
  WINDOW	*menuwin;
  moduledeflist::iterator it;
  moduledef *md;
  string text;
  string retstr;
#ifdef NCURSES_MOUSE_VERSION
  if (inputtype==0) mousemask(ALL_MOUSE_EVENTS, (mmask_t *)0);
#endif
  /*      'n' and 'p' act as down,up arrosws,
	  'b' and 'f' scroll up/down (page), 
	  'u' and 'd' (line).");
  */
    
  for (it=_moduleList->begin(); it!=_moduleList->end(); it++)
    {
      md = new moduledef;
      md->name = it->name;//md->name=moList->current()->name.copy();
      md->type = it->type;//moList->current()->type.copy();
      md->description = it->description;
      text = md->name + " (" + md->description + ")";
      md->custom = text;
      if (inputtype!=0) cout<<md->name<<"\n"; else
	*ip++ = new_item(md->name.c_str(), "");
    }
  if (inputtype==0)
    {
      *ip = (ITEM *)0;
      m = new_menu(items);
      set_menu_format(m,MENUROWS, 1);
      scale_menu(m, &mrows, &mcols);
      menuwin = newwin(mrows+2, mcols +  2, MENU_Y, MENU_X);
      set_menu_win(m, menuwin);
      keypad(menuwin, TRUE);
      box(menuwin, 0, 0);
	
      set_menu_sub(m, derwin(menuwin, mrows, mcols, 1, 1));
	
      post_menu(m);
	
      while ((c = menu_driver(m, menu_virtualize(wgetch(menuwin)))) != E_UNKNOWN_COMMAND) {
	if (c == E_REQUEST_DENIED)
	  beep();
	continue;
      }
	
      unpost_menu(m);
      delwin(menuwin);
      // viewModActivate(item_name(current_item(m)));
      retstr=item_name(current_item(m));
      free_menu(m);
      for (ip = items; *ip; ip++)
	free_item(*ip);
#ifdef NCURSES_MOUSE_VERSION
      mousemask(0, (mmask_t *)0);
#endif
    }
  if (inputtype==1) {
      
  }
  if (inputtype==2) {
      
  }
  return retstr;
}


void ioMgr::updateDisplay(SWModule &imodule) 
{
  if ( wd[vnum].textWindow!=NULL)
    Display(imodule,NULL);
}

void ioMgr::Display(SWModule &imodule) 
{
  Display(imodule,NULL);
}


void ioMgr::Display(SWModule &imodule,VerseKey *endvkey) {
  int dcolumn=1;
  int i;
  unsigned int pos,pos2;
  string line;
  string linestr;
  int linesize;
  int cwidth;
  char c1;
  char versenum[4];

  string tmpType;

  tmpType=imodule.Type();

  /* get module name */
  wd[vnum].modname=imodule.Name(); 
  
  /* get verse reference */
  wd[vnum].keyname=imodule.KeyText();
  
  if (inputtype==0) 
    {
      wclear(wd[vnum].textWindow);
      wmove(wd[vnum].textWindow,2,1);
    }
  
  // set recent Key
  if ((tmpType=="Biblical Texts")|(tmpType=="Commentaries")) 
    {
      if (objecttype[vnum]==0)
	{

      VerseKey *key = (VerseKey *)(SWKey *)imodule;
      int curVerse = key->Verse();
      int curChapter = key->Chapter();
      int curBook = key->Book();
      int curTestament = key->Testament();
      int endVerse=curVerse;
      int endChapter=curChapter;
      int endBook=curBook;
      int endTestament=curTestament;
      
      if (outlevel==1)
	{
	  if (endvkey==NULL) key->Verse(1);
	  endVerse=500; //larger than necessary
	  endChapter = curChapter;
	  endBook = curBook;
	  endTestament = curTestament;
	};
      if (endvkey!=NULL)
	{
	  endVerse = endvkey->Verse();
	  endChapter = endvkey->Chapter();
	  endBook = endvkey->Book();
	  endTestament = endvkey->Testament();
	}
      for (; //already did all necessary init
	   ( (key->Testament() <  endTestament)||
	     ((key->Testament() ==  endTestament)&&(key->Book() <  endBook))||
	     ((key->Testament() ==  endTestament)&&(key->Book() == endBook)&& ( key->Chapter() < endChapter))||
	     ((key->Testament() ==  endTestament)&&(key->Book() == endBook)&& ( key->Chapter() ==endChapter) && (key->Verse() <= endVerse))) && 
	     !imodule.Error(); 
	   imodule++) 
	{
	  sprintf(versenum,"%d", key->Verse());
	  // if (!hascolor)  XXX I want to add color and remove []
	      linestr="[";
	      linestr+=versenum;
	      linestr+="]";
	  linestr+=(const char *)imodule;
	    
	  pos=0;           //remove < ... > from text
	  while (pos<linestr.size())
	    {
	      pos=linestr.find_first_of("<");
	      pos2=linestr.find_first_of(">");
	      if ((pos<linestr.size())&(pos<linestr.size()))
		linestr=linestr.substr(0,pos)+linestr.substr(pos2+1,linestr.size()-pos2);
	    }
	    
	  pos=0;//remove { ... } from text
	  while (pos<linestr.size())
	    {
	      pos=linestr.find_first_of("{");
	      pos2=linestr.find_first_of("}");
	      if ((pos<linestr.size())&(pos<linestr.size()))
		linestr=linestr.substr(0,pos)+linestr.substr(pos2+1,linestr.size()-pos2);
	    }
	    
	    
	  // try to put in CR to get correct word wrap.w

	  linesize= linestr.size();
	  pos=0;
	  if (wd[vnum].textWindow!=NULL)
	  for (i=0;i<linesize;i++)
	    {	
	      cwidth=wd[vnum].portx-wd[vnum].top_x-2;
	      c1=linestr[i];
	      dcolumn++;
	      if (c1==' ') 
		pos=i; //pos identifies the last encountered space
	      else if (c1=='\n') 
		{
		  dcolumn=1;
		} //move back to left column
	      if (dcolumn>cwidth) 
		{
		  linestr[pos]='\n';
		  dcolumn=i-pos;
		}
	    }

	  if (wd[vnum].textWindow!=NULL)
	    wprintw(wd[vnum].textWindow,(char *)linestr.c_str());
	  else
	    cout <<linestr;
	}  //end for loop
 
      key->Book(curBook);
      key->Chapter(curChapter);
      key->Verse(curVerse);
	}
      else
	{
	  if (objecttype[vnum]==1) 
	    {
	      if (inputtype==0) 
		{
		  int n;
		  list<string>::iterator sn;
		  n=wd[1].basey;
		  for (sn=_searchresult[vnum]->begin();
		       sn!=_searchresult[vnum]->end();++sn)
		    displaysearch((*sn).c_str());
		}
	    }
	}
    } //end if bible or commentray
  else 
    {
      if (inputtype==0) 
	wprintw(wd[vnum].textWindow,"%s",(const char *)imodule);
      else cout<<(const char *)imodule;
    }
}


/*
void ioMgr::displayverse(string Text)
{
  if (inputtype==0) 
    wprintw(textWindow,"%s",Text.c_str());
  else cout<<Text;
}
*/
VerseKey ioMgr::gettopsearch()
{
  list<string>::iterator sn;
  string retstr;
  string tmpstr;
  if (inputtype==0) 
    {
      int n;
      n=wd[vnum].basey;
      int i=0;
      for (sn=_searchresult[vnum]->begin();i<n-1;++sn)
	i++;
      tmpstr=*sn;
      //retstr="Lev4:5";
      //retstr.replace(0,tmpstring.size(),tmpstring);

    }
  VerseKey vk=tmpstr.c_str();
  return vk;
}


void ioMgr::displaysearch(string Text)
{ 

  if (inputtype==0) 
    wprintw(wd[vnum].textWindow,"%s\n",Text.c_str());
  else cout<<Text.c_str()<<"\n";

  _searchresult[vnum]->push_back(Text);
}

/*
void ioMgr::displaysearch()
{ 
  list<string>::iterator sn;
  string Text="";
  for (sn=_searchresult[vnum]->begin();sn!=_searchresult[vnum]->end();sn++)
    {
      Text=sn;
      if (inputtype==0) 
	wprintw(wd[vnum].textWindow,"%s\n",Text.c_str());
      else cout<<Text.c_str()<<"\n";
      
      _searchresult->push_back(Text);
    }
}
*/
void ioMgr::clearsearch()
{ 
  if (inputtype==0) 
    {   
      wclear(wd[vnum].textWindow);
      wrefresh(wd[vnum].textWindow);
    }
  _searchresult[vnum]->clear();
}

void ioMgr::switchvirwin(int wn)
{ 
  if (vnum!=wn)
    {   
      vnum=wn;
      if (inputtype==0) 
        {   
          erase();
          refresh();
          panner(0,0);
        }
    }
}

void ioMgr::setvnum(int vn)
{ 
  if ((vn>-1)&(vn<NUMVWIN))
    {   
      vnum=vn;
      if (inputtype==0) 
	{   
	  erase();
	  refresh();
	}
      panner(0,0);
    }
}

int ioMgr::getvnum()
{ 
  return vnum;
}


void ioMgr::popwin(string msg)
{
      WINDOW *win;       
      win=newwin(20,70,0,0);
      wclear(win);
      attron(A_REVERSE);
      mvwprintw(win,2,2,(char *)msg.c_str()); 
      attroff(A_REVERSE);
      echo();
      wrefresh(win);
}

void ioMgr::setobjecttype(int mnum,int objtype)
{
  if ((mnum>-1)&(mnum<NUMVWIN))
  objecttype[mnum]=objtype;
}
