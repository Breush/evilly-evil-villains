#pragma once

// File created by A. Breust so that steam is in its own namespace.
// Each time you need another function of the api, just include the file here.

namespace steam
{
	// Steam officially knows only Visual C++ on Windows,
	// this corrects it to be aware of MinGW
	#if defined(_WIN32)
	#if defined(__GNUC__)
		#undef _WIN32
	#endif
	#endif
	
	#include "steam_api.h"
    #include "steam_gameserver.h"
}

