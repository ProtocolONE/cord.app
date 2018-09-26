@rem Should be installed this way to call `deploy` method. Deploy method register this qt version for visual studio add-in.
conan install Qt/5.5.1@common/stable -pr conan/msvcprofile

conan install ./conan/conanfiled.txt -pr ./conan/msvcprofiled --build missing --update -if ./conan 
conan install ./conan/conanfile.txt -pr ./conan/msvcprofile --build missing --update  -if ./conan
