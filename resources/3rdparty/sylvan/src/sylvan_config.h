/* Operation cache: use bitmasks for module (size must be power of 2!) */
#ifndef CACHE_MASK
#define CACHE_MASK 1
#endif

/* Nodes table: use bitmasks for module (size must be power of 2!) */
#ifndef LLMSSET_MASK
#define LLMSSET_MASK 1
#endif

/**
 * Use Fibonacci sequence as resizing strategy.
 * This MAY result in more conservative memory consumption, but is not
 * great for performance.
 * By default, powers of 2 should be used.
 * If you set this, then set CACHE_MASK and LLMSSET_MASK to 0.
 */
#ifndef SYLVAN_SIZE_FIBONACCI
#define SYLVAN_SIZE_FIBONACCI 0
#endif

/* Enable/disable counters and timers */
#ifndef SYLVAN_STATS
#define SYLVAN_STATS 0
#endif

/* Aggressive or conservative resizing strategy */
#ifndef SYLVAN_AGGRESSIVE_RESIZE
#define SYLVAN_AGGRESSIVE_RESIZE 1
#endif
