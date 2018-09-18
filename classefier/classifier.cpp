#include <string>
#include "Vision.h"
#include <opencv2/core.hpp>
#include <vector>

using namespace std;
using namespace cv;

void createPicture(string &nomeImg, Mat &imgNum, Vision &vision){
    Mat visual;
    cout << nomeImg << endl;
    vision.setEstado(1);

    while (1){
        vision.calculateTagCenter();
        
        vision.show();

        imgNum.at<uchar>((vision.getCenter().y) / 20, (vision.getCenter().x) / 20) = 255;
        flip(imgNum, visual, +1);
        imshow("saida", visual);        

        if (waitKey(1) == 27){
            vision.setEstado(2);
            cout << "Salvou!" << endl;
            cout << nomeImg << endl;
            //flip(imgNum, imgNum, +1);
            imwrite(nomeImg, visual);
            break;
        }
    }
}

int main(int argc, char **argv){
    string nomeImg = "saida.jpg";
    Mat imgNum(28, 28, CV_8UC1, 1);

    Vision vision(argc, argv);

    while (1){
        vision.calculateTagCenter();

        if (vision.isTargetOn()){
            vision.show();

            switch (waitKey(1)){
                case 110://n
                    imgNum = Mat::zeros(28,28,CV_8UC1);
                    createPicture(nomeImg, imgNum, vision);
                    break;
                case 104://h
                    vision.setEstado(4);
            }
        }
    }

    return 0;
}