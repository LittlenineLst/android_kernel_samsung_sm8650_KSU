// SPDX-License-Identifier: GPL-2.0-only
/*
 * Generic show_mem() implementation
 *
 * Copyright (C) 2008 Johannes Weiner <hannes@saeurebad.de>
 */

#include <linux/mm.h>
#include <linux/cma.h>
#include <linux/vmalloc.h>
#include <trace/hooks/mm.h>

#define K(x) ((x) << (PAGE_SHIFT-10))

#ifdef CONFIG_HUGEPAGE_POOL
extern unsigned long total_hugepage_pool_pages(void);
#endif

void __show_mem(unsigned int filter, nodemask_t *nodemask, int max_zone_idx)
{
	pg_data_t *pgdat;
	unsigned long total = 0, reserved = 0, highmem = 0;

	printk("Mem-Info:\n");
	__show_free_areas(filter, nodemask, max_zone_idx);

	for_each_online_pgdat(pgdat) {
		int zoneid;

		for (zoneid = 0; zoneid < MAX_NR_ZONES; zoneid++) {
			struct zone *zone = &pgdat->node_zones[zoneid];
			if (!populated_zone(zone))
				continue;

			total += zone->present_pages;
			reserved += zone->present_pages - zone_managed_pages(zone);

			if (is_highmem_idx(zoneid))
				highmem += zone->present_pages;
		}
	}

	printk("%lu pages RAM\n", total);
	printk("%lu pages HighMem/MovableOnly\n", highmem);
	printk("%lu pages reserved\n", reserved);
#ifdef CONFIG_CMA
	printk("%lu pages cma reserved\n", totalcma_pages);
#endif
#ifdef CONFIG_MEMORY_FAILURE
	printk("%lu pages hwpoisoned\n", atomic_long_read(&num_poisoned_pages));
#endif
	pr_info("%s: %lu kB\n", "VmallocUsed", K(vmalloc_nr_pages()));
	trace_android_vh_show_mem(filter, nodemask);
	pr_info("%s: %lu kB\n", "GpuSwap", K(gpu_page_reclaimed_pages()));
	pr_info("%s: %lu kB\n", "KgslShmemUsage", K(gpu_page_shmem_pages()));
#ifdef CONFIG_HUGEPAGE_POOL
	pr_info("%s: %lu kB\n", "HugepagePool", K(total_hugepage_pool_pages()));
#endif
}
