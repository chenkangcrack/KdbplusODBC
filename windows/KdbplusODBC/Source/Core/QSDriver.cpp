//==================================================================================================
///  @file QSDriver.cpp
///
///  Quickstart DSIDriver implementation class.
///
///  Copyright (C) 2009-2014 Simba Technologies Incorporated.
//==================================================================================================

#include "QSDriver.h"

#include "DSIDriverProperties.h"
#include "DSIFileLogger.h"
#include "DSIMessageSource.h"
#include "QSEnvironment.h"

using namespace Simba::Quickstart;
using namespace Simba::DSI;
using namespace std;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #3: Set the driver-wide logging details.")
QSDriver::QSDriver() : DSIDriver(), m_driverLog(new DSIFileLogger("KdbplusODBC_driver.log"))
{
    ENTRANCE_LOG(m_driverLog, "Simba::Quickstart", "QSDriver", "QSDriver");
    SetDriverPropertyValues();

#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #9: Register Messages xml file for handling by DSIMessageSource.")
    m_msgSrc->RegisterMessages(ERROR_MESSAGES_FILE, QS_ERROR);

#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #10: Set the vendor name, which will be prepended to error messages.")
    /*
    NOTE: We do not set the vendor name here on purpose, because the default vendor name is 'Simba'.
    The code below shows how to set the vendor name. A typical error message will take the form

    [<vendor>][<component>] (nativeErrorCode) <message>

    so setting the vendor name will change the [<vendor>] part from the default of [Simba].

    m_msgSrc->SetVendorName(DRIVER_VENDOR);
    */
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSDriver::~QSDriver()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IEnvironment* QSDriver::CreateEnvironment()
{
    ENTRANCE_LOG(m_driverLog, "Simba::Quickstart", "QSDriver", "CreateEnvironment");
    return new QSEnvironment(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ILogger* QSDriver::GetDriverLog()
{
    return m_driverLog.Get();
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
void QSDriver::SetDriverPropertyValues()
{
#pragma message (__FILE__ "(" MACRO_TO_STRING(__LINE__) ") : TODO #2: Set the driver properties.") 
    // Set the following
    //      DSI_DRIVER_DRIVER_NAME
    //          The name of the driver that is shown to the application, 
    //
    // Ensure that the following is set for your driver:
    //      DSI_DRIVER_STRING_DATA_ENCODING
    //          The encoding of char data from the perspective of the data store. 
    //          For this driver, the encoding is ENC_UTF8.
    //      DSI_DRIVER_WIDE_STRING_DATA_ENCODING
    //          The encoding of wide character data from the perspective of the data store.  
    //          For this driver, the encoding is ENC_UTF16_LE.

    // The name of the driver used to access the datastore.
    SetProperty(
        DSI_DRIVER_DRIVER_NAME, 
        AttributeData::MakeNewWStringAttributeData("KdbplusODBC"));

    // The default encoding used for string data and input/output parameter values.
    SetProperty(
        DSI_DRIVER_STRING_DATA_ENCODING,
        AttributeData::MakeNewInt16AttributeData(ENC_UTF8));

    // The default encoding used for wide string data and input/output parameter values.
#if (1 == PLATFORM_IS_LITTLE_ENDIAN)
    SetProperty(
        DSI_DRIVER_WIDE_STRING_DATA_ENCODING, 
        AttributeData::MakeNewInt16AttributeData(ENC_UTF16_LE));
#else
    SetProperty(
        DSI_DRIVER_WIDE_STRING_DATA_ENCODING, 
        AttributeData::MakeNewInt16AttributeData(ENC_UTF16_BE));
#endif
}
