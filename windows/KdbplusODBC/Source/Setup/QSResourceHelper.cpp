//==================================================================================================
///  @file QSResourceHelper.cpp
///
///  Implementation of the class QSResourceHelper.
///
///  Copyright (C) 2014 Simba Technologies Incorporated.
//==================================================================================================

#include "QSResourceHelper.h"

#include "NumberConverter.h"

using namespace Simba::Quickstart;

extern simba_handle s_quickstartModuleId;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSResourceHelper::LoadStringResource(UINT in_resourceStringId)
{
    static const simba_wstring loadError(L"Error loading the string.");

    // Try a buffer size of 2048, then increase the size until the entire string is retrieved.
    const std::vector<TCHAR>::size_type increment(2048);
    std::vector<TCHAR> buffer(increment);

    for (;;)
    {
        simba_int32 length = ::LoadString(
            GetModuleInstance(),
            in_resourceStringId,
            &buffer[0],
            static_cast<simba_int32>(buffer.size()));
        if (0 == length)
        {
            return loadError;
        }

        if (1 >= (buffer.size() - length))
        {
            buffer.resize(buffer.size() + increment);
        }
        else
        {
            return simba_wstring(&buffer[0], length);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSResourceHelper::LoadStringResource(UINT in_resourceStringId, CString& out_string)
{
    if (!out_string.LoadString(in_resourceStringId))
    {
        // The resource could not be found.
        QSTHROWGEN1(
            L"QSStringResourceError", 
            NumberConverter::ConvertUInt32ToWString(in_resourceStringId));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE QSResourceHelper::GetModuleInstance()
{
    return reinterpret_cast<HINSTANCE>(s_quickstartModuleId);
}
