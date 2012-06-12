/*
    INDI Driver for Optec TCF-S Focuser

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

#ifndef TCFS_H
#define TCFS_H

#include <string>

#include <indidevapi.h>
#include <indicom.h>
#include <indibase/defaultdevice.h>

using namespace std;

#define TCFS_MAX_CMD        16
#define TCFS_MAX_TRIES      3
#define TCFS_ERROR_BUFFER   1024

class TCFS : public INDI::DefaultDevice
{

public:

    enum TCFSCommand {         FMMODE ,         // Focuser Manual Mode
                               FFMODE,          // Focuser Free Mode
                               FAMODE,          // Focuser Auto-A Mode
                               FBMODE,          // Focuser Auto-B Mode
                               FCENTR,          // Focus Center
                               FIN,             // Focuser In “nnnn”
                               FOUT,            // Focuser Out “nnnn”
                               FPOSRO,          // Focuser Position Read Out
                               FTMPRO,          // Focuser Temperature Read Out
                               FSLEEP,          // Focuser Sleep
                               FWAKUP,          // Focuser Wake Up
                               FHOME,           // Focuser Home Command
			     };

    enum TCFSMode { TCFS_MANUAL_MODE, TCFS_A_MODE, TCFS_B_MODE };
    enum TCFSMotion { TCFS_INWARD, TCFS_OUTWARD };
    enum TCFSError { NO_ERROR, ER_1, ER_2, ER_3 };

    TCFS();
    ~TCFS();
   
    virtual bool Connect();
    virtual bool Disconnect();

    // Standard INDI interface fucntions
    virtual void ISGetProperties(const char *dev);
    virtual bool ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n);
    virtual bool ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n);
    virtual bool ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n);
 	

    void ISPoll();


private: 

    ISwitchVectorProperty *ConnectSP;
    ISwitchVectorProperty *FocusPowerSP;
    ISwitchVectorProperty *FocusModeSP;
    INumberVectorProperty *FocusStepNP;
    INumberVectorProperty *FocusPositionNP;
    INumberVectorProperty *FocusPositionRequestNP;
    INumberVectorProperty *FocusTemperatureNP;

    // Functions
    void init_properties();
    bool move_focuser(TCFSMotion dir);

    const char *getDefaultName();
    bool read_tcfs();
    bool dispatch_command(TCFSCommand command);

    // Variables
    string default_port;
			
    int fd;
    char command[TCFS_MAX_CMD];
    char response[TCFS_MAX_CMD];

    unsigned int simulated_position;
    unsigned int target_position;
    float simulated_temperature;

    bool isTCFS3;

};

#endif


