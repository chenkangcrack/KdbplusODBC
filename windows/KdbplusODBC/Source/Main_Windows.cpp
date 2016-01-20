//==================================================================================================
///  @file Main_Windows.cpp
///
///  Implementation of the DllMain() and DSIDriverFactory() for Windows platforms.
///
///  Copyright (C) 2009-2014 Simba Technologies Incorporated.
//==================================================================================================

#include "DSIDriverFactory.h"
#include "OLEDBBranding.h"
#include "SimbaSettingReader.h"
#include "QSDriver.h"

#ifdef SERVERTARGET
#include "SimbaServer.h"
#endif

using namespace Simba::Quickstart;
using namespace Simba::DSI;

simba_handle s_quickstartModuleId = 0;


//==================================================================================================
/// @brief DLL entry point.
///
/// @param in_module        Handle of the current module.
/// @param in_reasonForCall DllMain called with parameter telling if DLL is loaded into process or 
///                         thread address space.
/// @param in_reserved      Unused.
///
/// @return TRUE if successful, FALSE otherwise.
//==================================================================================================
BOOL APIENTRY DllMain(HINSTANCE in_module, DWORD in_reasonForCall, LPVOID in_reserved)
{
    // Avoid compiler warnings.
    UNUSED(in_reserved);

    if (in_reasonForCall == DLL_PROCESS_ATTACH)
    {
        s_quickstartModuleId = reinterpret_cast<simba_handle> (in_module);
    }

    return TRUE;
}

//==================================================================================================
/// @brief Creates an instance of IDriver for a driver. 
///
/// The resulting object is made available through DSIDriverSingleton::GetDSIDriver().
///
/// @param out_instanceID   Unique identifier for the IDriver instance.
///
/// @return IDriver instance. (OWN)
//==================================================================================================
IDriver* Simba::DSI::DSIDriverFactory(simba_handle& out_instanceID)
{
    out_instanceID = s_quickstartModuleId;

    // Set the branding for this driver. The branding affects where in the registry the driver 
    // values are looked up. For example, if the branding is set to "Simba\Quickstart", then in the
    // registry the values would be looked up at HKLM\SOFTWARE\Simba\Quickstart (or
    // HKLM\SOFTWARE\SOFTWARE\Wow6432Node\Simba\Quickstart if running a 32-bit driver on 64-bit
    // Windows) for a base registry path. If the DSII is compiled as a driver, then it will use
    // \Driver as a suffix, if it's configured as a server then it will use \Server as a suffix.
    // Therefore, a 64-bit driver would use the full path of HKLM\SOFTWARE\Simba\Quickstart\Driver
    // to look up the registry keys such as ErrorMessagesPath.
    SimbaSettingReader::SetConfigurationBranding(DRIVER_WINDOWS_BRANDING);

#ifdef SERVERTARGET
    // Set the server branding for this data source. This will only be used if the DSII is compiled
    // as a server and then installed as a service.
    Simba::Server::SetServiceName(
        _T("SimbaQuickstartService"),
        _T("SimbaQuickstartService"),
        _T("Server process for the Quickstart data source."));
#endif

#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #1: Construct driver singleton.")
    return new QSDriver();
}

/// @brief Returns information needed by SimbaOLEDB when building an OLEDB Provider.
/// @see Simba::OLEDB::GetOLEDBBranding declaration.
simba_wstring Simba::OLEDB::GetOLEDBBranding(
	ComBrandingInformation& out_providerBranding,
	ComBrandingInformation& out_errorLookupServiceBranding)
{
	out_providerBranding.m_name = L"Simba Quickstart OLE DB Provider";
	out_providerBranding.m_classID = "{6FDF2253-DD6F-4692-B8AD-D8AD4E747D90}";
	out_providerBranding.m_description = L"Simba Quickstart OLE DB Provider";
	out_providerBranding.m_progID = L"SimbaQuickstartProvider";
	out_providerBranding.m_versionIndependentProgID = "SimbaQuickstartProvider";

	out_errorLookupServiceBranding.m_name = L"Simba Quickstart Error Lookup Service";
	out_errorLookupServiceBranding.m_classID = "{FD6B9B53-F83B-4477-9969-0D9C95F9A255}";
	out_errorLookupServiceBranding.m_description = L"Error Lookup Service for the Simba Quickstart OLE DB Provider";
	out_errorLookupServiceBranding.m_progID = L"SimbaQuickstartProvider";
	out_errorLookupServiceBranding.m_versionIndependentProgID = L"SimbaQuickstartProvider";

	wchar_t filenameBuff[MAX_PATH + 1];
    if (!GetModuleFileNameW((HMODULE)s_quickstartModuleId, filenameBuff, sizeof(filenameBuff) / sizeof(filenameBuff[0])))
	{
		throw std::runtime_error("GetModuleFileNameW() in GetOLEDBBranding() failed.");
	}

	return filenameBuff;
}
