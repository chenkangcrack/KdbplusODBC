//// =================================================================================================
///  @file QSConnectionDialog.cpp
///
///  DSIConnection Dialog implementation.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
// =================================================================================================

#include "QSConnectionDialog.h"

#include "QSDSNConfiguration.h"
#include "QSMainDialog.h"

using namespace Simba::DSI;
using namespace Simba::Quickstart;

// Public Static ===================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSConnectionDialog::Prompt(
    DSIConnSettingRequestMap& io_connectionSettings, 
    ILogger* in_logger,
    HWND in_parentWindow, 
    bool in_isRequired)
{
    // Set the appropriate dialog properties.
    QSMainDialog dialog;
    QSDSNConfiguration dsnConfig("", in_logger);
    dsnConfig.LoadConnectionSettings(io_connectionSettings);

    // Pop the dialog. False return code indicates the dialog was canceled.
    if (dialog.Show(in_parentWindow, dsnConfig, in_isRequired))
    {
        dsnConfig.RetrieveConnectionSettings(io_connectionSettings);
        return true;
    }

    return false;
}
