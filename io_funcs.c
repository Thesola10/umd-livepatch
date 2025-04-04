/**
 * @file        io_funcs.c
 * @author      Karim Vergnes <me@thesola.io>
 * @copyright   GPLv2
 * @brief       Functions to interpose with UMD driver
 *
 * Functions to parse and manipulate devctl requests to the UMD driver,
 * allowing for live redirection of read requests to a patch file.
 */

#include "io_funcs.h"
#include <string.h>

extern PspIoDrvFuncs reserveUmdFuncs;

static char umd_id[11] = {0};

static char first_read = 1;

static char hdr[ISO_SECTOR_SIZE];

/**
 * @brief       Convert layout-based address (LBA) to an absolute offset.
 *
 * @param param The LBA parameter data, as obtained from a read devctl.
 */
static inline u32
_impl_lp_UmdLba_toAddr(lp_UmdLba *param)
{
    u32 offset;

    if (!param->byte_size_start) {
        offset = param->lba_top * ISO_SECTOR_SIZE;
    } else if (param->byte_size_centre) {
        offset = param->lba_top * ISO_SECTOR_SIZE - param->byte_size_start + ISO_SECTOR_SIZE;
    } else if (!param->byte_size_last) {
        offset = param->lba_top * ISO_SECTOR_SIZE + param->byte_size_start;
    } else {
        offset = param->lba_top * ISO_SECTOR_SIZE - param->byte_size_start + ISO_SECTOR_SIZE;
    }

    return offset;
}

/**
 * @brief      Read the header sector for the disc to memory.
 *
 * This function is meant to be read in response to an existing read devctl.
 * The parameters below should be patched through from a previous devctl call.
 *
 * @param arg      The devctl file argument to be passed through to the driver.
 * @param devname  The device name argument to be passed through to the driver.
 */
static inline int
_impl_lp_readDiscHeader(PspIoDrvFileArg *arg, const char *devname)
{
    int ret;

    lp_UmdLba param = {
        .unknown1 = 0,
        .cmd = 0, /* read */
        .lba_top = 0x8000 / ISO_SECTOR_SIZE,
        .lba_size = 1,
        .byte_size_total = ISO_SECTOR_SIZE,
        .byte_size_centre = ISO_SECTOR_SIZE,
        .byte_size_start = 0,
        .byte_size_last = 0
    };

    ret = reserveUmdFuncs.IoDevctl(arg, devname, lp_UmdDevctl_READ_SECTORS,
                                   &param, sizeof param, &hdr, ISO_SECTOR_SIZE);
    return ret;
}

/**
 * @brief      The effective low-level read command.
 *
 * This function is called whenever a devctl to read data off disc is
 * intercepted. All parameters are provided from the @ref PspIoDrvFuncs::IoDevctl
 * function call made by the caller, to allow seamless passthrough to the
 * original driver.
 *
 * @param param    The parsed {@ref lp_UmdLba} layout-based address.
 * @param outdata  The memory address where read data is expected.
 * @param outlen   The amount of data expected to be read.
 *
 * @see _impl_lp_lbaToAddr to convert the LBA into an absolute bytes offset.
 */
static int
_impl_lp_devctlRead(PspIoDrvFileArg *arg, const char *devname,
                    unsigned int cmd, lp_UmdLba *param, int inlen,
                    void *outdata, int outlen)
{
    u32 offset = _impl_lp_UmdLba_toAddr(param);
    int ret;

    if (first_read) {
        ret = reserveUmdFuncs.IoDevctl(arg, devname, cmd, param, inlen, outdata, outlen);

        _impl_lp_readDiscHeader(arg, devname);
        strncpy(umd_id, hdr + 0x373, 10);
        umd_id[10] = 0;
        Kprintf("Disc ID obtained: '%s'\n", umd_id);

        first_read = 0;

        return ret;
    }

passthru:
    return reserveUmdFuncs.IoDevctl(arg, devname, cmd, param, inlen, outdata, outlen);
}

int
patched_IoDevctl(PspIoDrvFileArg *arg, const char *devname,
                 unsigned int cmd, void *indata, int inlen,
                 void *outdata, int outlen)
{
    switch ((lp_UmdDevctl) cmd) {
    case lp_UmdDevctl_READ_GENERAL:
    case lp_UmdDevctl_READ_CACHE:
    case lp_UmdDevctl_READ_SECTORS:
        return _impl_lp_devctlRead(arg, devname, cmd, (lp_UmdLba *) indata,
                                   inlen, outdata, outlen);
    default:
        goto passthru;
    }

passthru:
    return reserveUmdFuncs.IoDevctl(arg, devname, cmd, indata, inlen, outdata, outlen);
}


int
patched_IoRead(PspIoDrvFileArg *arg, char *data, int len)
{
    Kprintf("Reading UMD data, hum dee dum...\n");
    return reserveUmdFuncs.IoRead(arg, data, len);
}

int
patched_IoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
    Kprintf("Opening UMD.\n");
    return reserveUmdFuncs.IoOpen(arg, file, flags, mode);
}

void
lp_pingDiscRemoved(void)
{
    first_read = 1;
}

// vim: ft=c.doxygen
