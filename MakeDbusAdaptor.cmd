@set cpp2xml="D:\Qt\521b\bin\qdbuscpp2xml.exe"
@set xml2cpp="D:\Qt\521b\bin\qdbusxml2cpp.exe"

@set Target=%1
@set ClassName=%~n1%

@set AdaptorName=%ClassName%Adaptor
@set ProxyName=%ClassName%Proxy
@set Interface=%ClassName%.interface.xml

@echo Class:     %ClassName%

@echo Adaptor:   %AdaptorName%
@echo Proxy:     %ProxyName%
@echo Interface: %Interface%

@%cpp2xml% -A -o %Interface% "%Target%"

echo Fix interface and press any key.
pause

%xml2cpp% -v -a %AdaptorName% -c %AdaptorName% %Interface%
%xml2cpp% -v -p %ProxyName% -c %ProxyName% %Interface%
