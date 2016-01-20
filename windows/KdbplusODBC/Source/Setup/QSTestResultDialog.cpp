//==================================================================================================
///  @file QSTestResultDialog.cpp
///
///  Implementation of the class QSTestResultDialog.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
//==================================================================================================

#include "QSTestResultDialog.h"

#include "DSIDriverSingleton.h"
#include "IConnection.h"
#include "IDriver.h"
#include "IEnvironment.h"
#include "resource.h"
#include "QSDialogConsts.h"
#include "QSResourceHelper.h"
#include "QSSetupException.h"

#include <atlstr.h>

using namespace Simba::DSI;
using namespace Simba::Quickstart;
using namespace std; 

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSTestResultDialog::Show(HWND in_parentWindow, QSDSNConfiguration& in_configSettings)
{
    in_configSettings.GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSTestResultDialog", "Show");

    // Use the windows functions to show a dialog box which returns true if the user OK'ed it, 
    // false otherwise.

    // Show the dialog box and get if the user pressed OK or cancel.
    // MAINTENANCE NOTE: "false !=" is added to avoid compiler warning:
    // warning C4800: 'INT_PTR' : forcing value to bool 'true' or 'false' (performance warning)
    return (false != DialogBoxParam(
        QSResourceHelper::GetModuleInstance(),
        MAKEINTRESOURCE(IDD_DIALOG_TESTRESULT),
        in_parentWindow,
        reinterpret_cast<DLGPROC>(ActionProc),
        reinterpret_cast<LPARAM>(&in_configSettings)));
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
INT_PTR QSTestResultDialog::ActionProc(
    HWND hwndDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    INT_PTR returnValue = static_cast<INT_PTR>(true);

    try
    {
        // Switch through the different messages that can be sent to the dialog by Windows and take
        // the appropriate action.
        switch (message) 
        { 
            case WM_INITDIALOG:
            {
                Initialize(hwndDlg, lParam);
                break;
            }

            case WM_COMMAND:
            {
                // The user has done some action on the dialog box.
                switch (LOWORD(wParam))
                {
                    case IDOK:
                    {
                        // OK button pressed.
                        DoOkAction(hwndDlg);
                        break;
                    }

                    default:
                    {
                        // Unknown command.
                        returnValue = static_cast<INT_PTR>(false);
                        break;
                    }
                }
                break;
            }

            case WM_DESTROY:
            case WM_NCDESTROY:
            {
                // WM_DESTROY - Destroy the dialog box. No action needs to be taken.
                // WM_NCDESTROY - Sent after the dialog box has been destroyed.  No action needs to 
                // be taken.
                break;
            }

            default:
            {
                // Unrecognized message.
                returnValue = static_cast<INT_PTR>(false);
                break;
            }
        }
    }
    catch (QSSetupException& e)
    {
        const simba_wstring wErrMsg = e.GetErrorMsg();
        const simba_string errMsg = wErrMsg.GetAsUTF8();

        QSDSNConfiguration* config = GET_CONFIG(hwndDlg, QSDSNConfiguration);
        assert(config);
        config->GetLog()->LogError(
            "Simba::Quickstart",
            "QSTestResultDialog",
            "ActionProc",
            "%s",
            errMsg.c_str());
        ShowErrorDialog(hwndDlg, wErrMsg);
    }
    catch (ErrorException& e)
    {
        QSDSNConfiguration* config = GET_CONFIG(hwndDlg, QSDSNConfiguration);
        assert(config);
        config->GetLog()->LogError(
            "Simba::Quickstart",
            "QSTestResultDialog",
            "ActionProc",
            e);
        ShowErrorDialog(hwndDlg, e);
    }

    return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTestResultDialog::DoOkAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSTestResultDialog", "DoOkAction");

    EndDialog(in_dialogHandle, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTestResultDialog::Initialize(HWND in_dialogHandle, LPARAM in_lParam)
{
    QSDSNConfiguration* settings = reinterpret_cast<QSDSNConfiguration*>(in_lParam);
    assert(settings);
    SetProp(in_dialogHandle, CONF_PROP, reinterpret_cast<HANDLE>(settings));

    settings->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSTestResultDialog", "Initialize");

    QSBaseDialog::CenterDialog(in_dialogHandle);
    QSBaseDialog::SetIcon(in_dialogHandle, IDI_ICON1);
    InitializeText(in_dialogHandle);

    simba_wstring resultDetail;
    try
    {
        SetCursorHourglass();

        // Generate the connection settings.
        DSIConnSettingRequestMap connSettings;
        settings->RetrieveConnectionSettings(connSettings);

        if (LOG_INFO <= settings->GetLog()->GetLogLevel())
        {
            // Log the settings only if at an applicable logging level.
            simba_string settingString;
            for (DSIConnSettingRequestMap::const_iterator itr = connSettings.begin();
                 itr != connSettings.end();
                 ++itr)
            {
                settingString += itr->first.GetAsUTF8() + "=" + itr->second.GetStringValue() + ";";
            }

            settings->GetLog()->LogInfo(
                "Simba::Quickstart",
                "QSTestResultDialog",
                "Initialize",
                "Testing connection using settings: %s",
                settingString.c_str());
        }

        // Create the connection.
        AutoPtr<IEnvironment> env(DSIDriverSingleton::GetDSIDriver()->CreateEnvironment());
        AutoPtr<IConnection> conn(env->CreateConnection());

        // Don't bother checking the responseSettings, the driver should fail on connect if
        // a required setting isn't present.
        DSIConnSettingResponseMap responseSettings;
        conn->UpdateConnectionSettings(connSettings, responseSettings);
        conn->Connect(connSettings);
        conn->Disconnect();

        settings->GetLog()->LogInfo(
            "Simba::Quickstart", 
            "QSTestResultDialog", 
            "Initialize",
            "Test connected successfully.");

        resultDetail = QSResourceHelper::LoadStringResource(IDS_TEST_SUCCESS);
    }
    catch (ErrorException& e)
    {
        simba_wstring errorMsg = GetErrorMessage(e);
        settings->GetLog()->LogError(
            "Simba::Quickstart", 
            "QSTestResultDialog", 
            "Initialize", 
            "%s", 
            errorMsg.GetAsPlatformString().c_str());
        resultDetail = QSResourceHelper::LoadStringResource(IDS_TEST_FAILURE) + errorMsg;
    }

    SetCursorArrow();

    // Set the text boxes.
    SetEditText(in_dialogHandle, IDC_EDIT_TESTRESULT, resultDetail);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTestResultDialog::InitializeText(HWND in_dialogHandle)
{
    CString string;
    QSResourceHelper::LoadStringResource(IDS_TEST_RESULT_TITLE, string);
    SetWindowText(in_dialogHandle, string);

    QSResourceHelper::LoadStringResource(IDS_BUTTON_OK, string);
    SetDlgItemText(in_dialogHandle, IDOK, string);
}
