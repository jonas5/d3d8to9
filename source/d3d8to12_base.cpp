/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to12.hpp"
#include "d3d8to12_device.hpp"

Direct3D8::Direct3D8()
{
	IDXGIFactory1* dxgi_factory = nullptr;
	if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgi_factory)))
	{
		for (UINT i = 0; ; ++i)
		{
			IDXGIAdapter1* adapter = nullptr;
			if (FAILED(dxgi_factory->EnumAdapters1(i, &adapter)))
				break;

			m_adapters.push_back(adapter);
		}
		dxgi_factory->Release();
	}
}
Direct3D8::~Direct3D8()
{
	for (auto adapter : m_adapters)
		adapter->Release();
}

HRESULT STDMETHODCALLTYPE Direct3D8::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3D8) ||
		riid == __uuidof(IUnknown))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3D8 *>(this);

		return S_OK;
	}

	return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE Direct3D8::AddRef()
{
	return ++m_refCount;
}
ULONG STDMETHODCALLTYPE Direct3D8::Release()
{
	const ULONG ref = --m_refCount;
	if (ref == 0)
		delete this;
	return ref;
}

HRESULT STDMETHODCALLTYPE Direct3D8::RegisterSoftwareDevice(void *pInitializeFunction)
{
	UNREFERENCED_PARAMETER(pInitializeFunction);
	return D3D_OK;
}
UINT STDMETHODCALLTYPE Direct3D8::GetAdapterCount()
{
	return (UINT)m_adapters.size();
}
HRESULT STDMETHODCALLTYPE Direct3D8::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8 *pIdentifier)
{
	UNREFERENCED_PARAMETER(Flags);

	if (Adapter >= GetAdapterCount() || pIdentifier == nullptr)
		return D3DERR_INVALIDCALL;

	DXGI_ADAPTER_DESC1 desc;
	m_adapters[Adapter]->GetDesc1(&desc);

	memcpy(pIdentifier->Driver, "d3d8to12", 10);
	memcpy(pIdentifier->Description, "d3d8to12", 10);
	pIdentifier->DriverVersion.QuadPart = 0;
	pIdentifier->VendorId = desc.VendorId;
	pIdentifier->DeviceId = desc.DeviceId;
	pIdentifier->SubSysId = desc.SubSysId;
	pIdentifier->Revision = desc.Revision;
	pIdentifier->DeviceIdentifier = GUID_NULL;
	pIdentifier->WHQLLevel = 0;

	return D3D_OK;
}
UINT STDMETHODCALLTYPE Direct3D8::GetAdapterModeCount(UINT Adapter)
{
	if (Adapter >= GetAdapterCount())
		return 0;

	// TODO: Implement
	return 1;
}
HRESULT STDMETHODCALLTYPE Direct3D8::EnumAdapterModes(UINT Adapter, UINT Mode, D3DDISPLAYMODE *pMode)
{
	if (Adapter >= GetAdapterCount() || pMode == nullptr)
		return D3DERR_INVALIDCALL;

	// TODO: Implement
	pMode->Width = 1920;
	pMode->Height = 1080;
	pMode->RefreshRate = 60;
	pMode->Format = D3DFMT_X8R8G8B8;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3D8::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode)
{
	return EnumAdapterModes(Adapter, 0, pMode);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
	UNREFERENCED_PARAMETER(Adapter);
	UNREFERENCED_PARAMETER(CheckType);
	UNREFERENCED_PARAMETER(DisplayFormat);
	UNREFERENCED_PARAMETER(BackBufferFormat);
	UNREFERENCED_PARAMETER(bWindowed);

	// TODO: Implement
	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	UNREFERENCED_PARAMETER(Adapter);
	UNREFERENCED_PARAMETER(DeviceType);
	UNREFERENCED_PARAMETER(AdapterFormat);
	UNREFERENCED_PARAMETER(Usage);
	UNREFERENCED_PARAMETER(RType);
	UNREFERENCED_PARAMETER(CheckFormat);

	// TODO: Implement
	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType)
{
	UNREFERENCED_PARAMETER(Adapter);
	UNREFERENCED_PARAMETER(DeviceType);
	UNREFERENCED_PARAMETER(SurfaceFormat);
	UNREFERENCED_PARAMETER(Windowed);
	UNREFERENCED_PARAMETER(MultiSampleType);

	// TODO: Implement
	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	UNREFERENCED_PARAMETER(Adapter);
	UNREFERENCED_PARAMETER(DeviceType);
	UNREFERENCED_PARAMETER(AdapterFormat);
	UNREFERENCED_PARAMETER(RenderTargetFormat);
	UNREFERENCED_PARAMETER(DepthStencilFormat);

	// TODO: Implement
	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3D8::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS8 *pCaps)
{
	if (Adapter >= GetAdapterCount() || pCaps == nullptr)
		return D3DERR_INVALIDCALL;

	// TODO: Fill this with more accurate values
	ZeroMemory(pCaps, sizeof(D3DCAPS8));
	pCaps->DeviceType = DeviceType;
	pCaps->AdapterOrdinal = Adapter;
	pCaps->Caps = 0;
	pCaps->Caps2 = 0;
	pCaps->Caps3 = 0;
	pCaps->PresentationIntervals = 0;
	pCaps->CursorCaps = 0;
	pCaps->DevCaps = 0;
	pCaps->PrimitiveMiscCaps = 0;
	pCaps->RasterCaps = 0;
	pCaps->ZCmpCaps = 0;
	pCaps->SrcBlendCaps = 0;
	pCaps->DestBlendCaps = 0;
	pCaps->AlphaCmpCaps = 0;
	pCaps->ShadeCaps = 0;
	pCaps->TextureCaps = 0;
	pCaps->TextureFilterCaps = 0;
	pCaps->CubeTextureFilterCaps = 0;
	pCaps->VolumeTextureFilterCaps = 0;
	pCaps->TextureAddressCaps = 0;
	pCaps->VolumeTextureAddressCaps = 0;
	pCaps->LineCaps = 0;
	pCaps->MaxTextureWidth = 16384;
	pCaps->MaxTextureHeight = 16384;
	pCaps->MaxVolumeExtent = 2048;
	pCaps->MaxTextureRepeat = 0;
	pCaps->MaxTextureAspectRatio = 0;
	pCaps->MaxAnisotropy = 16;
	pCaps->MaxVertexW = 0;
	pCaps->GuardBandLeft = 0;
	pCaps->GuardBandTop = 0;
	pCaps->GuardBandRight = 0;
	pCaps->GuardBandBottom = 0;
	pCaps->ExtentsAdjust = 0;
	pCaps->StencilCaps = 0;
	pCaps->FVFCaps = 0;
	pCaps->TextureOpCaps = 0;
	pCaps->MaxTextureBlendStages = 8;
	pCaps->MaxSimultaneousTextures = 8;
	pCaps->VertexProcessingCaps = 0;
	pCaps->MaxActiveLights = 0;
	pCaps->MaxUserClipPlanes = 6;
	pCaps->MaxVertexBlendMatrices = 0;
	pCaps->MaxVertexBlendMatrixIndex = 0;
	pCaps->MaxPointSize = 1.0f;
	pCaps->MaxPrimitiveCount = 0xFFFFFFFF;
	pCaps->MaxVertexIndex = 0xFFFFFFFF;
	pCaps->MaxStreams = 16;
	pCaps->MaxStreamStride = 255;
	pCaps->VertexShaderVersion = D3DVS_VERSION(1, 1);
	pCaps->MaxVertexShaderConst = 96;
	pCaps->PixelShaderVersion = D3DPS_VERSION(1, 4);
	pCaps->PixelShader1xMaxValue = 1.0f;
	pCaps->DevCaps2 = 0;

	return D3D_OK;
}
HMONITOR STDMETHODCALLTYPE Direct3D8::GetAdapterMonitor(UINT Adapter)
{
	if (Adapter >= GetAdapterCount())
		return nullptr;

	DXGI_ADAPTER_DESC1 desc;
	m_adapters[Adapter]->GetDesc1(&desc);
	return desc.Monitor;
}
HRESULT STDMETHODCALLTYPE Direct3D8::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS8 *pPresentationParameters, IDirect3DDevice8 **ppReturnedDeviceInterface)
{
	if (Adapter >= GetAdapterCount() || ppReturnedDeviceInterface == nullptr)
		return D3DERR_INVALIDCALL;

	ID3D12Device* device = nullptr;
	HRESULT hr = D3D12CreateDevice(m_adapters[Adapter], D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device);
	if (FAILED(hr))
		return hr;

	*ppReturnedDeviceInterface = new Direct3DDevice8(this, device, BehaviorFlags, pPresentationParameters);

	return D3D_OK;
}
