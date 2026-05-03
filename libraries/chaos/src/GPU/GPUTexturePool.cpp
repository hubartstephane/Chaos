#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	GPUTexturePool::GPUTexturePool(GPUDevice * in_gpu_device):
		GPUDeviceResourceInterface(in_gpu_device)
	{
	}

	GPUTexturePool::~GPUTexturePool()
	{
		assert(texture_info_groups.size() == 0); // ensure pool has properly been cleared before
	}

	bool GPUTexturePool::DoTick(float delta_time)
	{
		return true;
	}

	GPUTexture * GPUTexturePool::CreateTexture(TextureDescription const& in_texture_description)
	{
		if (GPUTexture * result = DoFindCachedTexture(in_texture_description))
			return result;
		return DoCreateTexture(in_texture_description);
	}

	GPUTexture * GPUTexturePool::DoFindCachedTexture(TextureDescription const& in_texture_description)
	{
		for (auto entry_it = texture_info_groups.begin() ; entry_it != texture_info_groups.end() ; ++entry_it)
		{
			GPUTexturePoolTextureInfoGroup & texture_info_group = *entry_it;

			for (auto texture_info_it = texture_info_group.textures_info.begin() ; texture_info_it != texture_info_group.textures_info.end() ; ++texture_info_it)
			{
				GPUTexturePoolTextureInfo & texture_info = *texture_info_it;

				if (DoMatchRequestedTextureInfo(texture_info, in_texture_description))
				{
					if (GPUTexture * result = new GPUTexture(GetGPUDevice(), texture_info.texture_id, texture_info.texture_description))
					{
						texture_info_group.textures_info.erase(texture_info_it);
						if (texture_info_group.textures_info.size() == 0)
							texture_info_groups.erase(entry_it);
						return result;
					}
				}
			}
		}
		return nullptr;
	}

	bool GPUTexturePool::DoMatchRequestedTextureInfo(GPUTexturePoolTextureInfo & in_texture_info, TextureDescription const& in_texture_description) const
	{
		if (in_texture_info.texture_description == in_texture_description)
			return true;
		return false;
	}

	GPUTexture * GPUTexturePool::DoCreateTexture(TextureDescription const& in_texture_description)
	{
		// check for coherency
		if (!in_texture_description.IsValid())
			return nullptr;

		// tweak for single channel formats
		GLPixelFormat gl_formats = GLTextureTools::GetGLPixelFormat(in_texture_description.pixel_format);
		if (!gl_formats.IsValid())
			return nullptr;

		// create the opengl texture
		GLuint texture_id = 0;
		glCreateTextures((GLenum)in_texture_description.type, 1, &texture_id);
		if (texture_id == 0)
			return nullptr;

		// tweak for single channel formats
		if (gl_formats.internal_format == GL_R8 || gl_formats.internal_format == GL_R32F)
		{
			glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTextureParameteri(texture_id, GL_TEXTURE_SWIZZLE_A, GL_ONE);
		}

		// compute mipmap count
		int mipmap_count = 1;
		if (in_texture_description.use_mipmaps)
		{
			switch (in_texture_description.type)
			{
			case TextureType::Texture1D:
			case TextureType::Texture1DArray:
				mipmap_count = GLTextureTools::GetMipmapLevelCount(in_texture_description.width);
				break;
			case TextureType::Texture2D:
			case TextureType::Texture2DArray:
			case TextureType::TextureCubeMap:
			case TextureType::TextureCubeMapArray:
				mipmap_count = GLTextureTools::GetMipmapLevelCount(in_texture_description.width, in_texture_description.height);
				break;
			case TextureType::Texture3D:
				mipmap_count = GLTextureTools::GetMipmapLevelCount(in_texture_description.width, in_texture_description.height, in_texture_description.depth);
				break;
			default:
				assert(0);
				break;
			}
		}

		// reserve storage
		switch (in_texture_description.type)
		{
		case TextureType::Texture1D:
			glTextureStorage1D(texture_id, mipmap_count, gl_formats.internal_format, in_texture_description.width);
			break;
		case TextureType::Texture1DArray:
			glTextureStorage2D(texture_id, mipmap_count, gl_formats.internal_format, in_texture_description.width, in_texture_description.depth);
			break;
		case TextureType::Texture2D:
		case TextureType::TextureCubeMap:
			glTextureStorage2D(texture_id, mipmap_count, gl_formats.internal_format, in_texture_description.width, in_texture_description.height);
			break;
		case TextureType::Texture2DArray:
		case TextureType::Texture3D:
		case TextureType::TextureCubeMapArray:
			glTextureStorage3D(texture_id, mipmap_count, gl_formats.internal_format, in_texture_description.width, in_texture_description.height, in_texture_description.depth);
			break;
		default:
			assert(0);
			break;
		}

		return new GPUTexture(GetGPUDevice(), texture_id, in_texture_description);
	}

	GPUTexturePoolTextureInfoGroup * GPUTexturePool::FindOrCreateTextureInfoGroup(GPUFence * in_fence)
	{
		// search if there is an entry matching wanted fence
		for (GPUTexturePoolTextureInfoGroup & texture_info_group : texture_info_groups)
			if (texture_info_group.fence == in_fence)
				return &texture_info_group;
		// create a new entry for this fence
		GPUTexturePoolTextureInfoGroup texture_info_group;
		texture_info_group.fence = in_fence;
		texture_info_groups.push_back(texture_info_group);
		return &texture_info_groups[texture_info_groups.size() - 1];
	}

	void GPUTexturePool::OnTextureUnused(GPUTexture * in_texture)
	{
		if (WantToReuseTexture(in_texture))
		{
			ReleaseTexture(in_texture->texture_id);
		}
		else if (GPUTexturePoolTextureInfoGroup * texture_info_group = FindOrCreateTextureInfoGroup(nullptr))
		{
			GPUTexturePoolTextureInfo texture_info;
			texture_info.texture_id          = in_texture->texture_id;
			texture_info.texture_description = in_texture->GetTextureDescription();
			texture_info_group->textures_info.push_back(texture_info);

			in_texture->texture_id = 0; // capture the underlying resource
		}
	}

	bool GPUTexturePool::WantToReuseTexture(GPUTexture * in_texture) const
	{
		return true;
	}

	void GPUTexturePool::ClearPool()
	{
		for (GPUTexturePoolTextureInfoGroup & texture_info_group : texture_info_groups)
			for (GPUTexturePoolTextureInfo & texture_info : texture_info_group.textures_info)
				ReleaseTexture(texture_info.texture_id);
		texture_info_groups.clear();
	}

	void GPUTexturePool::ReleaseTexture(GLuint & in_texture_id) const
	{
		glDeleteTextures(1, &in_texture_id);
		OnTextureDestroyed(in_texture_id);
		in_texture_id = 0;
	}

	void GPUTexturePool::OnTextureDestroyed(GLuint in_texture_id) const
	{
		gpu_device->OnTextureDestroyed(in_texture_id);
	}

}; // namespace chaos

