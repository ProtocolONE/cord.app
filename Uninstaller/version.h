#pragma once

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

