#ifndef __COMPARE_H
#define __COMPARE_H

/**
 * @file       compare.h
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      rsync-based diff calculator functions
 *
 * Generator functions to create a @ref umdiff_File object from computing the
 * difference between two files, using librsync algorithms.
 */

#include "umdiff.h"

#include <stddef.h>

/**
 * @brief      Add commands from a rdiff stream.
 *
 * This function takes a buffer in the rdiff delta file format, and adds UMDiff
 * commands matching the parsed rdiff commands to the specified file.
 *
 * As it is intended to be called repeatedly, it will statefully alter the
 * provided @ref umdiff_File object as it goes.
 */
int
umdiff_File_feedCommands(umdiff_File *file, char *buf, size_t len);

/**
 * @brief      Generate a UMDiff file from a source and target file contents.
 *
 * This function invokes a signature and delta job from librsync under the hood.
 * This is the optimum approach, as it includes our optimum parameters,
 * ensuring computed blocks line up with ISO9660 sectors.
 */
int
umdiff_File_fromCompare(umdiff_File *file, int source_fd, int target_fd);

#endif //__COMPARE_H

// vim: ft=c.doxygen
