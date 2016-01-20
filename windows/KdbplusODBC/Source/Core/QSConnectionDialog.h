// =================================================================================================
///  @file QSConnectionDialog.h
///
///  DSIConnection Dialog implementation.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
// =================================================================================================

#ifndef _SIMBA_QUICKSTART_QSCONNECTIONDIALOG_H_
#define _SIMBA_QUICKSTART_QSCONNECTIONDIALOG_H_

#include "QSConnection.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Class encapsulating the functions to operate the connection dialog.
    class QSConnectionDialog
    {
    // Public Static ===============================================================================
    public:
        /// @brief Displays a dialog box that prompts the user for settings for this connection.
        ///
        /// The return value for this method indicates if the user completed the process by 
        /// clicking OK on the dialog box (return true), or if the user aborts the process by 
        /// clicking CANCEL on the dialog box (return false).
        /// 
        /// @param io_connectionSettings    The connection settings map updated with settings that
        ///                                 are still needed and were not supplied.
        /// @param in_logger                The logger to use for the dialog. (NOT OWN)
        /// @param in_parentWindow          Handle to the parent window to which this dialog
        ///                                 belongs.
        /// @param in_isRequired            Flag indicating if only required settings are enabled.
        /// 
        /// @return True if the user clicks OK on the dialog box; false if the user clicks CANCEL.
        static bool Prompt(
            Simba::DSI::DSIConnSettingRequestMap& io_connectionSettings,
            ILogger* in_logger,
            HWND in_parentWindow,
            bool in_isRequired);
    };
}
}

#endif
