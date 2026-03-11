#ifndef _PC_H
#define _PC_H       1

#define min(a,b)    ((a)<(b)?(a):(b))
#define max(a,b)    ((a)<(b)?(b):(a))

#define FP_OFF(__p) ((unsigned)(__p))
#define FP_SEG(__p) ((unsigned)((unsigned long)(void __far*)(__p) >> 16))
#define MK_FP(seg,off) ((((unsigned long)(void __far *)(seg)) << 16) | ((unsigned long)(unsigned int)(off)))

#define xKL_BACKEND_UART
#define KL_BACKEND_KBD

typedef unsigned uint;

#ifdef __WATCOMC__
#pragma aux (cdecl) main;
#pragma disable_message (102, 106, 107, 113)
#pragma pack(1)
#endif

#endif
