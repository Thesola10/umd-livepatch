/**
 * @file       rdiff.c
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      rsync diff parser
 *
 * This file parses and converts rdiff commands into equivalent UMDiff commands
 * and manages the corresponding data bucket.
 */

#include "compare.h"

#include <librsync.h>
#include <prototab.h>
#include <stddef.h>

int
umdiff_File_feedCommands(umdiff_File *file, char *buf, size_t len)
{
    umdiff_Command *lastCommand;

    //TODO:
    // 1. skip magic number
    if (file->hdr.cmd_count == 0 && file->data_len == 0
            && (int) *buf == RS_DELTA_MAGIC) {
        buf += sizeof RS_DELTA_MAGIC;
        len -= sizeof RS_DELTA_MAGIC;
    }

    if (file->hdr.cmd_count > 0)
        lastCommand = &file->commands[file->hdr.cmd_count - 1];

    // 2. Update cmd_len on new command
    // 3. Update data_len otherwise
    // 4. If data_len < last command's patch area: complete data
}

// vim: ft=c.doxygen
