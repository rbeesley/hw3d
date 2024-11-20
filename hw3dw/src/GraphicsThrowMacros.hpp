#pragma once

// Graphics exception macros, some with DXGI info

// Get the resulting error message and log the exception, no info manager
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT_NOINFO(hresult) Graphics::HresultException(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check, no info manager
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_NOINFO(hresultCall) if( FAILED(hresult = (hresultCall)) ) { throw Graphics::HresultException(__LINE__, __FILE__, hresult); }

#if (IS_DEBUG)
// Get the resulting error message and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT(hresult) Graphics::HresultException(__LINE__, __FILE__, (hresult), infoManager_.getMessages())
// Wrap graphics call with error check
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_INFO(hresultCall) infoManager_.set(); if( FAILED(hresult = (hresultCall)) ) { throw GFX_EXCEPT(hresult); }
// Get the error for a device removed and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hresult), infoManager_.getMessages())
// Wrap graphics call with error check for a call which doesn't return an hresult
#define GFX_THROW_INFO_ONLY(call) infoManager_.set(); (call); { auto v = infoManager_.getMessages(); if( !v.empty() ) { throw Graphics::InfoException(__LINE__, __FILE__, v); }}
#else
// Get the resulting error message and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT(hresult) Graphics::HresultException(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_INFO(hresultCall) if( FAILED(hresult = (hresultCall)) ) { throw Graphics::HresultException(__LINE__, __FILE__, (hresult)); }
// Get the error for a device removed and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check for a call which doesn't return an hresult
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// Macro for importing infomanager into the local scope
// this.GetInfoManager() must exist
#if (IS_DEBUG)
#define INFOMAN(graphics) HRESULT hresult; DxgiInfoManager& infoManager_ = getInfoManager(graphics)
#else
#define INFOMAN(_) HRESULT hresult
#endif
