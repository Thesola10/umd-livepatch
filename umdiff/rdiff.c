/**
 * @file       rdiff.c
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      rsync-based diff calculator
 *
 * This file leverages librsync to compute the difference between two files,
 * and convert it in-memory into the UMDiff format.
 */

#include <librsync.h>


// vim: ft=c.doxygen
