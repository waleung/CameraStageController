#include "Stitcher.h"

Stitcher::Stitcher() {}
Stitcher::~Stitcher() {}

void Stitcher::loadImages(const char directory[100])
{
    path = directory;
    std::string ext = ".JPG";
    std::string pathfile;

    if (!path.empty())
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (entry.path().extension() == ext)
            {
                std::cout << "Loading image: " << entry.path() << std::endl;
                cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_COLOR);

                if ((img.size().height < 2) || (img.size().width < 2))
                {
                    std::cout << "Image not loaded, heigth or width is too small" << std::endl;
                    continue;
                }

                cv::resize(img, img, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR_EXACT); //Resize to half resolution
                images.push_back(img);
                cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
                images_gray.push_back(img);

            }
        }
    }


    if (images.empty() || images_gray.empty())
    {
        std::cout << "No images were loaded. Aborting stitching" << std::endl;
        stitchingReady = false;
    }
    else
        stitchingReady = true;
}

bool Stitcher::stitchImages(bool mask_enable)
{
    std::vector<cv::Mat> HMatrix;
    cv::Mat img;
    cv::Mat M, H, affineH, img_warp;
    cv::Mat row = (cv::Mat_<double>(1, 3) << 0, 0, 1);
    double det;

    //cv::copyMakeBorder(images.at(0), img, 0, (GRID_Y - 1) * images_gray.at(0).size().height, 0, (GRID_X - 1) * images_gray.at(0).size().width, cv::BORDER_CONSTANT); //Make a image large enough for the pano
    cv::copyMakeBorder(images.at(0), img, 0, 32768, 0, 32768, cv::BORDER_CONSTANT);

    for (int i = 0; i < (images_gray.size() - 1); ++i)
    {
        std::cout << "Calculating homography " << i + 1 << " -> " << i << std::endl;

        M = homographyCompute(images_gray.at(i), images_gray.at(i + 1), mask_enable);

        M.push_back(row); //3x2 Matrix -> 3x3 Matrix

        det = cv::determinant(M);

        std::cout << "Determinant of matrix: " << det << std::endl;

        if ((det < 0.9) || (det > 1.1))
        {
            std::cout << "Bad Homography" << std::endl;
            break;
        }

        HMatrix.push_back(M);
    }

    if (HMatrix.empty())
    {
        std::cout << "No good matches were found. Aborting stitching" << std::endl;
        return false;
    }

    H = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
    affineH = (cv::Mat_<double>(2, 3) << 0, 0, 0, 0, 0, 0);
    for (int i = 0; i < HMatrix.size(); ++i)
    {
        std::cout << "Stitching image: " << i + 1 << " -> " << i << std::endl;

        H = H * HMatrix.at(i);

        H.row(0).copyTo(affineH.row(0)); //3x3 Matrix -> 3x2 Matrix
        H.row(1).copyTo(affineH.row(1));

        cv::warpAffine(images.at(i + 1), img_warp, affineH, img.size());

        img = center_weighting_blend(img, img_warp);
    }

    img = removeBorder(img);

    std::cout << "Done!" << std::endl;

    stitchedImage.release();
    stitchedImage = img;

    clearLoadedImages();

    //cv::namedWindow("Display", cv::WINDOW_NORMAL);
    //cv::imshow("Display", img);
    //cv::imwrite(path + "result.JPG", img, { cv::IMWRITE_JPEG_QUALITY , 100 });
    //int k = cv::waitKey();

   return true;
}

void Stitcher::saveImage(const char name[100])
{
    std::string savePath;

    if (!path.empty() && !stitchedImage.empty())
    {
        if (strlen(name) == 0)
            savePath = path + "/result.JPG";
        else
            savePath = name;
            
        std::cout << "Saving stitched image to: " << savePath << std::endl;
        cv::imwrite(savePath, stitchedImage, {cv::IMWRITE_JPEG_QUALITY , 100});

        //Clear image and path
        savePath.clear();
        path.clear();
        stitchedImage.release();
    }
}

void Stitcher::clearLoadedImages()
{
    images.clear();
    images_gray.clear();
}

cv::Mat Stitcher::homographyCompute(cv::Mat img1, cv::Mat img2, bool mask_enable)
{
    cv::Ptr<cv::SIFT> detector;
    std::vector<cv::KeyPoint> kp1, kp2;
    cv::Mat dp1, dp2;
    cv::Ptr<cv::DescriptorMatcher> matcher;
    std::vector<std::vector<cv::DMatch>> knn_matches;
    double ratio_thresh;
    std::vector<cv::DMatch> good;
    std::vector<cv::Point2f> src_pts, dst_pts;
    cv::Mat M, inliers, mask, mask1, mask2;

//--Step 1: Detect the keypoints using SIFT Detector, compute the descriptors
    detector = cv::SIFT::create();

//---- Step 1.1: Make mask. Assume left to right. Ex. 1 -> 2
    if (mask_enable)
    {
        mask1 = cv::Mat::zeros(img1.size(), CV_8UC1);
        mask2 = cv::Mat::zeros(img2.size(), CV_8UC1);

        mask1(cv::Rect((int)img1.size().width * 0.85, 0, (int)img1.size().width * 0.15, img1.size().height)) = 255;
        mask2(cv::Rect(0, 0, (int)img2.size().width * 0.15, img2.size().height)) = 255;

        detector->detect(img1, kp1, mask1);
        detector->detect(img2, kp2, mask2);
    }
    else
    {
        detector->detect(img1, kp1);
        detector->detect(img2, kp2);
    }

    detector->compute(img1, kp1, dp1);
    detector->compute(img2, kp2, dp2);

    if (dp1.empty() || dp2.empty())
        return (cv::Mat_<double>(2,3) << 0, 0, 0, 0, 0, 0);

//-- Step 2: Matching descriptor vectors with a FLANN based matcher
    matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    matcher->knnMatch(dp2, dp1, knn_matches, 2);

//-- Filter matches using the Lowe's ratio test
    ratio_thresh = 0.7;
    for (int i = 0; i < knn_matches.size(); ++i)
    {
        if (knn_matches.at(i).at(0).distance < ratio_thresh * knn_matches.at(i).at(1).distance)
            good.push_back(knn_matches.at(i).at(0));
    }

    if (good.size() > 10)
    {
        for (int i = 0; i < good.size(); ++i)
        {
            src_pts.push_back(kp2.at(good.at(i).queryIdx).pt);
            dst_pts.push_back(kp1.at(good.at(i).trainIdx).pt);
        }

        M = cv::estimateAffine2D(src_pts, dst_pts, inliers, cv::RANSAC, 3, 1000000, 0.9999);

        return M;
    }

    else
        return (cv::Mat_<double>(2,3) << 0, 0, 0, 0, 0, 0);
}

cv::Rect Stitcher::getBounds(cv::Mat img)
{
    cv::Mat gray, thresh;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thresh, 1, 255, cv::THRESH_BINARY);

    return cv::boundingRect(thresh);
}

cv::Mat Stitcher::removeBorder(cv::Mat img)
{
    cv::Rect bounds;
    cv::Mat img_no_border;

    bounds = getBounds(img);
    img_no_border = img(bounds);

    return img_no_border;
}

cv::Mat Stitcher::center_weighting_blend(cv::Mat img1, cv::Mat img2)
{
    cv::Rect bounds;
    cv::Mat img1_sub, img2_sub, bw1, dist1, bw2, dist2, alpha, blend1, blend2, blended, img1_blend, img2_blend, alpha_blend;
    bounds = getBounds(img2);

    img1_sub = img1(cv::Rect(0, 0, bounds.x + bounds.width, bounds.y + bounds.height));
    img2_sub = img2(cv::Rect(0, 0, bounds.x + bounds.width, bounds.y + bounds.height));

    cv::cvtColor(img1_sub, bw1, cv::COLOR_BGR2GRAY);
    cv::threshold(bw1, bw1, 1, 255, cv::THRESH_BINARY);
    cv::distanceTransform(bw1, dist1, cv::DIST_L2, 3);
    cv::normalize(dist1, dist1, 0, 1, cv::NORM_MINMAX);

    cv::cvtColor(img2_sub, bw2, cv::COLOR_BGR2GRAY);
    cv::threshold(bw2, bw2, 1, 255, cv::THRESH_BINARY);
    cv::distanceTransform(bw2, dist2, cv::DIST_L2, 3);
    cv::normalize(dist2, dist2, 0, 1, cv::NORM_MINMAX);

    alpha = dist2 / (dist1 + dist2);

    cv::Mat alpha_merge[3] = {alpha, alpha, alpha};
    cv::merge(alpha_merge, 3, alpha);

    img1_blend = img1(bounds);
    img2_blend = img2(bounds);
    alpha_blend = alpha(bounds);

    //result = img1_blend * (1 - alpha_blend) + img2_blend * alpha_blend;
    cv::multiply(img2_blend, alpha_blend, blend1, 1 , CV_32FC3);
    cv::multiply(img1_blend, (cv::Scalar(1.0, 1.0, 1.0) - alpha_blend), blend2, 1, CV_32FC3);
    cv::add(blend1, blend2, blended);

    blended.convertTo(blended, CV_8UC3, 1);

    blended.copyTo(img1(bounds));

    return img1;
}

bool Stitcher::isReady()
{
    return stitchingReady;
}
