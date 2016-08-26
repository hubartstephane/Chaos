#include <chaos/WinTools.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/Application.h>
#include <chaos/FileTools.h>

// ====================================================================================
// Take 6 images and convert them into a single image. Then save into Temp. Show the file
// ====================================================================================

void TestConvertToSingle(boost::filesystem::path const & p, bool horizontal)
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
    boost::filesystem::path dst_p;
    if (chaos::FileTools::CreateTemporaryDirectory("toto", dst_p))
    {
      static glm::vec4 back_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); 

      chaos::SkyBoxImages single_sky_box = multiple_sky_box.ToSingleImage(horizontal, back_color);
      if (single_sky_box.IsSingleImage())
      {
        FIBITMAP * image = single_sky_box.GetImage(chaos::SkyBoxImages::IMAGE_SINGLE);
        if (image != nullptr)
        {
          boost::filesystem::path dst = dst_p / "image.png";
          if (FreeImage_Save(FIF_PNG ,image, dst.string().c_str(), 0))
            chaos::WinTools::ShowFile(dst.string().c_str());
        }        
        single_sky_box.Release(true);        
      }
    }
  }
  multiple_sky_box.Release(true);
}

// ====================================================================================
// Take 1 image and convert it into six images. Then save into Temp. Show the directory
// ====================================================================================

void TestConvertToMultiple(boost::filesystem::path const & p)
{
  chaos::SkyBoxImages single_sky_box = chaos::SkyBoxTools::LoadSingleSkyBox(p.string().c_str());
  if (single_sky_box.IsSingleImage())
  {
    boost::filesystem::path dst_p;
    if (chaos::FileTools::CreateTemporaryDirectory("toto", dst_p))
    {
      chaos::SkyBoxImages multiple_sky_box = single_sky_box.ToMultipleImages();
      if (multiple_sky_box.IsMultipleImage())
      {
        boost::filesystem::path left_image   = dst_p / "negx.png";  
        boost::filesystem::path front_image  = dst_p / "posz.png";
        boost::filesystem::path right_image  = dst_p / "posx.png";
        boost::filesystem::path back_image   = dst_p / "negz.png";
        boost::filesystem::path top_image    = dst_p / "posy.png";
        boost::filesystem::path bottom_image = dst_p / "negy.png"; 

        for (int i = chaos::SkyBoxImages::IMAGE_LEFT ; i <= chaos::SkyBoxImages::IMAGE_BACK ; ++i)
        {
          FIBITMAP * image = multiple_sky_box.GetImage(i);
          if (image != nullptr)
          {
            boost::filesystem::path dst;
            if (i == chaos::SkyBoxImages::IMAGE_LEFT)
              dst = left_image;
            else if (i == chaos::SkyBoxImages::IMAGE_RIGHT)
              dst = right_image;
            else if (i == chaos::SkyBoxImages::IMAGE_TOP)
              dst = top_image;
            else if (i == chaos::SkyBoxImages::IMAGE_BOTTOM)
              dst = bottom_image;
            else if (i == chaos::SkyBoxImages::IMAGE_FRONT)
              dst = front_image;
            else if (i == chaos::SkyBoxImages::IMAGE_BACK)
              dst = back_image;
                         
            FreeImage_Save(FIF_PNG ,image, dst.string().c_str(), 0);
          } 
        }
        multiple_sky_box.Release(true);
      }

      chaos::WinTools::ShowFile(dst_p.string().c_str());
    }
  }
  single_sky_box.Release(true);
}

// ====================================================================================
// Take 1 image and convert it into six images. Then convert back into a single image of the opposite direction (vertical/horizontal)
// save it into Temp. Show the file
// ====================================================================================

void TestDoubleConversion(boost::filesystem::path const & p)
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
        boost::filesystem::path dst_p;
        if (chaos::FileTools::CreateTemporaryDirectory("toto", dst_p))
        {
          FIBITMAP * image = single_image_back.GetImage(chaos::SkyBoxImages::IMAGE_SINGLE);
          if (image != nullptr)
          {
            boost::filesystem::path dst = dst_p / "image.png";
            if (FreeImage_Save(FIF_PNG ,image, dst.string().c_str(), 0))
              chaos::WinTools::ShowFile(dst.string().c_str());
          }
        }
        single_image_back.Release(true);
      }
      multiple_image.Release(true);
    }
    single_image.Release(true);
  }
}

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  FreeImage_Initialise();

  chaos::Application * application = chaos::Application::GetInstance();

  boost::filesystem::path resources_path = application->GetResourcesPath();

  TestConvertToSingle(resources_path / "Maskonaive", true);
  TestConvertToSingle(resources_path / "Maskonaive", false);

  TestConvertToMultiple(resources_path / "violentdays_large.jpg");
  TestConvertToMultiple(resources_path / "originalcubecross.png");

  TestDoubleConversion(resources_path / "violentdays_large.jpg");
  TestDoubleConversion(resources_path / "originalcubecross.png");

  FreeImage_DeInitialise();

  return 0;
}


