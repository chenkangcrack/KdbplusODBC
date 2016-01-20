//==================================================================================================
///  @file QSTypeInfoMetadataSource.cpp
///
///  Definition of the class QSTypeInfoMetadataSource.
///
///  Copyright (C) 2008-2015 Simba Technologies Incorporated.
//==================================================================================================

#include "QSTypeInfoMetadataSource.h"

using namespace Simba::Quickstart;
using namespace Simba::DSI;
using namespace Simba::SQLEngine;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSTypeInfoMetadataSource::QSTypeInfoMetadataSource(
    DSIMetadataRestrictions& in_restrictions,
    bool in_isODBCV3) :
        DSIExtTypeInfoMetadataSource(in_restrictions, in_isODBCV3)
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSTypeInfoMetadataSource::~QSTypeInfoMetadataSource()
{
    ; // Do nothing.
}

// Protected =======================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
TypePrepared QSTypeInfoMetadataSource::PrepareType(SqlTypeInfo& io_typeInfo)
{
    switch (io_typeInfo.m_sqlType)
    {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_BINARY:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
        case SQL_BIT:
        case SQL_DECIMAL:
        case SQL_NUMERIC:
        case SQL_DOUBLE:
        case SQL_FLOAT:
        case SQL_REAL:
        case SQL_TINYINT:
        case SQL_SMALLINT:
        case SQL_INTEGER:
        case SQL_BIGINT:
        case SQL_TYPE_DATE:
        case SQL_TYPE_TIME:
        case SQL_TYPE_TIMESTAMP:
        case SQL_DATE:
        case SQL_TIME:
        case SQL_TIMESTAMP:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        {
            // These types are supported. Leave the type settings as default. Note that this is
            // where changes would be done if the data-source settings for a type were different
            // than the default settings.
            return TP_DONE;
        }

        default:
        {
            // All the other types are not supported.
            return TP_NOT_SUPPORTED;
        }
    }
}
