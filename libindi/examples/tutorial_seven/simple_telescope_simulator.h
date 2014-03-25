#ifndef SCOPESIM_H
#define SCOPESIM_H

#include "indibase/indiguiderinterface.h"
#include "indibase/inditelescope.h"
#include "indibase/alignment/AlignmentSubsystemForDrivers.h"



class ScopeSim : public INDI::Telescope, public INDI::AlignmentSubsystem::AlignmentSubsystemForDrivers
{
public:
    ScopeSim() : AxisStatusRA(STOPPED), AxisDirectionRA(FORWARD),
                AxisSlewRateRA(DEFAULT_SLEW_RATE), CurrentEncoderMicrostepsRA(0),
                AxisStatusDEC(STOPPED), AxisDirectionDEC(FORWARD),
                AxisSlewRateDEC(DEFAULT_SLEW_RATE), CurrentEncoderMicrostepsDEC(90.0 * MICROSTEPS_PER_DEGREE),
                DBG_SCOPE(INDI::Logger::getInstance().addDebugLevel("Scope Verbose", "SCOPE")) {}


private:
    virtual bool Abort();
    bool canSync();
    virtual bool Connect();
    virtual bool Disconnect();
    virtual const char *getDefaultName();
    bool Goto(double,double);
    virtual bool initProperties();
    friend void ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n);
    virtual bool ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n);
    friend void ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n);
    virtual bool ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n);
    friend void ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n);
    virtual bool ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n);
    friend void ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n);
    virtual bool ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n);
    virtual bool MoveNS(TelescopeMotionNS dir);
    virtual bool MoveWE(TelescopeMotionWE dir);
    virtual bool ReadScopeStatus();
    bool Sync(double ra, double dec);
    virtual void TimerHit();

    static const int MICROSTEPS_PER_REVOLUTION = 1000000;
    static const double MICROSTEPS_PER_DEGREE = MICROSTEPS_PER_REVOLUTION / 360.0;
    static const double DEFAULT_SLEW_RATE;

    enum AxisStatus { STOPPED, SLEWING, SLEWING_TO };
    enum AxisDirection { FORWARD, REVERSE };

    AxisStatus AxisStatusDEC;
    AxisDirection AxisDirectionDEC;
    double AxisSlewRateDEC;
    double CurrentEncoderMicrostepsDEC;
    double GotoTargetMicrostepsDEC;

    AxisStatus AxisStatusRA;
    AxisDirection AxisDirectionRA;
    double AxisSlewRateRA;
    double CurrentEncoderMicrostepsRA;
    double GotoTargetMicrostepsRA;


    unsigned int DBG_SCOPE;
};

#endif // SCOPESIM_H