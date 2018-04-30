#include "RealSenseProcessor.h"
#include <iostream>
#include <Windows.h>

int wmain(int argc, WCHAR* argv[])
{
	try
	{
		RealSenseProcessor *rsp;
		rsp = new RealSenseProcessor();

		while (1)
		{
			if (rsp->run() == false)
			{
				delete rsp;
				return FALSE;
			}
			else
			{
				delete rsp;
				rsp = new RealSenseProcessor();
			}
		}
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}
