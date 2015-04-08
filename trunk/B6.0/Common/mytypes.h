#ifndef WIN32

/* Data Types */
#define uint64	unsigned __int64
#define uint32	unsigned __int32
#define uint16	unsigned __int16
#define uint8	unsigned __int8

/* unsigned types */
#define _u64	unsigned __int64	/* 64 bit */
#define _u32	unsigned __int32	/* 32 bit */
#define _u16	unsigned __int16	/* 16 bit */
#define _u8		unsigned __int8		/* 8 bit */

/* signed types */
#define _s64	__int64				/* 64 bit */
#define _s32	__int32				/* 32 bit */
#define _s16	__int16				/* 16 bit */
#define _s8		__int8				/* 8 bit */

#define EXCEPTION_EXECUTE_HANDLER 1
typedef int					HMODULE;
typedef int					HINSTANCE;
typedef int					HANDLE;
typedef int					HWND;
typedef int                 INT;
typedef unsigned int        UINT;
typedef __int64				LONGLONG;
typedef long                LONG;
typedef unsigned long       DWORD;
typedef char				CHAR;
typedef unsigned char       BYTE;
typedef short				SHORT;
typedef unsigned short      WORD;
typedef float               FLOAT;
//typedef const TCHAR*		LPCTSTR;
typedef const CHAR*			LPCSTR;
typedef CHAR*				LPSTR;

typedef int                 BOOL;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define NULL 0

/*#ifndef LARGE_INTEGER
#if defined(MIDL_PASS)
typedef struct _LARGE_INTEGER {
#else // MIDL_PASS
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
#endif //MIDL_PASS
    LONGLONG QuadPart;
} LARGE_INTEGER;
#endif*/
#ifdef _XBOX_ICC
// I don't want to make another file just for this...
// Enable ROM Paging in ICC lib, the rest of the project
// will need it in stdafx.h
#define USE_ROM_PAGING 
//#undef USE_ROM_PAGING
#endif

#endif