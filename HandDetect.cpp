#include "HandDetect.h"



HandDetect::HandDetect(double nearThreshold, double farThreshold)
{
	_nearThreshold = nearThreshold;
	_farThreshold = farThreshold;
}

std::vector<cv::Point> HandDetect::getTipData(cv::Mat depth, cv::Mat color)
{
	std::vector<cv::Point> tipPositions;

	CvSize sz = cvSize(color.cols, color.rows);

	depthBinaryImage = cv::Mat::zeros(cv::Size(depth.cols, depth.rows), CV_8U);
	depthImage = depth;
	colorImage = color;


	depthBinaryImage = getBinaryImage();

	//cv::imshow("binary", depthBinaryImage);


	IplImage* src = cvCreateImage(sz, IPL_DEPTH_8U, 3);
	//IplImage* gray = cvCreateImage(cvSize(270, 270), 8, 1);
	IplImage* gray = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	//*src = image;//このコードだったらメモリが増えるから下のコードで
	for (int y = 0; y < color.rows; y++)
	{
		cv::Vec3b *colorPtr = color.ptr<cv::Vec3b>(y);
		uchar *depthPtr = depthBinaryImage.ptr<uchar>(y);
		for (int x = 0; x < color.cols; x++)
		{
			for (int i = 0; i < 3; i++)
			{
				src->imageData[src->widthStep * y + x * 3 + i] = colorPtr[x][i];
			}
			gray->imageData[gray->widthStep*y + x] = depthPtr[x];
		}
	}

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;
	CvSeq* maxitem = NULL;
	int cn = cvFindContours(gray, storage, &first_contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	double area, max_area = 0.0;
	CvSeq* ptr = 0;
	//int maxn=0,n=0;

	if (cn > 0)
	{
		for (ptr = first_contour; ptr != NULL; ptr = ptr->h_next)
		{
			area = fabs(cvContourArea(ptr, CV_WHOLE_SEQ, 0));
			if (area > max_area)
			{
				max_area = area;
				maxitem = ptr;
				//maxn=n;
			}
			// n++;
		}
		if (max_area > 1000)
		{
			CvPoint pt0;
			CvMemStorage* storage1 = cvCreateMemStorage();
			CvMemStorage* storage2 = cvCreateMemStorage(0);
			CvSeq* ptseq = cvCreateSeq(CV_SEQ_KIND_GENERIC | CV_32SC2, sizeof(CvContour), sizeof(CvPoint), storage1);
			CvSeq* hull;
			CvSeq* defects;
			for (int i = 0; i < maxitem->total; i++)
			{
				CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, maxitem, i);
				pt0.x = p->x;
				pt0.y = p->y;
				cvSeqPush(ptseq, &pt0);
			}
			hull = cvConvexHull2(ptseq, 0, CV_CLOCKWISE, 0);
			int hullcount = hull->total;
			defects = cvConvexityDefects(ptseq, hull, storage2);
			// pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hullcount - 1 );
			// printf("** : %d :**",hullcount);
			CvConvexityDefect* defectArray;
			// int j=0;
			for (int i = 1; i <= hullcount; i++)
			{
				CvPoint pt = **CV_GET_SEQ_ELEM(CvPoint*, hull, i);
				cvLine(src, pt0, pt, CV_RGB(255, 0, 0), 1, CV_AA, 0);
				pt0 = pt;
			}
			for (; defects; defects = defects->h_next)
			{
				int nomdef = defects->total; // defect amount
				// outlet_float( m_nomdef, nomdef );
				// printf(" defect no %d \n",nomdef);
				if (nomdef == 0)
					continue;
				// Alloc memory for defect set.
				// fprintf(stderr,"malloc\n");
				defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*nomdef);
				// Get defect set.
				// fprintf(stderr,"cvCvtSeqToArray\n");
				cvCvtSeqToArray(defects, defectArray, CV_WHOLE_SEQ);
				// Draw marks for all defects.
				int con = 0;
				for (int i = 0; i < nomdef; i++)
				{
					if (defectArray[i].depth > 40)
					{
						con = con + 1;
						// printf(" defect depth for defect %d %f \n",i,defectArray[i].depth);
						cvLine(src, *(defectArray[i].start), *(defectArray[i].depth_point), CV_RGB(255, 255, 0), 1, CV_AA, 0);
						cvCircle(src, *(defectArray[i].depth_point), 5, CV_RGB(0, 0, 255), 2, 8, 0);
						cvCircle(src, *(defectArray[i].start), 5, CV_RGB(0, 255, 0), 2, 8, 0);
						cvLine(src, *(defectArray[i].depth_point), *(defectArray[i].end), CV_RGB(0, 255, 255), 1, CV_AA, 0);
						cvDrawContours(src, defects, CV_RGB(0, 0, 0), CV_RGB(255, 0, 0), -1, CV_FILLED, 8);
						cv::Point temp = cv::Point((*(defectArray[i].start)).x, (*(defectArray[i].start)).y);

						tipPositions.push_back(cv::Point((*(defectArray[i].start)).x, (*(defectArray[i].start)).y));

						tipPositions.push_back(cv::Point((*(defectArray[i].depth_point)).x, (*(defectArray[i].depth_point)).y));
						if (i > 0 && (*(defectArray[i - 1].start)).x != (*(defectArray[i].end)).x&& (*(defectArray[i - 1].start)).y != (*(defectArray[i].end)).y)
						{
							tipPositions.push_back(cv::Point((*(defectArray[i].end)).x, (*(defectArray[i].end)).y));
						}
					}
				}

				//cvNamedWindow("contour", 1); cvShowImage("contour", src);

				free(defectArray);
			}
			cvReleaseMemStorage(&storage1);
			cvReleaseMemStorage(&storage2);
		}
	}

	for (auto i = 0; i < tipPositions.size(); i++)
	{
		printf("tipPositions[%d]=(%d,%d)\n", i, tipPositions[i].x, tipPositions[i].y);
	}


	cvReleaseMemStorage(&storage);
	//cvNamedWindow("threshold", 1); cvShowImage("threshold", src);
	colorMarked = src;
	//printf_s("print\n");
	cvReleaseImage(&src);
	cvReleaseImage(&gray);
	if (tipPositions.empty())
	{
		tipPositions.push_back(cv::Point(-1, -1));
	}
	return tipPositions;
}


HandDetect::~HandDetect()
{
}

cv::Mat HandDetect::getBinaryImage(void)
{
	cv::Mat img = cv::Mat::zeros(cv::Size(depthImage.cols, depthImage.rows), CV_8U);
	for (int y = 0; y < depthImage.rows; y++)
	{
		float *depthImagePtr = depthImage.ptr<float>(y);
		unsigned char *imgPtr = img.ptr<uchar>(y);
		for (int x = 0; x < depthImage.cols; x++)
		{
			if (depthImagePtr[x] > _nearThreshold && depthImagePtr[x] < _farThreshold)
			{
				imgPtr[x] = 255;
			}
			else
			{
				imgPtr[x] = 0;
			}
		}
	}
	return img;
}
