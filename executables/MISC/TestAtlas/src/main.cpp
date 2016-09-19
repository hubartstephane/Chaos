#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/TextureAtlas.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>
#include <chaos/FileTools.h>
#include <chaos/FontTools.h>
#include <chaos/FontAtlas.h>

void TestAtlasReload(boost::filesystem::path const & filename)
{
  chaos::TextureAtlas atlas;
  atlas.LoadAtlas(filename);
}

void TestAtlasDebugMode(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{
  chaos::TextureAtlasInput input;

  input.AddFakeImageSource("A");
  input.AddFakeImageSource("B");
  input.AddFakeImageSource("C");
  input.AddFakeImageSource("D");
  input.AddFakeImageSource("E");
  input.AddFakeImageSource("F");
  input.AddFakeImageSource("G");
  input.AddFakeImageSource("H");
  input.AddFakeImageSource("I");
  input.AddFakeImageSource("J");

  int atlas_width   = 256;
  int atlas_height  = 256;
  int atlas_padding = 3;

  chaos::TextureAtlasHTMLOutputParams params;
  params.show_header       = true;
  params.show_atlas_header = true;  
  params.texture_scale     = 3.0f;
  params.auto_refresh      = false;  

  chaos::TextureAtlas          atlas;
  chaos::TextureAtlasGenerator generator;
  
  if (generator.ComputeResult(input, atlas, atlas_width, atlas_height, atlas_padding))
  {  
    boost::filesystem::path html_path = dest_p / "Atlas_Final.html";
    atlas.OutputToHTMLFile(html_path.string().c_str(), params);

    boost::filesystem::path dst_dir = dest_p / "AtlasResultFake" / "MyAtlas.x";
    atlas.SaveAtlas(dst_dir);
  }
}

void TestAtlasNormalMode(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{
  // XXX : the resources directory contains
  //        - a bmp image whose extension has been swapped to ".txt" => detected has an image
  //        - a text file                                            => not detected has an image
  // correct behavior 

  int atlas_width   = 512;
  int atlas_height  = 512;
  int atlas_padding = 10;

  boost::filesystem::path result_path = dest_p / "AtlasResult" / "MyAtlas.json";

  chaos::TextureAtlasGenerator::CreateAtlasFromDirectory(resources_path, result_path, atlas_width, atlas_height, atlas_padding);
}

void TestAtlasFont(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{ 
  boost::filesystem::path font_path = resources_path / "unispace bold italic.ttf";

  chaos::FontAtlas          atlas;
  chaos::FontAtlasGenerator generator;
  if (generator.GenerateTextureAtlas(nullptr, font_path.string().c_str(), atlas, nullptr))
  {
    boost::filesystem::path dst_dir = dest_p / "AtlasResultFont" / "MyAtlas";
    atlas.SaveAtlas(dst_dir);
  }
}

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::MathTools::ResetRandSeed();

  boost::filesystem::path dst_p;
  if (chaos::FileTools::CreateTemporaryDirectory("TestAtlas", dst_p))
  {
    boost::filesystem::path resources_path = chaos::Application::GetInstance()->GetResourcesPath();

    TestAtlasDebugMode(dst_p, resources_path);
    TestAtlasNormalMode(dst_p, resources_path);
    TestAtlasFont(dst_p, resources_path);

    TestAtlasReload(dst_p / "AtlasResult" / "MyAtlas");

    chaos::WinTools::ShowFile(dst_p.string().c_str());
  }

  chaos::WinTools::PressToContinue();

  chaos::Application::Finalize();

  return 0;
}


