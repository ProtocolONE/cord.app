conan install ./conan/conanfiled.txt -pr ./conan/msvcprofiled --build missing --update -if ./conan 
@rem conan install ./conan/conanfile.txt -pr ./conan/msvcprofile --build missing --update  -if ./conan

pushd conan\Release
conan install . -pr msvcprofile --build missing --update
popd
