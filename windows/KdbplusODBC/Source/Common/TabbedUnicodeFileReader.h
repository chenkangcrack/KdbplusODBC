//==================================================================================================
///  @file TabbedUnicodeFileReader.h
///
///  Definition of the class TabbedUnicodeFileReader.
///
///  Copyright (C) 2009-2014 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_TABBEDUNICODEFILEREADER_H_
#define _SIMBA_QUICKSTART_TABBEDUNICODEFILEREADER_H_

#include "Quickstart.h"

#include "BinaryFile.h"

#include <vector>

namespace Simba
{
namespace Quickstart
{
    // The offset in the row for the current token, along with a flag indicating if the token is 
    // enclosed in quotes.
    typedef std::pair<simba_int64, bool> TokenOffset;

    // The collection of TokenOffsets.
    typedef std::vector<TokenOffset> TokenOffsets;

    /// @brief A file reader that processes the tabbed unicode file format encoded using UTF-16LE.
    class TabbedUnicodeFileReader
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        TabbedUnicodeFileReader(const simba_wstring &in_filename);

        /// @brief Destructor.
        ~TabbedUnicodeFileReader();

        /// @brief Retrieves the data for the token specified by the in_colIndex.
        ///
        /// @param in_colIndex      The 0-based index of the column to read.
        /// @param io_buffer        Buffer into which to read the data. (NOT OWN)
        /// @param in_bufferSize    The buffer size.
        /// @param in_offset        The offset from which to start reading the column data.
        /// @param out_bytesRead    The number of bytes read into io_buffer.
        ///
        /// @return True if there is more data to be read; false otherwise.
        bool GetData(
            simba_size_t in_colIndex,
            simba_int8* io_buffer,
            simba_int64 in_bufferSize,
            simba_int64 in_offset,
            simba_int64& out_bytesRead);

        /// @brief Parses the contents of the first line in the file based on the tab
        /// character and returns the number of tokens present.
        ///
        /// @return The number of columns present in the file.
        simba_size_t GetNumColumns();

        /// @brief Repositions the file to the first row.
        ///
        /// @return True if there are more rows; false otherwise.
        bool MoveToFirst();

        /// @brief Repositions the file to the next row.
        ///
        /// @return True if there are more rows; false otherwise.
        bool MoveToNext();

    // Private =====================================================================================
    private:
        /// @brief Parses a row of the file and stores the locations of the tokens within the
        /// row in m_tokenOffsets.
        ///
        /// @return True if there are more rows to be read; false otherwise.
        bool ProcessRow();

        /// @brief Reads a character from the file and returns the number of bytes read.
        ///
        /// @param io_data          Buffer into which to read the data.
        ///
        /// @return The number of bytes read, which should always be BYTES_PER_CHAR or 0.
        simba_int64 ReadChar(simba_uint16& io_data);

        // file to be processed.
        BinaryFile m_file;

        // Offsets within the row to the beginning of the tokens, along with a flag indicating if 
        // the token represented by the offset is enclosed by quotes.
        TokenOffsets m_tokenOffsets;

        // An additional offset to mark the end of line.
        simba_int64 m_endOfLine;
    };
}
}

#endif
