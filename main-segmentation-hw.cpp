#include <iostream>
#include <iostream>
#include <string>
#include <fstream>

// Open CV headers
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "bg-estimate.h"

using namespace cv;
using namespace std;

using namespace std;

string currentDirectory;
string filename;

void binarizeShafait(Mat &gray, Mat &binary, int w, double k);
void conComps(const Mat &binary, vector<vector<Point2i> > &blobs, vector<Rect> &blob_rects);
void findLineBoxes(const Mat &greyImage, vector<Rect> &resultantBoxes);
void cropboxes(Mat, vector<Rect>);
void test(string);
void binarizeBG(cv::Mat &gray, cv::Mat &grayBG, cv::Mat &binary);


int main(int argc, char **argv) {
    test(argv[1]);
    return 0;
}


void test(string fname){

    Mat imgRgb;
    Mat imgGray;
    std::vector<cv::Rect> rboxes;
    std::vector<Rect> resultantBoxes;

    imgRgb = imread(fname, CV_LOAD_IMAGE_COLOR);   // Read the file

    string::size_type pAtPath = fname.find_last_of('/');
    string filePath = fname.substr(0, pAtPath + 1);
    string fileFullName = fname.substr(pAtPath + 1);
    string::size_type pAtExt = fileFullName.find_last_of('.');   // Find extension point
    string fileBaseName = fileFullName.substr(0, pAtExt);

    currentDirectory=filePath;
    filename=fileBaseName;

    cvtColor( imgRgb, imgGray, CV_BGR2GRAY );
    findLineBoxes(imgGray, resultantBoxes);

}

void findLineBoxes(const Mat &greyImage, vector<Rect> &resultantBoxes) {

    const float H_GAP_FACTOR = 10 ;

    Mat greyImage2 = greyImage.clone();

    // Convert image to binary
    Mat imgShahrukh;
    Mat temp;

   // binarizeBG(greyImage2,temp, imgShahrukh);

    binarizeShafait(greyImage2, imgShahrukh, 50, 0.30);

//    if (m_debug) {
//        string imgFileName = m_debugImgPrefix + "-00a-binary-shahrukh.png";
//        imwrite(imgFileName, imgShahrukh);
//    }
//
    cv::Mat copy = imgShahrukh.clone();
    copy /= 255;
    copy = 1 - copy;

    vector<vector<Point>> blobs;
    vector<Rect> rects;
    conComps(copy, blobs, rects);
    copy = 1 - copy;
    copy *= 255;
//    return std::max(0,std::min(a.y + a.height, b.y + b.height) - std::max(a.y, b.y)) >0;


    int medianHeight;
    {
        Mat filteredImage=copy.clone();
        Mat element5(5, 5, CV_8U,  Scalar(1));
        erode(filteredImage, filteredImage, element5);
        dilate(filteredImage, filteredImage, element5);
        vector<vector<Point>> blobsFilteredImage;
        vector<Rect> rectsFilteredImage;
        filteredImage /= 255;
        filteredImage= 1 - filteredImage;
        conComps(filteredImage, blobsFilteredImage, rectsFilteredImage);
        filteredImage= 1 - filteredImage;
        filteredImage *= 255;

        std::sort(rectsFilteredImage.begin(), rectsFilteredImage.end(),
                  [&](const Rect &d1, const Rect &d2) -> bool {
                      return d1.height > d2.height;
                  });

        vector<Rect> rectsFilteredImageSmallRemoved;

        Mat filteredImageBrg;
        cvtColor(filteredImage,filteredImageBrg,CV_GRAY2BGR);

        for (auto &i:rectsFilteredImage) {
            if (i.height < 10 || i.width < 3)
                continue;
            rectsFilteredImageSmallRemoved.push_back(i);
            rectangle(filteredImageBrg,i,Scalar(0,0,255),3);
        }

//        if (m_debug) {
//            string imgFileName = m_debugImgPrefix + "-filtered-image.png";
//            imwrite(imgFileName, filteredImageBrg);
//        }


        if (rectsFilteredImage.size() == 0)
            return;

//        if(m_debug) {
//            cout << "Debug: Filtered rects size "<<rectsFilteredImageSmallRemoved.size()<<endl;
//        }

        medianHeight = rectsFilteredImageSmallRemoved[rectsFilteredImageSmallRemoved.size() / 2].height;

    }

    vector<Rect> copiedRects;

    for (auto &i:rects) {
        if (i.height < 10 || i.width < 3)
            continue;
        copiedRects.push_back(i);
    }

//    if(m_debug) {
//        cout<<"Debug: Initially ";
//        cout<<rects.size()<<endl;
//        cout<<"Debug: Finally ";
//        cout<<copiedRects.size()<<endl;
//    }

    std::sort(rects.begin(), rects.end(),
              [&](const Rect &d1, const Rect &d2) -> bool {
                  return d1.height > d2.height;
              });

    if (copiedRects.size() == 0)
        return;

    //int medianHeight = copiedRects[copiedRects.size() / 2].height;

//    if(m_debug)
//        cout<<"Debug: Median height " <<medianHeight<<endl;


    for (auto i:copiedRects) {
        if (i.width > greyImage.cols / 3) {
            continue;
        }
        if (i.height > greyImage.rows / 3) {
            continue;
        }

        for (auto j:copiedRects) {
            if (i.width > greyImage.cols / 3)
                continue;
            if (i.height > greyImage.rows / 3)
                continue;

            bool overlap = max(0, std::min(i.y + i.height, j.y + j.height) - std::max(i.y, j.y)) > 0;
            int distance = j.x - i.x - i.width;
            if (distance >= 0 && distance <= (medianHeight * H_GAP_FACTOR) && overlap) {
                Rect rect(i.x, i.y, j.x + j.width - i.x, i.height);
                rectangle(copy, rect, 0, CV_FILLED, 0, 0);
            }
        }
    }

//   // if (m_debug) {
//        string imgFileName = m_debugImgPrefix + "-00a-rlsa-shahrukh.png";
//        imwrite(imgFileName, copy);
//    //}

    copy /= 255;
    copy = 1 - copy;

    // Clear up stuff before reuse
    blobs.clear();
    rects.clear();

    // Fine connected components again
    conComps(copy, blobs, rects);


    int totalBoundingRectArea = 0;
    int totalBoundingRects = rects.size();

    for (auto i:rects) {
        totalBoundingRectArea += i.area();
    }

    float meanBoundingRectsArea = totalBoundingRectArea / totalBoundingRects;

    resultantBoxes.clear();

    for (auto i:rects) {
        if (i.height > 0.7 * medianHeight && i.height < 4 * medianHeight && i.area() > 0.2*meanBoundingRectsArea) {
          //if(i.height>10){
            resultantBoxes.push_back(i);
        }
    }

    Mat imageTableComponentsDrawn;
    cvtColor((1 - copy) * 255, imageTableComponentsDrawn, COLOR_GRAY2BGR);
    Scalar redColor(0, 0, 255);

    for (auto i:resultantBoxes) {
        rectangle(imageTableComponentsDrawn, i, redColor);
    }

    cropboxes(imgShahrukh, resultantBoxes);


    if (1) {
        string imgFileName = "/Users/talha/Downloads/line-boxes.png";
        imwrite(imgFileName, imageTableComponentsDrawn);
    }

}

void binarizeShafait(Mat &gray, Mat &binary, int w, double k) {
    Mat sum, sumsq;
    gray.copyTo(binary);
    int half_width = w >> 1;
    integral(gray, sum, sumsq, CV_64F);
    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            int x_0 = (i > half_width) ? i - half_width : 0;
            int y_0 = (j > half_width) ? j - half_width : 0;
            int x_1 = (i + half_width >= gray.rows) ? gray.rows - 1 : i + half_width;
            int y_1 = (j + half_width >= gray.cols) ? gray.cols - 1 : j + half_width;
            double area = (x_1 - x_0) * (y_1 - y_0);
            double mean = (sum.at<double>(x_0, y_0) + sum.at<double>(x_1, y_1) - sum.at<double>(x_0, y_1) -
                           sum.at<double>(x_1, y_0)) / area;
            double sq_mean = (sumsq.at<double>(x_0, y_0) + sumsq.at<double>(x_1, y_1) - sumsq.at<double>(x_0, y_1) -
                              sumsq.at<double>(x_1, y_0)) / area;
            double stdev = sqrt(sq_mean - (mean * mean));
            double threshold = mean * (1 + k * ((stdev / 128) - 1));
            if (gray.at<uchar>(i, j) > threshold)
                binary.at<uchar>(i, j) = 255;
            else
                binary.at<uchar>(i, j) = 0;
        }
    }
}


void conComps(const Mat &binary, vector<vector<Point2i> > &blobs, vector<Rect> &blob_rects) {
    blobs.clear();
    blob_rects.clear();
    // Fill the label_image with the blobs
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground

    cv::Mat label_image;
    binary.convertTo(label_image, CV_32FC1); // weird it doesn't support CV_32S!

    int label_count = 2; // starts at 2 because 0,1 are used already

    for (int y = 0; y < binary.rows; y++) {
        for (int x = 0; x < binary.cols; x++) {
            if ((int) label_image.at<float>(y, x) != 1) {
                continue;
            }

            cv::Rect rect;
            cv::floodFill(label_image, cv::Point(x, y), cv::Scalar(label_count), &rect, cv::Scalar(0), cv::Scalar(0),
                          4);

            std::vector<cv::Point2i> blob;

            for (int i = rect.y; i < (rect.y + rect.height); i++) {
                for (int j = rect.x; j < (rect.x + rect.width); j++) {
                    if ((int) label_image.at<float>(i, j) != label_count) {
                        continue;
                    }

                    blob.push_back(cv::Point2i(j, i));
                }
            }
            blobs.push_back(blob);

            label_count++;
        }
    }

    for (size_t i = 0; i < blobs.size(); i++) {
        int top = 10000, bottom = -1, left = 10000, right = -1;
        for (size_t j = 0; j < blobs[i].size(); j++) {
            int x = blobs[i][j].x;
            int y = blobs[i][j].y;
            if (x < left)
                left = x;
            if (x > right)
                right = x;
            if (y < top)
                top = y;
            if (y > bottom)
                bottom = y;
        }
        int w = right - left;
        int h = bottom - top;
        blob_rects.push_back(Rect(left, top, w, h));
    }
}

void cropboxes(Mat imgBin, vector<Rect> lboxes){

    string newDirectory=currentDirectory+filename;
    string command="mkdir -p " + newDirectory;
    system(command.c_str());

    Mat cropped;

    for(int i=0; i<lboxes.size(); i++) {

        imgBin(lboxes[i]).copyTo(cropped);

        int top = (int) (0.1*cropped.rows);int  bottom = (int) (0.1*cropped.rows);
        int left = (int) (0.01*cropped.cols);int  right = (int) (0.01*cropped.cols);
        Mat outframe = cropped;

        Scalar value;
        value=Scalar(255,255,255);
        copyMakeBorder( cropped, outframe, top, bottom, left, right, BORDER_CONSTANT,value );

        imwrite(newDirectory+"/line-"+to_string(i)+".png", outframe);

    //    imwrite(newDirectory+"/line-"+to_string(i)+".png", cropped);

    }

}

void binarizeBG(cv::Mat &gray, cv::Mat &grayBG, cv::Mat &binary) {
    grayBG = gray.clone();

    // Estimate image background using Percentile filter
    CBgEstimate bg;
    bg.bgEstimatePercentile(gray, grayBG);
    bg.binarizeByBackgroundOtsu(gray, grayBG, binary);
}
