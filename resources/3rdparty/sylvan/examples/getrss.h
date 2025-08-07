#ifndef GETRSS_H
#define GETRSS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */
size_t getPeakRSS();

/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 */
size_t getCurrentRSS();

/**
 * Returns the maximum memory that can be allocated in bytes.
 */
size_t getMaxMemory();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
