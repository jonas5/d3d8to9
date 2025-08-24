/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 */

#include "d3d8to12.hpp"

#ifndef D3D8TO12_NOLOG
 // Very simple logging for the purpose of debugging only.
std::ofstream LOG;
#endif

extern "C" void WINAPI DebugSetMute()
{
#ifndef D3D8TO12_NOLOG
	LOG << "Redirecting '" << "DebugSetMute" << "(" << ")' ..." << std::endl;
#endif
}

extern "C" IDirect3D8 *WINAPI Direct3DCreate8(UINT SDKVersion)
{
#ifndef D3D8TO12_NOLOG
	static bool LogMessageFlag = true;

	if (!LOG.is_open())
	{
		LOG.open("d3d8.log", std::ios::trunc);
	}

	if (!LOG.is_open() && LogMessageFlag)
	{
		LogMessageFlag = false;
		MessageBox(nullptr, TEXT("Failed to open debug log file \"d3d8.log\"!"), nullptr, MB_ICONWARNING);
	}

	LOG << "Redirecting '" << "Direct3DCreate8" << "(" << SDKVersion << ")' ..." << std::endl;
#endif

	return new Direct3D8();
}
