/*******************************************************************************
  Copyright(c) 2015 Jasem Mutlaq. All rights reserved.

  Simple GPS Simulator

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.

  The full GNU General Public License is included in this distribution in the
  file called LICENSE.
*******************************************************************************/
#include <iostream>
#include <memory>
#include <libnova.h>
#include <time.h>

#include "gps_driver.h"

#include <libgpsmm.h>

using namespace std;



// We declare an auto pointer to GPSSimulator.
std::unique_ptr<GPSD> gpsd(new GPSD());

void ISGetProperties(const char *dev)
{
    gpsd->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num)
{
    gpsd->ISNewSwitch(dev, name, states, names, num);
}

void ISNewText(	const char *dev, const char *name, char *texts[], char *names[], int num)
{
    gpsd->ISNewText(dev, name, texts, names, num);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num)
{
    gpsd->ISNewNumber(dev, name, values, names, num);
}

void ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n)
{
  INDI_UNUSED(dev);
  INDI_UNUSED(name);
  INDI_UNUSED(sizes);
  INDI_UNUSED(blobsizes);
  INDI_UNUSED(blobs);
  INDI_UNUSED(formats);
  INDI_UNUSED(names);
  INDI_UNUSED(n);
}
void ISSnoopDevice (XMLEle *root)
{
    INDI_UNUSED(root);
}

GPSD::GPSD()
{
   setVersion(0,1);
   gps=NULL;
}

GPSD::~GPSD()
{

}

const char * GPSD::getDefaultName()
{
    return (char *)"GPSD driver";
}

bool GPSD::Connect()
{
    if (gps==NULL) {
        gps=new gpsmm("localhost", DEFAULT_GPSD_PORT);
    } 
    if (gps->stream(WATCH_ENABLE|WATCH_JSON) == NULL) {
        cerr << "No GPSD running.\n";
        return false;
    }
    return true;
}

bool GPSD::Disconnect()
{
    delete gps;
    gps=NULL;
    return true;
}

IPState GPSD::updateGPS()
{
    static char ts[32];
    struct tm *utc, *local;
    struct gps_data_t* data;

    time_t raw_time;
    time(&raw_time);

    utc  = gmtime(&raw_time);    
    strftime (ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", utc);
    IUSaveText(&TimeT[0], ts);

    local= localtime(&raw_time);
    snprintf(ts, sizeof(ts), "%4.2f", (local->tm_gmtoff/3600.0));
    IUSaveText(&TimeT[1], ts);

    TimeTP.s = IPS_OK;

	if (!gps->waiting(100000)) {
        cerr << "No GPSD running.\n";
        return IPS_ALERT;
    } else if ((data = gps->read()) == NULL) {
        cerr << "Read error.\n";
        return IPS_ALERT;
    } else if (data->status != STATUS_FIX) {
        cerr << "No fix.\n";
        return IPS_BUSY;
    } else {
        fprintf(stderr,"Fix: %d time: %lf\n", data->fix.mode, data->fix.time);
                
        LocationN[LOCATION_LATITUDE].value = data->fix.latitude;
        LocationN[LOCATION_LONGITUDE].value = data->fix.longitude;
        LocationN[LOCATION_ELEVATION].value = data->fix.altitude;
        LocationNP.s = IPS_OK;
        
        return IPS_OK;    
    }    
}

