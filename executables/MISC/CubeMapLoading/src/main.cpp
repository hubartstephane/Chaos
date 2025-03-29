#include "chaos/Chaos.h"

void DisplaySkyboxInfo(chaos::CubeMapImages const & cubemap, char const * title)
{
	std::cout << title << " GetCubeMapSize           : " << cubemap.GetCubeMapSize() << std::endl;
	std::cout << title << " IsGeometryEmpty                 : " << cubemap.IsEmpty() << std::endl;
	std::cout << title << " IsSingleImage           : " << cubemap.IsSingleImage() << std::endl;
	std::cout << title << " IsSingleImageHorizontal : " << cubemap.IsSingleImageHorizontal() << std::endl;
	std::cout << title << " IsSingleImageVertical   : " << cubemap.IsSingleImageVertical() << std::endl;
	std::cout << title << " IsMultipleImage         : " << cubemap.IsMultipleImage() << std::endl;
	std::cout << title << " IsMultipleImageComplete : " << cubemap.IsMultipleImageComplete() << std::endl;
}


class MyApplication : public chaos::Application
{
protected:

	virtual int Main() override
	{
		boost::filesystem::path const & rp = GetResourcesPath();
		boost::filesystem::path cubemap_path    = rp / "Maskonaive";

		boost::filesystem::path left_image   = cubemap_path / "negx.jpg";
		boost::filesystem::path front_image  = cubemap_path / "posz.jpg";
		boost::filesystem::path right_image  = cubemap_path / "posx.jpg";
		boost::filesystem::path back_image   = cubemap_path / "negz.jpg";
		boost::filesystem::path top_image    = cubemap_path / "posy.jpg";
		boost::filesystem::path bottom_image = cubemap_path / "negy.jpg";

		boost::filesystem::path square_image = rp / "RedSquare.bmp";

		boost::filesystem::path single_image = rp / "violentdays_large.jpg";


		// load multiple images
		chaos::CubeMapImages multiple_sky_box1 = chaos::CubeMapTools::LoadMultipleCubeMap(left_image, right_image, top_image, bottom_image, front_image, back_image);

		DisplaySkyboxInfo(multiple_sky_box1, "MULTIPLE SKYBOX");

		// load multiple with unspecified
		chaos::CubeMapImages multiple_sky_box2 = chaos::CubeMapTools::LoadMultipleCubeMap(left_image, nullptr, top_image, bottom_image, front_image, back_image);

		DisplaySkyboxInfo(multiple_sky_box2, "MULTIPLE INCOMPLETE SKYBOX");

		// load multiple images with format error
		chaos::CubeMapImages multiple_sky_box3 = chaos::CubeMapTools::LoadMultipleCubeMap(left_image, square_image, top_image, bottom_image, front_image, back_image);

		DisplaySkyboxInfo(multiple_sky_box3, "MULTIPLE INCOMPATIBLE SKYBOX");

		// load single image

		chaos::CubeMapImages single_sky_box1 = chaos::CubeMapTools::LoadSingleCubeMap(single_image);

		DisplaySkyboxInfo(single_sky_box1, "SINGLE SKYBOX");

		// load single image with bad size

		chaos::CubeMapImages single_sky_box2 = chaos::CubeMapTools::LoadSingleCubeMap(square_image);

		DisplaySkyboxInfo(single_sky_box2, "SINGLE SKYBOX BAD SIZE");

		chaos::WinTools::PressToContinue();

		return 0;
	}
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}