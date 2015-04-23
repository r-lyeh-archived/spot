
move /y spot.hpp ..
deps\amalgamate -p spot.hpp -w "*.*pp;*.c;*.h" spot.c   ..\spotc.c
deps\amalgamate -p spot.hpp -w "*.*pp;*.c;*.h" spot.cpp ..\spot.cpp
deps\fart.exe -- ..\spotc.c  "#line" "//#line"
deps\fart.exe -- ..\spotc.c  "#pragma once" "//#pragma once"
deps\fart.exe -- ..\spot.cpp "#line" "//#line"
deps\fart.exe -- ..\spot.cpp "#pragma once" "//#pragma once"
copy /y ..\spot.hpp

pushd ..\samples
cl converter.cc -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl filt.cc      -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl info.cc      -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl readme.cc    -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl transfer.cc  -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl viewer.cc    -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
popd

