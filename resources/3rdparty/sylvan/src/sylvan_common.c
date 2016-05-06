/*
 * Copyright 2011-2015 Formal Methods and Tools, University of Twente
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sylvan_config.h>

#include <sylvan.h>
#include <sylvan_common.h>

#ifndef cas
#define cas(ptr, old, new) (__sync_bool_compare_and_swap((ptr),(old),(new)))
#endif

/**
 * Static global variables
 */

llmsset_t nodes;

/**
 * Retrieve nodes
 */

llmsset_t
__sylvan_get_internal_data()
{
    return nodes;
}

/**
 * Calculate table usage (in parallel)
 */
VOID_TASK_IMPL_2(sylvan_table_usage, size_t*, filled, size_t*, total)
{
    size_t tot = llmsset_get_size(nodes);
    if (filled != NULL) *filled = llmsset_count_marked(nodes);
    if (total != NULL) *total = tot;
}

/**
 * Implementation of garbage collection
 */
static int gc_enabled = 1;
static volatile int gc; // variable used in cas switch to ensure only one gc at a time

struct reg_gc_mark_entry
{
    struct reg_gc_mark_entry *next;
    gc_mark_cb cb;
    int order;
};

static struct reg_gc_mark_entry *gc_mark_register = NULL;

void
sylvan_gc_add_mark(int order, gc_mark_cb cb)
{
    struct reg_gc_mark_entry *e = (struct reg_gc_mark_entry*)malloc(sizeof(struct reg_gc_mark_entry));
    e->cb = cb;
    e->order = order;
    if (gc_mark_register == NULL || gc_mark_register->order>order) {
        e->next = gc_mark_register;
        gc_mark_register = e;
        return;
    }
    struct reg_gc_mark_entry *f = gc_mark_register;
    for (;;) {
        if (f->next == NULL) {
            e->next = NULL;
            f->next = e;
            return;
        }
        if (f->next->order > order) {
            e->next = f->next;
            f->next = e;
            return;
        }
        f = f->next;
    }
}

static gc_hook_cb gc_hook;

void
sylvan_gc_set_hook(gc_hook_cb new_hook)
{
    gc_hook = new_hook;
}

void
sylvan_gc_enable()
{
    gc_enabled = 1;
}

void
sylvan_gc_disable()
{
    gc_enabled = 0;
}

/* Mark hook for cache */
VOID_TASK_0(sylvan_gc_mark_cache)
{
    /* We simply clear the cache.
     * Alternatively, we could implement for example some strategy
     * where part of the cache is cleared and part is marked
     */
    cache_clear();
}

/* Default hook */

size_t
next_size(size_t n)
{
#if SYLVAN_SIZE_FIBONACCI
    size_t f1=1, f2=1;
    for (;;) {
        f2 += f1;
        if (f2 > n) return f2;
        f1 += f2;
        if (f1 > n) return f1;
    }
#else
    return n*2;
#endif
}

VOID_TASK_IMPL_0(sylvan_gc_aggressive_resize)
{
    /**
     * Always resize when gc called
     */
    size_t max_size = llmsset_get_max_size(nodes);
    size_t size = llmsset_get_size(nodes);
    if (size < max_size) {
        size_t new_size = next_size(size);
        if (new_size > max_size) new_size = max_size;
        llmsset_set_size(nodes, new_size);
        size_t cache_size = cache_getsize();
        size_t cache_max = cache_getmaxsize();
        if (cache_size < cache_max) {
            new_size = next_size(cache_size);
            if (new_size > cache_max) new_size = cache_max;
            cache_setsize(new_size);
        }
    }
}

VOID_TASK_IMPL_0(sylvan_gc_default_hook)
{
    /**
     * Default behavior:
     * if we can resize the nodes set, and if we use more than 50%, then increase size
     */
    size_t max_size = llmsset_get_max_size(nodes);
    size_t size = llmsset_get_size(nodes);
    if (size < max_size) {
        size_t marked = llmsset_count_marked(nodes);
        if (marked*2 > size) {
            size_t new_size = next_size(size);
            if (new_size > max_size) new_size = max_size;
            llmsset_set_size(nodes, new_size);
            size_t cache_size = cache_getsize();
            size_t cache_max = cache_getmaxsize();
            if (cache_size < cache_max) {
                new_size = next_size(cache_size);
                if (new_size > cache_max) new_size = cache_max;
                cache_setsize(new_size);
            }
        }
    }
}

VOID_TASK_0(sylvan_gc_call_hook)
{
    // call hook function (resizing, reordering, etc)
    WRAP(gc_hook);
}

VOID_TASK_0(sylvan_gc_rehash)
{
    // rehash marked nodes
    llmsset_rehash(nodes);
}

VOID_TASK_0(sylvan_gc_destroy_unmarked)
{
    llmsset_destroy_unmarked(nodes);
}

VOID_TASK_0(sylvan_gc_go)
{
    sylvan_stats_count(SYLVAN_GC_COUNT);
    sylvan_timer_start(SYLVAN_GC);

    // clear hash array
    llmsset_clear(nodes);

    // call mark functions, hook and rehash
    struct reg_gc_mark_entry *e = gc_mark_register;
    while (e != NULL) {
        WRAP(e->cb);
        e = e->next;
    }

    sylvan_timer_stop(SYLVAN_GC);
}

/* Perform garbage collection */
VOID_TASK_IMPL_0(sylvan_gc)
{
    if (!gc_enabled) return;
    if (cas(&gc, 0, 1)) {
        NEWFRAME(sylvan_gc_go);
        gc = 0;
    } else {
        /* wait for new frame to appear */
        while (*(Task* volatile*)&(lace_newframe.t) == 0) {}
        lace_yield(__lace_worker, __lace_dq_head);
    }
}

/**
 * Package init and quit functions
 */
void
sylvan_init_package(size_t tablesize, size_t maxsize, size_t cachesize, size_t max_cachesize)
{
    if (tablesize > maxsize) tablesize = maxsize;
    if (cachesize > max_cachesize) cachesize = max_cachesize;

    if (maxsize > 0x000003ffffffffff) {
        fprintf(stderr, "sylvan_init_package error: tablesize must be <= 42 bits!\n");
        exit(1);
    }

    nodes = llmsset_create(tablesize, maxsize);
    cache_create(cachesize, max_cachesize);

    gc = 0;
#if SYLVAN_AGGRESSIVE_RESIZE
    gc_hook = TASK(sylvan_gc_aggressive_resize);
#else
    gc_hook = TASK(sylvan_gc_default_hook);
#endif
    sylvan_gc_add_mark(10, TASK(sylvan_gc_mark_cache));
    sylvan_gc_add_mark(19, TASK(sylvan_gc_destroy_unmarked));
    sylvan_gc_add_mark(20, TASK(sylvan_gc_call_hook));
    sylvan_gc_add_mark(30, TASK(sylvan_gc_rehash));

    LACE_ME;
    sylvan_stats_init();
}

struct reg_quit_entry
{
    struct reg_quit_entry *next;
    quit_cb cb;
};

static struct reg_quit_entry *quit_register = NULL;

void
sylvan_register_quit(quit_cb cb)
{
    struct reg_quit_entry *e = (struct reg_quit_entry*)malloc(sizeof(struct reg_quit_entry));
    e->next = quit_register;
    e->cb = cb;
    quit_register = e;
}

void
sylvan_quit()
{
    while (quit_register != NULL) {
        struct reg_quit_entry *e = quit_register;
        quit_register = e->next;
        e->cb();
        free(e);
    }

    while (gc_mark_register != NULL) {
        struct reg_gc_mark_entry *e = gc_mark_register;
        gc_mark_register = e->next;
        free(e);
    }

    cache_free();
    llmsset_free(nodes);
}
