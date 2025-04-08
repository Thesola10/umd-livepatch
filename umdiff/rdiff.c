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
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef struct {
    enum rs_op_kind kind;

    uint64_t len;
    uint64_t copy_offset;
} _impl_umdiff_RdiffCommand;

static size_t
_impl_umdiff_RdiffCommand_parse(_impl_umdiff_RdiffCommand *cmd, char *buf, size_t len)
{
    rs_prototab_ent_t entry = rs_prototab[*buf];
    *cmd = (_impl_umdiff_RdiffCommand) {
        .kind = entry.kind,
        .len = entry.immediate,
        .copy_offset = 0
    };

    if (entry.kind == RS_KIND_LITERAL && entry.immediate)
        return 1;

    memcpy(&cmd->len, buf+1, entry.len_1);

    if (entry.kind == RS_KIND_COPY) {
        memcpy(&cmd->copy_offset, buf+1, entry.len_1);
        switch (entry.len_1) {
        case 2:
            cmd->copy_offset = be16toh(cmd->copy_offset); break;
        case 4:
            cmd->copy_offset = be32toh(cmd->copy_offset); break;
        case 8:
            cmd->copy_offset = be64toh(cmd->copy_offset); break;
        }
        memcpy(&cmd->len, buf+entry.len_1+1, entry.len_2);
        switch (entry.len_2) {
        case 2:
            cmd->len = be16toh(cmd->len); break;
        case 4:
            cmd->len = be32toh(cmd->len); break;
        case 8:
            cmd->len = be64toh(cmd->len); break;
        }
        return entry.len_1 + entry.len_2 + 1;
    } else {
        memcpy(&cmd->len, buf+1, entry.len_1);
        return entry.len_1 + 1;
    }
}

int
_impl_umdiff_File_feedData(umdiff_File *file, char *buf, size_t len)
{
#define _impl_umdiff_Command_bytesLeft$(x) \
    ((x->patch_start + x->patch_sector_count) * ISO_SECTOR_SIZE - file->data_len)

    umdiff_Command *lastCommand = &file->commands[file->hdr.cmd_count - 1];
    size_t bytesLeft = _impl_umdiff_Command_bytesLeft$(lastCommand);

    if (bytesLeft > len) {
        memcpy(file->data + file->data_len, buf, len);
        file->data_len += len;

        return len;
    } else {
        memcpy(file->data + file->data_len, buf, bytesLeft);
        file->data_len += bytesLeft;

        return bytesLeft;
    }
}

int
umdiff_File_feedCommands(umdiff_File *file, char *buf, size_t len)
{
    umdiff_Command *lastCommand = NULL, *newCommand;
    _impl_umdiff_RdiffCommand rdiffCommand;

    int ret, progress = 0;

    if (file->hdr.cmd_count == 0 && file->data_len == 0
            && (int) *buf == htobe32(RS_DELTA_MAGIC)) {
        dprintf(1, "Encountered magic");
        buf += sizeof RS_DELTA_MAGIC;
        len -= sizeof RS_DELTA_MAGIC;
    }

#define _impl_umdiff_Command_isUnfinished$(x) \
    (x->data_source == 0 \
        && (x->patch_start + x->patch_sector_count) > (file->data_len / ISO_SECTOR_SIZE))

    if (file->hdr.cmd_count > 0)
        lastCommand = &(file->commands[file->hdr.cmd_count - 1]);

    while (len) {
        if (lastCommand && _impl_umdiff_Command_isUnfinished$(lastCommand)) {
            ret = _impl_umdiff_File_feedData(file, buf, len);
            buf += ret;
            len -= ret;
            progress += ret;

            if (len <= 0) break;
        }

        ret = _impl_umdiff_RdiffCommand_parse(&rdiffCommand, buf, len);
        len -= ret;
        buf += ret;
        progress += ret;

        dprintf(1, "Length: %ld, offset: %ld\n", rdiffCommand.len, rdiffCommand.copy_offset);

        if (rdiffCommand.len % ISO_SECTOR_SIZE
                || rdiffCommand.copy_offset % ISO_SECTOR_SIZE) {
            dprintf(1, "Misaligned command! Length %ld, offset %ld\n", rdiffCommand.len, rdiffCommand.copy_offset);
            exit(4);
        }

        newCommand = &file->commands[file->hdr.cmd_count];
        file->hdr.cmd_count += 1;

        newCommand->data_source = (rdiffCommand.kind == RS_KIND_COPY);
        newCommand->sector_start = lastCommand
            ? lastCommand->sector_start + lastCommand->sector_count
            : 0;
        newCommand->sector_count = rdiffCommand.len / ISO_SECTOR_SIZE;
        newCommand->patch_sector_count = rdiffCommand.len / ISO_SECTOR_SIZE;
        newCommand->patch_start = (rdiffCommand.kind == RS_KIND_COPY)
            ? (rdiffCommand.copy_offset / ISO_SECTOR_SIZE)
            : (file->data_len / ISO_SECTOR_SIZE);

        lastCommand = newCommand;
    }

    return progress;
}

// vim: ft=c.doxygen
