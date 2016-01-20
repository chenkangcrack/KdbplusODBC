//==================================================================================================
///  @file QSStatement.cpp
///
///  Quickstart DSIStatement implementation class.
///
///  Copyright (C) 2009-2015 Simba Technologies Incorporated.
//==================================================================================================

#include "QSStatement.h"

#include "IConnection.h"
#include "MapUtilities.h"
#include "NumberConverter.h"
#include "QSDataEngine.h"

using namespace Simba::Quickstart;
using namespace Simba::DSI;

// Static ==========================================================================================
QSCustomPropertyKeyMap QSStatement::s_customStmtPropertyKeys = InitializeCustomStmtPropertyKeys();

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSStatement::QSStatement(IConnection* in_connection, QuickstartSettings* in_settings) :
    DSIStatement(in_connection),
    m_settings(in_settings)
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSStatement", "QSStatement");
    // Default property values and limits can be overridden here.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSStatement::~QSStatement()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IDataEngine* QSStatement::CreateDataEngine()
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSStatement", "CreateDataEngine");
    return new QSDataEngine(this, m_settings);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AttributeData* QSStatement::GetCustomProperty(simba_int32 in_key)
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSStatement", "GetCustomProperty");

    // This code shows how to implement a custom statement property. It can be removed if no custom
    // statement properties are needed.
    QSCustomPropertyMap::iterator itr = m_customStmtProperties.find(in_key);
    if (m_customStmtProperties.end() == itr)
    {
        // Cannot find property with in_key.
        QSTHROWGEN1(L"CustomPropNotFound", NumberConverter::ConvertInt32ToWString(in_key));
    }

    return itr->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AttributeType QSStatement::GetCustomPropertyType(simba_int32 in_key)
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSStatement", "GetCustomPropertyType");

    // This code shows how to implement a custom statement property. It can be removed if no custom
    // statement properties are needed.
    QSCustomPropertyKeyMap::iterator itr = s_customStmtPropertyKeys.find(in_key);
    if (s_customStmtPropertyKeys.end() == itr)
    {
        // Cannot find property with in_key.
        QSTHROWGEN1(L"CustomPropKeyNotFound", NumberConverter::ConvertInt32ToWString(in_key));
    }

    return itr->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSStatement::IsCustomProperty(simba_int32 in_key)
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSStatement", "IsCustomProperty");

    // This code shows how to implement a custom statement property. It can be removed if no custom
    // statement properties are needed.
    return (s_customStmtPropertyKeys.end() != s_customStmtPropertyKeys.find(in_key));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSStatement::SetCustomProperty(simba_int32 in_key, AttributeData* in_value)
{
    AutoPtr<AttributeData> value(in_value);
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSStatement", "SetCustomProperty");

    // This code shows how to implement a custom statement property. It can be removed if no custom
    // statement properties are needed.
    MapUtilities::InsertOrUpdate(m_customStmtProperties, in_key, value);
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSCustomPropertyKeyMap QSStatement::InitializeCustomStmtPropertyKeys()
{
    // This code shows how to implement a custom statement property. It can be removed if no custom
    // statement properties are needed.
    QSCustomPropertyKeyMap map;

    map.insert(std::make_pair(QS_CUSTOM_PROP_POINTER, ATTR_POINTER));
    map.insert(std::make_pair(QS_CUSTOM_PROP_WSTRING, ATTR_WSTRING));
    map.insert(std::make_pair(QS_CUSTOM_PROP_INT32, ATTR_INT32));
    map.insert(std::make_pair(QS_CUSTOM_PROP_UINT32, ATTR_UINT32));
    map.insert(std::make_pair(QS_CUSTOM_PROP_INT16, ATTR_INT16));
    map.insert(std::make_pair(QS_CUSTOM_PROP_UINT16, ATTR_UINT16));

    return map;
}
