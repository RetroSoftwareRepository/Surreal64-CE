#include "RomList.h"
//#include "../Launchdata.h"	// Ez0n3 - launchdata isn't used anymore
#include "../config.h"
#include "zlib/zlib.h"
#include "zlib/unzip.h"
#include "musicmanager.h"

extern CMusicManager music;
extern int romcounter;
extern int actualrom;
#define BUFFER_SIZE 0x400000 // -> (4096 / 4KB pagesize) = 1MB
extern byte GetByteSwapType(byte *header);
extern bool ByteSwap(dword size, byte *image, byte bswapType);
extern int dw1964DynaMem;
extern int dw1964PagingMem;
extern int dwPJ64DynaMem;
extern int dwPJ64PagingMem;
extern bool bUseLLERSP; // Ez0n3 - use iAudioPlugin instead, but leave this in case it's set in ini
extern int m_emulator;
extern void CreateProgress();
extern void RenderProgress(int progress);
extern int ConfigAppSaveTemp();
extern int videoplugin;
extern bool onhd;

// Ez0n3 - use iAudioPlugin instead to determine which audio plugin is used
extern int iAudioPlugin;

// Ez0n3 - reinstate max video mem
extern int dwMaxVideoMem;


DWORD WINAPI PrepareRomThread(Rom *rom)
{

	//Ez0n3 - skip the loading screen if the temporary rom is the same as the rom that is loading (its the exact same rom file).
	char currom[256];
	sprintf(currom, "%08X-%08X-C:%02X", rom->GetCrc1(), rom->GetCrc2(), rom->GetCountry());
	
	ifstream romTmpFile;
	romTmpFile.open("Z:\\TemporaryRom.dat", ios_base::in | ios_base::binary);
	if (romTmpFile.is_open()) {
		byte header[0x40];
		romTmpFile.read(reinterpret_cast<char *>(header), 0x40);
		romTmpFile.close();
		
		byte byByteSwapType = GetByteSwapType(header);
		if (byByteSwapType != -1) {
			char tmprom[256];
			dword tmp_dwCrc1;
			dword tmp_dwCrc2;
			byte tmp_byCountry;
		
			ByteSwap(0x40, header, byByteSwapType);
			tmp_dwCrc1	= *(reinterpret_cast<dword *>(header + 0x10));
			tmp_dwCrc2	= *(reinterpret_cast<dword *>(header + 0x14));
			tmp_byCountry	= *(reinterpret_cast<byte *>(header + 0x3D));
			sprintf(tmprom, "%08X-%08X-C:%02X", tmp_dwCrc1, tmp_dwCrc2, tmp_byCountry);
			
			if (strcmp(currom, tmprom) == 0) {
				return 0;
			}
		}
	}
	

	CreateProgress();
	int progress=0;
	RenderProgress(progress);
	// the rom to prepare
	string szFileName = rom->GetFileName();
	string romName = rom->GetProperName();
	// allocate 1 meg of memory for temporary rom loading
	byte *pBuffer = static_cast<byte *>(VirtualAlloc(NULL, BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE));
	
	// save the ini file
	//Only save to T: if we're on CD/DVD
	//We should only have one Surreal.ini
	//g_iniFile.Save("T:\\Surreal10.ini");
	//FIXME: needs testing

	if (!onhd)
	{
		g_iniFile.Save("T:\\Surreal.ini");
	}
	else g_iniFile.Save("D:\\Surreal.ini");

	// open the destination rom
	ofstream destRom;
	destRom.open("Z:\\TemporaryRom.dat", ios_base::out | ios_base::binary);

	if (!destRom.is_open())
	{
		goto _FAIL;
	}

	// check to see if the source file is a zip file. if it is, use the zlib to extract the rom
	if (szFileName.substr(szFileName.length() - 4) == ".zip")
	{
		int retVal;
		unzFile	fp;
		fp = unzOpen(szFileName.c_str());

		retVal = unzGoToFirstFile(fp);

		if (retVal != UNZ_OK)
		{
			goto _FAIL;
		}

		do
		{
			unz_file_info fileInfo;
			char szUnzFileName[_MAX_FNAME];
			
			retVal = unzGetCurrentFileInfo(fp, &fileInfo, szUnzFileName, _MAX_FNAME, NULL, 0, NULL, 0);

			if (retVal != UNZ_OK)
			{
				goto _FAIL;
			}

			string szFn(szUnzFileName);

			// find a rom file in the zip file
			if (szFn.substr(szFn.length() - 4) == ".bin"
			||	szFn.substr(szFn.length() - 4) == ".v64"
			||	szFn.substr(szFn.length() - 4) == ".rom"
			||	szFn.substr(szFn.length() - 4) == ".usa"
			||	szFn.substr(szFn.length() - 4) == ".z64"
			||	szFn.substr(szFn.length() - 4) == ".j64"
			||	szFn.substr(szFn.length() - 4) == ".pal"
			||	szFn.substr(szFn.length() - 4) == ".n64")
			{
				retVal = unzOpenCurrentFile(fp);

				if (retVal != UNZ_OK)
				{
					unzClose(fp);

					goto _FAIL;
				}

				// read the first meg
				unzReadCurrentFile(fp, pBuffer, BUFFER_SIZE);

				// get the byte order of the rom
				byte byBswapType = GetByteSwapType(pBuffer);

				// byte swap the first meg. at this stage, we can check whether it is a n64 rom
				if (!ByteSwap(BUFFER_SIZE, pBuffer, byBswapType))
				{
					// not a rom file
					unzCloseCurrentFile(fp);
					unzClose(fp);
					
					goto _FAIL;
				}

				// write the pBuffer to the destination rom
				destRom.write(reinterpret_cast<char *>(pBuffer), BUFFER_SIZE);

				for(qword i = BUFFER_SIZE; i < fileInfo.uncompressed_size; i += BUFFER_SIZE)
				{
					progress =  static_cast<dword>((i) * 100 / fileInfo.uncompressed_size) ;
					RenderProgress(progress);

					// read a meg
					unzReadCurrentFile(fp, pBuffer, BUFFER_SIZE);

					// byte swap the meg
					ByteSwap(BUFFER_SIZE, pBuffer, byBswapType);

					// write the pBuffer to the destination rom
					destRom.write(reinterpret_cast<char *>(pBuffer), BUFFER_SIZE);
				}

				progress =  static_cast<dword> ((i) * 100 / fileInfo.uncompressed_size) ;
                RenderProgress(progress);

				unzCloseCurrentFile(fp);
				unzClose(fp);

				goto _SUCCESS;
			}
		} while (unzGoToNextFile(fp) == UNZ_OK);

		unzClose(fp);
	}
	else
	{
		ifstream fp;

		// open the rom
		fp.open(szFileName.c_str(), ios_base::in | ios_base::binary);

		if (!fp.is_open())
		{
			goto _FAIL;
		}

		// get the size of the rom
		fp.seekg(0, ios_base::end);
		qword qwFileSize = fp.tellg();
		fp.seekg(0, ios_base::beg);

		// read the first meg
		fp.read(reinterpret_cast<char *>(pBuffer), BUFFER_SIZE);

		// get the byte order of the rom
		byte byBswapType = GetByteSwapType(pBuffer);

		// byte swap the first meg. at this stage, we can check whether it is a n64 rom
		if (!ByteSwap(BUFFER_SIZE, pBuffer, byBswapType))
		{
			// not a rom file
			fp.close();

			goto _FAIL;
		}

		destRom.write(reinterpret_cast<char *>(pBuffer), BUFFER_SIZE);

		for(qword i = BUFFER_SIZE; i < qwFileSize; i += BUFFER_SIZE)
		{
			progress =  static_cast<dword>((i) * 100 / qwFileSize) ;
            RenderProgress(progress);

			// seek to the next meg
			fp.seekg(static_cast<dword>(i), ios_base::beg);

			// read a meg
			fp.read(reinterpret_cast<char *>(pBuffer), BUFFER_SIZE);

			// byte swap the meg
			ByteSwap(BUFFER_SIZE, pBuffer, byBswapType);

			// write the pBuffer to the destination rom
			destRom.write(reinterpret_cast<char *>(pBuffer), BUFFER_SIZE);
		}

		progress =  static_cast<dword>((i) * 100 / qwFileSize) ;
        RenderProgress(progress);

		fp.close();

		goto _SUCCESS;
	}

_SUCCESS:
	goto _END;

_FAIL:
	goto _END;

_END:
	destRom.close();
	VirtualFree(pBuffer, BUFFER_SIZE, MEM_DECOMMIT);

	return 0;
}


void Launch()
{
			Rom *m_pRom = g_romList.GetRomAt(actualrom);
			music.Stop();
			PrepareRomThread(m_pRom);

			ConfigAppSaveTemp();


			switch (m_emulator)
			{
				case _1964:
				{
					switch (videoplugin) {
				case 0 : XLaunchNewImage("D:\\1964-510.xbe", NULL);
					     break;
				case 1 : XLaunchNewImage("D:\\1964-531.xbe", NULL);
					     break;
				case 2 : XLaunchNewImage("D:\\1964-560.xbe", NULL);
					break;
				case 3 : XLaunchNewImage("D:\\1964-612.xbe", NULL);
					break;}
					break;
				}
				case _Project64:
				{
					switch (videoplugin) {
				case 0 : XLaunchNewImage("D:\\Pj64-510.xbe", NULL);
					     break;
				case 1 : XLaunchNewImage("D:\\Pj64-531.xbe", NULL);
					     break;
				case 2 : XLaunchNewImage("D:\\Pj64-560.xbe", NULL);
					break;
				case 3 : XLaunchNewImage("D:\\Pj64-612.xbe", NULL);
					break;}
					break;
				}
				case _UltraHLE:
				{
					XLaunchNewImage("D:\\UltraHLE.xbe", NULL);
				}
			}
}
