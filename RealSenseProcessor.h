#pragma once

#include <windows.h>
#include "RealSense/SenseManager.h"
#include "RealSense/SampleReader.h"
#include <conio.h>
#include <opencv2\opencv.hpp>
#include <opencv2/core/core_c.h>
#include <opencv/highgui.h>
#include <opencv\cv.h>
#include <direct.h>
#include "pxchandmodule.h"
#include "pxcsensemanager.h"
#include "pxchandconfiguration.h"
#include "pxcprojection.h"
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <fstream>

#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/console/parse.h>
#include <pcl/filters/fast_bilateral.h>

#include "wcolorIO.h"
#include "HandJointData.h"

#include "PointCloud2Mesh.h"


#ifdef _DEBUG
//Debug���[�h�̏ꍇ
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_core2411d.lib")
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_imgproc2411d.lib")
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_highgui2411d.lib")
#else
//Release���[�h�̏ꍇ
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_core2411.lib")
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_imgproc2411.lib")
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_highgui2411.lib")
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#define SHOW_DEPTH_ROW 7
#define AVERAGE_FRAME_NUM 10
#define CLOUD_SCALE 1000
#define CLOUD_PITCH 1
#define DIFF_EXCLUDE_THRESHOLD 20
#define GAUSS_EXCLUDE_THRESHOLD 10
#define CONTOUR_SIZE_THRESHOLD 10

using namespace Intel::RealSense;

class RealSenseProcessor
{
private:
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr hand_point_cloud_ptr;
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr camera_point_cloud_ptr;
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr hand_joint_cloud_ptr;

public:
	RealSenseProcessor();
	~RealSenseProcessor();
	bool run(void);
private:
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer;

	Status ppInit(void);
	void showStatus(Status sts);
	void initializeViewer(const std::string &id, pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &pointCloudPtr, double pointSize = 1.0);
	bool updateCameraImage(PXCImage* cameraFrame, bool isDepthImage);
	void realsenseHandStatus(PXCHandData *handAnalyzer);
	bool updateHandImage(void);
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr updatePointCloud(bool isHandDataArrived);
	void releaseHandImage(void);
	void updateViewerText(void);
	void keyboardCallback(const pcl::visualization::KeyboardEvent& event, void*);
	bool acqireImage(PXCImage* cameraFrame, cv::Mat &mat, PXCImage::PixelFormat pixelFormat);
	int countMat(cv::Mat mat, cv::Vec4b elm);
	int countMat(cv::Mat mat, unsigned char elm);
	int countMat(cv::Mat mat, float elm);
	bool keyboardCallBackSettings(int key);
	bool isOutliers(float rawDepthElem, float rawDepthPrevElem);
	int detC(cv::Mat src);

	Status sts;
	bool isContinue;
	bool isUserInterrupt;
	bool isExit = false;

	SenseManager *pp;

	enum
	{
		CLOUD_HAND,
		CLOUD_CAMERA,
		CLOUD_JOINT,
		CLOUD_NUM,
	};

	enum
	{
		CV_WAITKEY_CURSORKEY_TOP = 2490368,
		CV_WAITKEY_CURSORKEY_BOTTOM = 2621440,
		CV_WAITKEY_CURSORKEY_RIGHT = 2555904,
		CV_WAITKEY_CURSORKEY_LEFT = 2424832,
	};

	bool isCloudArrived[CLOUD_NUM];

	static const int COLOR_WIDTH = 1920;
	static const int COLOR_HEIGHT = 1080;
	static const int COLOR_FPS = 30;

	static const int DEPTH_WIDTH = 640;
	static const int DEPTH_HEIGHT = 480;
	static const int DEPTH_FPS = 30;


	//�摜�\���̋L�q
	cv::Mat colorImage;
	cv::Mat depthImage;
	cv::Mat rawDepthImage;
	cv::Mat rawDepthImagePrev;
	cv::Mat handImage1;
	cv::Mat handImage2;
	cv::Mat handImage;
	cv::Mat grayHandImage;
	cv::Mat handPoint;
	cv::Mat rawDepthDiffImage;
	cv::Mat rawDepthDiffImageFilterd;
	cv::Mat rawDepthImageGauss;

	pxcI32 numberOfHands;

	std::vector<unsigned short> depthBuffer;
	const std::string windowName[1] = { "handimage" };

	/*const int COLOR_WIDTH = 640;
	const int COLOR_HEIGHT = 480;
	const int COLOR_FPS = 30;

	const int DEPTH_WIDTH = 640;
	const int DEPTH_HEIGHT = 480;
	const int DEPTH_FPS = 30;*/

	//�N���X���ϐ�
	int colorImageNum = 0;
	int depthImageNum = 0;
	int imageNum = 0;
	int dataNum = 0;
	float bilateralS = 5;
	float bilateralR = 0.05;
	bool enableBilateral = false;

	wchar_t directoryName[20];
	char nallowDirectoryName[20];
	std::string dataFileName;
	std::ofstream dataFile;
	char windowTitle[20];

	double sigmaG = 1.0;
	int gSize = 1;

	int pointCloudNum[CLOUD_NUM];

	PXCHandConfiguration* config = NULL;
	PXCHandData* handData = NULL;
	PXCProjection *projection = nullptr;

	PXCHandModule *handAnalyzer = NULL;

	
	//pcl::PointCloud<pcl::PointXYZRGBA>::Ptr hand_joint_cloud_ptr_tmp;
	/*pcl::FastBilateralFilter<pcl::PointXYZRGBA> bilateralH;
	pcl::FastBilateralFilter<pcl::PointXYZRGBA> bilateralC;*/

	//��������dptviewer

	// init()�����s����O�ɐݒ肷��@���F1
	bool enableReadColor = false; // �J���[�摜�̎擾����
	bool enableReadDepth = false; // depth8i�摜�̎擾����
	bool enableHandTracking = false; // �n���h�g���b�L���O�̋���
	bool enableMirror = false; // �~���[�\���̋���

	bool isSaveMirror = false; // �~���[�ł̉摜�ۑ�

							   // ��̈ʒu�̓_�̃T�C�Y
	int handSize = true;

	const std::string extension = "dpt";

	enum
	{
		d1_mirror
	};

	// dpt�f�[�^�̃w�b�_�f�[�^
	typedef struct
	{
		unsigned char size = 0x10;
		unsigned char identifier = 0;
		unsigned short width = 0;
		unsigned short height = 0;
		unsigned short resoHori = 0;
		unsigned short resoVert = 0;
		unsigned char type = 0;
		// | �\�� | �\�� | �\�� | �\�� | �\�� | �\�� | �\�� | �~���[ |
		unsigned char data1 = 0x00;
	}dptHeader;

	// depth32f�摜���t�@�C���ɏ�������
	void writeDepth(const std::string name);
	std::string makeNameFolder(int hrgn);
	std::string makeNameFail(int hrgn, int num);
	cv::Mat drawGuide(const cv::Mat& input, int num);
	void printText(int hrgn, int num);
	void shorGuideImage(const cv::Mat depth, int num);
	cv::Mat RealSenseProcessor::readDepth(const std::string name);
	std::string getTime(void);

	const int numMax = 9; // �ۑ�����ꕶ���̐�
	const int distMin = 375; // ��O�̋���
	const int distMax = 425; // ���̋���
	const int sizeLine = 20; // ���̑���

	int num = 0; // �ԍ��i�[�p
	int hrgn = 0; // �����i�[�p

	std::string _time;

	int morph_elem = 0;
	int morph_size = 1;
	int const max_elem = 2;
	/*int morph_operator = 0;
	int const max_operator = 4;
	int const max_kernel_size = 21;*/

	int cloudAlphaCh=0;

	HandJointData handjointdata[2];

	PointCloud2Mesh::gpParameters param;
};
