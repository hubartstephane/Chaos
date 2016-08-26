#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/TextureAtlas.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>


boost::filesystem::path application_path;
boost::filesystem::path resources_path;

void TestAtlasDebugMode()
{
  chaos::TextureAtlasData data;
  data.SetDebugMode(true);

  data.AddTextureFile("A");
  data.AddTextureFile("B");
  data.AddTextureFile("C");
  data.AddTextureFile("D");
  data.AddTextureFile("E");
  data.AddTextureFile("F");
  data.AddTextureFile("G");
  data.AddTextureFile("H");
  data.AddTextureFile("I");
  data.AddTextureFile("J");

  int atlas_width   = 256;
  int atlas_height  = 256;
  int atlas_padding = 3;

  chaos::TextureAtlasHTMLOutputParams params;
  params.show_header       = true;
  params.show_atlas_header = true;  
  params.texture_scale     = 3.0f;
  params.auto_refresh      = true;  

  chaos::TextureAtlasCreator atlas_creator;
  
  if (atlas_creator.ComputeResult(data, atlas_width, atlas_height, atlas_padding))
  {  
    boost::filesystem::path html_path = chaos::WinTools::GetUserDesktopPath() / "Atlas_Final.html";
    atlas_creator.OutputToHTMLFile(html_path.string().c_str(), params);
  }
}

void TestAtlasNormalMode()
{
  // XXX : the resources directory contains
  //        - a bmp image whose extension has been swapped to ".txt" => detected has an image
  //        - a text file                                            => not detected has an image
  // correct behavior 

  int atlas_width   = 512;
  int atlas_height  = 512;
  int atlas_padding = 10;

  boost::filesystem::path result_path = chaos::WinTools::GetUserDesktopPath() / "AtlasResult" / "MyAtlas";

  chaos::TextureAtlasCreator::CreateAtlasFromDirectory(
    resources_path.string().c_str(), 
    result_path.string().c_str(), 
    atlas_width, 
    atlas_height, 
    atlas_padding);
}


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  FreeImage_Initialise();

  chaos::MathTools::ResetRandSeed();

  application_path = chaos::Application::GetInstance()->GetApplicationPath();
  resources_path   = chaos::Application::GetInstance()->GetResourcesPath(); 

  TestAtlasDebugMode();
  TestAtlasNormalMode();

  FreeImage_DeInitialise();

  chaos::WinTools::PressToContinue();

  return 0;
}


