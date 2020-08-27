/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>

#include "error_handler.h"

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

/*
 environ
 A pointer to a list of environment variables and their values.
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

/*
#include "FreeRTOSConfig.h"
#include "debug_helpers.h"

#define MAX_STACK_SIZE 0x2000
#define FreeRTOS

#ifndef FreeRTOS
  register char * stack_ptr asm("sp");
#endif

caddr_t _sbrk(int incr)
{
    extern char end asm("end");
    static char *heap_end;
    char *prev_heap_end,*min_stack_ptr;

    if (heap_end == 0)
        heap_end = &end;

    prev_heap_end = heap_end;

#ifdef FreeRTOS
    // Use the NVIC offset register to locate the main stack pointer.
    min_stack_ptr = (char*)(*(unsigned int *)*(unsigned int *)0xE000ED08);
    // Locate the STACK bottom address
    min_stack_ptr -= MAX_STACK_SIZE;

    if (heap_end + incr > min_stack_ptr)
#else
    if (heap_end + incr > stack_ptr)
#endif
    {
        debug_print("Heap and stack collision\n");
        abort();
        errno = ENOMEM;
        return (caddr_t) -1;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}
*/
int _write(int file, char *ptr, int len);

int _close(int file)
{
    (void) file;
    return -1;
}

void _exit(int status)
{
    app_exit_handler(status);
    while (1) {
    }
}

int _fork(void)
{
    errno = EAGAIN;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void) file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _getpid(void)
{
    return 1;
}

int _isatty(int file)
{
    switch (file){
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

int _kill(int pid, int sig)
{
    (void) pid; (void) sig;
    errno = EINVAL;
    return (-1);
}

int _lseek(int file, int ptr, int dir)
{
    (void) file; (void) ptr; (void) dir;
    return 0;
}

/*
 read
 Read a character from a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */


int _read(int file, char *ptr, int len)
{
    int ch;
    int n;
    int num = 0;
    switch (file) {
    case STDIN_FILENO:
        for (n = 0; n < len; n++) {
            ch = __io_getchar();
            if (ch == EOF) {
                errno = EIO;
                return -1;
            }
            *ptr++ = ch;
            num++;
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return num;
}

/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */

int _stat(const char *filepath, struct stat *st)
{
    (void) filepath;
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */
int _write(int file, char *ptr, int len)
{
    int n;
    char r = '\r';
    switch (file) {
    case STDOUT_FILENO: /*stdout*/
        for (n = 0; n < len; n++) {
            if (*ptr == '\n') {
                __io_putchar(r);
            }
            __io_putchar(*ptr++);
        }
        break;
    case STDERR_FILENO: /* stderr */
        for (n = 0; n < len; n++) {
            if (*ptr == '\n') {
                __io_putchar(r);
            }
            __io_putchar(*ptr++);
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
