#include <chaos/WinTools.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/Application.h>

void DisplaySkyboxInfo(chaos::SkyBoxImages const & skybox, char const * title)
{
	std::cout << title << " GetSkyBoxSize           : " << skybox.GetSkyBoxSize() << std::endl;
	std::cout << title << " IsEmpty                 : " << skybox.IsEmpty() << std::endl;
	std::cout << title << " IsSingleImage           : " << skybox.IsSingleImage() << std::endl;
	std::cout << title << " IsSingleImageHorizontal : " << skybox.IsSingleImageHorizontal() << std::endl;
	std::cout << title << " IsSingleImageVertical   : " << skybox.IsSingleImageVertical() << std::endl;
	std::cout << title << " IsMultipleImage         : " << skybox.IsMultipleImage() << std::endl;
	std::cout << title << " IsMultipleImageComplete : " << skybox.IsMultipleImageComplete() << std::endl;
}


class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		boost::filesystem::path const & rp = GetResourcesPath();
		boost::filesystem::path skybox_path    = rp / "Maskonaive";

		boost::filesystem::path left_image   = skybox_path / "negx.jpg";  
		boost::filesystem::path front_image  = skybox_path / "posz.jpg";
		boost::filesystem::path right_image  = skybox_path / "posx.jpg";
		boost::filesystem::path back_image   = skybox_path / "negz.jpg";
		boost::filesystem::path top_image    = skybox_path / "posy.jpg";
		boost::filesystem::path bottom_image = skybox_path / "negy.jpg";

		boost::filesystem::path square_image = rp / "RedSquare.bmp";

		boost::filesystem::path single_image = rp / "violentdays_large.jpg";


		// load multiple images  
		chaos::SkyBoxImages multiple_sky_box1 = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image, right_image, top_image, bottom_image, front_image, back_image);

		DisplaySkyboxInfo(multiple_sky_box1, "MULTIPLE SKYBOX");

		// load multiple with unspecified  
		chaos::SkyBoxImages multiple_sky_box2 = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image, nullptr, top_image, bottom_image, front_image, back_image);

		DisplaySkyboxInfo(multiple_sky_box2, "MULTIPLE INCOMPLETE SKYBOX");

		// load multiple images with format error 
		chaos::SkyBoxImages multiple_sky_box3 = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image, square_image, top_image, bottom_image, front_image, back_image);

		DisplaySkyboxInfo(multiple_sky_box3, "MULTIPLE INCOMPATIBLE SKYBOX");

		// load single image

		chaos::SkyBoxImages single_sky_box1 = chaos::SkyBoxTools::LoadSingleSkyBox(single_image);

		DisplaySkyboxInfo(single_sky_box1, "SINGLE SKYBOX");

		// load single image with bad size

		chaos::SkyBoxImages single_sky_box2 = chaos::SkyBoxTools::LoadSingleSkyBox(square_image);

		DisplaySkyboxInfo(single_sky_box2, "SINGLE SKYBOX BAD SIZE");

		chaos::WinTools::PressToContinue();

		return true;
	}
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);

	return 0;
}


