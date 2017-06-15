/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef MicrophoneDeviceDriverh
#define MicrophoneDeviceDriverh

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

namespace yarp {
    namespace dev {
        class MicrophoneDeviceDriver;
    }
}

/**
 * @ingroup dev_impl_media
 *
 * A basic microphone input source.
 * The implementation is very simple right now - if you want to
 * use this for real you should fix it up.
 *
 */
class yarp::dev::MicrophoneDeviceDriver :
    public IAudioGrabberSound, public DeprecatedDeviceDriver
{
private:
    MicrophoneDeviceDriver(const MicrophoneDeviceDriver&);
    void operator=(const MicrophoneDeviceDriver&);

public:
    /**
     * Constructor.
     */
    MicrophoneDeviceDriver();

    /**
     * Destructor.
     */
    virtual ~MicrophoneDeviceDriver();

    /**
     * Open the device driver.
     * @param config parameters for the device driver
     * @return returns true on success, false on failure.
     */
    virtual bool open(yarp::os::Searchable& config) override;

    /**
     * Closes the device driver.
     * @return returns true/false on success/failure.
     */
    virtual bool close(void) override;

    virtual bool getSound(yarp::sig::Sound& sound) override;

    virtual bool startRecording() override;
    virtual bool stopRecording() override;

protected:
    void *system_resources;
    int dsp;
};


#endif
