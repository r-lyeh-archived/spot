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

set target=
set devel=/Ox /Oy /DNDEBUG /MT

rem set target=info
rem set devel=/Zi /Oy- /DDEBUG /MDd

if not "%target%"=="" (
for %%i in (..\samples\%target%.cc) do cl /DUSE_OMP /Fe%%~ni.exe %%i -I .. ..\spot*.c* %* %devel% /EHsc /nologo /link setargv.obj
exit/b
)

del info.exe
for %%i in (..\samples\info.cc) do cl /DUSE_OMP /Fe%%~ni.exe %%i -I .. ..\spot*.c* %* %devel% /EHsc /nologo /link setargv.obj
info ..\images\panda.pug
info ..\images\etc1.ktx
info ..\images\pvrtc-2bpp-rgba.pvr
info ..\images\test.pkm
info ..\images\female_hair002_df.pvr
info ..\images\fallingwater.jpg
info ..\images\test-dxt5.crn
rem exit/b

del viewer.exe
for %%i in (..\samples\viewer.cc) do cl /DUSE_OMP /Fe%%~ni.exe %%i -I .. ..\spot*.c* %* %devel% /EHsc /nologo /link setargv.obj
viewer ..\images\field_128_cube.dds
viewer ..\images\panda.pug
viewer ..\images\etc1.ktx
viewer ..\images\pvrtc-2bpp-rgba.pvr
viewer ..\images\test.pkm
viewer ..\images\female_hair002_df.pvr
viewer ..\images\fallingwater.jpg
viewer ..\images\test-dxt5.crn

for %%i in (..\samples\*.cc) do cl /Fe%%~ni.exe %%i -I .. ..\spot*.c* %* %devel% /EHsc /nologo /link setargv.obj

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
