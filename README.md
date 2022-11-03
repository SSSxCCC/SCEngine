# SCEngine
### Build and run SCEngineEditor
first time build: ([SCEngine] is the SCEngine root path)
```
open terminal in windows
cd [SCEngine]
mkdir build
cd build
..\SCEngineEditor\compiler\cmake-3.25.0-rc1-windows-x86_64\bin\cmake.exe ../
..\SCEngineEditor\compiler\cmake-3.25.0-rc1-windows-x86_64\bin\cmake.exe --build . --config Debug -j8
..\SCEngineEditor\compiler\cmake-3.25.0-rc1-windows-x86_64\bin\cmake.exe --install . --config Debug --prefix install
```
build again:
```
cd [SCEngine]\build
..\SCEngineEditor\compiler\cmake-3.25.0-rc1-windows-x86_64\bin\cmake.exe --build . --config Debug -j8
..\SCEngineEditor\compiler\cmake-3.25.0-rc1-windows-x86_64\bin\cmake.exe --install . --config Debug --prefix install
```
run:
```
copy clang_rt.asan_dbg_dynamic-x86_64.dll to [SCEngine]\build\install\bin\
example path: D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x64\clang_rt.asan_dbg_dynamic-x86_64.dll
open [SCEngine]\build\install\bin\SCEngineEditor.exe
```
