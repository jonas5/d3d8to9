/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to9.hpp"

Direct3DSurface8::Direct3DSurface8(Direct3DDevice8 *device, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, DWORD Usage) :
	Device(device),
	Width(Width),
	Height(Height),
	Format(Format),
	MultiSample(MultiSample),
	MultisampleQuality(MultisampleQuality),
	Lockable(Lockable),
	Usage(Usage)
{
}
Direct3DSurface8::Direct3DSurface8(Direct3DDevice8 *device, IDirect3DSurface9 *ProxyInterface) :
	Device(device),
	ProxyInterface(ProxyInterface),
	Width(0),
	Height(0),
	Format(D3DFMT_UNKNOWN),
	MultiSample(D3DMULTISAMPLE_NONE),
	MultisampleQuality(0),
	Lockable(FALSE),
	Usage(0)
{
	Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DSurface8::~Direct3DSurface8()
{
}

void Direct3DSurface8::PreReset()
{
	if (ProxyInterface != nullptr)
	{
		D3DSURFACE_DESC desc;
		if (SUCCEEDED(ProxyInterface->GetDesc(&desc)) && desc.Pool == D3DPOOL_DEFAULT)
		{
			IUnknown *pContainer = nullptr;
			if (FAILED(ProxyInterface->GetContainer(IID_IUnknown, (void**)&pContainer)))
			{
				ProxyInterface->Release();
				ProxyInterface = nullptr;
			}
			if (pContainer)
			{
				pContainer->Release();
			}
		}
	}
}
void Direct3DSurface8::PostReset()
{
	if (ProxyInterface == nullptr && Usage != 0)
	{
		if (Usage & D3DUSAGE_RENDERTARGET)
		{
			Device->GetProxyInterface()->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, &ProxyInterface, nullptr);
		}
		else if (Usage & D3DUSAGE_DEPTHSTENCIL)
		{
			Device->GetProxyInterface()->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Device->GetZBufferDiscarding(), &ProxyInterface, nullptr);
		}
	}
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

	const HRESULT hr = ProxyInterface->QueryInterface(ConvertREFIID(riid), ppvObj);
	if (SUCCEEDED(hr))
		GenericQueryInterface(riid, ppvObj, Device);

	return hr;
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
	return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
	return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::FreePrivateData(REFGUID refguid)
{
	return ProxyInterface->FreePrivateData(refguid);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetContainer(REFIID riid, void **ppContainer)
{
	const HRESULT hr = ProxyInterface->GetContainer(ConvertREFIID(riid), ppContainer);
	if (SUCCEEDED(hr))
		GenericQueryInterface(riid, ppContainer, Device);

	return hr;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetDesc(D3DSURFACE_DESC8 *pDesc)
{
	if (pDesc == nullptr)
		return D3DERR_INVALIDCALL;

	D3DSURFACE_DESC SurfaceDesc;

	const HRESULT hr = ProxyInterface->GetDesc(&SurfaceDesc);
	if (FAILED(hr))
		return hr;

	ConvertSurfaceDesc(SurfaceDesc, *pDesc);

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::LockRect(D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags)
{
	return ProxyInterface->LockRect(pLockedRect, pRect, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::UnlockRect()
{
	return ProxyInterface->UnlockRect();
}
