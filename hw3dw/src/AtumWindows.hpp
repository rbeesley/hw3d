#pragma once

#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

#if defined(STRICT)
#undef STRICT
#define STRICT
#endif

// Windows.h macro defines
// Inhibit the definition of all items inherited from Windows.h.
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS // - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // - VK_ *
#define NOWINMESSAGES // - WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES // -  WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS // - SM_ * // !NOSYSMETRICS Required for GetSystemMetrics() for icons
#define NOMENUS // - MF_ * // !NOMENUS Required for GetSystemMenu
#define NOICONS // - IDI_ *
#define NOKEYSTATES // - MK_ *
#define NOSYSCOMMANDS // - SC_ * // !NOSYSCOMMANDS Required for EnableMenuItem
#define NORASTEROPS // - Binary and Tertiary raster ops
#define NOSHOWWINDOW // - SW_ *
#define OEMRESOURCE // - OEM Resource values
#define NOATOM // - Atom Manager routines
#define NOCLIPBOARD // - Clipboard routines
#define NOCOLOR // - Screen colors
#define NOCTLMGR // - Control and Dialog routines // !NOCTLMGR Required for SetWindowSubclass
#define NODRAWTEXT // - DrawText() and DT_ *
#define NOGDI // - All GDI defines and routines
#define NOKERNEL // - All KERNEL defines and routines
#define NOUSER // - All USER defines and routines
#define NONLS // - All NLS defines and routines // !NONLS Required for MultiByteToWideChar()
#define NOMB // - MB_ * and MessageBox()
#define NOMEMMGR // - GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE // - typedef METAFILEPICT // !NOMETAFILE Required for GDI
#define NOMINMAX // - Macros min(a, b) and max(a, b)
#define NOMSG // - typedef MSG and associated routines
#define NOOPENFILE // - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL // - SB_ * and scrolling routines
#define NOSERVICE // - All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND // - Sound driver routines
#define NOTEXTMETRIC // - typedef TEXTMETRIC and associated routines
#define NOWH // - SetWindowsHook and WH_ *
#define NOWINOFFSETS // - GWL_*, GCL_*, associated routines
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

// When needed for certain classes, reenable only what is necessary.
#if defined(CONSOLE_)
#if defined(NOWINMESSAGES)
#undef NOWINMESSAGES
#endif
#if defined(NOMENUS)
#undef NOMENUS
#endif
#if defined(NOSYSCOMMANDS)
#undef NOSYSCOMMANDS
#endif
#if defined(NOSHOWWINDOW)
#undef NOSHOWWINDOW
#endif
#if defined(NOCTLMGR)
#undef NOCTLMGR
#endif
#if defined(NOUSER)
#undef NOUSER
#endif
#if defined(NOWINOFFSETS)
#undef NOWINOFFSETS
#endif
#endif

#if defined(CPU_METRIC_)
#endif

#if defined(FPS_METRIC_)
#endif

#if defined(GDI_PLUS_MANAGER_)
#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif
#if defined(NOMETAFILE)
#undef NOMETAFILE
#endif
#if defined(NOCTLMGR)
#undef NOCTLMGR
#endif
#if defined(NOGDI)
#undef NOGDI
#endif
#if defined(NOUSER)
#undef NOUSER
#endif
#if defined(NOMSG)
#undef NOMSG
#endif
#endif

#if defined(KEYBOARD_)
#if defined(NOCTLMGR)
#undef NOCTLMGR
#endif
#if defined(NOMSG)
#undef NOMSG
#endif
#endif

#if defined(MOUSE_)
#if defined(NOUSER)
#undef NOUSER
#endif
#if defined(NOWINMESSAGES)
#undef NOWINMESSAGES
#endif
#if defined(NOCTLMGR)
#undef NOCTLMGR
#endif
#if defined(NOMSG)
#undef NOMSG
#endif
#endif

#if defined(SURFACE_)
#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif
#if defined(NOMETAFILE)
#undef NOMETAFILE
#endif
#if defined(NOWINSTYLES)
#undef NOWINSTYLES
#endif
#if defined(NOCTLMGR)
#undef NOCTLMGR
#endif
#if defined(NOGDI)
#undef NOGDI
#endif
#if defined(NOUSER)
#undef NOUSER
#endif
#if defined(NONLS)
#undef NONLS
#endif
#if defined(NOMSG)
#undef NOMSG
#endif
#endif

#if defined(VIRTUAL_KEY_MAP_)
#if defined(NOVIRTUALKEYCODES)
#undef NOVIRTUALKEYCODES
#endif
#if defined(NOCTLMGR)
#undef NOCTLMGR
#endif
#if defined(NOMSG)
#undef NOMSG
#endif
#endif

#if defined(WINDOW_)
#if defined(NOWINMESSAGES)
#undef NOWINMESSAGES
#endif
#if defined(NOWINSTYLES)
#undef NOWINSTYLES
#endif
#if defined(NOSYSMETRICS)
#undef NOSYSMETRICS
#endif
#if defined(NOSYSCOMMANDS)
#undef NOSYSCOMMANDS
#endif
#if defined(NOSHOWWINDOW)
#undef NOSHOWWINDOW
#endif
#if defined(NOCTLMGR)
#undef NOCTLMGR
#endif
#if defined(NOGDI)
#undef NOGDI
#endif
#if defined(NOUSER)
#undef NOUSER
#endif
#if defined(NOMSG)
#undef NOMSG
#endif
#if defined(NOWINOFFSETS)
#undef NOWINOFFSETS
#endif
#endif

#if defined(WINDOWS_MESSAGE_MAP_)
#endif

#include <Windows.h>

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
