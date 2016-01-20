//==================================================================================================
///  @file QSConnection.cpp
///
///  Quickstart DSIConnection implementation class.
///
///  Copyright (C) 2008-2014 Simba Technologies Incorporated.
// ================================================================================================

#include "QSConnection.h"

#include "DSIPropertyUtilities.h"
#include "IEnvironment.h"
#include "ILogger.h"
#include "MapUtilities.h"
#include "NumberConverter.h"
#include "QSStatement.h"
#include "SimbaSettingReader.h"

#define KXVER 3
#define WIN32 1
#include "k.h"


#if defined(WIN32) || defined(_WIN64)
#include "QSConnectionDialog.h"
#endif

#ifdef OLEDBTARGET
#include <oledb.h>
#endif

using namespace Simba::Quickstart;
using namespace Simba::DSI;

// Static ==========================================================================================
QSCustomPropertyKeyMap QSConnection::s_customConnPropertyKeys = InitializeCustomConnPropertyKeys();

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSConnection::QSConnection(IEnvironment* in_environment) :
    DSIConnection(in_environment),
    m_log(in_environment->GetLog()),
    m_isConnected(false)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "QSConnection");

    // Default connection properties are set by the DSIConnection parent class, however they can be
    // overridden as in SetConnectionPropertyValues. See DSIConnProperties.h for more information on
    // default properties.
    SetConnectionPropertyValues();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSConnection::~QSConnection()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSConnection::Connect(const DSIConnSettingRequestMap& in_connectionSettings)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "Connect");

#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #6: Establish A Connection.")
    // The SimbaEngine SDK will call UpdateConnectionSettings() prior to calling this function.
    // This will ensure that all valid keys required to create a connection have been specified
    // and are stored within in_connectionSettings. When this function has has been called, all of
    // the required keys that were marked in UpdateConnectionSettings() are present in
    // in_connectionSettings map, while the optional keys may or may not be present. Use
    // GetRequiredSetting()/GetOptionalSetting() to easily access key values in the map.
    //
    // This method should validate each of the incoming values and throw an error in the event
    // of an invalid value.

    // If QuickstartDSII is built as a server, the DBF key is in the HKLM\\Simba\\Quickstart\\Server
    // registry key on the server.
#ifdef SERVERTARGET
    UNUSED(in_connectionSettings);

    m_settings.m_dbfPath = SimbaSettingReader::ReadSetting(QS_DBF_KEY.GetAsPlatformString());

    if (m_settings.m_dbfPath.IsNull())
    {
        QSTHROW(DIAG_INVALID_AUTH_SPEC, L"QSDbfNotFound");
    }

    m_settings.defaultMaxColumnSize = QS_MAX_STRING_COLUMN_SIZE;
    m_settings.m_useCustomSQLStates = false;
#else

    // Store the DSN settings in case they're needed later.
    m_connectionSettingsMap = in_connectionSettings;

    // Look up the DBF path for Quickstart tables in the connection string.
    m_settings.m_dbfPath =
        GetRequiredSetting(QS_DBF_KEY, in_connectionSettings).GetStringValue();

    // Look up optional settings for the driver, substitute default values if the key is not present
    // in the connection string.
    const Variant* data = NULL;
    if (GetOptionalSetting(QS_DEFAULT_MAX_COLUMN_SIZE_KEY, in_connectionSettings, &data))
    {
        m_settings.defaultMaxColumnSize = data->GetUInt32Value();
    }
    else
    {
        m_settings.defaultMaxColumnSize = QS_MAX_STRING_COLUMN_SIZE;
    }

    if (GetOptionalSetting(QS_USE_CUSTOM_STATES_KEY, in_connectionSettings, &data))
    {
        m_settings.m_useCustomSQLStates = data->GetWStringValue().IsEqual("Y", false);
    }
    else
    {
        m_settings.m_useCustomSQLStates = false;
    }

    if (GetOptionalSetting(QS_USE_CACHING_KEY, in_connectionSettings, &data))
    {
        m_settings.m_useTableCaching = data->GetWStringValue().IsEqual("Y", false);
    }
    else
    {
        m_settings.m_useTableCaching = false;
    }

    if (GetOptionalSetting(QS_LOCALE, in_connectionSettings, &data))
    {
        m_locale = data->GetWStringValue().GetAsPlatformString();
    }

#endif

	kdbconn = khp("localhost",5001);
    // This flag is simply to fake a connected status, there is no real connection to a data-source
    // as all of the data is in flat files. Keep or remove this flag as needed.
    m_isConnected = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IStatement* QSConnection::CreateStatement()
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "CreateStatement");
    return new QSStatement(this, &m_settings);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSConnection::Disconnect()
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "Disconnect");
    m_isConnected = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AttributeData* QSConnection::GetCustomProperty(simba_int32 in_key)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "GetCustomProperty");

    // This code shows how to implement a custom connection property. It can be removed if no custom
    // connection properties are needed.
    QSCustomPropertyMap::iterator itr = m_customConnProperties.find(in_key);
    if (m_customConnProperties.end() == itr)
    {
        // Cannot find property with in_key.
        QSTHROWGEN1(L"CustomPropNotFound", NumberConverter::ConvertInt32ToWString(in_key));
    }

    return itr->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AttributeType QSConnection::GetCustomPropertyType(simba_int32 in_key)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "GetCustomPropertyType");

    // This code shows how to implement a custom connection property. It can be removed if no custom
    // connection properties are needed.
    QSCustomPropertyKeyMap::iterator itr = s_customConnPropertyKeys.find(in_key);
    if (s_customConnPropertyKeys.end() == itr)
    {
        // Cannot find property with in_key.
        QSTHROWGEN1(L"CustomPropKeyNotFound", NumberConverter::ConvertInt32ToWString(in_key));
    }

    return itr->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ILogger* QSConnection::GetLog()
{
#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #4: Set the connection-wide logging details.")
    // This implementation uses one log for the entire driver, however it's possible to also use a
    // separate log for each connection, thus enabling better debugging of multiple connections
    // at once.
    return m_log;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSConnection::IsCustomProperty(simba_int32 in_key)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "IsCustomProperty");

    // This code shows how to implement a custom connection property. It can be removed if no custom
    // connection properties are needed.
    return (s_customConnPropertyKeys.end() != s_customConnPropertyKeys.find(in_key));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSConnection::PromptDialog(
    DSIConnSettingResponseMap& io_connResponseMap,
    DSIConnSettingRequestMap& io_connectionSettings,
    HWND in_parentWindow,
    PromptType in_promptType)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "PromptDialog");

    // Avoid compiler warnings.
    UNUSED(io_connResponseMap);

#if !defined(SERVERTARGET) && (defined(WIN32) || defined(_WIN64))

    return QSConnectionDialog::Prompt(
        io_connectionSettings,
        m_log,
        in_parentWindow,
        (PROMPT_REQUIRED == in_promptType));

#else
    UNUSED(in_parentWindow);
    UNUSED(in_promptType);

    // Unrecognized prompt type.
    return false;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSConnection::SetCustomProperty(simba_int32 in_key, AttributeData* in_value)
{
    AutoPtr<AttributeData> value(in_value);
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "SetCustomProperty");

    // This code shows how to implement a custom connection property. It can be removed if no custom
    // connection properties are needed.
     MapUtilities::InsertOrUpdate(m_customConnProperties, in_key, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSConnection::UpdateConnectionSettings(
    const DSIConnSettingRequestMap& in_connectionSettings,
    DSIConnSettingResponseMap& out_connectionSettings)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSConnection", "UpdateConnectionSettings");

    // If QuickstartDSII is built as a server, delegate looking for the DBF key to Connect().
#ifdef SERVERTARGET
    UNUSED(in_connectionSettings);
    UNUSED(out_connectionSettings);
    return;
#else

#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #5: Check Connection Settings.")
    // This function will receive all incoming connection settings as specified in the DSN used to
    // establish the connection.  Use the VerifyRequiredSetting()/VerifyOptionalSetting() utility
    // functions to ensure that all required and optional keys are present to establish a
    // connection. If any required values are missing, then the driver will fail to connect or call
    // PromptDialog(), depending on the connection settings. If all required values are present,
    // then Connect() will be called.
    //
    // If a key is missing from the DSN, then add it to the out_connectionSettings.
    //
    // Data validation for the keys should be done in Connect()
    //
    // This driver requires a single key in the DSN, "DBF" which represents the file location to
    // be searched.
    // This driver has the following settings:
    //      Required Key: DBF - represents the file location to use for data files.
    //      Optional Key: DEFAULTMAXCOLUMNSIZE - the default variable length column size.
    //      Optional Key: USECUSTOMSQLSTATES - flag indicating custom SQL states should be used if a
    //                                         table is opened that doesn't exist.
    //      Optional Key: ENABLECACHING - flag indicating table caching should be enabled.
    //      Optional Key: LOCALE - key for changing the connection locale.
    VerifyRequiredSetting(QS_DBF_KEY, in_connectionSettings, out_connectionSettings);
    VerifyOptionalSetting(QS_DEFAULT_MAX_COLUMN_SIZE_KEY, in_connectionSettings, out_connectionSettings);
    VerifyOptionalSetting(QS_USE_CUSTOM_STATES_KEY, in_connectionSettings, out_connectionSettings);
    VerifyOptionalSetting(QS_USE_CACHING_KEY, in_connectionSettings, out_connectionSettings);
    VerifyOptionalSetting(QS_LOCALE, in_connectionSettings, out_connectionSettings);
#endif
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSCustomPropertyKeyMap QSConnection::InitializeCustomConnPropertyKeys()
{
    // This code shows how to implement a custom connection property. It can be removed if no custom
    // connection properties are needed.
    QSCustomPropertyKeyMap map;

    map.insert(std::make_pair(QS_CUSTOM_PROP_POINTER, ATTR_POINTER));
    map.insert(std::make_pair(QS_CUSTOM_PROP_WSTRING, ATTR_WSTRING));
    map.insert(std::make_pair(QS_CUSTOM_PROP_INT32, ATTR_INT32));
    map.insert(std::make_pair(QS_CUSTOM_PROP_UINT32, ATTR_UINT32));
    map.insert(std::make_pair(QS_CUSTOM_PROP_INT16, ATTR_INT16));
    map.insert(std::make_pair(QS_CUSTOM_PROP_UINT16, ATTR_UINT16));

    return map;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSConnection::SetConnectionPropertyValues()
{
    // Indicate the driver is read-only.
    DSIPropertyUtilities::SetReadOnly(this, true);

    // Turn off schema support.
    DSIPropertyUtilities::SetSchemaSupport(this, false);

#ifdef OLEDBTARGET
    ConnectionSettingInfo dbfPropInfo;
    dbfPropInfo.key = "DBF";
    dbfPropInfo.isRequired = true;
    dbfPropInfo.valueType = Variant::TYPE_WSTRING;

    m_settingInfo[DBPROP_INIT_DATASOURCE] = dbfPropInfo;
#endif
    // Note that DSI_CONN_SERVER_NAME and DSI_CONN_USER_NAME should be updated after connection to
    // reflect the server that was connected to and the user that connected. If no credentials are
    // needed, then DSI_CONN_USER_NAME can be set to an empty simba_wstring.
    SetProperty(DSI_CONN_SERVER_NAME, AttributeData::MakeNewWStringAttributeData("Quickstart"));
    SetProperty(DSI_CONN_USER_NAME, AttributeData::MakeNewWStringAttributeData("User"));
    SetProperty(DSI_CONN_CURRENT_CATALOG, AttributeData::MakeNewWStringAttributeData(QS_CATALOG));
}
