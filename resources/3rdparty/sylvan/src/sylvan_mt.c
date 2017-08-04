/*
 * Copyright 2011-2016 Formal Methods and Tools, University of Twente
 * Copyright 2016-2017 Tom van Dijk, Johannes Kepler University Linz
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

#include <sylvan_int.h> // for llmsset*, nodes, sylvan_register_quit

#include <inttypes.h>
#include <string.h>

/**
 * Handling of custom leaves "registry"
 */

typedef struct
{
    sylvan_mt_hash_cb hash_cb;
    sylvan_mt_equals_cb equals_cb;
    sylvan_mt_create_cb create_cb;
    sylvan_mt_destroy_cb destroy_cb;
    sylvan_mt_to_str_cb to_str_cb;
    sylvan_mt_write_binary_cb write_binary_cb;
    sylvan_mt_read_binary_cb read_binary_cb;
} customleaf_t;

static customleaf_t *cl_registry;
static size_t cl_registry_count;
static size_t cl_registry_size;

/**
 * Implementation of hooks for llmsset
 */

/**
 * Internal helper function
 */
static inline customleaf_t*
sylvan_mt_from_node(uint64_t a, uint64_t b)
{
    uint32_t type = a & 0xffffffff;
    assert(type < cl_registry_count);
    return cl_registry + type;
    (void)b;
}

static void
_sylvan_create_cb(uint64_t *a, uint64_t *b)
{
    customleaf_t *c = sylvan_mt_from_node(*a, *b);
    if (c->create_cb != NULL) c->create_cb(b);
}

static void
_sylvan_destroy_cb(uint64_t a, uint64_t b)
{
    // for leaf
    customleaf_t *c = sylvan_mt_from_node(a, b);
    if (c->destroy_cb != NULL) c->destroy_cb(b);
}

static uint64_t
_sylvan_hash_cb(uint64_t a, uint64_t b, uint64_t seed)
{
    customleaf_t *c = sylvan_mt_from_node(a, b);
    if (c->hash_cb != NULL) return c->hash_cb(b, seed ^ a);
    else return llmsset_hash(a, b, seed);
}

static int
_sylvan_equals_cb(uint64_t a, uint64_t b, uint64_t aa, uint64_t bb)
{
    if (a != aa) return 0;
    customleaf_t *c = sylvan_mt_from_node(a, b);
    if (c->equals_cb != NULL) return c->equals_cb(b, bb);
    else return b == bb ? 1 : 0;
}

uint32_t
sylvan_mt_create_type()
{
    if (cl_registry_count == cl_registry_size) {
        // resize registry array
        cl_registry_size += 8;
        cl_registry = (customleaf_t *)realloc(cl_registry, sizeof(customleaf_t) * (cl_registry_size));
        memset(cl_registry + cl_registry_count, 0, sizeof(customleaf_t) * (cl_registry_size-cl_registry_count));
    }
    return cl_registry_count++;
}

void sylvan_mt_set_hash(uint32_t type, sylvan_mt_hash_cb hash_cb)
{
    customleaf_t *c = cl_registry + type;
    c->hash_cb = hash_cb;
}

void sylvan_mt_set_equals(uint32_t type, sylvan_mt_equals_cb equals_cb)
{
    customleaf_t *c = cl_registry + type;
    c->equals_cb = equals_cb;
}

void sylvan_mt_set_create(uint32_t type, sylvan_mt_create_cb create_cb)
{
    customleaf_t *c = cl_registry + type;
    c->create_cb = create_cb;
}

void sylvan_mt_set_destroy(uint32_t type, sylvan_mt_destroy_cb destroy_cb)
{
    customleaf_t *c = cl_registry + type;
    c->destroy_cb = destroy_cb;
}

void sylvan_mt_set_to_str(uint32_t type, sylvan_mt_to_str_cb to_str_cb)
{
    customleaf_t *c = cl_registry + type;
    c->to_str_cb = to_str_cb;
}

void sylvan_mt_set_write_binary(uint32_t type, sylvan_mt_write_binary_cb write_binary_cb)
{
    customleaf_t *c = cl_registry + type;
    c->write_binary_cb = write_binary_cb;
}

void sylvan_mt_set_read_binary(uint32_t type, sylvan_mt_read_binary_cb read_binary_cb)
{
    customleaf_t *c = cl_registry + type;
    c->read_binary_cb = read_binary_cb;
}

/**
 * Initialize and quit functions
 */

static int mt_initialized = 0;

static void
sylvan_mt_quit()
{
    if (mt_initialized == 0) return;
    mt_initialized = 0;

    free(cl_registry);
    cl_registry = NULL;
    cl_registry_count = 0;
    cl_registry_size = 0;
}

void
sylvan_init_mt()
{
    if (mt_initialized) return;
    mt_initialized = 1;

    // Register quit handler to free structures
    sylvan_register_quit(sylvan_mt_quit);

    // Tell llmsset to use our custom hooks
    llmsset_set_custom(nodes, _sylvan_hash_cb, _sylvan_equals_cb, _sylvan_create_cb, _sylvan_destroy_cb);

    // Initialize data structures
    cl_registry_size = 8;
    cl_registry = (customleaf_t *)calloc(sizeof(customleaf_t), cl_registry_size);
    cl_registry_count = 3; // 0, 1, 2 are taken
}

/**
 * Return 1 if the given <type> has a custom hash callback, or 0 otherwise.
 */
int
sylvan_mt_has_custom_hash(uint32_t type)
{
    assert(type < cl_registry_count);
    customleaf_t *c = cl_registry + type;
    return c->hash_cb != NULL ? 1 : 0;
}

/**
 * Convert a leaf (possibly complemented) to a string representation.
 * If it does not fit in <buf> of size <buflen>, returns a freshly allocated char* array.
 */
char*
sylvan_mt_to_str(int complement, uint32_t type, uint64_t value, char* buf, size_t buflen)
{
    assert(type < cl_registry_count);
    customleaf_t *c = cl_registry + type;
    if (type == 0) {
        size_t required = (size_t)snprintf(NULL, 0, "%" PRId64, (int64_t)value);
        char *ptr = buf;
        if (buflen < required) {
            ptr = (char*)malloc(required);
            buflen = required;
        }
        if (ptr != NULL) snprintf(ptr, buflen, "%" PRId64, (int64_t)value);
        return ptr;
    } else if (type == 1) {
        size_t required = (size_t)snprintf(NULL, 0, "%f", *(double*)&value);
        char *ptr = buf;
        if (buflen < required) {
            ptr = (char*)malloc(required);
            buflen = required;
        }
        if (ptr != NULL) snprintf(ptr, buflen, "%f", *(double*)&value);
        return ptr;
    } else if (type == 2) {
        int32_t num = (int32_t)(value>>32);
        uint32_t denom = value&0xffffffff;
        size_t required = (size_t)snprintf(NULL, 0, "%" PRId32 "/%" PRIu32, num, denom);
        char *ptr = buf;
        if (buflen < required) {
            ptr = (char*)malloc(required);
            buflen = required;
        }
        if (ptr != NULL) snprintf(ptr, buflen, "%" PRId32 "/%" PRIu32, num, denom);
        return ptr;
    } else if (c->to_str_cb != NULL) {
        return c->to_str_cb(complement, value, buf, buflen);
    } else {
        return NULL;
    }
}

uint64_t
sylvan_mt_hash(uint32_t type, uint64_t value, uint64_t seed)
{
    assert(type < cl_registry_count);
    customleaf_t *c = cl_registry + type;
    if (c->hash_cb != NULL) return c->hash_cb(value, seed);
    else return llmsset_hash((uint64_t)type, value, seed);
}

int
sylvan_mt_write_binary(uint32_t type, uint64_t value, FILE *out)
{
    assert(type < cl_registry_count);
    customleaf_t *c = cl_registry + type;
    if (c->write_binary_cb != NULL) return c->write_binary_cb(out, value);
    else return 0;
}

int
sylvan_mt_read_binary(uint32_t type, uint64_t *value, FILE *in)
{
    assert(type < cl_registry_count);
    customleaf_t *c = cl_registry + type;
    if (c->read_binary_cb != NULL) return c->read_binary_cb(in, value);
    else return 0;
}
