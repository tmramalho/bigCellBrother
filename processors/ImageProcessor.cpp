/*
 * ImageProcessor.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tiago
 */

#include "ImageProcessor.h"

ImageProcessor::ImageProcessor() {

}

ImageProcessor::~ImageProcessor() {

}

cv::Mat ImageProcessor::simplifyImage(cv::Mat &origImage, int blurWindow, int stretchMinVal, int equalize) {
	cv::Mat improvImage;
	Histogram1D h;

	if(origImage.type() != 0) cv::cvtColor( origImage, improvImage, CV_BGR2GRAY );
	else origImage.copyTo(improvImage);

	if (equalize) {
		cv::equalizeHist(improvImage, improvImage);
	} else {
		improvImage = h.stretch(improvImage, stretchMinVal);
	}

	if(blurWindow > 0)
		cv::medianBlur(improvImage, improvImage, blurWindow);

	return improvImage;
}

cv::Mat ImageProcessor::threshold(cv::Mat &origImage, int thresholdVal, bool invert) {
	cv::Mat binaryImage;

	if(invert) {
		cv::threshold(origImage, binaryImage, thresholdVal, WHITE, cv::THRESH_BINARY_INV);
	} else {
		cv::threshold(origImage, binaryImage, thresholdVal, WHITE, cv::THRESH_BINARY);
	}

	return binaryImage;
}

cv::Mat ImageProcessor::adaptiveThreshold(cv::Mat &origImage, int thresholdVal, int adaptiveWindow, bool invert) {
	cv::Mat binaryImage;

	if(invert) {
		cv::adaptiveThreshold(origImage, binaryImage, WHITE, cv::ADAPTIVE_THRESH_GAUSSIAN_C ,
					cv::THRESH_BINARY_INV, adaptiveWindow, thresholdVal);
	} else {
		cv::adaptiveThreshold(origImage, binaryImage, WHITE, cv::ADAPTIVE_THRESH_GAUSSIAN_C ,
					cv::THRESH_BINARY, adaptiveWindow, thresholdVal);
	}

	return binaryImage;
}

cv::Mat ImageProcessor::applyMorphologyOp(cv::Mat &origImage, int operation, int kernelSize) {
	cv::Mat binaryImage;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));

	cv::morphologyEx(origImage, binaryImage, operation, kernel);
	return binaryImage;
}



cv::Mat ImageProcessor::floodBackground(cv::Mat &origImage, int x, int y, int bound) {

	cv::Mat backgroundMask(origImage.rows + 2, origImage.cols + 2, CV_8U, cv::Scalar(BLACK));

	cv::Point2i seedPoint;
	seedPoint.x = x;
	seedPoint.y = y;

	cv::floodFill(origImage, backgroundMask, seedPoint, WHITE, 0,
			cv::Scalar(bound), cv::Scalar(bound), 4 /*| cv::FLOODFILL_FIXED_RANGE*/ | cv::FLOODFILL_MASK_ONLY | (WHITE<<8));

	cv::Mat returnMask(origImage.rows, origImage.cols, CV_8U);

	for( int i = 0; i < origImage.rows; i++ )
			for( int j = 0; j < origImage.cols; j++ )
				returnMask.at<uchar>(i,j) = backgroundMask.at<uchar>(i+1,j+1);

	return returnMask;
}

cv::Mat ImageProcessor::cannyOp(cv::Mat &origImage, int ub, int lb, int windowSize) {
	cv::Mat cannyImage;
	cv::Canny(origImage, cannyImage, ub, lb, windowSize, true);
	return cannyImage;
}

cv::Mat ImageProcessor::invertImage(cv::Mat &origImage) {
	Histogram1D h;
	return h.invert(origImage);
}

cv::Mat ImageProcessor::getHistogram(cv::Mat &origImage) {
	Histogram1D h;
	return h.getHistogramImage(origImage);
}

cv::Mat ImageProcessor::distanceTransform(cv::Mat& origImage) {
	cv::Mat result;
	cv::Mat dists;
	cv::distanceTransform(origImage, dists, CV_DIST_L2, CV_DIST_MASK_PRECISE);
	double max;
	cv::minMaxLoc(dists, NULL, &max);
	cv::convertScaleAbs(dists, result, 255/max);
	return result;
}

cv::Mat ImageProcessor::distanceTransformWithVoronoi(cv::Mat & origImage, cv::Mat &voronoiImage) {
	cv::Mat dists;
	cv::Mat targetImage;
	cv::Mat voronoi(origImage.size(), CV_32SC1, cv::Scalar::all(BLACK));
	cv::bitwise_not(origImage, targetImage);

	//TODO: uncomment this
	//cv::distanceTransform(targetImage, dists, voronoi, CV_DIST_L2, CV_DIST_MASK_5, cv::DIST_LABEL_CCOMP);

	return voronoi;
}

cv::Mat ImageProcessor::erode(cv::Mat& targetImage, int kernelSize) {
	cv::Mat binaryImage;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));

	cv::erode(targetImage, binaryImage, kernel);
	return binaryImage;
}

cv::Mat ImageProcessor::dilate(cv::Mat& targetImage, int kernelSize) {
	cv::Mat binaryImage;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));

	cv::dilate(targetImage, binaryImage, kernel);
	return binaryImage;
}

cv::Mat ImageProcessor::laplacian(cv::Mat& targetImage, int kernelSize) {
	cv::Mat laplace;
	cv::Laplacian(targetImage,laplace,CV_32F,kernelSize);

	double lapmin, lapmax;
	cv::minMaxLoc(laplace,&lapmin,&lapmax);

	double scale = 255/ std::max(-lapmin,lapmax);
	cv::Mat laplaceImage;
	cv::convertScaleAbs(laplace, laplaceImage, scale);

	return laplaceImage;
}

bool ImageProcessor::checkIfEmpty(cv::Mat& origImage) {
	if(origImage.type() != CV_8U) return true;

	int nl = origImage.rows;
	int nc = origImage.cols;

	for (int j = 0; j < nl; j++) {
		uchar* data = origImage.ptr<uchar>(j);
		for (int k = 0; k < nc; k++) {
			if(data[k] != 0) return false;
		}
	}

	return true;
}

cv::Mat ImageProcessor::harrisCorners(cv::Mat & targetImage, int blockSize, double k, double threshold) {
	cv::Mat cornerStrength;
	cv::cornerHarris(targetImage, cornerStrength, blockSize, 3, k);
	cv::Mat harrisCorners;
	cv::threshold(cornerStrength, harrisCorners, threshold, WHITE, cv::THRESH_BINARY);
	return harrisCorners;
}


