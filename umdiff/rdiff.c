/**
 * @file       rdiff.c
 * @author     Karim Vergnes <me@thesola.io>
 * @copyright  GPLv2
 * @brief      rsync-based diff calculator
 *
 * This file leverages librsync to compute the difference between two files,
 * and convert it in-memory into the UMDiff format.
 */

#include "rdiff.h"

#include <librsync.h>

typedef union {
    int fd;
    void *opaque;
} _impl_umdiff_OpaqueFd;

rs_signature_t *workSignatures;

rs_result
_impl_umdiff_sigJobSink(rs_job_t *job, rs_buffers_t *buf, void *dest)
{

}

rs_result
_impl_umdiff_sigJobSource(rs_job_t *job, rs_buffers_t *buf, void *fd_)
{
    _impl_umdiff_OpaqueFd fd = { .opaque = fd_ };
}

rs_result
_impl_umdiff_deltaJobSink(rs_job_t *job, rs_buffers_t *buf, void *unk)
{

}

rs_result
_impl_umdiff_deltaJobSource(rs_job_t *job, rs_buffers_t *buf, void *fd_)
{
    _impl_umdiff_OpaqueFd fd = { .opaque = fd_ };
}


umdiff_File *
umdiff_File_fromCompare(int source_fd, int target_fd)
{
    _impl_umdiff_OpaqueFd source_fd_ = { .fd = source_fd };
    _impl_umdiff_OpaqueFd target_fd_ = { .fd = target_fd };
    umdiff_File *resultFile;

    rs_buffers_t buffers;
    rs_job_t *sigJob, *deltaJob;

    sigJob = rs_sig_begin(ISO_SECTOR_SIZE,
                          RS_DEFAULT_MIN_STRONG_LEN,
                          RS_RK_BLAKE2_SIG_MAGIC);
    deltaJob = rs_delta_begin(workSignatures);

    rs_job_drive(sigJob, &buffers,
                _impl_umdiff_sigJobSource,  source_fd_.opaque,
                _impl_umdiff_sigJobSink,    workSignatures);
    rs_job_drive(deltaJob, &buffers,
                _impl_umdiff_deltaJobSource, target_fd_.opaque,
                _impl_umdiff_deltaJobSink,   resultFile);

    return resultFile;
}

// vim: ft=c.doxygen
