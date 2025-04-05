/**
 * @file       patch.c
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      UMDiff patch algorithms
 *
 * Functions to redirect read requests and reconstruct the result file given
 * a set of UMDiff commands.
 *
 * @warning This file is reused by umd_livepatch. Do not allocate on the heap!
 */

#include "umdiff.h"

#include <string.h>

static umdiff_File *current_file;

static int
_impl_umdiff_Command_read(umdiff_Command cmd, void *dest, long count, long offset,
                          umdiff_ReadCallback read_source,
                          umdiff_ReadCallback read_patchFile)
{
    long rel_offset = offset - (cmd.sector_start * ISO_SECTOR_SIZE);

    //TODO: implement read system
    // 1. perform first truncated read
    // 2. if repeat-length remains, perform second full read
    // 3. if repeat-length still remains, memcpy second read range onwards
    //    -> saved a bunch of i/o calls at the guest ptr's expense!
}

static umdiff_Command *
_impl_umdiff_File_readIndexCmds(umdiff_File *file, long offset_sector,
                                umdiff_ReadCallback read_patchFile)
{
    int res;
    int index = offset_sector / 1024;

    // stateful optimization: don't re-read the same commands
    if (index == file->last_index)
        return 0;

    if (file->mode == umdiff_FileFlags_HEADER_ONLY) {
        res = read_patchFile(file->commands, sizeof(umdiff_Command) * 1024,
                             file->hdr.index[index]);
        return file->commands;
    } else {
        return file->commands + file->hdr.index[index] - umdiff_File_$COMMANDS_START;
    }

    file->last_index = index;
}

/**
 * Virtual read callback for @ref umdiff_File with @ref umdiff_FileFlags_LOAD_FULL.
 *
 * Automatically set by @ref umdiff_File_readPatched as needed.
 */
int
_impl_umdiff_ReadCallback_fullFile(void *dest, long count, long offset)
{
    // Undo relative-to-absolute translation from _impl_umdiff_Command_read
    long ds = current_file->hdr.data_start;
    long real_offset = offset - ds;

    memcpy(dest, current_file->data + real_offset, count);

    return count;
}

int
umdiff_File_readPatched(umdiff_File *file, void *dest, long count, long offset,
                        umdiff_ReadCallback read_source,
                        umdiff_ReadCallback read_patchFile)
{
    int res;
    umdiff_Command *commands;
    umdiff_Command *curCommand;

#define $offset_sectors (offset / ISO_SECTOR_SIZE)

    current_file = file;

    if (file->mode == umdiff_FileFlags_LOAD_FULL) {
        read_patchFile = _impl_umdiff_ReadCallback_fullFile;
    }

    while (count > 0) {
        commands = _impl_umdiff_File_readIndexCmds(file, $offset_sectors,
                                                   read_patchFile);
        curCommand = 0;

        for (int i = 0; i < 1024; i++) {
            if (commands[i].sector_start <= $offset_sectors) {
                curCommand = &commands[i];
                break;
            }
        }
        res = _impl_umdiff_Command_read(*curCommand, dest, count, offset,
                                        read_source, read_patchFile);
        dest += res;
        count -= res;
    }

    return res;
}

// vim: ft=c.doxygen
