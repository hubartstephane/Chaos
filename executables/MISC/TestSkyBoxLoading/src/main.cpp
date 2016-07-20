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

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  FreeImage_Initialise();

  chaos::Application * application = chaos::Application::GetInstance();

  boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
  boost::filesystem::path skybox_path    = resources_path / "Maskonaive";
 
  boost::filesystem::path left_image   = skybox_path / "negx.jpg";  
  boost::filesystem::path front_image  = skybox_path / "posz.jpg";
  boost::filesystem::path right_image  = skybox_path / "posx.jpg";
  boost::filesystem::path back_image   = skybox_path / "negz.jpg";
  boost::filesystem::path top_image    = skybox_path / "posy.jpg";
  boost::filesystem::path bottom_image = skybox_path / "negy.jpg";

  boost::filesystem::path square_image = resources_path / "RedSquare.bmp";

  boost::filesystem::path single_image = resources_path / "violentdays_large.jpg";


  // load multiple images  
  chaos::SkyBoxImages multiple_sky_box1 = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image.string().c_str(), right_image.string().c_str(), top_image.string().c_str(), bottom_image.string().c_str(), front_image.string().c_str(), back_image.string().c_str());

  DisplaySkyboxInfo(multiple_sky_box1, "MULTIPLE SKYBOX");

  // load multiple with unspecified  
  chaos::SkyBoxImages multiple_sky_box2 = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image.string().c_str(), nullptr, top_image.string().c_str(), bottom_image.string().c_str(), front_image.string().c_str(), back_image.string().c_str());

  DisplaySkyboxInfo(multiple_sky_box2, "MULTIPLE INCOMPLETE SKYBOX");

  // load multiple images with format error 
  chaos::SkyBoxImages multiple_sky_box3 = chaos::SkyBoxTools::LoadMultipleSkyBox(left_image.string().c_str(), square_image.string().c_str(), top_image.string().c_str(), bottom_image.string().c_str(), front_image.string().c_str(), back_image.string().c_str());

  DisplaySkyboxInfo(multiple_sky_box3, "MULTIPLE INCOMPATIBLE SKYBOX");

  // load single image
  
  chaos::SkyBoxImages single_sky_box1 = chaos::SkyBoxTools::LoadSingleSkyBox(single_image.string().c_str());

  DisplaySkyboxInfo(single_sky_box1, "SINGLE SKYBOX");

  // load single image with bad size

  chaos::SkyBoxImages single_sky_box2 = chaos::SkyBoxTools::LoadSingleSkyBox(square_image.string().c_str());

  DisplaySkyboxInfo(single_sky_box2, "SINGLE SKYBOX BAD SIZE");

  // free images
  multiple_sky_box1.Release(true);
  multiple_sky_box2.Release(true);
  multiple_sky_box3.Release(true);
  single_sky_box1.Release(true);
  single_sky_box2.Release(true);

  FreeImage_DeInitialise();

  chaos::WinTools::PressToContinue();

  return 0;
}


