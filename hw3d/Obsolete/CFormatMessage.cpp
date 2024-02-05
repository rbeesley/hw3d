#include "CFormatMessage.h"

CFormatMessage::CFormatMessage(DWORD dwMessageId, DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))
	:
	m_text(nullptr)
{
	Assign(dwMessageId, dwLanguageId);
}

CFormatMessage::~CFormatMessage()
{
	Clear();
}

void CFormatMessage::Clear()
{
	if (m_text) {
		LocalFree(m_text);
		m_text = nullptr;
	}
}

void CFormatMessage::Assign(DWORD dwMessageId, DWORD dwLanguageID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))
{
	Clear();
	DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS
		| FORMAT_MESSAGE_MAX_WIDTH_MASK;
	FormatMessage(
		dwFlags,
		nullptr,
		dwMessageId,
		dwLanguageID,
		reinterpret_cast<LPTSTR>(m_text),
		0,
		nullptr);
}

LPTSTR CFormatMessage::text() const
{
	return m_text;
}

CFormatMessage::operator LPTSTR() const
{
	return text();
}
