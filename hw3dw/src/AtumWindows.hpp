#pragma once

#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

#if defined(STRICT)
#undef STRICT
#define STRICT
#endif

// Windows.h macro defines
// Inhibit the definition of all items inherited from Windows.h except for those commented out for specific classes

// GDIPlusManager.hpp, Surface.hpp
//#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS // - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
// VirtualKeyMap.hpp
//#define NOVIRTUALKEYCODES // - VK_ *
// Console.hpp, Keyboard.hpp, Mouse.hpp, Window.hpp
//#define NOWINMESSAGES // - WM_*, EM_*, LB_*, CB_*
// Surface.hpp, Window.hpp
//#define NOWINSTYLES // -  WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
// Window.hpp
//#define NOSYSMETRICS // - SM_ * // !NOSYSMETRICS Required for GetSystemMetrics() for icons
// Console.hpp
//#define NOMENUS // - MF_ * // !NOMENUS Required for GetSystemMenu
#define NOICONS // - IDI_ *
#define NOKEYSTATES // - MK_ *
// Console.hpp, Window.hpp
//#define NOSYSCOMMANDS // - SC_ * // !NOSYSCOMMANDS Required for EnableMenuItem
#define NORASTEROPS // - Binary and Tertiary raster ops
// Console.hpp, Window.hpp
//#define NOSHOWWINDOW // - SW_ *
#define OEMRESOURCE // - OEM Resource values
#define NOATOM // - Atom Manager routines
#define NOCLIPBOARD // - Clipboard routines
#define NOCOLOR // - Screen colors
// Console.hpp, GDIPlusManager.hpp, Keyboard.hpp, Mouse.hpp, Surface.hpp, VirtualKeyMap.hpp, Window.hpp
//#define NOCTLMGR // - Control and Dialog routines // !NOCTLMGR Required for SetWindowSubclass
#define NODRAWTEXT // - DrawText() and DT_ *
// GDIPlusManager.hpp, Surface.hpp, Window.hpp
//#define NOGDI // - All GDI defines and routines
#define NOKERNEL // - All KERNEL defines and routines
// Console.hpp, GDIPlusManager.hpp, Mouse.hpp, Surface.hpp, Window.hpp
//#define NOUSER // - All USER defines and routines
// Surface.hpp
//#define NONLS // - All NLS defines and routines // !NONLS Required for MultiByteToWideChar()
#define NOMB // - MB_ * and MessageBox()
#define NOMEMMGR // - GMEM_*, LMEM_*, GHND, LHND, associated routines
// GDIPlusManager.hpp, Surface.hpp
//#define NOMETAFILE // - typedef METAFILEPICT // !NOMETAFILE Required for GDI
#define NOMINMAX // - Macros min(a, b) and max(a, b)
// GDIPlusManager.hpp, Keyboard.hpp, Mouse.hpp, Surface.hpp, VirtualKeyMap.hpp, Window.hpp
//#define NOMSG // - typedef MSG and associated routines
#define NOOPENFILE // - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL // - SB_ * and scrolling routines
#define NOSERVICE // - All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND // - Sound driver routines
#define NOTEXTMETRIC // - typedef TEXTMETRIC and associated routines
#define NOWH // - SetWindowsHook and WH_ *
// Console.hpp, Window.hpp
//#define NOWINOFFSETS // - GWL_*, GCL_*, associated routines
#define NOCOMM // - COMM driver routines
#define NOKANJI // - Kanji support stuff.
#define NOHELP // - Help engine interface.
#define NOPROFILER // - Profiler interface.
#define NODEFERWINDOWPOS // - DeferWindowPos routines
#define NOMCX // - Modem Configuration Extensions
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#include <Windows.h>

#define WM_APP_CLOSE (WM_APP + 1)
#define WM_APP_CONSOLE_CLOSE (WM_APP + 2)
#define WM_APP_WINDOW_CLOSE (WM_APP + 3)

// CommCtrl
#if FALSE
// CommCtrl.h macro defines
#define NOTOOLBAR			// - Customizable bitmap-button toolbar control.
#define NOUPDOWN			// - Up and Down arrow increment/decrement control.
#define NOSTATUSBAR			// - Status bar control.
#define NOMENUHELP			// - APIs to help manage menus, especially with a status bar.
#define NOTRACKBAR			// - Customizable column-width tracking control.
#define NODRAGLIST			// - APIs to make a listbox source and sink drag&drop actions.
#define NOPROGRESS			// - Progress gas gauge.
#define NOHOTKEY			// - HotKey control
#define NOHEADER			// - Header bar control.
#define NOIMAGEAPIS			// - ImageList apis.
#define NOLISTVIEW			// - ListView control.
#define NOTREEVIEW			// - TreeView control.
#define NOTABCONTROL		// - Tab control.
#define NOANIMATE			// - Animate control.
#define NOBUTTON			// - Button control.
#define NOSTATIC			// - Static control.
#define NOEDIT				// - Edit control.
#define NOLISTBOX			// - Listbox control.
#define NOCOMBOBOX			// - Combobox control.
#define NOSCROLLBAR			// - Scrollbar control.
#define NOTASKDIALOG		// - Task Dialog.
#define NOMUI				// - MUI APIs.
#define NOTRACKMOUSEEVENT	// - TrackMouseEvent API.
#define NOFLATSBAPIS		// - Flat SB APIs. (Disable LPSCROLLINFO requirements)

#include <CommCtrl.h>
#pragma comment(lib,"comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
