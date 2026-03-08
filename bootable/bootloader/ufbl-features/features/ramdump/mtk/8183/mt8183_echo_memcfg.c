/*
 * Copyright (c) 2014 - 2019 Amazon.com, Inc. or its affiliates.  All rights reserved.
 *
 */

#include <compiler.h>
#include <debug.h>
#include <string.h>
#include <arch.h>
#include <platform.h>
#include <target.h>
#include <stdlib.h>
#include <string.h>
#include <arch/ops.h>
#include <platform/mt_typedefs.h>

#include <platform/mtk_wdt.h>
#include <platform/mt_logo.h>
#include <platform/boot_mode.h>
#include <platform/memory_layout.h>
#include <target/cust_display.h>

#include "../../ram_compress.h"
#include "ramdump.h"
#include "mtk8183_ramdump.h"

struct mdump_bank s_mdump_banks[] __attribute__ ((aligned (4)))= {
	{ "DRAM", 0x5EC00000, 0x40000000 },
	{ "",     0x00000000, 0x00000000 }
};

extern unsigned char s_data_buf[];

#ifdef CONFIG_MDUMP_COMPRESS
struct compress_segment_request segment_reqs[6] = {
	{ /* segment 1: region 0x60300000-DRAM_END; normal compress */
	.start_phyaddr = (mz_uint8 *)COMPRESS_START_ADDRESS,
	.seg_origin_size = 0x52C00000,  /* Automatic setting */
	.seg_order = 6,
	.compress_type = BOOTC_NORMAL_COMPRESS,
	},
	{ /* segment 2: region 0x40000000-0x56000000; normal compress */
	.start_phyaddr = (mz_uint8 *)0x40000000,
	.seg_origin_size = LK_LK_BASE - 0x40000000,
	.seg_order = 2,
	.compress_type = BOOTC_NORMAL_COMPRESS,
	},
	{ /* segment 3: region 0x56000000-0x56400000; skip area for lk */
	.start_phyaddr = (mz_uint8 *)(LK_LK_BASE),
	.seg_origin_size = MEMSIZE, /* exclude LK_LK_BASE(0x56000000) -> 0x56400000 */
	.seg_order = 3,
	.compress_type = BOOTC_ALL_SAME,
	.same_val_in_byte = 0,
	},

	{ /* segment 3: region 0x56400000-0x60000000; normal compress */
	.start_phyaddr = (mz_uint8 *)(LK_LK_BASE + MEMSIZE),
	.seg_origin_size = CONFIG_MDUMP_RESERVED_ADDRESS  - LK_LK_BASE - MEMSIZE, /* exclude LK_LK_BASE(0x56000000) -> 0x56400000 */
	.seg_order = 4,
	.compress_type = BOOTC_NORMAL_COMPRESS,
	},
	{ /* segment 3: region 0x60000000-0x60300000;
	     mdump/scratch area, skip (fill area with 0) */
	.start_phyaddr = (mz_uint8 *)CONFIG_MDUMP_RESERVED_ADDRESS,
	.seg_origin_size = CONFIG_MDUMP_RESERVED_SIZE,
	.seg_order = 5,
	.compress_type = BOOTC_ALL_SAME,
	.same_val_in_byte = 0,
	},
	{ /* segment 5: mdump header; no compress */
	.start_phyaddr = (mz_uint8 *)s_data_buf,
	.seg_origin_size = MDUMP_BLOCK_SIZE,
	.seg_order = 1,
	.compress_type = BOOTC_NO_COMPRESS,
	},
};

struct compress_fullram_request compress_req = {
	.compress_to_phyaddr = (mz_uint8 *)COMPRESS_START_ADDRESS,
	.total_memsize = 0x5EC00000,
	.scratch_phyaddr = (mz_uint8 *)COMPRESS_SCRATCH_ADDRESS,
	.scratch_area_size = COMPRESS_SCRATCH_SIZE,
	.first_compress_buffer = (mz_uint8 *)COMPRESS_FIRST_BUFFER,
	.first_buffer_size = COMPRESS_FIRST_BUFFER_SIZE,
	.chunk_upper_limit = 64*COMPRESS_1MB,
	.num_of_segments = 6,
	.seg_reqs = segment_reqs,
	.progress_report_callback = NULL,
	.system_ping_callback = NULL,
};

extern u32 memory_size(void);
void adjust_mem_cfg(void)
{
	s_mdump_banks[0].size = compress_req.total_memsize = memory_size();
	segment_reqs[0].seg_origin_size = compress_req.total_memsize - ((mz_uint32)segment_reqs[0].start_phyaddr - 0x40000000);
};
#endif
