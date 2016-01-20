//==================================================================================================
///  @file QSLoggingDialog.cpp
///
///  Implementation of the class QSLoggingDialog.
///
///  Copyright (C) 2014-2015 Simba Technologies Incorporated.
//==================================================================================================

#include "QSLoggingDialog.h"

#include "DSIFileLogger.h"
#include "NumberConverter.h"
#include "QSDialogConsts.h"
#include "QSResourceHelper.h"
#include "QSSetupException.h"
#include "resource.h"

#include <atlstr.h>
#include <shlobj.h>

using namespace Simba::Quickstart;
using namespace std;

// Static ==========================================================================================
const wchar_t* LOG_LEVELS[] =
{
    L"LOG_OFF",
    L"LOG_FATAL",
    L"LOG_ERROR",
    L"LOG_WARNING",
    L"LOG_INFO",
    L"LOG_DEBUG",
    L"LOG_TRACE"
};

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSLoggingDialog::Show(HWND in_parentWindow, QSDSNConfiguration& in_configSettings)
{
    in_configSettings.GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSLoggingDialog", "Show");

    // Use the windows functions to show a dialog box which returns true if the user OK'ed it,
    // false otherwise.

    // Show the dialog box and get if the user pressed OK or cancel.
    // MAINTENANCE NOTE: "false !=" is added to avoid compiler warning:
    // warning C4800: 'INT_PTR' : forcing value to bool 'true' or 'false' (performance warning)
    return (false != DialogBoxParam(
        QSResourceHelper::GetModuleInstance(),
        MAKEINTRESOURCE(IDD_DIALOG_LOGGING),
        in_parentWindow,
        reinterpret_cast<DLGPROC>(ActionProc),
        reinterpret_cast<LPARAM>(&in_configSettings)));
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
INT_PTR QSLoggingDialog::ActionProc(
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

                    case IDCANCEL:
                    {
                        // Cancel button pressed.
                        DoCancelAction(hwndDlg);
                        break;
                    }

                    case IDC_BUTTON_BROWSE:
                    {
                        DoBrowseAction(hwndDlg);
                        break;
                    }

                    case IDC_EDIT_LOGPATH:
                    case IDC_EDIT_LOGSIZE:
                    case IDC_EDIT_LOGNUM:
                    {
                        // Only changes to required fields will be checked for disabling/enabling of
                        // OK button.
                        if (EN_CHANGE == HIWORD(wParam))
                        {
                            // Enable/Disable the OK button if required fields are filled/empty.
                            CheckEnableButtons(hwndDlg);
                        }

                        break;
                    }

                    case IDC_COMBO_LOGLEVEL:
                    {
                        if (CBN_SELCHANGE == HIWORD(wParam))
                        {
                            // If the logging level combo box has changed, check if the logging
                            // options should be enabled or disabled.
                            CheckEnableLoggingOptions(hwndDlg);
                        }

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
            "QSLoggingDialog",
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
            "QSLoggingDialog",
            "ActionProc",
            e);
        ShowErrorDialog(hwndDlg, e);
    }

    return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSLoggingDialog::CheckEnableButtons(HWND in_dialogHandle)
{
    bool isLoggingEnabled = (0 != SendDlgItemMessage(
        in_dialogHandle,
        IDC_COMBO_LOGLEVEL,
        CB_GETCURSEL,
        0,
        0));
    bool enableOK = true;

    if (isLoggingEnabled)
    {
        enableOK = !IsEditEmpty(in_dialogHandle, IDC_EDIT_LOGPATH);
        enableOK = enableOK && !IsEditEmpty(in_dialogHandle, IDC_EDIT_LOGSIZE);
        enableOK = enableOK && !IsEditEmpty(in_dialogHandle, IDC_EDIT_LOGNUM);
    }

    // If any required fields are empty, then disable the OK button.
    EnableWindow(GetDlgItem(in_dialogHandle, IDOK), enableOK);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSLoggingDialog::CheckEnableLoggingOptions(HWND in_dialogHandle)
{
    // Disable or enable the logging fields depending on the logging level. If the level is LOG_OFF,
    // disable the other options.
    bool isLoggingEnabled = (0 != SendDlgItemMessage(
        in_dialogHandle,
        IDC_COMBO_LOGLEVEL,
        CB_GETCURSEL,
        0,
        0));

    // Get the components and set the enabled status of each.
    EnableWindow(GetDlgItem(in_dialogHandle, IDC_EDIT_LOGPATH), isLoggingEnabled);
    EnableWindow(GetDlgItem(in_dialogHandle, IDC_BUTTON_BROWSE), isLoggingEnabled);
    EnableWindow(GetDlgItem(in_dialogHandle, IDC_EDIT_LOGNUM), isLoggingEnabled);
    EnableWindow(GetDlgItem(in_dialogHandle, IDC_EDIT_LOGSIZE), isLoggingEnabled);

    CheckEnableButtons(in_dialogHandle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSLoggingDialog::DoBrowseAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSLoggingDialog", "DoBrowseAction");
    BROWSEINFO info = { 0 };

    simba_wstring title(QSResourceHelper::LoadStringResource(IDS_BROWSE_LOG_TITLE));
    wstring titleWstr = title.GetAsPlatformWString();
    info.lpszTitle = const_cast<LPWSTR>(titleWstr.c_str());

    LPITEMIDLIST pidl = SHBrowseForFolder(&info);

    if (NULL != pidl)
    {
        // Get the name of the folder.
        WCHAR path[MAX_PATH];

        // NOTE that SHGetPathFromIDList fails for objects other than folders.
        if (SHGetPathFromIDList(pidl, path))
        {
            SetEditText(in_dialogHandle, IDC_EDIT_LOGPATH, path);
        }

        // Free memory used.
        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSLoggingDialog::DoCancelAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSLoggingDialog", "DoCancelAction");

    EndDialog(in_dialogHandle, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSLoggingDialog::DoOkAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSLoggingDialog", "DoOkAction");

    // Record the logging level
    LRESULT index = SendDlgItemMessage(
        in_dialogHandle,
        IDC_COMBO_LOGLEVEL,
        CB_GETCURSEL,
        0,
        0);

    config->SetLogLevel(
        NumberConverter::ConvertInt32ToWString(static_cast<simba_int32>(index + LOG_MIN)));
    config->SetLogPath(GetEditText(in_dialogHandle, IDC_EDIT_LOGPATH));
    config->SetLogMaxSize(GetEditText(in_dialogHandle, IDC_EDIT_LOGSIZE));
    config->SetLogMaxNum(GetEditText(in_dialogHandle, IDC_EDIT_LOGNUM));

    EndDialog(in_dialogHandle, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSLoggingDialog::Initialize(HWND in_dialogHandle, LPARAM in_lParam)
{
    // Store handle to connect information.
    QSDSNConfiguration* settings = reinterpret_cast<QSDSNConfiguration*>(in_lParam);
    assert(settings);
    SetProp(in_dialogHandle, CONF_PROP, reinterpret_cast<HANDLE>(settings));

    settings->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSLoggingDialog", "Initialize");

    QSBaseDialog::CenterDialog(in_dialogHandle);
    QSBaseDialog::SetIcon(in_dialogHandle, IDI_ICON1);
    InitializeText(in_dialogHandle);

    // Clear the contents of the log level combo-box.
    SendDlgItemMessage(
        in_dialogHandle,
        IDC_COMBO_LOGLEVEL,
        CB_RESETCONTENT,
        0,
        0);

    // Fill in the log level combo-box.
    for (simba_int32 level = LOG_MIN; LOG_MAX >= level; ++level)
    {
        SendDlgItemMessage(
            in_dialogHandle,
            IDC_COMBO_LOGLEVEL,
            CB_ADDSTRING,
            0,
            reinterpret_cast<LPARAM>(LOG_LEVELS[level - LOG_MIN]));
    }

    // Set the selection to the correct level.
    SendDlgItemMessage(
        in_dialogHandle,
        IDC_COMBO_LOGLEVEL,
        CB_SETCURSEL,
        Simba::DSI::DSIFileLogger::ConvertStringToLogLevel(settings->GetLogLevel()) - LOG_MIN,
        0);

    // Set the log file textbox.
    SetEditText(in_dialogHandle, IDC_EDIT_LOGPATH, settings->GetLogPath());
    SetEditText(in_dialogHandle, IDC_EDIT_LOGNUM, settings->GetLogMaxNum());
    SetEditText(in_dialogHandle, IDC_EDIT_LOGSIZE, settings->GetLogMaxSize());

    CheckEnableLoggingOptions(in_dialogHandle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSLoggingDialog::InitializeText(HWND in_dialogHandle)
{
    CString string;
    QSResourceHelper::LoadStringResource(IDS_LOGGING_TITLE, string);
    SetWindowText(in_dialogHandle, string);

    QSResourceHelper::LoadStringResource(IDS_STATIC_LOGLEVEL, string);
    SetDlgItemText(in_dialogHandle, IDC_STATIC_LOGLEVEL, string);

    QSResourceHelper::LoadStringResource(IDS_STATIC_LOGPATH, string);
    SetDlgItemText(in_dialogHandle, IDC_STATIC_LOGPATH, string);

    QSResourceHelper::LoadStringResource(IDS_BUTTON_BROWSE, string);
    SetDlgItemText(in_dialogHandle, IDC_BUTTON_BROWSE, string);

    QSResourceHelper::LoadStringResource(IDS_STATIC_ROTATION, string);
    SetDlgItemText(in_dialogHandle, IDC_STATIC_ROTATION, string);

    QSResourceHelper::LoadStringResource(IDS_STATIC_LOGNUM, string);
    SetDlgItemText(in_dialogHandle, IDC_STATIC_LOGNUM, string);

    QSResourceHelper::LoadStringResource(IDS_STATIC_LOGSIZE, string);
    SetDlgItemText(in_dialogHandle, IDC_STATIC_LOGSIZE, string);

    QSResourceHelper::LoadStringResource(IDS_BUTTON_OK, string);
    SetDlgItemText(in_dialogHandle, IDOK, string);

    QSResourceHelper::LoadStringResource(IDS_BUTTON_CANCEL, string);
    SetDlgItemText(in_dialogHandle, IDCANCEL, string);
}
