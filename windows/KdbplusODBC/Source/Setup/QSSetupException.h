//==================================================================================================
///  @file QSSetupException.h
///
///  Definition of the Class QSSetupException.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSSETUPEXCEPTION_H_
#define _SIMBA_QUICKSTART_QSSETUPEXCEPTION_H_

#include "Quickstart.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Exception for configuration dialog errors.
    class QSSetupException
    {
    // Public ======================================================================================
    public:
        /// Error codes enumeration.
        enum ErrorCode
        {
            /// The hwndParent argument was invalid.
            CFG_DLG_ERROR_INVALID_HWND,

            /// The lpszAttributes argument contained a syntax error.
            CFG_DLG_ERROR_INVALID_KEYWORD_VALUE,

            /// The lpszDriver argument was invalid. It could not be found in the registry.
            CFG_DLG_ERROR_INVALID_NAME,

            /// The fRequest argument was not one of the following:
            /// ODBC_ADD_DSN ODBC_CONFIG_DSN ODBC_REMOVE_DSN
            CFG_DLG_ERROR_INVALID_REQUEST_TYPE,

            /// Could not perform the operation requested by the fRequest argument.
            CFG_DLG_ERROR_REQUEST_FAILED,

            /// A driver-specific error for which there is no defined ODBC installer error. 
            /// The SzError argument in a call to the SQLPostInstallerError function should 
            /// contain the driver-specific error message.
            CFG_DLG_ERROR_DRIVER_SPECIFIC,

            /// Error writing to the registry.
            CFG_DLG_ERROR_REGISTRY_WRITE,

            /// The DBF was invalid.
            CFG_DLG_ERROR_INVALID_DBF,

            /// The maximum number of log files was invalid.
            CFG_DLG_ERROR_INVALID_MAX_LOG_NUM,

            /// The maximum size of each log file was invalid.
            CFG_DLG_ERROR_INVALID_MAX_LOG_SIZE,

            /// Denotes last item in the enumeration. Do not use.
            CFG_DLG_ERROR_LAST = CFG_DLG_ERROR_INVALID_MAX_LOG_SIZE + 1
        };

        /// @brief Constructor.
        /// 
        /// Constructs a new instance of the class with the specified error code.
        ///
        /// @param in_errorCode     Error code.
        QSSetupException(ErrorCode in_errorCode) : m_errorCode(in_errorCode) {}

        /// @brief Destructor.
        virtual ~QSSetupException() {}

        /// @brief Gets the error message string.
        ///
        /// @return The error message string.
        simba_wstring GetErrorMsg() const;

        /// @brief Gets the error code translated to ODBC Installer API.
        ///
        /// @return error code.
        DWORD GetErrorCode() const;

    // Private =====================================================================================
    private:
        // The error code for this exception.
        ErrorCode m_errorCode;
    };
}
}

#endif
