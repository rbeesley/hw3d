#pragma once
#include <AtumWindows.h> // for UTF8 conversion functions and constants
#include <exception>
#include <string>
#include <vector>

class wexception_base
{
public:
    static std::wstring wwhat(std::exception const& e)
    {
        return FromUtf8OrAnsi(e.what());
    }
    static bool IsUtf8Bom(char const* pmsg)
    {
        if (pmsg == nullptr)
            return false;
        for (size_t i = 0; i < sizeof(m_Utf8Bom); ++i)
            if (pmsg[i] != m_Utf8Bom[i])
                return false;
        return true;
    }
    static std::wstring FromUtf8OrAnsi(char const* pmsg)
    {
        if (pmsg == nullptr)
            return {};
        size_t len = strlen(pmsg);
        bool const isUtf = IsUtf8Bom(pmsg);
        if (isUtf)
        {
            pmsg += sizeof(m_Utf8Bom);
            len -= sizeof(m_Utf8Bom);
            if (len == 0)
                return {}; // Quick exit for empty message
        }
        int const cp = isUtf ? CP_UTF8 : CP_ACP;
        size_t required = MultiByteToWideChar(cp, 0, pmsg, -1, nullptr, 0);
        _ASSERT(required > 0);
        _ASSERT(required <= INT_MAX);
        if (required > INT_MAX - 16) // why 16? probably 2 would be enough
            required = INT_MAX - 16;
        std::vector<wchar_t> buffer(required + 1);
        auto const used = MultiByteToWideChar(cp, 0, pmsg, -1, buffer.data(), static_cast<int>(buffer.size()));
        _ASSERT(used > 0);
        return buffer.data();
    }
    static std::string ToUtf8(wchar_t const* pmsg)
    {
        size_t required = WideCharToMultiByte(CP_UTF8, 0, pmsg, -1, nullptr, 0, nullptr, nullptr);
        _ASSERT(required > 0);
        _ASSERT(required <= INT_MAX);
        if (required >= INT_MAX)
            return {};
        std::vector<char> buffer(required + sizeof(m_Utf8Bom) + 1);
        memcpy_s(buffer.data(), required + 3, m_Utf8Bom, sizeof(m_Utf8Bom));
        auto const used = WideCharToMultiByte(CP_UTF8, 0, pmsg, -1, buffer.data() + sizeof(m_Utf8Bom), static_cast<int>(buffer.size()), nullptr, nullptr);
        _ASSERT(used > 0);
        return buffer.data();
    }
    static char m_Utf8Bom[3];
}; // class wexception_base
/*static*/ __declspec(selectany) char wexception_base::m_Utf8Bom[3] = { '\xEF', '\xBB', '\xBF' };

template<class BASE = std::exception>
class wexception : public BASE, public wexception_base
{
public:
    using BASE::BASE;
    template<typename... ARGS>
    wexception(wchar_t const* message, ARGS&&... args) : BASE(ToUtf8(message).c_str(), std::forward<ARGS>(args...))
    {
        static_assert(std::is_base_of_v<std::exception, BASE>, "BASE must be std::exception, or derived from std::exception");
    }
    template<typename... ARGS>
    wexception(std::wstring const& message, ARGS&&... args) : BASE(ToUtf8(message.c_str()).c_str(), std::forward<ARGS>(args)...)
    {
        static_assert(std::is_base_of_v<std::exception, BASE>, "BASE must be std::exception, or derived from std::exception");
    }
    template<typename... ARGS>
    wexception(std::string const& message, ARGS&&... args) : BASE(message.c_str(), std::forward<ARGS>(args)...)
    {
        static_assert(std::is_base_of_v<std::exception, BASE>, "BASE must be std::exception, or derived from std::exception");
    }
}; // template class wexception