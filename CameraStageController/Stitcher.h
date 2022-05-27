#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <filesystem>
//namespace fs = std::filesystem;

class Stitcher
{
public:
	Stitcher();
	~Stitcher();

public:
	void loadImages(const char directory[100]); //Load images to stitch
    bool stitchImages(bool mask_enable = false); //Stitch the loaded images. Will call clearLoadedImages after stitching
	void saveImage(const char name[100] = ""); //Save the latest stitched image to the loaded images folder. Default name is result.JPG.
    void clearLoadedImages();
    bool isReady();

private:
	cv::Mat homographyCompute(cv::Mat img1, cv::Mat img2, bool mask_enable);
	cv::Rect getBounds(cv::Mat img);
	cv::Mat removeBorder(cv::Mat img);
	cv::Mat center_weighting_blend(cv::Mat img1, cv::Mat img2);

public:
	int GRID_X = 0;
	int GRID_Y = 0;

private:
	std::vector<cv::Mat> images;
	std::vector<cv::Mat> images_gray;
	std::string path;
	cv::Mat stitchedImage;

    bool stitchingReady = false;
};

