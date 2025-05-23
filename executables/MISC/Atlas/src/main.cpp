#include "chaos/Chaos.h"

static int ATLAS_BPP = 0;
static int ATLAS_PADDING = 10;

void SaveAtlasAndOutputAtlasToHTML(chaos::Atlas & atlas, chaos::FilePathParam const & dest_p, chaos::FilePathParam const & html_dest_p)
{
	atlas.SaveAtlas(dest_p);

	chaos::AtlasHTMLOutputParams html_params;
	html_params.show_header = true;
	html_params.show_atlas_header = true;
	html_params.texture_scale = 3.0f;
	html_params.auto_refresh = false;

	chaos::AtlasHTMLGenerator html_generator;
	html_generator.OutputToHTMLFile(atlas, html_dest_p, html_params);
}

bool AddFakeBitmap(chaos::AtlasFolderInfoInput * folder_input, char const * name)
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

		chaos::AtlasBitmapInfoInput * result = folder_input->AddBitmap(bitmap, true, name, 0);
		if (result == nullptr)
			FreeImage_Unload(bitmap);
	}
	return result;
}

void TestAtlasDebugMode(boost::filesystem::path const & dest_p)
{
	chaos::AtlasInput input;

	chaos::AtlasFolderInfoInput * folder_input = input.AddFolder("folder_input1", 0);

	AddFakeBitmap(folder_input, "A");
	AddFakeBitmap(folder_input, "B");
	AddFakeBitmap(folder_input, "C");
	AddFakeBitmap(folder_input, "D");
	AddFakeBitmap(folder_input, "E");
	AddFakeBitmap(folder_input, "F");
	AddFakeBitmap(folder_input, "G");
	AddFakeBitmap(folder_input, "H");
	AddFakeBitmap(folder_input, "I");
	AddFakeBitmap(folder_input, "J");

	chaos::PixelFormatMergeParams            merge_params;
	chaos::Atlas                atlas;
	chaos::AtlasGenerator       generator;
	chaos::AtlasGeneratorParams params = chaos::AtlasGeneratorParams(256, 256, ATLAS_PADDING, merge_params);

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
	chaos::Atlas          atlas;
	chaos::AtlasGenerator generator;
	chaos::AtlasInput     input;

	input.AddFont(font_path1.string().c_str(), nullptr, true, "font_info1", 0, chaos::AtlasFontInfoInputParams());

	input.AddFont(font_path2.string().c_str(), nullptr, true, "font_info2", 0, chaos::AtlasFontInfoInputParams());

	chaos::AtlasFolderInfoInput * folder_input =
		input.AddFolder("folder_input1", 0);

	folder_input->AddBitmapFilesFromDirectory(resources_path / "Images", true);

	chaos::AtlasGeneratorParams params = chaos::AtlasGeneratorParams(512, 512, ATLAS_PADDING, merge_params);
	if (generator.ComputeResult(input, atlas, params))
		SaveAtlasAndOutputAtlasToHTML(atlas, dst_dir1 / "MyAtlas", dst_dir1 / "MyAtlas.html");

	chaos::Atlas atlas_reloaded;
	if (atlas_reloaded.LoadAtlas(dst_dir1 / "MyAtlas"))
		SaveAtlasAndOutputAtlasToHTML(atlas_reloaded, dst_dir2 / "MyAtlas", dst_dir2 / "MyAtlas.html");
}







void TestAtlasNormalMode(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path, chaos::PixelFormatMergeParams const & merge_params = chaos::PixelFormatMergeParams())
{
	// XXX : the resources directory contains
	//        - a bmp image whose extension has been swapped to ".txt" => detected has an image
	//        - a text file                                            => not detected has an image
	// correct behavior

	chaos::AtlasGeneratorParams params = chaos::AtlasGeneratorParams(512, 512, ATLAS_PADDING, merge_params);

	boost::filesystem::path result_path = dest_p / "MyAtlas.json";

	chaos::AtlasGenerator::CreateAtlasFromDirectory(resources_path, result_path, true, params);
}







class MyApplication : public chaos::Application
{
protected:

	void TestGIF()
	{




#if 0

		boost::filesystem::path ip = GetResourcesPath() / "images";

		FIMULTIBITMAP * a1 = chaos::ImageTools::LoadMultiImageFromFile(ip / "moving_gif.gif");
		FIBITMAP      * b1 = chaos::ImageTools::LoadImageFromFile(ip / "moving_gif.gif");

		FIMULTIBITMAP * a2 = chaos::ImageTools::LoadMultiImageFromFile(ip / "static_gif.gif");
		FIBITMAP      * b2 = chaos::ImageTools::LoadImageFromFile(ip / "static_gif.gif");

		FIMULTIBITMAP * a3 = chaos::ImageTools::LoadMultiImageFromFile(ip / "logo_jpg.jpg");
		FIBITMAP      * b3 = chaos::ImageTools::LoadImageFromFile(ip / "logo_jpg.jpg");

		a1 = a1;
		a2 = a2;
		a3 = a3;

		b1 = b1;
		b2 = b2;
		b3 = b3;
#endif

		chaos::BitmapGridAnimationInfo info1;
		std::string s1;
		bool i1 = chaos::BitmapGridAnimationInfo::ParseFromName("truc", info1, &s1);

		chaos::BitmapGridAnimationInfo info2;
		std::string s2;
		bool i2 = chaos::BitmapGridAnimationInfo::ParseFromName("truc.png", info2, &s2);

		chaos::BitmapGridAnimationInfo info3;
		std::string s3;
		bool i3 = chaos::BitmapGridAnimationInfo::ParseFromName("truc.", info3, &s3);

		chaos::BitmapGridAnimationInfo info4;
		std::string s4;
		bool i4 = chaos::BitmapGridAnimationInfo::ParseFromName("_15x12.png", info4, &s4);

		chaos::BitmapGridAnimationInfo info5;
		std::string s5;
		bool i5 = chaos::BitmapGridAnimationInfo::ParseFromName("_15x12s17.png", info5, &s5);

		chaos::BitmapGridAnimationInfo info6;
		std::string s6;
		bool i6 = chaos::BitmapGridAnimationInfo::ParseFromName("o_15x12.png", info6, &s6);

		chaos::BitmapGridAnimationInfo info7;
		std::string s7;
		bool i7 = chaos::BitmapGridAnimationInfo::ParseFromName("o_15x12s17.png", info7, &s7);

		i1 = i1;
		i2 = i2;
		i3 = i3;
		i4 = i4;
		i5 = i5;
	}

	virtual int Main() override
	{
		TestGIF();








		chaos::MathTools::ResetRandSeed();

		boost::filesystem::path dst_p;
		if (chaos::FileTools::CreateTemporaryDirectory("TestAtlas", dst_p))
		{
			boost::filesystem::path const & rp = GetResourcesPath();

			TestAtlasDebugMode(dst_p / "TestAtlasDebugMode");
			TestAtlasNormalMode(dst_p / "TestAtlasNormalMode", rp / "Images");
			TestAtlasNormalMode(dst_p / "TestAtlasGrayOnly", rp / "GrayOnlyImages");
			TestAtlasNormalMode(dst_p / "TestAtlasMixed24", rp / "MixedImages24"); // there is a 8bpp GRAY + 24bpp => 24 bpp atlas
			TestAtlasNormalMode(dst_p / "TestAtlasMixed32", rp / "MixedImages32"); // there is a 8bpp NON-GRAY picture that will be converted to 32 bpp => causing a 32bpp atlas


			chaos::PixelFormatMergeParams no_luminance_merge_params;
			no_luminance_merge_params.accept_luminance = false;
			TestAtlasNormalMode(dst_p / "TestAtlasNoLuminance", rp / "GrayOnlyImages", no_luminance_merge_params);

			TestAtlasFont(dst_p, rp);

			chaos::WinTools::ShowFile(dst_p);
		}

		return 0;
	}
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}