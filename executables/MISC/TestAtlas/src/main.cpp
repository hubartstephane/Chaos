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
#include <chaos/ImageTools.h>

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

bool AddFakeBitmap(chaos::BitmapAtlas::BitmapSetInput * bitmap_set, char const * name)
{
	assert(name != nullptr);

	int w = 15 * (1 + rand() % 10);
	int h = 15 * (1 + rand() % 10);

	bool result = false;

	FIBITMAP * bitmap = FreeImage_Allocate(w, h, 32); // allocate an image
	if (bitmap != nullptr)
	{
		float color = chaos::MathTools::RandFloat();

    chaos::ImageDescription image_description = chaos::ImageTools::GetImageDescription(bitmap);

		chaos::ImageTools::FillImageBackground(image_description, glm::vec4(color, color, color, 1.0f));

		result = bitmap_set->AddBitmap(name, bitmap, true);
		if (!result)
			FreeImage_Unload(bitmap);
	}
	return result;
}

void TestAtlasDebugMode(boost::filesystem::path const & dest_p)
{
	chaos::BitmapAtlas::AtlasInput input;

	chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("bitmap_set1");

	AddFakeBitmap(bitmap_set, "A");
	AddFakeBitmap(bitmap_set, "B");
	AddFakeBitmap(bitmap_set, "C");
	AddFakeBitmap(bitmap_set, "D");
	AddFakeBitmap(bitmap_set, "E");
	AddFakeBitmap(bitmap_set, "F");
	AddFakeBitmap(bitmap_set, "G");
	AddFakeBitmap(bitmap_set, "H");
	AddFakeBitmap(bitmap_set, "I");
	AddFakeBitmap(bitmap_set, "J");

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







void TestAtlasNormalMode(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path, chaos::PixelFormatMergeParams const & merge_params = chaos::PixelFormatMergeParams())
{
	// XXX : the resources directory contains
	//        - a bmp image whose extension has been swapped to ".txt" => detected has an image
	//        - a text file                                            => not detected has an image
	// correct behavior 

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
		TestAtlasNormalMode(dst_p / "TestAtlasMixed24", resources_path / "MixedImages24"); // there is a 8bpp GRAY + 24bpp => 24 bpp atlas
		TestAtlasNormalMode(dst_p / "TestAtlasMixed32", resources_path / "MixedImages32"); // there is a 8bpp NON-GRAY picture that will be converted to 32 bpp => causing a 32bpp atlas


		chaos::PixelFormatMergeParams no_luminance_merge_params;
		no_luminance_merge_params.accept_luminance = false;
		TestAtlasNormalMode(dst_p / "TestAtlasNoLuminance", resources_path / "GrayOnlyImages", no_luminance_merge_params);
		
		TestAtlasFont(dst_p, resources_path);

		chaos::WinTools::ShowFile(dst_p.string().c_str());
	}

	chaos::Application::Finalize();

	return 0;
}


