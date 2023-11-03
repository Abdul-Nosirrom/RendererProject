#pragma once


#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, hr)
#define GFX_THROW_NOINFO(hrcall) if (HRESULT hr1 = hrcall; FAILED(hr1)) throw Graphics::HrException(__LINE__, __FILE__, hr1)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, hr, m_InfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) m_InfoManager.Set(); if (HRESULT hr1 = hrcall; FAILED(hr1)) throw GFX_EXCEPT(hr1)
#define GFX_THROW_INFO_ONLY(call) m_InfoManager.Set(); (call); {auto v = m_InfoManager.GetMessages(); if (!v.empty()) { throw Graphics::InfoException(__LINE__, __FILE__, v); } }
#define GFX_DEVICE_REMOVED_EXCEPT(hrcall) if (HRESULT hr2 = hrcall; FAILED(hr2)) { if (hr2 == DXGI_ERROR_DEVICE_REMOVED) throw Graphics::DeviceRemovedException(__LINE__, __FILE__, pDevice->GetDeviceRemovedReason(), m_InfoManager.GetMessages()); else GFX_EXCEPT(hr2); }
#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_THROW_INFO_ONLY(call) (call)
#define GFX_DEVICE_REMOVED_EXCEPT(hrcall) if (HRESULT hr2 = hrcall; FAILED(hr2)) { if (hr2 == DXGI_ERROR_DEVICE_REMOVED) throw Graphics::DeviceRemovedException(__LINE__, __FILE__, pDevice->GetDeviceRemovedReason()); else GFX_EXCEPT(hr2); }
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager() must exist
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr;
#else
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& m_InfoManager = GetInfoManager(gfx)
#endif