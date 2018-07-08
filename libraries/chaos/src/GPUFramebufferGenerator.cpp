#include <chaos/GPUFramebufferGenerator.h>
#include <chaos/GLTools.h>
#include <chaos/GLTextureTools.h>
#include <chaos/GPURenderbufferLoader.h>

namespace chaos
{
	GPUFramebuffer * GPUFramebufferGenerator::GenerateFramebuffer(glm::ivec2 const & wanted_size)
	{
		GPUFramebuffer * result = nullptr;

		// compute final size and test whether it is valid
		glm::ivec2 null_size(0, 0);

		glm::ivec2 final_size;
		if (size == null_size) // content does not define any size .. use parameter
		{
			if (wanted_size == null_size) // cannot define any resolution
				return nullptr;
			final_size = wanted_size;
		}
		else
		{
			if (wanted_size != null_size && wanted_size != size) // the parameter does not match the size
				return nullptr;
			final_size = size;
		}
		// create the framebuffer
		GLuint framebuffer = 0;
		glGenFramebuffers(1, &framebuffer);

		if (framebuffer != 0)
		{
			result = new GPUFramebuffer(framebuffer);
			if (!InitializeFramebuffer(result, final_size))
			{
				delete(result);
				result = nullptr;
			}
		}
		// clear the generator
		Clear();

		return result;
	}

	bool GPUFramebufferGenerator::InitializeFramebuffer(GPUFramebuffer * framebuffer, glm::ivec2 const & final_size)
	{
		GPURenderbufferLoader loader;

		GLuint framebuffer_id = framebuffer->GetResourceID();

		for (GPUFramebufferGeneratorAttachmentInfo & info : attachment_info)
		{
			// dynamically generated renderbuffer
			if (info.texture == nullptr && info.renderbuffer == nullptr)
				info.renderbuffer = loader.GenRenderbufferObject(info.pixel_format, final_size);

			// texture provided
			if (info.texture != nullptr)
			{
				glNamedFramebufferTexture(framebuffer_id, info.attachment_point, info.texture->GetResourceID(), info.texture_mipmap);
			}
			// renderbuffer provided
			else if (info.renderbuffer != nullptr)
			{
				glNamedFramebufferRenderbuffer(framebuffer_id, info.attachment_point, GL_RENDERBUFFER, info.renderbuffer->GetResourceID());
			}
			// case of error
			else
			{
				continue;
			}

			// add information into the framebuffer
			framebuffer->attachment_info.push_back(std::move(info)); // automatic conversion
		}
		return true;
	}

	void GPUFramebufferGenerator::Clear()
	{
		attachment_info.clear();
		size = glm::ivec2(0, 0);
	}

	bool GPUFramebufferGenerator::IsSurfaceSizeCompatible(GPUSurface * surface, int mipmap) const
	{
		assert(surface != nullptr);
		assert(mipmap >= 0);
		return IsSurfaceSizeCompatible(surface->GetSurfaceDescription().GetSize(mipmap));
	}

	bool GPUFramebufferGenerator::IsSurfaceSizeCompatible(glm::ivec2 const & in_size) const
	{
		// size is not known yet
		if (size == glm::ivec2(0, 0))
			return true;
		if (in_size == glm::ivec2(0, 0))
			return true;
		// test whether the size are compatible
		return (size == in_size);
	}

	bool GPUFramebufferGenerator::IsColorAttachmentValid(int color_index) const
	{
		assert(color_index >= 0);
		if (max_color_attachment < 0)
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachment);
		assert(max_color_attachment >= 0);
		return (color_index < max_color_attachment);
	}

	bool GPUFramebufferGenerator::IsColorAttachmentValid(int color_index, char const * name) const
	{
		if (!IsColorAttachmentValid(color_index))
			return false;
		if (IsColorAttachmentInUse(color_index))
			return false;
		if (IsAttachmentNameInUse(name))
			return false;
		return true;
	}

	void GPUFramebufferGenerator::CompleteAndInsertAttachment(GPUFramebufferGeneratorAttachmentInfo & info, char const * name)
	{
		// complete the name if required
		if (name != nullptr)
			info.name = name;
		// compute size if required
		if (size == glm::ivec2(0, 0)) 
		{
			glm::ivec2 new_size = glm::ivec2(0, 0);
			if (info.texture != nullptr)
				new_size = info.texture->GetSurfaceDescription().GetSize(info.texture_mipmap);
			else if (info.renderbuffer != nullptr)
				new_size = info.renderbuffer->GetSurfaceDescription().GetSize(0);
			else
				new_size = info.size;

			if (new_size != glm::ivec2(0, 0))
				size = new_size;
		}
		// insert the attachment
		attachment_info.push_back(std::move(info));
	}

	bool GPUFramebufferGenerator::AddDepthStencilAttachment(GPUTexture * texture, int mipmap, char const * name)
	{
		assert(texture != nullptr);
		assert(mipmap >= 0);

		if (!IsSurfaceSizeCompatible(texture, mipmap))
			return false;
		if (IsAttachmentNameInUse(name))
			return false;
		if (IsSurfaceInUse(texture))
			return false;
		if (HasDepthStencilAttachment())
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_DEPTH_STENCIL_ATTACHMENT;
		info.texture = texture;
		info.texture_mipmap = mipmap;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddDepthStencilAttachment(GPURenderbuffer * renderbuffer, char const * name)
	{
		assert(renderbuffer != nullptr);

		if (!IsSurfaceSizeCompatible(renderbuffer))
			return false;
		if (IsAttachmentNameInUse(name))
			return false;
		if (IsSurfaceInUse(renderbuffer))
			return false;
		if (HasDepthStencilAttachment())
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_DEPTH_STENCIL_ATTACHMENT;
		info.renderbuffer = renderbuffer;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddDepthStencilAttachment(glm::ivec2 const & in_size, char const * name)
	{
		if (!IsSurfaceSizeCompatible(in_size))
			return false;
		if (IsAttachmentNameInUse(name))
			return false;
		if (HasDepthStencilAttachment())
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_DEPTH_STENCIL_ATTACHMENT;
		info.size = in_size;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddColorAttachment(int color_index, GPUTexture * texture, int mipmap, char const * name)
	{
		assert(texture != nullptr);
		assert(mipmap >= 0);

		if (!IsSurfaceSizeCompatible(texture, mipmap))
			return false;
		if (!IsColorAttachmentValid(color_index, name))
			return false;
		if (IsSurfaceInUse(texture))
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_COLOR_ATTACHMENT0 + color_index;
		info.texture = texture;
		info.texture_mipmap = mipmap;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddColorAttachment(int color_index, GPURenderbuffer * renderbuffer, char const * name)
	{
		assert(renderbuffer != nullptr);

		if (!IsSurfaceSizeCompatible(renderbuffer))
			return false;
		if (!IsColorAttachmentValid(color_index, name))
			return false;
		if (IsSurfaceInUse(renderbuffer))
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_COLOR_ATTACHMENT0 + color_index;
		info.renderbuffer = renderbuffer;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddColorAttachment(int color_index, PixelFormat const & pixel_format, glm::ivec2 const & in_size, char const * name)
	{
		if (!IsSurfaceSizeCompatible(in_size))
			return false;
		if (!IsColorAttachmentValid(color_index, name))
			return false;
		if (!pixel_format.IsValid())
			return false;
		if (!pixel_format.IsColorPixel())
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_COLOR_ATTACHMENT0 + color_index;
		info.pixel_format = pixel_format;
		info.size = in_size;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	glm::ivec2 GPUFramebufferGenerator::GetSize() const
	{
		return size;
	}

}; // namespace chaos
