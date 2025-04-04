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
#include <macros.h>

PSP_MODULE_INFO("umd_livepatch", PSP_MODULE_KERNEL, 2, 1);


PspIoDrv *umdDriver;
PspIoDrvFuncs reserveUmdFuncs;

SceUID vshCallbackId = 0;
SceUID umdCallbackId;
SceUID umdCallbackThread;

u32 reserveRegisterUmdCallback[4];
u32 fn_RegisterUmdCallback;

#define MAX_MODULE_NUMBER 256


/**
 * @brief      Callback handler for UMD events
 *
 * In order to obtain the UMD's disc ID and select the correct patch file,
 * our module keeps track of the first read command sent by the system. However,
 * without keeping track of drive removal, we might miss a disc change and
 * wrongly patch a different disc, such as on VSH.
 *
 * This callback does two things, in order:
 * - Reset the first read flag by calling @ref lp_pingDiscRemoved
 * - Call the 'guest callback', the callback that was originally meant to handle
 *   UMD drive events.
 *
 * @see lp_discChangeWatcher the entry point for this callback thread
 * @see lp_catchUmdCallback to intercept a UMD callback register request
 */
int lp_discChangeCallback(int unk, int event, void *data)
{
    if (event == PSP_UMD_NOT_PRESENT)
        lp_pingDiscRemoved();
    if (vshCallbackId)
        sceKernelNotifyCallback(vshCallbackId, event);
    return 0;
}

/**
 * @brief      Substitute function for sceUmdRegisterUMDCallBack
 *
 * The UMD driver can only handle one callback at once, and we need our callback
 * to handle switching out discs.
 * This function is useful if umd_livepatch was loaded at boot, before any app
 * had a chance to register a callback, as it allows us to store the callback
 * and call it after our own.
 */
int lp_catchUmdCallback(int cbid)
{
    Kprintf("Caught request to register UMD callback 0x%08x\n", cbid);
    vshCallbackId = cbid;
    sceKernelNotifyCallback(cbid, PSP_UMD_NOT_PRESENT);

    return 0;
}

/**
 * @brief      Entry point for UMD callback thread
 *
 * This function is the entry point for our UMD callback thread. In order to
 * keep things in sync, it is also responsible for setting up our callback
 * redirect system.
 *
 * It performs the following tasks, in order:
 * - Look up a callback named "SceVshMediaDetectUMD" or "DVDUMD", and write it
 *   down as our guest callback.
 * - Register @ref lp_discChangeCallback as the UMD event callback.
 * - Redirect sceUmdRegisterUMDCallBack to @ref lp_catchUmdCallback, taking
 *   care to save the original instructions for cleanup.
 * - Go to sleep and wait for callbacks.
 */
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
        } else if (!strcmp(cbinfo.name, "DVDUMD")) {
            Kprintf("Found game UMD callback: 0x%08x\n", vshCallbackId);
            vshCallbackId = callbacks[i];
            break;
        }
    }

    umdCallbackId = sceKernelCreateCallback("lp_discChangeCallback",
                                            lp_discChangeCallback,
                                            NULL);
    sceUmdRegisterUMDCallBack(umdCallbackId);

    fn_RegisterUmdCallback = sctrlHENFindFunction("sceUmd_driver", "sceUmdUser", 0xAEE7404D);
    if (fn_RegisterUmdCallback) {
        _sw(fn_RegisterUmdCallback, (u32) &reserveRegisterUmdCallback);
        _sw(fn_RegisterUmdCallback + 4, (u32) &reserveRegisterUmdCallback + 4);
        REDIRECT_FUNCTION(fn_RegisterUmdCallback, lp_catchUmdCallback);
    }

    sceKernelSleepThreadCB();

    return 1; // This should be unreachable
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

    sceKernelDeleteThread(umdCallbackThread);

    // put things back where we found them
    _sw((u32) &reserveRegisterUmdCallback, fn_RegisterUmdCallback);
    _sw((u32) &reserveRegisterUmdCallback + 4, fn_RegisterUmdCallback + 4);

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
