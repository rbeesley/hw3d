#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>
#ifdef UNICODE
#include <tchar.h>
#endif

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if( FAILED(hresult = (hrcall))) throw graphics::hresult_exception(__LINE__, __FILE__, hresult)

dxgi_info_manager::dxgi_info_manager()
{
	// define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// load the dll that contains the function DXGIGetDebugInterface
#ifdef UNICODE
	const auto module = LoadLibraryEx(_T("dxgidebug.dll"), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
#else
	const auto module = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
#endif
	if (module == nullptr)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	// get address of DXGIGetDebugInterface in dll
	const auto debug_interface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(module, "DXGIGetDebugInterface"))
		);
	if (debug_interface == nullptr)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	HRESULT hresult;
	GFX_THROW_NOINFO(debug_interface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&dxgi_info_queue_)));
}

dxgi_info_manager::~dxgi_info_manager()
{
	if(dxgi_info_queue_ != nullptr)
	{
		dxgi_info_queue_->Release();
	}
}

void dxgi_info_manager::set() noexcept
{
	// set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	next_ = dxgi_info_queue_->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> dxgi_info_manager::get_messages() const
{
	std::vector<std::string> messages;
	const auto end = dxgi_info_queue_->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for(auto i = next_; i < end; i++)
	{
		HRESULT hresult;
		SIZE_T message_length = 0;

		// Get the size of message i in bytes
		GFX_THROW_NOINFO(dxgi_info_queue_->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &message_length));

		// Allocate memory for the message
		auto bytes = std::make_unique<byte[]>(message_length);
		auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// Retrieve the message and push its description into the messages queue
		GFX_THROW_NOINFO(dxgi_info_queue_->GetMessage(DXGI_DEBUG_ALL, i, message, &message_length));
		messages.emplace_back(message->pDescription);
	}

	return messages;
}
