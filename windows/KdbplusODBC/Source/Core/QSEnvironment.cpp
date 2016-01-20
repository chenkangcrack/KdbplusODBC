//==================================================================================================
///  @file QSEnvironment.cpp
///
///  Quickstart DSIEnvironment implementation class.
///
///  Copyright (C) 2009-2014 Simba Technologies Incorporated.
//==================================================================================================

#include "QSEnvironment.h"

#include "IDriver.h"
#include "QSConnection.h"

using namespace Simba::Quickstart;
using namespace Simba::DSI;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSEnvironment::QSEnvironment(IDriver* in_driver) : DSIEnvironment(in_driver)
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSEnvironment", "QSEnvironment");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSEnvironment::~QSEnvironment()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IConnection* QSEnvironment::CreateConnection()
{
    ENTRANCE_LOG(GetLog(), "Simba::Quickstart", "QSEnvironment", "CreateConnection");
    return new QSConnection(this);
}
