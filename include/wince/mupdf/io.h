

#ifndef	__BASE_IO_H__
#define	__BASE_IO_H__


#ifndef O_BINARY
#define O_BINARY 0
#endif

#define _O_RDONLY       0x0000  /* open for reading only */
#define _O_WRONLY       0x0001  /* open for writing only */
#define _O_RDWR         0x0002  /* open for reading and writing */
#define _O_APPEND       0x0008  /* writes done at eof */

#define _O_CREAT        0x0100  /* create and open file */
#define _O_TRUNC        0x0200  /* open and truncate */
#define _O_EXCL         0x0400  /* open only if file doesn't already exist */


/* Microsoft defines some things with slightly different names */
# define O_RDWR _O_RDWR
# define O_RDONLY _O_RDONLY
# define O_WRONLY _O_WRONLY
# define O_CREAT _O_CREAT
# define O_TRUNC _O_TRUNC
# define O_APPEND _O_APPEND
# define O_EXCL _O_EXCL

extern int errno;


int __cdecl open(const char *, int , ...);
//int __cdecl _wopen(const wchar_t * path, int oflag , int _PermissionMode);
int __cdecl write(int , const void * , unsigned int );
int __cdecl read(int , void * , unsigned int );
int __cdecl lseek(int , long , int );
int __cdecl close(int);
int __cdecl unlink(const char *);
char* __cdecl getenv(const char * _VarName);
const char *  __cdecl strerror(int);
//int __cdecl _wopen(const wchar_t * , int , int);
#endif