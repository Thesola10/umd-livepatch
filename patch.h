#ifndef __PATCH_H
#define __PATCH_H

/**
 * @file       patch.h
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      UMDiff patch loader functions
 *
 * This module is responsible for interfacing with a UMDiff file, and carrying
 * out the actual patching algorithm.
 */

#include "umdiff/umdiff.h"

/**
 * How many @ref lp_UMDiffCommand objects should be loaded in memory at once.
 */
#define CMD_BUFFER_SIZE 1024

/**
 * @brief      Load buffer with commands at index offset for sector
 *
 * This loader is designed with the worst-case scenario in mind -- one command
 * per sector. Since the resulting commands table would weigh in at 1.6 million
 * commands, each 20 bytes, we couldn't possibly hope to store all of it in the
 * PSP's working memory. Instead, we load a buffer with @ref CMD_BUFFER_SIZE
 * commands, and perform lookup within.
 *
 * This function is responsible for seeking to the index pointer containing the
 * given sector, and filling the buffer with commands starting at that point.
 *
 * This function will not do anything if the buffer already contains commqnds
 * from the requested index.
 *
 * @param sector       The sector offset the read request starts at.
 */
int
lp_loadCmdsForIndex(long sector);

#endif //__PATCH_H

// vim: ft=c.doxygen
