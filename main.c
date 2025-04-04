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
#include <pspumd.h>

PSP_MODULE_INFO("umd_livepatch", PSP_MODULE_KERNEL, 2, 1);


void lp_patchFunction(u32 addr, void *newaddr, void *fptr);

PspIoDrv *umdDriver;
PspIoDrvFuncs reserveUmdFuncs;

SceUID vshCallbackId = 0;
SceUID umdCallbackId;
SceUID umdCallbackThread;

#define MAX_MODULE_NUMBER 256



int lp_discChangeCallback(int unk, int event, void *data)
{
    if (event == PSP_UMD_NOT_PRESENT)
        lp_pingDiscRemoved();
    if (vshCallbackId)
        sceKernelNotifyCallback(vshCallbackId, event);
    return 0;
}


int lp_discChangeWatcher(SceSize argc, void *argp)
{
    SceUID callbacks[50];
    int count;
    SceKernelCallbackInfo cbinfo;

    sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Callback, callbacks, 50, &count);

    for (int i = 0; i < count; i++) {
        sceKernelReferCallbackStatus(callbacks[i], &cbinfo);
        if (!strcmp(cbinfo.name, "SceVshMediaDetectUMD")) {
            Kprintf("Found VSH UMD callback: 0x%08x\n", vshCallbackId);
            vshCallbackId = callbacks[i];
            break;
        }
    }

    umdCallbackId = sceKernelCreateCallback("lp_discChangeCallback",
                                            lp_discChangeCallback,
                                            NULL);
    sceUmdRegisterUMDCallBack(umdCallbackId);

    sceKernelSleepThreadCB();
}

int module_start(SceSize argc, void *argp)
{
    int ret;

    Kprintf("------------------\nUMD Livepatch starting...\n");
#ifdef CUSTOM_PATH
    //get_plugin_path(argp);
#endif

    umdDriver = sctrlHENFindDriver("umd");
    if (umdDriver) {
        reserveUmdFuncs = *umdDriver->funcs;
        Kprintf("Found UMD driver at 0x%08x\n", umdDriver);
    } else {
        Kprintf("Could not find UMD driver...\n");
        return 1;
    }

    umdCallbackThread = sceKernelCreateThread("lp_discChangeWatcher",
                                              lp_discChangeWatcher,
                                              0x10, 0x1000, 0, NULL);
    sceKernelStartThread(umdCallbackThread, 0, NULL);

    umdDriver->funcs->IoRead = patched_IoRead;
    umdDriver->funcs->IoOpen = patched_IoOpen;
    umdDriver->funcs->IoDevctl = patched_IoDevctl;

    return 0;
}

int module_stop(void)
{
    Kprintf("Unloading UMD Livepatch.");

    *umdDriver->funcs = reserveUmdFuncs;
    Kprintf("Restored original UMD driver functions.\n");

    sceUmdUnRegisterUMDCallBack(umdCallbackId);
    sceKernelDeleteCallback(umdCallbackId);

    if (vshCallbackId)
        sceUmdRegisterUMDCallBack(vshCallbackId);
    Kprintf("Disconnected drive state callback.\n");
    return 0;
}



void infernoSetDiscType(void) {}
void infernoCacheSetPolicy(void) {}
void infernoCacheInit(void) {}
void infernoSetUmdDelay(void) {}

// vim: ft=c.doxygen
