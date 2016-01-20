//==================================================================================================
///  @file QSTableMetadataFile_Windows.cpp
///
///  Implementation of the class QSTableMetadataFile for Windows.
///
///  Copyright (C) 2008-2011 Simba Technologies Incorporated.
//==================================================================================================

#include "QSTableMetadataFile.h"

#include "DSIColumnMetadata.h"
#include "DSIResultSetColumn.h"
#include "DSIResultSetColumns.h"
#include "NumberConverter.h"
#include "QSUtilities.h"
#include "SqlData.h"
#include "SqlDataTypeUtilities.h"
#include "SqlTypeMetadata.h"
#include "SqlTypeMetadataFactory.h"
#include "TabbedUnicodeFileReader.h"

#include "xmlparse.h"
#include <sstream>
#include <algorithm>
#include <ctype.h>

using namespace Simba::Quickstart;
using namespace Simba::DSI;
using namespace Simba::Support;

// Static ==========================================================================================
static const int FILE_READ_BUFFER_SIZE = 8192;

// Helpers =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief converts between ODBC 2.X/3.X date/time types as needed 
/// 
/// @param in_type      The type to be (potentially) converted
/// @param in_isODBCV3  Whether ODBC 3.X is in use
/// 
/// @return the resulting type
inline simba_int16 convertTypeIfNeeded(simba_int16 in_type, bool in_isODBCV3)
{
    if (in_isODBCV3)
    {
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

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSTableMetadataFile::QSTableMetadataFile(QuickstartSettings* in_quickstartSettings, bool in_isODBCV3) :
    m_quickstartSettings(in_quickstartSettings),
    m_resultSetColumns(NULL),
    m_isODBCV3(in_isODBCV3)
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSTableMetadataFile::~QSTableMetadataFile()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTableMetadataFile::Write(
    const simba_wstring &in_tableName,
    AutoPtr<DSIResultSetColumns> in_resultColumns)
{
    QSUtilities utilities(m_quickstartSettings);

    simba_wstring metadataFile(utilities.GetTableMetadataFullPathName(in_tableName));

    simba_uint16 columnsCount = in_resultColumns->GetColumnCount();

    FILE* file = NULL;
    errno_t err = _wfopen_s(&file, metadataFile.GetAsPlatformWString().c_str(), L"w,ccs=UNICODE");

    if (0 != err)
    {
        QSTHROWGEN1(L"QSTableMetadataFileOpenError", m_tableName);
    }

    // Write the header of the xml file.
    WriteToFile(L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>", file);

    // Open Columns tag.
    WriteToFile(L"<Columns>", file);
    
    for (simba_uint16 columnIdx = 0; columnIdx < columnsCount; columnIdx++)
    {
        simba_wstring name;
        
        IColumn* column = in_resultColumns->GetColumn(columnIdx);
        column->GetName(name);

        simba_wstring type(
            NumberConverter::ConvertInt32ToWString(column->GetMetadata()->GetSqlType()));
        
        // Write the column in this format: <Column Name="column name" SqlType="-9" />
        simba_wstring columnData = L"<Column";
        columnData += L" name=\"";
        columnData += name + L"\"";

        columnData += L" type=\"";
        columnData += type + L"\"";

        columnData += L" />";
        
        WriteToFile(columnData, file);
    }

    // Close Columns tag.
    WriteToFile(L"</Columns>", file);

    fclose(file);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AutoPtr<DSIResultSetColumns> QSTableMetadataFile::Read(const simba_wstring &in_tableName)
{
    QSUtilities utilities(m_quickstartSettings);

    m_tableName = in_tableName;

    m_resultSetColumns = new DSIResultSetColumns();
    ParseMetadataFile(utilities.GetTableMetadataFullPathName(in_tableName));

    return AutoPtr<DSIResultSetColumns>(m_resultSetColumns);
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTableMetadataFile::ParseMetadataFile(const simba_wstring& in_file)
{
    FILE* file = NULL;
    errno_t err = _wfopen_s(&file, in_file.GetAsPlatformWString().c_str(), L"r,ccs=UNICODE");

    if (0 != err)
    {
        QSTHROWGEN1(L"QSTableMetadataFileOpenError", m_tableName);
    }

    // Store the writer.
    XML_Parser parser = XML_ParserCreate(NULL);

    // We pass "this" in as the UserData argument.
    // The input data in the handler functions will be whatever the UserData argument is.
    XML_SetUserData(parser, this);
    XML_SetElementHandler(parser, QSTableMetadataFile::StartElement, NULL);

    simba_char buf[FILE_READ_BUFFER_SIZE];
    bool isFileDone = false;

    do 
    {
        simba_size_t fileLength = fread(buf, 1, sizeof(buf), file);
        isFileDone = fileLength < sizeof(buf);

        // Try to parse.
        if (!XML_Parse(parser, buf, static_cast<simba_int32> (fileLength), isFileDone)) 
        {
            simba_wstring error(XML_ErrorString(XML_GetErrorCode(parser)));
            error += " at line ";
            error += NumberConverter::ConvertInt32ToWString(XML_GetCurrentLineNumber(parser));

            // Free the parser and close the file.
            XML_ParserFree(parser);
            fclose(file);

            QSTHROWGEN2(L"QSTableMetadataParseFormatError", m_tableName, error);
        }
    } while (!isFileDone);

    // Free the parser and close the file.
    XML_ParserFree(parser);
    fclose(file);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTableMetadataFile::StartElement(
    void* in_userData, 
    const simba_char*  in_name, 
    const simba_char** in_attributes)
{
    QSTableMetadataFile* it = reinterpret_cast<QSTableMetadataFile*>(in_userData);

    simba_size_t nameLen = strlen(in_name);
    if (0 == strncmp(in_name, "Column", nameLen))
    {
        simba_int16 attrIndex = FindColumnAttribute("name", in_attributes);
        if (attrIndex == -1)
        {
            QSTHROWGEN(L"QSTableMetadataInvalidFormat");
        }
        simba_wstring columnName(in_attributes[attrIndex + 1]);

        attrIndex = FindColumnAttribute("type", in_attributes);
        if (attrIndex == -1)
        {
            QSTHROWGEN(L"QSTableMetadataInvalidFormat");
        }
        simba_wstring typeString(in_attributes[attrIndex + 1]);

        // optional attributes
        bool is_unsigned = false;

        // signed or unsigned column?
        attrIndex = FindColumnAttribute("is_signed", in_attributes);
        if (attrIndex != -1)
        {			
            simba_string is_signed(in_attributes[attrIndex + 1]);
            std::transform(is_signed.begin(), is_signed.end(), is_signed.begin(), tolower); // make it lowercase (for case-insensitive comparison)
            if("true" == is_signed)
            {
                is_unsigned = false;
            }
            else if("false" == is_signed)
            {
                is_unsigned = true;
            }
            else // neither true nor false?
            {
                QSTHROWGEN(L"QSTableMetadataInvalidFormat");
            }
        }

        it->AddColumnMetadata(columnName, typeString, is_unsigned);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTableMetadataFile::AddColumnMetadata(
    const simba_wstring& in_columnName, 
    const simba_wstring& in_sqlType,
    const bool in_isUnsigned)
{
    AutoPtr<DSIColumnMetadata> columnMetadata(new DSIColumnMetadata());

    columnMetadata->m_name = in_columnName;
    columnMetadata->m_schemaName = L"";
    columnMetadata->m_tableName = m_tableName;
    columnMetadata->m_label = columnMetadata->m_name;
    columnMetadata->m_unnamed = false;
    columnMetadata->m_charOrBinarySize = m_quickstartSettings->defaultMaxColumnSize;
    columnMetadata->m_nullable = DSI_NULLABLE;

    simba_int16 type = convertTypeIfNeeded(
        static_cast<simba_int16>(NumberConverter::ConvertWStringToIntNative(in_sqlType)),
        m_isODBCV3);

    SqlTypeMetadata* sqlTypeMetadata = 
        SqlTypeMetadataFactorySingleton::GetInstance()->CreateNewSqlTypeMetadata(type, in_isUnsigned);

    if(SqlDataTypeUtilitiesSingleton::GetInstance()->IsCharacterOrBinaryType(type))
    {
        sqlTypeMetadata->SetLengthOrIntervalPrecision(columnMetadata->m_charOrBinarySize);
    }
    else
    {
        columnMetadata->m_charOrBinarySize = sqlTypeMetadata->GetColumnSize(0);
    }

    if ((type == SQL_DECIMAL) || 
        (type == SQL_NUMERIC))
    {
        sqlTypeMetadata->SetScale(10); 
    }

    m_resultSetColumns->AddColumn(new DSIResultSetColumn(sqlTypeMetadata, columnMetadata.Detach()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTableMetadataFile::WriteToFile(const simba_wstring& in_data, FILE* in_file)
{
    size_t bufferSize = in_data.GetLengthInBytes();
    if (fwrite(in_data.GetAsPlatformWString().c_str(), 1, bufferSize, in_file) != bufferSize)
    {
        QSTHROWGEN1(L"QSTableMetadataFileWriteError", m_tableName);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_int16 QSTableMetadataFile::FindColumnAttribute(
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
