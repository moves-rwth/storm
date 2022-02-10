/*
 * MappedFile.cpp
 *
 *  Created on: Jan 21, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "storm-parsers/parser/MappedFile.h"

#include <fcntl.h>
#include <cstring>
#include <fstream>

#include <boost/integer/integer_mask.hpp>

#include "storm/exceptions/FileIoException.h"
#include "storm/io/file.h"
#include "storm/utility/macros.h"

namespace storm {
namespace parser {

MappedFile::MappedFile(const char* filename) {
    STORM_LOG_THROW(storm::utility::fileExistsAndIsReadable(filename), storm::exceptions::FileIoException,
                    "Error while reading " << filename << ": The file does not exist or is not readable.");

#if defined LINUX || defined MACOSX

    // Do file mapping for reasonable systems.
    // stat64(), open(), mmap()

#ifdef MACOSX
    if (stat(filename, &(this->st)) != 0) {
#else
    if (stat64(filename, &(this->st)) != 0) {
#endif
        STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Error in stat(" << filename << "): Probably, this file does not exist.");
    }
    this->file = open(filename, O_RDONLY);

    STORM_LOG_THROW(this->file >= 0, storm::exceptions::FileIoException, "Error in open(" << filename << "): Probably, we may not read this file.");

    this->data = static_cast<char*>(mmap(NULL, this->st.st_size, PROT_READ, MAP_PRIVATE, this->file, 0));
    if (this->data == MAP_FAILED) {
        close(this->file);
        STORM_LOG_ERROR("Error in mmap(" << filename << "): " << std::strerror(errno));
        throw exceptions::FileIoException() << "MappedFile Error in mmap(): " << std::strerror(errno);
    }
    this->dataEnd = this->data + this->st.st_size;
#elif defined WINDOWS

    // Do file mapping for windows.
    // _stat64(), CreateFile(), CreateFileMapping(), MapViewOfFile()

    if (_stat64(filename, &(this->st)) != 0) {
        STORM_LOG_ERROR("Error in _stat(" << filename << "): Probably, this file does not exist.");
        throw exceptions::FileIoException("MappedFile Error in stat(): Probably, this file does not exist.");
    }

    this->file = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (this->file == INVALID_HANDLE_VALUE) {
        STORM_LOG_ERROR("Error in CreateFileA(" << filename << "): Probably, we may not read this file.");
        throw exceptions::FileIoException("MappedFile Error in CreateFileA(): Probably, we may not read this file.");
    }

    this->mapping = CreateFileMappingA(this->file, NULL, PAGE_READONLY, (DWORD)(st.st_size >> 32), (DWORD)st.st_size, NULL);
    if (this->mapping == NULL) {
        CloseHandle(this->file);
        STORM_LOG_ERROR("Error in CreateFileMappingA(" << filename << ").");
        throw exceptions::FileIoException("MappedFile Error in CreateFileMappingA().");
    }

    this->data = static_cast<char*>(MapViewOfFile(this->mapping, FILE_MAP_READ, 0, 0, this->st.st_size));
    if (this->data == NULL) {
        CloseHandle(this->mapping);
        CloseHandle(this->file);
        STORM_LOG_ERROR("Error in MapViewOfFile(" << filename << ").");
        throw exceptions::FileIoException("MappedFile Error in MapViewOfFile().");
    }
    this->dataEnd = this->data + this->st.st_size;
#endif
}

MappedFile::~MappedFile() {
#if defined LINUX || defined MACOSX
    munmap(this->data, this->st.st_size);
    close(this->file);
#elif defined WINDOWS
    CloseHandle(this->mapping);
    CloseHandle(this->file);
#endif
}

char const* MappedFile::getData() const {
    return data;
}

char const* MappedFile::getDataEnd() const {
    return dataEnd;
}

std::size_t MappedFile::getDataSize() const {
    return this->getDataEnd() - this->getData();
}

}  // namespace parser
}  // namespace storm
