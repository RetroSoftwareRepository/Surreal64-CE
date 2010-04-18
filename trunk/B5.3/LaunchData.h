
// Ez0n3 - this file isn't used anymore

#pragma once

typedef struct _LaunchData
{
	// 1964 settings
	DWORD dw1964DynaMem;
	DWORD dw1964PagingMem;
	
	// Pj64 settings
	DWORD dwPJ64DynaMem;
	DWORD dwPJ64PagingMem;

	// common settings
	BOOL bUseLLERSP;
	
	// Ez0n3 - reinstate max video mem
	DWORD dwMaxVideoMem;
	
} LaunchData;