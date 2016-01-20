//==================================================================================================
///  @file QSBaseDialog.cpp
///
///  Implementation of the class QSBaseDialog.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
//==================================================================================================

#include "QSBaseDialog.h"

#include "AutoArrayPtr.h"
#include "DSIMessageSource.h"
#include "NumberConverter.h"
#include "resource.h"
#include "QSResourceHelper.h"
#include "SimbaSettingReader.h"
#include "SimbaUtility_Windows.h"

#include <atlstr.h>

using namespace Simba::Quickstart;
using namespace std;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
void QSBaseDialog::ShowErrorDialog(HWND in_dialogHandle, const simba_wstring& in_errorMessage)
{
    CString title;
    QSResourceHelper::LoadStringResource(IDS_CONFIG_ERROR_TITLE, title);

    // Get the error message for the exception.
    wstring errMsg = in_errorMessage.GetAsPlatformWString();

    // Display the error message.
    MessageBox(
        in_dialogHandle,
        CString(errMsg.c_str()),
        title,
        (MB_ICONEXCLAMATION | MB_OK));
}

// Protected =======================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
void QSBaseDialog::CenterDialog(HWND in_dialogHandle)
{
    // Get the parent window, or the desktop if there is no parent.
    HWND parentWindowHandle = GetParent(in_dialogHandle);

    if (NULL == parentWindowHandle)
    {
        parentWindowHandle = GetDesktopWindow();
    }

    RECT parentRect;
    RECT dialogRect;

    // Get the rectangles for the parent and the dialog.
    GetWindowRect(parentWindowHandle, &parentRect);
    GetWindowRect(in_dialogHandle, &dialogRect);

    // Get the height and width of the dialog.
    simba_int32 width = dialogRect.right - dialogRect.left;
    simba_int32 height = dialogRect.bottom - dialogRect.top;

    // Determine the top left point for the new centered position of the dialog.
    // The computations are a bit odd to avoid negative numbers, which would result in overflow.
    simba_int32 leftPoint = 
        ((parentRect.left * 2) + (parentRect.right - parentRect.left) - width) / 2;

    simba_int32 topPoint = 
        ((parentRect.top * 2) + (parentRect.bottom - parentRect.top) - height) / 2;

    // Ensure that the dialog stays on the screen.
    RECT desktopRect;

    GetWindowRect(GetDesktopWindow(), &desktopRect);

    // Horizontal adjustment.
    if (desktopRect.left > leftPoint)
    {
        leftPoint = desktopRect.left;
    }
    else if (desktopRect.right < (leftPoint + width))
    {
        leftPoint = desktopRect.right - width;
    }

    // Vertical adjustment.
    if (desktopRect.top > topPoint)
    {
        topPoint = desktopRect.top;
    }
    else if (desktopRect.bottom < (topPoint + height))
    {
        topPoint = desktopRect.top - height;
    }

    // Place the dialog.
    MoveWindow(in_dialogHandle, leftPoint, topPoint, width, height, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSBaseDialog::GetListText(
    HWND in_dialogHandle,
    simba_int32 in_component,
    simba_uint32 in_index)
{
    // Get the length of the selection.
    LRESULT textLength = SendDlgItemMessage(
        in_dialogHandle, 
        in_component, 
        LB_GETTEXTLEN, 
        in_index, 
        0);
    if (CB_ERR == textLength)
    {
        // The index is invalid.
        return L"";
    }

    // Allocate a buffer for the selection.
    AutoArrayPtr<wchar_t> buffer(textLength + 1);

    // Retrieve the selected text.
    SendDlgItemMessage(
        in_dialogHandle, 
        in_component, 
        LB_GETTEXT, 
        in_index, 
        reinterpret_cast<LPARAM>(buffer.Get()));

    return simba_wstring(buffer.Get());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSBaseDialog::GetEditText(HWND in_dialogHandle, simba_int32 in_component)
{
    // Get the length of the selection.
    LRESULT textLength = SendDlgItemMessage(
        in_dialogHandle, 
        in_component, 
        WM_GETTEXTLENGTH, 
        0, 
        0);
    if (CB_ERR == textLength)
    {
        // An error happened, so just return an empty string.
        return L"";
    }

    // Allocate a buffer for the selection.
    AutoArrayPtr<wchar_t> buffer(textLength + 1);

    // Retrieve the selected text.
    GetDlgItemText(
        in_dialogHandle, 
        in_component, 
        buffer.Get(),
        static_cast<simba_int32>(textLength) + 1);

    // Trim any excess whitespace.
    return simba_wstring(buffer.Get()).Trim();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSBaseDialog::GetErrorMessage(ErrorException& in_exception)
{
    // Load a message source to read the messages from.
    Simba::DSI::DSIMessageSource messageSource;
    messageSource.SetVendorName(DRIVER_VENDOR);
    messageSource.RegisterMessages(ERROR_MESSAGES_FILE, QS_ERROR);

    // Read and return the exception message.
    return in_exception.GetMessageText(&messageSource, SimbaSettingReader::GetDriverLocale());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSBaseDialog::GetSelectedComboText(
    HWND in_dialogHandle, 
    simba_int32 in_component)
{
    // Get the selection at the current index in the dropdown.
    LRESULT index = SendDlgItemMessage(in_dialogHandle, in_component, CB_GETCURSEL, 0, 0);
    if (CB_ERR == index)
    {
        // There is nothing selected, which should never happen.
        return "";
    }

    // Get the length of the selection.
    LRESULT textLength = SendDlgItemMessage(
        in_dialogHandle, 
        in_component, 
        CB_GETLBTEXTLEN, 
        index, 
        0);
    if (CB_ERR == textLength)
    {
        // The index is invalid, which should also never happen.
        return "";
    }

    // Allocate a buffer for the selection.
    AutoArrayPtr<wchar_t> buffer(textLength + 1);

    // Retrieve the selected text.
    SendDlgItemMessage(
        in_dialogHandle, 
        in_component, 
        CB_GETLBTEXT, 
        index, 
        reinterpret_cast<LPARAM>(buffer.Get()));

    return simba_wstring(buffer.Get()).Trim();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSBaseDialog::SetCursorArrow()
{
    HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
    if (!cursor)
    {
        // Failed to load the cursor.
        QSTHROWGEN1(L"QSCursorResourceError", GetLastErrorText());
    }

    SetCursor(cursor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSBaseDialog::SetCursorHourglass()
{
    HCURSOR cursor = LoadCursor(NULL, IDC_WAIT);
    if (!cursor)
    {
        // Failed to load the cursor.
        QSTHROWGEN1(L"QSCursorResourceError", GetLastErrorText());
    }

    SetCursor(cursor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSBaseDialog::SetEditText(
    HWND in_dialogHandle, 
    simba_int32 in_component, 
    const simba_wstring& in_text)
{
    // Safely set the dialog item text.
    std::wstring dbfStr = in_text.GetAsPlatformWString();
    SetDlgItemText(in_dialogHandle, in_component, dbfStr.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSBaseDialog::SetIcon(HWND in_dialogHandle, simba_int32 in_component)
{
    HANDLE hIcon = LoadImage(
        QSResourceHelper::GetModuleInstance(),
        MAKEINTRESOURCE(in_component),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        0);

    if (hIcon)
    {
        SendMessage(in_dialogHandle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSBaseDialog::ShowChoiceDialog(
    HWND in_dialogHandle, 
    const simba_wstring& in_title, 
    const simba_wstring& in_message,
    simba_uint32 in_icon,
    bool in_showYesNo)
{
    // Get the title and message for the dialog.
    wstring title = in_title.GetAsPlatformWString();
    wstring message = in_message.GetAsPlatformWString();

    // Display the error message.
    if (in_showYesNo)
    {
        return (IDYES == MessageBox(
            in_dialogHandle,
            CString(message.c_str()),
            CString(title.c_str()),
            (in_icon| MB_YESNO)));
    }

    return (IDOK == MessageBox(
        in_dialogHandle,
        CString(message.c_str()),
        CString(title.c_str()),
        (in_icon| MB_OKCANCEL)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSBaseDialog::ShowErrorDialog(HWND in_dialogHandle, ErrorException& in_exception)
{
    ShowErrorDialog(in_dialogHandle, GetErrorMessage(in_exception));
}
