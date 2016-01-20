//==================================================================================================
///  @file QSTableUtilities.h
///
///  Definition of the class QSTableUtilities.
///
///  Copyright (C) 2009-2015 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSTABLEUTILITIES_H_
#define _SIMBA_QUICKSTART_QSTABLEUTILITIES_H_

#include "Quickstart.h"
#include "AutoPtr.h"

#include "DSIResultSetColumns.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Exposes utilities for determining result set columns.
    class QSTableUtilities
    {
    // Public Static ===============================================================================
    public:
        /// @brief Returns the column metadata for a particular table.
        ///
        /// This method infers the column metadata based on the number of columns defined within
        /// the file.  All columns within the file are assumed to be SQL_WVARCHAR with a preset
        /// maximum length characters controlled by the DSN.
        ///
        /// @param in_settings              The settings for this connection. (NOT OWN)
        /// @param in_tableName             The table name.
        ///
        /// @return The columns in the table.
        static AutoPtr<Simba::DSI::DSIResultSetColumns> InferColumnsFromDataFile(
            QuickstartSettings* in_settings,
            const simba_wstring& in_tableName);

    // Protected ===================================================================================
    protected:
        /// @brief Constructor.
        QSTableUtilities() {}
    };
}
}

#endif
