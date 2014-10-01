// IoSupport.h: interface for the CIoSupport class.
#ifndef _IOSUPPORT_H__1964_
#define _IOSUPPORT_H__1964_

#if _MSC_VER > 1000
#pragma once
#endif //_MSC_VER > 1000

#include <xtl.h>
#include "Undocumented.h"

#define TRAY_OPEN					16
#define TRAY_CLOSED_NO_MEDIA		64
#define TRAY_CLOSED_MEDIA_PRESENT	96

#define DRIVE_OPEN						0 // Open...
#define DRIVE_NOT_READY					1 // Opening.. Closing... 
#define DRIVE_READY						2  
#define DRIVE_CLOSED_NO_MEDIA			3 // CLOSED...but no media in drive
#define DRIVE_CLOSED_MEDIA_PRESENT		4 // Will be send once when the drive just have closed

class CIoSupport  
{
public:
	CIoSupport();
	virtual ~CIoSupport();

	HRESULT Mount(CHAR* szDrive, CHAR* szDevice);
	HRESULT Unmount(CHAR* szDrive);

	HRESULT Remount(CHAR* szDrive, CHAR* szDevice);
	HRESULT Remap(CHAR* szMapping);

	DWORD	GetTrayState();
	HRESULT EjectTray();
	HRESULT CloseTray();
	HRESULT Shutdown();

	HANDLE  CreateFileIO();
	BOOL	GetFirstFile(CHAR* szFilename);

private:
	DWORD m_dwTrayState;
	DWORD m_dwTrayCount;
	DWORD m_dwLastTrayState;
};

extern CIoSupport g_IOSupport;

#endif //_IOSUPPORT_H__1964_
