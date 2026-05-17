#include "chaos/Chaos.h"

class ImageGenerationParams
{
public:

	bool IsValid() const
	{
		return
			IsCountValid() &&
			IsMinSizeXValid() &&
			IsMinSizeYValid() &&
			IsMaxSizeXValid() &&
			IsMaxSizeYValid();
	}

	bool IsCountValid() const
	{
		return (count > 0 && count < 500);
	}

	bool IsMinSizeXValid() const
	{
		return IsSizeValid(min_size.x) && (min_size.x < max_size.x);
	}

	bool IsMinSizeYValid() const
	{
		return IsSizeValid(min_size.y) && (min_size.y < max_size.y);
	}

	bool IsMaxSizeXValid() const
	{
		return IsSizeValid(max_size.x) && (min_size.x < max_size.x);
	}

	bool IsMaxSizeYValid() const
	{
		return IsSizeValid(max_size.y) && (min_size.y < max_size.y);
	}

	bool IsSizeValid(int size) const
	{
		return (size > 0) && (size < 1024);
	}

public:

	int count = 15;
	glm::ivec2 min_size = { 1, 1 };
	glm::ivec2 max_size = { 256, 256 };
};

// ---------------------------------------------------------------------------------

class AtlasBuilderApplication : public chaos::WindowApplication
{
	using chaos::WindowApplication::WindowApplication;

	CHAOS_DECLARE_OBJECT_CLASS(AtlasBuilderApplication, chaos::WindowApplication);

public:

	bool SaveImage(FIBITMAP* image, FREE_IMAGE_FORMAT target_format, boost::filesystem::path const path)
	{
		if (image == nullptr)
			return false;

		chaos::ImageDescription desc = chaos::ImageTools::GetImageDescription(image);
		if (!desc.IsValid(false))
			return false;


		FREE_IMAGE_TYPE pixel_type = chaos::ImageTools::GetFreeImageType(desc.pixel_format);





		FIBITMAP* converted_image = nullptr;
		if (!FreeImage_FIFSupportsExportType(target_format, pixel_type))
		{
			switch(target_format)
			{
			case FIF_BMP:
				converted_image = FreeImage_ConvertToType(image, FIT_BITMAP);
				break;
			case FIF_JPEG:
				converted_image = FreeImage_ConvertToType(image, FIT_BITMAP);
				break;
			case FIF_PNG:
				converted_image = FreeImage_ConvertToType(image, FIT_BITMAP);
				break;
			case FIF_DDS:
				converted_image = FreeImage_ConvertToType(image, FIT_BITMAP);
				break;
			case FIF_GIF:
				converted_image = FreeImage_ConvertToType(image, FIT_BITMAP);
				break;
			case FIF_HDR:
				converted_image = FreeImage_ConvertToType(image, FIT_BITMAP);
				break;
			case FIF_EXR:	
				converted_image = FreeImage_ConvertToType(image, FIT_BITMAP);
				break;
			default:
				break;
			}
			if (converted_image == nullptr)
				return false;
		}

		bool result = false;
		if (converted_image == nullptr)
		{
			result = FreeImage_Save(target_format, image, path.string().c_str());
		}
		else
		{
			result = FreeImage_Save(target_format, converted_image, path.string().c_str());
			FreeImage_Unload(converted_image);
		}
		return result;
	}

	bool GenerateImageDirectory()
	{
		if (image_generation_directory.empty())
		{
			boost::filesystem::path dst_path = GetApplicationTemporaryPath() / "GeneratedImages";

			if (!boost::filesystem::is_directory(dst_path))
				if (!boost::filesystem::create_directories(dst_path))
					return false;
			image_generation_directory = std::move(dst_path);
		}
		return true;
	}

	bool ClearImages()
	{
		if (!GenerateImageDirectory())
			return false;
		ClearDirectory(image_generation_directory);
		chaos::WinTools::ShowFile(image_generation_directory);
		return true;
	}

	bool GenerateImages(ImageGenerationParams const& image_generation_params)
	{
		if (!image_generation_params.IsValid())
			return false;

		if (!ClearImages())
			return false;

		std::vector<std::pair<FREE_IMAGE_FORMAT, char const* >> format_maps =
		{
		//	{FIF_BMP, "bmp"},
		//	{FIF_JPEG, "jpg"},
			{FIF_PNG, "png"},
		//	{FIF_DDS, "dds"},
		//	{FIF_GIF, "gif"},
		//	{FIF_HDR, "hdr"},
		//	{FIF_EXR, "exr"}
		};

		std::random_device rd;
		std::mt19937 rng(rd());

		for (int i = 0; i < image_generation_params.count; ++i)
		{
			int width = std::uniform_int_distribution<int>(image_generation_params.min_size.x, image_generation_params.max_size.x)(rng);
			int height = std::uniform_int_distribution<int>(image_generation_params.min_size.y, image_generation_params.max_size.y)(rng);
			int bpp = 4;

			size_t format_index = std::uniform_int_distribution<size_t>(0, format_maps.size() - 1)(rng);

			if (FIBITMAP* image = chaos::ImageTools::GenFreeImage(chaos::PixelFormat::BGRA, width, height))
			{
				chaos::ImageDescription desc = chaos::ImageTools::GetImageDescription(image);

				glm::vec4 color;
				color.r = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
				color.g = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
				color.b = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
				color.a = 1.0f;

				chaos::ImageTools::FillImageBackground(desc, color);

				// save image
				FREE_IMAGE_FORMAT format = format_maps[format_index].first;
				char const * extension = format_maps[format_index].second;

				std::string image_name = chaos::StringTools::Printf("image%d.%s", i, extension);
				boost::filesystem::path image_path = image_generation_directory / image_name;

				SaveImage(image, format, image_path);

				// free image
				FreeImage_Unload(image);
			}
		}

		return true;

	}

protected:

	virtual int MainBody() override
	{
		return chaos::WindowApplication::MainBody();
	}

	void ClearDirectory(boost::filesystem::path const & path) const
	{
		if (!boost::filesystem::is_directory(path))
			return;
		for (const auto& entry : boost::filesystem::directory_iterator(path))
			boost::filesystem::remove_all(entry.path());
	}

protected:

	boost::filesystem::path image_generation_directory;
};

// ---------------------------------------------------------------------------------

class ImageFileInfo
{
public:

	boost::filesystem::path path;
	boost::filesystem::path filename;
	chaos::bitmap_ptr       image;
	glm::ivec2              image_size = {0, 0};
};

class AtlasBuilderWindow : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(AtlasBuilderWindow, chaos::Window);

public:

	virtual void OnDrawImGuiContent() override
	{
		chaos::ImGuiTools::FullViewportWindow("content", 0, [this]()
		{
			ImVec2 avail1 = ImGui::GetContentRegionAvail();

			if (ImGui::BeginChild("ImageGenerationPanel", ImVec2(avail1.x, 300.0f), ImGuiChildFlags_Borders))
			{
				ImGuiDrawImageGenerationPanel();

				ImGui::EndChild();
			}

			float spacing = ImGui::GetStyle().ItemSpacing.x;
			ImVec2 avail2 = ImGui::GetContentRegionAvail();

			ImVec2 panel_size = {(avail2.x - spacing) * 0.5f, avail2.y};

			if (ImGui::BeginChild("AtlasGenerationPanel", panel_size, ImGuiChildFlags_Borders))
			{
				ImGuiDrawAtlasGenerationPanel();

				ImGui::EndChild();
			}

			ImGui::SameLine();

			if (ImGui::BeginChild("AtlasPanel", panel_size, ImGuiChildFlags_Borders))
			{
				ImGuiDrawAtlas();

				ImGui::EndChild();
			}		
		});
	}

	virtual void OnDropFile(int count, char const** paths) override
	{
		for (int i = 0 ; i < count ; ++i)
		{
			boost::filesystem::path path = paths[i];

			auto it = std::ranges::find_if(image_files, [&](ImageFileInfo const & info)
			{
				return info.path == path;	
			});

			if (it != image_files.end())
				continue;

			if (FIBITMAP * image = chaos::ImageTools::LoadImageFromFile(paths[i]))
			{
				chaos::ImageDescription image_description = chaos::ImageTools::GetImageDescription(image);

				ImageFileInfo info;
				info.path = std::move(path);
				info.filename = info.path.filename();
				info.image = chaos::bitmap_ptr(image);
				info.image_size = { image_description.width, image_description.height };
				image_files.push_back(std::move(info));
			}
		}
		GenerateAtlas();
	}

protected:

	void Title(char const * title)
	{
		ImGui::SetWindowFontScale(1.5f);
		ImGui::Text("%s", title);
		ImGui::SetWindowFontScale(1.0f);

		ImGui::Dummy({1.0f, 20.0f});
	}

	void GenerateAtlas()
	{
		if (!IsAtlasGenerationParamsValid(atlas_generation_params))
			return;

		chaos::AtlasInput input;
		for (ImageFileInfo const & info : image_files)
		{
			input.AddBitmap(info.image.get(), false, info.filename.string().c_str(), 0);
		}

		chaos::AtlasGenerator generator;
		generator.ComputeResult(input, atlas, atlas_generation_params);

		atlas_textures.clear();

		for (chaos::bitmap_ptr const & bitmap : atlas.GetBitmaps())
		{
			if (chaos::GPUTexture * texture = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(bitmap.get()))
				atlas_textures.push_back(texture);
		}	
	}

	bool IsAtlasGenerationParamsValid(chaos::AtlasGeneratorParams const & params) const
	{





		return true;
	}

	void ImGuiDrawAtlasGenerationPanel()
	{
		Title("Generate atlas");

		if (ImGui::BeginTable("image_generation_params", 2, 0))
		{
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

			// Atlas width
			ImGui::TableNextColumn();
			ImGui::Text("Atlas width");

			ImGui::TableNextColumn();
			ImGui::InputInt("##atlas_width", &atlas_generation_params.atlas_width);

			// Atlas height
			ImGui::TableNextColumn();
			ImGui::Text("Atlas height");

			ImGui::TableNextColumn();
			ImGui::InputInt("##atlas_height", &atlas_generation_params.atlas_height);

			// Atlas max width
			ImGui::TableNextColumn();
			ImGui::Text("Atlas max width");

			ImGui::TableNextColumn();
			ImGui::InputInt("##atlas_max_width", &atlas_generation_params.atlas_max_width);

			// Atlas max height
			ImGui::TableNextColumn();
			ImGui::Text("Atlas max height");

			ImGui::TableNextColumn();
			ImGui::InputInt("##atlas_max_height", &atlas_generation_params.atlas_max_height);

			// Atlas padding
			ImGui::TableNextColumn();
			ImGui::Text("Atlas padding");

			ImGui::TableNextColumn();
			ImGui::InputInt("##atlas_padding", &atlas_generation_params.atlas_padding);

			// Power of 2 
			ImGui::TableNextColumn();
			ImGui::Text("Force power of 2");

			ImGui::TableNextColumn();
			ImGui::Checkbox("##force_power_of_2", &atlas_generation_params.force_power_of_2);

			// Force square 
			ImGui::TableNextColumn();
			ImGui::Text("Force square");

			ImGui::TableNextColumn();
			ImGui::Checkbox("##force_square", &atlas_generation_params.force_square);

			// Duplicate image border
			ImGui::TableNextColumn();
			ImGui::Text("Image border duplication");

			ImGui::TableNextColumn();
			ImGui::Checkbox("##duplicate_image_border", &atlas_generation_params.duplicate_image_border);

			// background color
			ImGui::TableNextColumn();
			ImGui::Text("Background color");

			ImGui::TableNextColumn();
			ImGui::ColorEdit4("##background_color", &atlas_generation_params.background_color.r);

			ImGui::EndTable();
		}

		bool generation_required = false;

		chaos::ImGuiTools::ConditionalDisable(!IsAtlasGenerationParamsValid(atlas_generation_params), [&]()
		{
			if (ImGui::Button("Generate"))
				generation_required = true;
		});

		ImGui::SameLine();
		if (ImGui::Button("Clear"))
			ClearAtlas();

		ImGui::Dummy({1.0f, 20.0f});

		Title("File list");

		// draw file list
		std::optional<int> image_to_remove;
		if (ImGui::BeginTable("objects", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV))
		{
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);

			for (int i = 0 ; i < image_files.size() ; ++i)
			{
				ImGui::PushID(i);

				ImGui::TableNextColumn();
				ImGui::Text("%s", image_files[i].filename.string().c_str());

				ImGui::TableNextColumn();
				chaos::PixelFormat pixel_format = chaos::ImageTools::GetPixelFormat(image_files[i].image.get());
				ImGui::Text("  %s  ", EnumToString(pixel_format));

				ImGui::TableNextColumn();
				ImGui::Text("  %d x %d  ", image_files[i].image_size.x, image_files[i].image_size.y);
			
				ImGui::TableNextColumn();
				if (ImGui::Button("X"))
					image_to_remove = i;

				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		if (image_to_remove.has_value())
		{
			image_files.erase(image_files.begin() + image_to_remove.value());
			generation_required = true;
		}
	
		if (generation_required)
			GenerateAtlas();

		ImGui::Dummy({ 1.0f, 50.0f });

		char const * text = "Drag & Drop image files here";

		float col_width = ImGui::GetColumnWidth();
		float text_width = ImGui::CalcTextSize(text).x;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (col_width - text_width) * 0.5f);
		ImGui::Text("%s", text);
	}

	void ImGuiDrawAtlas()
	{
		Title("Atlas");

		if (atlas.GetBitmapCount() == 0)
			return;

		glm::ivec2 dimension = atlas.GetAtlasDimension();
		size_t bitmap_count = atlas.GetBitmapCount();

		ImGui::Text("width  = %d", dimension.x);
		ImGui::Text("height = %d", dimension.y);
		ImGui::Text("Layer count = %d", (int)bitmap_count);

		ImGui::Dummy({ 1.0f, 20.0f });

		ImVec2 avail = ImGui::GetContentRegionAvail();

		ImVec2 image_dimension = { 0.0f, 0.0f };
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		image_dimension.x = (avail.x - 2.0f * spacing) / 3.0f;
		image_dimension.y = image_dimension.x * (float(dimension.y) / float(dimension.x));

		int index = 0;
		for (chaos::shared_ptr<chaos::GPUTexture> const & texture : atlas_textures)
		{
			ImGui::Image(
				(ImTextureID)(intptr_t)texture->GetResourceID(),
				image_dimension,
				ImVec2(0, 1),		// axis Y for texture may be inverted
				ImVec2(1, 0),
				ImVec4(1, 1, 1, 1), // tint
				ImVec4(1, 1, 1, 1)  // border
			);

			if (ImGui::IsItemHovered())
			{
				ImVec2 mouse_pos = ImGui::GetMousePos();

				// get current image size and position
				ImVec2 rect_min = ImGui::GetItemRectMin();
				ImVec2 rect_max = ImGui::GetItemRectMax();
				ImVec2 rect_size = { rect_max.x - rect_min.x, rect_max.y - rect_min.y };

				// get mouse position relative to the image
				ImVec2 relative_pos;
				relative_pos.x = (mouse_pos.x - rect_min.x) / rect_size.x;
				relative_pos.y = (mouse_pos.y - rect_min.y) / rect_size.y;





				
			}


			if ((index % 3) != 2)
				ImGui::SameLine();
			++index;
		}

	}

	void ImGuiDrawImageGenerationPanel()
	{
		Title("Generate images");
		
		bool count_valid      = image_generation_params.IsCountValid();
		bool min_size_x_valid = image_generation_params.IsMinSizeXValid();
		bool min_size_y_valid = image_generation_params.IsMinSizeYValid();
		bool max_size_x_valid = image_generation_params.IsMaxSizeXValid();
		bool max_size_y_valid = image_generation_params.IsMaxSizeYValid();
		bool valid = count_valid && min_size_x_valid && min_size_y_valid && max_size_x_valid && max_size_y_valid;

		if (ImGui::BeginTable("image_generation_params", 2, 0))
		{
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

			// count 
			ImGui::TableNextColumn();
			ImGui::Text("count");

			ImGui::TableNextColumn();
			SelectTextColor(count_valid, [&]()
			{
				ImGui::InputInt("##count", &image_generation_params.count);
			});

			// min size
			ImGui::TableNextColumn();
			ImGui::Text("min size");

			ImGui::TableNextColumn();
			SelectTextColor(min_size_x_valid, [&]()
			{
				ImGui::InputInt("##min_size.x", &image_generation_params.min_size.x);
			});

			ImGui::TableNextColumn(); // empty column for min_size.y

			ImGui::TableNextColumn();
			SelectTextColor(min_size_y_valid, [&]()
			{
				ImGui::InputInt("##min_size.y", &image_generation_params.min_size.y);
			});

			// max size
			ImGui::TableNextColumn();
			ImGui::Text("max size");

			ImGui::TableNextColumn();
			SelectTextColor(max_size_x_valid, [&]()
			{
				ImGui::InputInt("##max_size.x", &image_generation_params.max_size.x);
			});

			ImGui::TableNextColumn(); // empty column for max_size.y

			ImGui::TableNextColumn();
			SelectTextColor(max_size_y_valid, [&]()
			{
				ImGui::InputInt("##max_size.y", &image_generation_params.max_size.y);
			});

			ImGui::EndTable();
		};

		chaos::ImGuiTools::ConditionalDisable(!valid, [&]()
		{
			if (ImGui::Button("Generate"))
				GenerateImages();
		});	

		ImGui::SameLine();
		if (ImGui::Button("Clear"))
			ClearImages();
	}

	bool GenerateImages() const
	{
		if (AtlasBuilderApplication* application = chaos::Application::GetInstance())
		{
			return application->GenerateImages(image_generation_params);
		}
		return false;
	}

	bool ClearImages()
	{
		if (AtlasBuilderApplication* application = chaos::Application::GetInstance())
		{
			return application->ClearImages();
		}
		return false;
	}

	void SelectTextColor(bool normal, chaos::LightweightFunction<void()> func)
	{
		ImVec4 normal_color = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImVec4 error_color  = ImVec4(1.0f, 0, 0, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Text, normal ? normal_color : error_color);
		func();
		ImGui::PopStyleColor(1);
	}

	void ClearAtlas()
	{
		atlas.Clear();
		image_files.clear();
		atlas_textures.clear();
	}

protected:

	ImageGenerationParams image_generation_params;

	chaos::AtlasGeneratorParams atlas_generation_params;

	std::vector<ImageFileInfo> image_files;

	chaos::Atlas atlas;

	std::vector<chaos::shared_ptr<chaos::GPUTexture>> atlas_textures;
};

// ---------------------------------------------------------------------------------




int main(int argc, char ** argv, char ** env)
{
	chaos::WindowApplicationData window_application_data;
	window_application_data.main_window_class = AtlasBuilderWindow::GetStaticClass();
	return chaos::RunApplication<AtlasBuilderApplication>(argc, argv, env, &window_application_data);
}