# Cord.App
![License](https://img.shields.io/hexpm/l/plug.svg)

## Description
The goal of this project is to give you a desktop application that you can download and run games.

## Getting started
To start using this project follow this [guide](https://github.com/ProtocolONE/cord.app/wiki). 

## Downloads
You can download releases from [github.com](https://github.com/ProtocolONE/cord.app/releases) or build it.

### Build Tools ###
 * MSVC 2013-2017 runtime 120_xp
 * conan.io (>= 1.7.2)
 * .NET 3.5

### Build ###
First install [conan.io](https://conan.io/downloads.html)

Clone application
```sh
git clone https://github.com/ProtocolONE/cord.app.git
```

Init submodules
```sh
git submodule init
git submodule update
```
    
Install dependencies
```sh
conan install Qt/5.5.1@common/stable -pr conan/msvcprofile

conan install ./conan/conanfiled.txt -pr ./conan/msvcprofiled --build missing --update -if ./conan 
conan install ./conan/conanfile.txt -pr ./conan/msvcprofile --build missing --update  -if ./conan
```
    
Build by MSVC Launcher.sln
    
    