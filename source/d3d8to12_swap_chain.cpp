/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to12.hpp"
#include "d3d8to12_device.hpp"

Direct3DSwapChain8::Direct3DSwapChain8(Direct3DDevice8 *device, IDXGISwapChain *ProxyInterface) :
	Device(device), ProxyInterface(ProxyInterface)
{
}
Direct3DSwapChain8::~Direct3DSwapChain8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DSwapChain8::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DSwapChain8) ||
		riid == __uuidof(IUnknown))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DSwapChain8 *>(this);

		return S_OK;
	}

	return ProxyInterface->QueryInterface(riid, ppvObj);
}
ULONG STDMETHODCALLTYPE Direct3DSwapChain8::AddRef()
{
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DSwapChain8::Release()
{
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DSwapChain8::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	UNREFERENCED_PARAMETER(pDestRect);
	UNREFERENCED_PARAMETER(hDestWindowOverride);
	UNREFERENCED_PARAMETER(pDirtyRegion);

	return ProxyInterface->Present(1, 0);
}
HRESULT STDMETHODCALLTYPE Direct3DSwapChain8::GetBackBuffer(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8 **ppBackBuffer)
{
	// TODO: Implement
	return E_NOTIMPL;
}
