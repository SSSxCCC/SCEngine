# SCEngine

## Build and run SCEngineEditor

1 Install Visual Studio 2022 and VSCode in win10/win11

2 Install "Desktop development with C++" in Visual Studio 2022 and keep the default check

3 Install [Cmake](https://cmake.org/download/), and check 'add cmake to path'

4 Install [Vulkan SDK](https://vulkan.lunarg.com/)

5 Download the code of this project and use VSCode to open the root directory of this project

6 Press F5 to compile and run

7 If asan is enabled, you need copy clang_rt.asan_dbg_dynamic-x86_64.dll to SCEngine\build\install\bin\
(dll example path: D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x64\clang_rt.asan_dbg_dynamic-x86_64.dll)

## 编译并运行SC引擎编辑器

1 在win10/win11安装Visual Studio 2022和VSCode

2 在Visual Studio 2022中安装"Desktop development with C++"，保持默认勾选即可

3 安装[Cmake](https://cmake.org/download/)，安装时勾选“添加cmake到path”

4 安装[Vulkan SDK](https://vulkan.lunarg.com/)

5 下载本项目代码，使用VSCode打开本项目根目录

6 按F5即可编译运行

7 如果开启了asan，还需要复制clang_rt.asan_dbg_dynamic-x86_64.dll到SCEngine\build\install\bin\目录下
（dll示例位置：D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x64\clang_rt.asan_dbg_dynamic-x86_64.dll）
