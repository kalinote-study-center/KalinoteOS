/* C语言标准函数库 errno for KalinoteOS */

#if (!defined(ERRNO_H))

#define ERRNO_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

extern int errno;

#define	ENOENT		2	/* No such file or directory */
#define	ERANGE		34	/* Result too large (or too small) */

#if (defined(__cplusplus))
	}
#endif

#endif
