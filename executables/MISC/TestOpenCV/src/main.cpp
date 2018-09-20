#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	cv::Mat image = cv::imread("toto", 0 /*CV_LOAD_IMAGE_COLOR*/);




  return 0;
}


