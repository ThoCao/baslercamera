#include "BaslerCamUsb.h"





BaslerCamUsb::BaslerCamUsb(TRIGGERSOURCE trigger): _trigger(trigger)
{

}

BaslerCamUsb::~BaslerCamUsb()
{
	destroyCam();
}

bool BaslerCamUsb::initCam()
{
	Pylon::PylonInitialize();
	try {
		try {

			Pylon::CDeviceInfo info;
			info.SetDeviceClass(Pylon::CBaslerUsbInstantCamera::DeviceClass());
			pDevice_ = Pylon::CTlFactory::GetInstance().CreateFirstDevice(info);
			camera_ = new Pylon::CBaslerUsbInstantCamera(pDevice_);


		}
		catch (const Pylon::GenericException & e) {
			std::cerr << "[ERROR] an exception occured : " << e.GetDescription() << std::endl;
		}

		std::cout << "Using Device: " << camera_->GetDeviceInfo().GetModelName() << std::endl;
		std::cout << "Serial Number: " << camera_->GetDeviceInfo().GetSerialNumber() << std::endl;
		camera_->RegisterImageEventHandler(new BaslerImageEvent(this), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

		/// Open the camera , so that you could set camera parameter
		camera_->Open();
		camera_->MaxNumBuffer = 5;
		/// set hardware trigger parameter
		/// here we used a Arduino microcontroller to generate hardware trigger signal
		camera_->AcquisitionMode.SetValue(Basler_UsbCameraParams::AcquisitionMode_Continuous);
		camera_->TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);
		camera_->TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);
		camera_->AcquisitionFrameRateEnable.SetValue(false);
		camera_->TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line1);
		camera_->ExposureMode.SetValue(Basler_UsbCameraParams::ExposureMode_Timed);		camera_->ExposureTime.SetValue(3000.0);

		isConnected_ = true;
	}
	catch (const  GenericException &e) {
		std::cerr << "[ERROR] an exception occured :" << e.GetDescription() << std::endl;
		isConnected_ = 0;
	}
	return isConnected_;
}

bool BaslerCamUsb::startGrab()
{
	std::cerr << "************[INFO] Start Grabbing Images************" << std::endl;
	try {

		camera_->AcquisitionStart.Execute();
		camera_->StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

	}
	catch (const Pylon::GenericException & e) {
		std::cerr << "[ERROR] an exception occured :" << e.GetDescription() << std::endl;
	}

	return false;
}

bool BaslerCamUsb::stopGrab()
{
	std::cerr << "************[INFO] Grabbing images end***********" << std::endl;
	try {
		camera_->AcquisitionStop.Execute();
	}
	catch (const Pylon::GenericException & e) {
		std::cerr << "[ERROR] an exception occured : " << e.GetDescription() << std::endl;
	}
	return false;
}

void BaslerCamUsb::destroyCam()
{
	std::cerr << "[INFO] Basler Camera Destroyed" << std::endl;

	// close the camera
	try {
		camera_->Close();

	}
	catch (const Pylon::GenericException & e) {
		std::cerr << "[ERROR] an exception occured : " << e.GetDescription() << std::endl;
	}

	Pylon::PylonTerminate();
}

BaslerImageEvent::BaslerImageEvent(BaslerCamUsb * cameraGrabber)
{
	cameraGrabber_ = cameraGrabber;
}

void BaslerImageEvent::OnImageGrabbed(Pylon::CBaslerUsbInstantCamera & camera, const Pylon::CBaslerUsbGrabResultPtr & ptrGrabResult)
{
	int imgHeight = ptrGrabResult->GetHeight();
	int imgWidth = ptrGrabResult->GetWidth();

	std::cout << "[INFO] Image Grabbed Result Report : " << std::endl;
	std::cout << "image height = " << imgHeight << " , image width = " << imgWidth << std::endl;

	// display image in an opencv window
	cv::Size imgSize(imgWidth, imgHeight);
	cv::Mat frame(imgSize, CV_8UC1, (uchar *)ptrGrabResult->GetBuffer());
	if (cameraGrabber_->showView_) {

		cv::imshow("frame", frame);
		cv::waitKey(1);
	}
}
