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

#include <pspkernel.h>
#include <string.h>

PSP_MODULE_INFO("umd_livepatch", PSP_MODULE_KERNEL, 0, 8);

int module_found = 0;
int loader_found = 0;

#define MAX_MODULE_NUMBER 256


int module_start(SceSize argc, void *argp)
{
    Kprintf("------------------\nUMD Livepatch starting...\n");
#ifdef CUSTOM_PATH
    //get_plugin_path(argp);
#endif
}

int module_stop(void)
{
    Kprintf("Unloading UMD Livepatch.");
}


void infernoSetDiscType(void) {}
void infernoCacheSetPolicy(void) {}
void infernoCacheInit(void) {}
void infernoSetUmdDelay(void) {}

// vim: ft=c.doxygen
