pushd conan\Debug
conan install . -pr msvcprofiled --build missing --update
popd

pushd conan\Release
conan install . -pr msvcprofile --build missing --update
popd
