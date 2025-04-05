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


/**
 * @brief      Generate a UMDiff file from a source and target file contents.
 *
 * This function invokes a signature and delta job from librsync under the hood.
 * This is the optimum approach, as it includes our optimum parameters,
 * ensuring computed blocks line up with ISO9660 sectors.
 */
umdiff_File *
umdiff_File_fromCompare(int source_fd, int target_fd);

#endif //__COMPARE_H

// vim: ft=c.doxygen
