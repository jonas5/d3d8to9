/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to12.hpp"
#include "d3d8to12_device.hpp"

Direct3DSurface8::Direct3DSurface8(Direct3DDevice8 *device, ID3D12Resource *ProxyInterface) :
	Device(device), ProxyInterface(ProxyInterface)
{
}
Direct3DSurface8::~Direct3DSurface8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DSurface8::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DSurface8) ||
		riid == __uuidof(IUnknown))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DSurface8 *>(this);

		return S_OK;
	}

	return ProxyInterface->QueryInterface(riid, ppvObj);
}
ULONG STDMETHODCALLTYPE Direct3DSurface8::AddRef()
{
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DSurface8::Release()
{
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetDevice(IDirect3DDevice8 **ppDevice)
{
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();

	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
	return ProxyInterface->SetPrivateData(refguid, SizeOfData, pData);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
	return ProxyInterface->GetPrivateData(refguid, pSizeOfData, pData);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::FreePrivateData(REFGUID refguid)
{
	return ProxyInterface->SetPrivateData(refguid, 0, nullptr);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetContainer(REFIID riid, void **ppContainer)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetDesc(D3DSURFACE_DESC8 *pDesc)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::LockRect(D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::UnlockRect()
{
	// TODO: Implement
	return E_NOTIMPL;
}
