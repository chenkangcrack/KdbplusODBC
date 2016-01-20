//==================================================================================================
///  @file QSTableUtilities.cpp
///
///  Implementation of the class QSTableUtilities.
///
///  Copyright (C) 2009-2014 Simba Technologies Incorporated.
//==================================================================================================

#include "QSTableUtilities.h"

#include "AutoPtr.h"
#include "DSIColumnMetadata.h"
#include "DSIResultSetColumn.h"
#include "DSIResultSetColumns.h"
#include "NumberConverter.h"
#include "QSUtilities.h"
#include "SqlTypeMetadata.h"
#include "SqlTypeMetadataFactory.h"
#include "TabbedUnicodeFileReader.h"

using namespace Simba;
using namespace Simba::Quickstart;
using namespace Simba::DSI;

// Static ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
AutoPtr<DSIResultSetColumns> QSTableUtilities::InferColumnsFromDataFile(
    QuickstartSettings* in_settings,
    const simba_wstring& in_tableName)
{
    QSUtilities utilities(in_settings);

    // Open a file reader to the data file.
    simba_wstring dataFilePath(utilities.GetTableDataFullPathName(in_tableName));
    TabbedUnicodeFileReader fileReader(dataFilePath);

    // Get the number of columns defined.
    const simba_size_t columnCount = fileReader.GetNumColumns();
    const simba_wstring columnNamePrefix(L"Column_");

    // Create a new columnMetadata for each defined column.
    // By default each column specified is SQL_WVARCHAR of a pre-chosen length.
    AutoPtr<DSIResultSetColumns> columns(new DSIResultSetColumns());
    SqlTypeMetadataFactory* typeFactory = SqlTypeMetadataFactorySingleton::GetInstance();
    for (simba_size_t i = 0; i < columnCount; ++i)
    {
        AutoPtr<DSIColumnMetadata> columnMetadata(new DSIColumnMetadata());

        columnMetadata->m_catalogName = in_settings->m_dbfPath;
        columnMetadata->m_schemaName.Clear();
        columnMetadata->m_tableName = in_tableName;

        // When the label/name are set on a column, m_unnamed should be set to false, otherwise
        // the name will not be reported.
        columnMetadata->m_name = columnNamePrefix + NumberConverter::ConvertInt64ToWString(i);
        columnMetadata->m_label = columnMetadata->m_name;
        columnMetadata->m_unnamed = false;

        assert(!columnMetadata->m_name.IsNull());
        assert(!columnMetadata->m_tableName.IsNull());

        columnMetadata->m_charOrBinarySize = in_settings->defaultMaxColumnSize;
        columnMetadata->m_nullable = DSI_NULLABLE;

        AutoPtr<SqlTypeMetadata> sqlTypeMetadata(typeFactory->CreateNewSqlTypeMetadata(SQL_WVARCHAR));
        sqlTypeMetadata->SetLengthOrIntervalPrecision(columnMetadata->m_charOrBinarySize);

        columns->AddColumn(new DSIResultSetColumn(sqlTypeMetadata, columnMetadata));
    }

    return columns;
}
