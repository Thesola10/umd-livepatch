#include <pspkernel.h>
#include <pspdisplay_kernel.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <string.h>
#include <systemctrl.h>

#define _impl_lp_mkJump$(a, f) _sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), a)
#define _impl_lp_mkCall$(a, f) _sw(0x0C000000 | (((u32)(f) >> 2)  & 0x03ffffff), a)

#define _impl_lp_mkRedirect$(a, f) {                            \
    u32 address = a;                                            \
    _sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), address); \
    _sw(0, address+4);                                          \
}

void sceDisplaySetBrightness_Patched(int brightness, int unk1)
{
	//Set Brightness here
	sceDisplaySetBrightness(brightness, unk1);
}

u32 lp_findSyscall(const char *modname, const char *lib, u32 nid)
{
	int i = 0, u;
	
	/* try and find the module by name */
	SceModule *mod = sceKernelFindModuleByName(modname);
	
	/* if fail */
	if (!mod)
	{
		/* fail */
		return 0;
	}
	
	/* copy over the structure data */
	u32 entry_size = mod->ent_size;
	u32 entry_start = (u32)mod->ent_top;
	
	/* loop until end of entry table */
	while (i < entry_size)
	{
		/* cast structure to memory */
		SceLibraryEntryTable *entry = (SceLibraryEntryTable *)(entry_start + i);
		
		/* if there is a libname, compare it to the lib else if there is no lib and there is no libname */
		if ((entry->libname && (strcmp(entry->libname, lib) == 0)) || (lib == NULL && entry->libname == NULL))
		{
			/* copy the table pointer and get the total number of entries */
			u32 *table = entry->entrytable;
			int total = entry->stubcount + entry->vstubcount;
			
			/* if there is some entries continue */
			if (total > 0)
			{ 
				/* loop through the entries */
				for (u = 0; u < total; u++)
				{
					/* if the nid matches */
					if (table[u] == nid)
					{
						/* return the pointer */
						return table[u + total];
					}
				} 
			} 	
		}
		
		/* increment the counter */
		i += (entry->len << 2);
	}
	
	/* could not find function */
	return 0;
}

void lp_patchSyscall(u32 addr, void *newaddr)
{
    u32 *vectors, i;

    /* get the vectors address from the co-processor */
    __asm__ volatile ("cfc0 %0, $12\n" "nop\n" : "=r" (vectors));

    /* loop through them */
    for (i = 0; i < 0x1000; i++) {
        /* if this is the address */
        if (vectors[i + 4] == addr) {
            /* then replace it :D */
            vectors[i + 4] = (u32) newaddr;
        }
    }
}

void lp_patchFunction(u32 addr, void *newaddr, void *fptr)
{
    static u32 patch_buffer[3];
    _sw(_lw(addr + 0x00), (u32) patch_buffer + 0x00);
    _sw(_lw(addr + 0x04), (u32) patch_buffer + 0x08);
    _impl_lp_mkJump$((u32) patch_buffer + 0x04, addr + 0x08);
    _impl_lp_mkRedirect$(addr, newaddr);
    fptr = (void *)patch_buffer;
}

static void ClearCaches(void)
{
	sceKernelIcacheClearAll();
	sceKernelDcacheWritebackAll();
}

static void PatchBrightness(void)
{
    /* find the Brightness module */
    u32 text_addr = lp_findSyscall("sceDisplay_Service", "sceDisplay_driver",  0xFF5A5D52); // 6.20 NID
    if (text_addr != 0) {
        /* patch the Brightness set */
        lp_patchSyscall(text_addr, sceDisplaySetBrightness_Patched);
        /* ok, lets patch it */
        lp_patchFunction(text_addr, sceDisplaySetBrightness_Patched, sceDisplaySetBrightness);
        // Clear caches
        ClearCaches();
    }
}
