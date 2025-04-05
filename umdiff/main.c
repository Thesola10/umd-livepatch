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

#include <fcntl.h>

#include "usage.rl.h"
#include "rdiff.h"

int
umdiff_delta(char *source, char *target, char *output)
{
    int source_fd, target_fd, output_fd;
    umdiff_File *result;

    source_fd = open(source, O_RDONLY);
    target_fd = open(target, O_RDONLY);

    result = umdiff_File_fromCompare(source_fd, target_fd);

    output_fd = open(output, O_WRONLY|O_CREAT|O_TRUNC);
    umdiff_File_write(result, output_fd);

    return 1;
}

int
umdiff_patch(char *source, char *umdiff, char *output)
{
    return 1;
}

int
umdiff_fromRdiff(char *rdiff, char *output)
{
    return 1;
}

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

    if (opts.delta)
        return umdiff_delta(opts.source_file, opts.target_file, opts.output_file);
    else if (opts.patch)
        return umdiff_patch(opts.source_file, opts.umdiff_file, opts.output_file);
    else if (opts.fromrdiff)
        return umdiff_fromRdiff(opts.rdiff_file, opts.output_file);

    return 1;
}

// vim: ft=c.doxygen
