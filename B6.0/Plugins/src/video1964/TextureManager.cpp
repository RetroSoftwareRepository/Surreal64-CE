/*
Copyright (C) 2003 Rice1964

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "stdafx.h"
#include "Config.h"

#ifdef _XBOX
#include "../../../config.h"
#include "../../../ultraxle/IoSupport.h"
#endif

static const DWORD MEM_KEEP_FREE = (2*1024*1024); // keep 2MB free
DWORD g_maxTextureMemUsage = (5*1024*1024);
BOOL bPurgeOldBeforeIGM = FALSE;
bool g_bUseSetTextureMem = true;

CTextureManager gTextureManager;
extern RecentCIInfo* g_uRecentCIInfoPtrs[];

// Returns the first prime greater than or equal to nFirst
inline LONG GetNextPrime(LONG nFirst)
{
	LONG nCurrent;

	LONG i;

	nCurrent = nFirst;

	// Just make sure it's odd
	if ((nCurrent % 2) == 0)
		nCurrent++;

	for (;;)
	{
		LONG nSqrtCurrent;
		BOOL bIsComposite;

		// nSqrtCurrent = nCurrent^0.5 + 1 (round up)
		nSqrtCurrent = (LONG)sqrt((double)nCurrent) + 1;


		bIsComposite = FALSE;
		
		// Test all odd numbers from 3..nSqrtCurrent
		for (i = 3; i <= nSqrtCurrent; i+=2)
		{
			if ((nCurrent % i) == 0)
			{
				bIsComposite = TRUE;
				break;
			}
		}

		if (!bIsComposite)
		{			
			return nCurrent;
		}

		// Select next odd candidate...
		nCurrent += 2;
	}

}



///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////

CTextureManager::CTextureManager() :
	m_pHead(NULL),
	m_pCacheTxtrList(NULL),
	m_numOfCachedTxtrList(809)
{
	m_numOfCachedTxtrList = GetNextPrime(800);
#ifdef LIMIT_TEXTMEM
	m_currentTextureMemUsage	= 0;
	m_pYoungestTexture			= NULL;
	m_pOldestTexture			= NULL;
#endif

	m_pCacheTxtrList = new TxtrCacheEntry *[m_numOfCachedTxtrList];
	SAFE_CHECK(m_pCacheTxtrList);

	for (uint32 i = 0; i < m_numOfCachedTxtrList; i++)
		m_pCacheTxtrList[i] = NULL;

	memset(&m_blackTextureEntry, 0, sizeof(TxtrCacheEntry));
	memset(&m_PrimColorTextureEntry, 0, sizeof(TxtrCacheEntry));
	memset(&m_EnvColorTextureEntry, 0, sizeof(TxtrCacheEntry));
	memset(&m_LODFracTextureEntry, 0, sizeof(TxtrCacheEntry));
	memset(&m_PrimLODFracTextureEntry, 0, sizeof(TxtrCacheEntry));
}

CTextureManager::~CTextureManager()
{
	CleanUp();
#ifdef LIMIT_TEXTMEM
	{
		while (m_pHead)
		{
			TxtrCacheEntry * pVictim = m_pHead;
			m_pHead = pVictim->pNext;
			
			delete pVictim;
		}
	}
	m_currentTextureMemUsage	= 0;
#endif

	delete []m_pCacheTxtrList;
	m_pCacheTxtrList = NULL;	
}
#ifdef _XBOX
//
// Decommit textures to file
//
bool CTextureManager::ReInitTextureMemory(bool SaveTexture)
{
	FILE *fp;
	bool isComplete = 0;
	char TexturePath[100];
	char InfoPath[100];
	//int numTexts;
	//TxtrInfo TItoLoad;
	TxtrCacheEntry * TextureToLoad;
	
	if(XGetDiskSectorSize("Z:\\") == 0)
		g_IOSupport.Mount("Z:","Harddisk0\\Partition5");
	
	if (SaveTexture) {
		for ( uint32 i = 0; i <= m_numOfCachedTxtrList+1; i++ )
		{
			TxtrCacheEntry * pEntry;
			
			pEntry = m_pCacheTxtrList[i];

			sprintf(TexturePath,"Z:\\Textures\\Temp\\T\\%s-%s-%s_%d.dat", g_curRomInfo.romheader.dwCRC1,
				g_curRomInfo.romheader.dwCRC2, g_curRomInfo.romheader.nCountryID,i);
				
			DeleteFile(TexturePath);
			if(pEntry != NULL){
				fp=fopen(TexturePath,"wb");
				if(fp){
					fwrite(pEntry, sizeof(TxtrCacheEntry), 1, fp);
					fclose(fp);
				}
			}
			CloseHandle(TexturePath);
				
			if(i == m_numOfCachedTxtrList+1){
				sprintf(InfoPath,"Z:\\Textures\\Temp\\I\\%s-%s-%s_num.dat", g_curRomInfo.romheader.dwCRC1,
					g_curRomInfo.romheader.dwCRC2, g_curRomInfo.romheader.nCountryID);
				DeleteFile(InfoPath);
				fp=fopen(InfoPath,"wb");
				if(fp){
					fprintf(fp,"%d",i-1);
					fclose(fp);
				}
				CloseHandle(InfoPath);
				isComplete = 1;
				break;
			}
		}
	}
	else {
		int numTexts=0;
		//unsigned int TextSize;
		char Temp[100];
		
		sprintf(InfoPath,"Z:\\Textures\\Temp\\I\\%s-%s-%s_num.dat", g_curRomInfo.romheader.dwCRC1,
				g_curRomInfo.romheader.dwCRC2, g_curRomInfo.romheader.nCountryID);
		fp=fopen(InfoPath,"rb");
		if(fp){
		fgets(Temp,100, fp);
		for(int i = 0; i < 100; i++) {
			numTexts = (int) atoi(&Temp[i]);
			break;
		}
		fclose(fp);
		}
		DeleteFile(InfoPath);
		CloseHandle(InfoPath);

		for ( int i = 0; i <= numTexts; i++ )
		{
			sprintf(TexturePath,"Z:\\Textures\\Temp\\T\\%s-%s-%s_%d.dat", g_curRomInfo.romheader.dwCRC1,
				g_curRomInfo.romheader.dwCRC2, g_curRomInfo.romheader.nCountryID,i);
			sprintf(InfoPath,"Z:\\Textures\\Temp\\I\\%s-%s-%s_%d.dat", g_curRomInfo.romheader.dwCRC1,
				g_curRomInfo.romheader.dwCRC2, g_curRomInfo.romheader.nCountryID,i);
			TextureToLoad = NULL;
		
			fp=fopen(TexturePath,"rb");
			if(fp){
				fread(TextureToLoad, sizeof(TxtrCacheEntry), 1, fp);
				fclose(fp);
			}
			DeleteFile(TexturePath);
			CloseHandle(TexturePath);

			AddTexture(TextureToLoad);
		}
		isComplete = 1;
	}
	return isComplete;
}
#endif
//
//  Delete all textures.
//
bool CTextureManager::CleanUp()
{
	RecycleAllTextures();

	while (m_pHead)
	{
		TxtrCacheEntry * pVictim = m_pHead;
		m_pHead = pVictim->pNext;

		delete pVictim;
	}

	if( m_blackTextureEntry.pTexture )		delete m_blackTextureEntry.pTexture;	
	if( m_PrimColorTextureEntry.pTexture )	delete m_PrimColorTextureEntry.pTexture;
	if( m_EnvColorTextureEntry.pTexture )	delete m_EnvColorTextureEntry.pTexture;
	if( m_LODFracTextureEntry.pTexture )	delete m_LODFracTextureEntry.pTexture;
	if( m_PrimLODFracTextureEntry.pTexture )	delete m_PrimLODFracTextureEntry.pTexture;
	memset(&m_blackTextureEntry, 0, sizeof(TxtrCacheEntry));
	memset(&m_PrimColorTextureEntry, 0, sizeof(TxtrCacheEntry));
	memset(&m_EnvColorTextureEntry, 0, sizeof(TxtrCacheEntry));
	memset(&m_LODFracTextureEntry, 0, sizeof(TxtrCacheEntry));
	memset(&m_PrimLODFracTextureEntry, 0, sizeof(TxtrCacheEntry));

#ifdef LIMIT_TEXTMEM
	m_currentTextureMemUsage = 0;
#endif
	return true;
}

// Purge any textures whos last usage was over 5 seconds ago
void CTextureManager::PurgeOldTextures()
{
	if (m_pCacheTxtrList == NULL)
		return;
#ifdef LIMIT_TEXTMEM
	// PurgeOldTextures breaks OOT and possibly others
	// Quake 2 needs it otherwise it leaks pretty bad. 
	if(!g_bUseSetTextureMem)
	{
		gTextureManager.CleanUp();
		m_currentTextureMemUsage = 0;
		bPurgeOldBeforeIGM = FALSE;
		return;
	}
	else if(bPurgeOldBeforeIGM)
	{
		bPurgeOldBeforeIGM = FALSE;
		//if(options.enableHackForGames == HACK_FOR_BANJO_TOOIE)
		//	return;
	}
	else if(options.enableHackForGames != HACK_FOR_QUAKE_2)
	{
		return;
	}
#endif
	
	static const uint32 dwFramesToKill = 5*30;			// 5 secs at 30 fps
	static const uint32 dwFramesToDelete = 30*30;		// 30 secs at 30 fps
	
	for ( uint32 i = 0; i < m_numOfCachedTxtrList; i++ )
	{
		TxtrCacheEntry * pEntry;
		TxtrCacheEntry * pNext;
		
		pEntry = m_pCacheTxtrList[i];
		while (pEntry)
		{
			pNext = pEntry->pNext;
			
			if ( status.gDlistCount - pEntry->FrameLastUsed > dwFramesToKill )
			{
				RemoveTexture(pEntry);
			}
			pEntry = pNext;
		}
	}
	
	
	// Remove any old textures that haven't been recycled in 1 minute or so
	// Normally these would be reused
	TxtrCacheEntry * pPrev;
	TxtrCacheEntry * pCurr;
	TxtrCacheEntry * pNext;
	
	
	pPrev = NULL;
	pCurr = m_pHead;
	
	while (pCurr)
	{
		pNext = pCurr->pNext;
		
		if (( status.gDlistCount - pCurr->FrameLastUsed > dwFramesToDelete )||(m_currentTextureMemUsage>=g_maxTextureMemUsage))
		{
			if (pPrev != NULL) pPrev->pNext        = pCurr->pNext;
			else			   m_pHead = pCurr->pNext;
			
#ifdef LIMIT_TEXTMEM
			m_currentTextureMemUsage -= (pCurr->pTexture->m_dwWidth * pCurr->pTexture->m_dwHeight * 2);
#endif
			delete pCurr;
			pCurr = pNext;	
		}
		else
		{
			pPrev = pCurr;
			pCurr = pNext;
		}
	}
}

void CTextureManager::RecycleAllTextures()
{
	if (m_pCacheTxtrList == NULL)
		return;
	
	uint32 dwCount = 0;
	uint32 dwTotalUses = 0;

#ifdef LIMIT_TEXTMEM
	m_pYoungestTexture			= NULL;
	m_pOldestTexture			= NULL;
#endif

	for (uint32 i = 0; i < m_numOfCachedTxtrList; i++)
	{
		while (m_pCacheTxtrList[i])
		{
			TxtrCacheEntry *pTVictim = m_pCacheTxtrList[i];
			m_pCacheTxtrList[i] = pTVictim->pNext;
			
			dwTotalUses += pTVictim->dwUses;
			dwCount++;
#ifdef LIMIT_TEXTMEM
			m_currentTextureMemUsage -= (pTVictim->pTexture->m_dwWidth * pTVictim->pTexture->m_dwHeight * 2);
			
			delete pTVictim;
#else
			RecycleTexture(pTVictim);
#endif
		}
	}
}


// Add to the recycle list
void CTextureManager::RecycleTexture(TxtrCacheEntry *pEntry)
{
#ifndef LIMIT_TEXTMEM
	if( CDeviceBuilder::GetGeneralDeviceType() == OGL_DEVICE )
	{
		// Fix me, why I can not reuse the texture in OpenGL,
		// how can I unload texture from video card memory for OpenGL
		delete pEntry;
		return;
	}

	if (pEntry->pTexture == NULL)
	{
		// No point in saving!
		delete pEntry;
	}
	else
	{
		// Add to the list
		pEntry->pNext = m_pHead;
		SAFE_DELETE(pEntry->pEnhancedTexture);
		m_pHead = pEntry;
	}
#else
return;
#endif
}

// Search for a texture of the specified dimensions to recycle
TxtrCacheEntry * CTextureManager::ReviveTexture( uint32 width, uint32 height )
{
#ifndef LIMIT_TEXTMEM
	TxtrCacheEntry * pPrev;
	TxtrCacheEntry * pCurr;
	
	pPrev = NULL;
	pCurr = m_pHead;
	
	while (pCurr)
	{
		if (pCurr->ti.WidthToCreate == width &&
			pCurr->ti.HeightToCreate == height)
		{
			// Remove from list
			if (pPrev != NULL) pPrev->pNext        = pCurr->pNext;
			else			   m_pHead = pCurr->pNext;
			
			return pCurr;
		}
		
		pPrev = pCurr;
		pCurr = pCurr->pNext;
	}
#endif
	return NULL;
}


uint32 CTextureManager::Hash(uint32 dwValue)
{
	// Divide by four, because most textures will be on a 4 byte boundry, so bottom four
	// bits are null
	return (dwValue>>2) % m_numOfCachedTxtrList;
}

void CTextureManager::MakeTextureYoungest(TxtrCacheEntry *pEntry)
{
	if (pEntry == m_pYoungestTexture)
		return;

	// if its the oldest, then change the oldest pointer
	if (pEntry == m_pOldestTexture)
	{
		m_pOldestTexture = pEntry->pNextYoungest;
	}

	// if its a not a new texture, close the gap in the age list
	// where pEntry use to reside
	if (pEntry->pNextYoungest != NULL || pEntry->pLastYoungest != NULL)
	{
		if (pEntry->pNextYoungest != NULL)
		{
			pEntry->pNextYoungest->pLastYoungest = pEntry->pLastYoungest;
		}
		if (pEntry->pLastYoungest != NULL)
		{
			pEntry->pLastYoungest->pNextYoungest = pEntry->pNextYoungest;
		}
	}

	// this texture is now the youngest, so place it on the end of the list
	if (m_pYoungestTexture != NULL)
	{
		m_pYoungestTexture->pNextYoungest = pEntry;
	}

	pEntry->pNextYoungest = NULL;
	pEntry->pLastYoungest = m_pYoungestTexture;
	m_pYoungestTexture = pEntry;
	 
	// if this is the first texture in memory then its also the oldest
	if (m_pOldestTexture == NULL)
	{
		m_pOldestTexture = pEntry;
	}
}

void CTextureManager::AddTexture(TxtrCacheEntry *pEntry)
{	
	uint32 dwKey = Hash(pEntry->ti.Address);
	
	if (m_pCacheTxtrList == NULL)
		return;
	
	TxtrCacheEntry **p = &m_pCacheTxtrList[dwKey];
	
	// Add to head (not tail, for speed - new textures are more likely to be accessed next)
	pEntry->pNext = m_pCacheTxtrList[dwKey];
	m_pCacheTxtrList[dwKey] = pEntry;
#ifdef LIMIT_TEXTMEM
	// Move the texture to the top of the age list
	MakeTextureYoungest(pEntry);
#endif
}



TxtrCacheEntry * CTextureManager::GetTxtrCacheEntry(TxtrInfo * pti)
{
	TxtrCacheEntry *pEntry;
	
	if (m_pCacheTxtrList == NULL)
		return NULL;
	
	// See if it is already in the hash table
	uint32 dwKey = Hash(pti->Address);

	for (pEntry = m_pCacheTxtrList[dwKey]; pEntry; pEntry = pEntry->pNext)
	{
		if ( pEntry->ti == *pti )
		{
#ifdef LIMIT_TEXTMEM
			MakeTextureYoungest(pEntry);
#endif
			return pEntry;
		}
	}

	return NULL;
}



void CTextureManager::RemoveTexture(TxtrCacheEntry * pEntry)
{
	TxtrCacheEntry * pPrev;
	TxtrCacheEntry * pCurr;
	
	if (m_pCacheTxtrList == NULL)
		return;
	
	// See if it is already in the hash table
	uint32 dwKey = Hash(pEntry->ti.Address);
	
	pPrev = NULL;
	pCurr = m_pCacheTxtrList[dwKey];
	
	while (pCurr)
	{
		// Check that the attributes match
		if ( pCurr->ti == pEntry->ti )
		{
			if (pPrev != NULL) pPrev->pNext = pCurr->pNext;
			else			   m_pCacheTxtrList[dwKey] = pCurr->pNext;
#ifdef LIMIT_TEXTMEM
			// remove the texture from the age list
			if (pEntry->pNextYoungest != NULL)
			{
				pEntry->pNextYoungest->pLastYoungest = pEntry->pLastYoungest;
			}
			if (pEntry->pLastYoungest != NULL)
			{
				pEntry->pLastYoungest->pNextYoungest = pEntry->pNextYoungest;
			}
			m_currentTextureMemUsage -= (pEntry->pTexture->m_dwWidth * pEntry->pTexture->m_dwHeight * 2);
			delete pEntry;
#endif
			break;
		}

		pPrev = pCurr;
		pCurr = pCurr->pNext;
	}
#ifndef LIMIT_TEXTMEM
	RecycleTexture(pEntry);
#endif
}

#ifdef LIMIT_TEXTMEM
void CTextureManager::FreeTextures()
{
	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);

	// Clear all textures if memory is low
	if (ms.dwAvailPhys < MEM_KEEP_FREE)
	{
		gTextureManager.PurgeOldTextures();
		gTextureManager.CleanUp();
		m_currentTextureMemUsage = 0;
	}
	
}
#endif

TxtrCacheEntry * CTextureManager::CreateNewCacheEntry(uint32 dwAddr, uint32 dwWidth, uint32 dwHeight)
{
	TxtrCacheEntry * pEntry = NULL;

	// Find a used texture
#ifdef LIMIT_TEXTMEM
	uint32 widthToCreate = dwWidth;
	uint32 heightToCreate = dwHeight;

	DWORD freeUpSize = (widthToCreate * heightToCreate * 2);

	FreeTextures();

	// make sure there is enough room for the new texture by deleting old textures
	if((g_bUseSetTextureMem) && ((m_currentTextureMemUsage + freeUpSize) > g_maxTextureMemUsage))
	{
		while ((m_currentTextureMemUsage + freeUpSize) > g_maxTextureMemUsage && m_pOldestTexture != NULL)
		{
			TxtrCacheEntry *nextYoungest = m_pOldestTexture->pNextYoungest;

			RemoveTexture(m_pOldestTexture);

			m_pOldestTexture = nextYoungest;

			//OutputDebugString("Freeing Texture\n");
		}
	}
	else if((!g_bUseSetTextureMem) && ((m_currentTextureMemUsage + freeUpSize) > g_maxTextureMemUsage) && (options.enableHackForGames != HACK_FOR_QUAKE_2))
	{
		gTextureManager.CleanUp();
		m_currentTextureMemUsage = 0;
	}
	else
	{
		pEntry = ReviveTexture(dwWidth, dwHeight);
	}
	m_currentTextureMemUsage += (dwWidth * dwHeight * 2);
	
	if (pEntry == NULL || g_bUseSetTextureMem)
#else
	pEntry = ReviveTexture(dwWidth, dwHeight);
	if (pEntry == NULL)
#endif
	{
		// Couldn't find on - recreate!
		pEntry = new TxtrCacheEntry;
		if (pEntry == NULL)
			return NULL;

		pEntry->pTexture = CDeviceBuilder::GetBuilder()->CreateTexture(dwWidth, dwHeight);
		if (pEntry->pTexture == NULL || pEntry->pTexture->GetTexture() == NULL)
		{
		}
		else
		{
			pEntry->pTexture->m_bScaledS = false;
			pEntry->pTexture->m_bScaledT = false;
		}
	}
	
	// Initialize
	pEntry->ti.Address = dwAddr;
	pEntry->pNext = NULL;
#ifdef LIMIT_TEXTMEM
	pEntry->pNextYoungest = NULL;
	pEntry->pLastYoungest = NULL;
#endif
	pEntry->dwUses = 0;
	pEntry->dwTimeLastUsed = status.gRDPTime;
	pEntry->dwCRC = 0;
	pEntry->FrameLastUsed = status.gDlistCount;
	pEntry->FrameLastUpdated = 0;
	pEntry->lastEntry = NULL;
	pEntry->maxCI = -1;

	// Add to the hash table
	AddTexture(pEntry);
	return pEntry;	
}

// If already in table, return
// Otherwise, create surfaces, and load texture into memory
uint32 dwAsmHeight;
uint32 dwAsmPitch;
uint32 dwAsmdwBytesPerLine;
uint32 dwAsmCRC;
uint8* pAsmStart;

TxtrCacheEntry *g_lastTextureEntry=NULL;
bool lastEntryModified = false;

extern int CheckAndSaveBackBuffer(uint32 addr, uint32 memsize, bool copyToRDRAM = false);
extern uint32 CalculateRDRAMCRC(void *pAddr, uint32 left, uint32 top, uint32 width, uint32 height, uint32 size, uint32 pitchInBytes );

TxtrCacheEntry * CTextureManager::GetTexture(TxtrInfo * pgti, bool fromTMEM, bool doCRCCheck, bool AutoExtendTexture)
{
	TxtrCacheEntry *pEntry;

	if( g_curRomInfo.bDisableTextureCRC )
		doCRCCheck = false;

	gRDP.texturesAreReloaded = true;

	dwAsmCRC = 0;
	uint32 dwPalCRC = 0;

	pEntry = GetTxtrCacheEntry(pgti);
	bool loadFromTextureBuffer=false;
	int txtBufIdxToLoadFrom = -1;
	if( (frameBufferOptions.bCheckTxtBufs&&!frameBufferOptions.bWriteBackBufToRDRAM) || (frameBufferOptions.bCheckBackBufs&&!frameBufferOptions.bWriteBackBufToRDRAM) )
	{
#ifdef _RICE6FB
		txtBufIdxToLoadFrom = g_pFrameBufferManager->CheckAddrInRenderTextures(pgti->Address);
#else
		txtBufIdxToLoadFrom = CGraphicsContext::g_pGraphicsContext->CheckAddrInTxtrBufs(*pgti);
#endif
		if( txtBufIdxToLoadFrom >= 0 )
		{
			loadFromTextureBuffer = true;
			// Check if it is the same size,
			TextureBufferInfo &info = gTextureBufferInfos[txtBufIdxToLoadFrom];
			//if( info.pTxtBuffer && info.CI_Info.dwAddr == pgti->Address && info.CI_Info.dwFormat == pgti->Format 
			if( info.pTxtBuffer && info.CI_Info.dwFormat == pgti->Format 
				&& info.CI_Info.dwSize == pgti->Size )
			{
				info.txtEntry.ti = *pgti;
				return &info.txtEntry;
			}
		}
	}

	bool loadFromBackBuffer=false;
#ifdef _RICE6FB
	if( frameBufferOptions.bCheckBackBufs && g_pFrameBufferManager->CheckAddrInBackBuffers(pgti->Address, pgti->HeightToLoad*pgti->Pitch) >= 0 )
#else
	if( frameBufferOptions.bCheckBackBufs && CheckAndSaveBackBuffer(pgti->Address, pgti->HeightToLoad*pgti->Pitch) >= 0 )
#endif
	{
		if( !frameBufferOptions.bWriteBackBufToRDRAM )
		{
			// Load the texture from recent back buffer
			loadFromBackBuffer = true;
#ifdef _RICE6FB
			txtBufIdxToLoadFrom = g_pFrameBufferManager->CheckAddrInRenderTextures(pgti->Address);
#else
			txtBufIdxToLoadFrom = CGraphicsContext::g_pGraphicsContext->CheckAddrInTxtrBufs(*pgti);
#endif
			if( txtBufIdxToLoadFrom >= 0 )
			{
				loadFromTextureBuffer = true;
				// Check if it is the same size,
				TextureBufferInfo &info = gTextureBufferInfos[txtBufIdxToLoadFrom];
				//if( info.pTxtBuffer && info.CI_Info.dwAddr == pgti->Address && info.CI_Info.dwFormat == pgti->Format 
				if( info.pTxtBuffer && info.CI_Info.dwFormat == pgti->Format 
					&& info.CI_Info.dwSize == pgti->Size )
				{
					info.txtEntry.ti = *pgti;
					return &info.txtEntry;
				}
			}
		}
	}

	if (pEntry && pEntry->dwTimeLastUsed == status.gRDPTime && status.gDlistCount != 0 && !status.bN64FrameBufferIsUsed )		// This is not good, Palatte may changes
	{
		// We've already calculated a CRC this frame!
		dwAsmCRC = pEntry->dwCRC;
	}
	else
	{
		if ( doCRCCheck && loadFromTextureBuffer )
		{
			dwAsmCRC = gTextureBufferInfos[txtBufIdxToLoadFrom].crcInRDRAM;
		}
		else if ( doCRCCheck)
		{
			CalculateRDRAMCRC(pgti->pPhysicalAddress, pgti->LeftToLoad, pgti->TopToLoad, pgti->WidthToLoad, pgti->HeightToLoad, pgti->Size, pgti->Pitch);
		}
	}

	int maxCI = 0;
	if ( doCRCCheck && (pgti->Format == TXT_FMT_CI || (pgti->Format == TXT_FMT_RGBA && pgti->Size <= TXT_SIZE_8b )))
	{
#ifdef _RICE6FB		
		//maxCI = pgti->Size == TXT_SIZE_8b ? 255 : 15;
		extern BYTE CalculateMaxCI(void *pPhysicalAddress, uint32 left, uint32 top, uint32 width, uint32 height, uint32 size, uint32 pitchInBytes );

		if( !pEntry || pEntry->dwCRC != dwAsmCRC || pEntry->maxCI < 0 )
		{
			maxCI = CalculateMaxCI(pgti->pPhysicalAddress, pgti->LeftToLoad, pgti->TopToLoad, pgti->WidthToLoad, pgti->HeightToLoad, pgti->Size, pgti->Pitch);
		}
		else
		{
			maxCI = pEntry->maxCI;
		}
#endif

		//Check PAL CRC
		uint8 * pStart;
		uint32 dwPalSize = 16;
		uint32 dwOffset, y;
		if( pgti->Size == TXT_SIZE_8b )
		{
			dwPalSize = 256;
			dwOffset = 0;
		}
		else
		{
			dwOffset = pgti->Palette << 4;
		}

		pStart = (uint8*)pgti->PalAddress+dwOffset*2;
#ifndef _RICE612_CRC
		for (y = 0; y < dwPalSize*2; y+=4)
		{
			dwPalCRC = (dwPalCRC + *(uint32*)&pStart[y]);
		}
#endif
		uint32 dwAsmCRCSave = dwAsmCRC;
		//dwPalCRC = CalculateRDRAMCRC(pStart, 0, 0, dwPalSize, 1, TXT_SIZE_16b, dwPalSize*2);
		dwPalCRC = CalculateRDRAMCRC(pStart, 0, 0, maxCI+1, 1, TXT_SIZE_16b, dwPalSize*2);
		dwAsmCRC = dwAsmCRCSave;
	}

	if (pEntry && doCRCCheck )
	{
		if(pEntry->dwCRC == dwAsmCRC && pEntry->dwPalCRC == dwPalCRC &&
			(!loadFromTextureBuffer || gTextureBufferInfos[txtBufIdxToLoadFrom].updateAtFrame < pEntry->FrameLastUsed ) )
		{
			// Tile is ok, return
			pEntry->dwUses++;
			pEntry->dwTimeLastUsed = status.gRDPTime;
			pEntry->FrameLastUsed = status.gDlistCount;
			pEntry->lastEntry = g_lastTextureEntry;
			g_lastTextureEntry = pEntry;
			lastEntryModified = false;

			return pEntry;
		}
		else
		{
			; //Do something
		}
	}

	if (pEntry == NULL)
	{
		// We need to create a new entry, and add it
		//  to the hash table.
		pEntry = CreateNewCacheEntry(pgti->Address, pgti->WidthToCreate, pgti->HeightToCreate);

		if (pEntry == NULL)
		{
			g_lastTextureEntry = pEntry;
			return NULL;
		}
	}

	pEntry->ti = *pgti;
	pEntry->dwCRC = dwAsmCRC;
	pEntry->dwPalCRC = dwPalCRC;
	pEntry->maxCI = maxCI;

#ifndef _RICE612_FIX
	if( pEntry->pTexture->m_dwCreatedTextureWidth < pgti->WidthToCreate )
	{
		pEntry->ti.WidthToLoad = pEntry->pTexture->m_dwCreatedTextureWidth;
		pEntry->pTexture->m_bScaledS = false;
		pEntry->pTexture->m_bScaledT = false;
	}
	if( pEntry->pTexture->m_dwCreatedTextureHeight < pgti->HeightToCreate )
	{
		pEntry->ti.HeightToLoad = pEntry->pTexture->m_dwCreatedTextureHeight;
		pEntry->pTexture->m_bScaledT = false;
		pEntry->pTexture->m_bScaledS = false;
	}
#endif

	try 
	{
		if (pEntry->pTexture != NULL)
		{
#ifdef _RICE612_FIX		
				if( pEntry->pTexture->m_dwCreatedTextureWidth < pgti->WidthToCreate )
				{
					pEntry->ti.WidthToLoad = pEntry->pTexture->m_dwCreatedTextureWidth;
					pEntry->pTexture->m_bScaledS = false;
					pEntry->pTexture->m_bScaledT = false;
				}
				if( pEntry->pTexture->m_dwCreatedTextureHeight < pgti->HeightToCreate )
				{
					pEntry->ti.HeightToLoad = pEntry->pTexture->m_dwCreatedTextureHeight;
					pEntry->pTexture->m_bScaledT = false;
					pEntry->pTexture->m_bScaledS = false;
				}
#endif
			TextureFmt dwType = pEntry->pTexture->GetSurfaceFormat();
			SAFE_DELETE(pEntry->pEnhancedTexture);
			pEntry->dwEnhancementFlag = TEXTURE_NO_ENHANCEMENT;

			if (dwType != TEXTURE_FMT_UNKNOWN)
			{
				if( loadFromTextureBuffer )
				{
#ifdef _RICE6FB
					g_pFrameBufferManager->LoadTextureFromRenderTexture(pEntry, txtBufIdxToLoadFrom);
#else
					CGraphicsContext::g_pGraphicsContext->LoadTextureFromTextureBuffer(pEntry, txtBufIdxToLoadFrom);
#endif

					extern void ConvertTextureRGBAtoI(TxtrCacheEntry* pEntry, bool alpha);
					if( g_pTxtBufferInfo->CI_Info.dwFormat == TXT_FMT_I )
					{
						// Convert texture from RGBA to I
						ConvertTextureRGBAtoI(pEntry,false);
					}
					else if( g_pTxtBufferInfo->CI_Info.dwFormat == TXT_FMT_IA )
					{
						// Convert texture from RGBA to IA
						ConvertTextureRGBAtoI(pEntry,true);
					}
				}
				else
				{
					if (dwType == TEXTURE_FMT_A8R8G8B8)
						ConvertTexture(pEntry, fromTMEM);		
					else
						ConvertTexture_16(pEntry, fromTMEM);
					pEntry->FrameLastUpdated = status.gDlistCount;
					SAFE_DELETE(pEntry->pEnhancedTexture);
					pEntry->dwEnhancementFlag = TEXTURE_NO_ENHANCEMENT;
				}
			}

			pEntry->ti.WidthToLoad = pgti->WidthToLoad;
			pEntry->ti.HeightToLoad = pgti->HeightToLoad;
			
			if( AutoExtendTexture )
			{
				ExpandTextureS(pEntry);
				ExpandTextureT(pEntry);
			}

		}
	}
	catch (...)
	{
		g_lastTextureEntry = NULL;
		return NULL;
	}

	pEntry->lastEntry = g_lastTextureEntry;
	g_lastTextureEntry = pEntry;
	lastEntryModified = true;
	return pEntry;
}




char *pszImgFormat[8] = {"RGBA", "YUV", "CI", "IA", "I", "?1", "?2", "?3"};
uint8 pnImgSize[4]   = {4, 8, 16, 32};
char *textlutname[4] = {"RGB16", "I16?", "RGBA16", "IA16"};

extern uint16 g_wRDPTlut[];
extern ConvertFunction	gConvertFunctions_FullTMEM[ 8 ][ 4 ];
extern ConvertFunction	gConvertFunctions[ 8 ][ 4 ];
extern ConvertFunction	gConvertTlutFunctions[ 8 ][ 4 ];
extern ConvertFunction	gConvertFunctions_16[ 8 ][ 4 ];
extern ConvertFunction	gConvertFunctions_16_FullTMEM[ 8 ][ 4 ];
extern ConvertFunction	gConvertTlutFunctions_16[ 8 ][ 4 ];
void CTextureManager::ConvertTexture(TxtrCacheEntry * pEntry, bool fromTMEM)
{
	static uint32 dwCount = 0;
	
	ConvertFunction pF;
	if( options.bUseFullTMEM && fromTMEM && status.bAllowLoadFromTMEM )
	{
		pF = gConvertFunctions_FullTMEM[ pEntry->ti.Format ][ pEntry->ti.Size ];
	}
	else
	{
		if( gRDP.tiles[7].dwFormat == TXT_FMT_YUV )
		{
			if( gRDP.otherMode.text_tlut>=2 )
				pF = gConvertTlutFunctions[ TXT_FMT_YUV ][ pEntry->ti.Size ];
			else
				pF = gConvertFunctions[ TXT_FMT_YUV ][ pEntry->ti.Size ];
		}
		else
		{
			if( gRDP.otherMode.text_tlut>=2 )
				pF = gConvertTlutFunctions[ pEntry->ti.Format ][ pEntry->ti.Size ];
			else
				pF = gConvertFunctions[ pEntry->ti.Format ][ pEntry->ti.Size ];
		}
	}

	if( pF )
	{
		pF( pEntry->pTexture, pEntry->ti );
	
	}
	else
	{
	}

	dwCount++;
}

void CTextureManager::ConvertTexture_16(TxtrCacheEntry * pEntry, bool fromTMEM)
{
	static uint32 dwCount = 0;
	
	ConvertFunction pF;

	if( options.bUseFullTMEM && fromTMEM && status.bAllowLoadFromTMEM )
	{
		pF = gConvertFunctions_16_FullTMEM[ pEntry->ti.Format ][ pEntry->ti.Size ];
	}
	else
	{
		if( gRDP.otherMode.text_tlut>=2 )
			pF = gConvertTlutFunctions_16[ pEntry->ti.Format ][ pEntry->ti.Size ];
		else
			pF = gConvertFunctions_16[ pEntry->ti.Format ][ pEntry->ti.Size ];
	}

	if( pF )
	{
		pF( pEntry->pTexture, pEntry->ti );

	}
	else
	{
	}

	dwCount++;
}

#define absi(x)		((x)>=0?(x):(-x))
#define S_FLAG	0
#define T_FLAG	1
void CTextureManager::ExpandTexture(TxtrCacheEntry * pEntry, uint32 sizeToLoad, uint32 sizeToCreate, uint32 sizeCreated,
	int arrayWidth, int flag, int mask, int mirror, int clamp, uint32 otherSize)
{
	if( sizeToLoad >= sizeCreated )	return;

	uint32 maskWidth = (1<<mask);
	int size = pEntry->pTexture->GetPixelSize();

	// Doing Mirror And/Or Wrap in S direction
	// Image has been loaded with width=WidthToLoad, we need to enlarge the image
	// to width = pEntry->ti.WidthToCreate by doing mirroring or wrapping

	DrawInfo di;
	if( !(pEntry->pTexture->StartUpdate(&di)) )
	{
		return;
	}


	if( mask == 0 )
	{
		// Clamp
		Clamp(di.lpSurface, sizeToLoad, sizeCreated, arrayWidth, otherSize, 
			flag, size);
		pEntry->pTexture->EndUpdate(&di);
		return;
	}

	if( sizeToLoad == maskWidth )
	{
		uint32 tempwidth = clamp ? sizeToCreate : sizeCreated;
		if( mirror )
		{
			Mirror(di.lpSurface, sizeToLoad, mask, tempwidth,
				arrayWidth, otherSize, flag, size );
		}
		else
		{
			Wrap(di.lpSurface, sizeToLoad, mask, tempwidth,
				arrayWidth, otherSize, flag, size );
		}

		if( tempwidth < sizeCreated )
		{
			Clamp(di.lpSurface, tempwidth, sizeCreated, arrayWidth, otherSize, 
				flag, size );
		}

		pEntry->pTexture->EndUpdate(&di);
		return;
	}


	if( sizeToLoad < sizeToCreate && sizeToCreate == maskWidth && maskWidth == sizeCreated )
	{
		// widthToLoad < widthToCreate = maskWidth
		Wrap(di.lpSurface, sizeToLoad, mask, sizeCreated, arrayWidth, otherSize, flag, size );

		pEntry->pTexture->EndUpdate(&di);
		return;
	}

	if( sizeToLoad == sizeToCreate && sizeToCreate < maskWidth )
	{
		Clamp(di.lpSurface, sizeToLoad, sizeCreated, arrayWidth, otherSize, flag, size );

		pEntry->pTexture->EndUpdate(&di);
		return;
	}

	if( sizeToLoad < sizeToCreate && sizeToCreate < maskWidth )
	{
		Clamp(di.lpSurface, sizeToLoad, sizeCreated, arrayWidth, otherSize, flag, size );
		pEntry->pTexture->EndUpdate(&di);
		return;
	}

	pEntry->pTexture->EndUpdate(&di);
}

void CTextureManager::ExpandTextureS(TxtrCacheEntry * pEntry)
{
	TxtrInfo &ti =  pEntry->ti;
	uint32 textureWidth = pEntry->pTexture->m_dwCreatedTextureWidth;
	ExpandTexture(pEntry, ti.WidthToLoad, ti.WidthToCreate, textureWidth, 
		textureWidth, S_FLAG, ti.maskS, ti.mirrorS, ti.clampS, ti.HeightToLoad);
}

void CTextureManager::ExpandTextureT(TxtrCacheEntry * pEntry)
{
	TxtrInfo &ti =  pEntry->ti;
	uint32 textureHeight = pEntry->pTexture->m_dwCreatedTextureHeight;
	uint32 textureWidth = pEntry->pTexture->m_dwCreatedTextureWidth;
	ExpandTexture(pEntry, ti.HeightToLoad, ti.HeightToCreate, textureHeight,
		textureWidth, T_FLAG, ti.maskT, ti.mirrorT, ti.clampT, ti.WidthToLoad);
}


void CTextureManager::ClampS32(uint32 *array, uint32 width, uint32 towidth, uint32 arrayWidth, uint32 rows)
{
	for( uint32 y = 0; y<rows; y++ )
	{
		uint32* line = array+y*arrayWidth;
		uint32 val = line[width-1];
		for( uint32 x=width; x<towidth; x++ )
		{
			line[x] = val;
		}
	}
}

void CTextureManager::ClampS16(uint16 *array, uint32 width, uint32 towidth, uint32 arrayWidth, uint32 rows)
{
	for( uint32 y = 0; y<rows; y++ )
	{
		uint16* line = array+y*arrayWidth;
		uint16 val = line[width-1];
		for( uint32 x=width; x<towidth; x++ )
		{
			line[x] = val;
		}
	}
}

void CTextureManager::ClampT32(uint32 *array, uint32 height, uint32 toheight, uint32 arrayWidth, uint32 cols)
{
	uint32* linesrc = array+arrayWidth*(height-1);
	for( uint32 y = height; y<toheight; y++ )
	{
		uint32* linedst = array+arrayWidth*y;
		for( uint32 x=0; x<arrayWidth; x++ )
		{
			linedst[x] = linesrc[x];
		}
	}
}

void CTextureManager::ClampT16(uint16 *array, uint32 height, uint32 toheight, uint32 arrayWidth, uint32 cols)
{
	uint16* linesrc = array+arrayWidth*(height-1);
	for( uint32 y = height; y<toheight; y++ )
	{
		uint16* linedst = array+arrayWidth*y;
		for( uint32 x=0; x<arrayWidth; x++ )
		{
			linedst[x] = linesrc[x];
		}
	}
}

void CTextureManager::MirrorS32(uint32 *array, uint32 width, uint32 mask, uint32 towidth, uint32 arrayWidth, uint32 rows)
{
	uint32 maskval1 = (1<<mask)-1;
	uint32 maskval2 = (1<<(mask+1))-1;

	for( uint32 y = 0; y<rows; y++ )
	{
		uint32* line = array+y*arrayWidth;
		for( uint32 x=width; x<towidth; x++ )
		{
			line[x] = (x&maskval2)<=maskval1 ? line[x&maskval1] : line[maskval2-(x&maskval2)];
		}
	}
}

void CTextureManager::MirrorS16(uint16 *array, uint32 width, uint32 mask, uint32 towidth, uint32 arrayWidth, uint32 rows)
{
	uint32 maskval1 = (1<<mask)-1;
	uint32 maskval2 = (1<<(mask+1))-1;

	for( uint32 y = 0; y<rows; y++ )
	{
		uint16* line = array+y*arrayWidth;
		for( uint32 x=width; x<towidth; x++ )
		{
			line[x] = (x&maskval2)<=maskval1 ? line[x&maskval1] : line[maskval2-(x&maskval2)];
		}
	}
}

void CTextureManager::MirrorT32(uint32 *array, uint32 height, uint32 mask, uint32 toheight, uint32 arrayWidth, uint32 cols)
{
	uint32 maskval1 = (1<<mask)-1;
	uint32 maskval2 = (1<<(mask+1))-1;

	for( uint32 y = height; y<toheight; y++ )
	{
		uint32 srcy = (y&maskval2)<=maskval1 ? y&maskval1 : maskval2-(y&maskval2);
		uint32* linesrc = array+arrayWidth*srcy;
		uint32* linedst = array+arrayWidth*y;;
		for( uint32 x=0; x<arrayWidth; x++ )
		{
			linedst[x] = linesrc[x];
		}
	}
}

void CTextureManager::MirrorT16(uint16 *array, uint32 height, uint32 mask, uint32 toheight, uint32 arrayWidth, uint32 cols)
{
	uint32 maskval1 = (1<<mask)-1;
	uint32 maskval2 = (1<<(mask+1))-1;

	for( uint32 y = height; y<toheight; y++ )
	{
		uint32 srcy = (y&maskval2)<=maskval1 ? y&maskval1 : maskval2-(y&maskval2);
		uint16* linesrc = array+arrayWidth*srcy;
		uint16* linedst = array+arrayWidth*y;;
		for( uint32 x=0; x<arrayWidth; x++ )
		{
			linedst[x] = linesrc[x];
		}
	}
}

void CTextureManager::WrapS32(uint32 *array, uint32 width, uint32 mask, uint32 towidth, uint32 arrayWidth, uint32 rows)
{
	uint32 maskval = (1<<mask)-1;

	for( uint32 y = 0; y<rows; y++ )
	{
		uint32* line = array+y*arrayWidth;
		for( uint32 x=width; x<towidth; x++ )
		{
			line[x] = line[(x&maskval)<width?(x&maskval):towidth-(x&maskval)];
		}
	}
}

void CTextureManager::WrapS16(uint16 *array, uint32 width, uint32 mask, uint32 towidth, uint32 arrayWidth, uint32 rows)
{
	uint32 maskval = (1<<mask)-1;

	for( uint32 y = 0; y<rows; y++ )
	{
		uint16* line = array+y*arrayWidth;
		for( uint32 x=width; x<towidth; x++ )
		{
			line[x] = line[(x&maskval)<width?(x&maskval):towidth-(x&maskval)];
		}
	}
}

void CTextureManager::WrapT32(uint32 *array, uint32 height, uint32 mask, uint32 toheight, uint32 arrayWidth, uint32 cols)
{
	uint32 maskval = (1<<mask)-1;
	for( uint32 y = height; y<toheight; y++ )
	{
		uint32* linesrc = array+arrayWidth*(y>maskval?y&maskval:y-height);
		uint32* linedst = array+arrayWidth*y;;
		for( uint32 x=0; x<arrayWidth; x++ )
		{
			linedst[x] = linesrc[x];
		}
	}
}

void CTextureManager::WrapT16(uint16 *array, uint32 height, uint32 mask, uint32 toheight, uint32 arrayWidth, uint32 cols)
{
	uint32 maskval = (1<<mask)-1;
	for( uint32 y = height; y<toheight; y++ )
	{
		uint16* linesrc = array+arrayWidth*(y>maskval?y&maskval:y-height);
		uint16* linedst = array+arrayWidth*y;;
		for( uint32 x=0; x<arrayWidth; x++ )
		{
			linedst[x] = linesrc[x];
		}
	}
}

void CTextureManager::Clamp(void *array, uint32 width, uint32 towidth, uint32 arrayWidth, uint32 rows, int flag, int size )
{
	if( flag == S_FLAG )	// s
	{
		if( size == 4 )	// 32 bit
		{
			ClampS32((uint32*)array, width, towidth, arrayWidth, rows);
		}
		else	// 16 bits
		{
			ClampS16((uint16*)array, width, towidth, arrayWidth, rows);
		}
	}
	else	// t
	{
		if( size == 4 )	// 32 bit
		{
			ClampT32((uint32*)array, width, towidth, arrayWidth, rows);
		}
		else	// 16 bits
		{
			ClampT16((uint16*)array, width, towidth, arrayWidth, rows);
		}
	}
}
void CTextureManager::Wrap(void *array, uint32 width, uint32 mask, uint32 towidth, uint32 arrayWidth, uint32 rows, int flag, int size )
{
	if( flag == S_FLAG )	// s
	{
		if( size == 4 )	// 32 bit
		{
			WrapS32((uint32*)array, width, mask, towidth, arrayWidth, rows);
		}
		else	// 16 bits
		{
			WrapS16((uint16*)array, width, mask, towidth, arrayWidth, rows);
		}
	}
	else	// t
	{
		if( size == 4 )	// 32 bit
		{
			WrapT32((uint32*)array, width, mask, towidth, arrayWidth, rows);
		}
		else	// 16 bits
		{
			WrapT16((uint16*)array, width, mask, towidth, arrayWidth, rows);
		}
	}
}
void CTextureManager::Mirror(void *array, uint32 width, uint32 mask, uint32 towidth, uint32 arrayWidth, uint32 rows, int flag, int size )
{
	if( flag == S_FLAG )	// s
	{
		if( size == 4 )	// 32 bit
		{
			MirrorS32((uint32*)array, width, mask, towidth, arrayWidth, rows);
		}
		else	// 16 bits
		{
			MirrorS16((uint16*)array, width, mask, towidth, arrayWidth, rows);
		}
	}
	else	// t
	{
		if( size == 4 )	// 32 bit
		{
			MirrorT32((uint32*)array, width, mask, towidth, arrayWidth, rows);
		}
		else	// 16 bits
		{
			MirrorT16((uint16*)array, width, mask, towidth, arrayWidth, rows);
		}
	}

}

TxtrCacheEntry * CTextureManager::GetBlackTexture(void)
{
	if( m_blackTextureEntry.pTexture == NULL )
	{
		m_blackTextureEntry.pTexture = CDeviceBuilder::GetBuilder()->CreateTexture(4, 4);
		m_blackTextureEntry.ti.WidthToCreate = 4;
		m_blackTextureEntry.ti.HeightToCreate = 4;
		updateColorTexture(m_blackTextureEntry.pTexture,0x00000000);
	}
	return &m_blackTextureEntry;
}
TxtrCacheEntry * CTextureManager::GetPrimColorTexture(uint32 color)
{
	static uint32 mcolor = 0;
	if( m_PrimColorTextureEntry.pTexture == NULL )
	{
		m_PrimColorTextureEntry.pTexture = CDeviceBuilder::GetBuilder()->CreateTexture(4, 4);
		m_PrimColorTextureEntry.ti.WidthToCreate = 4;
		m_PrimColorTextureEntry.ti.HeightToCreate = 4;
		updateColorTexture(m_PrimColorTextureEntry.pTexture,color);
		gRDP.texturesAreReloaded = true;
	}
	else if( mcolor != color )
	{
		updateColorTexture(m_PrimColorTextureEntry.pTexture,color);
		gRDP.texturesAreReloaded = true;
	}

	mcolor = color;
	return &m_PrimColorTextureEntry;
}
TxtrCacheEntry * CTextureManager::GetEnvColorTexture(uint32 color)
{
	static uint32 mcolor = 0;
	if( m_EnvColorTextureEntry.pTexture == NULL )
	{
		m_EnvColorTextureEntry.pTexture = CDeviceBuilder::GetBuilder()->CreateTexture(4, 4);
		m_EnvColorTextureEntry.ti.WidthToCreate = 4;
		m_EnvColorTextureEntry.ti.HeightToCreate = 4;
		gRDP.texturesAreReloaded = true;

		updateColorTexture(m_EnvColorTextureEntry.pTexture,color);
	}
	else if( mcolor != color )
	{
		updateColorTexture(m_EnvColorTextureEntry.pTexture,color);
		gRDP.texturesAreReloaded = true;
	}

	mcolor = color;
	return &m_EnvColorTextureEntry;
}
TxtrCacheEntry * CTextureManager::GetLODFracTexture(uint8 fac)
{
	static uint8 mfac = 0;
	if( m_LODFracTextureEntry.pTexture == NULL )
	{
		m_LODFracTextureEntry.pTexture = CDeviceBuilder::GetBuilder()->CreateTexture(4, 4);
		m_LODFracTextureEntry.ti.WidthToCreate = 4;
		m_LODFracTextureEntry.ti.HeightToCreate = 4;
		uint32 factor = fac;
		uint32 color = fac;
		color |= factor << 8;
		color |= color << 16;
		updateColorTexture(m_LODFracTextureEntry.pTexture,color);
		gRDP.texturesAreReloaded = true;
	}
	else if( mfac != fac )
	{
		uint32 factor = fac;
		uint32 color = fac;
		color |= factor << 8;
		color |= color << 16;
		updateColorTexture(m_LODFracTextureEntry.pTexture,color);
		gRDP.texturesAreReloaded = true;
	}

	mfac = fac;
	return &m_LODFracTextureEntry;
}

TxtrCacheEntry * CTextureManager::GetPrimLODFracTexture(uint8 fac)
{
	static uint8 mfac = 0;
	if( m_PrimLODFracTextureEntry.pTexture == NULL )
	{
		m_PrimLODFracTextureEntry.pTexture = CDeviceBuilder::GetBuilder()->CreateTexture(4, 4);
		m_PrimLODFracTextureEntry.ti.WidthToCreate = 4;
		m_PrimLODFracTextureEntry.ti.HeightToCreate = 4;
		uint32 factor = fac;
		uint32 color = fac;
		color |= factor << 8;
		color |= color << 16;
		updateColorTexture(m_PrimLODFracTextureEntry.pTexture,color);
		gRDP.texturesAreReloaded = true;
	}
	else if( mfac != fac )
	{
		uint32 factor = fac;
		uint32 color = fac;
		color |= factor << 8;
		color |= color << 16;
		updateColorTexture(m_PrimLODFracTextureEntry.pTexture,color);
		gRDP.texturesAreReloaded = true;
	}

	mfac = fac;
	return &m_PrimLODFracTextureEntry;
}

TxtrCacheEntry * CTextureManager::GetConstantColorTexture(uint32 constant)
{
	switch( constant )
	{
	case MUX_PRIM:
		return GetPrimColorTexture(gRDP.primitiveColor);
		break;
	case MUX_ENV:
		return GetEnvColorTexture(gRDP.envColor);
		break;
	case MUX_LODFRAC:
		return GetLODFracTexture((uint8)gRDP.LODFrac);
		break;
	default:	// MUX_PRIMLODFRAC
		return GetPrimLODFracTexture((uint8)gRDP.primLODFrac);
		break;
	}
}

void CTextureManager::updateColorTexture(CTexture *ptexture, uint32 color)
{
	DrawInfo di;
	if( !(ptexture->StartUpdate(&di)) )
	{
		return;
	}

	int size = ptexture->GetPixelSize();
	switch( size )
	{
	case 2:	// 16 bits
		{
			uint16 *buf = (uint16*)di.lpSurface;
			uint16 color16= (uint16)((color>>4)&0xF);
			color16 |= ((color>>12)&0xF)<<4;
			color16 |= ((color>>20)&0xF)<<8;
			color16 |= ((color>>28)&0xF)<<12;
			for( int i=0; i<16; i++ )
			{
				buf[i] = color16;
			}
		}
		break;
	case 4: // 32 bits
		{
			uint32 *buf = (uint32*)di.lpSurface;
			for( int i=0; i<16; i++ )
			{
				buf[i] = color;
			}
		}
		break;
	}

	ptexture->EndUpdate(&di);
}

void ConvertTextureRGBAtoI(TxtrCacheEntry* pEntry, bool alpha)
{
	DrawInfo srcInfo;	
	if( pEntry->pTexture->StartUpdate(&srcInfo) )
	{
		uint32 *buf;
		uint32 val;
		uint32 r,g,b,a,i;

		for(int nY = 0; nY < srcInfo.dwCreatedHeight; nY++)
		{
			buf = (uint32*)((uint8*)srcInfo.lpSurface+nY*srcInfo.lPitch);
			for(int nX = 0; nX < srcInfo.dwCreatedWidth; nX++)
			{
				val = buf[nX];
				b = (val>>0)&0xFF;
				g = (val>>8)&0xFF;
				r = (val>>16)&0xFF;
				i = (r+g+b)/3;
				a = alpha?(val&0xFF000000):(i<<24);
				buf[nX] = (a|(i<<16)|(i<<8)|i);
			}
		}
		pEntry->pTexture->EndUpdate(&srcInfo);	}
}