#pragma once
#define ATUMWINDOWS
//#define CALLTRACING

#define NTDDI_VERSION NTDDI_WIN10
#define _WIN32_WINNT _WIN32_WINNT_WIN10

// Windows.h macro defines
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS // - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#ifndef ATUMWINDOWS
#define NOVIRTUALKEYCODES // - VK_ *
#define NOWINMESSAGES // - WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES // -  WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#endif // ATUMWINDOWS
#define NOSYSMETRICS // - SM_ *
#ifndef ATUMWINDOWS // !NOMENUS Required for GetSystemMenu
#define NOMENUS // - MF_ *
#endif // ATUMWINDOWS
#define NOICONS // - IDI_ *
#ifndef ATUMWINDOWS
#define NOKEYSTATES // - MK_ *
#endif // ATUMWINDOWS
#ifndef ATUMWINDOWS // !NOSYSCOMMANDS Required for EnableMenuItem
#define NOSYSCOMMANDS // - SC_ *
#endif // ATUMWINDOWS
#define NORASTEROPS // - Binary and Tertiary raster ops
#ifndef ATUMWINDOWS
#define NOSHOWWINDOW // - SW_ *
#endif // ATUMWINDOWS
#define OEMRESOURCE // - OEM Resource values
#define NOATOM // - Atom Manager routines
#define NOCLIPBOARD // - Clipboard routines
#define NOCOLOR // - Screen colors
//#ifndef ATUMWINDOWS // !NOCTLMGR Required for SetWindowSubclass
#define NOCTLMGR // - Control and Dialog routines
//#endif // ATUMWINDOWS
#define NODRAWTEXT // - DrawText() and DT_ *
#ifndef ATUMWINDOWS
#define NOGDI // - All GDI defines and routines
#endif // ATUMWINDOWS
#define NOKERNEL // - All KERNEL defines and routines
#ifndef ATUMWINDOWS
#define NOUSER // - All USER defines and routines
#endif // ATUMWINDOWS
#define NONLS // - All NLS defines and routines
#ifndef ATUMWINDOWS
#define NOMB // - MB_ * and MessageBox()
#endif // ATUMWINDOWS
#define NOMEMMGR // - GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE // - typedef METAFILEPICT
#define NOMINMAX // - Macros min(a, b) and max(a, b)
#ifndef ATUMWINDOWS
#define NOMSG // - typedef MSG and associated routines
#endif // ATUMWINDOWS
#define NOOPENFILE // - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL // - SB_ * and scrolling routines
#define NOSERVICE // - All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND // - Sound driver routines
#define NOTEXTMETRIC // - typedef TEXTMETRIC and associated routines
#define NOWH // - SetWindowsHook and WH_ *
#ifndef ATUMWINDOWS
#define NOWINOFFSETS // - GWL_*, GCL_*, associated routines
#endif // ATUMWINDOWS
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

#define STRICT

#include <Windows.h>

//#include <WinUser.h>

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
#define NOFLATSBAPIS		// - Flat SB APIs. (Disable LPSCROLLINFO requrements)

//#include <CommCtrl.h>
//#pragma comment(lib,"comctl32.lib")
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
