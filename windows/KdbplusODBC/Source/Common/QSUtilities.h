//==================================================================================================
///  @file QSUtilities.h
///
///  Definition of the class QSUtilities.
///
///  Copyright (C) 2009-2015 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSUTILITIES_H_
#define _SIMBA_QUICKSTART_QSUTILITIES_H_

#include "Quickstart.h"

#include <vector>

namespace Simba
{
namespace Quickstart
{
    /// @brief Quickstart platform specific utility functions.
    class QSUtilities
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        ///
        /// @param in_settings              The connection settings to use. (NOT OWN)
        QSUtilities(QuickstartSettings* in_settings);

        /// @brief Destructor.
        ~QSUtilities();

        /// @brief Utility function to determine if a data file representing the table exists or
        /// not.
        ///
        /// @param in_tableName             The name of the table to check.
        ///
        /// @return True if the table exists; false otherwise.
        bool DoesTableDataExist(const simba_wstring& in_tableName) const;

        /// @brief Utility function to determine if a data file representing the table's metadata
        /// exists or not.
        ///
        /// @param in_tableName             The name of the table to check.
        ///
        /// @return True if the table metadata exists; false otherwise.
        bool DoesTableMetadataExist(const simba_wstring& in_tableName) const;

        /// @brief Gets the full path to the data file for the given file name.
        ///
        /// @param in_fileName              The data file name.
        ///
        /// @return The full path to the data file for a given file name.
        simba_wstring GetTableDataFullPathName(const simba_wstring& in_fileName) const;

        /// @brief Gets the full path to the metadata file for the given file name.
        ///
        /// @param in_fileName              The data file name.
        ///
        /// @return The full path to the metadata file for a given file name.
        simba_wstring GetTableMetadataFullPathName(const simba_wstring& in_fileName) const;
        /// @brief Gets the table names in the DBF.
        /// @param out_tables               Vector of table names.
        void GetTables(std::vector<simba_wstring>& out_tables) const;

    // Private =====================================================================================
    private:
        // Struct containing connection settings and error codes. (NOT OWN)
        QuickstartSettings* m_settings;
    };
}
}

#endif
