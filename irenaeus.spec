# 
# Irenaeus
# 
%define prefix   /usr

Summary: An ncurses front end to the sword library
Name: irenaeus
Version: 0.14 
Release: 1 
Copyright: GPL 
Group: Applications 
Source: ftp://www4.ncsu.edu/unity/users/r/raholcom/www/irenaeus/irenaeus-0.14.tar.gz 
URL: http://www4.ncsu.edu/unity/users/r/raholcom/www/irenaeus/index.html 
Packager: Richard Holcombe <raholcom@eos.ncsu.edu>

%description 
This program interfaces to the sword library.  It can be run as an ncurses
program or simply as a command line program.

%prep
rm -rf $RPM_BUILD_DIR/irenaeus
zcat $RPM_SOURCE_DIR/irenaeus-0.14.tar.gz | tar -xvf -

%build
cd /usr/src/redhat/BUILD/irenaeus
./configure
make

%install
cd /usr/src/redhat/BUILD/irenaeus
make install


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%doc /usr/src/redhat/BUILD/irenaeus/AUTHORS 
%doc /usr/src/redhat/BUILD/irenaeus/COPYING 
%doc /usr/src/redhat/BUILD/irenaeus/ChangeLog 
%doc /usr/src/redhat/BUILD/irenaeus/NEWS 
%doc /usr/src/redhat/BUILD/irenaeus/README 
 
/usr/src/redhat/BUILD/irenaeus/Makefile.am	
/usr/src/redhat/BUILD/irenaeus/TODO	      
/usr/src/redhat/BUILD/irenaeus/filters	 
/usr/src/redhat/BUILD/irenaeus/mkinstalldirs  
/usr/src/redhat/BUILD/irenaeus/irenaeus.spec    
/usr/src/redhat/BUILD/irenaeus/Makefile.in	
/usr/src/redhat/BUILD/irenaeus/aclocal.m4    
/usr/src/redhat/BUILD/irenaeus/install-sh  
/usr/src/redhat/BUILD/irenaeus/sword.el	 
/usr/src/redhat/BUILD/irenaeus/irenaeussword.cpp		
/usr/src/redhat/BUILD/irenaeus/configure     
/usr/src/redhat/BUILD/irenaeus/iomgr.cpp   
/usr/src/redhat/BUILD/irenaeus/irenaeus.cpp
/usr/src/redhat/BUILD/irenaeus/INSTALL    	
/usr/src/redhat/BUILD/irenaeus/configure.in  
/usr/src/redhat/BUILD/irenaeus/missing	  
/usr/src/redhat/BUILD/irenaeus/irenaeus.h
/usr/src/redhat/BUILD/irenaeus/test/test.sh
/usr/src/redhat/BUILD/irenaeus/test/testout.dat


/usr/local/bin/irenaeus


