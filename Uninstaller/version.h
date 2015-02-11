/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_QGNA_VERSION_H_
#define _GGS_QGNA_VERSION_H_

#ifndef HUDSON_BUILD
    #define FILEVER          1,0,0,0
    #define PRODUCTVER       FILEVER
    #define STRFILEVER       "1,0,0,0"
    #define STRPRODUCTVER    STRFILEVER

    #define COMPANYNAME      "GGS"
    #define FILEDESCRIPTION  "Developer version of GameNet Uninstaller"
    #define INTERNALNAME     "Uninstaller"
    #define LEGALCOPYRIGHT   "Copyright(c) 2015"
    #define ORIGINALFILENAME "Uninstaller.EXE"
    #define PRODUCTNAME      "GameNet Uninstaller"
#else
    #define FILEVER          $$MAJOR$$,$$MINOR$$,$$PATCH$$,$$BUILD$$
    #define PRODUCTVER       FILEVER
    #define STRFILEVER       "$$MAJOR$$,$$MINOR$$,$$PATCH$$,$$BUILD$$"
    #define STRPRODUCTVER    STRFILEVER

    #define COMPANYNAME      "$$COMPANYNAME$$"
    #define FILEDESCRIPTION  "$$FILEDESCRIPTION$$"
    #define INTERNALNAME     "$$INTERNALNAME$$"
    #define LEGALCOPYRIGHT   "$$LEGALCOPYRIGHT$$"
    #define ORIGINALFILENAME "$$FILENAME$$"
    #define PRODUCTNAME      "$$PRODUCTNAME$$"
#endif

#endif //_GGS_RESTAPI_VERSION_H_





