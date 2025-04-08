/**
 * @file       file.c
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      Functions to read and write UMDiff files
 *
 * The functions defined here control reading and writing UMDiff files from
 * in-memory structure @ref umdiff_File.
 *
 * @warning This file is reused by umd_livepatch. Do not allocate on the heap!
 */

#include "umdiff.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef union {
    char data[sizeof(umdiff_Header)];
    umdiff_Header hdr;
} _impl_umdiff_RawHeader;

#define _impl_umdiff_alignSector$(x) \
    ( x + ISO_SECTOR_SIZE - (x % ISO_SECTOR_SIZE) )

void
_impl_umdiff_Header_rectify(umdiff_Header *hdr)
{
    long len_preamble = sizeof(umdiff_Header)
                      + (sizeof(umdiff_Command) * hdr->cmd_count);

    memcpy(hdr->magic, umdiff_File_$MAGIC, 7);
    hdr->version = umdiff_File_$VERSION;
    hdr->data_start = _impl_umdiff_alignSector$(len_preamble);
}

int
umdiff_File_load(umdiff_File *file, int fd, umdiff_FileFlags flags)
{
    _impl_umdiff_RawHeader rheader;
    int ret;

    if (!file) return 1;

    ret = read(fd, &rheader.data, sizeof(umdiff_Header));
    file->hdr = rheader.hdr;

    if (strncmp(file->hdr.magic, umdiff_File_$MAGIC, 7))
        return 1;
    if (file->hdr.version != umdiff_File_$VERSION)
        return 1;

    if (flags >= umdiff_FileFlags_HEADER_AND_COMMANDS) {
        ret = read(fd, file->commands, sizeof(umdiff_Command) * file->hdr.cmd_count);
    }
    if (flags == umdiff_FileFlags_LOAD_FULL) {
        file->data_len = lseek(fd, 0, SEEK_END) - file->hdr.data_start;
        ret = lseek(fd, file->hdr.data_start, SEEK_SET);
        ret = read(fd, file->data, file->data_len);
    }

    return 0;
}

int
umdiff_File_write(umdiff_File *file, int outfd)
{
    _impl_umdiff_RawHeader rheader;
    int ret;

    if (!file || file->mode != umdiff_FileFlags_LOAD_FULL) {
        dprintf(1, "File object is in incomplete mode. Cannot proceed.\n");
        return 1;
    } else if (!file->commands || !file->data) {
        dprintf(1, "File object is missing commands or data buffer. Cannot proceed.\n");
        return 1;
    } else if (!file->hdr.cmd_count) {
        dprintf(1, "File object has zero commands. Cannot proceed.\n");
        return 1;
    } else if (!file->hdr.index[0]) {
        dprintf(1, "File object has an empty index.\n");
        //return 1;
    }

    rheader.hdr = file->hdr;

    _impl_umdiff_Header_rectify(&rheader.hdr);

    ret = write(outfd, rheader.data, sizeof(umdiff_Header));
    ret = write(outfd, file->commands, sizeof(umdiff_Command) * rheader.hdr.cmd_count);
    ret = lseek(outfd, rheader.hdr.data_start, SEEK_SET);
    ret = write(outfd, file->data, file->data_len);

    return 0;
}

// vim: ft=c.doxygen
