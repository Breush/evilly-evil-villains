# Evilly Evil Villains

## Notice

All documents within this repository are written in English for an unknown reason, please follow this rule if you modify anything. The [wiki](https://github.com/Breush/evilly-evil-villains/wiki), which is containing the development guide [DevG], is written in French for convenience. This wiki is surely an important resource for developers.

## Dependencies

You'll need C++11-compliant compiler. On Windows, getting g++-4.8 through [MinGW](http://www.mingw.org/) is a way better deal than any Visual C++. On Linux, g++-4.8+ or clang-3.4+ will work as expected. 

### Integrated libraries

The program is based on [SFML](https://github.com/SFML/SFML). One objective is to provide the game on Steam portal, which, under Linux, runs the game in a small virtual machine. If the virtual machine does not contain a package, you have to provide it when shipping. And, it does not contain the SFML packages, that is why the source code of SFML in incorporated in order to recompile the library (and slightly improve it).

The integrated library (header-only) [pugixml](https://github.com/zeux/pugixml) is a light-weight C++11-friendly tool to read/write XML files. It has no dependencies.

[Selene](https://github.com/jeremyong/Selene) is a C++11 wrapper around Lua interface.

The Steamworks API has been integrated too. It provides access too Steam interfacing for Achievements, Network over Steam friends, and other things.

[Box2D](https://github.com/erincatto/Box2D) is a C++ library providing physics and collisions for 2D games.

## Linux

You need development packages. The following list is for debian-based systems:
```
sudo apt-get install libfreetype6 libjpeg8 libopenal1 libvorbisenc2 libvorbisfile3
sudo apt-get install libfreetype6-dev libjpeg-dev libx11-dev libxrandr-dev libxcb1-dev libx11-xcb-dev libxcb-randr0-dev libxcb-image0-dev libflac-dev libogg-dev libvorbis-dev libopenal-dev libpthread-stubs0-dev
sudo apt-get install libgl1-mesa-dev
sudo apt-get install liblua5.2-0 liblua5.2-dev
sudo apt-get install gettext
```

### Windows

SFML comes with all pre-compiled libraries it needs.

- *OpenGL* should already be installed.
- *Lua 5.2+* just get compiled libraries [there](http://sourceforge.net/projects/luabinaries/files/5.2.3/Windows%20Libraries/Dynamic/lua-5.2.3_Win32_dll12_lib.zip/download)
- *gettext* (libintl.h) you can get [there](http://gnuwin32.sourceforge.net/packages/gettext.htm)
