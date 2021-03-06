#pragma once

// includes for ARSDK
extern "C"
{
#include <libARController/ARController.h>
#include <libARDiscovery/ARDiscovery.h>
#include <libARSAL/ARSAL.h>
}

#include <iostream>
#include <string>

#include "../common/semaphore.h"

// these values are hardcoded for Bebop drones
#define BEBOP_IP_ADDRESS "192.168.42.1"
#define BEBOP_DISCOVERY_PORT 44444

/*
 * If the DUMMY_DRONE flag is set, we don't connect to drone and just output
 * messages to console when steering commands are sent.
 *
 * If the NO_FLY flag is set, we *do* connect to the drone, but don't let it
 * fly.
 */
#ifdef DUMMY_DRONE
#define NO_FLY
#endif

typedef unsigned char u8;
typedef signed char i8;

namespace Parrot {
using namespace std;
using namespace Parrot;

#ifndef DUMMY_DRONE
/*
 * Wrapper around ARDISCOVERY_Device_t functionality.
 *
 * This is used for discovering a Bebop drone on the local network.
 */
class DiscoveryDevice
{
public:
    ARDISCOVERY_Device_t *dev = nullptr;

    DiscoveryDevice();
    ~DiscoveryDevice();
    void discover();

private:
    static inline void checkError(eARDISCOVERY_ERROR err)
    {
        if (err != ARDISCOVERY_OK) {
            throw std::runtime_error(std::string("Discovery error: ") +
                                     ARDISCOVERY_Error_ToString(err));
        }
    }
};
#endif // DUMMY_DRONE

/*
 * Simply throws a runtime_error with appropriate message if
 * err != ARCONTROLLER_OK.
 */
inline void
checkError(eARCONTROLLER_ERROR err)
{
    if (err != ARCONTROLLER_OK) {
        throw runtime_error(string("Controller error: ") +
                            ARCONTROLLER_Error_ToString(err));
    }
}

using flightEventHandler = void (*)(bool takeoff);

/*
 * Main class for interfacing with drone. Handles connection/disconnection and
 * sending steering commands.
 *
 * Video stream functionality is implemented in video.h by BebopVideoStream
 * class.
 */
class Bebop
{
public:
    ARCONTROLLER_Device_t *m_Device = nullptr;

    Bebop();
    ~Bebop();
    void connect();
    void disconnect();
    void takeOff();
    void land();
    void setPitch(i8 pitch);
    void setRoll(i8 right);
    void setUpDown(i8 up);
    void setYaw(i8 right);
    void stopMoving();
    void takePhoto();
    void setFlightEventHandler(flightEventHandler);

private:
    Semaphore m_Semaphore;
    bool m_IsConnected = false;
    flightEventHandler m_FlightEventHandler = nullptr;

#ifndef DUMMY_DRONE
    inline eARCONTROLLER_DEVICE_STATE getStateUpdate();
    inline eARCONTROLLER_DEVICE_STATE getState();
    inline void createControllerDevice(DiscoveryDevice &device);
    inline void addEventHandlers();
    void batteryChanged(ARCONTROLLER_DICTIONARY_ELEMENT_t *dict);
    static int printCallback(eARSAL_PRINT_LEVEL level,
                             const char *tag,
                             const char *format,
                             va_list va);
    static void stateChanged(eARCONTROLLER_DEVICE_STATE newstate,
                             eARCONTROLLER_ERROR err,
                             void *data);
    static void commandReceived(eARCONTROLLER_DICTIONARY_KEY key,
                                ARCONTROLLER_DICTIONARY_ELEMENT_t *dict,
                                void *data);
#endif // !DUMMY_DRONE
};     // class Bebop
} // namespace Parrot
