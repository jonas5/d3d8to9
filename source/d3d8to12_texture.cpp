/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to12.hpp"
#include "d3d8to12_device.hpp"

Direct3DTexture8::Direct3DTexture8(Direct3DDevice8 *device, ID3D12Resource *ProxyInterface) :
	Device(device), ProxyInterface(ProxyInterface)
{
}
Direct3DTexture8::~Direct3DTexture8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DTexture8::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DTexture8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8) ||
		riid == __uuidof(IDirect3DBaseTexture8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DTexture8 *>(this);

		return S_OK;
	}

	return ProxyInterface->QueryInterface(riid, ppvObj);
}
ULONG STDMETHODCALLTYPE Direct3DTexture8::AddRef()
{
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DTexture8::Release()
{
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DTexture8::GetDevice(IDirect3DDevice8 **ppDevice)
{
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
	return ProxyInterface->SetPrivateData(refguid, SizeOfData, pData);
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
	return ProxyInterface->GetPrivateData(refguid, pSizeOfData, pData);
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::FreePrivateData(REFGUID refguid)
{
	return ProxyInterface->SetPrivateData(refguid, 0, nullptr);
}
DWORD STDMETHODCALLTYPE Direct3DTexture8::SetPriority(DWORD PriorityNew)
{
	// TODO: Implement
	return 0;
}
DWORD STDMETHODCALLTYPE Direct3DTexture8::GetPriority()
{
	// TODO: Implement
	return 0;
}
void STDMETHODCALLTYPE Direct3DTexture8::PreLoad()
{
	// TODO: Implement
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DTexture8::GetType()
{
	return D3DRTYPE_TEXTURE;
}

DWORD STDMETHODCALLTYPE Direct3DTexture8::SetLOD(DWORD LODNew)
{
	// TODO: Implement
	return 0;
}
DWORD STDMETHODCALLTYPE Direct3DTexture8::GetLOD()
{
	// TODO: Implement
	return 0;
}
DWORD STDMETHODCALLTYPE Direct3DTexture8::GetLevelCount()
{
	return ProxyInterface->GetDesc().MipLevels;
}

HRESULT STDMETHODCALLTYPE Direct3DTexture8::GetLevelDesc(UINT Level, D3DSURFACE_DESC8 *pDesc)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::GetSurfaceLevel(UINT Level, IDirect3DSurface8 **ppSurfaceLevel)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::LockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::UnlockRect(UINT Level)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DTexture8::AddDirtyRect(const RECT *pDirtyRect)
{
	// TODO: Implement
	return E_NOTIMPL;
}

Direct3DCubeTexture8::Direct3DCubeTexture8(Direct3DDevice8 *device, ID3D12Resource *ProxyInterface) :
	Device(device),
	ProxyInterface(ProxyInterface)
{
}
Direct3DCubeTexture8::~Direct3DCubeTexture8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DCubeTexture8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8) ||
		riid == __uuidof(IDirect3DBaseTexture8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DCubeTexture8 *>(this);

		return S_OK;
	}

	return ProxyInterface->QueryInterface(riid, ppvObj);
}
ULONG STDMETHODCALLTYPE Direct3DCubeTexture8::AddRef()
{
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DCubeTexture8::Release()
{
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::GetDevice(IDirect3DDevice8 **ppDevice)
{
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
	return ProxyInterface->SetPrivateData(refguid, SizeOfData, pData);
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
	return ProxyInterface->GetPrivateData(refguid, pSizeOfData, pData);
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::FreePrivateData(REFGUID refguid)
{
	return ProxyInterface->SetPrivateData(refguid, 0, nullptr);
}
DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::SetPriority(DWORD PriorityNew)
{
	// TODO: Implement
	return 0;
}
DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::GetPriority()
{
	// TODO: Implement
	return 0;
}
void STDMETHODCALLTYPE Direct3DCubeTexture8::PreLoad()
{
	// TODO: Implement
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DCubeTexture8::GetType()
{
	return D3DRTYPE_CUBETEXTURE;
}

DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::SetLOD(DWORD LODNew)
{
	// TODO: Implement
	return 0;
}
DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::GetLOD()
{
	// TODO: Implement
	return 0;
}
DWORD STDMETHODCALLTYPE Direct3DCubeTexture8::GetLevelCount()
{
	return ProxyInterface->GetDesc().MipLevels;
}

HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::GetLevelDesc(UINT Level, D3DSURFACE_DESC8 *pDesc)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface8 **ppCubeMapSurface)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DCubeTexture8::AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT *pDirtyRect)
{
	// TODO: Implement
	return E_NOTIMPL;
}
