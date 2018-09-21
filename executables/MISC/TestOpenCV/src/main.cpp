#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class MyGLFWWindowOpenCVTest : public chaos::MyGLFW::Window
{
public:

	void TestFile(char const * filename)
	{
		chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
		if (application == nullptr)
			return;

		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path filepath = resources_path / "images" / filename;

		cv::Mat image = cv::imread(filepath.string().c_str(), 0 /*CV_LOAD_IMAGE_COLOR*/);

		filename = filename;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		TestFile("static_gif.gif");
		TestFile("static_png.png");
		TestFile("animated_gif.gif");
		TestFile("animated_png.png");
		return true;
	}
};



int _tmain(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenCVTest>(argc, argv, env, params);
  return 0;
}


