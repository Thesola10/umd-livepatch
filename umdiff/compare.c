/**
 * @file       compare.c
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      rsync-based diff calculator
 *
 * This file leverages librsync to compute the difference between two files,
 * and convert it in-memory into the UMDiff format.
 */

#include "compare.h"

#include <librsync.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFERS_SIZE 1048576

#define _impl_umdiff_alignBufferSize$(x) \
    ( x + BUFFERS_SIZE - (x % BUFFERS_SIZE) )

typedef union {
    int fd;
    void *opaque;
} _impl_umdiff_OpaqueFd;

rs_signature_t *workSignatures;

size_t workSigs_counter = 0;
size_t workSigs_size = 0;

rs_result
_impl_umdiff_sigJobSink(rs_job_t *job, rs_buffers_t *buf, void *dest)
{
    int res;
    rs_signature_t *sigs = dest;

    if (buf->avail_out + workSigs_counter > workSigs_size) {
        sigs = realloc(sigs, workSigs_size + _impl_umdiff_alignBufferSize$(buf->avail_out));
        workSigs_size += _impl_umdiff_alignBufferSize$(buf->avail_out);
    }
    memcpy((char *) sigs + workSigs_counter, buf->next_out, buf->avail_out);
    workSigs_counter += buf->avail_out;

    buf->avail_out = 0;

    return RS_DONE;
}

rs_result
_impl_umdiff_sigJobSource(rs_job_t *job, rs_buffers_t *buf, void *fd_)
{
    int res;
    _impl_umdiff_OpaqueFd fd = { .opaque = fd_ };

    buf->avail_in = read(fd.fd, buf->next_in, BUFFERS_SIZE);
    if (!buf->avail_in)
        buf->eof_in = 1;

    return RS_DONE;
}

rs_result
_impl_umdiff_deltaJobSink(rs_job_t *job, rs_buffers_t *buf, void *file_)
{
    umdiff_File *file = file_;

    //TODO: parse each command output

    return RS_DONE;
}

rs_result
_impl_umdiff_deltaJobSource(rs_job_t *job, rs_buffers_t *buf, void *fd_)
{
    int res;
    _impl_umdiff_OpaqueFd fd = { .opaque = fd_ };

    buf->avail_in = read(fd.fd, buf->next_in, BUFFERS_SIZE);
    if (!buf->avail_in)
        buf->eof_in = 1;

    return RS_DONE;
}


int
umdiff_File_fromCompare(umdiff_File *file, int source_fd, int target_fd)
{
    _impl_umdiff_OpaqueFd source_fd_ = { .fd = source_fd };
    _impl_umdiff_OpaqueFd target_fd_ = { .fd = target_fd };

    rs_job_t *sigJob, *deltaJob;
    rs_buffers_t buffers = {
        .eof_in = 0,
        .avail_in = 0,
        .avail_out = 0,
        .next_in = malloc(BUFFERS_SIZE),
        .next_out = malloc(BUFFERS_SIZE)
    };

    workSignatures = malloc(BUFFERS_SIZE);
    workSigs_size = BUFFERS_SIZE;

    sigJob = rs_sig_begin(ISO_SECTOR_SIZE,
                          RS_DEFAULT_MIN_STRONG_LEN,
                          RS_RK_BLAKE2_SIG_MAGIC);
    deltaJob = rs_delta_begin(workSignatures);

    rs_job_drive(sigJob, &buffers,
                _impl_umdiff_sigJobSource,  source_fd_.opaque,
                _impl_umdiff_sigJobSink,    workSignatures);
    rs_job_drive(deltaJob, &buffers,
                _impl_umdiff_deltaJobSource, target_fd_.opaque,
                _impl_umdiff_deltaJobSink,   file);

    return 0;
}

// vim: ft=c.doxygen
