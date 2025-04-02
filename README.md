# UMD Live Patch

This project is a _system module_ for the PlayStation Portable, specifically the [ARK-4] custom firmware.

## Setup

As with any other ARK-4 plugin, place the `umd_livepatch.prx` file in the `SEPLUGINS` directory in your MemoryStick, then enable the plugin for the `VSH` and `UMD` modes.

The `VSH` mode is optional, but will allow you to see the patched game info for your game.

### Patching a game

For performance reasons, this system module does not accept `xdelta3` files you might commonly find. Instead, it uses its own format: UMDiff.

For a given game, copy the `umdiff` file to `/SEPLUGINS/UMDPATCH/` in your MemoryStick, renaming it to match the game ID you want to patch.

For example, a patch for _K-ON! Ho-Kago Live!!_ (game ID `ULJM-05709`) will be saved under `/SEPLUGINS/UMDPATCH/ULJM-05709.UMDIFF`.

> [!WARNING]
> This plugin **does not** perform integrity checks. It will gleefully "patch"
> the wrong file if you tell it to, which will probably prevent the game from booting.

### Generating a UMDiff file

This repository contains the `umdiff` tool to generate a UMDiff file.


[ARK-4]: https://github.com/psp-archive/ARK-4
