#include "RealSenseProcessor.h"

RealSenseProcessor::RealSenseProcessor() :
	viewer(new pcl::visualization::PCLVisualizer("3D Viewer"))
//	hand_point_cloud_ptr(new pcl::PointCloud<pcl::PointXYZRGBA>),
//	camera_point_cloud_ptr(new pcl::PointCloud<pcl::PointXYZRGBA>),
//	hand_joint_cloud_ptr(new pcl::PointCloud<pcl::PointXYZRGBA>)//PCL関連の変数の初期化
{
	wColorIO(wColorIO::PRINT_INFO, L"Start\n");

	std::vector<RealSenseUpdater> rsu(NUM);

/*	hand_point_cloud_ptr->width = DEPTH_WIDTH;
	hand_point_cloud_ptr->height = DEPTH_HEIGHT;

	camera_point_cloud_ptr->width = DEPTH_WIDTH;
	camera_point_cloud_ptr->height = DEPTH_HEIGHT;

	hand_joint_cloud_ptr->points.resize(DEPTH_WIDTH*DEPTH_HEIGHT);
	camera_point_cloud_ptr->points.resize(DEPTH_WIDTH*DEPTH_HEIGHT);

	if (hand_point_cloud_ptr->isOrganized())
		wColorIO(wColorIO::PRINT_INFO, L"organized\n");
	else
		wColorIO(wColorIO::PRINT_INFO, L"unorganized\n");

	if (camera_point_cloud_ptr->isOrganized())
		wColorIO(wColorIO::PRINT_INFO, L"organized\n");
	else
		wColorIO(wColorIO::PRINT_INFO, L"unorganized\n");

	pp = SenseManager::CreateInstance();
	if (!pp)
	{
		wprintf_s(L"Unable to create the SenseManager\n");
	}

	cv::namedWindow(windowName[0], CV_WINDOW_AUTOSIZE);
	rawDepthImage = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_32FC1);
	handImage1 = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC1);
	handImage2 = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC1);
	handImage = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC4);
	handPoint = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC4);
	rawDepthDiffImage = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC1);
	rawDepthDiffImageFilterd = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC3);*/

	viewer->registerKeyboardCallback(&RealSenseProcessor::keyboardCallback, *this);

	isContinue = false;
	isUserInterrupt = false;
	_time = getTime();
}


RealSenseProcessor::~RealSenseProcessor()
{
	// Releases lock so pipeline can process next frame 
	//pp->ReleaseFrame();
	viewer->close();

	cv::destroyAllWindows();

	/*if (handAnalyzer == nullptr)
	{
		pp->Release();
		pp = nullptr;
	}
	if (handData == nullptr)
	{
		handData->Release();
		handData = nullptr;
	}

	if (projection == nullptr)
	{
		projection->Release();
		projection = nullptr;
	}*/
	wColorIO(wColorIO::PRINT_SUCCESS, L"Exiting\n");
}

bool RealSenseProcessor::run(void)
{
	return true;
/*	sts = ppInit();
	if (sts >= Status::PXC_STATUS_NO_ERROR)
	{
		while (!viewer->wasStopped())
		{
			for (int i = 0; i < CLOUD_NUM; i++)
			{
				isCloudArrived[i] = false;
			}
			// 表示を更新する
			viewer->spinOnce();

			//Waits until new frame is available and locks it for application processing
			sts = pp->AcquireFrame(true);

			if (!pp->IsConnected())
			{
				wColorIO(wColorIO::PRINT_ERROR, L"device removed\n");
				pp->Close();
				isContinue = true;
				break;
			}

			if (sts < Status::STATUS_NO_ERROR)
			{
				break;
			}

			const Capture::Sample *sample = pp->QuerySample();
			if (sample)
			{
				if (sample->color && !updateCameraImage(sample->color, false)) break;
				if (sample->depth && !updateCameraImage(sample->depth, true)) break;

				detC(rawDepthDiffImage.clone());

				
				if (handData)
				{
					handData->Update();
					updateHandImage();
					hand_point_cloud_ptr = updatePointCloud(true);

					viewer->updatePointCloud(hand_point_cloud_ptr, "handcloud");

					viewer->updatePointCloud(hand_joint_cloud_ptr, "handjoint");

					if (pointCloudNum[CLOUD_HAND] != 0)
						isCloudArrived[CLOUD_HAND] = true;

					//viewer = app.rgbVis(point_cloud_ptr);

					realsenseHandStatus(handData);
				}
				else
				{
					wColorIO(wColorIO::PRINT_ERROR, L"Hands couldn't be detected\n");
					releaseHandImage();
				}

				camera_point_cloud_ptr = updatePointCloud(false);

				pcl::visualization::PointCloudColorHandlerRGBAField<pcl::PointXYZRGBA> rgba(camera_point_cloud_ptr);

				viewer->updatePointCloud(camera_point_cloud_ptr, rgba, "cameracloud");
				//viewer->updatePointCloud(camera_point_cloud_ptr,  "cameracloud");
				if (pointCloudNum[CLOUD_CAMERA] != 0)
					isCloudArrived[CLOUD_CAMERA] = true;


				rawDepthImagePrev = rawDepthImage.clone();

				updateViewerText();
			}

			// Releases lock so pipeline can process next frame 
			pp->ReleaseFrame();
			//pp->Release();
			if (handAnalyzer == nullptr)
			{
				pp->Release();
				pp = nullptr;
			}
			if (handData == nullptr)
			{
				handData->Release();
				handData = nullptr;
			}

			if (projection == nullptr)
			{
				projection->Release();
				projection = nullptr;
			}

			//config->Release();

			if (viewer->wasStopped())
			{
				wColorIO(wColorIO::PRINT_ERROR, L"PCLViewer has been stoped\n");
			}

			// ウィンドウが消されても再表示する
			cv::namedWindow("カメラ");
			cv::namedWindow("保存済み");

			shorGuideImage(rawDepthImage, num);

			keyboardCallBackSettings(cv::waitKey(1));

			if (_kbhit())
			{ // Break loop
				int c = _getch() & 255;
				if (c == 27 || c == 'q' || c == 'Q')
				{
					isUserInterrupt = true;
					c = 0;
					break;
				} // ESC|q|Q for Exit
			}
		}
		isExit = true;
	}

	viewer->close();
	cv::destroyAllWindows();
	pp->Close();

	if (sts == Status::STATUS_STREAM_CONFIG_CHANGED)
	{
		wprintf_s(L"Stream configuration was changed, re-initilizing\n");
		isContinue = true;
		return(isContinue);
	}
	else if (isUserInterrupt)
	{
		wColorIO(wColorIO::PRINT_WARN, L"Stream has been stoped by user interrupt.\n");
	}
	else
	{
		wprintf_s(L"Stream has been stoped due to error(s)\nsts=");
		showStatus(sts);
	}
	wprintf_s(L"Do you want to re-initilizing?\nY/N\n");

	while (true)
	{
		int c = _getch();
		if (c == 'Y' || c == 'y')
		{
			isContinue = true;
			isExit = false;
			break;
		}
		else if (c == 'N' || c == 'n' || c == 27)
		{
			isContinue = false;
			break;
		}
	}
	return(isContinue);*/
}

bool RealSenseProcessor::keyboardCallBackSettings(int key)
{
	cv::Mat tmp;

	switch (key)
	{
	case CV_WAITKEY_CURSORKEY_TOP:num++;	break; // 数字 + 1
	case CV_WAITKEY_CURSORKEY_BOTTOM:num--;	break; // 数字 - 1
	case CV_WAITKEY_CURSORKEY_RIGHT:hrgn++;	break; // 文字 + 1
	case CV_WAITKEY_CURSORKEY_LEFT:hrgn--;	break; // 文字 - 1
/*	case 'r':
		for (int i = 0; i < HandJointData::NUM_OF_JOINT_DST; i++)
		{
			wColorIO(wColorIO::PRINT_INFO, L"right hand %d=%lf\n", i, handjointdata[0].jointDistance[i]);
		}
		break;
	case 'l':
		for (int i = 0; i < HandJointData::NUM_OF_JOINT_DST; i++)
		{
			wColorIO(wColorIO::PRINT_INFO, L"right hand %d=%lf\n", i, handjointdata[1].jointDistance[i]);
		}
		break;
	case 't':
		if (cloudAlphaCh == 2)
			cloudAlphaCh = 0;
		else
			cloudAlphaCh++;
		break;
	case 'w':
	case 'W':
		morph_size++;
		break;
	case 's':
	case 'S':
		if (morph_size > 1)
			morph_size--;
		else
			morph_size = 0;
		break;
	case 'd':
	case 'D':
		if (morph_elem >= 2)
			morph_elem = 2;
		else
			morph_elem++;
		break;
	case 'a':
	case 'A':
		if (morph_elem <= 0)
			morph_elem = 0;
		else
			morph_elem--;
		break;
	case 'z':
		if (gSize > 1)
			gSize -= 2;
		else
			gSize = 1;
		break;
	case 'x':
		gSize += 2;
		break;
	case 'c':
		if (sigmaG > 0)
			sigmaG -= 1;
		else
			sigmaG = 0;
		break;
	case 'v':
		sigmaG += 1;
		break;*/
	case ' ': // 保存
		CreateDirectory((_time).c_str(), NULL); // 大本のフォルダ作成（名前が時間）
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-Color").c_str(), NULL); // color画像フォルダ作成
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-Depth").c_str(), NULL); // depth画像フォルダ作成
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-HandImage").c_str(), NULL); // HandImage画像フォルダ作成
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-HandPoint").c_str(), NULL); // HandPoint画像フォルダ作成
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-PCLHand").c_str(), NULL); // PCLHand画像フォルダ作成
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-PCLJoint").c_str(), NULL); // PCLJoint画像フォルダ作成
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-PCLCamera").c_str(), NULL); // PCLCamera画像フォルダ作成
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-Depth見る用").c_str(), NULL); // depth見る用画像フォルダ作成

/*		flip(colorImage, tmp, 1); // 反転
		cv::imwrite(_time + "\\" + makeNameFolder(hrgn) + "-Color" + "\\" + makeNameFail(hrgn, num) + ".tif", tmp); // color画像保存
		flip(handImage, tmp, 1); // 反転
		cv::imwrite(_time + "\\" + makeNameFolder(hrgn) + "-HandImage" + "\\" + makeNameFail(hrgn, num) + ".tif", tmp); // handImage画像保存
		flip(handPoint, tmp, 1); // 反転
		cv::imwrite(_time + "\\" + makeNameFolder(hrgn) + "-HandPoint" + "\\" + makeNameFail(hrgn, num) + ".tif", tmp); // handPoint画像保存
		flip(rawDepthImage, tmp, 1); // 反転
		imwrite(_time + "\\" + makeNameFolder(hrgn) + "-Depth見る用" + "\\" + makeNameFail(hrgn, num) + ".tif", tmp * 0x60 / 0x100); // depth見る用画像保存
		writeDepth(_time + "\\" + makeNameFolder(hrgn) + "-Depth" + "\\" + makeNameFail(hrgn, num)); // depth画像保存
		if (isCloudArrived[CLOUD_HAND])
		{
			pcl::io::savePCDFileBinary(_time + "\\" + makeNameFolder(hrgn) + "-PCLHand" + "\\" + makeNameFail(hrgn, num) + ".pcd", *hand_point_cloud_ptr);
			PointCloud2Mesh(hand_point_cloud_ptr, _time + "\\" + makeNameFolder(hrgn) + "-PCLHand" + "\\" + makeNameFail(hrgn, num) + "s.obj", param, true);
		}
		if (isCloudArrived[CLOUD_CAMERA])
			pcl::io::savePCDFileBinary(_time + "\\" + makeNameFolder(hrgn) + "-PCLCamera" + "\\" + makeNameFail(hrgn, num) + ".pcd", *camera_point_cloud_ptr);
		if (isCloudArrived[CLOUD_JOINT])
			pcl::io::savePCDFileBinary(_time + "\\" + makeNameFolder(hrgn) + "-PCLJoint" + "\\" + makeNameFail(hrgn, num) + ".pcd", *hand_joint_cloud_ptr);
		tmp = readDepth(_time + "\\" + makeNameFolder(hrgn) + "-Depth" + "\\" + makeNameFail(hrgn, num)) * 0x60 / 0x10000;
		
		cv::imshow("保存済み", tmp); // 保存したものの表示*/
		num++;
		break;
	case 'q': // 終了
		viewer->close();
		cv::destroyAllWindows();
		isUserInterrupt = true;
		return false;
	default:
		/*if (key != -1)
		wColorIO(wColorIO::PRINT_VALUE, L"%d\n", key);*/
		return true;
	}

	if (numMax - 1 < num)
	{
		num = 0;
		hrgn++;
	}
	else if (num < 0)
	{
		num = numMax - 1;
		hrgn--;
	}
	if (hrgn < 0)
	{
		num = 0;
		hrgn = 0;
	}
	else if ('ﾝ' - 'ｱ' < hrgn)
	{
		num = numMax - 1;
		hrgn = 'ﾝ' - 'ｱ';
	}
	printText(hrgn, num);
	// ボタンが押されたときのみ表示
	return true;
}

/*Status RealSenseProcessor::ppInit(void)
{
	//isContinue = false;
	isExit = false;
	isUserInterrupt = false;

	// ストリームを有効にする
	sts = pp->EnableStream(PXCCapture::StreamType::STREAM_TYPE_COLOR, COLOR_WIDTH, COLOR_HEIGHT, COLOR_FPS);

	if (sts < Status::STATUS_NO_ERROR)
	{
		wColorIO(wColorIO::PRINT_ERROR, L"Create Color-Image pipeline has been unsuccessful.\n");
		return sts;
	}
	else
	{
		wColorIO(wColorIO::PRINT_SUCCESS, L"Create Color-Image pipeline has been successful.\n");
		wColorIO(wColorIO::PRINT_INFO, L"Width:");
		wColorIO(wColorIO::PRINT_VALUE, L"%dpx ", COLOR_WIDTH);
		wColorIO(wColorIO::PRINT_INFO, L"Height:");
		wColorIO(wColorIO::PRINT_VALUE, L"%dpx ", COLOR_HEIGHT);
		wColorIO(wColorIO::PRINT_INFO, L"FPS:");
		wColorIO(wColorIO::PRINT_VALUE, L"%d\n", COLOR_FPS);
	}

	sts = pp->EnableStream(PXCCapture::StreamType::STREAM_TYPE_DEPTH, DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_FPS);
	if (sts < Status::STATUS_NO_ERROR)
	{
		wColorIO(wColorIO::PRINT_ERROR, L"Create Depth-Image pipeline has been unsuccessful.\n");
		return sts;
	}
	else
	{
		wColorIO(wColorIO::PRINT_SUCCESS, L"Create Depth-Image pipeline has been successful.\n");
		wColorIO(wColorIO::PRINT_INFO, L"Width:");
		wColorIO(wColorIO::PRINT_VALUE, L"%dpx ", DEPTH_WIDTH);
		wColorIO(wColorIO::PRINT_INFO, L"Height:");
		wColorIO(wColorIO::PRINT_VALUE, L"%dpx ", DEPTH_HEIGHT);
		wColorIO(wColorIO::PRINT_INFO, L"FPS:");
		wColorIO(wColorIO::PRINT_VALUE, L"%d\n", DEPTH_FPS);
	}

	sts = pp->EnableHand(0);

	if (sts < PXC_STATUS_NO_ERROR)
	{
		wColorIO(wColorIO::PRINT_ERROR, L"Failed to pair the hand module with I/O.\n");
		return sts;
	}
	else
	{
		wColorIO(wColorIO::PRINT_SUCCESS, L"Succeeded to pair the hand module with I/O.\n");
	}

	handAnalyzer = pp->QueryHand();

	if (handAnalyzer == NULL)
	{
		wColorIO(wColorIO::PRINT_ERROR, L"Failed to pair the hand module with I/O\n");
		return sts;
	}
	else
	{
		wColorIO(wColorIO::PRINT_SUCCESS, L"Succeeded to pair the hand module with I/O\n");
	}


	// パイプラインを初期化する
	sts = pp->Init();


	if (sts >= PXC_STATUS_NO_ERROR)
	{
		PXCCapture::Device *device = pp->QueryCaptureManager()->QueryDevice();
		PXCCapture::DeviceInfo dinfo;

		if (device != NULL)// ミラー表示にする
		{
			device->SetMirrorMode(Capture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);
			wColorIO(wColorIO::PRINT_INFO, L"MirrorMode has been enabled.\n");
		}

		device->QueryDeviceInfo(&dinfo);
		if (dinfo.model == PXCCapture::DEVICE_MODEL_IVCAM)
		{
			device->SetDepthConfidenceThreshold(1);
			device->SetIVCAMFilterOption(6);
		}

		projection = device->CreateProjection();

		if (handAnalyzer)
		{
			handData = handAnalyzer->CreateOutput();
			config = handAnalyzer->CreateActiveConfiguration();
			//config->SetTrackingMode(PXCHandData::TRACKING_MODE_EXTREMITIES);//高速・輪郭モード時にコメントアウト
			//config->EnableNormalizedJoints(showNormalizedSkeleton);
			config->EnableSegmentationImage(true);
			config->DisableAllGestures();
		}
		else
		{
			wColorIO(wColorIO::PRINT_ERROR, L"Failed to set up handData\n");
			return sts;
		}

		config->ApplyChanges();
		config->Update();
	}
	else
	{
		wColorIO(wColorIO::PRINT_ERROR, L"Pipeline initializing has been failed.\n");
		return sts;
	}

	viewer->setBackgroundColor(0, 0, 0);
	initializeViewer("handcloud", hand_point_cloud_ptr);
	initializeViewer("cameracloud", camera_point_cloud_ptr, 0.1);
	initializeViewer("handjoint", hand_joint_cloud_ptr, 10);
	viewer->addCoordinateSystem(0.01);
	viewer->initCameraParameters();

	return sts;
}

void RealSenseProcessor::initializeViewer(const std::string &id, pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &pointCloudPtr, double pointSize)
{
	pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGBA> hand_rgb(pointCloudPtr);
	viewer->addPointCloud<pcl::PointXYZRGBA>(pointCloudPtr, hand_rgb, id);
	//viewer->addPointCloud(pointCloudPtr, id);
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, pointSize, id);
}

bool RealSenseProcessor::acqireImage(PXCImage* cameraFrame, cv::Mat &mat, PXCImage::PixelFormat pixelFormat)
{
	PXCImage::ImageData data;

	sts = cameraFrame->AcquireAccess(PXCImage::Access::ACCESS_READ, pixelFormat, &data);

	if (sts < PXC_STATUS_NO_ERROR)
	{
		wColorIO(wColorIO::PRINT_ERROR, L"Cannot reach the cameraFrame\n");
	}

	// データをコピーする
	PXCImage::ImageInfo info = cameraFrame->QueryInfo();

	if (pixelFormat == PXCImage::PIXEL_FORMAT_RGB32)
		mat = cv::Mat(info.height, info.width, CV_8UC4);
	else if (pixelFormat == PXCImage::PIXEL_FORMAT_DEPTH_F32)
		mat = cv::Mat(info.height, info.width, CV_32FC1);
	else
		return false;

	memcpy(mat.data, data.planes[0], data.pitches[0] * info.height);

	// データを解放する
	cameraFrame->ReleaseAccess(&data);
	return true;
}

bool RealSenseProcessor::updateCameraImage(PXCImage* cameraFrame, bool isDepthImage)
{
	if (cameraFrame == nullptr)
	{
		wColorIO(wColorIO::PRINT_ERROR, L"cameraFrame throws nullptr.\n");
		return false;
	}

	if (isDepthImage)
	{
		if (!acqireImage(cameraFrame, depthImage, PXCImage::PIXEL_FORMAT_RGB32))
			return false;
		if (!acqireImage(cameraFrame, rawDepthImage, PXCImage::PIXEL_FORMAT_DEPTH_F32))
			return false;
		if (rawDepthImagePrev.total() < 1)
			return true;

		cv::GaussianBlur(rawDepthImage, rawDepthImageGauss, cv::Size(gSize, gSize), sigmaG, sigmaG);

		for (int y = 0; y < rawDepthImage.rows; y++)
		{
			float *rawDepthImagePtr = rawDepthImage.ptr<float>(y);
			float *rawDepthImagePrevPtr = rawDepthImagePrev.ptr<float>(y);
			unsigned char *rawDepthDiffImagePtr = rawDepthDiffImage.ptr<uchar>(y);
			float *rawDepthImageGaussPtr = rawDepthImageGauss.ptr<float>(y);

			for (int x = 0; x < rawDepthImage.cols; x++)
			{
				if (abs(rawDepthImagePtr[x] - rawDepthImagePrevPtr[x]) > DIFF_EXCLUDE_THRESHOLD)
				{
					rawDepthDiffImagePtr[x] = 255;
				}
				else if (abs(rawDepthImagePtr[x] - rawDepthImageGaussPtr[x]) > GAUSS_EXCLUDE_THRESHOLD)
				{
					rawDepthDiffImagePtr[x] = 128;
				}
				else
				{
					rawDepthDiffImagePtr[x] = 0;
				}
			}
		}
	}
	else
	{
		if (!acqireImage(cameraFrame, colorImage, PXCImage::PIXEL_FORMAT_RGB32))
			return false;
	}
	return true;
}

bool RealSenseProcessor::updateHandImage(void)
{
	PXCImage::ImageData data;
	pointCloudNum[CLOUD_JOINT] = 0;

	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr hand_joint_cloud_ptr_tmp(new pcl::PointCloud<pcl::PointXYZRGBA>);
	if (handData != nullptr&&handData)
	{
		numberOfHands = handData->QueryNumberOfHands();

		handImage = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC4);
		grayHandImage = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC1);
		handPoint = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC4);

		for (int i = 0; i < numberOfHands; i++)
		{
			//pxcUID handID;
			PXCHandData::IHand* hand;

			sts = handData->QueryHandData(PXCHandData::AccessOrderType::ACCESS_ORDER_BY_ID, i, hand);
			if (sts < PXC_STATUS_NO_ERROR)
			{
				wColorIO(wColorIO::PRINT_ERROR, L"Couldn't update hand frame.\n");
				continue;
			}

			PXCImage* image = 0;
			sts = hand->QuerySegmentationImage(image);
			if (sts < PXC_STATUS_NO_ERROR)
			{
				wColorIO(wColorIO::PRINT_ERROR, L"Couldn't acquire hand image.\n");
				continue;
			}

			sts = image->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_Y8, &data);
			if (sts < PXC_STATUS_NO_ERROR)
			{
				wColorIO(wColorIO::PRINT_ERROR, L"Couldn't acquire masked hand image.\n");
				continue;
			}

			PXCImage::ImageInfo info = image->QueryInfo();

			auto& handImageTemp = i ? handImage2 : handImage1;
			memcpy(handImageTemp.data, data.planes[0], info.height*info.width);

			for (int y = 0; y < handImage.rows; y++)
			{
				cv::Vec4b *handImagePtr = handImage.ptr<cv::Vec4b>(y);
				unsigned char *handImage1Ptr = handImage1.ptr<uchar>(y);
				unsigned char *handImage2Ptr = handImage2.ptr<uchar>(y);
				unsigned char *grayHandImagePtr = grayHandImage.ptr<uchar>(y);
				for (int x = 0; x < handImage.cols; x++)
				{
					if ((i ? handImage2Ptr[x] : handImage1Ptr[x]) == 255)
					{
						handImagePtr[x] = cv::Vec4b(255, 255, 255, 255);
						grayHandImagePtr[x] = 255;
					}
				}
			}

			for (int j = 0; j < PXCHandData::NUMBER_OF_JOINTS; j++)
			{
				//int x, y = 0;
				PXCHandData::JointData jointData;
				sts = hand->QueryTrackedJoint((PXCHandData::JointType)j, jointData);
				if (sts < PXC_STATUS_NO_ERROR)
				{
					continue;
				}
				cv::circle(handPoint, cv::Point(jointData.positionImage.x, jointData.positionImage.y), 5, cv::Scalar(128 * (1 - i), 128 * i, 0, 0));

				pcl::PointXYZRGBA point;

				point.x = jointData.positionWorld.x;
				point.y = jointData.positionWorld.y;
				point.z = jointData.positionWorld.z;
				point.r = 255 * (1 - i);
				point.g = 255 * i;
				point.b = 10 * j;
				//point.a = 0;

				handjointdata[i].x[j] = jointData.positionWorld.x;
				handjointdata[i].y[j] = jointData.positionWorld.y;
				handjointdata[i].z[j] = jointData.positionWorld.z;

				isCloudArrived[CLOUD_JOINT] = true;
				pointCloudNum[CLOUD_JOINT]++;

				hand_joint_cloud_ptr_tmp->points.push_back(point);
			}
			image->ReleaseAccess(&data);

			auto center = hand->QueryMassCenterImage();
			cv::circle(handPoint, cv::Point(center.x, center.y), 5, cv::Scalar(0, 255 * i, 255 * (1 - i)), -1);
		}

		for (int y = 0; y < handImage.rows; y++)
		{
			unsigned char *grayHandImagePtr = grayHandImage.ptr<uchar>(y);
			float *rawDepthImagePtr = rawDepthImage.ptr<float>(y);
			cv::Vec4b *handImagePtr = handImage.ptr<cv::Vec4b>(y);

			for (int x = 0; x < handImage.cols; x++)
			{
				if (grayHandImagePtr[x] == 255)
				{
					PXCPointF32 dColorPoint;
					PXCPoint3DF32 dDepthPoint;

					dDepthPoint.x = x;
					dDepthPoint.y = y;
					dDepthPoint.z = rawDepthImagePtr[x];

					projection->MapDepthToColor(1, &dDepthPoint, &dColorPoint);

					if (dColorPoint.x != -1.0 && dColorPoint.y != -1.0)
					{
						cv::Vec4b *colorImagePtr = colorImage.ptr<cv::Vec4b>((int)dColorPoint.y);

						cv::Vec4b colorPx = colorImagePtr[(int)dColorPoint.x];

						handImagePtr[x] = colorPx;
					}
				}
			}
		}
		if (handjointdata[0].isHandSurfaceVartical(1))
			handjointdata[0].getJointLength();

		if (handjointdata[1].isHandSurfaceVartical(1))
			handjointdata[1].getJointLength();

		cv::imshow(windowName[0], handImage);

		hand_joint_cloud_ptr = hand_joint_cloud_ptr_tmp;

		return(true);
	}
	else
	{
		wColorIO(wColorIO::PRINT_ERROR, L"handData is nullptr.\n");
		//return(point_cloud_ptr);
		return(false);
	}
}

bool RealSenseProcessor::isOutliers(float rawDepthElem, float rawDepthPrevElem)
{
	if (abs(rawDepthElem - rawDepthPrevElem) > DIFF_EXCLUDE_THRESHOLD)
	{
		return true;
	}
	else
	{
		return false;
	}
}
int RealSenseProcessor::detC(cv::Mat img)
{
	cv::Mat src;
	cv::Mat element = cv::getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1), cv::Point(morph_size, morph_size));
	if (!(src = img).data)
		return -1;

	cv::Mat dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC3);

	cv::morphologyEx(src, dst, CV_MOP_TOPHAT, element);

	rawDepthDiffImageFilterd = dst.clone();
	return 0;
}

pcl::PointCloud<pcl::PointXYZRGBA>::Ptr RealSenseProcessor::updatePointCloud(bool isHandDataArrived)
{
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr point_cloud_ptr(new pcl::PointCloud<pcl::PointXYZRGBA>);
	if (isHandDataArrived)
		pointCloudNum[CLOUD_HAND] = 0;
	else
		pointCloudNum[CLOUD_CAMERA] = 0;

	for (int y = 0; y < handImage.rows; y += (isHandDataArrived ? 1 : CLOUD_PITCH))
	{
		unsigned char *grayHandImagePtr = grayHandImage.ptr<uchar>(y);
		float *rawDepthImagePtr = rawDepthImage.ptr<float>(y);
		cv::Vec4b *handImagePtr = handImage.ptr<cv::Vec4b>(y);
		unsigned char *rawDepthDiffImagePtr = rawDepthDiffImage.ptr<uchar>(y);
		unsigned char *rawDepthDiffImageFilterdPtr = rawDepthDiffImageFilterd.ptr<uchar>(y);

		for (int x = 0; x < handImage.cols; x += (isHandDataArrived ? 1 : CLOUD_PITCH))
		{
			if (rawDepthImagePrev.total() <= 10)
				continue;
			float *rawDepthImagePrevPtr = rawDepthImagePrev.ptr<float>(y);
			if (rawDepthImagePtr[x] != 0 && !(grayHandImagePtr[x] != 255 && isHandDataArrived))//&&!isOutliers(rawDepthImagePtr[x],rawDepthImagePrevPtr[x])
			{
				PXCPointF32 dColorPoint;
				PXCPoint3DF32 dDepthPoint;
				PXCPoint3DF32 cDepthPoint;
				pcl::PointXYZRGBA point;
				bool isSkip = false;

				dDepthPoint.x = x;
				dDepthPoint.y = y;
				dDepthPoint.z = rawDepthImagePtr[x];

				projection->MapDepthToColor(1, &dDepthPoint, &dColorPoint);
				projection->ProjectDepthToCamera(1, &dDepthPoint, &cDepthPoint);

				point.x = cDepthPoint.x / CLOUD_SCALE;
				point.y = cDepthPoint.y / CLOUD_SCALE;
				point.z = cDepthPoint.z / CLOUD_SCALE;

				if (dColorPoint.x != -1.0 && dColorPoint.y != -1.0)
				{
					cv::Vec4b *colorImagePtr = colorImage.ptr<cv::Vec4b>((int)dColorPoint.y);

					cv::Vec4b colorPx = colorImagePtr[(int)dColorPoint.x];

					if (colorPx[0] == 255 && colorPx[1] == 255 && colorPx[2] == 255)
					{
						isSkip = true;
					}

					if (isHandDataArrived)
						handImagePtr[x] = colorPx;
					point.r = colorPx[2];
					point.g = colorPx[1];
					point.b = colorPx[0];
				}
				else
				{
					point.r = 255;
					point.g = 255;
					point.b = 255;
				}

				point.a = (2 - cloudAlphaCh) * 127;
				//point.a = 0;

				if (rawDepthDiffImageFilterdPtr[x] == 255)
				{
					point.r = 255;
				}
				if (rawDepthDiffImagePtr[x] == 128)
				{
					point.b = 255;
				}

				if (!isSkip)
					point_cloud_ptr->points.push_back(point);
				else
					continue;
				if (isHandDataArrived)
					pointCloudNum[CLOUD_HAND]++;
				else
					pointCloudNum[CLOUD_CAMERA]++;
			}
		}
	}
	return(point_cloud_ptr);
}

void RealSenseProcessor::releaseHandImage(void)
{
	handImage = cv::Mat::zeros(cv::Size(DEPTH_WIDTH, DEPTH_HEIGHT), CV_8UC4);
	cv::imshow(windowName[0], handImage);
}*/

void RealSenseProcessor::updateViewerText(void)
{
	//const char* TF[] = { "off", "median", "average" };
	const char* elemStr[] = { "Rect","Cross","Ellipse" };
	//char morph_str;
	std::vector<boost::format> entries;
	/*entries.push_back(boost::format("Hand Point Cloud Num = %i") % pointCloudNum[CLOUD_HAND]);
	entries.push_back(boost::format("Camera Point Cloud Num = %i") % pointCloudNum[CLOUD_CAMERA]);
	entries.push_back(boost::format("Joint Point Cloud Num = %i") % pointCloudNum[CLOUD_JOINT]);
	entries.push_back(boost::format("morph_size = %i") % morph_size);
	entries.push_back(boost::format("morph_elem = %s") % elemStr[morph_elem]);
	entries.push_back(boost::format("sigmaG = %.1f") % sigmaG);
	entries.push_back(boost::format("gSize = %i") % gSize);*/
	//"Element:\n 0: Rect - 1: Cross - 2: Ellipse"

	const int dx = 5;
	const int dy = 14;
	const int fs = 10;
	boost::format name_fmt("text%i");

	for (size_t i = 0; i < entries.size(); ++i)
	{
		std::string name = boost::str(name_fmt % i);
		std::string entry = boost::str(entries[i]);
		if (!viewer->updateText(entry, dx, dy + i * (fs + 2), fs, 1.0, 1.0, 1.0, name))
			viewer->addText(entry, dx, dy + i * (fs + 2), fs, 1.0, 1.0, 1.0, name);
	}
}

void RealSenseProcessor::keyboardCallback(const pcl::visualization::KeyboardEvent& event, void*)
{
	if (event.keyDown())
	{
		int key = event.getKeyCode();
		if (key != 0)
			keyboardCallBackSettings(key);
		else
		{
			std::string str = event.getKeySym();
			int key;
			if (str == "Right")
				key = CV_WAITKEY_CURSORKEY_RIGHT;
			else if (str == "Left")
				key = CV_WAITKEY_CURSORKEY_LEFT;
			else if (str == "Up")
				key = CV_WAITKEY_CURSORKEY_TOP;
			else if (str == "Down")
				key = CV_WAITKEY_CURSORKEY_BOTTOM;
			keyboardCallBackSettings(key);
		}
	}
}

/*int RealSenseProcessor::countMat(cv::Mat mat, cv::Vec4b elm)
{
	int matElmNum = 0;
	for (int y = 0; y > mat.rows; y++)
	{
		cv::Vec4b *matPtr = mat.ptr<cv::Vec4b>(y);
		for (int x = 0; x > mat.cols; x++)
		{
			if (matPtr[x] == elm)
				matElmNum++;
		}
	}
	return matElmNum;
}

int RealSenseProcessor::countMat(cv::Mat mat, unsigned char elm)
{
	int matElmNum = 0;
	for (int y = 0; y > mat.rows; y++)
	{
		unsigned char *matPtr = mat.ptr<uchar>(y);
		for (int x = 0; x > mat.cols; x++)
		{
			if (matPtr[x] == elm)
				matElmNum++;
		}
	}
	return matElmNum;
}

int RealSenseProcessor::countMat(cv::Mat mat, float elm)
{
	int matElmNum = 0;
	for (int y = 0; y > mat.rows; y++)
	{
		float *matPtr = mat.ptr<float>(y);
		for (int x = 0; x > mat.cols; x++)
		{
			if (matPtr[x] == elm)
				matElmNum++;
		}
	}
	return matElmNum;
}

void RealSenseProcessor::writeDepth(const std::string name)
{
	cv::Mat matDepth = rawDepthImage;
	std::fstream fs;

	std::string dir = name;

	// 拡張子がついているか
	std::string suffix = "." + extension;
	if (dir.size() < suffix.size() || dir.find(suffix, dir.size() - suffix.size()) == std::string::npos)
		dir += suffix;

	fs.open(dir, std::ios::out | std::ios::binary);

	if (!fs.is_open())
		throw std::runtime_error("depth32f画像の書き込みファイルの呼び出しに失敗");

	if (enableMirror != isSaveMirror) // ミラーするか
	{
		cv::flip(matDepth.clone(), matDepth, 1); // 反転
	}

	dptHeader header;
	header.width = DEPTH_WIDTH;
	header.height = DEPTH_HEIGHT;
	header.type = matDepth.type();
	if (isSaveMirror) // ミラーしているか
	{
		header.data1 |= 1 << d1_mirror; // ヘッダにミラー情報書き込み
	}

	if (!fs.write((const char*)(&header), sizeof(header)) // ヘッダ書き込み
		|| !fs.seekp(header.size) // ポインタ位置移動
		|| !fs.write((const char*)(matDepth.data), matDepth.total() * matDepth.elemSize())) // データ書き込み
		throw std::runtime_error("depth32f画像の書き込みに失敗");

	fs.close();
}


cv::Mat RealSenseProcessor::readDepth(const std::string name)
{
	std::fstream fs;

	std::string dir = name;

	// 拡張子がついているか
	std::string suffix = "." + extension;
	if (dir.size() < suffix.size() || dir.find(suffix, dir.size() - suffix.size()) == std::string::npos)
		dir += suffix;

	fs.open(dir, std::ios::in | std::ios::binary);
	if (!fs.is_open())
		throw std::runtime_error("depth32f画像の読み込みファイルの呼び出しに失敗");

	dptHeader header;

	if (!fs.read((char*)(&header.size), sizeof(header.size))
		|| !fs.read((char*)(&header.identifier), sizeof(header) - sizeof(header.size))
		|| !fs.seekg(header.size))
		throw std::runtime_error("depth32f画像のヘッダの読み込みに失敗");

	cv::Mat img(header.height, header.width, header.type);

	if (!fs.read((char*)(img.data), img.total() * img.elemSize()))
		throw std::runtime_error("depth32f画像のデータの読み込みに失敗");

	fs.close();

	if (enableMirror != (header.data1 & 1 << d1_mirror)) // ミラーするか
	{
		cv::flip(img.clone(), img, 1); // 反転
	}

	return img.clone();
}*/

std::string RealSenseProcessor::makeNameFolder(int hrgn)
{
	std::string nameFolder = "ｱ";
	nameFolder[0] = 'ｱ' + hrgn;
	return nameFolder;
}

std::string RealSenseProcessor::makeNameFail(int hrgn, int num)
{
	std::string nameFail = "ｱ-00";
	nameFail[0] = 'ｱ' + hrgn;
	nameFail[2] = '0' + num / 10;
	nameFail[3] = '0' + num % 10;
	return nameFail;
}

/*cv::Mat RealSenseProcessor::drawGuide(const cv::Mat& input, int num)
{
	cv::Point senter = cv::Point(input.cols / 2, input.rows / 2);
	cv::Mat mat = cv::Mat::zeros(input.rows, input.cols, CV_8UC1) * 0xff;
	cv::circle(mat, senter, input.rows / 2 * 0.75, cv::Scalar::all(0xff), 2);
	cv::circle(mat, senter + cv::Point(input.rows / 2 * 0.75 * cos(num * CV_PI / 4), input.rows / 2 * 0.75 * sin(num * CV_PI / 4)), input.rows / 2 * 0.25, cv::Scalar::all(0xff), 2);
	cv::circle(mat, senter + cv::Point(input.rows / 2 * 0.75 * cos(num * CV_PI / 4), input.rows / 2 * 0.75 * sin(num * CV_PI / 4)), input.rows / 2 * 0.25 - 2, cv::Scalar::all(0x00), -1);
	return mat.clone();
}*/

// 文字出力
void RealSenseProcessor::printText(int hrgn, int num)
{
	//system("cls");

	cout << "ファイルについて" << endl;
	cout << "文字：" + makeNameFolder(hrgn) << "(" << hrgn << ")" << "  ";
	cout << "番号：" << num << "  ";
	cout << "ディレクトリ：" << makeNameFolder(hrgn) + "\\" + makeNameFail(hrgn, num) << "  ";
	cout << endl;
	cout << "操作方法" << endl;
	cout << "文字：+1…w  -1…s" << endl;
	cout << "数字：+1…d  -1…a" << endl;
	cout << "保存：スペースキー(自動で次の文字に移行)" << endl;
	cout << "終了：q" << endl;
	cout << endl;
	cout << "・ピンクに手を重ねて撮る" << endl;
	cout << "・なるべく手が黄色い範囲で撮る" << endl;
	cout << "・白い枠内で撮る" << endl;
	cout << endl;
}

/*void RealSenseProcessor::shorGuideImage(const cv::Mat depth, int num)
{
	const int T = 70;
	const int B = 70;
	const int L = 50;
	const int R = 0;

	cv::vector<cv::Mat> colors(3);
	cv::Mat image;

	// 範囲指定処理
	colors[0] = depth.clone();
	colors[1] = depth.clone();
	colors[2] = depth.clone();
	cv::Mat tmp;
	threshold(depth, tmp, distMax, 0, CV_THRESH_TOZERO_INV);
	colors[1] = tmp.clone();
	colors[2] = tmp.clone();
	threshold(depth, tmp, distMin, 0, CV_THRESH_TOZERO);
	colors[1] &= tmp.clone();
	threshold(depth, tmp, distMax, 0, CV_THRESH_TOZERO);
	colors[0] = tmp.clone();
	merge(colors, image);
	image.clone().convertTo(image, CV_16UC3); // ビット演算用に変換

	cv::Mat guide(depth.rows, depth.cols, CV_8UC3, cv::Scalar::all(0)); // ガイド用行列
	const int x = (guide.cols - R - L - sizeLine) / 3, y = (guide.rows - T - B - sizeLine) / 3; // 計算用の定数
	cv::Point p(x * (2 - num % 3) + L + sizeLine / 2, y * ((num / 3) % 3) + T + sizeLine / 2); // 文字に応じての位置の計算
	circle(guide, p + cv::Point(x / 2, y / 2), 10, cv::Scalar(0xff, 0x00, 0xff), -1, CV_AA); // 丸
	rectangle(guide, cv::Point(L, T), cv::Point(guide.cols - R, guide.rows - B), cv::Scalar(0xff, 0xff, 0xff), sizeLine); // 映る範囲
	guide.clone().convertTo(guide, CV_16UC3); // 16bitに型を変換
	guide |= guide * (1 << 8); // 16bitに値を変換

	image = image.clone() ^ guide.clone(); // ガイドと画像の合成

	image.clone().convertTo(image, CV_32FC3); // カラー表示用に変換
	imshow("カメラ", image * 0x60 / 0x10000); // 表示
}*/

// http://rinov.sakura.ne.jp/wp/cpp-date
// 時刻取得
std::string RealSenseProcessor::getTime(void)
{
	//現在日時を取得する
	time_t t = time(nullptr);

	//形式を変換する    
	const tm* lt = localtime(&t);

	//sに独自フォーマットになるように連結していく
	std::stringstream s;
	s << "20";
	s << lt->tm_year - 100; // 100を引くことで20xxのxxの部分になる
	s << "年";
	s << lt->tm_mon + 1; // 月を0からカウントしているため
	s << "月";
	s << lt->tm_mday; // そのまま
	s << "日";
	s << lt->tm_hour; // 時間
	s << "時";
	s << lt->tm_min; // 分
	s << "分";

	return s.str();
}

/*void RealSenseProcessor::realsenseHandStatus(PXCHandData *handAnalyzer)
{
	PXCHandData::AlertData alertData;

	for (int i = 0; i < handAnalyzer->QueryFiredAlertsNumber(); ++i)
	{
		pxcStatus sts = handAnalyzer->QueryFiredAlertData(i, alertData);
		if (sts == PXC_STATUS_NO_ERROR)
		{
			switch (alertData.label)
			{
			case PXCHandData::ALERT_HAND_DETECTED:
			{
				wColorIO(wColorIO::PRINT_SUCCESS, L"Hand Detected.\n");
				break;
			}
			case PXCHandData::ALERT_HAND_NOT_DETECTED:
			{
				wColorIO(wColorIO::PRINT_ERROR, L"Hand Not Detected.\n");
				break;
			}
			case PXCHandData::ALERT_HAND_CALIBRATED:
			{
				wColorIO(wColorIO::PRINT_SUCCESS, L"Hand Calibrated.\n");
				break;
			}
			case PXCHandData::ALERT_HAND_NOT_CALIBRATED:
			{
				wColorIO(wColorIO::PRINT_ERROR, L"Hand Not Calibrated.\n");
				break;
			}
			case PXCHandData::ALERT_HAND_INSIDE_BORDERS:
			{
				wColorIO(wColorIO::PRINT_ERROR, L"Hand Inside Borders.\n");
				break;
			}
			case PXCHandData::ALERT_HAND_OUT_OF_BORDERS:
			{
				wColorIO(wColorIO::PRINT_SUCCESS, L"Hand Out Of Borders.\n");
				break;
			}
			}
		}
	}
}

void RealSenseProcessor::showStatus(Status sts)
{
	switch (sts)
	{
	case Intel::RealSense::NSStatus::STATUS_NO_ERROR:
		wColorIO(wColorIO::PRINT_SUCCESS, L"STATUS_NO_ERROR");
		break;
	case Intel::RealSense::NSStatus::STATUS_FEATURE_UNSUPPORTED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_FEATURE_UNSUPPORTED");
		break;
	case Intel::RealSense::NSStatus::STATUS_PARAM_UNSUPPORTED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_PARAM_UNSUPPORTED");
		break;
	case Intel::RealSense::NSStatus::STATUS_ITEM_UNAVAILABLE:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_ITEM_UNAVAILABLE");
		break;
	case Intel::RealSense::NSStatus::STATUS_HANDLE_INVALID:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_HANDLE_INVALID");
		break;
	case Intel::RealSense::NSStatus::STATUS_ALLOC_FAILED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_ALLOC_FAILED");
		break;
	case Intel::RealSense::NSStatus::STATUS_DEVICE_FAILED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_DEVICE_FAILED");
		break;
	case Intel::RealSense::NSStatus::STATUS_DEVICE_LOST:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_DEVICE_LOST");
		break;
	case Intel::RealSense::NSStatus::STATUS_DEVICE_BUSY:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_DEVICE_BUSY");
		break;
	case Intel::RealSense::NSStatus::STATUS_EXEC_ABORTED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_EXEC_ABORTED");
		break;
	case Intel::RealSense::NSStatus::STATUS_EXEC_INPROGRESS:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_EXEC_INPROGRESS");
		break;
	case Intel::RealSense::NSStatus::STATUS_EXEC_TIMEOUT:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_EXEC_TIMEOUT");
		break;
	case Intel::RealSense::NSStatus::STATUS_FILE_WRITE_FAILED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_FILE_WRITE_FAILED");
		break;
	case Intel::RealSense::NSStatus::STATUS_FILE_READ_FAILED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_FILE_READ_FAILED");
		break;
	case Intel::RealSense::NSStatus::STATUS_FILE_CLOSE_FAILED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_FILE_CLOSE_FAILED");
		break;
	case Intel::RealSense::NSStatus::STATUS_DATA_UNAVAILABLE:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_DATA_UNAVAILABLE");
		break;
	case Intel::RealSense::NSStatus::STATUS_DATA_NOT_INITIALIZED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_DATA_NOT_INITIALIZED");
		break;
	case Intel::RealSense::NSStatus::STATUS_INIT_FAILED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_INIT_FAILED");
		break;
	case Intel::RealSense::NSStatus::STATUS_STREAM_CONFIG_CHANGED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_STREAM_CONFIG_CHANGED");
		break;
	case Intel::RealSense::NSStatus::STATUS_POWER_UID_ALREADY_REGISTERED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_POWER_UID_ALREADY_REGISTERED");
		break;
	case Intel::RealSense::NSStatus::STATUS_POWER_UID_NOT_REGISTERED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_POWER_UID_NOT_REGISTERED");
		break;
	case Intel::RealSense::NSStatus::STATUS_POWER_ILLEGAL_STATE:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_POWER_ILLEGAL_STATE");
		break;
	case Intel::RealSense::NSStatus::STATUS_POWER_PROVIDER_NOT_EXISTS:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_POWER_PROVIDER_NOT_EXISTS");
		break;
	case Intel::RealSense::NSStatus::STATUS_CAPTURE_CONFIG_ALREADY_SET:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_CAPTURE_CONFIG_ALREADY_SET");
		break;
	case Intel::RealSense::NSStatus::STATUS_COORDINATE_SYSTEM_CONFLICT:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_COORDINATE_SYSTEM_CONFLICT");
		break;
	case Intel::RealSense::NSStatus::STATUS_NOT_MATCHING_CALIBRATION:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_NOT_MATCHING_CALIBRATION");
		break;
	case Intel::RealSense::NSStatus::STATUS_ACCELERATION_UNAVAILABLE:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_ACCELERATION_UNAVAILABLE");
		break;
	case Intel::RealSense::NSStatus::STATUS_WRONG_META_DATA_VALUE:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_WRONG_META_DATA_VALUE");
		break;
	case Intel::RealSense::NSStatus::STATUS_TIME_GAP:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_TIME_GAP");
		break;
	case Intel::RealSense::NSStatus::STATUS_PARAM_INPLACE:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_PARAM_INPLACE");
		break;
	case Intel::RealSense::NSStatus::STATUS_DATA_NOT_CHANGED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_DATA_NOT_CHANGED");
		break;
	case Intel::RealSense::NSStatus::STATUS_PROCESS_FAILED:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_PROCESS_FAILED");
		break;
	case Intel::RealSense::NSStatus::STATUS_VALUE_OUT_OF_RANGE:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_VALUE_OUT_OF_RANGE");
		break;
	case Intel::RealSense::NSStatus::STATUS_DATA_PENDING:
		wColorIO(wColorIO::PRINT_ERROR, L"STATUS_DATA_PENDING");
		break;
	default:
		break;
	}
	wColorIO(wColorIO::PRINT_ERROR, L"(code:%d)\n", sts);
}*/