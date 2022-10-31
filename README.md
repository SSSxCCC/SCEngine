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
open [SCEngine]\build\install\bin\SCEngineEditor.exe
```
