/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusAndorCam_h
#define __vtkPlusAndorCam_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
 \class vtkPlusAndorCam
 \brief Class for acquiring images from Andor cameras

 Requires PLUS_USE_ANDOR_CAM option in CMake.
 Requires the Andor SDK (SDK provided by Andor).

 \ingroup PlusLibDataCollection.
*/
class vtkPlusDataCollectionExport vtkPlusAndorCam: public vtkPlusDevice
{
public:
  /*! Constructor for a smart pointer of this class*/
  static vtkPlusAndorCam* New();
  vtkTypeMacro(vtkPlusAndorCam, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Specify the device connected to this class */
  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  /*! Get the version of SDK */
  virtual std::string GetSdkVersion();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorShutter(int shutter);

  /*! Get the shutter for the camera */
  int GetAndorShutter();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorExposureTime(float exposureTime);

  /*! Get the shutter for the camera */
  float GetAndorExposureTime();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorPreAmpGain(int preAmptGain);

  /*! Get the shutter for the camera */
  int GetAndorPreAmpGain();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorAcquisitionMode(int acquisitionMode);

  /*! Get the shutter for the camera */
  int GetAndorAcquisitionMode();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorReadMode(int setReadMode);

  /*! Get the shutter for the camera */
  int GetAndorReadMode();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorTriggerMode(int triggerMode);

  /*! Get the shutter for the camera */
  int GetAndorTriggerMode();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorHbin(int hbin);

  /*! Get the shutter for the camera */
  int GetAndorHbin();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorVbin(int vbin);

  /*! Get the shutter for the camera */
  int GetAndorVbin();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorCoolTemperature(int coolTemp);

  /*! Get the shutter for the camera */
  int GetAndorCoolTemperature();

  /*! Set the shutter for the camera */
  PlusStatus SetAndorSafeTemperature(int safeTemp);

  /*! Get the shutter for the camera */
  int GetAndorSafeTemperature();

protected:
  /*! Constructor */
  vtkPlusAndorCam();

  /*! Destructor */
  ~vtkPlusAndorCam();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  //virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  //virtual PlusStatus InternalStopRecording();

  /*! Initialize vtkPlusAndorCam */
  PlusStatus InitializeAndorCam();

  /*! The internal function which actually does the grab. */
  PlusStatus InternalUpdate();

  /*! Wait US Data from US device  */
  //PlusStatus WaitForFrame();

  ///* Calculate US Image Display */
  //PlusStatus CalculateDisplay();

  ///* Calculate US Image Display with a given B-Mode view option */
  //PlusStatus CalculateDisplay(unsigned int option);

  ///*! Get probe name from the device */
  //PlusStatus GetProbeNameDevice(std::string& probeName);

  /*! Check for errors in Andor SDK */
  PlusStatus CheckAndorSDKError(unsigned int _ui_err, const std::string _cp_func);

  /*! Check and adjust the camera temperature */
  PlusStatus CheckAndAdjustCameraTemperature(int targetTemp);

  // HS: TODO: Need to handle differet cases for  read/acquisiton modes?
  // HS: TODO: Handle temperature handling correctly.

  int                   AndorShutter;
  float                 AndorExposureTime;  // seconds
  std::array<int, 2>    AndorHSSpeed;
  int                   AndorPreAmpGain;
  /*! From AndorSDK:=> 1: Single Scan   2: Accumulate   3: Kinetics   4: Fast Kinetics   5: Run till abort  */
  int                   AndorAcquisitionMode;
  /*! From AndorSDK:=> 0: Full Vertical Binning   1: Multi-Track   2: Random-Track   3: Single-Track   4: Image */
  int                   AndorReadMode;
  /*! From AndorSDK:=> 0. Internal   1. External  6. External Start  7. External Exposure(Bulb)  9. External FVB EM(only valid for EM Newton models in FVB mode) 10. Software Trigger  12. External Charge Shifting */
  int                   AndorTriggerMode;
  int                   AndorHbin;
  int                   AndorVbin;
  int                   AndorCoolTemperature;
  int                   AndorSafeTemperature;
  int                   AndorCurrentTemperature;

private:
  vtkPlusAndorCam(const vtkPlusAndorCam&);  // Not implemented
  void operator=(const vtkPlusAndorCam&);  // Not implemented
};

#endif