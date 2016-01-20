//==================================================================================================
///  @file QSTableMetadataFile.h
///
///  Definition of the Interface QSTableMetadataFile.
///
///  Copyright (C) 2009-2015 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSTABLEMETADATAFILE_H_
#define _SIMBA_QUICKSTART_QSTABLEMETADATAFILE_H_

#include "Quickstart.h"
#include "AutoPtr.h"
#include "DSIResultSetColumns.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Helper class to read table metadata files.
    class QSTableMetadataFile
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        ///
        /// @param in_settings          The settings for this connection. (NOT OWN)
        /// @param in_isODBCV3          Whether ODBC 3.X is in use
        QSTableMetadataFile(QuickstartSettings* in_settings, bool in_isODBCV3 = true);

        /// @brief Destructor.
        ~QSTableMetadataFile();

        /// @brief Reads an XML metadata file that corresponds to the specified table name.
        ///
        /// @param in_tableName         The name of the table to read.
        ///
        /// @return DSIResultSetColumns for the specified table.
        AutoPtr<Simba::DSI::DSIResultSetColumns> Read(const simba_wstring& in_tableName);

    // Private =====================================================================================
    private:
        /// @brief A helper method to add a newly parsed column.
        ///
        /// This method is called from ParseMetadataFile() for every Column tag.
        ///
        /// @param in_columnName        The column name.
        /// @param in_sqlType           The SQL type of the column.
        /// @param is_isSigned          Whether the column is signed.
        void AddColumnMetadata(
            const simba_wstring& in_columnName,
            const simba_wstring& in_sqlType,
            const bool in_isUnsigned = false);

        /// @brief Parses the specified XML metadata file.
        ///
        /// @param in_file              The path to the XML metadata file.
        void ParseMetadataFile(const simba_wstring& in_file);

        /// @brief Handler for the XML Parser. Called for a starting element tag.
        ///
        /// @param in_userData          Pointer to a buffer that gets adjusted when this is called.
        /// @param in_name              The element name. This is NULL terminated.
        /// @param in_attributes        The element attributes. This array stores attribute names
        ///                             in the even indices and corresponding attribute values in
        ///                             odd indices. A NULL pointer is placed in the index after
        ///                             the last attribute value. All entries are NULL-terminated.
        ///                             An example is if the element <x type=1> appeared,
        ///                             in_attributes = {"type", "1", NULL}
        static void StartElement(
            void* in_userData,
            const simba_char*  in_name,
            const simba_char** in_attributes);

        // Struct containing connection settings and error codes. (NOT OWN)
        QuickstartSettings* m_settings;

        // Cached temporary to overcome static nature of the SAX parser. (OWN)
        AutoPtr<Simba::DSI::DSIResultSetColumns> m_resultSetColumns;

        // The name of the table as passed in Read().
        simba_wstring m_tableName;

        // Whether ODBC 3.X is in use
        bool m_isODBCV3;
    };
}
}

#endif
