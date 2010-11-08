/*
    Tutorial Client
    Copyright (C) 2010 Jasem Mutlaq (mutlaqja@ikarustech.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef TUTORIAL_CLIENT_H
#define TUTORIAL_CLIENT_H

#include "indidevapi.h"
#include "indicom.h"
#include "indibase/baseclient.h"

class MyClient : public INDI::BaseClient
{
 public:

    MyClient();
    ~MyClient();

    void setTemperature();

protected:

    virtual void newDevice(const char *device_name);
    virtual void newProperty(const char *device_name, const char *property_name);
    virtual void newBLOB(IBLOB *bp) {}
    virtual void newSwitch(ISwitchVectorProperty *svp);
    virtual void newNumber(INumberVectorProperty *nvp);
    virtual void newText(ITextVectorProperty *tvp) {}
    virtual void newLight(ILightVectorProperty *lvp) {}
    virtual void serverConnected() {}
    virtual void serverDisconnected() {}

private:
   INDI::BaseDriver * ccd_simulator;

};

#endif
