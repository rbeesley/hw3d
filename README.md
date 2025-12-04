# hw3d
Based on ChiliTomatoNoodle's [C++ 3D DirectX Programming tutorial](https://www.youtube.com/playlist?list=PLqCJpWy5Fohd3S7ICFXwUomYW0Wv67pDD). This project follow the same basic milestones, but this implementation adds additional capabilites and architecture not used by the source tutorial.

Checkout the source, including submodules:
`git clone --recurse-submodules -- git@github.com:rbeesley/hw3d.git`

## Solution structure
The solution has two projects, hw3d and hw3dw. The file naming convention loosely follows the binary names generated with an Unreal project.

### hw3d
This is a launcher for hw3dw. It is only built for debug builds and it provides a console logger. If executed from a terminal, this will do things like showing the framerate and CPU usage in the console titlebar, and it will restore things back to how they were before it was launched. If invoked from a Windows Explorer window, it will launch its own conhost.exe instance and then cleanup afterwards.

### hw3dw
This is the main application. This is probably the project you will want to open for authoring, running, and debugging. If the project detects that it is running from within Visual Studio, it will make certain changes which make it easier to debug.

#### Class Hierarchy
```
 -------
|  App  |
 ------- 
  | | |
  | |  \______________
  |  \______          \
  |         \          \
 ---------   --------   ---------------
| Console | | Window | | ImGUI Context |
 ---------   --------   ---------------
             | |  | |
             | |  |  \_____________
            /  |   \___            \
           /   |       \            \
 ----------   -------   ----------   -------------
| Graphics | | Mouse | | Keyboard | | ImGUI Win32 |
 ----------   -------   ----------   -------------
  |      \
 ------   ------------
| DX11 | | ImGUI DX11 |
 ------   ------------
```

#### `WinMain.cpp`
This is the main entry point. Its primary function is parsing the command-line to establish the pipeline with hw3d and then starting the App.

#### App Class (`AppConfig.hpp`, `App.hpp`, and `App.cpp`)
App is the main class responsible for managing the lifecycle of the rest of the framework. The first thing it does is to instantiate a Window class and set up the message pump. Because this is the top level class, it is also desgnated as the default message handler. If it is a debug build, this is also where the Console class is instatiated and wired for logging.

This is where you will also find the render loop.

#### Console Class (`Console.hpp` and `Console.cpp`)
For such a simple class Console has a lot of Win32 moving parts. It was inspired by UltraHLE. When UltraHLE was released, it turns out there was a lot of dormant code that was unreachable, but there was a patch released shortly afterwards which enabled a console window for logging and as a way to issue commands. In Windows, you can normally make an application which runs as a Window or you can have a Console, but being able to launch both was a facinating challenege. It has its own message handler, an event handler, and can process control events like CTRL-Break.

#### Window Class (`Window.hpp` and `Window.cpp`)
Window is next in the hierarchy. This class is used as the canvas for DX11. It is also responsible for setting up the I/O for the app, instantiating the Mouse and Keyboard objects which handle things like key mapping and mouse movement. Special attention was made for handling mouse movement when a button is clicked within the window and then the mouse moves outside the client area. The mouse has full support for all 5 buttons and the scroll wheel, both horizontal and vertical.

#### Graphics Class (`Graphics.hpp` and `Graphics.cpp`)
Graphics is used to map the application to a DX11 hardware implementation.

#### Mouse and Keyboard Classes (`Mouse.hpp`, `Mouse.cpp`, `Keyboard.hpp`, and `Keyboard.cpp`)
These classes add Win32 support for the mouse and keyboard. These classes listen to window messages and provide a way to react to mouse and keyboard inputs. ImGui can also hook into the mouse, keyboard, (and gamepad) messages, so this is architected in a way which allows for the use of both as appropriate.

#### Dear ImGui (App, Window, and Graphics Classes)
App is responsible for creating the ImGui Context, beginning the ImGui frame, ending the ImGui frame, and rendering the ImGui frame.

Window provides the ImGui Win32 access.

Graphics provides the ImGui DX11 access.

#### Configuration headers (`AppConfig.hpp`, `LoggingConfig.hpp`)
`AppConfig.hpp` is intended to be where global configurations can be set. Currently it has a debug #define for setting the framerate to 1 fps and is is also used to change Dear ImGui to use either the main branch logic or IMGUI_DOCKING. Dear ImGui is a submodule pinned to the IMGUI_DOCKING branch, and while considered experimental, it provides a way to create draggable/dockable windows which can be removed from the main window.

`LoggingConfig.hpp` is used to control log levels and can be used to enable or disable logging messages for major systems like graphics calls, window messages, mouse messages, and keyboard messages. These can be useful for debugging.

#### Message Handlers
App, Window, Graphics, Mouse, and Keyboard all have their own message handlers, but App coordinates and dispatches to the correct handler based on the message. This allows App to remain issolated from implementation details and subsystems are localized.

#### Special App messages
`WM_APP`, `WM_APP_CLOSE`, `WM_APP_CONSOLE_CLOSE`, and `WM_APP_WINDOW_CLOSE` are custom application messages which can be extended or used to control the application state. Currently `WM_APP_CONSOLE_CLOSE` and `WM_APP_CLOSE` are used when closing the app by different ways. WM_APP_CONSOLE_CLOSE is fired when you close the debug console. The console system close button and close menu item are disabled by design, but you can still send this even using the Windows 10/11 thumbnail preview close button. It is not enough to close the child `conhost.exe` process. This causes a Ctrol-Close event which is captured by the Console ctrlHandler and fires the `WM_APP_CONSOLE_CLOSE` message to close the rest of the application.

For another example of how this works in action, apply this patch to `imgui_demp.cpp` and you will be able to click on Dear ImGui Demo -> Menu -> Quit to exit. Because this is modifying `imgui_demp.cpp` as limited as possible, it is necessary to use thread messages and those need to be redispatched. It is for this reason there is currently a static pointer to the mainWindow handle.

imgui_demo.cpp.patch:
```cpp
diff --git "a/imgui_demo.cpp" "b/imgui_demo.cpp"
index 05b39a39f..c407e031e 100644
--- "a/imgui_demo.cpp"
+++ "b/imgui_demo.cpp"
@@ -114,6 +114,8 @@ Index of this file:
 #include <stdio.h>          // vsnprintf, sscanf, printf
 #include <stdlib.h>         // NULL, malloc, free, atoi
 #include <stdint.h>         // intptr_t
+
+#include "src/AtumWindows.hpp"
 #if !defined(_MSC_VER) || _MSC_VER >= 1800
 #include <inttypes.h>       // PRId64/PRIu64, not avail in some MinGW headers.
 #endif
@@ -8391,7 +8393,7 @@ static void ShowExampleMenuFile()
     }
     if (ImGui::MenuItem("Checked", NULL, true)) {}
     ImGui::Separator();
-    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
+    if (ImGui::MenuItem("Quit", "Alt+F4")) { ::PostMessage(nullptr, WM_APP_CLOSE, WPARAM(0), LPARAM(0)); }
 }
 
 //-----------------------------------------------------------------------------

```

## Build and debug problems

### Error missing file `dxgidebug.dll`
To run a debug build, install the optional feature `Graphics Tools`:
Open Settings using `Win+i`, search for `optional`, select `Manage Optional Features`, add `Graphics Tools`. Release builds should not have this dependency.

### Error missing file `PixelShader.cso`
To be able to run under debug, change:
`Project Property Pages -> Configuration Properties -> Debugging -> Working Directory` : `$(SolutionDir)bin\$(Platform)\$(Configuration)\`. Running the release or debug builds directly, resources should be accessible relativly from the working directory in their expected locations. This change is saved in `"[solution]\hw3d\hw3d.vcxproj.user"`, so it does not persist to version control.