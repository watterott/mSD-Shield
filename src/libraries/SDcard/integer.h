/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER
#define _INTEGER

#ifdef _WIN32	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */

/* These types must be 16-bit, 32-bit or larger integer */
#ifndef INT
typedef int				INT;
#endif
#ifndef UINT
typedef unsigned int	UINT;
#endif

/* These types must be 8-bit integer */
#ifndef CHAR
typedef char			CHAR;
#endif
#ifndef UCHAR
typedef unsigned char	UCHAR;
#endif
#ifndef BYTE
typedef unsigned char	BYTE;
#endif

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;

#endif

#endif
