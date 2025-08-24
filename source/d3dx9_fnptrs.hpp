#pragma once

#include <d3d9.h>
#include <d3dx9tex.h>
#include <d3dx9shader.h>

// Typedefs
typedef HRESULT(WINAPI* PFN_D3DXAssembleShader)(LPCSTR, UINT, const D3DXMACRO*, LPD3DXINCLUDE, DWORD, LPD3DXBUFFER*, LPD3DXBUFFER*);
typedef HRESULT(WINAPI* PFN_D3DXDisassembleShader)(const DWORD*, BOOL, LPCSTR, LPD3DXBUFFER*);
typedef HRESULT(WINAPI* PFN_D3DXLoadSurfaceFromSurface)(LPDIRECT3DSURFACE9, const PALETTEENTRY*, const RECT*, LPDIRECT3DSURFACE9, const PALETTEENTRY*, const RECT*, DWORD, D3DCOLOR);

// Extern declarations
extern PFN_D3DXAssembleShader g_pfnD3DXAssembleShader;
extern PFN_D3DXDisassembleShader g_pfnD3DXDisassembleShader;
extern PFN_D3DXLoadSurfaceFromSurface g_pfnD3DXLoadSurfaceFromSurface;
