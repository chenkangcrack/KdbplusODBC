//==================================================================================================
///  @file QSDataEngine.h
///
///  Definition of the class QSDataEngine.
///
///  Copyright (C) 2008-2015 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSDATAENGINE_H_
#define _SIMBA_QUICKSTART_QSDATAENGINE_H_

#include "Quickstart.h"

#include "DSIExtSqlDataEngine.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Quickstart data engine implementation.
    ///
    /// The Quickstart Data Engine leverages Simba's SQL Engine to perform the SQL processing.
    class QSDataEngine : public Simba::SQLEngine::DSIExtSqlDataEngine
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        ///
        /// @param in_statement             The parent statement. (NOT OWN)
        /// @param in_settings              The connection settings. (NOT OWN)
        QSDataEngine(Simba::DSI::IStatement* in_statement, QuickstartSettings* in_settings);

        /// @brief Destructor.
        virtual ~QSDataEngine();

        /// @brief Makes a new metadata source that contains raw metadata.
        ///
        /// A map of column tags to literal string restrictions will be passed into the metadata
        /// source, which enables the metadata source cut down on the size or the raw table.
        /// Although a map of restrictions is given to the metadata source, the restrictions are
        /// just a suggestion.
        /// Metadata source may choose to ignore the restrictions.
        ///
        /// @param in_metadataTableID       Identifier to create the appropriate metadata table
        /// @param in_restrictions          Restrictions that may be applied to the metadata table.
        /// @param in_escapeChar            Escape character used in filtering.
        /// @param in_identifierQuoteChar   Quote identifier, which is the quotation mark that this
        ///                                 filter recognizes.
        /// @param in_filterAsIdentifier    Indicates if string filters are treated as identifiers.
        ///                                 If in_metadataID is true, string filters are treated as
        ///                                 identifiers.  Otherwise, filters are not treated as
        ///                                 identifiers.
        ///
        /// @return New metadata source that contains raw metadata, or NULL if not supported. (OWN)
        Simba::DSI::DSIMetadataSource* MakeNewMetadataTable(
            Simba::DSI::DSIMetadataTableID in_metadataTableID,
            Simba::DSI::DSIMetadataRestrictions& in_restrictions,
            const simba_wstring& in_escapeChar,
            const simba_wstring& in_identifierQuoteChar,
            bool in_filterAsIdentifier);

        /// @brief Open a physical table/view.
        ///
        /// This method will be called during the preparation of a SQL statement.
        ///
        /// Once the table is opened, it should allow retrieving of metadata. That is, calling
        /// GetSelectColumns() on the returned table should return column metadata. SimbaEngine
        /// needs the table metadata to infer the column metadata of the result set if the
        /// SQL statement is a query.
        ///
        /// Before data can be retrieved from the table, SetCursorType() will have to called.
        /// Since this is done at the execution time, the DSII should _NOT_ try to make the data
        /// ready for retrieval until SetCursorType() is called.
        ///
        /// The DSII decides how catalog and schema are interpreted or supported. If the same
        /// table is asked to open twice (that is, OpenTable() is called twice), the DSII _MUST_
        /// return two separate DSIExtResultSet instances since two cursors will be needed.
        ///
        /// SimbaEngine will ensure this method is called only once if the table is referenced only
        /// once in a SQL statement.
        ///
        /// @param in_catalogName           The name of the catalog in which the table resides.
        /// @param in_schemaName            The name of the schema in which the table resides.
        /// @param in_tableName             The name of the table to open.
        /// @param in_openType              An enum indicating how the table should be opened.
        ///
        /// @return the opened table, NULL if the table does not exist.
        virtual SharedPtr<Simba::SQLEngine::DSIExtResultSet> OpenTable(
            const simba_wstring& in_catalogName,
            const simba_wstring& in_schemaName,
            const simba_wstring& in_tableName,
            Simba::SQLEngine::DSIExtTableOpenType in_openType);

        /// @brief Create a helper object to retrieve basic metadata that can be used to provide
        /// default implementations of catalog metadata sources.
        ///
        /// The default implementation in this class returns NULL. That is, no default metadata
        /// is supported.
        ///
        /// @return A helper object to retrieve basic metadata if supported, NULL otherwise.
        virtual AutoPtr<Simba::SQLEngine::DSIExtMetadataHelper> CreateMetadataHelper();

    // Private =====================================================================================
    private:
        // Struct containing connection settings and error codes. (NOT OWN)
        QuickstartSettings* m_settings;
    };
}
}

#endif
