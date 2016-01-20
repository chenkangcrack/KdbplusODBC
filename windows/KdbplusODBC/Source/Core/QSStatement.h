//==================================================================================================
///  @file QSStatement.h
///
///  Definition of the Quickstart DSIStatement implementation class.
///
///  Copyright (C) 2009-2015 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSSTATEMENT_H_
#define _SIMBA_QUICKSTART_QSSTATEMENT_H_

#include "Quickstart.h"
#include "DSIStatement.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Quickstart DSIStatement implementation class.
    class QSStatement : public Simba::DSI::DSIStatement
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        ///
        /// Default statement property values and limits can be overridden here.
        ///
        /// @param in_connection            The parent connection. (NOT OWN)
        /// @param in_settings              The connection settings. (NOT OWN)
        QSStatement(
            Simba::DSI::IConnection* in_connection,
            QuickstartSettings* in_settings);

        /// @brief Destructor.
        ~QSStatement();

        /// @brief Constructs a new data engine instance.
        ///
        /// @return A new IDataEngine instance. (OWN)
        virtual Simba::DSI::IDataEngine* CreateDataEngine();

        /// @brief Retrieves a custom property value.
        ///
        /// @param in_key                   ODBC attribute key.
        ///
        /// @return Custom property value. (NOT OWN)
        ///
        /// @exception ErrorException if the custom property cannot be found.
        virtual AttributeData* GetCustomProperty(simba_int32 in_key);

        /// @brief Retrieves the type associated with the custom property with the given ODBC
        /// attribute key.
        ///
        /// @param in_key                   ODBC attribute key.
        ///
        /// @return Type associated with the custom property.
        ///
        /// @exception ErrorException if the custom property key cannot be found.
        virtual AttributeType GetCustomPropertyType(simba_int32 in_key);

        /// @brief Should return true if the given ODBC attribute key is associated with a custom
        /// statement property recognized by the DSII.
        ///
        /// @param in_key                   Statement property key supported by DSI.
        ///
        /// @return True if the in_key is associated with a custom statement property
        /// recognized by the DSII; false otherwise.
        virtual bool IsCustomProperty(simba_int32 in_key);

        /// @brief Sets the custom statement property with the given ODBC attribute key to the
        /// given value.
        ///
        /// If the value is modified before saving it, an OPT_VAL_CHANGED warning should be
        /// posted. This method should post an OPT_VAL_CHANGED warning if a value is substituted
        /// with another value before storage. For properties which accept more than just a discrete
        /// set of values, this method should throw an INVALID_ATTR_VAL error if an
        /// illegal value is given which can't be substituted with another value. For values that
        /// cannot take effect immediately but will take effect at a later time, this method should
        /// also post an OPT_VAL_CHANGED warning.
        ///
        /// @param in_key                   ODBC attribute key.
        /// @param in_value                 Custom statement property value to be set for in_key. (OWN)
        virtual void SetCustomProperty(simba_int32 in_key, AttributeData* in_value);

    // Private =====================================================================================
    private:
        /// @brief Initializes the map of custom statement property keys.
        ///
        /// @return The initialized custom statement property map.
        static QSCustomPropertyKeyMap InitializeCustomStmtPropertyKeys();

        // Struct containing connection settings and error codes. (NOT OWN)
        QuickstartSettings* m_settings;

        // Set containing the custom statement property keys.
        static QSCustomPropertyKeyMap s_customStmtPropertyKeys;

        // Map containing custom statement properties.
        QSCustomPropertyMap m_customStmtProperties;
    };
}
}

#endif
