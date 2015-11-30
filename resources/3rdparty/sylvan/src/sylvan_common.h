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

#ifndef SYLVAN_COMMON_H
#define SYLVAN_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Garbage collection test task - t */
#define sylvan_gc_test() YIELD_NEWFRAME()

// BDD operations
#define CACHE_BDD_ITE             (0LL<<40)
#define CACHE_BDD_AND             (1LL<<40)
#define CACHE_BDD_XOR             (2LL<<40)
#define CACHE_BDD_EXISTS          (3LL<<40)
#define CACHE_BDD_AND_EXISTS      (4LL<<40)
#define CACHE_BDD_RELNEXT         (5LL<<40)
#define CACHE_BDD_RELPREV         (6LL<<40)
#define CACHE_BDD_SATCOUNT        (7LL<<40)
#define CACHE_BDD_COMPOSE         (8LL<<40)
#define CACHE_BDD_RESTRICT        (9LL<<40)
#define CACHE_BDD_CONSTRAIN       (10LL<<40)
#define CACHE_BDD_CLOSURE         (11LL<<40)
#define CACHE_BDD_ISBDD           (12LL<<40)
#define CACHE_BDD_SUPPORT         (13LL<<40)
#define CACHE_BDD_PATHCOUNT       (14LL<<40)

// MDD operations
#define CACHE_MDD_RELPROD         (20LL<<40)
#define CACHE_MDD_MINUS           (21LL<<40)
#define CACHE_MDD_UNION           (22LL<<40)
#define CACHE_MDD_INTERSECT       (23LL<<40)
#define CACHE_MDD_PROJECT         (24LL<<40)
#define CACHE_MDD_JOIN            (25LL<<40)
#define CACHE_MDD_MATCH           (26LL<<40)
#define CACHE_MDD_RELPREV         (27LL<<40)
#define CACHE_MDD_SATCOUNT        (28LL<<40)
#define CACHE_MDD_SATCOUNTL1      (29LL<<40)
#define CACHE_MDD_SATCOUNTL2      (30LL<<40)

// MTBDD operations
#define CACHE_MTBDD_APPLY         (40LL<<40)
#define CACHE_MTBDD_UAPPLY        (41LL<<40)
#define CACHE_MTBDD_ABSTRACT      (42LL<<40)
#define CACHE_MTBDD_ITE           (43LL<<40)
#define CACHE_MTBDD_AND_EXISTS    (44LL<<40)
#define CACHE_MTBDD_SUPPORT       (45LL<<40)
#define CACHE_MTBDD_COMPOSE       (46LL<<40)
#define CACHE_MTBDD_EQUAL_NORM    (47LL<<40)
#define CACHE_MTBDD_EQUAL_NORM_REL (48LL<<40)
#define CACHE_MTBDD_MINIMUM       (49LL<<40)
#define CACHE_MTBDD_MAXIMUM       (50LL<<40)
#define CACHE_MTBDD_LEQ           (51LL<<40)
#define CACHE_MTBDD_LESS          (52LL<<40)
#define CACHE_MTBDD_GEQ           (53LL<<40)
#define CACHE_MTBDD_GREATER       (54LL<<40)

/**
 * Registration of quit functions
 */
typedef void (*quit_cb)();
void sylvan_register_quit(quit_cb cb);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
