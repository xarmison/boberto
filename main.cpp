#include <fstream>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include "Vision.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <stdio.h>
#include <vector>

using namespace std;

void createFota(vector<string> &folders, int &contFolder, string &nomeImg, cv::Mat &imgNum, Vision &vision, string &auxExt, int &cont){
    std::string nome = "data/" + folders[contFolder] + nomeImg + std::to_string(cont) + auxExt;
    std::cout << nome << std::endl;
    vision.setEstado(1);

    while (1)
    {
        vision.calculateTagCenter();
        //std::cout << "mostrou";
        vision.show();

        imgNum.at<uchar>((vision.getCenter().y) / 20, (vision.getCenter().x) / 20) = 255;

        if (cv::waitKey(1) == 27)
        {
            vision.setEstado(2);
            std::cout << "Salvou!" << std::endl;
            cout << nome << endl;
            cv::flip(imgNum, imgNum, +1);
            cv::imwrite(nome, imgNum);
            cont++;
            break;
        }
    }
}

int main(int argc, char **argv){
    std::string nomeImg = "imagemNumero", auxExt = ".jpg";
    std::vector<std::string> folders = {"0/", "1/", "2/", "3/", "4/", "5/", "6/", "7/", "8/", "9/"};
    int cont = 0, contFolder = 0;
    cv::Mat imgNum(28, 28, CV_8UC1, 1);

    Vision vision(argc, argv);

    while (1)
    {
        vision.calculateTagCenter();

        //vision.show();

        if (vision.isTargetOn())
        {
            vision.show();

            switch (cv::waitKey(1)){
                case 110:
                    for(int i = 0; i < 28; i++){
                        for(int j = 0; j < 28; j++){
                            imgNum.at<uchar>(i,j) = 0;
                        }
                    }
                    
                    createFota(folders, contFolder, nomeImg, imgNum, vision, auxExt, cont);
                    break;
                case 112:
                    contFolder++;
                    cout << folders[contFolder] << endl;
                    cont = 0;
                    break;
                case 97:
                    contFolder--;
                    cout << folders[contFolder] << endl;
                    break;
            }
        }
    }

    return 0;
}