	//opencv
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
	//C
#include <stdio.h>
	//C++
#include <iostream>
#include <string>
	//Custom
#include "backgroundSubtractor.hpp"
#include "imageProcessor.hpp"
#include "wizard.hpp"
#include "classifierTrain.hpp"
	//Boost
#include <boost/progress.hpp>



void backgroundSubtractorProcessor(const std::string& videoFilename, const std::string& savePath, params par) {

	cv::Mat frame; //current frame
	cv::Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
	cv::Ptr<cv::BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	int keyboard=0; //input from keyboard

		//Image writer PNG parameters.
	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(3);

	imageProcessor imProcessor;
	int numberOfFrame=0;

	if (par.watchOutputvideo){
			//create GUI windows
		cv::namedWindow("Frame");
		cv::namedWindow("FG Mask MOG 2");
		cv::moveWindow("FG Mask MOG 2", 600,0);
	}
		//create a Background Subtractor object
		//BackgroundSubtractorMOG2(int history, float varThreshold, bool bShadowDetection=true)
	pMOG2 = cv::createBackgroundSubtractorMOG2(2000,220,false);

		//create the capture object
	cv::VideoCapture capture(videoFilename);
	auto length = long(capture.get(cv::CAP_PROP_FRAME_COUNT));
		//Progress Bar from Boost.
	boost::progress_display show_progress(length);


	if(!capture.isOpened()){
			//error in opening the video input
		std::cerr << "Unable to open video file: " << videoFilename << std::endl;
		exit(EXIT_FAILURE);
	}
		//read input data. ESC or 'q' for quitting
	while( (char)keyboard != 'q' && (char)keyboard != 27 ){
			//read the current frame
		if(!capture.read(frame)) {
			std::cerr << "Unable to read next frame." << std::endl;
			std::cerr << "Exiting..." << std::endl;
			exit(EXIT_FAILURE);
		}
		++show_progress;
			//update the background model
		pMOG2->apply(frame, fgMaskMOG2);

			//Naming and processing of blobs
		numberOfFrame++;
		std::string naming = "frame"+std::to_string(numberOfFrame);
		imProcessor.blobExtractor(fgMaskMOG2, savePath, naming);
		
			//Only applicable to showing video
		if (par.watchOutputvideo){
			imshow("Frame", frame);
			imshow("FG Mask MOG 2", fgMaskMOG2);

		}
	}
		//delete capture object
	capture.release();
	cv::destroyAllWindows();
}


void backgroundSubtractorProcessor(const int& camera) {
	/*
	 Camera live video feed.
	 */

	int width = 320;
	int height = 240;

		//Image writer PNG parameters.
	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(3);

	imageProcessor imProcessor;


		// Global variables
	cv::Mat frame; //current frame
	cv::Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
	cv::Ptr<cv::BackgroundSubtractor> pMOG2; //MOG2 Background subtractor

		//create GUI windows
	cv::namedWindow("Frame");
	cv::namedWindow("FG Mask MOG 2");
	cv::moveWindow("FG Mask MOG 2", width,50);

		//create a Background Subtractor object
		//BackgroundSubtractorMOG2(int history, float varThreshold, bool bShadowDetection=true)
	pMOG2 = cv::createBackgroundSubtractorMOG2(100,100,false);

		//create the capture object
	cv::VideoCapture capture(camera);

	if(!capture.isOpened()){
			//error in opening the video input
		std::cerr << "Unable to open video file: " << camera << std::endl;
		exit(EXIT_FAILURE);
	}

		//Resizing the camera output.
	capture.set(CV_CAP_PROP_FRAME_WIDTH,width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,height);

	int counter=0;

	while (capture.isOpened()){

		if(counter % 2 == 0 ){			//Regulate the fps
										//read the current frame
			if(!capture.read(frame)) {
				std::cerr << "Unable to read next frame." << std::endl;
				std::cerr << "Exiting..." << std::endl;
				exit(EXIT_FAILURE);
			}

				//update the background model
			pMOG2->apply(frame, fgMaskMOG2);
			imshow("Frame", frame);
			imshow("FG Mask MOG 2", fgMaskMOG2);

			cv::imwrite("/Users/stefanosmitropoulos/Developer/C++ Programming/decisionTreeHumanClassifier /build/Debug/demo_real_time/temp.png",fgMaskMOG2,compression_params);
			classifierLive(fgMaskMOG2);


		}
		counter++;
	}
		//delete capture object
	capture.release();
	cv::destroyAllWindows();
}
