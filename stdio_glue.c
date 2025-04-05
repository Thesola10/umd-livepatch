/**
 * @file       stdio_glue.c
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      Glue code between POSIX stdio and PSP kernel
 *
 * This module allows me to reuse code from the UMDiff utility, by redirecting
 * POSIX standard I/O functions to their PSP kernel counterparts.
 * Probably won't work for more complex programs.
 */

#include <unistd.h>
#include <pspkernel.h>

off_t
lseek(int fd, off_t offset, int whence)
{
    return sceIoLseek(fd, offset, whence);
}

int
read(int fd, void *buf, size_t count)
{
    return sceIoRead(fd, buf, count);
}

int
write(int fd, const void *buf, size_t count)
{
    return sceIoWrite(fd, buf, count);
}

// vim: ft=c.doxygen
