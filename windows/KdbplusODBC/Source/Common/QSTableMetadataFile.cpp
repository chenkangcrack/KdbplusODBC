//==================================================================================================
///  @file QSTableMetadataFile.cpp
///
///  Implementation of the class QSTableMetadataFile for Windows.
///
///  Copyright (C) 2008-2015 Simba Technologies Incorporated.
//==================================================================================================

#include "QSTableMetadataFile.h"

#include "DSIColumnMetadata.h"
#include "DSIResultSetColumn.h"
#include "NumberConverter.h"
#include "QSUtilities.h"
#include "SqlTypeMetadata.h"
#include "SqlTypeMetadataFactory.h"
#include "TextFile.h"

#include "xmlparse.h"

#ifndef _WIN32
#include <stdio.h>
#endif

using namespace Simba::Quickstart;
using namespace Simba::DSI;

// Static ==========================================================================================
static const int FILE_READ_BUFFER_SIZE = 8192;

// Helpers =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Converts between ODBC 2.X/3.X date/time types as needed.
///
/// @param in_type      The type to be (potentially) converted.
/// @param in_isODBCV3  Whether ODBC 3.X is in use.
///
/// @return The resulting type.
////////////////////////////////////////////////////////////////////////////////////////////////////
inline simba_int16 ConvertTypeIfNeeded(simba_int16 in_type, bool in_isODBCV3)
{
    if (in_isODBCV3)
    {
        // Convert the ODBC 2x datetime types to 3x types.
        switch (in_type)
        {
            case SQL_DATE:
            {
                return SQL_TYPE_DATE;
            }

            case SQL_TIME:
            {
                return SQL_TYPE_TIME;
            }

            case SQL_TIMESTAMP:
            {
                return SQL_TYPE_TIMESTAMP;
            }

            default:
            {
                return in_type;
            }
        }
    }
    else
    {
        // Convert the ODBC 3x datetime types to 2x types.
        switch (in_type)
        {
            case SQL_TYPE_DATE:
            {
                return SQL_DATE;
            }

            case SQL_TYPE_TIME:
            {
                return SQL_TIME;
            }

            case SQL_TYPE_TIMESTAMP:
            {
                return SQL_TIMESTAMP;
            }

            default:
            {
                return in_type;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Find an attribute in a starting element tag.
///
/// @param in_attribute         The attribute to look for.
/// @param in_elements          The elements of the starting element tag.
///
/// @return The index of the attribute name if it's found; -1 otherwise.
////////////////////////////////////////////////////////////////////////////////////////////////////
inline simba_int16 FindColumnAttribute(
    const simba_string& in_attribute,
    const simba_char** in_elements)
{
    // Search the NULL-terminated array for the element
    for (simba_int16 index = 0; NULL != in_elements[index]; index += 2)
    {
        if (in_attribute == in_elements[index])
        {
            return index;
        }
    }

    return -1;
}

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSTableMetadataFile::QSTableMetadataFile(QuickstartSettings* in_settings, bool in_isODBCV3) :
    m_settings(in_settings),
    m_resultSetColumns(NULL),
    m_isODBCV3(in_isODBCV3)
{
    assert(in_settings);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSTableMetadataFile::~QSTableMetadataFile()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AutoPtr<DSIResultSetColumns> QSTableMetadataFile::Read(const simba_wstring& in_tableName)
{
    QSUtilities utilities(m_settings);

    m_tableName = in_tableName;

    m_resultSetColumns = new DSIResultSetColumns();
    ParseMetadataFile(utilities.GetTableMetadataFullPathName(in_tableName));

    // Take ownership of the m_resultSetColumns and return them.
    return m_resultSetColumns;
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTableMetadataFile::AddColumnMetadata(
    const simba_wstring& in_columnName,
    const simba_wstring& in_sqlType,
    const bool in_isUnsigned)
{
    AutoPtr<DSIColumnMetadata> columnMetadata(new DSIColumnMetadata());

    columnMetadata->m_name = in_columnName;
    columnMetadata->m_schemaName.Clear();
    columnMetadata->m_tableName = m_tableName;
    columnMetadata->m_label = columnMetadata->m_name;
    columnMetadata->m_unnamed = false;
    columnMetadata->m_charOrBinarySize = m_settings->defaultMaxColumnSize;
    columnMetadata->m_nullable = DSI_NULLABLE;

    simba_int16 type = ConvertTypeIfNeeded(
        NumberConverter::ConvertWStringToInt16(in_sqlType),
        m_isODBCV3);

    AutoPtr<SqlTypeMetadata> sqlTypeMetadata(
        SqlTypeMetadataFactorySingleton::GetInstance()->CreateNewSqlTypeMetadata(type, in_isUnsigned));

    if (sqlTypeMetadata->IsCharacterOrBinaryType())
    {
        // Variable length columns need to have their lengths match the binary size.
        sqlTypeMetadata->SetLengthOrIntervalPrecision(columnMetadata->m_charOrBinarySize);
    }
    else if (sqlTypeMetadata->IsExactNumericType())
    {
        // Exact numeric columns have a preset precision of 38 (default) and scale of 10.
        sqlTypeMetadata->SetScale(10);
    }

    m_resultSetColumns->AddColumn(new DSIResultSetColumn(sqlTypeMetadata, columnMetadata));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTableMetadataFile::ParseMetadataFile(const simba_wstring& in_file)
{
    // Create and store the XML parser.
    XML_Parser parser = XML_ParserCreate(NULL);

    try
    {
        // We pass "this" in as the UserData argument.
        // The input data in the handler functions will be whatever the UserData argument is.
        XML_SetUserData(parser, this);
        XML_SetElementHandler(parser, QSTableMetadataFile::StartElement, NULL);

        // Open the metadata file for reading and set up the read buffer.
        TextFile file(in_file, OPENMODE_READONLY);
        simba_char buffer[FILE_READ_BUFFER_SIZE];
        bool isFileDone = false;

        do
        {
            const simba_int64 bytesRead = file.Read(buffer, FILE_READ_BUFFER_SIZE);
            assert(bytesRead <= FILE_READ_BUFFER_SIZE);
            isFileDone = bytesRead < FILE_READ_BUFFER_SIZE;

            // Try to parse. Note that the cast here is safe as FILE_READ_BUFFER_SIZE is less than
            // SIMBA_INT32_MAX.
            if (!XML_Parse(parser, buffer, static_cast<simba_int32>(bytesRead), isFileDone))
            {
                simba_wstring error(XML_ErrorString(XML_GetErrorCode(parser)));
                simba_wstring line(
                    NumberConverter::ConvertInt32ToWString(XML_GetCurrentLineNumber(parser)));

                QSTHROWGEN3(L"QSTableMetadataParseFormatError", m_tableName, error, line);
            }
        } while (!isFileDone);

        // Free the parser.
        XML_ParserFree(parser);
    }
    catch (...)
    {
        // Free the parser.
        XML_ParserFree(parser);
        throw;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTableMetadataFile::StartElement(
    void* in_userData,
    const simba_char*  in_name,
    const simba_char** in_attributes)
{
    simba_size_t nameLen = strlen(in_name);
    if (0 == strncmp(in_name, "Column", nameLen))
    {
        simba_int16 attrIndex = FindColumnAttribute("name", in_attributes);
        if (attrIndex == -1)
        {
            QSTHROWGEN(L"QSTableMetadataInvalidFormat");
        }

        // Handle column names that may not be ASCII.
        simba_wstring columnName(
            reinterpret_cast<const simba_byte*>(in_attributes[attrIndex + 1]),
            strlen(in_attributes[attrIndex + 1]),
            ENC_UTF8);

        attrIndex = FindColumnAttribute("type", in_attributes);
        if (attrIndex == -1)
        {
            QSTHROWGEN(L"QSTableMetadataInvalidFormat");
        }
        simba_wstring typeString(in_attributes[attrIndex + 1]);

        // optional attributes
        bool isUnsigned = false;

        // signed or unsigned column?
        attrIndex = FindColumnAttribute("is_signed", in_attributes);
        if (attrIndex != -1)
        {
            simba_string isSignedStr(in_attributes[attrIndex + 1]);

            // Make it lowercase (for case-insensitive comparison).
            StrToLower(isSignedStr);
            if ("true" == isSignedStr)
            {
                isUnsigned = false;
            }
            else if ("false" == isSignedStr)
            {
                isUnsigned = true;
            }
            else // neither true nor false?
            {
                QSTHROWGEN(L"QSTableMetadataInvalidFormat");
            }
        }

        QSTableMetadataFile* it = reinterpret_cast<QSTableMetadataFile*>(in_userData);
        it->AddColumnMetadata(columnName, typeString, isUnsigned);
    }
}
