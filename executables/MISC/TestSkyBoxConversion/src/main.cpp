#include <chaos/WinTools.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/Application.h>
#include <chaos/FileTools.h>
#include <chaos/StringTools.h>

// ====================================================================================
// Take 6 images and convert them into a single image. Then save into Temp. Show the file
// ====================================================================================

void TestConvertToSingle(boost::filesystem::path const & dst_p, boost::filesystem::path const & p, bool horizontal)
{
  boost::filesystem::path left_image   = p / "negx.jpg";  
  boost::filesystem::path front_image  = p / "posz.jpg";
  boost::filesystem::path right_image  = p / "posx.jpg";
  boost::filesystem::path back_image   = p / "negz.jpg";
  boost::filesystem::path top_image    = p / "posy.jpg";
  boost::filesystem::path bottom_image = p / "negy.jpg";

  chaos::SkyBoxImages multiple_sky_box = chaos::SkyBoxTools::LoadMultipleSkyBox(
    left_image.string().c_str(), 
    right_image.string().c_str(), 
    top_image.string().c_str(), 
    bottom_image.string().c_str(), 
    front_image.string().c_str(), 
    back_image.string().c_str());
  if (multiple_sky_box.IsMultipleImage())
  {
    static glm::vec4 back_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); 

    chaos::SkyBoxImages single_sky_box = multiple_sky_box.ToSingleImage(horizontal, back_color);
    if (single_sky_box.IsSingleImage())
    {
      FIBITMAP * image = single_sky_box.GetImage(chaos::SkyBoxImages::IMAGE_SINGLE);
      if (image != nullptr)
      {
        boost::filesystem::path dst = (horizontal)?
          dst_p / "ConvertToSingle_Horiz.png":
          dst_p / "ConvertToSingle_Vert.png";
        FreeImage_Save(FIF_PNG ,image, dst.string().c_str(), 0);
      }        
    }
  }
}

// ====================================================================================
// Take 1 image and convert it into six images. Then save into Temp. Show the directory
// ====================================================================================

void TestConvertToMultiple(boost::filesystem::path const & dst_p, boost::filesystem::path const & p, char const * dst_filename)
{
  chaos::SkyBoxImages single_sky_box = chaos::SkyBoxTools::LoadSingleSkyBox(p.string().c_str());
  if (single_sky_box.IsSingleImage())
  {
    chaos::SkyBoxImages multiple_sky_box = single_sky_box.ToMultipleImages();
    if (multiple_sky_box.IsMultipleImage())
    {
      char const * left_image   = "negx.png";  
      char const * front_image  = "posz.png";
      char const * right_image  = "posx.png";
      char const * back_image   = "negz.png";
      char const * top_image    = "posy.png";
      char const * bottom_image = "negy.png"; 

      for (int i = chaos::SkyBoxImages::IMAGE_LEFT ; i <= chaos::SkyBoxImages::IMAGE_BACK ; ++i)
      {
        FIBITMAP * image = multiple_sky_box.GetImage(i);
        if (image != nullptr)
        {
         char const * suffix = nullptr;
          if (i == chaos::SkyBoxImages::IMAGE_LEFT)
            suffix = left_image;
          else if (i == chaos::SkyBoxImages::IMAGE_RIGHT)
            suffix = right_image;
          else if (i == chaos::SkyBoxImages::IMAGE_TOP)
            suffix = top_image;
          else if (i == chaos::SkyBoxImages::IMAGE_BOTTOM)
            suffix = bottom_image;
          else if (i == chaos::SkyBoxImages::IMAGE_FRONT)
            suffix = front_image;
          else if (i == chaos::SkyBoxImages::IMAGE_BACK)
            suffix = back_image;

          std::string filename = chaos::StringTools::Printf("%s_%s", dst_filename, suffix);         
          FreeImage_Save(FIF_PNG ,image, (dst_p / filename).string().c_str(), 0);
        } 
      }
    }
  }
}

// ====================================================================================
// Take 1 image and convert it into six images. Then convert back into a single image of the opposite direction (vertical/horizontal)
// save it into Temp. Show the file
// ====================================================================================

void TestDoubleConversion(boost::filesystem::path const & dst_p, boost::filesystem::path const & p, char const * dst_filename)
{
  chaos::SkyBoxImages single_image = chaos::SkyBoxTools::LoadSingleSkyBox(p.string().c_str());
  if (single_image.IsSingleImage())
  {
    bool horizontal = single_image.IsSingleImageHorizontal();

    chaos::SkyBoxImages multiple_image = single_image.ToMultipleImages();
    if (multiple_image.IsMultipleImage())
    {
      chaos::SkyBoxImages single_image_back = multiple_image.ToSingleImage(!horizontal, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
      if (single_image_back.IsSingleImage())
      {
        FIBITMAP * image = single_image_back.GetImage(chaos::SkyBoxImages::IMAGE_SINGLE);
        if (image != nullptr)
        {
          boost::filesystem::path dst = dst_p / dst_filename;
          FreeImage_Save(FIF_PNG ,image, dst.string().c_str(), 0);
        }
      }
    }
  }
}

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  FreeImage_Initialise();

  chaos::Application * application = chaos::Application::GetInstance();

  boost::filesystem::path resources_path = application->GetResourcesPath();

  boost::filesystem::path dst_p;
  if (chaos::FileTools::CreateTemporaryDirectory("TestSkyBox", dst_p))
  {
    TestConvertToSingle(dst_p, resources_path / "Maskonaive", true);
    TestConvertToSingle(dst_p, resources_path / "Maskonaive", false);

    TestConvertToMultiple(dst_p, resources_path / "violentdays_large.jpg", "MultipleConversionViolent");
    TestConvertToMultiple(dst_p, resources_path / "originalcubecross.png", "MultipleConversionOriginalCube");

    TestDoubleConversion(dst_p, resources_path / "violentdays_large.jpg", "DoubleConversionViolent.png");
    TestDoubleConversion(dst_p, resources_path / "originalcubecross.png", "DoubleConversionOriginalCube.png");

    chaos::WinTools::ShowFile(dst_p.string().c_str());
  }

  FreeImage_DeInitialise();

  return 0;
}


