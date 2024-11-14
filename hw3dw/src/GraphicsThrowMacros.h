#pragma once

// Graphics exception macros, some with DXGI info

// Get the resulting error message and log the exception, no info manager
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT_NOINFO(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check, no info manager
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_NOINFO(hresult_call) if( FAILED(hresult = (hresult_call)) ) { throw graphics::hresult_exception(__LINE__, __FILE__, hresult); }

#if (IS_DEBUG)
// Get the resulting error message and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult), info_manager_.get_messages())
// Wrap graphics call with error check
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_INFO(hresult_call) info_manager_.set(); if( FAILED(hresult = (hresult_call)) ) { throw GFX_EXCEPT(hresult); }
// Get the error for a device removed and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) graphics::device_removed_exception(__LINE__, __FILE__, (hresult), info_manager_.get_messages())
// Wrap graphics call with error check for a call which doesn't return an hresult
#define GFX_THROW_INFO_ONLY(call) info_manager_.set(); (call); { auto v = info_manager_.get_messages(); if( !v.empty() ) { throw graphics::info_exception(__LINE__, __FILE__, v); }}
#else
// Get the resulting error message and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_INFO(hresult_call) if( FAILED(hresult = (hresult_call)) ) { throw graphics::hresult_exception(__LINE__, __FILE__, (hresult)); }
// Get the error for a device removed and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) graphics::device_removed_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check for a call which doesn't return an hresult
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// Macro for importing infomanager into the local scope
// this.GetInfoManager() must exist
#if (IS_DEBUG)
#define INFOMAN(graphics) HRESULT hresult; dxgi_info_manager& info_manager_ = get_info_manager(graphics)
#else
#define INFOMAN(_) HRESULT hresult
#endif
