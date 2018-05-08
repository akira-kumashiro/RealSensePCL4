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
	case CV_WAITKEY_CURSORKEY_TOP:num++;	break; // ���� + 1
	case CV_WAITKEY_CURSORKEY_BOTTOM:num--;	break; // ���� - 1
	case CV_WAITKEY_CURSORKEY_RIGHT:hrgn++;	break; // ���� + 1
	case CV_WAITKEY_CURSORKEY_LEFT:hrgn--;	break; // ���� - 1
	case ' ': // �ۑ�
		CreateDirectory((_time).c_str(), NULL); // ��{�̃t�H���_�쐬�i���O�����ԁj
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-Color").c_str(), NULL); // color�摜�t�H���_�쐬
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-Depth").c_str(), NULL); // depth�摜�t�H���_�쐬
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-HandImage").c_str(), NULL); // HandImage�摜�t�H���_�쐬
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-HandPoint").c_str(), NULL); // HandPoint�摜�t�H���_�쐬
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-PCLHand").c_str(), NULL); // PCLHand�摜�t�H���_�쐬
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-PCLJoint").c_str(), NULL); // PCLJoint�摜�t�H���_�쐬
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-PCLCamera").c_str(), NULL); // PCLCamera�摜�t�H���_�쐬
		CreateDirectory((_time + "\\" + makeNameFolder(hrgn) + "-Depth����p").c_str(), NULL); // depth����p�摜�t�H���_�쐬
		for (int i = 0; i < NUM; i++)
		{
			rsu[i].saveData(_time + "\\" + makeNameFolder(hrgn), "\\" + makeNameFail(hrgn, num) + "(" + std::to_string(i) + ")");
		}
		num++;
		break;
	case 'q': // �I��
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
	else if ('�' - '�' < hrgn)
	{
		num = numMax - 1;
		hrgn = '�' - '�';
	}
	printText(hrgn, num);
	// �{�^���������ꂽ�Ƃ��̂ݕ\��
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
	std::string nameFolder = "�";
	nameFolder[0] = '�' + hrgn;
	return nameFolder;
}

std::string RealSenseProcessor::makeNameFail(int hrgn, int num)
{
	std::string nameFail = "�-00";
	nameFail[0] = '�' + hrgn;
	nameFail[2] = '0' + num / 10;
	nameFail[3] = '0' + num % 10;
	return nameFail;
}

// �����o��
void RealSenseProcessor::printText(int hrgn, int num)
{
	//system("cls");

	cout << "�t�@�C���ɂ���" << endl;
	cout << "�����F" + makeNameFolder(hrgn) << "(" << hrgn << ")" << "  ";
	cout << "�ԍ��F" << num << "  ";
	cout << "�f�B���N�g���F" << makeNameFolder(hrgn) + "\\" + makeNameFail(hrgn, num) << "  ";
	cout << endl;
	cout << "������@" << endl;
	cout << "�����F+1�cw  -1�cs" << endl;
	cout << "�����F+1�cd  -1�ca" << endl;
	cout << "�ۑ��F�X�y�[�X�L�[(�����Ŏ��̕����Ɉڍs)" << endl;
	cout << "�I���Fq" << endl;
	cout << endl;
	cout << "�E�s���N�Ɏ���d�˂ĎB��" << endl;
	cout << "�E�Ȃ�ׂ��肪���F���͈͂ŎB��" << endl;
	cout << "�E�����g���ŎB��" << endl;
	cout << endl;
}

// http://rinov.sakura.ne.jp/wp/cpp-date
// �����擾
std::string RealSenseProcessor::getTime(void)
{
	//���ݓ������擾����
	time_t t = time(nullptr);

	//�`����ϊ�����    
	const tm* lt = localtime(&t);

	//s�ɓƎ��t�H�[�}�b�g�ɂȂ�悤�ɘA�����Ă���
	std::stringstream s;
	s << "20";
	s << lt->tm_year - 100; // 100���������Ƃ�20xx��xx�̕����ɂȂ�
	s << "�N";
	s << lt->tm_mon + 1; // ����0����J�E���g���Ă��邽��
	s << "��";
	s << lt->tm_mday; // ���̂܂�
	s << "��";
	s << lt->tm_hour; // ����
	s << "��";
	s << lt->tm_min; // ��
	s << "��";

	return s.str();
}