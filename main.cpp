#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <stdlib.h>

struct Circles {
    int x;
    int y = 0;
    int r;
    int v;
    cv::Scalar color;
};


void generateMeteor(std::vector<Circles> & meteors, int width){
    cv::RNG rng;
    rng.state = time(NULL);
    Circles meteor;
    meteor.x = rng.uniform(0, width);
    meteor.r = rng.uniform(3, 50);
    meteor.v = rng.uniform(1, 10);
    meteor.color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    meteors.push_back(meteor);
}


int areTouching(Circles user, Circles meteor){
    int distSum = (user.x - meteor.x) * (user.x - meteor.x) +
            (user.y - meteor.y) * (user.y - meteor.y);
    int radSum = (user.r + meteor.r) * (user.r + meteor.r);

    if (distSum < radSum){
        return 1;
    } else {
        return 0;
    }
}


int main( int argc, char** argv )
{
    cv::Mat src = cv::imread("/Users/mac/Documents/img/heart.png");
    cv::Mat heart;
    cv::Mat frameGray;
    cv::resize(src, heart, cv::Size(src.cols * 0.1, src.rows * 0.1));

    int lives = 3;
    int emptyFrameCount = 0;

    std::cout << cv::getBuildInformation();

    cv::VideoCapture cap(0);
    if(!cap.isOpened()) {
        return -1;
    }

    std::vector<Circles> meteors;
    Circles meteor;
    int frameCount = 0;

    for(;;)
    {
        cv::Mat frame;
        cap >> frame;

        if(frame.empty())
        {
            emptyFrameCount ++;
            if(emptyFrameCount > 20) break;
            frame = cv::Mat::zeros(480,640,CV_8UC3);
            cv::waitKey(100);
        }

        cvtColor( frame, frameGray, cv::COLOR_BGR2GRAY );
        GaussianBlur( frameGray, frameGray, cv::Size(9, 9), 2, 2 );
        std::vector<cv::Vec3f> circles;
        HoughCircles( frameGray, circles, cv::HOUGH_GRADIENT, 1, frameGray.rows/32, 240, 70, 5, 100 );
        Circles user;

        for( size_t i = 0; i < circles.size(); i++ )
        {
            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            circle( frame, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
            circle( frame, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );

            user.x = cvRound(circles[i][0]);
            user.y = cvRound(circles[i][1]);
            user.r = cvRound(circles[i][2]);

            for (int j = 0; j < meteors.size(); ++j) {
                if(0 < areTouching(user, meteors[j])) {
                    lives--;
                    meteors.clear();
                }
            }
        }

        if(frameCount % 20 == 0) {
            generateMeteor(meteors, frame.cols);
        }

        for (int j = 0; j < meteors.size(); ++j) {
            meteors[j].y += meteors[j].v;
            cv::circle(frame, cv::Point(meteors[j].x, meteors[j].y), meteors[j].r, meteors[j].color, cv::FILLED);
        }

        heart.copyTo(frame(cv::Rect(20, 20, heart.cols, heart.rows)));
        cv::putText(frame, std::to_string(lives), cv::Point(43, 57), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255), 3);

        imshow("VideoCapture", frame);
        if(cv::waitKey(30) >= 0 || lives <= 0){
            break;
        }
        frameCount++;
    }

    return 0;
}
