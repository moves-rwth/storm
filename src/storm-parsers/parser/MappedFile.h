/*
 * MappedFile.h
 *
 *  Created on: Jan 21, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_PARSER_MAPPEDFILE_H_
#define STORM_PARSER_MAPPEDFILE_H_

#include <sys/stat.h>
#include <cstddef>

#include "storm/utility/OsDetection.h"

namespace storm {
namespace parser {

#if !defined LINUX && !defined MACOSX && !defined WINDOWS
#error Platform not supported
#endif

/*!
 * Opens a file and maps it to memory providing a char*
 * containing the file content.
 *
 * This class is a very simple interface to read files efficiently.
 * The given file is opened and mapped to memory using mmap().
 * The public member data is a pointer to the actual file content.
 * Using this method, the kernel will take care of all buffering.
 * This is most probably much more efficient than doing this manually.
 */
class MappedFile {
   public:
    /*!
     * Constructs a MappedFile.
     * This will stat the given file, open it and map it to memory.
     * If anything of this fails, an appropriate exception is raised and a log entry is written.
     *
     * @param filename Path and name of the file to be opened.
     */
    MappedFile(const char* filename);

    /*!
     * Destructs a MappedFile.
     * This will unmap the data and close the file.
     */
    ~MappedFile();

    /*!
     * Returns a pointer to the beginning of the mapped file data.
     *
     * @return A pointer to the first character of the mapped file data.
     */
    char const* getData() const;

    /*!
     * Returns a pointer to the end of the mapped file data.
     *
     * @return A pointer to the first position after the last character of the mapped file data.
     */
    char const* getDataEnd() const;

    /*!
     * Returns the size of the mapped file data.
     */
    std::size_t getDataSize() const;

   private:
    //! A pointer to the mapped file content.
    char* data;

    //! A pointer to end of the mapped file content.
    char* dataEnd;

#if defined LINUX || defined MACOSX

    //! The file descriptor obtained by open().
    int file;
#elif defined WINDOWS
    //! The file handle obtained by opening the file.
    HANDLE file;

    //! The handle referencing the created memory mapping.
    HANDLE mapping;
#endif

#if defined LINUX

    //! Stat information about the file.
    struct stat64 st;
#elif defined MACOSX

    //! Stat information about the file.
    struct stat st;
#elif defined WINDOWS

    //! Stat information about the file.
    struct __stat64 st;
#endif
};

}  // namespace parser
}  // namespace storm

#endif /* STORM_PARSER_MAPPEDFILE_H_ */
