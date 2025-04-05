#ifndef __UMDIFF_H
#define __UMDIFF_H

/**
 * @file       umdiff.h
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      Definitions for UMDiff file format
 *
 * This file contains all required types to read and parse a UMDiff file.
 */

#define UMDIFF_VERSION 0x00

/**
 * @brief      Index of commands at 1024 sector interval.
 *
 * This list contains the offset of the first command for every 1024 sector
 * block.
 *
 * The value represents an offset to read the file from, in the command list.
 * If a command exists across a 1024-sector boundary, the offset shall point
 * to the start of that command.
 *
 * Pointers spanning past the end of the disc are undefined and shall not be
 * used.
 */
typedef long umdiff_CmdIndex[1024];

/**
 * @brief      UMDiff file header structure.
 *
 * This is the format for a UMDiff file header. The commands index is included,
 * as with the constrained memory of the PSP, reading the index in full is
 * mandatory.
 *
 * With the tradeoff of an O(n) space and O(n) time conversion process on PC,
 * we gain a worst-case patch application process in O(1) space and O(log(n))
 * time on PSP.
 */
typedef struct 
__attribute__((packed)) {
    char magic[7]; /* = 0x7f 'UMDiff' */
    char version;
    long cmd_len;
    long data_start;
    umdiff_CmdIndex index;
} umdiff_Header;

/**
 * @brief      Definition for a single patch command.
 *
 * This is the format for a unit 'patch command'. A patch command is comprised
 * of five parameters:
 *
 * - <tt>sector_start</tt>: Where on the original disc the patch command takes
 *   effect, in sectors.
 * - <tt>sector_count</tt>: How many sectors on the original disc the patch
 *   command spans.
 * - <tt>patch_start</tt>: Where in the patch source the substitute data is
 *   found, in sectors.
 * - <tt>patch_sector_count</tt>: How many sectors on the patch source the
 *   substitute data occupies. If smaller than <tt>sector_count</tt>, then
 *   the substitute is a repeating pattern of that length.
 * - <tt>data_source</tt>: 1 if the substitute data is on the original disc,
 *   0 if it is in the UMDiff file's data area.
 */
typedef struct {
    long sector_start;
    long sector_count;

    long patch_start;
    long patch_sector_count; /* if < sector_count, repeat */

    long data_source; /* 0 = patchfile, 1 = source */
} umdiff_Command;

// Commands shall start immediately after the full header
#define UMDIFF_COMMANDS_START (sizeof lp_UMDiffHeader)

typedef struct {
    umdiff_Header *hdr;
    umdiff_Command *commands;
} umdiff_File;

#endif //__UMDIFF_H

// vim: ft=c.doxygen
