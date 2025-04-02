/**
 * @file        main.c
 * @author      Karim Vergnes <me@thesola.io>
 * @copyright   GPLv2
 * @brief       live patch application module for UMD games
 *
 * This module intercepts read commands from UMD, and selectively replaces them
 * with contents from a given rdiff file.
 * This allows patches such as translations to be applied to physical media
 * without requiring a dump.
 */

#include "io_funcs.h"
#include <string.h>

PSP_MODULE_INFO("umd_livepatch", PSP_MODULE_KERNEL, 2, 1);


void lp_patchFunction(u32 addr, void *newaddr, void *fptr);

PspIoDrvFuncs reserveUmdFuncs;

PspIoDrvFuncs patchedUmdFuncs;

PspIoDrv originalUmdDriver;

PspIoDrv reserveUmdDriver = {
    .name = "umdraw",
    .dev_type = 4,
    .unk2 = 0x800,
    .name2 = "UMD_RAW",
    .funcs = &reserveUmdFuncs
};

PspIoDrv patchedUmdDriver = {
    .name = "umd",
    .dev_type = 4, // block device
    .unk2 = 0x800,
    .name2 = "UMD9660",
    .funcs = &patchedUmdFuncs
};

#define MAX_MODULE_NUMBER 256

// Bogus read to test intercepting
static int patched_IoRead(PspIoDrvFileArg *arg, char *data, int len)
{
    Kprintf("Reading UMD data, hum dee dum...\n");
    return reserveUmdFuncs.IoRead(arg, data, len);
}

int module_start(SceSize argc, void *argp)
{
    PspIoDrv *umdDriver = 0;
    int ret;

    Kprintf("------------------\nUMD Livepatch starting...\n");
#ifdef CUSTOM_PATH
    //get_plugin_path(argp);
#endif

    umdDriver = sctrlHENFindDriver("umd");
    if (umdDriver) {
        originalUmdDriver = *umdDriver;
        reserveUmdFuncs = *umdDriver->funcs;
        Kprintf("Found UMD driver at 0x%08x\n", umdDriver);
    } else {
        Kprintf("Could not find UMD driver...\n");
        return 1;
    }

    patchedUmdFuncs = reserveUmdFuncs;

    umdDriver->funcs->IoRead = patched_IoRead;
    umdDriver->funcs->IoDevctl = patched_IoDevctl;

    return 0;
}

int module_stop(void)
{
    Kprintf("Unloading UMD Livepatch.");
    sceIoDelDrv("umd");

    sceIoAddDrv(&originalUmdDriver);
    sceIoDelDrv("umdraw");
    Kprintf("Restored original UMD driver\n");
    return 0;
}



void infernoSetDiscType(void) {}
void infernoCacheSetPolicy(void) {}
void infernoCacheInit(void) {}
void infernoSetUmdDelay(void) {}

// vim: ft=c.doxygen
