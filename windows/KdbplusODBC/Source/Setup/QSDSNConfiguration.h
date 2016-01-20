//==================================================================================================
///  @file QSDSNConfiguration.h
///
///  Definition of the class QSDSNConfiguration.
///
///  Copyright (C) 2014-2015 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSDSNCONFIGURATION_H_
#define _SIMBA_QUICKSTART_QSDSNCONFIGURATION_H_

#include "Quickstart.h"
#include "IConnection.h" // For DSIConnSettingRequestMap

namespace Simba
{
namespace Quickstart
{
    static const simba_wstring s_emptyVal(L"");
    enum ConfigType
    {
        CT_NEW_DSN,
        CT_EXISTING_DSN,
        CT_CONNECTION
    };

    // Configuration Map
    typedef std::map<
        simba_wstring,
        simba_wstring,
        simba_wstring::CaseInsensitiveComparator> ConfigurationMap;

    /// @brief QSDSNConfiguration class encapsulates the logic of DSN entry creation and
    /// modification.
    class QSDSNConfiguration
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        ///
        /// @param in_driverDescription     Driver description. This is the entry of the driver in
        ///                                 'ODBCINST/ODBC Drivers'.
        /// @param in_logger                The logger to use for this object. (NOT OWN)
        QSDSNConfiguration(const simba_wstring& in_driverDescription, ILogger* in_logger);

        /// @brief Destructor.
        ~QSDSNConfiguration() {}

        /// @brief Get if the configuration is a new DSN, modifying an existing DSN, or a connection.
        ///
        /// @return The configuration type.
        ConfigType GetConfigType() const;

        /// @brief Gets the DBF of the DSN entry.
        ///
        /// @return The DBF value.
        const simba_wstring& GetDBF() const;

        /// @brief Gets the description of the DSN entry.
        ///
        /// @return The DSN description.
        const simba_wstring& GetDescription() const;

        /// @brief Get the Driver name.
        ///
        /// @return The configured Driver name.
        const simba_wstring& GetDriver() const;

        /// @brief Gets the name of the DSN entry.
        ///
        /// @return The DSN name.
        const simba_wstring& GetDSN() const;

        /// @brief Get the logger for the configuration object.
        ///
        /// @return The logger object. (NOT OWN)
        ILogger* GetLog()
        {
            return m_logger;
        }

        /// @brief Gets the logging level of the driver.
        ///
        /// @return The logging level.
        const simba_wstring& GetLogLevel() const;

        /// @brief Gets the maximum number of log files to use in rotation.
        ///
        /// @return The maximum number of log files to use in rotation.
        const simba_wstring& GetLogMaxNum() const;

        /// @brief Gets the maximum size in MB of log files for rotation.
        ///
        /// @return The maximum size in MB of log files for rotation.
        const simba_wstring& GetLogMaxSize() const;

        /// @brief Gets the logging path of the driver.
        ///
        /// @return The logging path.
        const simba_wstring& GetLogPath() const;

        /// @brief Loads the DSN entry specified by the arguments from the registry
        ///
        /// @param in_attributeString       List of attributes in the form of keyword-value pairs.
        ///                                 The list is passed into ConfigDSN. NOTE that the DSN
        ///                                 value is always expected to be in the list.
        void Load(const simba_wstring& in_attributeString);

        /// @brief Load the connection into the configuration.
        ///
        /// @param in_connectionSettings    The settings to load.
        void LoadConnectionSettings(
            const Simba::DSI::DSIConnSettingRequestMap& in_connectionSettings);

        /// @brief Loads the DSN defaults into the configuration object.
        void LoadDefaults();

        /// @brief Retrieve the configuration and load it into the settings.
        ///
        /// @param out_connectionSettings   The settings to load from the configuration.
        void RetrieveConnectionSettings(
            Simba::DSI::DSIConnSettingRequestMap& out_connectionSettings);

        /// @brief Saves the DSN entry into the registry.
        void Save();

        /// @brief Sets the DBF of the DSN entry.
        ///
        /// @param in_dbf                   New DBF value.
        void SetDBF(const simba_wstring& in_dbf);

        /// @brief Sets the name of the DSN entry.
        ///
        /// @param in_dsn                   New DSN name.
        void SetDSN(const simba_wstring& in_dsn);

        /// @brief Sets the description of the DSN entry being created/edited.
        ///
        /// @param in_description           New DESCRIPTION value.
        void SetDescription(const simba_wstring& in_description);

        /// @brief Sets the logging level of the driver
        ///
        /// @param in_level                 New logging level value.
        void SetLogLevel(const simba_wstring& in_level);

        /// @brief Sets the maximum number of log files to use in rotation.
        ///
        /// @param in_maxNum                The maximum number of log files to use in rotation.
        void SetLogMaxNum(const simba_wstring& in_maxNum);

        /// @brief Sets the maximum size in MB of log files for rotation.
        ///
        /// @param in_maxSize               The maximum size in MB of log files for rotation.
        void SetLogMaxSize(const simba_wstring& in_maxSize);

        /// @brief Sets the logging path of the driver
        ///
        /// @param in_path                  New logging path value.
        void SetLogPath(const simba_wstring& in_path);

    // Private =====================================================================================
    private:
        /// @brief Load the logging properties for the driver.
        void LoadLoggingProperties();

        /// @brief Parses attribute list in order to find the value of the DSN attribute.
        ///
        /// @param in_attributeString       List of attributes in the form of keyword-value pairs.
        void ParseDSNAttribute(const simba_wstring& in_attributeString);

        // The collection of configuration settings.
        ConfigurationMap m_configuration;

        // The name of the driver being configured. Note that this is not the path, it is the
        // name of the driver.
        simba_wstring m_driver;

        // The logger to use for this configuration object. (NOT OWN)
        ILogger* m_logger;

        // Enum specifying the type of configuration.
        ConfigType m_configType;
    };
}
}

#endif
