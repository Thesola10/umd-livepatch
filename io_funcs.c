#include "io_funcs.h"
#include <string.h>

extern PspIoDrvFuncs reserveUmdFuncs;

static char umd_id[11] = {0};

static char first_read = 1;

static char hdr[ISO_SECTOR_SIZE];

static inline u32
_impl_lp_lbaToAddr(struct LbaParams *param)
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

static inline int
_impl_lp_readDiscHeader(PspIoDrvFileArg *arg, const char *devname, void *outdata)
{
    int ret;

    struct LbaParams param = {
        .unknown1 = 0,
        .cmd = 0, /* read */
        .lba_top = 0x8000 / ISO_SECTOR_SIZE,
        .lba_size = 1,
        .byte_size_total = ISO_SECTOR_SIZE,
        .byte_size_centre = ISO_SECTOR_SIZE,
        .byte_size_start = 0,
        .byte_size_last = 0
    };

    ret = reserveUmdFuncs.IoDevctl(arg, devname, lp_UmdIoctl_READ_SECTORS,
                                   &param, sizeof param, outdata, ISO_SECTOR_SIZE);
    return ret;
}


static int
_impl_lp_devctlRead(PspIoDrvFileArg *arg, const char *devname,
                    unsigned int cmd, struct LbaParams *param, int inlen,
                    void *outdata, int outlen)
{
    u32 offset = _impl_lp_lbaToAddr(param);
    int ret;

    if (first_read) {
        ret = reserveUmdFuncs.IoDevctl(arg, devname, cmd, param, inlen, outdata, outlen);

        _impl_lp_readDiscHeader(arg, devname, &hdr);
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
    switch ((lp_UmdIoctl) cmd) {
    case lp_UmdIoctl_READ_GENERAL:
    case lp_UmdIoctl_READ_CACHE:
    case lp_UmdIoctl_READ_SECTORS:
        return _impl_lp_devctlRead(arg, devname, cmd, (struct LbaParams *) indata,
                                   inlen, outdata, outlen);
    default:
        goto passthru;
    }

passthru:
    return reserveUmdFuncs.IoDevctl(arg, devname, cmd, indata, inlen, outdata, outlen);
}
