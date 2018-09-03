pushd conan\Debug
conan install . -pr msvcprofiled --build missing
popd

pushd conan\Release
conan install . -pr msvcprofile --build missing
popd
