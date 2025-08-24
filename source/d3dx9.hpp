#pragma once

#ifndef CUSTOM_D3DX9_HPP
#define CUSTOM_D3DX9_HPP

// Prevent min/max macro conflicts
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <d3d9.h>

namespace CustomD3DX {


    constexpr DWORD D3DX_CUSTOM_FILTER_NONE = 0x00000001;
    constexpr DWORD D3DX_CUSTOM_FILTER_POINT    = 0x00000002;
    constexpr DWORD D3DX_CUSTOM_FILTER_LINEAR   = 0x00000003;
    constexpr DWORD D3DX_CUSTOM_FILTER_TRIANGLE = 0x00000004;
    constexpr DWORD D3DX_CUSTOM_FILTER_BOX      = 0x00000005;

    // Macro flags for shader assembly
    constexpr DWORD D3DXASM_DEBUG = 0x0001;
    constexpr DWORD D3DXASM_SKIPVALIDATION = 0x0010;

    #ifdef NDEBUG
    constexpr DWORD D3DXASM_FLAGS = 0;
    #else
    constexpr DWORD D3DXASM_FLAGS = D3DXASM_DEBUG;
    #endif

    // Shader macro definition
    struct D3DXMACRO {
        LPCSTR Name;
        LPCSTR Definition;
    };

    // Forward declarations
    struct ID3DXBuffer;
    struct ID3DXInclude;

    typedef ID3DXBuffer* LPD3DXBUFFER;
    typedef ID3DXInclude* LPD3DXINCLUDE;

    // ID3DXBuffer interface
    struct ID3DXBuffer : public IUnknown {
        virtual LPVOID GetBufferPointer() = 0;
        virtual DWORD GetBufferSize() = 0;
    };

    // Function pointer typedefs
    typedef HRESULT(WINAPI* PFN_D3DXAssembleShader)(
        LPCSTR pSrcData,
        UINT SrcDataLen,
        const D3DXMACRO* pDefines,
        LPD3DXINCLUDE pInclude,
        DWORD Flags,
        LPD3DXBUFFER* ppShader,
        LPD3DXBUFFER* ppErrorMsgs
    );

    typedef HRESULT(WINAPI* PFN_D3DXDisassembleShader)(
        const DWORD* pShader,
        BOOL EnableColorCode,
        LPCSTR pComments,
        LPD3DXBUFFER* ppDisassembly
    );

    typedef HRESULT(WINAPI* PFN_D3DXLoadSurfaceFromSurface)(
        LPDIRECT3DSURFACE9 pDestSurface,
        const PALETTEENTRY* pDestPalette,
        const RECT* pDestRect,
        LPDIRECT3DSURFACE9 pSrcSurface,
        const PALETTEENTRY* pSrcPalette,
        const RECT* pSrcRect,
        DWORD Filter,
        D3DCOLOR ColorKey
    );

    // External function pointers (to be assigned at runtime)
    extern PFN_D3DXAssembleShader D3DXAssembleShader;
    extern PFN_D3DXDisassembleShader D3DXDisassembleShader;
    extern PFN_D3DXLoadSurfaceFromSurface D3DXLoadSurfaceFromSurface;

} // namespace CustomD3DX

#endif // CUSTOM_D3DX9_HPP
