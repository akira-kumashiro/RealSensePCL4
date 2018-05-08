#include "RealSenseProcessor.h"

RealSenseProcessor::RealSenseProcessor() :
	//viewer(new pcl::visualization::PCLVisualizer("3D Viewer")),
	rsu(std::vector<RealSenseUpdater>(NUM))
{
	wColorIO(wColorIO::PRINT_INFO, L"RSP>");
	wColorIO(wColorIO::PRINT_INFO, L"Start\n");

	//viewer->registerKeyboardCallback(&RealSenseProcessor::keyboardCallback, *this);

	isContinue = false;
	isUserInterrupt = false;
	_time = getTime();
}


RealSenseProcessor::~RealSenseProcessor()
{
	for (int i = 0; i < NUM; i++)
	{
		
	}

	//viewer->close();

	cv::destroyAllWindows();

	wColorIO(wColorIO::PRINT_INFO, L"RSP>");
	wColorIO(wColorIO::PRINT_SUCCESS, L"Exiting\n");
}

bool RealSenseProcessor::run(void)
{
	for (int i = 0; i < NUM; i++)
	{
		wColorIO(wColorIO::PRINT_INFO, L"RSP>");
		wColorIO(wColorIO::PRINT_SUCCESS, L"Prosessing #%d device.\n",i);

		if (rsu[i].init() < RealSenseUpdater::RSU_NO_ERROR)
		{
			return false;
		}
	}
	
	wColorIO(wColorIO::PRINT_INFO, L"RSP>");
	wColorIO(wColorIO::PRINT_SUCCESS, L"init finish\n");

	while (1)
	{
		for (int i = 0; i < NUM; i++)
		{
			wColorIO(wColorIO::PRINT_INFO, L"RSP>");
			wColorIO(wColorIO::PRINT_SUCCESS, L"Prosessing #%d device.\n", i);
			rsu[i].setLaserPower(POWER);
			if (rsu[i].run() < RealSenseUpdater::RSU_NO_ERROR)
				return false;
			//printf_s("\n%d\n",rsu[i].run());
			if (NUM > 1)
				rsu[i].setLaserPower(0);
		}
	}

	return true;
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
		for (int i = 0; i < NUM; i++)
		{
			rsu[i].saveData(_time + "\\" + makeNameFolder(hrgn), "\\" + makeNameFail(hrgn, num) + "(" + std::to_string(i) + ")");
		}
		num++;
		break;
	case 'q': // 終了
		//viewer->close();
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

void RealSenseProcessor::updateViewerText(void)
{
	std::vector<boost::format> entries;

	const int dx = 5;
	const int dy = 14;
	const int fs = 10;
	boost::format name_fmt("text%i");

	for (size_t i = 0; i < entries.size(); ++i)
	{
//		std::string name = boost::str(name_fmt % i);
//		std::string entry = boost::str(entries[i]);
//		if (!viewer->updateText(entry, dx, dy + i * (fs + 2), fs, 1.0, 1.0, 1.0, name))
//			viewer->addText(entry, dx, dy + i * (fs + 2), fs, 1.0, 1.0, 1.0, name);
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