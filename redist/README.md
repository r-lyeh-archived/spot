spot/redist
===========

- This optional folder is used to regenerate the amalgamated distribution. Do not include it into your project.
- Regenerate the distribution by typing the following lines:
```bash
move /y spot.hpp ..
deps\amalgamate -p spot.hpp -w "*.*pp;*.c;*.h" spot.c   ..\spotc.c
deps\amalgamate -p spot.hpp -w "*.*pp;*.c;*.h" spot.cpp ..\spot.cpp
deps\fart.exe -- ..\spotc.c  "#line" "//#line"
deps\fart.exe -- ..\spotc.c  "#pragma once" "//#pragma once"
deps\fart.exe -- ..\spot.cpp "#line" "//#line"
deps\fart.exe -- ..\spot.cpp "#pragma once" "//#pragma once"
copy /y ..\spot.hpp
```

- If you want to regenerate the samples:
```bash
pushd ..\samples
cl converter.cc -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl readme.cc    -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl viewer.cc    -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl filt.cc      -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
cl transfer.cc  -I .. ..\spot*.c* /Ox /Oy /MT /DNDEBUG /link setargv.obj
popd
```
