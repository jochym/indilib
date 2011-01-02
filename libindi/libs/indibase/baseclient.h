#ifndef INDIBASECLIENT_H
#define INDIBASECLIENT_H

#include <vector>
#include <map>
#include <string>

#include "indiapi.h"
#include "indidevapi.h"
#include "indibase.h"

#define MAXRBUF 2048

using namespace std;

/**
 * \class INDI::BaseClient
   \brief Class to provide basic client functionality.

   BaseClient enables accelerated development of INDI Clients by providing a framework that facilitates communication, device
   handling, and event notification. By subclassing BaseClient, clients can quickly connect to an INDI server, and query for
   a set of INDI::BaseDriver devices, and read and write properties seamlessly. Event driven programming is possible due to
   notifications upon reception of new devices or properties.

   \attention All notifications functions defined in INDI::BaseMediator must be implemented in the client class even if
   they are not used because these are pure virtual functions.
   \author Jasem Mutlaq

 */
class INDI::BaseClient : public INDI::BaseMediator
{
public:
    enum { INDI_DEVICE_NOT_FOUND=-1, INDI_PROPERTY_INVALID=-2, INDI_PROPERTY_DUPLICATED = -3, INDI_DISPATCH_ERROR=-4 };

    BaseClient();
    virtual ~BaseClient();

    /** \brief Set the server host name and port
        \param hostname INDI server host name or IP address.
        \param port INDI server port.
    */
    void setServer(const char * hostname, unsigned int port);

    /** \brief Add a device to the watch list.

        A client may select to receive notifications of only a specific device or a set of devices.
        If the client encounters any of the devices set via this function, it will create a corresponding
        INDI::BaseDriver object to handle them. If no devices are watched, then all devices owned by INDI server
        will be created and handled.
    */
    void watchDevice(const char * deviceName);


    /** \brief Connect to INDI server.

        \returns True if the connection is successful, false otherwise.
        \note This function blocks until connection is either successull or unsuccessful.
    */
    bool connectServer();

    /** \brief Disconnect from INDI server.

        Disconnects from INDI servers. Any devices previously created will be deleted and memory cleared.
        \return True if disconnection is successful, false otherwise.
    */
    bool disconnectServer();

    /** \brief Connect/Disconnect to INDI driver
        \param status If true, the client will attempt to turn on CONNECTION property within the driver (i.e. turn on the device).
         Otherwise, CONNECTION will be turned off.
        \param deviceName Name of the device to connect to.
    */
    void setDriverConnection(bool status, const char *deviceName);

    /** \param deviceName Name of device to search for in the list of devices owned by INDI server,
         \returns If \e deviceName exists, it returns an instance of the device. Otherwise, it returns NULL.
    */
    INDI::BaseDriver * getDriver(const char * deviceName);

    /** \returns Returns a vector of all devices created in the client.
    */
    const vector<INDI::BaseDriver *> & getDrivers() const { return cDevices; }

    /** \brief Set Binary Large Object policy mode

      Set the BLOB handling mode for the client. The client may either recieve:
      <ul>
      <li>Only BLOBS</li>
      <li>BLOBs mixed with normal messages</li>
      <li>Normal messages only, no BLOBs</li>
      </ul>

      If \e dev and \e prop are supplied, then the BLOB handling policy is set for this particular device and property.
      if \e prop is NULL, then the BLOB policy applies to the whole device.


      \param blobH BLOB handling policy
      \param dev name of device, required.
      \param prop name of property, optional.
    */
    void setBLOBMode(BLOBHandling blobH, const char *dev, const char *prop = NULL);

    // Update
    static void * listenHelper(void *context);

protected:

    /** \brief Dispatch command received from INDI server to respective devices handled by the client */
    int dispatchCommand(XMLEle *root, char* errmsg);

    /** \brief Remove device */
    int removeDevice( const char * devName, char * errmsg );

    /** \brief Delete property command */
    int delPropertyCmd (XMLEle *root, char * errmsg);

    /** \brief Find and return a particular device */
    INDI::BaseDriver * findDev( const char * devName, char * errmsg);
    /** \brief Add a new device */
    INDI::BaseDriver * addDevice (XMLEle *dep, char * errmsg);
    /** \brief Find a device, and if it doesn't exist, create it if create is set to 1 */
    INDI::BaseDriver * findDev (XMLEle *root, int create, char * errmsg);

    /**  Process messages */
    int messageCmd (XMLEle *root, char * errmsg);
    /**  Process messages */
    void checkMsg (XMLEle *root, INDI::BaseDriver *dp);
    /**  Process messages */
    void doMsg (XMLEle *msg, INDI::BaseDriver *dp);

    /** \brief Send new Text command to server */
    void sendNewText (ITextVectorProperty *pp);
    /** \brief Send new Number command to server */
    void sendNewNumber (INumberVectorProperty *pp);
    /** \brief Send new Switch command to server */
    void sendNewSwitch (ISwitchVectorProperty *pp, ISwitch *lp);
    /** \brief Send opening tag for BLOB command to server */
    void startBlob( const char *devName, const char *propName, const char *timestamp);
    /** \brief Send ONE blob content to server */
    void sendOneBlob( const char *blobName, unsigned int blobSize, const char *blobFormat, unsigned char * blobBuffer);
    /** \brief Send closing tag for BLOB command to server */
    void finishBlob();

private:

    // Listen to INDI server and process incoming messages
    void listenINDI();

    // Thread for listenINDI()
    pthread_t listen_thread;

    vector<INDI::BaseDriver *> cDevices;
    vector<string> cDeviceNames;

    string cServer;
    unsigned int cPort;
    bool sConnected;

    // Parse & FILE buffers for IO
    int sockfd;
    LilXML *lillp;			/* XML parser context */
    FILE *svrwfp;			/* FILE * to talk to server */

};

#endif // INDIBASECLIENT_H
