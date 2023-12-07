Note: SCEngine is renamed to 'The Steel Game Engine' and rewritten in rust, see https://github.com/SSSxCCC/steel

# SCEngine

SC engine is an open source cross-platform C++ game engine with the following features:
* It is completely open source, and the engine layer code can be easily modified if there is a need for customization;
* With a visual editor, you can develop games efficiently;
* The game can be compiled into a Windows program or an Android application with one click
* Using modern C++ language, while ensuring code readability and game performance;
* Use widely used C++ open source libraries, such as box2d, glm, imgui, etc., to speed up the speed of getting started;
* Using vulkan, an advanced graphics API, can achieve any modern graphics effect.

## Build and run SCEngineEditor

Currently, it is recommended to run in VSCode in debugging mode, so that when a C++ crash occurs, the cause of the problem can be found immediately.  
1. Install Visual Studio 2022 and VSCode in win10/win11  
2. Install "Desktop development with C++" in Visual Studio 2022 and keep the default check  
3. Install [Cmake](https://cmake.org/download/), and check 'add cmake to path'  
4. Install [Vulkan SDK](https://vulkan.lunarg.com/)  
5. Download the code of this project and use VSCode to open the root directory of this project  
6. Press F5 to compile and run  
7. If asan is enabled, you need copy clang_rt.asan_dbg_dynamic-x86_64.dll to SCEngine\build\install\bin\
(dll example path: D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x64\clang_rt.asan_dbg_dynamic-x86_64.dll)

## Development Roadmap

- [x] Game core module (Engine, Scene, GameObject, Script)
- [x] Visual editor
- [x] Vulkan render pipeline
- [x] 2D rendering basic
- [ ] 2D texture
- [ ] 3D rendering basic
- [ ] 3D model
- [ ] Ray traced rendering
- [x] 2D physics
- [ ] 3D physics
- [x] Build Windows program
- [x] Build Android application
- [ ] Customize build
- [ ] Write a tutorial
- [ ] Tests

---

SC引擎是一个开源跨平台C++游戏引擎，主要有以下特性：
* 是完全开源的，如果有定制需要可以方便的修改引擎层代码；
* 具有可视化编辑器，可以高效的开发游戏；
* 制作的游戏可以一键编译成Windows程序或Android应用
* 使用现代C++语言，同时保证了代码可阅读性与游戏运行性能；
* 使用了被广泛使用的C++开源库，例如box2d，glm，imgui等，加快上手速度；
* 使用了vulkan这种先进的图形api，可以实现任何现代图形效果。

## 编译并运行SC引擎编辑器

目前推荐在VSCode中以调试的方式运行，这样当出现C++崩溃问题时可以立刻找到问题原因。  
1. 在win10/win11安装Visual Studio 2022和VSCode  
2. 在Visual Studio 2022中安装"Desktop development with C++"，保持默认勾选即可  
3. 安装[Cmake](https://cmake.org/download/)，安装时勾选“添加cmake到path”  
4. 安装[Vulkan SDK](https://vulkan.lunarg.com/)  
5. 下载本项目代码，使用VSCode打开本项目根目录  
6. 按F5即可编译运行  
7. 如果开启了asan，还需要复制clang_rt.asan_dbg_dynamic-x86_64.dll到SCEngine\build\install\bin\目录下
（dll示例位置：D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x64\clang_rt.asan_dbg_dynamic-x86_64.dll）

## 开发路线图

- [x] 游戏核心模块（引擎，场景，游戏物体，脚本）
- [x] 可视化编辑器
- [x] Vulkan渲染管线
- [x] 2D渲染基础
- [ ] 2D纹理
- [ ] 3D渲染基础
- [ ] 3D模型
- [ ] 光线追踪渲染
- [x] 2D物理
- [ ] 3D物理
- [x] 编译Windows程序
- [x] 编译Android应用
- [ ] 定制编译
- [ ] 写一个教程
- [ ] 测试
