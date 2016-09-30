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

static int ATLAS_BPP = 8;

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

  chaos::TextureAtlas                atlas;
  chaos::TextureAtlasGenerator       generator;
  chaos::TextureAtlasGeneratorParams params = chaos::TextureAtlasGeneratorParams(256, 256, 3, ATLAS_BPP);
  
  if (generator.ComputeResult(input, atlas, params))
  {  
    chaos::TextureAtlasHTMLOutputParams html_params;
    html_params.show_header       = true;
    html_params.show_atlas_header = true;
    html_params.texture_scale     = 3.0f;
    html_params.auto_refresh      = false;

    boost::filesystem::path html_path = dest_p / "Atlas_Final.html";
    atlas.OutputToHTMLFile(html_path.string().c_str(), html_params);

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

  chaos::TextureAtlasGeneratorParams params = chaos::TextureAtlasGeneratorParams(512, 512, 10, ATLAS_BPP);

  boost::filesystem::path result_path = dest_p / "AtlasResult" / "MyAtlas.json";

  chaos::TextureAtlasGenerator::CreateAtlasFromDirectory(resources_path, result_path, params);
}

void TestAtlasFont(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{ 
  boost::filesystem::path font_path = resources_path / "unispace bold italic.ttf";

  boost::filesystem::path dst_dir1 = dest_p / "AtlasResultFont" / "MyAtlas";
  boost::filesystem::path dst_dir2 = dest_p / "AtlasResultFontReloaded" / "MyAtlas";

  chaos::FontAtlas          atlas;
  chaos::FontAtlasGenerator generator;
  if (generator.GenerateTextureAtlas(nullptr, font_path.string().c_str(), atlas, nullptr))   
    atlas.SaveAtlas(dst_dir1);

  chaos::FontAtlas atlas_reloaded;
  if (atlas_reloaded.LoadAtlas(dst_dir1))
    atlas_reloaded.SaveAtlas(dst_dir2);
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

  chaos::Application::Finalize();

  return 0;
}


