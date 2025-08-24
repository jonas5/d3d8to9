/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to12.hpp"
#include "d3d8to12_device.hpp"
#include <d3dcompiler.h>

Direct3DDevice8::Direct3DDevice8(Direct3D8 *d3d, ID3D12Device* device, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS8 *pPresentationParameters) :
	m_d3d(d3d),
	m_device(device),
	m_rootSignature(nullptr),
	m_pipelineState(nullptr),
	m_vertexBuffer(nullptr),
	m_commandQueue(nullptr),
	m_swapChain(nullptr),
	m_commandAllocator(nullptr),
	m_commandList(nullptr),
	m_fence(nullptr),
	m_fenceEvent(nullptr),
	m_fenceValue(0),
	m_frameIndex(0)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_commandQueue);

	IDXGIFactory4* dxgi_factory = nullptr;
	if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&dxgi_factory)))
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Width = pPresentationParameters->BackBufferWidth;
		swapChainDesc.Height = pPresentationParameters->BackBufferHeight;
		swapChainDesc.Format = (DXGI_FORMAT)pPresentationParameters->BackBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		IDXGISwapChain1* swapChain = nullptr;
		dxgi_factory->CreateSwapChainForHwnd(
			m_commandQueue,
			pPresentationParameters->hDeviceWindow,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		);
		m_swapChain = (IDXGISwapChain3*)swapChain;
		dxgi_factory->Release();
	}

	m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_commandAllocator);
	m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)&m_commandList);
	m_commandList->Close();

	m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_fence);
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_rtvHeap);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT n = 0; n < 2; n++)
	{
		m_swapChain->GetBuffer(n, __uuidof(ID3D12Resource), (void**)&m_renderTargets[n]);
		m_device->CreateRenderTargetView(m_renderTargets[n], nullptr, rtvHandle);
		rtvHandle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature;
	ID3DBlob* error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_rootSignature);

	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;
	D3DCompileFromFile(L"source/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vertexShader, nullptr);
	D3DCompileFromFile(L"source/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &pixelShader, nullptr);

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature;
	psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
	psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	psoDesc.BlendState.IndependentBlendEnable = FALSE;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = (DXGI_FORMAT)pPresentationParameters->BackBufferFormat;
	psoDesc.SampleDesc.Count = 1;
	m_device->CreateGraphicsPipelineState(&psoDesc, __uuidof(ID3D12PipelineState), (void**)&m_pipelineState);
}

Direct3DDevice8::~Direct3DDevice8()
{
	if (m_fenceEvent != nullptr)
		CloseHandle(m_fenceEvent);
	if (m_fence != nullptr)
		m_fence->Release();
	if (m_pipelineState != nullptr)
		m_pipelineState->Release();
	if (m_rootSignature != nullptr)
		m_rootSignature->Release();
	if (m_vertexBuffer != nullptr)
		m_vertexBuffer->Release();
	if (m_commandList != nullptr)
		m_commandList->Release();
	if (m_commandAllocator != nullptr)
		m_commandAllocator->Release();
	if (m_rtvHeap != nullptr)
		m_rtvHeap->Release();
	for (int i = 0; i < 2; ++i)
		if (m_renderTargets[i] != nullptr)
			m_renderTargets[i]->Release();
	if (m_swapChain != nullptr)
		m_swapChain->Release();
	if (m_commandQueue != nullptr)
		m_commandQueue->Release();
	if (m_device != nullptr)
		m_device->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DDevice8::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DDevice8) ||
		riid == __uuidof(IUnknown))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DDevice8 *>(this);

		return S_OK;
	}

	return m_device->QueryInterface(riid, ppvObj);
}
ULONG STDMETHODCALLTYPE Direct3DDevice8::AddRef()
{
	return m_device->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DDevice8::Release()
{
	const ULONG ref = m_device->Release();
	if (ref == 0)
		delete this;
	return ref;
}

HRESULT STDMETHODCALLTYPE Direct3DDevice8::TestCooperativeLevel()
{
	return S_OK;
}
UINT STDMETHODCALLTYPE Direct3DDevice8::GetAvailableTextureMem()
{
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	// TODO: Get adapter from device
	// m_adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);
	// return (UINT)videoMemoryInfo.AvailableForReservation;
	return 0;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::ResourceManagerDiscardBytes(DWORD Bytes)
{
	UNREFERENCED_PARAMETER(Bytes);
	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetDirect3D(IDirect3D8 **ppD3D8)
{
	if (ppD3D8 == nullptr)
		return D3DERR_INVALIDCALL;

	m_d3d->AddRef();
	*ppD3D8 = m_d3d;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetDeviceCaps(D3DCAPS8 *pCaps)
{
	return m_d3d->GetDeviceCaps(0, D3DDEVTYPE_HAL, pCaps);
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetDisplayMode(D3DDISPLAYMODE *pMode)
{
	return m_d3d->GetAdapterDisplayMode(0, pMode);
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8 *pCursorBitmap)
{
	// TODO: Implement
	return E_NOTIMPL;
}
void STDMETHODCALLTYPE Direct3DDevice8::SetCursorPosition(UINT XScreenSpace, UINT YScreenSpace, DWORD Flags)
{
	// TODO: Implement
}
BOOL STDMETHODCALLTYPE Direct3DDevice8::ShowCursor(BOOL bShow)
{
	// TODO: Implement
	return TRUE;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS8 *pPresentationParameters, IDirect3DSwapChain8 **ppSwapChain)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::Reset(D3DPRESENT_PARAMETERS8 *pPresentationParameters)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	UNREFERENCED_PARAMETER(pSourceRect);
	UNREFERENCED_PARAMETER(pDestRect);
	UNREFERENCED_PARAMETER(hDestWindowOverride);
	UNREFERENCED_PARAMETER(pDirtyRegion);

	m_commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_commandList);
	m_swapChain->Present(1, 0);

	const UINT64 fence = m_fenceValue;
	m_commandQueue->Signal(m_fence, fence);
	m_fenceValue++;

	if (m_fence->GetCompletedValue() < fence)
	{
		m_fence->SetEventOnCompletion(fence, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetBackBuffer(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8 **ppBackBuffer)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetRasterStatus(D3DRASTER_STATUS *pRasterStatus)
{
	// TODO: Implement
	return E_NOTIMPL;
}
void STDMETHODCALLTYPE Direct3DDevice8::SetGammaRamp(DWORD Flags, const D3DGAMMARAMP *pRamp)
{
	// TODO: Implement
}
void STDMETHODCALLTYPE Direct3DDevice8::GetGammaRamp(D3DGAMMARAMP *pRamp)
{
	// TODO: Implement
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8 **ppTexture)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture8 **ppVolumeTexture)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture8 **ppCubeTexture)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer8 **ppVertexBuffer)
{
	UNREFERENCED_PARAMETER(Usage);
	UNREFERENCED_PARAMETER(FVF);
	UNREFERENCED_PARAMETER(Pool);

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = Length;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)&m_vertexBuffer);
	*ppVertexBuffer = new Direct3DVertexBuffer8(this, m_vertexBuffer);

	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8 **ppIndexBuffer)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8 **ppSurface)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface8 **ppSurface)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateImageSurface(UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface8 **ppSurface)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CopyRects(IDirect3DSurface8 *pSourceSurface, const RECT *pSourceRectsArray, UINT cRects, IDirect3DSurface8 *pDestinationSurface, const POINT *pDestPointsArray)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::UpdateTexture(IDirect3DBaseTexture8 *pSourceTexture, IDirect3DBaseTexture8 *pDestinationTexture)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetFrontBuffer(IDirect3DSurface8 *pDestSurface)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetRenderTarget(IDirect3DSurface8 *pRenderTarget, IDirect3DSurface8 *pNewZStencil)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetRenderTarget(IDirect3DSurface8 **ppRenderTarget)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetDepthStencilSurface(IDirect3DSurface8 **ppZStencilSurface)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::BeginScene()
{
	// TODO: Implement
	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::EndScene()
{
	// TODO: Implement
	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	UNREFERENCED_PARAMETER(Count);
	UNREFERENCED_PARAMETER(pRects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Z);
	UNREFERENCED_PARAMETER(Stencil);

	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator, nullptr);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_renderTargets[m_frameIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_commandList->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle.ptr = m_rtvHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_frameIndex * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	const float clearColor[] = { ((Color >> 16) & 0xFF) / 255.0f, ((Color >> 8) & 0xFF) / 255.0f, (Color & 0xFF) / 255.0f, ((Color >> 24) & 0xFF) / 255.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_commandList->ResourceBarrier(1, &barrier);

	m_commandList->Close();

	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetViewport(const D3DVIEWPORT8 *pViewport)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetViewport(D3DVIEWPORT8 *pViewport)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetMaterial(const D3DMATERIAL8 *pMaterial)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetMaterial(D3DMATERIAL8 *pMaterial)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetLight(DWORD Index, const D3DLIGHT8 *pLight)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetLight(DWORD Index, D3DLIGHT8 *pLight)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::LightEnable(DWORD Index, BOOL Enable)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetLightEnable(DWORD Index, BOOL *pEnable)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetClipPlane(DWORD Index, const float *pPlane)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetClipPlane(DWORD Index, float *pPlane)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::BeginStateBlock()
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::EndStateBlock(DWORD *pToken)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::ApplyStateBlock(DWORD Token)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CaptureStateBlock(DWORD Token)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DeleteStateBlock(DWORD Token)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateStateBlock(D3DSTATEBLOCKTYPE Type, DWORD *pToken)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetClipStatus(const D3DCLIPSTATUS8 *pClipStatus)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetClipStatus(D3DCLIPSTATUS8 *pClipStatus)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetTexture(DWORD Stage, IDirect3DBaseTexture8 **ppTexture)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetTexture(DWORD Stage, IDirect3DBaseTexture8 *pTexture)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::ValidateDevice(DWORD *pNumPasses)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetInfo(DWORD DevInfoID, void *pDevInfoStruct, DWORD DevInfoStructSize)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY *pEntries)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetCurrentTexturePalette(UINT PaletteNumber)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetCurrentTexturePalette(UINT *PaletteNumber)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	m_commandList->SetGraphicsRootSignature(m_rootSignature);
	m_commandList->SetPipelineState(m_pipelineState);

	D3D12_VIEWPORT viewport = { 0, 0, 800, 600, 0, 1 };
	D3D12_RECT scissorRect = { 0, 0, 800, 600 };
	m_commandList->RSSetViewports(1, &viewport);
	m_commandList->RSSetScissorRects(1, &scissorRect);

	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->DrawInstanced(PrimitiveCount * 3, 1, StartVertex, 0);

	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer8 *pDestBuffer, DWORD Flags)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreateVertexShader(const DWORD *pDeclaration, const DWORD *pFunction, DWORD *pHandle, DWORD Usage)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetVertexShader(DWORD Handle)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetVertexShader(DWORD *pHandle)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DeleteVertexShader(DWORD Handle)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetVertexShaderConstant(DWORD Register, const void *pConstantData, DWORD ConstantCount)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetVertexShaderConstant(DWORD Register, void *pConstantData, DWORD ConstantCount)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetVertexShaderDeclaration(DWORD Handle, void *pData, DWORD *pSizeOfData)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetVertexShaderFunction(DWORD Handle, void *pData, DWORD *pSizeOfData)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8 *pStreamData, UINT Stride)
{
	UNREFERENCED_PARAMETER(StreamNumber);

	if (pStreamData == nullptr)
	{
		return D3DERR_INVALIDCALL;
	}

	Direct3DVertexBuffer8* pVertexBufferImpl = static_cast<Direct3DVertexBuffer8*>(pStreamData);
	m_vertexBufferView.BufferLocation = pVertexBufferImpl->GetProxyInterface()->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = Stride;
	m_vertexBufferView.SizeInBytes = pVertexBufferImpl->GetProxyInterface()->GetDesc().Width;

	return S_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8 **ppStreamData, UINT *pStride)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetIndices(IDirect3DIndexBuffer8 *pIndexData, UINT BaseVertexIndex)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetIndices(IDirect3DIndexBuffer8 **ppIndexData, UINT *pBaseVertexIndex)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::CreatePixelShader(const DWORD *pFunction, DWORD *pHandle)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetPixelShader(DWORD Handle)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetPixelShader(DWORD *pHandle)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DeletePixelShader(DWORD Handle)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::SetPixelShaderConstant(DWORD Register, const void *pConstantData, DWORD ConstantCount)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetPixelShaderConstant(DWORD Register, void *pConstantData, DWORD ConstantCount)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::GetPixelShaderFunction(DWORD Handle, void *pData, DWORD *pSizeOfData)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DrawRectPatch(UINT Handle, const float *pNumSegs, const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DrawTriPatch(UINT Handle, const float *pNumSegs, const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	// TODO: Implement
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE Direct3DDevice8::DeletePatch(UINT Handle)
{
	// TODO: Implement
	return E_NOTIMPL;
}
