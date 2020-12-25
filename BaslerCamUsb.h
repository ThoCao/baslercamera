#pragma once
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
# include <pylon/PylonGUI.h>
#endif
#include <iostream>

#include <pylon\gige\BaslerGigEInstantCamera.h>
#include <pylon\usb\BaslerUsbInstantCamera.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// Namespace for using pylon objects.
using namespace Pylon;

enum TRIGGERSOURCE{ SOFTWARE=1,HARDWARE=2 };

class BaslerCamUsb
{
	friend class BaslerImageEvent;
public:
	BaslerCamUsb(TRIGGERSOURCE trigger);
	~BaslerCamUsb();

	bool initCam();
	bool startGrab();
	bool stopGrab();
	void destroyCam();


protected:
	bool showView_ = true;

private:
	TRIGGERSOURCE _trigger;
	bool isConnected_ = false;
	Pylon::CBaslerUsbInstantCamera * camera_ = NULL;
	Pylon::IPylonDevice * pDevice_ = NULL;

};

class BaslerImageEvent : public Pylon::CBaslerUsbImageEventHandler {
public:

	BaslerImageEvent(BaslerCamUsb * cameraGrabber);

	virtual void OnImageGrabbed(Pylon::CBaslerUsbInstantCamera & camera, const Pylon::CBaslerUsbGrabResultPtr & ptrGrabResult);
protected:
	BaslerCamUsb * cameraGrabber_;
};

