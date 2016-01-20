//==================================================================================================
///  @file QSDriver.h
///
///  Definition of the Quickstart DSIDriver implementation class.
///
///  Copyright (C) 2009-2014 Simba Technologies Incorporated.
// =================================================================================================

#ifndef _SIMBA_QUICKSTART_QSDRIVER_H_
#define _SIMBA_QUICKSTART_QSDRIVER_H_

#include "Quickstart.h"

#include "DSIDriver.h"
#include "AutoPtr.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Quickstart DSIDriver implementation class.
    ///
    /// This driver exposes data stored within Tabbed Unicode Text Files to applications making use
    /// of the ODBC 3.80 driver.
    class QSDriver : public Simba::DSI::DSIDriver
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        QSDriver();

        /// @brief Destructor.
        virtual ~QSDriver();

        /// @brief Factory method for creating IEnvironments.
        ///
        /// @return The newly created IEnvironment. (OWN)
        virtual Simba::DSI::IEnvironment* CreateEnvironment();

        /// @brief Gets the driver-wide logging interface.
        ///
        /// @return The driver-wide logging interface. (NOT OWN)
        virtual ILogger* GetDriverLog();

    // Private =====================================================================================
    private:
        /// @brief Overrides some of the default driver properties as defined in 
        /// DSIDriverProperties.h.
        void SetDriverPropertyValues();

        // Driver-wide ILogger. (OWN)
        AutoPtr<ILogger> m_driverLog;
    };
}
}

#endif
