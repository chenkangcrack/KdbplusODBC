//==================================================================================================
///  @file QSDataEngine.cpp
///
///  Implementation of the class QSDataEngine.
///
///  Copyright (C) 2008-2015 Simba Technologies Incorporated.
//==================================================================================================

#include "QSDataEngine.h"

#include "QSMetadataHelper.h"
#include "QSTable.h"
#include "QSTypeInfoMetadataSource.h"
#include "QSUtilities.h"

using namespace Simba::Quickstart;
using namespace Simba::DSI;
using namespace Simba::SQLEngine;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSDataEngine::QSDataEngine(IStatement* in_statement, QuickstartSettings* in_settings) :
    DSIExtSqlDataEngine(in_statement),
    m_settings(in_settings)
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSDataEngine", "QSDataEngine");

    if (in_settings->m_useTableCaching)
    {
        // Enable table caching, so tables are only retrieved from the data source once per query
        // and then cached internally by the SQLEngine.
        SetProperty(
            DSIEXT_DATAENGINE_TABLE_CACHING,
            AttributeData::MakeNewWStringAttributeData(L"Y"));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSDataEngine::~QSDataEngine()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
DSIMetadataSource* QSDataEngine::MakeNewMetadataTable(
    DSIMetadataTableID in_metadataTableID,
    DSIMetadataRestrictions& in_restrictions,
    const simba_wstring& in_escapeChar,
    const simba_wstring& in_identifierQuoteChar,
    bool in_filterAsIdentifier)
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSDataEngine", "MakeNewMetadataTable");

    // Avoid compiler warnings.
    UNUSED(in_escapeChar);
    UNUSED(in_identifierQuoteChar);
    UNUSED(in_filterAsIdentifier);

#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #7: Create and return your Metadata Sources.")
    // The metadata sources return the catalog metadata about your data-source. They correspond to
    // the catalog functions for the various APIs, for instance SQLColumns in ODBC would result in
    // an DSI_COLUMNS_METADATA source being created to return the data. At the very least, ODBC
    // conforming applications will require the following metadata sources:
    //
    //  DSI_TABLES_METADATA
    //      List of all tables defined in the data source.
    //      This source is constructed via the QSMetadataHelper and SQL Engine.
    //
    //  DSI_CATALOGONLY_METADATA
    //      List of all catalogs defined in the data source.
    //      This source is constructed via the QSMetadataHelper and SQL Engine.
    //
    //  DSI_SCHEMA_METADATA
    //      List of all schemas defined in the data source.
    //      This source is constructed via the QSMetadataHelper and SQL Engine.
    // QuickstartDSII does not support schemas.
    //
    //  DSI_TABLETYPEONLY_METADATA
    //      List of all table types (TABLE,VIEW,SYSTEM) defined within the data source.
    //      This source is constructed via the QSMetadataHelper and SQL Engine.
    //
    //  DSI_COLUMNS_METADATA
    //      List of all columns defined across all tables in the data source.
    //      This source is constructed via the QSMetadataHelper and SQL Engine.
    //
    //  DSI_TYPE_INFO_METADATA
    //      List of the supported types by the data source.
    //
    //  In some cases applications may provide values to restrict the metadata sources.
    //  These restrictions are stored within in_restrictions and can be used by you to restrict
    //  the number of rows that are returned from the metadata source.

    // Most catalog types are created using the metadata helper in conjunction with the SQL Engine.
    switch (in_metadataTableID)
    {
        case DSI_TYPE_INFO_METADATA:
        {
            return new QSTypeInfoMetadataSource(in_restrictions, IsODBCVersion3());
        }

        default:
        {
            return NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
SharedPtr<DSIExtResultSet> QSDataEngine::OpenTable(
    const simba_wstring& in_catalogName,
    const simba_wstring& in_schemaName,
    const simba_wstring& in_tableName,
    DSIExtTableOpenType in_openType)
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSDataEngine", "OpenTable");
    assert(in_openType == TABLE_OPEN_READ_ONLY);

#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #8: Open A Table.")

    // This method simply needs to validate that a "table" exists within the given catalog.
    // Simply return NULL if the table name cannot be resolved.
    UNUSED(in_catalogName);

    // QuickstartDSII does not support schemas, so in_schemaName should always be empty.
    assert(in_schemaName.IsEmpty());

    SharedPtr<DSIExtResultSet> table;
    QSUtilities utilities(m_settings);

    // Linux is case-sensitive, therefore change the table name to upper case for comparison. Don't
    // do it on a per-platform basis so that metadata is consistent (ie table name is all caps).
    simba_wstring tableName(in_tableName);
    tableName.ToUpper();

    if (utilities.DoesTableDataExist(tableName))
    {
        table = new QSTable(
            m_settings,
            GetLog(),
            tableName,
            m_statement->GetWarningListener(),
            IsODBCVersion3());
    }
    else if (m_settings->m_useCustomSQLStates)
    {
        // This is an example of throwing an error with a custom SQL state. If a query involving
        // a non-existent table is executed, the driver will return SQL_ERROR, a SQL state of
        // QS_DATAENGINE_STATE (QS001) and an error message that the table does not exist.
        //
        // NOTE: This is only an example of throwing a custom SQL State, to indicate a table does
        // not exist to the SDK this function should return an empty SharedPtr in a production
        // driver.
        QSTHROW1(QS_DATAENGINE_STATE, L"QSTableDoesNotExist", in_tableName);
    }

    return table;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AutoPtr<DSIExtMetadataHelper> QSDataEngine::CreateMetadataHelper()
{
    return AutoPtr<DSIExtMetadataHelper> (new QSMetadataHelper(m_statement, m_settings));
}
