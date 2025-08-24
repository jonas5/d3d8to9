#pragma once

#ifndef CUSTOM_D3DX9_HPP
#define CUSTOM_D3DX9_HPP

// Prevent min/max macro conflicts
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <d3d9.h>

// These are custom definitions that are not part of the standard d3dx9.h header.
// They are used in the shader translation code.

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

#endif // CUSTOM_D3DX9_HPP
