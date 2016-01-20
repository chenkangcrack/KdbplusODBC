//==================================================================================================
///  @file QSMainDialog.cpp
///
///  Implementation of the class QSMainDialog.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
//==================================================================================================

#include "QSMainDialog.h"

#include "NumberConverter.h"
#include "ProductVersion.h"
#include "QSDialogConsts.h"
#include "QSLoggingDialog.h"
#include "QSResourceHelper.h"
#include "QSSetupException.h"
#include "resource.h"

#include <shlobj.h>

using namespace Simba::Quickstart;
using namespace std; 

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSMainDialog::Show(
    HWND in_parentWindow, 
    QSDSNConfiguration& in_configSettings, 
    bool in_onlyRequired /* = false */)
{
    in_configSettings.GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSMainDialog", "Show");

    DialogSettings settings(in_configSettings, in_onlyRequired);

    // Use the windows functions to show a dialog box which returns true if the user OK'ed it, 
    // false otherwise.

    // Show the dialog box and get if the user pressed OK or cancel.
    // MAINTENANCE NOTE: "false !=" is added to avoid compiler warning:
    // warning C4800: 'INT_PTR' : forcing value to bool 'true' or 'false' (performance warning)
    return (false != DialogBoxParam(
        QSResourceHelper::GetModuleInstance(),
        MAKEINTRESOURCE(IDD_DIALOG_DSN_CONFIG),
        in_parentWindow,
        reinterpret_cast<DLGPROC>(ActionProc),
        reinterpret_cast<LPARAM>(&settings)));
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
INT_PTR QSMainDialog::ActionProc(
    HWND hwndDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    INT_PTR returnValue = static_cast<INT_PTR> (true);

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

                    case IDC_BUTTON_LOGGING:
                    {
                        DoLoggingDialogAction(hwndDlg);
                        break;
                    }

                    case IDC_BUTTON_TEST:
                    {
                        DoTestAction(hwndDlg);
                        break;
                    }

                    case IDC_BUTTON_BROWSE:
                    {
                        DoBrowseAction(hwndDlg);
                        break;
                    }

                    case IDC_EDIT_DSN:
                    case IDC_EDIT_DBF:
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

                    default:
                    {
                        // Unknown command.
                        returnValue = static_cast<INT_PTR> (false);
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
                returnValue = static_cast<INT_PTR> (false);
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
            "QSMainDialog",
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
            "QSMainDialog",
            "ActionProc",
            e);
        ShowErrorDialog(hwndDlg, e);
    }

    return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::CheckEnableButtons(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    bool enableOK = !IsEditEmpty(in_dialogHandle, IDC_EDIT_DBF);

    if (enableOK &&
        (CT_CONNECTION != config->GetConfigType()))
    {
        // We don't care about the DSN if only doing a connection dialog.
        enableOK = !IsEditEmpty(in_dialogHandle, IDC_EDIT_DSN);
    }

    // If any required fields are empty, then disable the OK and test button.
    EnableWindow(GetDlgItem(in_dialogHandle, IDOK), enableOK);
    EnableWindow(GetDlgItem(in_dialogHandle, IDC_BUTTON_TEST), enableOK);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::DoBrowseAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSMainDialog", "DoBrowseAction");
    BROWSEINFO info = { 0 };

    simba_wstring title(QSResourceHelper::LoadStringResource(IDS_BROWSE_TITLE));
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
            SetEditText(in_dialogHandle, IDC_EDIT_DBF, path);
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
void QSMainDialog::DoCancelAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSMainDialog", "DoCancelAction");

    EndDialog(in_dialogHandle, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::DoLoggingDialogAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSMainDialog", "DoLoggingDialogAction");

    QSLoggingDialog loggingDialog;
    loggingDialog.Show(in_dialogHandle, *config);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::DoOkAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSMainDialog", "DoOkAction");

    try
    {
        SaveSettings(in_dialogHandle);
        EndDialog(in_dialogHandle, true);
    }
    catch (QSSetupException& e)
    {
        const simba_wstring wErrMsg = e.GetErrorMsg();
        const simba_string errMsg = wErrMsg.GetAsUTF8();

        config->GetLog()->LogError(
            "Simba::Quickstart", 
            "QSMainDialog", 
            "DoOkAction",
            "%s",
            errMsg.c_str());
        ShowErrorDialog(in_dialogHandle, wErrMsg);
    }
    catch (ErrorException& e)
    {
        config->GetLog()->LogError(
            "Simba::Quickstart", 
            "QSMainDialog", 
            "DoOkAction",
            e);
        ShowErrorDialog(in_dialogHandle, e);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::DoTestAction(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSMainDialog", "DoTestAction");

    try
    {
        SaveSettings(in_dialogHandle);

        QSTestResultDialog testDialog;
        testDialog.Show(in_dialogHandle, *config);
    }
    catch (QSSetupException& e)
    {
        config->GetLog()->LogError(
            "Simba::Quickstart",
            "QSMainDialog",
            "DoTestAction",
            "%s",
            e.GetErrorMsg());
        QSBaseDialog::ShowErrorDialog(in_dialogHandle, e.GetErrorMsg());
    }
    catch (ErrorException& e)
    {
        config->GetLog()->LogError(
            "Simba::Quickstart",
            "QSMainDialog",
            "DoTestAction",
            e);
        ShowErrorDialog(in_dialogHandle, e);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::Initialize(HWND in_dialogHandle, LPARAM in_lParam)
{
    // Store handle to connect information.
    DialogSettings* dialogSettings = reinterpret_cast<DialogSettings*>(in_lParam);
    assert(dialogSettings);

    QSDSNConfiguration* settings = &dialogSettings->m_configSettings;
    SetProp(in_dialogHandle, CONF_PROP, reinterpret_cast<HANDLE>(settings));

    settings->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSMainDialog", "Initialize");

    QSBaseDialog::CenterDialog(in_dialogHandle);
    QSBaseDialog::SetIcon(in_dialogHandle, IDI_ICON1);
    InitializeText(in_dialogHandle, (CT_CONNECTION == settings->GetConfigType()));

    // Restrict the length of the DSN edit box.
    SendDlgItemMessage(
        in_dialogHandle,
        IDC_EDIT_DSN,
        EM_SETLIMITTEXT,
        QSDialogConsts::MaxDsnLength,
        0);

    // Restrict the length of the description edit box.
    SendDlgItemMessage(
        in_dialogHandle,
        IDC_EDIT_DESC,
        EM_SETLIMITTEXT,
        QSDialogConsts::MaxDescLength,
        0);

    // Restrict the length of the DBF edit box.
    SendDlgItemMessage(
        in_dialogHandle,
        IDC_EDIT_DBF,
        EM_SETLIMITTEXT,
        QSDialogConsts::MaxDbfLength,
        0);

    // Set the text boxes.
    SetEditText(in_dialogHandle, IDC_EDIT_DSN, settings->GetDSN());
    SetEditText(in_dialogHandle, IDC_EDIT_DESC, settings->GetDescription());
    SetEditText(in_dialogHandle, IDC_EDIT_DBF, settings->GetDBF());

    if (CT_EXISTING_DSN == settings->GetConfigType())
    {
        settings->GetLog()->LogTrace(
            "Simba::Quickstart",
            "QSMainDialog",
            "Initialize",
            "Modifying existing DSN.");

        // Disable editing the DSN name when not creating a new DSN.
        EnableWindow(GetDlgItem(in_dialogHandle, IDC_EDIT_DSN), FALSE);
    }
    else if (CT_NEW_DSN == settings->GetConfigType())
    {
        settings->GetLog()->LogTrace(
            "Simba::Quickstart",
            "QSMainDialog",
            "Initialize",
            "Creating a new DSN.");
    }
    else
    {
        settings->GetLog()->LogTrace(
            "Simba::Quickstart",
            "QSMainDialog",
            "Initialize",
            "Showing a connection dialog.");

        // Hide some of the configuration settings and disable others if this is a connection
        // dialog.
        EnableWindow(GetDlgItem(in_dialogHandle, IDC_EDIT_DSN), FALSE);
        EnableWindow(GetDlgItem(in_dialogHandle, IDC_EDIT_DESC), FALSE);
        ShowWindow(GetDlgItem(in_dialogHandle, IDC_BUTTON_TEST), SW_HIDE);
        ShowWindow(GetDlgItem(in_dialogHandle, IDC_BUTTON_LOGGING), SW_HIDE);

        if (dialogSettings->m_onlyRequired)
        {
            // No optional fields to disable for the Quickstart dialog.
        }
    }

    // Check to make sure that the required fields are filled.
    CheckEnableButtons(in_dialogHandle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::InitializeText(HWND in_dialogHandle, bool in_isConnDialog)
{
    CString string;

    if (in_isConnDialog)
    {
        QSResourceHelper::LoadStringResource(IDS_CONN_MAIN_TITLE, string);
    }
    else
    {
        QSResourceHelper::LoadStringResource(IDS_MAIN_TITLE, string);
    }
    SetWindowText(in_dialogHandle, string);

    QSResourceHelper::LoadStringResource(IDS_STATIC_DSN, string);
    SetDlgItemText(in_dialogHandle, IDC_STATIC_DSN, string);

    QSResourceHelper::LoadStringResource(IDS_STATIC_DESC, string);
    SetDlgItemText(in_dialogHandle, IDC_STATIC_DESC, string);

    QSResourceHelper::LoadStringResource(IDS_STATIC_DBF, string);
    SetDlgItemText(in_dialogHandle, IDC_STATIC_DBF, string);

    CString version("v");
    version += VER_PRODUCTVERSION_STR;
#if (1 == PLATFORM_IS_64_BIT)
    QSResourceHelper::LoadStringResource(IDS_STATIC_VERSION_64, string);
#else
    QSResourceHelper::LoadStringResource(IDS_STATIC_VERSION_32, string);
#endif
    version += string;
    SetDlgItemText(in_dialogHandle, IDC_STATIC_VERSION, version);

    QSResourceHelper::LoadStringResource(IDS_BUTTON_TEST, string);
    SetDlgItemText(in_dialogHandle, IDC_BUTTON_TEST, string);

    QSResourceHelper::LoadStringResource(IDS_BUTTON_OK, string);
    SetDlgItemText(in_dialogHandle, IDOK, string);

    QSResourceHelper::LoadStringResource(IDS_BUTTON_CANCEL, string);
    SetDlgItemText(in_dialogHandle, IDCANCEL, string);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::SaveSettings(HWND in_dialogHandle)
{
    QSDSNConfiguration* config = GET_CONFIG(in_dialogHandle, QSDSNConfiguration);
    assert(config);

    config->GetLog()->LogFunctionEntrance("Simba::Quickstart", "QSMainDialog", "SaveSettings");

    ValidateDbf(in_dialogHandle);

    config->SetDSN(GetEditText(in_dialogHandle, IDC_EDIT_DSN));
    config->SetDescription(GetEditText(in_dialogHandle, IDC_EDIT_DESC));
    config->SetDBF(GetEditText(in_dialogHandle, IDC_EDIT_DBF));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSMainDialog::ValidateDbf(HWND in_dialogHandle)
{
    simba_wstring dbf = GetEditText(in_dialogHandle, IDC_EDIT_DBF);
    if (dbf.IsEmpty())
    {
        throw QSSetupException(QSSetupException::CFG_DLG_ERROR_INVALID_DBF);
    }

    DWORD attrs = GetFileAttributes(dbf.GetAsPlatformWString().c_str());
    if ((INVALID_FILE_ATTRIBUTES == attrs) ||
        !(attrs & FILE_ATTRIBUTE_DIRECTORY))
    {
        // The given path is invalid or points to something other than a directory.
        throw QSSetupException(QSSetupException::CFG_DLG_ERROR_INVALID_DBF);
    }
}
