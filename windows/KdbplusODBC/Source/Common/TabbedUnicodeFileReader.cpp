//==================================================================================================
///  @file TabbedUnicodeFileReader.cpp
///
///  Implementation of the class TabbedUnicodeFileReader.
///
///  Copyright (C) 2008-2014 Simba Technologies Incorporated.
//==================================================================================================

#include "TabbedUnicodeFileReader.h"

#include "NumberConverter.h"

using namespace Simba::Quickstart;

#if (PLATFORM_IS_LITTLE_ENDIAN == 1)
    #define HEADER_SIZE 2
    #define HEADER 0xFEFF
    #define UNICODE_TAB_DELIM 0x0009
    #define UNICODE_CARRIAGE_RETURN 0x000D
    #define UNICODE_NEWLINE 0x000A
    #define UNICODE_QUOTE 0x022
    #define BYTES_PER_CHAR 0x02
#else
    #define HEADER_SIZE 2
    #define HEADER 0xFFFE
    #define UNICODE_TAB_DELIM 0x0900
    #define UNICODE_CARRIAGE_RETURN 0x0D00
    #define UNICODE_NEWLINE 0x0A00
    #define UNICODE_QUOTE 0x220
    #define BYTES_PER_CHAR 0x02
#endif

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
TabbedUnicodeFileReader::TabbedUnicodeFileReader(const simba_wstring& in_filename) : 
    m_file(in_filename, OPENMODE_READONLY)
{
    // Check that the file opened safely.
    if (!m_file.IsOpen())
    {
        QSTHROWGEN1(L"QSFileOpenError", in_filename);
    }

    // Check that the file is indeed a unicode text file encoded using UTF-16LE.
    simba_int8 header[HEADER_SIZE];
    
    // Read the header.
    if (HEADER_SIZE != m_file.Read(header, HEADER_SIZE))
    {
        QSTHROWGEN1(L"QSFileReadError", in_filename);
    }

    // Validate the header.
    if (HEADER != *(reinterpret_cast<simba_uint16*>(header)))
    {
        QSTHROWGEN1(L"QSInvalidFileFormat", in_filename);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
TabbedUnicodeFileReader::~TabbedUnicodeFileReader()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TabbedUnicodeFileReader::GetData(
    simba_size_t in_colIndex,
    simba_int8* io_buffer, 
    simba_int64 in_bufferSize, 
    simba_int64 in_offset,
    simba_int64& out_bytesRead)
{
    // Assume we are going to read the full token.
    bool moreToRead = false;

    // Check that the row has been processed.
    if (m_tokenOffsets.empty())
    {
        QSTHROWGEN(L"QSRowNotProcessed");
    }

    // Check that the provided index is valid.
    if ((0 > in_colIndex) && (m_tokenOffsets.size() < in_colIndex))
    {
        QSTHROWGEN1(L"QSInvalidColumnIndex", NumberConverter::ConvertInt32ToWString(in_colIndex));
    }

    simba_int64 tokenLength = 0;
    bool skipEOL = false;

    // The length of the token is calculated from the start of the next token to the beginning
    // of the token being requested.
    if ((in_colIndex + 1) < m_tokenOffsets.size())
    {
        // Offset the length so as to not read the tab delimiter
        tokenLength = 
            m_tokenOffsets[in_colIndex + 1].first - 
            m_tokenOffsets[in_colIndex].first - 
            BYTES_PER_CHAR;
    }
    else
    {
        // Read from the start of the requested token until the end of the line.
        // The m_endOfLine marker includes 0x0A 0x0D sequence.
        tokenLength = m_endOfLine - (2 * BYTES_PER_CHAR) - m_tokenOffsets[in_colIndex].first;
        skipEOL = true;
    }

    simba_int64 startOffset = m_tokenOffsets[in_colIndex].first + in_offset;

    // Check if the value is enclosed by quotes. If so, remove them.
    if (m_tokenOffsets[in_colIndex].second)
    {
        // Remove the enclosing quotes from the length.
        tokenLength -= 2 * BYTES_PER_CHAR;

        // Adjust the offset to take into account the opening quote.
        startOffset += BYTES_PER_CHAR;
    }

    // Adjust the offset.
    tokenLength -= in_offset;

    // Truncate the tokenLength if necessary.
    if (tokenLength > in_bufferSize)
    {
        // We are not reading the full token.
        moreToRead = true;
        tokenLength = in_bufferSize;
    }

    // Seek to the beginning of the token.
    m_file.Seek(startOffset, BinaryFile::DIRECTION_START);

    // Read the data.
    out_bytesRead = m_file.Read(io_buffer, tokenLength);

    if (skipEOL && !moreToRead)
    {
        m_file.Seek(4, BinaryFile::DIRECTION_FORWARD);
    }

    return moreToRead;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_size_t TabbedUnicodeFileReader::GetNumColumns()
{
    // If there is no row.
    if (!ProcessRow())
    {
        return 0;
    }
    
    // Otherwise return the number of tokens in the row.
    simba_size_t numColumns = m_tokenOffsets.size();
    MoveToFirst();

    return numColumns;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TabbedUnicodeFileReader::MoveToFirst()
{
    if (-1 == m_file.Seek(HEADER_SIZE, BinaryFile::DIRECTION_START))
    {
        return false;
    }

    return ProcessRow();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool TabbedUnicodeFileReader::MoveToNext()
{
    // Move to the beginning of the next line.
    if (-1 == m_file.Seek(m_endOfLine, BinaryFile::DIRECTION_START))
    {
        return false;
    }

    // Process each of the delimited values in the current line.
    return ProcessRow();
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
bool TabbedUnicodeFileReader::ProcessRow()
{
    m_tokenOffsets.clear();

    // Read one unicode character at a time.
    simba_uint16 data;

    // Haven't detected an the end of line.
    m_endOfLine = -1;

    simba_int64 tokenStart = m_file.GetPosition();
    bool isQuoted = false;
    bool isValueStart = true;
    
    // While there is still more data.
    while (0 != ReadChar(data))
    {
        if (isValueStart)
        {
            isValueStart = false;
            isQuoted = (UNICODE_QUOTE == data);
        }

        switch(data)
        {
            // We hit a tab delimiter. Store the offset to the token.
            case UNICODE_TAB_DELIM:
            {
                m_tokenOffsets.push_back(TokenOffset(tokenStart, isQuoted));
                isValueStart = true;

                tokenStart = m_file.GetPosition();
                break;
            }

            // End of line detected.
            case UNICODE_CARRIAGE_RETURN:
            {
                // Read the next character.
                ReadChar(data);
                
                // Did we hit a new line?
                if (UNICODE_NEWLINE == data)
                {
                    m_tokenOffsets.push_back(TokenOffset(tokenStart, isQuoted));
                    isValueStart = true;
                    
                    // Mark the position of the end of line.
                    m_endOfLine = m_file.GetPosition();
                    return true;
                }

                break;
            }

            default:
            {
                break; // Do nothing.
            }
        }
    }  

    // Use the end of the file to mark the end of the line.
    m_endOfLine = m_file.GetPosition();
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_int64 TabbedUnicodeFileReader::ReadChar(simba_uint16& io_data)
{
    // Read data 1 char at a time.  We must be aligned with the number of bytes in a char.
    simba_int64 numBytes = m_file.Read(reinterpret_cast<simba_int8*>(&io_data), BYTES_PER_CHAR);

    // If we cannot read 2 bytes then we throw an exception.
    if ((BYTES_PER_CHAR > numBytes) && 
        (0 != numBytes))
    {
        QSTHROWGEN1(L"QSInvalidFileFormat", m_file.GetName());
    }

    return numBytes;
}
