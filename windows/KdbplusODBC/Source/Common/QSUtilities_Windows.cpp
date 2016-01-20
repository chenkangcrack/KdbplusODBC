//==================================================================================================
///  @file QSUtilities_Windows.cpp
///
///  Implementation of the class QSUtilities for Windows.
///
///  Copyright (C) 2008-2015 Simba Technologies Incorporated.
//==================================================================================================

#include "QSUtilities.h"

#include "TabbedUnicodeFileReader.h"

using namespace Simba::Quickstart;
using namespace std;

// Helpers =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Determine if the given file exists or not.
///
/// @param in_file              The file name to check for existence.
///
/// @return True if the file exists; false otherwise.
////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool DoesExist(const simba_wstring& in_file)
{
    // Try to find the file.
    WIN32_FIND_DATA winFindData;
    HANDLE findHandle = FindFirstFile(in_file.GetAsPlatformWString().c_str(), &winFindData);

    bool isFound = (INVALID_HANDLE_VALUE != findHandle);

    FindClose(findHandle);

    return isFound;
}

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSUtilities::QSUtilities(QuickstartSettings* in_settings) : m_settings(in_settings)
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSUtilities::~QSUtilities()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSUtilities::DoesTableDataExist(const simba_wstring& in_tableName) const
{
    return DoesExist(GetTableDataFullPathName(in_tableName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSUtilities::DoesTableMetadataExist(const simba_wstring& in_tableName) const
{
    return DoesExist(GetTableMetadataFullPathName(in_tableName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSUtilities::GetTableDataFullPathName(const simba_wstring& in_fileName) const
{
    return m_settings->m_dbfPath + QS_PATH_SEPARATOR + in_fileName + QS_DATAFILE_SUFFIX;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSUtilities::GetTableMetadataFullPathName(const simba_wstring& in_fileName) const
{
    return m_settings->m_dbfPath + QS_PATH_SEPARATOR + in_fileName + QS_METADATAFILE_SUFFIX;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void QSUtilities::GetTables(std::vector<simba_wstring>& out_tables) const
{
    // Look in the catalog directory for new files.
    simba_wstring fileFilter(m_settings->m_dbfPath + QS_PATH_SEPARATOR + QS_DATAFILE_FILTER);

    WIN32_FIND_DATA winFindData;
    HANDLE findHandle = FindFirstFile(fileFilter.GetAsPlatformWString().c_str(), &winFindData);

    // Add all the tables in the catalog.
    if (findHandle)
    {
        try
        {
            do
            {
                simba_wstring fileName(winFindData.cFileName);

                // Get the table name without the .dbf extension.
                simba_int32 length =
                    static_cast<simba_int32>(fileName.GetLength() - QS_DATAFILE_SUFFIX.length());
                simba_wstring tableName = fileName.Substr(0, length);

                out_tables.push_back(tableName);
            } while (FindNextFile(findHandle, &winFindData));

            FindClose(findHandle);
        }
        catch (...)
        {
            FindClose(findHandle);
            throw;
        }
    }
}
