#ifndef Vision_h
#define Vision_h

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include <fstream>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <string>
#include <stdexcept>

class CmdLineParser
{
    int argc;
    char **argv;

  public:
    CmdLineParser(int _argc, char **_argv) : argc(_argc), argv(_argv) {}
    bool operator[](std::string param)
    {
        int idx = -1;
        for (int i = 0; i < argc && idx == -1; i++)
            if (std::string(argv[i]) == param)
                idx = i;
        return (idx != -1);
    }
    std::string operator()(std::string param, std::string defvalue = "-1")
    {
        int idx = -1;
        for (int i = 0; i < argc && idx == -1; i++)
            if (std::string(argv[i]) == param)
                idx = i;
        if (idx == -1)
            return defvalue;
        else
            return (argv[idx + 1]);
    }
};
struct TimerAvrg
{
    std::vector<double> times;
    size_t curr = 0, n;
    std::chrono::high_resolution_clock::time_point begin, end;
    TimerAvrg(int _n = 30)
    {
        n = _n;
        times.reserve(n);
    }
    inline void start() { begin = std::chrono::high_resolution_clock::now(); }
    inline void stop()
    {
        end = std::chrono::high_resolution_clock::now();
        double duration = double(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) * 1e-6;
        if (times.size() < n)
            times.push_back(duration);
        else
        {
            times[curr] = duration;
            curr++;
            if (curr >= times.size())
                curr = 0;
        }
    }
    double getAvrg()
    {
        double sum = 0;
        for (auto t : times)
            sum += t;
        return sum / double(times.size());
    }
};

class Vision
{
  private:
    aruco::MarkerDetector MDetector;
    cv::VideoCapture TheVideoCapturer;
    std::vector<aruco::Marker> TheMarkers;
    cv::Mat TheInputImage, TheInputImageGrey, TheInputImageCopy;
    aruco::CameraParameters TheCameraParameters;
    std::string dictionaryString;
    int iDetectMode = 0, iMinMarkerSize = 0, iCorrectionRate = 0, iShowAllCandidates = 0, iEnclosed = 0, iThreshold, iCornerMode, iDictionaryIndex = 0;
    int waitTime = 0;
    bool showMennu = false, bPrintHelp = false, isVideo = false;
    char key = 0;
    int index = 0, indexSave = 0;
    TimerAvrg Fps;
    float TheMarkerSize;
    float resizeFactor;

    cv::Point center;
    bool target_on;
    cv::VideoWriter video;

    std::ofstream save_points;

    void setParamsFromGlobalVariables();
    cv::Mat resize(const cv::Mat &in, int width);
    void createMenu();
    void putText(cv::Mat &im, std::string text, cv::Point p, float size);
    void printHelp(cv::Mat &im);
    void printInfo(cv::Mat &im);
    void printMenuInfo();
    cv::Mat resizeImage(cv::Mat &in, float resizeFactor);
    void cvTackBarEvents(int pos, void *);
    bool iniciou = false;
    int setTexto = 0;
    std::string text, textMenu, textMenu1, textMenu2;

  public:
    Vision(int argc, char **argv);
    ~Vision();
    //bool start();
    inline const cv::Point &getCenter() const { return center; }
    void calculateTagCenter();
    inline void saveVideo() { video.write(TheInputImage); }
    inline bool isTargetOn() { return target_on; }

    void drawError(cv::Point pos, cv::Point error);
    void show()
    {
        flip(TheInputImageCopy, TheInputImageCopy, 1);
        cv::Point posText, posTextMenu, posTextMenu1, posTextMenu2;
        posText.x = 20, posText.y = 30;
        posTextMenu.x = 20, posTextMenu.y = 70;
        posTextMenu1.x = 20, posTextMenu1.y = 110;
        posTextMenu2.x = 20, posTextMenu2.y = 150;
        cv::putText(TheInputImageCopy, text, posText, 3, 1, cv::Scalar(0, 0, 255), 1, 8);
        cv::putText(TheInputImageCopy, textMenu, posTextMenu, 3, 1, cv::Scalar(0, 0, 255), 1, 8);
        cv::putText(TheInputImageCopy, textMenu1, posTextMenu1, 3, 1, cv::Scalar(0, 0, 255), 1, 8);
        cv::putText(TheInputImageCopy, textMenu2, posTextMenu2, 3, 1, cv::Scalar(0, 0, 255), 1, 8);
        imshow("in", TheInputImageCopy);
    }

    void record(std::string filename);
    void setIniciou(bool _iniciou = false) { iniciou = _iniciou; }
    void setEstado(int _setTexto)
    {
        setTexto = _setTexto;
        switch (setTexto)
        {
        case 1:
            text = "Iniciou!";
            break;
        case 2:
            text = "Terminou!";
            break;
        case 3:
            text = "Enviou o Programa!";
            break;
        case 4:
            textMenu =  " n  - Nova Imagem";
            textMenu1 = " p  - Proximo Numero";
            textMenu2 = "Esc - Sair";
            text =      " h  - Ajuda";
        default:
            break;
        }
    }
    void saveMovement(std::string file);
    void savePoint(cv::Point pos);
    void endSaving();
};

#endif