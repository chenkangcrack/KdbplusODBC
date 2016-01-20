//==================================================================================================
///  @file QSMetadataHelper.cpp
///
///  Implementation of the class QSMetadataHelper.
///
///  Copyright (C) 2010-2014 Simba Technologies Incorporated.
//==================================================================================================

#include "QSMetadataHelper.h"

#include "IStatement.h"
#include "QSUtilities.h"

using namespace Simba::Quickstart;
using namespace Simba::DSI;
using namespace Simba::SQLEngine;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSMetadataHelper::QSMetadataHelper(IStatement* in_statement, QuickstartSettings* in_settings) :
    m_statement(in_statement),
    m_settings(in_settings),
    m_hasStartedTableFetch(false)
{
    assert(in_statement);
    assert(in_settings);

    ENTRANCE_LOG(m_statement->GetLog(), "Simba::Quickstart", "QSMetadataHelper", "QSMetadataHelper");

    // Retrieve the list of all the tables.
	simba_wstring tblName(L"trade");
	m_tables.push_back(tblName);

    // Set the table iterator to the first item.
    m_tableIter = m_tables.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSMetadataHelper::~QSMetadataHelper()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSMetadataHelper::GetNextProcedure(Identifier& out_procedure)
{
    ENTRANCE_LOG(m_statement->GetLog(), "Simba::Quickstart", "QSMetadataHelper", "GetNextProcedure");

    // Quickstart doesn't support stored procedures.
    UNUSED(out_procedure);
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSMetadataHelper::GetNextTable(Identifier& out_table)
{
    ENTRANCE_LOG(m_statement->GetLog(), "Simba::Quickstart", "QSMetadataHelper", "GetNextTable");

    if (m_hasStartedTableFetch)
    {
        // Move to the next table in the collection.
        ++m_tableIter;
    }
    else
    {
        // Start at the initial element in the collection.
        m_hasStartedTableFetch = true;
    }

    if (m_tableIter < m_tables.end())
    {
        // A table was found.
        out_table.m_catalog = QS_CATALOG;
        out_table.m_schema.Clear();
        out_table.m_name = *m_tableIter;

        return true;
    }

    return false;
}
