#include "chaos/Chaos.h"

// ====================================================================================
// Take 6 images and convert them into a single image. Then save into Temp. Show the file
// ====================================================================================

void TestConvertToSingle(boost::filesystem::path const & dst_p, boost::filesystem::path const & p, chaos::CubeMapSingleImageLayoutType layout_type)
{
	boost::filesystem::path left_image   = p / "negx.jpg";
	boost::filesystem::path front_image  = p / "posz.jpg";
	boost::filesystem::path right_image  = p / "posx.jpg";
	boost::filesystem::path back_image   = p / "negz.jpg";
	boost::filesystem::path top_image    = p / "posy.jpg";
	boost::filesystem::path bottom_image = p / "negy.jpg";

	chaos::CubeMapImages multiple_sky_box = chaos::CubeMapTools::LoadMultipleCubeMap(left_image, right_image, top_image, bottom_image, front_image, back_image);
	if (multiple_sky_box.IsMultipleImage())
	{
		static glm::vec4 back_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		chaos::CubeMapImages single_sky_box = multiple_sky_box.ToSingleImage(layout_type, back_color, chaos::PixelFormatMergeParams());
		if (single_sky_box.IsSingleImage())
		{
			FIBITMAP * image = single_sky_box.GetImage(chaos::CubeMapImageType::ImageSingle);
			if (image != nullptr)
			{
				boost::filesystem::path dst;
				if (layout_type == chaos::CubeMapSingleImageLayoutType::Horizontal)
					dst_p / "ConvertToSingle_Horiz.png";
				else if (layout_type == chaos::CubeMapSingleImageLayoutType::Vertical)
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
	chaos::CubeMapImages single_sky_box = chaos::CubeMapTools::LoadSingleCubeMap(p);
	if (single_sky_box.IsSingleImage())
	{
		chaos::CubeMapImages multiple_sky_box = single_sky_box.ToMultipleImages();
		if (multiple_sky_box.IsMultipleImage())
		{
			char const * left_image   = "negx.png";
			char const * front_image  = "posz.png";
			char const * right_image  = "posx.png";
			char const * back_image   = "negz.png";
			char const * top_image    = "posy.png";
			char const * bottom_image = "negy.png";

			for (int i = (int)chaos::CubeMapImageType::ImageLeft ; i <= (int)chaos::CubeMapImageType::ImageBack ; ++i)
			{
				chaos::CubeMapImageType Type = chaos::CubeMapImageType(i);

				FIBITMAP * image = multiple_sky_box.GetImage(Type);
				if (image != nullptr)
				{
					char const * suffix = nullptr;
					if (Type == chaos::CubeMapImageType::ImageLeft)
						suffix = left_image;
					else if (Type == chaos::CubeMapImageType::ImageRight)
						suffix = right_image;
					else if (Type == chaos::CubeMapImageType::ImageTop)
						suffix = top_image;
					else if (Type == chaos::CubeMapImageType::ImageBottom)
						suffix = bottom_image;
					else if (Type == chaos::CubeMapImageType::ImageFront)
						suffix = front_image;
					else if (Type == chaos::CubeMapImageType::ImageBack)
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
	chaos::CubeMapImages single_image = chaos::CubeMapTools::LoadSingleCubeMap(p);
	if (single_image.IsSingleImage())
	{
		bool horizontal = single_image.IsSingleImageHorizontal();

		chaos::CubeMapImages multiple_image = single_image.ToMultipleImages();
		if (multiple_image.IsMultipleImage())
		{
			chaos::CubeMapSingleImageLayoutType  other_layout_type = (horizontal)?
				chaos::CubeMapSingleImageLayoutType::Vertical:
				chaos::CubeMapSingleImageLayoutType::Horizontal;

			chaos::CubeMapImages single_image_back = multiple_image.ToSingleImage(other_layout_type, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), chaos::PixelFormatMergeParams());
			if (single_image_back.IsSingleImage())
			{
				FIBITMAP * image = single_image_back.GetImage(chaos::CubeMapImageType::ImageSingle);
				if (image != nullptr)
				{
					boost::filesystem::path dst = dst_p / dst_filename;
					FreeImage_Save(FIF_PNG ,image, dst.string().c_str(), 0);
				}
			}
		}
	}
}

class MyApplication : public chaos::Application
{
protected:

	virtual int Main() override
	{
		boost::filesystem::path const & rp = GetResourcesPath();

		boost::filesystem::path dst_p;
		if (chaos::FileTools::CreateTemporaryDirectory("TestCubeMap", dst_p))
		{
			TestConvertToSingle(dst_p, rp / "Maskonaive", chaos::CubeMapSingleImageLayoutType::Horizontal);
			TestConvertToSingle(dst_p, rp / "Maskonaive", chaos::CubeMapSingleImageLayoutType::Vertical);

			TestConvertToMultiple(dst_p, rp / "violentdays_large.jpg", "MultipleConversionViolent");
			TestConvertToMultiple(dst_p, rp / "originalcubecross.png", "MultipleConversionOriginalCube");

			TestDoubleConversion(dst_p, rp / "violentdays_large.jpg", "DoubleConversionViolent.png");
			TestDoubleConversion(dst_p, rp / "originalcubecross.png", "DoubleConversionOriginalCube.png");

			chaos::WinTools::ShowFile(dst_p);
		}

		return 0;
	}
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}