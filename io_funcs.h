#ifndef __IO_FUNCS_H
#define __IO_FUNCS_H

/**
 * @file        io_funcs.h
 * @author      Karim Vergnes <me@thesola.io>
 * @copyright   GPLv2
 * @brief       Functions to interpose with UMD driver
 *
 * Functions to parse and manipulate devctl requests to the UMD driver,
 * allowing for live redirection of read requests to a patch file.
 */

#include <pspkernel.h>
#include <systemctrl.h>

/*
    UMD access RAW routine

    lba_param[0] = 0 , unknown
    lba_param[1] = cmd,3 = ctrl-area , 0 = data-read
    lba_param[2] = top of LBA
    lba_param[3] = total LBA size
    lba_param[4] = total byte size
    lba_param[5] = byte size of center LBA
    lba_param[6] = byte size of start  LBA
    lba_param[7] = byte size of last   LBA
 */

typedef enum: int {
    lp_UmdIoctl_01F010DB   = 0x01F010DB,
    lp_UmdIoctl_GET_OFFSET = 0x01D20001,
    lp_UmdIoctl_LSEEK      = 0x01F100A6,
    lp_UmdIoctl_READ       = 0x01F30003
} lp_UmdIoctl;

typedef enum: int {
    lp_UmdDevctl_01F00003  = 0x01F00003,
    lp_UmdDevctl_01F010DB  = 0x01F010DB,

    lp_UmdDevctl_DISC_TYPE = 0x01F20001,
    lp_UmdDevctl_SEEK_RAW  = 0x01F100A3,
    lp_UmdDevctl_CACHE_ADD  = 0x01F100A4,
    lp_UmdDevctl_CACHE_ADD2 = 0x01F300A5,

    /* CacDev control, worth intercepting */
    lp_UmdDevctl_01F300A7  = 0x01F300A7,
    lp_UmdDevctl_01F300A8  = 0x01F300A8,
    lp_UmdDevctl_01F300A9  = 0x01F300A9,
    lp_UmdDevctl_NUM_SECTORS  = 0x01F20002,
    lp_UmdDevctl_NUM_SECTORS2 = 0x01F20003,
    lp_UmdDevctl_01E18030  = 0x01E18030,
    lp_UmdDevctl_01E180D3  = 0x01E180D3,
    lp_UmdDevctl_01E080A8  = 0x01E080A8,
    lp_UmdDevctl_GET_SECTOR_BUF = 0x01E28035,
    lp_UmdDevctl_GET_SECTOR_SIZE = 0x01E280A9,
    lp_UmdDevctl_01E38034  = 0x01E38034,
    lp_UmdDevctl_READ_GENERAL = 0x01E380C0,
    lp_UmdDevctl_READ_SECTORS = 0x01F200A1,
    lp_UmdDevctl_READ_CACHE   = 0x01F200A2,
    lp_UmdDevctl_GET_INFO     = 0x01E38012
} lp_UmdDevctl;

typedef struct {
    int unknown1; // 0
    int cmd; // 4
    int lba_top; // 8
    int lba_size; // 12
    int byte_size_total;  // 16
    int byte_size_centre; // 20
    int byte_size_start; // 24
    int byte_size_last;  // 28
} lp_UmdLba;

#define ISO_SECTOR_SIZE 2048

/**
 * @brief      Replacement handler for UMD devctl.
 *
 * This function is called as a replacement for the UMD driver's devctl handler.
 * Contrary to Inferno2, we have a backing driver to which any unrecognized or
 * irrelevant devctls should be passed through.
 *
 * @see _impl_lp_devctlRead which handles reading data from disc.
 */
int
patched_IoDevctl(PspIoDrvFileArg *arg, const char *devname,
                 unsigned int cmd, void *indata, int inlen,
                 void *outdata, int outlen);

int
patched_IoRead(PspIoDrvFileArg *arg, char *data, int len);

int
patched_IoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode);

/**
 * @brief      Tell this module to expect a new disc.
 *
 * This function resets the "first read" flag which prompts this module to read
 * the UMD's disc ID.
 * On its own, this function does not invalidate the cache. It will only be
 * invalidated if the effective disc ID has changed on the next read devctl.
 */
void
lp_pingDiscRemoved(void);

#endif //__IO_FUNCS_H

// vim: ft=c.doxygen
