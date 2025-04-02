#ifndef __RPATCH_H
#define __RPATCH_H


typedef struct {
    long sector_start;
    long sector_count;

    /* If zero, block is unchanged */
    long patch_start;

    /* If smaller than sector_count, repeating data */
    long patch_len;
} lp_Patch;

typedef struct {

} lp_PatchSet;

#endif //__RPATCH_H
