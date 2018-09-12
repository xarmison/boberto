#include <string>
#include "Vision.h"
#include <opencv2/core.hpp>
#include <vector>

using namespace std;
using namespace cv;

void createPicture(vector<string> &folders, int &contFolder, string &nomeImg, Mat &imgNum, Vision &vision, string &auxExt, int &cont){
    string nome = "../dataset_num_camera/" + folders[contFolder] + nomeImg + to_string(cont) + auxExt;
    cout << nome << endl;
    vision.setEstado(1);

    while (1){
        vision.calculateTagCenter();
        
        vision.show();

        imgNum.at<uchar>((vision.getCenter().y) / 20, (vision.getCenter().x) / 20) = 255;

        if (waitKey(1) == 27){
            vision.setEstado(2);
            cout << "Salvou!" << endl;
            cout << nome << endl;
            flip(imgNum, imgNum, +1);
            imwrite(nome, imgNum);
            cont++;
            break;
        }
    }
}

int main(int argc, char **argv){
    string nomeImg = "imagemNumero", auxExt = ".jpg";
    vector<string> folders = {"0/", "1/", "2/", "3/", "4/", "5/", "6/", "7/", "8/", "9/"};
    int cont = 0, contFolder = 0;
    Mat imgNum(28, 28, CV_8UC1, 1);

    Vision vision(argc, argv);

    while (1){
        vision.calculateTagCenter();

        if (vision.isTargetOn()){
            vision.show();

            switch (waitKey(1)){
                case 110:
                    imgNum = Mat::zeros(28,28,CV_8UC1);
                    createPicture(folders, contFolder, nomeImg, imgNum, vision, auxExt, cont);
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