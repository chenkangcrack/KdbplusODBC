//==================================================================================================
///  @file QSEnvironment.h
///
///  Definition of the Quickstart DSIEnvironment implementation class.
///
///  Copyright (C) 2009-2014 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSENVIRONMENT_H_
#define _SIMBA_QUICKSTART_QSENVIRONMENT_H_

#include "Quickstart.h"
#include "DSIEnvironment.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Quickstart DSIEnvironment implementation class.
    class QSEnvironment : public Simba::DSI::DSIEnvironment
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        ///
        /// @param in_driver        The parent IDriver. (NOT OWN)
        QSEnvironment(Simba::DSI::IDriver* in_driver);

        /// @brief Destructor.
        ~QSEnvironment();

        /// @brief Creates and returns a new IConnection instance.
        ///
        /// @return New IConnection instance. (OWN)
        Simba::DSI::IConnection* CreateConnection();
    };
}
}

#endif
