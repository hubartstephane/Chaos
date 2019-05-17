#include <chaos/GPUFence.h>

namespace chaos
{

	GPUFence::GPUFence(GLsync in_fence) :
		fence(in_fence)
	{
		if (fence == nullptr)
			fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0); // a fence object is pushed inside the OpenGL command queue
	}

	GPUFence::~GPUFence()
	{
		DoRelease();
	}

	bool GPUFence::WaitForCompletion(float timeout)
	{
		GLuint64   timeout64 = (GLuint64)(timeout * 10.0e9);
		GLbitfield flags     = 0; 

		GLenum result = glClientWaitSync(fence, flags, timeout64);
		if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED)
			return true;
		return false; // GL_TIMEOUT_EXPIRED or GL_WAIT_FAILED
	}


	bool GPUFence::DoRelease()
	{
		if (fence == nullptr)
			return false;
		glDeleteSync(fence);
		fence = nullptr;
		return true;	
	}

}; // namespace chaos
