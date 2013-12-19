move /y spot.hpp ..
deps\amalgamate -p spot.hpp -w "*.*pp;*.c;*.h" spot.c   ..\spotc.c
deps\amalgamate -p spot.hpp -w "*.*pp;*.c;*.h" spot.cpp ..\spot.cpp
deps\fart.exe -- ..\spotc.c  "#line" "//#line"
deps\fart.exe -- ..\spotc.c  "#pragma once" "//#pragma once"
deps\fart.exe -- ..\spot.cpp "#line" "//#line"
deps\fart.exe -- ..\spot.cpp "#pragma once" "//#pragma once"
copy /y ..\spot.hpp

if exist viewer.exe del viewer.exe
if exist viewer.exe exit /b

if exist readme.exe del readme.exe 
if exist readme.exe exit /b

if exist filt.exe del filt.exe 
if exist filt.exe exit /b

if exist transfer.exe del transfer.exe 
if exist transfer.exe exit /b

del ktx.exe
for %%i in (..\samples\ktx.cc) do cl /DUSE_OMP /Fe%%~ni.exe %%i -I .. ..\spot*.c* %* /Ox /Oy /DNDEBUG /MT /EHsc /nologo /link setargv.obj
ktx ..\images\etc1.ktx
ktx ..\images\test-dxt5.crn
ktx ..\images\fallingwater.jpg

for %%i in (..\samples\*.cc) do cl /Fe%%~ni.exe %%i -I .. ..\spot*.c* %* /Ox /Oy /DNDEBUG /MT /EHsc /nologo /link setargv.obj

if exist converter.exe if exist viewer.exe converter.exe webp -m ..\images\MipMap_Example_STS101[1].png -o .
if exist converter.exe if exist viewer.exe viewer.exe .\MipMap_Example_STS101[1].png.webp

if exist transfer.exe transfer.exe ..\images\panda.pug ..\images\lenna2.jpg
if exist transfer.exe transfer.exe ..\images\lenna2.jpg ..\images\5.webp

if exist readme.exe readme.exe ..\images\1.webp 

if exist filt.exe filt.exe ..\images\1.webp    --hsla
if exist filt.exe filt.exe ..\images\1.webp    --rgba
if exist filt.exe filt.exe ..\images\23.svg    --rgba --bleed
if exist filt.exe filt.exe ..\images\panda.png --hsla --bleed
if exist filt.exe filt.exe ..\images\panda.png --hsla --bleed --check

