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

#ifndef ISO_SECTOR_SIZE
#  define ISO_SECTOR_SIZE 2048
#endif

#define umdiff_File_$MAGIC "\x7fUMDiff"
#define umdiff_File_$VERSION 0x00

// Commands shall start immediately after the full header
#define umdiff_File_$COMMANDS_START (sizeof(umdiff_Header))

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
    long cmd_count;
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
 *   found, in sectors (past @ref data_start for in-file patches).
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

/**
 * @brief      Flags to control loading a UMDiff file.
 *
 * In order to adapt to memory-constrained environments, one can choose to
 * not load certain parts of a UMDiff file, instead resorting to in-situ reads.
 */
typedef enum {
    umdiff_FileFlags_HEADER_ONLY         = 0,
    umdiff_FileFlags_HEADER_AND_COMMANDS = 1,
    umdiff_FileFlags_LOAD_FULL           = 2
} umdiff_FileFlags;

/**
 * @brief      In-memory structure for a UMDiff file.
 *
 * This struct represents the contents of a UMDiff file, loaded in memory.
 * If read from a file, only the header is guaranteed to be populated.
 *
 * If <tt>commands</tt> is set to point to a 1024-command buffer, and <tt>mode</tt>
 * is set to @ref umdiff_FileFlags_HEADER_ONLY, it will be used as a holding
 * buffer when reading commands from file.
 *
 * @see umdiff_FileFlags for the possible cases.
 */
typedef struct {
    umdiff_Header hdr;

    /**
     * @brief List of loaded commands from file
     *
     * If <tt>mode</tt> is set to @ref umdiff_FileFlags_HEADER_ONLY, this is
     * expected to be a pre-allocated buffer holding 1024 commands.
     * Otherwise, it contains all commands in the file.
     */
    umdiff_Command *commands;
    char *data;
    long data_len;

    /** Keeps track of the last command batch pulled from index. */
    int last_index;
    /** Which loading mode was used when this file was opened. */
    umdiff_FileFlags mode;
} umdiff_File;

/**
 * @brief      Callback function to read from a source.
 *
 * This is used by @ref umdiff_File_readPatched to perform an actual read from
 * a data source, be it the patch file or original media.
 */
typedef int (*umdiff_ReadCallback)(void *dest, long count, long offset);

/**
 * @brief      Load a UMDiff file from an open file descriptor.
 *
 * This function takes a pre-allocated @ref umdiff_File structure, and populates
 * it from an open file descriptor, according to the given mode flags.
 *
 * @param file     The target @ref umdiff_File to load into.
 * @param fd       The open file descriptor to read from.
 * @param flags    Flags to control how much data to read in memory.
 *
 * @return 0 on success, any other value indicates an error.
 */
int
umdiff_File_load(umdiff_File *file, int fd, umdiff_FileFlags flags);

/**
 * @brief      Write a fully-populated UMDiff into a file descriptor.
 *
 * This function takes a fully populated @ref umdiff_File structure, and writes
 * it down into an open file descriptor. The output header will be set with
 * correct values, only cmd_len and index must both be set.
 *
 * @param file     The @ref umdiff_File to write from. Must be fully populated.
 * @param outfd    The open file descriptor to write into.
 *
 * @return 0 on success, any other value indicates an error.
 */
int
umdiff_File_write(umdiff_File *file, int outfd);

/**
 * @brief      Perform a read at a given offset and size with patches applied.
 *
 * This function maps a given read request into the patched data, using the
 * provided callback functions to carry out the actual read commands needed.
 *
 * @param file             The @ref umdiff_File to perform the patch with.
 * @param dest             Like pread(2), the buffer to write the data into.
 * @param count            Like pread(2), the amount of data to read.
 * @param offset           Like pread(2), the offset at which to start reading.
 *
 * @param read_source      The function to read the source media.
 * @param read_patchFile   The function to read the patch file from storage.
 */
int
umdiff_File_readPatched(umdiff_File *file, void *dest, long count, long offset,
                        umdiff_ReadCallback read_source,
                        umdiff_ReadCallback read_patchFile);

#endif //__UMDIFF_H

// vim: ft=c.doxygen
