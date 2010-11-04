#ifndef INDIBASEDRIVER_H
#define INDIBASEDRIVER_H

#include <vector>
#include <map>
#include <string>

#include "indiapi.h"
#include "indidevapi.h"
#include "indibase.h"

#define MAXRBUF 2048

/**
 * \class INDI::BaseDriver
   \brief Class to provide basic INDI driver functionality.

   INDI::BaseClient contains a vector list of INDI::BaseDrivers. Upon connection with INDI server, the client create a INDI::BaseDriver
   \e instance for each driver owned by the INDI server. Properties of the driver can be build either by loading an external
   skeleton file that contains a list of defXXX commands, or by dynamically building properties as they arrive from the server.

   \author Jasem Mutlaq
 */
class INDI::BaseDriver
{
public:
    BaseDriver();
    virtual ~BaseDriver();

    /*! INDI error codes. */
    enum INDI_ERROR
    {
        INDI_DEVICE_NOT_FOUND=-1,       /*!< INDI Device was not found. */
        INDI_PROPERTY_INVALID=-2,       /*!< Property has an invalid syntax or attribute. */
        INDI_PROPERTY_DUPLICATED = -3,  /*!< INDI Device was not found. */
        INDI_DISPATCH_ERROR=-4          /*!< Dispatching command to driver failed. */
    };

    /*! INDI property type */
    enum INDI_TYPE
    {
        INDI_NUMBER, /*!< INumberVectorProperty. */
        INDI_SWITCH, /*!< ISwitchVectorProperty. */
        INDI_TEXT,   /*!< ITextVectorProperty. */
        INDI_LIGHT,  /*!< ILightVectorProperty. */
        INDI_BLOB    /*!< IBLOBVectorProperty. */
    };

    /** \return Return vector number property given its name */
    INumberVectorProperty * getNumber(const char *name);
    /** \return Return vector text property given its name */
    ITextVectorProperty * getText(const char *name);
    /** \return Return vector switch property given its name */
    ISwitchVectorProperty * getSwitch(const char *name);
    /** \return Return vector light property given its name */
    ILightVectorProperty * getLight(const char *name);
    /** \return Return vector BLOB property given its name */
    IBLOBVectorProperty * getBLOB(const char *name);

    /** \brief Remove a property
        \param name name of property to be removed
        \return 0 if successul, -1 otherwise.
    */
    int removeProperty(const char *name);

    /** \brief Return a property and its type given its name.
        \param name of property to be found.
        \param type of property found.
        \return If property is found, it is returned. To be used you must use static_cast with given the type of property
        returned.

        \note This is a low-level function and should not be called directly unless necessary. Use getXXX instead where XXX
        is the property type (Number, Text, Switch..etc).

    */
    void * getProperty(const char *name, INDI_TYPE & type);

    /** \brief Build driver properties from a skeleton file.
        \param filename full path name of the file.

    A skeloton file defines the properties supported by this driver. It is a list of defXXX elements enclosed by @<INDIDriver>@
 and @</INDIDriver>@ opening and closing tags. After the properties are created, they can be rerieved, manipulated, and defined
 to other clients.

 \see An example skeleton file can be found under examples/tutorial_four_sk.xml

    */
    void buildSkeleton(const char *filename);

    /** \return True if the device is connected (CONNECT=ON), False otherwise */
    bool isConnected();
    /** \brief Connect or Disconnect a device.
      \param status If true, the driver will attempt to connect to the device (CONNECT=ON). If false, it will attempt
to disconnect the device.
    */
    virtual void setConnected(bool status);

    /** \brief Set the device name
      \param dev new device name
      */
    void setDeviceName(const char *dev);
    /** \return Returns the device name */
    const char *deviceName();

    /** \brief Add message to the driver's message queue.
        \param msg Message to add.
    */
    void addMessage(const char *msg);
    //** \returns Returns the contents of the driver's message queue. *;
    const char *message() { return messageQueue.c_str(); }

    /** \brief Set the driver's mediator to receive notification of news devices and updated property values. */
    void setMediator(INDI::BaseMediator *med) { mediator = med; }
    /** \returns Get the meditator assigned to this driver */
    INDI::BaseMediator * getMediator() { return mediator; }

protected:

    /** \brief Build a property given the supplied XML element (defXXX)
      \param root XML element to parse and build.
      \param errmsg buffer to store error message in parsing fails.

      \return 0 if parsing is successful, -1 otherwise and errmsg is set */
    int buildProp(XMLEle *root, char *errmsg);

    /** \brief handle SetXXX commands from client */
    int setValue (XMLEle *root, char * errmsg);
    /** \brief handle SetBLOB command from client */
    int processBLOB(IBLOB *blobEL, XMLEle *ep, char * errmsg);
    /** \brief Parse and store BLOB in the respective vector */
    int setBLOB(IBLOBVectorProperty *pp, XMLEle * root, char * errmsg);

    char deviceID[MAXINDINAME];

private:

    typedef struct
    {
        INDI_TYPE type;
        void *p;
    } pOrder;

    std::vector<INumberVectorProperty *> pNumbers;
    std::vector<ITextVectorProperty *> pTexts;
    std::vector<ISwitchVectorProperty *> pSwitches;
    std::vector<ILightVectorProperty *> pLights;
    std::vector<IBLOBVectorProperty *> pBlobs;

    LilXML *lp;

    std::vector<pOrder> pAll;

    std::string messageQueue;

    INDI::BaseMediator *mediator;

    friend class INDI::BaseClient;
    friend class INDI::DefaultDriver;

};

#endif // INDIBASEDRIVER_H
