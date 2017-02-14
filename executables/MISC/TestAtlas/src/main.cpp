#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/BitmapAtlasHTMLGenerator.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>
#include <chaos/FileTools.h>
#include <chaos/FontTools.h>

static int ATLAS_BPP = 0;
static int ATLAS_PADDING = 10;

void SaveAtlasAndOutputAtlasToHTML(chaos::BitmapAtlas::Atlas & atlas, boost::filesystem::path const & dest_p, boost::filesystem::path const & html_dest_p)
{
	atlas.SaveAtlas(dest_p);

	chaos::BitmapAtlas::AtlasHTMLOutputParams html_params;
	html_params.show_header = true;
	html_params.show_atlas_header = true;
	html_params.texture_scale = 3.0f;
	html_params.auto_refresh = false;

	chaos::BitmapAtlas::AtlasHTMLGenerator::OutputToHTMLFile(atlas, html_dest_p.string().c_str(), html_params);
}

void TestAtlasDebugMode(boost::filesystem::path const & dest_p)
{
	chaos::BitmapAtlas::AtlasInput input;

	chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("bitmap_set1");

	bitmap_set->AddFakeBitmap("A");
	bitmap_set->AddFakeBitmap("B");
	bitmap_set->AddFakeBitmap("C");
	bitmap_set->AddFakeBitmap("D");
	bitmap_set->AddFakeBitmap("E");
	bitmap_set->AddFakeBitmap("F");
	bitmap_set->AddFakeBitmap("G");
	bitmap_set->AddFakeBitmap("H");
	bitmap_set->AddFakeBitmap("I");
	bitmap_set->AddFakeBitmap("J");

	chaos::PixelFormatMergeParams            merge_params;
	chaos::BitmapAtlas::Atlas                atlas;
	chaos::BitmapAtlas::AtlasGenerator       generator;
	chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(256, 256, ATLAS_PADDING, merge_params);

	if (generator.ComputeResult(input, atlas, params))
		SaveAtlasAndOutputAtlasToHTML(atlas, dest_p / "MyAtlas.x", dest_p / "atlas.html");
}

void TestAtlasFont(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{
	boost::filesystem::path font_path1 = resources_path / "unispace bold italic.ttf";
	boost::filesystem::path font_path2 = resources_path / "unispace.ttf";

	boost::filesystem::path dst_dir1 = dest_p / "AtlasResultFont";
	boost::filesystem::path dst_dir2 = dest_p / "AtlasResultFontReloaded";

	chaos::PixelFormatMergeParams      merge_params;
	chaos::BitmapAtlas::Atlas          atlas;
	chaos::BitmapAtlas::AtlasGenerator generator;
	chaos::BitmapAtlas::AtlasInput     input;

	chaos::BitmapAtlas::CharacterSetInput * character_set1 =
		input.AddCharacterSet("character_set1", nullptr, font_path1.string().c_str(), nullptr, true, chaos::BitmapAtlas::CharacterSetInputParams());

	chaos::BitmapAtlas::CharacterSetInput * character_set2 =
		input.AddCharacterSet("character_set2", nullptr, font_path2.string().c_str(), nullptr, true, chaos::BitmapAtlas::CharacterSetInputParams());

	chaos::BitmapAtlas::BitmapSetInput * bitmap_set =
		input.AddBitmapSet("bitmap_set1");

	bitmap_set->AddBitmapFilesFromDirectory(resources_path / "ButtonImages");

	chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(512, 512, ATLAS_PADDING, merge_params);
	if (generator.ComputeResult(input, atlas, params))
		SaveAtlasAndOutputAtlasToHTML(atlas, dst_dir1 / "MyAtlas", dst_dir1 / "MyAtlas.html");

	chaos::BitmapAtlas::Atlas atlas_reloaded;
	if (atlas_reloaded.LoadAtlas(dst_dir1 / "MyAtlas"))
		SaveAtlasAndOutputAtlasToHTML(atlas_reloaded, dst_dir2 / "MyAtlas", dst_dir2 / "MyAtlas.html");
}







void TestAtlasNormalMode(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{
	// XXX : the resources directory contains
	//        - a bmp image whose extension has been swapped to ".txt" => detected has an image
	//        - a text file                                            => not detected has an image
	// correct behavior 

	chaos::PixelFormatMergeParams merge_params;

	chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(512, 512, ATLAS_PADDING, merge_params);

	boost::filesystem::path result_path = dest_p / "MyAtlas.json";

	chaos::BitmapAtlas::AtlasGenerator::CreateAtlasFromDirectory(resources_path, result_path, params);
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

		TestAtlasDebugMode(dst_p / "TestAtlasDebugMode");
		TestAtlasNormalMode(dst_p / "TestAtlasNormalMode", resources_path / "Images");
		TestAtlasNormalMode(dst_p / "TestAtlasGrayOnly", resources_path / "GrayOnlyImages");
		TestAtlasNormalMode(dst_p / "TestAtlasMixed", resources_path / "MixedImages24");
		TestAtlasNormalMode(dst_p / "TestAtlasMixed", resources_path / "MixedImages32");
		TestAtlasFont(dst_p, resources_path);

		chaos::WinTools::ShowFile(dst_p.string().c_str());
	}

	chaos::Application::Finalize();

	return 0;
}


