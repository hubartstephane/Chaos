#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>

class MyApplication : public chaos::Application
{
protected:

	float GetPixelAlpha(glm::ivec2 v)
	{

	}

	int GetLayerCount(int w, int h)
	{
		int d = std::min(w, h);

		auto x = std::clamp(5, 6, 11);

		

		return 0;

	}





	virtual bool Main() override
	{
		auto [i, j] = std::make_pair(3, 4);




		int l = GetLayerCount(16, 9);




		return true;
	}
};

int CHAOS_MAIN(int argc, char** argv, char** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);

	return 0;
}


