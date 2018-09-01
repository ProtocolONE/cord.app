pushd conan\Debug
conan install . -pr msvcprofiled
popd

pushd conan\Release
conan install . -pr msvcprofile
popd
