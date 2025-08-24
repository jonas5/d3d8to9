/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to12.hpp"
#include "d3d8to12_device.hpp"

Direct3DVertexBuffer8::Direct3DVertexBuffer8(Direct3DDevice8 *Device, ID3D12Resource *ProxyInterface) :
	Device(Device), ProxyInterface(ProxyInterface)
{
}
Direct3DVertexBuffer8::~Direct3DVertexBuffer8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DVertexBuffer8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DVertexBuffer8 *>(this);

		return S_OK;
	}

	return ProxyInterface->QueryInterface(riid, ppvObj);
}
ULONG STDMETHODCALLTYPE Direct3DVertexBuffer8::AddRef()
{
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DVertexBuffer8::Release()
{
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetDevice(IDirect3DDevice8 **ppDevice)
{
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
	return ProxyInterface->SetPrivateData(refguid, SizeOfData, pData);
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
	return ProxyInterface->GetPrivateData(refguid, pSizeOfData, pData);
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::FreePrivateData(REFGUID refguid)
{
	return ProxyInterface->SetPrivateData(refguid, 0, nullptr);
}
DWORD STDMETHODCALLTYPE Direct3DVertexBuffer8::SetPriority(DWORD PriorityNew)
{
	// TODO: Implement
	return 0;
}
DWORD STDMETHODCALLTYPE Direct3DVertexBuffer8::GetPriority()
{
	// TODO: Implement
	return 0;
}
void STDMETHODCALLTYPE Direct3DVertexBuffer8::PreLoad()
{
	// TODO: Implement
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DVertexBuffer8::GetType()
{
	return D3DRTYPE_VERTEXBUFFER;
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::Lock(UINT OffsetToLock, UINT SizeToLock, BYTE **ppbData, DWORD Flags)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::Unlock()
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetDesc(D3DVERTEXBUFFER_DESC *pDesc)
{
	// TODO: Implement
	return E_NOTIMPL;
}
