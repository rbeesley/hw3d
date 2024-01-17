#pragma once

//
// NTDDI version constants
//
#define TARGET_WIN7					0x06010000
#define TARGET_WIN8                 0x06020000
#define TARGET_WINBLUE              0x06030000
#define TARGET_WINTHRESHOLD         0x0A000000  /* ABRACADABRA_THRESHOLD */
#define TARGET_WIN10                0x0A000000  /* ABRACADABRA_THRESHOLD */
#define TARGET_WIN10_TH2            0x0A000001  /* ABRACADABRA_WIN10_TH2 */
#define TARGET_WIN10_RS1            0x0A000002  /* ABRACADABRA_WIN10_RS1 */
#define TARGET_WIN10_RS2            0x0A000003  /* ABRACADABRA_WIN10_RS2 */
#define TARGET_WIN10_RS3            0x0A000004  /* ABRACADABRA_WIN10_RS3 */
#define TARGET_WIN10_RS4            0x0A000005  /* ABRACADABRA_WIN10_RS4 */
#define TARGET_WIN10_RS5            0x0A000006  /* ABRACADABRA_WIN10_RS5 */
#define TARGET_WIN10_19H1           0x0A000007  /* ABRACADABRA_WIN10_19H1*/

#define WINDOWS_TARGET_VERSION      TARGET_WIN10 /* ABRACADABRA_THRESHOLD */

//
// masks for version macros
//
#define OSVERSION_MASK      0xFFFF0000
#define SPVERSION_MASK      0x0000FF00
#define SUBVERSION_MASK     0x000000FF

//
// macros to extract various version fields from the NTDDI version
//
#define OSVERSION(Version)  (((Version) & OSVERSION_MASK) >> 16)
#define SPVERSION(Version)  ((((Version) & SPVERSION_MASK) << 8) >> 16)
#define SUBVERION(Version)	((((Version) & SUBVERSION_MASK) << 16) >> 16)

#define WINVER OSVERSION(WINDOWS_TARGET_VERSION)
#define _WIN32_WINNT OSVERSION(WINDOWS_TARGET_VERSION)
//#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define STRICT

#include <Windows.h>