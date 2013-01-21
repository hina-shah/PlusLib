/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkDataCollectorTest2.cxx 
  \brief This a test program acquires both video and tracking data and writes them into separate metafiles
*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusDevice.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkSmartPointer.h"
#include "vtkTimerLog.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"

int main(int argc, char **argv)
{
  int numberOfFailures(0); 
  std::string inputConfigFileName;
  double inputAcqTimeLength(20);
  std::string outputFolder("./"); 
  std::string outputTrackerBufferSequenceFileName("TrackerBufferMetafile"); 
  std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  bool outputCompressed(true);

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 20s)");	
  args.AddArgument("--video-buffer-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferMetafile, "Video buffer sequence metafile.");
  args.AddArgument("--tracker-buffer-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferMetafile, "Tracker buffer sequence metafile.");
  args.AddArgument("--output-tracker-buffer-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker buffer sequence metafile (Default: TrackerBufferMetafile)");
  args.AddArgument("--output-video-buffer-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVideoBufferSequenceFileName, "Filename of the output video buffer sequence metafile (Default: VideoBufferMetafile)");
  args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder (Default: ./)");
  args.AddArgument("--output-compressed", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputCompressed, "Compressed output (0=non-compressed, 1=compressed, default:compressed)");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFileName.empty())
  {
    std::cerr << "input-config-file-name is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  ///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    std::cerr << "Unable to read configuration from file " << inputConfigFileName.c_str() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 

  dataCollector->ReadConfiguration( configRootElement );  
  vtkPlusDevice* videoDevice = NULL;
  vtkPlusDevice* trackerDevice = NULL;

  if ( !inputVideoBufferMetafile.empty() )
  {
    if( dataCollector->GetDevice(videoDevice, "SavedDataVideo") != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to locate the device with Id=\"SavedDataVideo\". Check config file.");
      exit(EXIT_FAILURE);
    }
    vtkSavedDataVideoSource* videoSource = dynamic_cast<vtkSavedDataVideoSource*>(videoDevice); 
    if ( videoSource == NULL )
    {
      LOG_ERROR( "Unable to cast device to vtkSavedDataVideoSource." );
      exit( EXIT_FAILURE );
    }
    videoSource->SetSequenceMetafile(inputVideoBufferMetafile.c_str()); 
  }

  if ( !inputTrackerBufferMetafile.empty() )
  {
    if( dataCollector->GetDevice(trackerDevice, "SavedDataTracker") != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to locate the device with Id=\"SavedDataTracker\". Check config file.");
      exit(EXIT_FAILURE);
    }
    vtkSavedDataVideoSource* tracker = dynamic_cast<vtkSavedDataVideoSource*>(trackerDevice); 
    if ( tracker == NULL )
    {
      LOG_ERROR( "Unable to cast tracker to vtkSavedDataVideoSource" );
      exit( EXIT_FAILURE );
    }
    tracker->SetSequenceMetafile(inputTrackerBufferMetafile.c_str()); 
  }

  if ( dataCollector->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to connect to data collector!"); 
    exit( EXIT_FAILURE );
  }

  if ( dataCollector->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start data collection"); 
    exit( EXIT_FAILURE );
  }

  const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

  while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
  {
    LOG_INFO( acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() << " seconds left..." ); 
    vtksys::SystemTools::Delay(1000); 
  }

  vtkSmartPointer<vtkPlusStreamBuffer> videobuffer = vtkSmartPointer<vtkPlusStreamBuffer>::New(); 
  if ( videoDevice != NULL ) 
  {
    LOG_INFO("Copy video buffer"); 
    videobuffer->DeepCopy(videoDevice->GetBuffer());
  }

  vtkSmartPointer<vtkPlusDevice> tracker = vtkSmartPointer<vtkPlusDevice>::New(); 
  if ( trackerDevice != NULL )
  {
    LOG_INFO("Copy tracker"); 
    tracker->DeepCopy(trackerDevice);
  }

  if ( videoDevice != NULL ) 
  {
    LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
    videobuffer->WriteToMetafile(outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), outputCompressed); 
  }

  if ( trackerDevice != NULL )
  {
    LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
    tracker->WriteToMetafile(outputFolder.c_str(), outputTrackerBufferSequenceFileName.c_str(), outputCompressed); 
  }


  if ( videoDevice != NULL )
  {
    std::ostringstream filepath; 
    filepath << outputFolder << "/" << outputVideoBufferSequenceFileName << ".mha"; 

    if ( vtksys::SystemTools::FileExists(filepath.str().c_str(), true) )
    {
      LOG_INFO("Remove generated video metafile!"); 
      if ( !vtksys::SystemTools::RemoveFile(filepath.str().c_str()) )
      {
        LOG_ERROR("Unable to remove generated video buffer: " << filepath.str() ); 
        numberOfFailures++; 
      }
    }
    else
    {
      LOG_ERROR("Unable to find video buffer at: " << filepath.str() ); 
      numberOfFailures++; 
    }
  }

  if ( trackerDevice != NULL )
  {
    std::ostringstream filepath; 
    filepath << outputFolder << "/" << outputTrackerBufferSequenceFileName << ".mha"; 

    if ( vtksys::SystemTools::FileExists(filepath.str().c_str(), true) )
    {
      LOG_INFO("Remove generated tracker metafile!"); 
      if ( !vtksys::SystemTools::RemoveFile(filepath.str().c_str()) )
      {
        LOG_ERROR("Unable to remove generated tracker buffer: " << filepath.str() ); 
        numberOfFailures++; 
      }
    }
    else
    {
      LOG_ERROR("Unable to find tracker buffer at: " << filepath.str() ); 
      numberOfFailures++; 
    }
  }

  if ( dataCollector->Stop() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to stop data collection!"); 
    numberOfFailures++; 
  }

  if ( numberOfFailures > 0 ) 
  {
    LOG_ERROR("Number of failures: " << numberOfFailures ); 
    return EXIT_FAILURE; 
  }

  std::cout << "Test completed successfully!" << std::endl;
  return EXIT_SUCCESS; 

}

