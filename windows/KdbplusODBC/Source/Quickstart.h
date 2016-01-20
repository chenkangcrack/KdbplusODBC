//==================================================================================================
///  @file Quickstart.h
///
///  Quickstart constants and macro definitions.
///
///  Copyright (C) 2009-2014 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QUICKSTART_H_
#define _SIMBA_QUICKSTART_QUICKSTART_H_

#include "Simba.h"

#include "ILogger.h"

#include "AttributeData.h"
#include "DiagState.h"
#include "ErrorException.h"
#include "SQLState.h"

#include <map>

namespace Simba
{
namespace Quickstart
{
    /// Quickstart table data file suffix.
    #define QS_DATAFILE_SUFFIX simba_string(".txt")

    /// Filter for Quickstart table data files.
    #define QS_DATAFILE_FILTER simba_wstring(L"*.txt")

    /// Quickstart metadata file.
    #define QS_METADATAFILE_SUFFIX simba_wstring(L".xml")

    /// Component identifier for Quickstart errors.
    static const simba_int32 QS_ERROR = 100;

    // The error messages file to use.
    #define ERROR_MESSAGES_FILE "QSMessages"

    // The Windows branding for the driver.
    #define DRIVER_WINDOWS_BRANDING "AquaQ\\KdbplusODBC"

    // The Linux branding for the driver.
    #define DRIVER_LINUX_BRANDING "simba.quickstart.ini"

    // The Linux branding for the server.
    #define SERVER_LINUX_BRANDING "simbaserver.quickstart.ini"

    // The driver vendor name.
    #define DRIVER_VENDOR "Simba"

    /// The connection key to use when looking up the DBF path in the connection string.
    #define QS_DBF_KEY simba_wstring(L"DBF")

    /// The catalog name of all tables.
    #define QS_CATALOG simba_wstring(L"DBF")

    /// The connection key to use when looking up the Description in the connection string.
    #define QS_DESC_KEY simba_wstring(L"DESCRIPTION")

    #define QS_DEFAULT_MAX_COLUMN_SIZE_KEY simba_wstring(L"DEFAULTMAXCOLUMNSIZE")

    /// The connection key to use when enabling table caching from the connection string.
    #define QS_USE_CACHING_KEY simba_wstring(L"ENABLECACHING")

    /// If DefaultMaxColumnSize is not specified use this.
    static const simba_uint32 QS_MAX_STRING_COLUMN_SIZE = 255;

    /// The connection key for the 'Use Custom SQL States' flag in the connection string.
    #define QS_USE_CUSTOM_STATES_KEY simba_wstring(L"USECUSTOMSQLSTATES")

    /// The custom SQL state for errors/warning originating from QSDataEngine.
    #define QS_DATAENGINE_STATE SQLState("QS001")

    /// The custom SQL state for errors/warning originating from QSTable.
    #define QS_TABLE_STATE SQLState("QS002")

    /// The connection key to use when looking up Locale in the connection string.
    #define QS_LOCALE simba_wstring(L"Locale")

    /// OS file system path separator.
#if defined(WIN32) || defined(_WIN64)
    #define QS_PATH_SEPARATOR simba_string("\\")
#else
    #define QS_PATH_SEPARATOR simba_string("/")
#endif

    /// Custom property example for the POINTER attribute type.
    static const simba_int32 QS_CUSTOM_PROP_POINTER = 30000;

    /// Custom property example for the WSTRING attribute type.
    static const simba_int32 QS_CUSTOM_PROP_WSTRING = 30002;

    /// Custom property example for the INT32 attribute type.
    static const simba_int32 QS_CUSTOM_PROP_INT32 = 30003;

    /// Custom property example for the UINT32 attribute type.
    static const simba_int32 QS_CUSTOM_PROP_UINT32 = 30004;

    /// Custom property example for the INT16 attribute type.
    static const simba_int32 QS_CUSTOM_PROP_INT16 = 30005;

    /// Custom property example for the UINT16 attribute type.
    static const simba_int32 QS_CUSTOM_PROP_UINT16 = 30006;

    /// @brief Struct to contain both the Quickstart settings and the DBF path to the datafiles.
    struct QuickstartSettings
    {
        /// The DBF path where the Quickstart database files are located.
        /// Specified in the connection string. 
        simba_wstring m_dbfPath;    

        /// This is set to the DefaultMaxColumnSize registry entry and is used to specify
        /// the largest size of the columns in the file in the event that there is no metadata
        /// specified about the file.
        simba_uint32 defaultMaxColumnSize;

        /// Indicates that the driver should use custom SQL states.  This is for demonstration 
        /// purposes only.
        bool m_useCustomSQLStates;

        /// Flag indicating table caching should be used. This is for demonstration purposes only,
        /// it does not benefit Quickstart as Quickstart reads data from a local data source. Table
        /// caching is meant for use when reading data remotely when the DSII does not do caching,
        /// as it allows for a table to be fetched at most once per query even when doing joins 
        /// which may require multiple passes over the data.
        bool m_useTableCaching;
    };

    /// Set type to store custom property keys in QSConnection and QSStatement.
    typedef std::map<simba_int32, Simba::Support::AttributeType> QSCustomPropertyKeyMap;

    /// Map type to store custom properties in QSConnection and QSStatement.
    typedef std::map<simba_int32, Simba::Support::AttributeData*> QSCustomPropertyMap;
}   
}   


////////////////////////////////////////////////////////////////////////////////////////////////////
/// MACRO Definition
////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macros to facilitate exception throwing in Quickstart DSII.
// Throw an ErrorException with state key DIAG_GENERAL_ERROR, component id QS_ERROR, and the given
// message id.  Row number and column number may also be specified.
#define QSTHROWGEN(...)                                                                 \
{                                                                                       \
    throw Simba::Support::ErrorException(Simba::Support::DIAG_GENERAL_ERROR, QS_ERROR, __VA_ARGS__); \
}

// Throw an ErrorException with state key DIAG_GENERAL_ERROR, component id QS_ERROR, and the given
// message id and message parameter.
#define QSTHROWGEN1(id, param)                                                          \
{                                                                                       \
    std::vector<simba_wstring> msgParams;                                               \
    msgParams.push_back(param);                                                         \
    throw Simba::Support::ErrorException(Simba::Support::DIAG_GENERAL_ERROR, QS_ERROR, id, msgParams);\
}

// Throw an ErrorException with state key DIAG_GENERAL_ERROR, component id QS_ERROR, and the given
// message id and message parameters.
#define QSTHROWGEN2(id, param1, param2)                                                 \
{                                                                                       \
    std::vector<simba_wstring> msgParams;                                               \
    msgParams.push_back(param1);                                                        \
    msgParams.push_back(param2);                                                        \
    throw Simba::Support::ErrorException(Simba::Support::DIAG_GENERAL_ERROR, QS_ERROR, id, msgParams);\
}

// Throw an ErrorException with state key DIAG_GENERAL_ERROR, component id QS_ERROR, and the given
// message id and message parameters.
#define QSTHROWGEN3(id, param1, param2, param3)                                         \
{                                                                                       \
    std::vector<simba_wstring> msgParams;                                               \
    msgParams.push_back(param1);                                                        \
    msgParams.push_back(param2);                                                        \
    msgParams.push_back(param3);                                                        \
    throw Simba::Support::ErrorException(Simba::Support::DIAG_GENERAL_ERROR, QS_ERROR, id, msgParams);\
}


// Throw an ErrorException with the given state key/custom state and component id QS_ERROR, with the
// given message id.
#define QSTHROW(key, id)                                                                \
{                                                                                       \
    throw Simba::Support::ErrorException(key, QS_ERROR, id);                            \
}

// Throw an ErrorException with the given state key/custom state, component id QS_ERROR, the given
// message id and the given message parameter.
#define QSTHROW1(key, id, param)                                                        \
{                                                                                       \
    std::vector<simba_wstring> msgParams;                                               \
    msgParams.push_back(param);                                                         \
    throw Simba::Support::ErrorException(key, QS_ERROR, id, msgParams);                 \
}

// Throw an ErrorException with the given state key/custom state, component id QS_ERROR, the given
// message id and the two given message parameters.
#define QSTHROW2(key, id, param1, param2)                                               \
{                                                                                       \
    std::vector<simba_wstring> msgParams;                                               \
    msgParams.push_back(param1);                                                        \
    msgParams.push_back(param2);                                                        \
    throw Simba::Support::ErrorException(key, QS_ERROR, id, msgParams);                 \
}


#endif
