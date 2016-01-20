//==================================================================================================
///  @file QSResourceHelper.h
///
///  Definition of the class QSResourceHelper.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSRESOURCEHELPER_H_
#define _SIMBA_QUICKSTART_QSRESOURCEHELPER_H_

#include "Quickstart.h"

#include <atlstr.h>

namespace Simba
{
namespace Quickstart
{
    /// @brief Helper class to load resource strings from the String table.
    class QSResourceHelper
    {
    // Public ======================================================================================
    public:
        /// @brief Loads the specified string from the resource's String table.
        ///
        /// @param in_resourceStringId      The integer identifier of the string to be loaded.
        /// 
        /// @return The string corresponding to the given identifier.
        static simba_wstring LoadStringResource(UINT in_resourceStringId);

        /// @brief Loads the specified string from the resource's String table.
        ///
        /// @param in_resourceStringId      The integer identifier of the string to be loaded.
        /// @param out_string               The string corresponding to the given identifier.
        static void LoadStringResource(UINT in_resourceStringId, CString& out_string);

        /// @brief Gets the handle of this DLL module.
        /// 
        /// @return The handle of this DLL module.
        static HINSTANCE GetModuleInstance();
    };
}
}

#endif
