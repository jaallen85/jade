# jade
Jade is a cross-platform diagram editor based on the Qt framework.

# Build Instructions
To build zlib, download source code from https://www.zlib.net/.  Unzip the files to a folder such as C:\dev\zlib-1.3-src, then open a terminal and run the following commands:
```
$ vcvarsall.bat x64
$ cd C:\dev\zlib-1.3-src
$ cmake -S . -B ./build -D CMAKE_INSTALL_PREFIX=C:\dev\zlib
$ cmake --build ./build --config Debug --target install
$ cmake --build ./build --config Release --target install
```

To build quazip, download source code from https://github.com/stachenov/quazip.  Unzip the files to a folder such as C:\dev\quazip-1.4-src, then open a terminal and run the following commands:
```
$ vcvarsall.bat x64
$ cd C:\dev\quazip-1.4-src
$ cmake -S . -B ./build -D QUAZIP_QT_MAJOR_VERSION=6 -D CMAKE_PREFIX_PATH="C:\dev\qt\6.5.3\msvc2019_64;C:\dev\zlib" -D CMAKE_INSTALL_PREFIX=C:\dev\quazip
$ cmake --build ./build --config Debug --target install
$ cmake --build ./build --config Release --target install
```
