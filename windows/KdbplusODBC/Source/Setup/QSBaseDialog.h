//==================================================================================================
///  @file QSBaseDialog.h
///
///  Definition of the class QSBaseDialog.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSBASEDIALOG_H_
#define _SIMBA_QUICKSTART_QSBASEDIALOG_H_

#include "Quickstart.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Base class for installer dialogs to encapsulate common logic.
    class QSBaseDialog
    {
    // Public ======================================================================================
    public:
        /// @brief Shows an error dialog.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_errorMessage          The error message to display.
        static void ShowErrorDialog(HWND in_dialogHandle, const simba_wstring& in_errorMessage);

    // Protected ===================================================================================
    protected:
        /// @brief Constructor.
        QSBaseDialog() {}

        /// @brief Centers the dialog in the screen.
        ///
        /// @param in_dialogHandle          The handle to the dialog to be centered.
        static void CenterDialog(HWND in_dialogHandle);

        /// @brief Fetches the text at the given index from the indicated listbox component.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_component             The component identifier to get the text from.
        /// @param in_index                 The index in the listbox to get the text from.
        ///
        /// @return The selected text as a simba_wstring.
        static simba_wstring GetListText(
            HWND in_dialogHandle,
            simba_int32 in_component, 
            simba_uint32 in_index);

        /// @brief Fetches the trimmed text from an Edit component.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_component             The component identifier to get the text from.
        ///
        /// @return The trimmed edit text.
        static simba_wstring GetEditText(HWND in_dialogHandle, simba_int32 in_component);

        /// @brief Get the error message for the specified exception.
        ///
        /// @param in_exception             The exception to get the message for.
        ///
        /// @return The error message.
        static simba_wstring GetErrorMessage(ErrorException& in_exception);

        /// @brief Fetches the selected text from the indicated combo-box component.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_component             The component identifier to get the text from.
        ///
        /// @return The selected combo-box text.
        static simba_wstring GetSelectedComboText(HWND in_dialogHandle, simba_int32 in_component);

        /// @brief Determine if the edit component is empty.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_component             The component identifier to get the check for emptiness.
        /// 
        /// @return True if empty; false otherwise.
        static bool IsEditEmpty(HWND in_dialogHandle, simba_int32 in_component)
        {
            return GetEditText(in_dialogHandle, in_component).IsEmpty();
        }

        /// @brief Set the cursor to the arrow.
        static void SetCursorArrow();

        /// @brief Set the cursor to the hourglass.
        static void SetCursorHourglass();

        /// @brief Sets the trimmed text for an Edit component.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_component             The component identifier to set the text for.
        /// @param in_text                  The text to set.
        static void SetEditText(
            HWND in_dialogHandle, 
            simba_int32 in_component, 
            const simba_wstring& in_text);

        /// @brief Set the icon for the dialog.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_component             The icon identifier to use.
        static void SetIcon(HWND in_dialogHandle, simba_int32 in_componentId);

        /// @brief Shows a choice dialog.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_title                 The title to display.
        /// @param in_message               The message to display.
        /// @param in_icon                  The icon to show in the dialog.
        /// @param in_showYesNo             True to show Yes/No buttons, false for OK/Cancel.
        ///
        /// @return True if the user pressed OK; false otherwise.
        static bool ShowChoiceDialog(
            HWND in_dialogHandle, 
            const simba_wstring& in_title, 
            const simba_wstring& in_message,
            simba_uint32 in_icon = MB_ICONQUESTION,
            bool in_showYesNo = false);

        /// @brief Shows an error dialog.
        ///
        /// @param in_dialogHandle          The handle to the dialog.
        /// @param in_exception             The exception to display.
        static void ShowErrorDialog(HWND in_dialogHandle, ErrorException& in_exception);
    };
}
}

#endif
