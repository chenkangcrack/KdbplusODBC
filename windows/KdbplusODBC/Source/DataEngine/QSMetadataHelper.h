//==================================================================================================
///  @file QSMetadataHelper.h
///
///  Definition of the class QSMetadataHelper.
///
///  Copyright (C) 2010-2015 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSMETADATAHELPER_H_
#define _SIMBA_QUICKSTART_QSMETADATAHELPER_H_

#include "Quickstart.h"

#include "DSIExtMetadataHelper.h"

namespace Simba
{
namespace DSI
{
    class IStatement;
}
}

namespace Simba
{
namespace Quickstart
{
    /// @brief Quickstart metadata helper implementation.
    ///
    /// This class provides a list of tables and stored procedures that are used to construct
    /// default implementations of the metadata sources for tables, columns, procedures, and
    /// procedure columns.
    ///
    /// Each table or procedure will be opened to obtain metadata about it, although data will
    /// not be fetched.
    ///
    /// Note that use of this class is optional, and if an implementation is returned via
    /// IDataEngine.MakeNewMetadataTable(), the default implementation will _NOT_ be used.
    class QSMetadataHelper : public Simba::SQLEngine::DSIExtMetadataHelper
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        ///
        /// @param in_statement         The parent statement. (NOT OWN)
        /// @param in_settings          The connection settings. (NOT OWN)
        QSMetadataHelper(Simba::DSI::IStatement* in_statement, QuickstartSettings* in_settings);

        /// @brief Destructor.
        virtual ~QSMetadataHelper();

        /// @brief Get the next procedure in the data source.
        ///
        /// When this function is first called it should return the first procedure in the data
        /// source, and on each successive call it should return the next procedure. When there are
        /// no more procedures, it should return false.
        ///
        /// @param out_procedure        The struct to fill with information about the next procedure.
        ///
        /// @return True if there was another procedure; false otherwise.
        virtual bool GetNextProcedure(Simba::SQLEngine::Identifier& out_procedure);

        /// @brief Get the next procedure in the data source.
        ///
        /// When this function is first called it should return the first table in the data source,
        /// and on each successive call it should return the next table. When there are no more
        /// tables, it should return false.
        ///
        /// @param out_table            The struct to fill with information about the next table.
        ///
        /// @return True if there was another table; false otherwise.
        virtual bool GetNextTable(Simba::SQLEngine::Identifier& out_table);

    // Private =====================================================================================
    private:
        // The parent statement. (NOT OWN)
        Simba::DSI::IStatement* m_statement;

        // Struct containing connection settings and error codes. (NOT OWN)
        QuickstartSettings* m_settings;

        // The list of tables in the data source.
        std::vector<simba_wstring> m_tables;

        // The current table.
        std::vector<simba_wstring>::iterator m_tableIter;

        // Flag indicating if the table fetch has started.
        bool m_hasStartedTableFetch;
    };
}
}

#endif
