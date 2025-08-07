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

/**
 * Self-balancing binary tree implemented using C macros
 *
 * See also: http://en.wikipedia.org/wiki/AVL_tree
 * Data structure originally by Adelson-Velskii, Landis, 1962.
 *
 * Usage of this AVL implementation:
 *
 * AVL(some_name, some_type)
 * {
 *    Compare some_type *left with some_type *right
 *    Return <0 when left<right, >0 when left>right, 0 when left=right
 * }
 *
 * You get:
 *  - some_type *some_name_put(avl_node_t **root_node, some_type *data, int *inserted);
 *    Either insert new or retrieve existing key, <inserted> if non-NULL receives 0 or 1.
 *  - int some_name_insert(avl_node_t **root_node, some_type *data);
 *    Try to insert, return 1 if succesful, 0 if existed.
 *  - int some_name_delete(avl_node_t **root_node, some_type *data);
 *    Try to delete, return 1 if deleted, 0 if no match.
 *  - some_type *some_name_search(avl_node_t *root_node, some_type *data);
 *    Retrieve existing data, returns NULL if unsuccesful
 *  - void some_name_free(avl_node_t **root_node);
 *    Free all memory used by the AVL tree
 *  - some_type *some_name_toarray(avl_node_t *root_node);
 *    Malloc an array and put the sorted data in it...
 *  - size_t avl_count(avl_node_t *root_node);
 *    Returns the number of items in the tree
 *
 * For example:
 * struct my_struct { ... };
 * AVL(some_name, struct my_struct)
 * {
 *    Compare struct my_struct *left with struct my_struct *right
 *    Return <0 when left<right, >0 when left>right, 0 when left=right
 * }
 *
 * avl_node_t *the_root = NULL;
 * struct mystuff;
 * if (!some_name_search(the_root, &mystuff)) some_name_insert(&the_root, &mystuff);
 * some_name_free(&the_root);
 *
 * For questions, feedback, etc: t.vandijk@utwente.nl
 */

#include <stdlib.h>
#include <string.h>

#ifndef __AVL_H__
#define __AVL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct avl_node
{
    struct avl_node *left, *right;
    unsigned int height;
    char pad[8-sizeof(unsigned int)];
    char data[0];
} avl_node_t;

/* Retrieve the height of a tree */
static inline int
avl_get_height(avl_node_t *node)
{
    return node == NULL ? 0 : node->height;
}

/* Helper for rotations to update the heights of trees */
static inline void
avl_update_height(avl_node_t *node)
{
    int h1 = avl_get_height(node->left);
    int h2 = avl_get_height(node->right);
    node->height = 1 + (h1 > h2 ? h1 : h2);
}

/* Helper for avl_balance_tree */
static inline int
avl_update_height_get_balance(avl_node_t *node)
{
    int h1 = avl_get_height(node->left);
    int h2 = avl_get_height(node->right);
    node->height = 1 + (h1 > h2 ? h1 : h2);
    return h1 - h2;
}

/* Helper for avl_check_consistent */
static inline int
avl_verify_height(avl_node_t *node)
{
    int h1 = avl_get_height(node->left);
    int h2 = avl_get_height(node->right);
    int expected_height = 1 + (h1 > h2 ? h1 : h2);
    return expected_height == avl_get_height(node);
}

/* Optional consistency check */
static inline int __attribute__((unused))
avl_check_consistent(avl_node_t *root)
{
    if (root == NULL) return 1;
    if (!avl_check_consistent(root->left)) return 0;
    if (!avl_check_consistent(root->right)) return 0;
    if (!avl_verify_height(root)) return 0;
    return 1;
}

/* Perform LL rotation, returns the new root */
static avl_node_t*
avl_rotate_LL(avl_node_t *parent)
{
    avl_node_t *child = parent->left;
    parent->left = child->right;
    child->right = parent;
    avl_update_height(parent);
    avl_update_height(child);
    return child;
}

/* Perform RR rotation, returns the new root */
static avl_node_t*
avl_rotate_RR(avl_node_t *parent)
{
    avl_node_t *child = parent->right;
    parent->right = child->left;
    child->left = parent;
    avl_update_height(parent);
    avl_update_height(child);
    return child;
}

/* Perform RL rotation, returns the new root */
static avl_node_t*
avl_rotate_RL(avl_node_t *parent)
{
    avl_node_t *child = parent->right;
    parent->right = avl_rotate_LL(child);
    return avl_rotate_RR(parent);
}

/* Perform LR rotation, returns the new root */
static avl_node_t*
avl_rotate_LR(avl_node_t *parent)
{
    avl_node_t *child = parent->left;
    parent->left = avl_rotate_RR(child);
    return avl_rotate_LL(parent);
}

/* Calculate balance factor */
static inline int
avl_get_balance(avl_node_t *node)
{
    if (node == NULL) return 0;
    return avl_get_height(node->left) - avl_get_height(node->right);
}

/* Balance the tree */
static void
avl_balance_tree(avl_node_t **node)
{
    int factor = avl_update_height_get_balance(*node);

    if (factor > 1) {
        if (avl_get_balance((*node)->left) > 0) *node = avl_rotate_LL(*node);
        else *node = avl_rotate_LR(*node);
    } else if (factor < -1) {
        if (avl_get_balance((*node)->right) < 0) *node = avl_rotate_RR(*node);
        else *node = avl_rotate_RL(*node);
    }
}

/* Get number of items in the AVL */
static size_t
avl_count(avl_node_t *node)
{
    if (node == NULL) return 0;
    return 1 + avl_count(node->left) + avl_count(node->right);
}

/* Structure for iterator */
typedef struct avl_iter
{
    size_t height;
    avl_node_t *nodes[0];
} avl_iter_t;

/**
 * nodes[0] = root node
 * nodes[1] = some node
 * nodes[2] = some node
 * nodes[3] = leaf node (height = 4)
 * nodes[4] = NULL (max = height + 1)
 */

/* Create a new iterator */
static inline avl_iter_t*
avl_iter(avl_node_t *node)
{
    size_t max = node ? node->height+1 : 1;
    avl_iter_t *result = (avl_iter_t*)malloc(sizeof(avl_iter_t) + sizeof(avl_node_t*) * max);
    result->height = 0;
    result->nodes[0] = node;
    return result;
}

/* Get the next node during iteration */
static inline avl_node_t*
avl_iter_next(avl_iter_t *iter)
{
    /* when first node is NULL, we're done */
    if (iter->nodes[0] == NULL) return NULL;

    /* if the head is not NULL, first entry... */
    while (iter->nodes[iter->height] != NULL) {
        iter->nodes[iter->height+1] = iter->nodes[iter->height]->left;
        iter->height++;
    }

    /* head is now NULL, take parent as result */
    avl_node_t *result = iter->nodes[iter->height-1];

    if (result->right != NULL) {
        /* if we can go right, do that */
        iter->nodes[iter->height] = result->right;
    } else {
        /* cannot go right, backtrack */
        do {
            iter->height--;
        } while (iter->height > 0 && iter->nodes[iter->height] == iter->nodes[iter->height-1]->right);
        iter->nodes[iter->height] = NULL; /* set head to NULL: second entry */
    }

    return result;
}

#define AVL(NAME, TYPE)                                                                     \
static inline int                                                                           \
NAME##_AVL_compare(TYPE *left, TYPE *right);                                                \
static __attribute__((unused)) TYPE*                                                        \
NAME##_put(avl_node_t **root, TYPE *data, int *inserted)                                    \
{                                                                                           \
    if (inserted && *inserted) *inserted = 0; /* reset inserted once */                     \
    TYPE *result;                                                                           \
    avl_node_t *it = *root;                                                                 \
    if (it == NULL) {                                                                       \
        *root = it = (avl_node_t*)malloc(sizeof(struct avl_node)+sizeof(TYPE));             \
        it->left = it->right = NULL;                                                        \
        it->height = 1;                                                                     \
        memcpy(it->data, data, sizeof(TYPE));                                               \
        result = (TYPE *)it->data;                                                          \
        if (inserted) *inserted = 1;                                                        \
    } else {                                                                                \
        int cmp = NAME##_AVL_compare(data, (TYPE*)(it->data));                              \
        if (cmp == 0) return (TYPE *)it->data;                                              \
        if (cmp < 0) result = NAME##_put(&it->left, data, inserted);                        \
        else result = NAME##_put(&it->right, data, inserted);                               \
        avl_balance_tree(root);                                                             \
    }                                                                                       \
    return result;                                                                          \
}                                                                                           \
static __attribute__((unused)) int                                                          \
NAME##_insert(avl_node_t **root, TYPE *data)                                                \
{                                                                                           \
    int inserted = 0;                                                                       \
    NAME##_put(root, data, &inserted);                                                      \
    return inserted;                                                                        \
}                                                                                           \
static void                                                                                 \
NAME##_exchange_and_balance(avl_node_t *target, avl_node_t **node)                          \
{                                                                                           \
    avl_node_t *it = *node;                                                                 \
    if (it->left == 0) { /* leftmost node contains lowest value */                          \
        memcpy(target->data, it->data, sizeof(TYPE));                                       \
        *node = it->right;                                                                  \
        free(it);                                                                           \
    } else {                                                                                \
        NAME##_exchange_and_balance(target, &it->left);                                     \
    }                                                                                       \
    avl_balance_tree(node);                                                                 \
}                                                                                           \
static __attribute__((unused)) int                                                          \
NAME##_delete(avl_node_t **node, TYPE *data)                                                \
{                                                                                           \
    avl_node_t *it = *node;                                                                 \
    if (it == NULL) return 0;                                                               \
    int cmp = NAME##_AVL_compare(data, (TYPE *)((*node)->data)), res;                       \
    if (cmp < 0) res = NAME##_delete(&it->left, data);                                      \
    else if (cmp > 0) res = NAME##_delete(&it->right, data);                                \
    else {                                                                                  \
        int h_left = avl_get_height(it->left);                                              \
        int h_right = avl_get_height(it->right);                                            \
        if (h_left == 0) {                                                                  \
            if (h_right == 0) { /* Leaf */                                                  \
                *node = NULL;                                                               \
                free(it);                                                                   \
                return 1;                                                                   \
            } else { /* Only right child */                                                 \
                *node = it->right;                                                          \
                free(it);                                                                   \
                return 1;                                                                   \
            }                                                                               \
        } else if (h_right == 0) { /* Only left child */                                    \
            *node = it->left;                                                               \
            free(it);                                                                       \
            return 1;                                                                       \
        } else { /* Exchange with successor */                                              \
            NAME##_exchange_and_balance(it, &it->right);                                    \
            res = 1;                                                                        \
        }                                                                                   \
    }                                                                                       \
    if (res) avl_balance_tree(node);                                                        \
    return res;                                                                             \
}                                                                                           \
static __attribute__((unused)) TYPE*                                                        \
NAME##_search(avl_node_t *node, TYPE *data)                                                 \
{                                                                                           \
    while (node != NULL) {                                                                  \
        int result = NAME##_AVL_compare((TYPE *)node->data, data);                          \
        if (result == 0) return (TYPE *)node->data;                                         \
        if (result > 0) node = node->left;                                                  \
        else node = node->right;                                                            \
    }                                                                                       \
    return NULL;                                                                            \
}                                                                                           \
static __attribute__((unused)) void                                                         \
NAME##_free(avl_node_t **node)                                                              \
{                                                                                           \
    avl_node_t *it = *node;                                                                 \
    if (it) {                                                                               \
        NAME##_free(&it->left);                                                             \
        NAME##_free(&it->right);                                                            \
        free(it);                                                                           \
        *node = NULL;                                                                       \
    }                                                                                       \
}                                                                                           \
static void                                                                                 \
NAME##_toarray_rec(avl_node_t *node, TYPE **ptr)                                            \
{                                                                                           \
    if (node->left != NULL) NAME##_toarray_rec(node->left, ptr);                            \
    memcpy(*ptr, node->data, sizeof(TYPE));                                                 \
    (*ptr)++;                                                                               \
    if (node->right != NULL) NAME##_toarray_rec(node->right, ptr);                          \
}                                                                                           \
static __attribute__((unused)) TYPE*                                                        \
NAME##_toarray(avl_node_t *node)                                                            \
{                                                                                           \
    size_t count = avl_count(node);                                                         \
    TYPE *arr = (TYPE *)malloc(sizeof(TYPE) * count);                                       \
    TYPE *ptr = arr;                                                                        \
    NAME##_toarray_rec(node, &ptr);                                                         \
    return arr;                                                                             \
}                                                                                           \
static __attribute__((unused)) avl_iter_t*                                                  \
NAME##_iter(avl_node_t *node)                                                               \
{                                                                                           \
    return avl_iter(node);                                                                  \
}                                                                                           \
static __attribute__((unused)) TYPE*                                                        \
NAME##_iter_next(avl_iter_t *iter)                                                          \
{                                                                                           \
    avl_node_t *result = avl_iter_next(iter);                                               \
    if (result == NULL) return NULL;                                                        \
    return (TYPE*)(result->data);                                                           \
}                                                                                           \
static __attribute__((unused)) void                                                         \
NAME##_iter_free(avl_iter_t *iter)                                                          \
{                                                                                           \
    free(iter);                                                                             \
}                                                                                           \
static inline int                                                                           \
NAME##_AVL_compare(TYPE *left, TYPE *right)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
