#include "Vision.h"

void Vision::cvTackBarEvents(int pos, void*){
    (void)(pos);


    setParamsFromGlobalVariables();

        Fps.start();
        MDetector.detect(TheInputImage, TheMarkers, TheCameraParameters);
        Fps.stop();
    TheInputImage.copyTo(TheInputImageCopy);
    if (iShowAllCandidates){
        auto candidates=MDetector.getCandidates();
        for(auto cand:candidates)
            aruco::Marker(cand,-1).draw(TheInputImageCopy, cv::Scalar(255, 0, 255),1);
    }

    for (unsigned int i = 0; i < TheMarkers.size(); i++){
        std::cout << TheMarkers[i] << std::endl;
        TheMarkers[i].draw(TheInputImageCopy, cv::Scalar(0, 0, 255),2);
    }

    if (TheCameraParameters.isValid())
        for (unsigned int i = 0; i < TheMarkers.size(); i++)
            aruco::CvDrawingUtils::draw3dCube(TheInputImageCopy, TheMarkers[i], TheCameraParameters);
    cv::imshow("in",  TheInputImageCopy );
}

cv::Mat Vision::resize(const cv::Mat& in, int width){
    if (in.size().width <= width)
        return in;
    float yf = float(width) / float(in.size().width);
    cv::Mat im2;
    cv::resize(in, im2, cv::Size(width, static_cast<int>(in.size().height * yf)));
    return im2;
}

void Vision::setParamsFromGlobalVariables(){
    MDetector.setDetectionMode((aruco::DetectionMode)iDetectMode,float(iMinMarkerSize)/1000.);
    MDetector.getParameters().setCornerRefinementMethod( (aruco::CornerRefinementMethod) iCornerMode);

    MDetector.getParameters().detectEnclosedMarkers(iEnclosed);
    MDetector.getParameters().ThresHold=iThreshold;
    if ( aruco::Dictionary::getTypeFromString( MDetector.getParameters().dictionary)!=aruco::Dictionary::CUSTOM)
    MDetector.setDictionary((aruco::Dictionary::DICT_TYPES) iDictionaryIndex,float(iCorrectionRate)/10. );
}

cv::Mat Vision::resizeImage(cv::Mat &in,float resizeFactor){
    if (fabs(1-resizeFactor)<1e-3 )return in;
    float nc=float(in.cols)*resizeFactor;
    float nr=float(in.rows)*resizeFactor;
    cv::Mat imres;
    cv::resize(in,imres,cv::Size(nc,nr));
    std::cout<<"Imagesize="<<imres.size()<<std::endl;
    return imres;
}

void Vision::record(std::string filename){
    int f_width = TheVideoCapturer.get(CV_CAP_PROP_FRAME_WIDTH); 
    int f_height = TheVideoCapturer.get(CV_CAP_PROP_FRAME_HEIGHT); 

    video.open(filename,CV_FOURCC('M','J','P','G'),10, cv::Size(f_width,f_height));

}

Vision::Vision(int argc, char **argv){
    target_on = false;

    CmdLineParser cml(argc, argv);
    if (argc < 2 || cml["-h"]){
            std::cerr << "Invalid number of arguments" << std::endl;
            std::cerr << "Usage: (in.avi|live[:camera_index(e.g 0 or 1)]) [-c camera_params.yml] [-s  marker_size_in_meters] [-d "
                    "dictionary:ALL_DICTS by default] [-h]"
                 << std::endl;
            std::cerr << "\tDictionaries: ";
            for (auto dict : aruco::Dictionary::getDicTypes())
                std::cerr << dict << " ";
            std::cerr << std::endl;
            std::cerr << "\t Instead of these, you can directly indicate the path to a file with your own generated "
                    "dictionary"
                 << std::endl;
            //return false;
    }

    std::string TheInputVideo = argv[1];
    if (cml["-c"])
        TheCameraParameters.readFromXMLFile(cml("-c"));

    TheMarkerSize = std::stof(cml("-s", "-1"));
    resizeFactor=stof(cml("-rf","1"));

    if (TheInputVideo.find("live") != std::string::npos){
        /* 0 -> WEBCAM
           1 -> CAMERA EXTERNA */
        int vIdx = 0;
        char cad[100];
        if (TheInputVideo.find(":") != std::string::npos){
            std::replace(TheInputVideo.begin(), TheInputVideo.end(), ':', ' ');
            sscanf(TheInputVideo.c_str(), "%s %d", cad, &vIdx);
        }

        std::cout << "Opening camera index " << vIdx << std::endl;
        TheVideoCapturer.open(vIdx);
        waitTime = 10;
        isVideo=true;
    } else{
        TheVideoCapturer.open(TheInputVideo);
        if ( TheVideoCapturer.get(CV_CAP_PROP_FRAME_COUNT)>=2) isVideo=true;
    }

    if (!TheVideoCapturer.isOpened())
        throw std::runtime_error("Could not open video");

    TheVideoCapturer >> TheInputImage;
    if (TheCameraParameters.isValid())
        TheCameraParameters.resize(TheInputImage.size());
    
    dictionaryString=cml("-d", "ALL_DICTS");
    iDictionaryIndex=(uint64_t)aruco::Dictionary::getTypeFromString(dictionaryString);
    MDetector.setDictionary(dictionaryString,float(iCorrectionRate)/10. );  // sets the dictionary to be employed (ARUCO,APRILTAGS,ARTOOLKIT,etc)
    iThreshold=MDetector.getParameters().ThresHold;
    iCornerMode= MDetector.getParameters().cornerRefinementM;

    cv::namedWindow("in",cv::WINDOW_NORMAL);
    cv::resizeWindow("in",560,560);

    setParamsFromGlobalVariables();

    {
        float w=std::min(int(1920),int(TheInputImage.cols));
        float f=w/float(TheInputImage.cols);
        cv::resizeWindow("in",w,float(TheInputImage.rows)*f);
    }
}

Vision::~Vision(){

}

void Vision::saveMovement(std::string file){
    save_points.open(file);
}

void Vision::endSaving(){
    save_points.close();
}

void Vision::savePoint(cv::Point pos){
    save_points << pos.x << "," << pos.y << "\n";
    save_points.flush();
}

void Vision::calculateTagCenter(){
        TheVideoCapturer.retrieve(TheInputImage);
        TheInputImage=resizeImage(TheInputImage,resizeFactor);
        
        Fps.start();
        TheMarkers = MDetector.detect(TheInputImage, TheCameraParameters, TheMarkerSize);
        Fps.stop();
    
        TheInputImage.copyTo(TheInputImageCopy);

        if (iShowAllCandidates){
            auto candidates=MDetector.getCandidates();
            for(auto cand:candidates)
                aruco::Marker(cand,-1).draw(TheInputImageCopy, cv::Scalar(255, 0, 255));
        }

        for (unsigned int i = 0; i < TheMarkers.size(); i++){
            target_on = true;
            if (TheMarkers[i].id == 213) {
                TheMarkers[i].draw(TheInputImageCopy, cv::Scalar(0, 0, 255),2,true);
                center.x = TheMarkers[i].getCenter().x;
                center.y = TheMarkers[i].getCenter().y;
            }            
        }

        if (TheCameraParameters.isValid() && TheMarkerSize > 0)
            for (unsigned int i = 0; i < TheMarkers.size(); i++){
                aruco::CvDrawingUtils::draw3dCube(TheInputImageCopy, TheMarkers[i], TheCameraParameters);
                aruco::CvDrawingUtils::draw3dAxis(TheInputImageCopy, TheMarkers[i], TheCameraParameters);
        }

        key = cv::waitKey(waitTime);  // wait for key to be pressed        

        index++;  // number of images captured

        if (isVideo)
            if ( TheVideoCapturer.grab()==false) key=27;
}

void Vision::drawError(cv::Point pos, cv::Point error){
    arrowedLine(TheInputImageCopy, pos, error, cv::Scalar(0, 0, 255), 5, 8, 0, 0.5);
}