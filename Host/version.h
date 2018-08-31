#pragma once

#ifndef HUDSON_BUILD
  #define FILEVER          1,0,0,0
  #define PRODUCTVER       FILEVER
  #define STRFILEVER       "1,0,0,0"
  #define STRPRODUCTVER    STRFILEVER

  #define COMPANYNAME      "ProtocolOne"
  #define FILEDESCRIPTION  "Developer version ProtocolOne Launcher Host application"
  #define INTERNALNAME     "launcher.host"
  #define LEGALCOPYRIGHT   "Copyright ( ) 2014"
  #define ORIGINALFILENAME "launcher.host.exe"
  #define PRODUCTNAME      "ProtocolOne Launcher Host application"
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
