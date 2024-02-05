#pragma once
#include "AtumWindows.h"

class CFormatMessage {
public:
	CFormatMessage(DWORD dwMessageId, DWORD dwLanguageId);
	~CFormatMessage();
	void Clear();
	void Assign(DWORD dwMessageId, DWORD dwLanguageID);
	LPTSTR text() const;
	operator LPTSTR() const;
protected:
	LPTSTR m_text;
};