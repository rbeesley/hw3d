#include "VirtualKeyMap.hpp"

#include <format>
#include <iomanip>
#include <sstream>
#include <string>

VirtualKeyMap::VirtualKeyMap()
	:
	map_({
	// VK_*s defined in WinUser.h
	{ 0x1, "VK_LBUTTON" },
	{ 0x2, "VK_RBUTTON" },
	{ 0x3, "VK_CANCEL" },
	{ 0x4, "VK_MBUTTON" },

	{ 0x5, "VK_XBUTTON1" },
	{ 0x6, "VK_XBUTTON2" },

	{ 0x7, "[reserved]" },

	{ 0x8, "VK_BACK" },
	{ 0x9, "VK_TAB" },

	{ 0xA, "[reserved]" },
	{ 0xB, "[reserved]" },

	{ 0xC, "VK_CLEAR" },
	{ 0xD, "VK_RETURN" },

	{ 0xE, "[unassigned]" },
	{ 0xF, "[unassigned]" },

	{ 0x10, "VK_SHIFT" },
	{ 0x11, "VK_CONTROL" },
	{ 0x12, "VK_MENU" },
	{ 0x13, "VK_PAUSE" },
	{ 0x14, "VK_CAPITAL" },

	{ 0x15, "VK_KANA" },
	{ 0x15, "VK_HANGEUL" }, // Old name
	{ 0x15, "VK_HANGUL" },
	{ 0x16, "VK_IME_ON" },
	{ 0x17, "VK_JUNJA" },
	{ 0x18, "VK_FINAL" },
	{ 0x19, "VK_HANJA" },
	{ 0x19, "VK_KANJI" },
	{ 0x1A, "VK_IME_OFF" },

	{ 0x1B, "VK_ESCAPE" },

	{ 0x1C, "VK_CONVERT" },
	{ 0x1D, "VK_NONCONVERT" },
	{ 0x1E, "VK_ACCEPT" },
	{ 0x1F, "VK_MODECHANGE" },

	{ 0x20, "VK_SPACE" },
	{ 0x21, "VK_PRIOR" },
	{ 0x22, "VK_NEXT" },
	{ 0x23, "VK_END" },
	{ 0x24, "VK_HOME" },
	{ 0x25, "VK_LEFT" },
	{ 0x26, "VK_UP" },
	{ 0x27, "VK_RIGHT" },
	{ 0x28, "VK_DOWN" },
	{ 0x29, "VK_SELECT" },
	{ 0x2A, "VK_PRINT" },
	{ 0x2B, "VK_EXECUTE" },
	{ 0x2C, "VK_SNAPSHOT" },
	{ 0x2D, "VK_INSERT" },
	{ 0x2E, "VK_DELETE" },
	{ 0x2F, "VK_HELP" },

	{ 0x30, "0" },
	{ 0x31, "1" },
	{ 0x32, "2" },
	{ 0x33, "3" },
	{ 0x34, "4" },
	{ 0x35, "5" },
	{ 0x36, "6" },
	{ 0x37, "7" },
	{ 0x38, "8" },
	{ 0x39, "9" },

	{ 0x3A, "[unassigned]" },
	{ 0x3B, "[unassigned]" },
	{ 0x3C, "[unassigned]" },
	{ 0x3D, "[unassigned]" },
	{ 0x3E, "[unassigned]" },
	{ 0x3F, "[unassigned]" },
	{ 0x40, "[unassigned]" },

	{ 0x41, "A" },
	{ 0x42, "B" },
	{ 0x43, "C" },
	{ 0x44, "D" },
	{ 0x45, "E" },
	{ 0x46, "F" },
	{ 0x47, "G" },
	{ 0x48, "H" },
	{ 0x49, "I" },
	{ 0x4A, "J" },
	{ 0x4B, "K" },
	{ 0x4C, "L" },
	{ 0x4D, "M" },
	{ 0x4E, "N" },
	{ 0x4F, "O" },
	{ 0x50, "P" },
	{ 0x51, "Q" },
	{ 0x52, "R" },
	{ 0x53, "S" },
	{ 0x54, "T" },
	{ 0x55, "U" },
	{ 0x56, "V" },
	{ 0x57, "W" },
	{ 0x58, "X" },
	{ 0x59, "Y" },
	{ 0x5A, "Z" },

	{ 0x5B, "VK_LWIN" },
	{ 0x5C, "VK_RWIN" },
	{ 0x5D, "VK_APPS" },

	{ 0x5E, "[reserved]" },

	{ 0x5F, "VK_SLEEP" },

	{ 0x60, "VK_NUMPAD0" },
	{ 0x61, "VK_NUMPAD1" },
	{ 0x62, "VK_NUMPAD2" },
	{ 0x63, "VK_NUMPAD3" },
	{ 0x64, "VK_NUMPAD4" },
	{ 0x65, "VK_NUMPAD5" },
	{ 0x66, "VK_NUMPAD6" },
	{ 0x67, "VK_NUMPAD7" },
	{ 0x68, "VK_NUMPAD8" },
	{ 0x69, "VK_NUMPAD9" },
	{ 0x6A, "VK_MULTIPLY" },
	{ 0x6B, "VK_ADD" },
	{ 0x6C, "VK_SEPARATOR" },
	{ 0x6D, "VK_SUBTRACT" },
	{ 0x6E, "VK_DECIMAL" },
	{ 0x6F, "VK_DIVIDE" },
	{ 0x70, "VK_F1" },
	{ 0x71, "VK_F2" },
	{ 0x72, "VK_F3" },
	{ 0x73, "VK_F4" },
	{ 0x74, "VK_F5" },
	{ 0x75, "VK_F6" },
	{ 0x76, "VK_F7" },
	{ 0x77, "VK_F8" },
	{ 0x78, "VK_F9" },
	{ 0x79, "VK_F10" },
	{ 0x7A, "VK_F11" },
	{ 0x7B, "VK_F12" },
	{ 0x7C, "VK_F13" },
	{ 0x7D, "VK_F14" },
	{ 0x7E, "VK_F15" },
	{ 0x7F, "VK_F16" },
	{ 0x80, "VK_F17" },
	{ 0x81, "VK_F18" },
	{ 0x82, "VK_F19" },
	{ 0x83, "VK_F20" },
	{ 0x84, "VK_F21" },
	{ 0x85, "VK_F22" },
	{ 0x86, "VK_F23" },
	{ 0x87, "VK_F24" },

	// [reserved]
	{ 0x88, "VK_NAVIGATION_VIEW" },
	{ 0x89, "VK_NAVIGATION_MENU" },
	{ 0x8A, "VK_NAVIGATION_UP" },
	{ 0x8B, "VK_NAVIGATION_DOWN" },
	{ 0x8C, "VK_NAVIGATION_LEFT" },
	{ 0x8D, "VK_NAVIGATION_RIGHT" },
	{ 0x8E, "VK_NAVIGATION_ACCEPT" },
	{ 0x8F, "VK_NAVIGATION_CANCEL" },

	{ 0x90, "VK_NUMLOCK" },
	{ 0x91, "VK_SCROLL" },

	// [oem_specific]
	{ 0x92, "VK_OEM_NEC_EQUAL" },
	{ 0x92, "VK_OEM_FJ_JISHO" },
	{ 0x93, "VK_OEM_FJ_MASSHOU" },
	{ 0x94, "VK_OEM_FJ_TOUROKU" },
	{ 0x95, "VK_OEM_FJ_LOYA" },
	{ 0x96, "VK_OEM_FJ_ROYA" },

	{ 0x97, "[unassigned]" },
	{ 0x98, "[unassigned]" },
	{ 0x99, "[unassigned]" },
	{ 0x9A, "[unassigned]" },
	{ 0x9B, "[unassigned]" },
	{ 0x9C, "[unassigned]" },
	{ 0x9D, "[unassigned]" },
	{ 0x9E, "[unassigned]" },
	{ 0x9F, "[unassigned]" },

	// * Used only as parameters to GetAsyncKeyState() and GetKeyState().
	// * No other API or message will distinguish left and right keys in this way.
	{ 0xA0, "VK_LSHIFT" },
	{ 0xA1, "VK_RSHIFT" },
	{ 0xA2, "VK_LCONTROL" },
	{ 0xA3, "VK_RCONTROL" },
	{ 0xA4, "VK_LMENU" },
	{ 0xA5, "VK_RMENU" },

	{ 0xA6, "VK_BROWSER_BACK" },
	{ 0xA7, "VK_BROWSER_FORWARD" },
	{ 0xA8, "VK_BROWSER_REFRESH" },
	{ 0xA9, "VK_BROWSER_STOP" },
	{ 0xAA, "VK_BROWSER_SEARCH" },
	{ 0xAB, "VK_BROWSER_FAVORITES" },
	{ 0xAC, "VK_BROWSER_HOME" },
	{ 0xAD, "VK_VOLUME_MUTE" },
	{ 0xAE, "VK_VOLUME_DOWN" },
	{ 0xAF, "VK_VOLUME_UP" },
	{ 0xB0, "VK_MEDIA_NEXT_TRACK" },
	{ 0xB1, "VK_MEDIA_PREV_TRACK" },
	{ 0xB2, "VK_MEDIA_STOP" },
	{ 0xB3, "VK_MEDIA_PLAY_PAUSE" },
	{ 0xB4, "VK_LAUNCH_MAIL" },
	{ 0xB5, "VK_LAUNCH_MEDIA_SELECT" },
	{ 0xB6, "VK_LAUNCH_APP1" },
	{ 0xB7, "VK_LAUNCH_APP2" },

	{ 0xB8, "[reserved]" },
	{ 0xB9, "[reserved]" },

	// ';:' for US
	{ 0xBA, "VK_OEM_1" },
	// '+' any country
	{ 0xBB, "VK_OEM_PLUS" },
	// ',' any country
	{ 0xBC, "VK_OEM_COMMA" },
	// '-' any country
	{ 0xBD, "VK_OEM_MINUS" },
	// '.' any country
	{ 0xBE, "VK_OEM_PERIOD" },
	// '/?' for US
	{ 0xBF, "VK_OEM_2" },
	// '`~' for US
	{ 0xC0, "VK_OEM_3" },

	{ 0xC1, "[reserved]" },
	{ 0xC2, "[reserved]" },

	// [reserved]
	{ 0xC3, "VK_GAMEPAD_A" },
	{ 0xC4, "VK_GAMEPAD_B" },
	{ 0xC5, "VK_GAMEPAD_X" },
	{ 0xC6, "VK_GAMEPAD_Y" },
	{ 0xC7, "VK_GAMEPAD_RIGHT_SHOULDER" },
	{ 0xC8, "VK_GAMEPAD_LEFT_SHOULDER" },
	{ 0xC9, "VK_GAMEPAD_LEFT_TRIGGER" },
	{ 0xCA, "VK_GAMEPAD_RIGHT_TRIGGER" },
	{ 0xCB, "VK_GAMEPAD_DPAD_UP" },
	{ 0xCC, "VK_GAMEPAD_DPAD_DOWN" },
	{ 0xCD, "VK_GAMEPAD_DPAD_LEFT" },
	{ 0xCE, "VK_GAMEPAD_DPAD_RIGHT" },
	{ 0xCF, "VK_GAMEPAD_MENU" },
	{ 0xD0, "VK_GAMEPAD_VIEW" },
	{ 0xD1, "VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON" },
	{ 0xD2, "VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON" },
	{ 0xD3, "VK_GAMEPAD_LEFT_THUMBSTICK_UP" },
	{ 0xD4, "VK_GAMEPAD_LEFT_THUMBSTICK_DOWN" },
	{ 0xD5, "VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT" },
	{ 0xD6, "VK_GAMEPAD_LEFT_THUMBSTICK_LEFT" },
	{ 0xD7, "VK_GAMEPAD_RIGHT_THUMBSTICK_UP" },
	{ 0xD8, "VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN" },
	{ 0xD9, "VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT" },
	{ 0xDA, "VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT" },

	// '[{' for US
	{ 0xDB, "VK_OEM_4" },
	// '\|' for US
	{ 0xDC, "VK_OEM_5" },
	// ']}' for US
	{ 0xDD, "VK_OEM_6" },
	// ''"' for US
	{ 0xDE, "VK_OEM_7" },
	{ 0xDF, "VK_OEM_8" },

	{ 0xE0, "[reserved]" },

	// 'AX' key on Japanese AX kbd
	{ 0xE1, "VK_OEM_AX" },
	// "<>" or "\|" on RT 102-key kbd.
	{ 0xE2, "VK_OEM_102" },
	// Help key on ICO
	{ 0xE3, "VK_ICO_HELP" },
	// 00 key on ICO
	{ 0xE4, "VK_ICO_00" },

	{ 0xE5, "VK_PROCESSKEY" },

	{ 0xE6, "VK_ICO_CLEAR" },

	{ 0xE7, "VK_PACKET" },

	{ 0xE8, "[unassigned]" },

	// Nokia/Ericsson OEM definitions
	{ 0xE9, "VK_OEM_RESET" },
	{ 0xEA, "VK_OEM_JUMP" },
	{ 0xEB, "VK_OEM_PA1 " },
	{ 0xEC, "VK_OEM_PA2" },
	{ 0xED, "VK_OEM_PA3" },
	{ 0xEE, "VK_OEM_WSCTRL" },
	{ 0xEF, "VK_OEM_CUSEL" },
	{ 0xF0, "VK_OEM_ATTN" },
	{ 0xF1, "VK_OEM_FINISH" },
	{ 0xF2, "VK_OEM_COPY" },
	{ 0xF3, "VK_OEM_AUTO" },
	{ 0xF4, "VK_OEM_ENLW" },
	{ 0xF5, "VK_OEM_BACKTAB" },

	{ 0xF6, "VK_ATTN" },
	{ 0xF7, "VK_CRSEL" },
	{ 0xF8, "VK_EXSEL" },
	{ 0xF9, "VK_EREOF" },
	{ 0xFA, "VK_PLAY" },
	{ 0xFB, "VK_ZOOM" },
	{ 0xFC, "VK_NONAME" },
	{ 0xFD, "VK_PA1" },
	{ 0xFE, "VK_OEM_CLEAR" },

	{ 0xFF, "[reserved]" },
		})
{}

std::string VirtualKeyMap::operator()(const unsigned char virtualKeyCode) const
{
	const auto it = map_.find(virtualKeyCode);

	std::ostringstream out;
	if (it != map_.end())
	{
		out << it->second;
	}
	else
	{
		out << " Unknown message: 0x"
			<< std::setw(8) << std::right << std::setfill('0') << std::hex << virtualKeyCode;
	}

	return out.str();
}

/**
 * This is an alternative to using the map, using Windows built in method for identifying the VK_*.
 * GetKeyNameText() doesn't seem to handle some virtual keys like VK_BROWSER_SEARCH without error.
 */
 //std::string VirtualKeyMap::operator()(const unsigned char virtual_key_code) const
 //{
 //	UINT scan_code = MapVirtualKey(virtual_key_code, MAPVK_VK_TO_VSC);
 //
 //	CHAR name[128];
 //	int result = 0;
 //	switch (virtual_key_code)
 //	{
 //	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
 //	case VK_RCONTROL: case VK_RMENU:
 //	case VK_LWIN: case VK_RWIN: case VK_APPS:
 //	case VK_PRIOR: case VK_NEXT:
 //	case VK_END: case VK_HOME:
 //	case VK_INSERT: case VK_DELETE:
 //	case VK_DIVIDE:
 //	case VK_NUMLOCK:
 //		scan_code |= KF_EXTENDED;
 //		[[fallthrough]];
 //	default:
 //		result = GetKeyNameTextA(static_cast<LONG>(scan_code << 16), name, 128);
 //	}
 //
 //	if (result == 0)
 //		throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()),
 //			"WinAPI Error occured.");
 //	return name;
 //}
