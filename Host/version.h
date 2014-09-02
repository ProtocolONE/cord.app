/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_HOST_VERSION_H_
#define _GGS_HOST_VERSION_H_

#ifndef HUDSON_BUILD
  #define FILEVER          1,0,0,0
  #define PRODUCTVER       FILEVER
  #define STRFILEVER       "1,0,0,0"
  #define STRPRODUCTVER    STRFILEVER

  #define COMPANYNAME      "GGS"
  #define FILEDESCRIPTION  "Developer version GameNet Host application"
  #define INTERNALNAME     "HOST"
  #define LEGALCOPYRIGHT   "Copyright ( ) 2014"
  #define ORIGINALFILENAME "HOST.EXE"
  #define PRODUCTNAME      "GameNet host application"
#else
  #define FILEVER          $$MAJOR$$,$$MINOR$$,$$HUDSON_BUILD$$
  #define PRODUCTVER       FILEVER
  #define STRFILEVER       "$$MAJOR$$,$$MINOR$$,$$HUDSON_BUILD$$,$$GIT_REVISION$$"
  #define STRPRODUCTVER    STRFILEVER

  #define COMPANYNAME      "$$COMPANYNAME$$"
  #define FILEDESCRIPTION  "$$FILEDESCRIPTION$$"
  #define INTERNALNAME     "$$INTERNALNAME$$"
  #define LEGALCOPYRIGHT   "$$LEGALCOPYRIGHT$$"
  #define ORIGINALFILENAME "$$FILENAME$$"
  #define PRODUCTNAME      "$$PRODUCTNAME$$"
#endif

#endif //_GGS_HOST_VERSION_H_
