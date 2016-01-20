//==================================================================================================
///  @file QSDSNConfiguration.cpp
///
///  Implementation of the class QSDSNConfiguration.
///
///  Copyright (C) 2014-2015 Simba Technologies Incorporated.
//==================================================================================================

#include "QSDSNConfiguration.h"

#include "AutoArrayPtr.h"
#include "ConfigurationReader.h"
#include "DSIFileLogger.h"
#include "NumberConverter.h"
#include "SimbaSettingReaderConstants.h"
#include "QSDialogConsts.h"
#include "QSSetupException.h"

#include <odbcinst.h>

using namespace Simba::Quickstart;
using namespace Simba::DSI;
using namespace std;

// Static =========================================================================================
static const simba_wstring DSN_CONFIG_KEY_DSN(L"DSN");
static const simba_wstring DSN_CONFIG_KEY_DRIVER(L"Driver");
#define BYTES_PER_MB 1048576U

// Macros ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Retrieve the specified value from the configurations.
///
/// @param key      The key to look up in the configurations.
////////////////////////////////////////////////////////////////////////////////////////////////////
#define GET_VALUE(key)                                                                             \
{                                                                                                  \
    ConfigurationMap::const_iterator itr = m_configuration.find(key);                              \
    if (itr == m_configuration.end())                                                              \
    {                                                                                              \
        return s_emptyVal;                                                                         \
    }                                                                                              \
    return itr->second;                                                                            \
}

namespace
{
// Helpers =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Get a driver registry value.
///
/// @param in_key                   The key of the value to get. (NOT OWN)
/// @param in_default               The default value to use if the key isn't present.
///                                 (NOT OWN)
///
/// @return The value fetched from the driver registry.
////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring GetDriverRegistryValue(
    const simba_char* in_key,
    const simba_char* in_default)
{
    simba_wstring path(L"SOFTWARE\\");
    path += DRIVER_WINDOWS_BRANDING;
    path += L"\\Driver";

    // Load the driver registry value via the Configuration reader.
    ConfigurationReader reader;
    SectionConfigMap configMap;
    reader.LoadConfiguration(configMap, L"HKEY_LOCAL_MACHINE", path);

    SectionConfigMap::iterator itr = configMap.find(in_key);
    if (configMap.end() == itr)
    {
        // Return the default value if the value isn't found.
        return in_default;
    }

    return itr->second.GetWStringValue();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Set a driver value in the registry.
///
/// @param in_key                   The key of the value to set. (NOT OWN)
/// @param in_value                 The value to set in the registry.
////////////////////////////////////////////////////////////////////////////////////////////////////
void SetDriverRegistryValue(const simba_char* in_key, const simba_wstring& in_value)
{
    simba_wstring path(L"SOFTWARE\\");
    path += DRIVER_WINDOWS_BRANDING;
    path += L"\\Driver";
    std::wstring wPath = path.GetAsPlatformWString();

    // Open the registry key.
    HKEY key;
    if (RegCreateKey(HKEY_LOCAL_MACHINE, wPath.c_str(), &key))
    {
        throw QSSetupException(QSSetupException::CFG_DLG_ERROR_REGISTRY_WRITE);
    }

    try
    {
        // Write the value.
        std::wstring setting = simba_wstring(in_key).GetAsPlatformWString();
        std::wstring value = in_value.GetAsPlatformWString();
        DWORD valueLength = static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t));
        if (RegSetValueEx(
                key,
                setting.c_str(),
                0,
                REG_SZ,
                reinterpret_cast<const BYTE*>(value.c_str()),
                valueLength))
        {
            throw QSSetupException(QSSetupException::CFG_DLG_ERROR_REGISTRY_WRITE);
        }
    }
    catch (...)
    {
        RegCloseKey(key);
        throw;
    }

    RegCloseKey(key);
}
}

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSDSNConfiguration::QSDSNConfiguration(
    const simba_wstring& in_driverDescription,
    ILogger* in_logger) :
        m_driver(in_driverDescription),
        m_logger(in_logger),
        m_configType(CT_NEW_DSN)
{
    assert(m_logger);
    m_logger->LogFunctionEntrance("Simba::Quickstart", "QSDSNConfiguration", "QSDSNConfiguration");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ConfigType QSDSNConfiguration::GetConfigType() const
{
    return m_configType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const simba_wstring& QSDSNConfiguration::GetDBF() const
{
    GET_VALUE(QS_DBF_KEY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const simba_wstring& QSDSNConfiguration::GetDescription() const
{
    GET_VALUE(DESC_KEY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const simba_wstring& QSDSNConfiguration::GetDriver() const
{
    return m_driver;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const simba_wstring& QSDSNConfiguration::GetDSN() const
{
    GET_VALUE(DSN_CONFIG_KEY_DSN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const simba_wstring& QSDSNConfiguration::GetLogLevel() const
{
    GET_VALUE(SETTING_LOGLEVEL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const simba_wstring& QSDSNConfiguration::GetLogMaxNum() const
{
    GET_VALUE(SETTING_LOGFILECOUNT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const simba_wstring& QSDSNConfiguration::GetLogMaxSize() const
{
    GET_VALUE(SETTING_LOGFILESIZE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const simba_wstring& QSDSNConfiguration::GetLogPath() const
{
    GET_VALUE(SETTING_LOGPATH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::Load(const simba_wstring& in_attributeString)
{
 #ifndef OLEDBTARGET
    m_logger->LogFunctionEntrance("Simba::Quickstart", "QSDSNConfiguration", "Load");

    m_configType = CT_EXISTING_DSN;

    // Loading the defaults will also load the logging properties.
    LoadDefaults();

    // NOTE: only DSN attribute is set.
    ParseDSNAttribute(in_attributeString);
    wstring dsn = GetDSN().GetAsPlatformWString();

    if (FALSE == SQLValidDSNW(dsn.c_str()))
    {
        throw QSSetupException(QSSetupException::CFG_DLG_ERROR_INVALID_KEYWORD_VALUE);
    }

    if (!dsn.empty())
    {
        wstring defaultValue;
        AutoArrayPtr<wchar_t> keyBuf(4096);
        simba_int32 keyBufLen = 0;

        // Choose 32KB as max size to ensure the loop will always terminate.
        while (keyBuf.GetLength() < SIMBA_INT16_MAX)
        {
            keyBufLen = SQLGetPrivateProfileStringW(
                dsn.c_str(),            // The DSN or Driver to check.
                NULL,                   // NULL means get key names in this DSN or Driver.
                defaultValue.c_str(),   // Default value of empty string is unused for the case where getting key names.
                keyBuf.Get(),           // Buffer to store key names in.
                static_cast<simba_int32>(keyBuf.GetLength()),// Length of the buffer.
                L"odbc.ini");           // Section/filename to get the keys/values from (ODBC.INI or ODBCINST.INI).

            if (keyBufLen >= static_cast<simba_int32>(keyBuf.GetLength() - 1))
            {
                // SQLGetPrivateProfileString returns bufLen - 1 if it fills the buffer completely.
                // In this case, loop and retry with a larger buffer to see if it has more keys.
                keyBuf.Attach(new wchar_t[keyBuf.GetLength() * 2], keyBuf.GetLength() * 2);
            }
            else
            {
                break;
            }
        }

        if (keyBufLen > 0)
        {
            // Use 4096 as initial buffer length.
            AutoArrayPtr<wchar_t> valBuf(4096);
            simba_int32 valBufLen = 0;

            wchar_t* keyNamePtr = keyBuf.Get();

            while (*keyNamePtr)
            {
                // Null terminate the empty buffer in case no characters are copied into it.
                *valBuf.Get() = '\0';

                while (valBuf.GetLength() < SIMBA_INT16_MAX)
                {
                    valBufLen = SQLGetPrivateProfileStringW(
                        dsn.c_str(),            // The DSN to check.
                        keyNamePtr,             // Get a value for a particular key
                        defaultValue.c_str(),   // Default value of empty string is unused because we know the key must exist.
                        valBuf.Get(),           // Buffer to store the value in.
                        static_cast<simba_int32>(valBuf.GetLength()),// Length of the buffer.
                        L"odbc.ini");           // Section/filename to get the keys/values from (ODBC.INI or ODBCINST.INI).

                    if (valBufLen >= static_cast<simba_int32>(valBuf.GetLength() - 1))
                    {
                        // SQLGetPrivateProfileString returns bufLen - 1 if it fills the buffer
                        // completely. In this case, loop and retry with a larger buffer
                        valBuf.Attach(new wchar_t[valBuf.GetLength() * 2], valBuf.GetLength() * 2);
                    }
                    else
                    {
                        break;
                    }
                }

                simba_wstring keyName(keyNamePtr);
                m_configuration[keyName] = simba_wstring(valBuf.Get());
                keyNamePtr += keyName.GetLength() + 1;
            }
        }
    }
#else
    UNUSED(in_attributeString);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::LoadConnectionSettings(
    const DSIConnSettingRequestMap& in_connectionSettings)
{
    ENTRANCE_LOG(m_logger, "Simba::Quickstart", "QSDSNConfiguration", "LoadConnectionSettings");

    m_configuration.clear();

    for (DSIConnSettingRequestMap::const_iterator itr = in_connectionSettings.begin();
         itr != in_connectionSettings.end();
         ++itr)
    {
        m_configuration.insert(ConfigurationMap::value_type(
            itr->first,
            itr->second.GetWStringValue()));
    }

    m_configType = CT_CONNECTION;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::LoadDefaults()
{
    m_logger->LogFunctionEntrance("Simba::Quickstart", "QSDSNConfiguration", "LoadDefaults");

    m_configuration[DSN_CONFIG_KEY_DRIVER] = m_driver;

    // Load the proper logging properties even if this is new.
    LoadLoggingProperties();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::RetrieveConnectionSettings(
    DSIConnSettingRequestMap& out_connectionSettings)
{
    ENTRANCE_LOG(m_logger, "Simba::Quickstart", "QSDSNConfiguration", "RetrieveConnectionSettings");

    out_connectionSettings.clear();

    for (ConfigurationMap::const_iterator itr = m_configuration.begin();
         itr != m_configuration.end();
         ++itr)
    {
        out_connectionSettings.insert(DSIConnSettingRequestMap::value_type(
            itr->first,
            itr->second));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::Save()
{
#ifndef OLEDBTARGET
    m_logger->LogFunctionEntrance("Simba::Quickstart", "QSDSNConfiguration", "Save");

    // Add data source to the system information.
    // If the data source specification section already exists, SQLWriteDSNToIni removes the old
    // section before creating the new one.
    const wstring dsn = GetDSN().GetAsPlatformWString();
    const wstring driver = GetDriver().GetAsPlatformWString();

    if (SQLWriteDSNToIniW(dsn.c_str(), driver.c_str()))
    {
        // Write the configuration settings to odbc.ini.
        for (ConfigurationMap::iterator itr = m_configuration.begin();
             itr != m_configuration.end();
             ++itr)
        {
            if (itr->first.IsEqual(SETTING_LOGLEVEL, false) ||
                itr->first.IsEqual(SETTING_LOGPATH, false) ||
                itr->first.IsEqual(SETTING_LOGFILECOUNT, false) ||
                itr->first.IsEqual(SETTING_LOGFILESIZE, false) ||
                itr->first.IsEqual(DSN_CONFIG_KEY_DSN, false))
            {
                // Don't write the DSN, or logging settings.
                continue;
            }

            wstring key = itr->first.GetAsPlatformWString();
            if (key.empty())
            {
                // Remove this value from odbc.ini.
                SQLWritePrivateProfileStringW(dsn.c_str(), key.c_str(), NULL, L"ODBC.INI");

                continue;
            }

            wstring value = itr->second.GetAsPlatformWString();

            if (!SQLWritePrivateProfileStringW(
                    dsn.c_str(),
                    key.c_str(),
                    value.c_str(),
                    L"ODBC.INI"))
            {
                // An error occurred.
                throw QSSetupException(QSSetupException::CFG_DLG_ERROR_REQUEST_FAILED);
            }
        }

        // Write out the logging settings.
        ::SetDriverRegistryValue(SETTING_LOGLEVEL, GetLogLevel());
        ::SetDriverRegistryValue(SETTING_LOGPATH, GetLogPath());
        ::SetDriverRegistryValue(SETTING_LOGFILECOUNT, GetLogMaxNum());

        // Convert from MB to bytes.
        simba_uint32 size = NumberConverter::ConvertWStringToUInt32(GetLogMaxSize());
        ::SetDriverRegistryValue(
            SETTING_LOGFILESIZE,
            NumberConverter::ConvertToWString(size * BYTES_PER_MB));
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::SetDBF(const simba_wstring& in_dbf)
{
    m_configuration[QS_DBF_KEY] = in_dbf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::SetDescription(const simba_wstring& in_description)
{
    m_configuration[DESC_KEY] = in_description;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::SetDSN(const simba_wstring& in_dsn)
{
#ifndef OLEDBTARGET
    if ((CT_CONNECTION == m_configType) &&
        in_dsn.IsEmpty())
    {
        // Don't do any validation if only doing a connection dialog as the DSN doesn't matter.
        return;
    }

    wstring dsnWstr = in_dsn.GetAsPlatformWString();

    if (SQLValidDSNW(dsnWstr.c_str()))
    {
        m_configuration[DSN_CONFIG_KEY_DSN] = in_dsn;
    }
    else
    {
        throw QSSetupException(QSSetupException::CFG_DLG_ERROR_REQUEST_FAILED);
    }

#else
    UNUSED(in_dsn);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::SetLogLevel(const simba_wstring& in_level)
{
    m_configuration[SETTING_LOGLEVEL] = in_level;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::SetLogMaxNum(const simba_wstring& in_maxNum)
{
    try
    {
        if (!in_maxNum.IsEmpty())
        {
            NumberConverter::ConvertWStringToUInt32(in_maxNum);
        }

        m_configuration[SETTING_LOGFILECOUNT] = in_maxNum;
    }
    catch (ErrorException&)
    {
        throw QSSetupException(QSSetupException::CFG_DLG_ERROR_INVALID_MAX_LOG_NUM);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::SetLogMaxSize(const simba_wstring& in_maxSize)
{
    try
    {
        if (!in_maxSize.IsEmpty() &&
            (4096 < NumberConverter::ConvertWStringToUInt16(in_maxSize)))
        {
            // Restrict the size of the log files to 4096 MB, since the log rotation code will
            // simply clamp to this size anyway.
            throw QSSetupException(QSSetupException::CFG_DLG_ERROR_INVALID_MAX_LOG_SIZE);
        }

        m_configuration[SETTING_LOGFILESIZE] = in_maxSize;
    }
    catch (ErrorException&)
    {
        throw QSSetupException(QSSetupException::CFG_DLG_ERROR_INVALID_MAX_LOG_SIZE);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::SetLogPath(const simba_wstring& in_path)
{
    m_configuration[SETTING_LOGPATH] = in_path;
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::LoadLoggingProperties()
{
    m_logger->LogFunctionEntrance("Simba::Quickstart", "QSDSNConfiguration", "LoadLoggingProperties");

    // Get the logging level, and ensure it fits one of the expected values.
    simba_wstring logLevel = GetDriverRegistryValue(SETTING_LOGLEVEL, "0");
    logLevel = NumberConverter::ConvertInt32ToWString(
        Simba::DSI::DSIFileLogger::ConvertStringToLogLevel(logLevel.GetAsUTF8()));

    m_configuration[SETTING_LOGLEVEL] = logLevel;
    m_configuration[SETTING_LOGPATH] = GetDriverRegistryValue(SETTING_LOGPATH, "");
    m_configuration[SETTING_LOGFILECOUNT] = GetDriverRegistryValue(
        SETTING_LOGFILECOUNT,
        NumberConverter::ConvertToString(DSI_FILELOGGER_DEFAULT_MAXFILECOUNT).c_str());

    simba_wstring size = GetDriverRegistryValue(
        SETTING_LOGFILESIZE,
        NumberConverter::ConvertToString(DSI_FILELOGGER_DEFAULT_MAXFILESIZE).c_str());
    try
    {
        // Convert from bytes to MB.
        simba_uint32 intSize = NumberConverter::ConvertWStringToUInt32(size);
        size = NumberConverter::ConvertToWString(intSize / BYTES_PER_MB);
    }
    catch (...)
    {
        // Use the default if something goes wrong.
        size = NumberConverter::ConvertToWString(DSI_FILELOGGER_DEFAULT_MAXFILESIZE / BYTES_PER_MB);
    }

    m_configuration[SETTING_LOGFILESIZE] = size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDSNConfiguration::ParseDSNAttribute(const simba_wstring& in_attributeString)
{
    m_logger->LogFunctionEntrance("Simba::Quickstart", "QSDSNConfiguration", "ParseDSNAttribute");
    INFO_LOG(
        m_logger,
        "Simba::Quickstart",
        "QSDSNConfiguration",
        "ParseDSNAttribute",
        "%s",
        in_attributeString.GetAsUTF8().c_str());

    if (!in_attributeString.IsEmpty())
    {
        static const wstring pairDelimiter(L"\0", 1);
        static const wstring entryDelimiter = L"=";

        wstring attributeString = in_attributeString.GetAsPlatformWString();

        // Split the attribute string into key/value pairs, so each entry will have a single
        // key/value.
        simba_size_t beginPairSeparator = 0;
        simba_size_t endPairSeparator = attributeString.find(pairDelimiter);

        while (true)
        {
            wstring entry = attributeString.substr(beginPairSeparator, endPairSeparator);

            // Split this pair into a key and value.
            simba_size_t entrySeparator = entry.find(entryDelimiter);

            if (wstring::npos != entrySeparator)
            {
                wstring key = entry.substr(0, entrySeparator);
                if (key == DSN_CONFIG_KEY_DSN.GetAsPlatformWString())
                {
                    wstring dsn(entry.substr(entrySeparator + 1));
                    SetDSN(dsn.c_str());
                }
                else
                {
                    m_configuration[key] = simba_wstring(entry.substr(entrySeparator + 1));
                }
            }

            if (wstring::npos == endPairSeparator)
            {
                break;
            }

            // Advance to the next pair.
            beginPairSeparator = endPairSeparator + 1;
            endPairSeparator = attributeString.find(pairDelimiter, beginPairSeparator);
        }
    }
}
