/**
 * @file       main.c
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      Indexed delta generator for umd_livepatch
 *
 * This tool generates a delta file between two ISOs, in the UMDiff file format.
 * The rationale for this file format can be found in the docs for @ref patch.h
 * in the umd_livepatch module.
 */

#include "usage.rl.h"

int main(int argc, char *argv[])
{
    int res;
    options_t opts;

    res = parse_options(argc, argv, &opts);
    if (res == -1)
        return -1;

    if (opts.help) {
        show_usage();
        return 0;
    }

    return 1;
}

// vim: ft=c.doxygen
