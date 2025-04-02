#ifndef __IO_FUNCS_H
#define __IO_FUNCS_H

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
    lp_UmdIoctl_01F00003  = 0x01F00003,
    lp_UmdIoctl_01F010DB  = 0x01F010DB,

    lp_UmdIoctl_DISC_TYPE = 0x01F20001,
    lp_UmdIoctl_SEEK_RAW  = 0x01F100A3,
    lp_UmdIoctl_CACHE_ADD  = 0x01F100A4,
    lp_UmdIoctl_CACHE_ADD2 = 0x01F300A5,

    /* Cache control, worth intercepting */
    lp_UmdIoctl_01F300A7  = 0x01F300A7,
    lp_UmdIoctl_01F300A8  = 0x01F300A8,
    lp_UmdIoctl_01F300A9  = 0x01F300A9,
    lp_UmdIoctl_NUM_SECTORS  = 0x01F20002,
    lp_UmdIoctl_NUM_SECTORS2 = 0x01F20003,
    lp_UmdIoctl_01E18030  = 0x01E18030,
    lp_UmdIoctl_01E180D3  = 0x01E180D3,
    lp_UmdIoctl_01E080A8  = 0x01E080A8,
    lp_UmdIoctl_GET_SECTOR_BUF = 0x01E28035,
    lp_UmdIoctl_GET_SECTOR_SIZE = 0x01E280A9,
    lp_UmdIoctl_01E38034  = 0x01E38034,
    lp_UmdIoctl_READ_GENERAL = 0x01E380C0,
    lp_UmdIoctl_READ_SECTORS = 0x01F200A1,
    lp_UmdIoctl_READ_CACHE   = 0x01F200A2,
    lp_UmdIoctl_GET_INFO     = 0x01E38012
} lp_UmdIoctl;

struct LbaParams {
    int unknown1; // 0
    int cmd; // 4
    int lba_top; // 8
    int lba_size; // 12
    int byte_size_total;  // 16
    int byte_size_centre; // 20
    int byte_size_start; // 24
    int byte_size_last;  // 28
};

#define ISO_SECTOR_SIZE 2048

int
patched_IoDevctl(PspIoDrvFileArg *arg, const char *devname,
                 unsigned int cmd, void *indata, int inlen,
                 void *outdata, int outlen);

#endif //__IO_FUNCS_H
