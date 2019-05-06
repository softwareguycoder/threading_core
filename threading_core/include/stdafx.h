#ifndef __STDAFX_H__
#define __STDAFX_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <errno.h>

#ifndef OK
#define OK		0		// Code to return to the operating system to indicate successful program termination
#endif // OK

#ifndef ERROR
#define ERROR	-1		// Code to return to the operating system to indicate an error condition
#endif // ERROR

#include <../../api_core/api_core/include/api_core.h>
#include <../../../debug_core/debug_core/include/debug_core.h>

#endif //__STDAFX_H__
