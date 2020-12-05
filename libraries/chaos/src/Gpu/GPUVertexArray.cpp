#include <chaos/Chaos.h>


namespace chaos
{

	GPUVertexArray::GPUVertexArray(Window* in_window)
	{
		CreateResource(in_window);
	}

	GPUVertexArray::GPUVertexArray(Window* in_window, GLuint in_id, bool in_ownership)
	{
		SetResource(in_window, in_id, in_ownership);
	}

	GPUVertexArray::~GPUVertexArray()
	{
		Release();
	}

	bool GPUVertexArray::IsValid() const 
	{ 
		return (vertex_array_id != 0);
	}

	void GPUVertexArray::Release()
	{
		if (window != nullptr && context != nullptr && window->GetGLFWHandler() == context) // ensure window has not be destroyed/recreated
		{
			if (vertex_array_id == 0 && ownership)
			{
				WindowApplication::WithGLContext<void>(context, [this]()
				{
					glDeleteVertexArrays(1, &vertex_array_id);
				});
			}
		}
		// reset the members
		vertex_array_id = 0;
		window = nullptr;
		context = nullptr;
		ownership = false;
	}

	bool GPUVertexArray::CreateResource(Window * in_window)
	{
		// release previous resource
		Release();
		// create new resource
		if (in_window != nullptr && in_window->GetGLFWHandler() != nullptr)
		{
			return WindowApplication::WithGLContext<bool>(in_window->GetGLFWHandler(), [this, in_window]()
			{
				glCreateVertexArrays(1, &vertex_array_id);
				if (vertex_array_id == 0)
					return false;
				window = in_window;
				context = in_window->GetGLFWHandler();
				ownership = true;
				return true;
			});
		}
		return false;
	}

	bool GPUVertexArray::SetResource(Window * in_window, GLuint in_id, bool in_ownership)
	{
		// early exit
		if (vertex_array_id == in_id && window != nullptr && context != nullptr && window == in_window && context == in_window->GetGLFWHandler())
		{
			ownership = in_ownership;
			return true;
		}
		// release previous resource
		Release();
		// reference new resource (if exisiting)
		if (in_id != 0 && in_window != nullptr && in_window->GetGLFWHandler() != nullptr)
		{
			// bad incomming resource
			return WindowApplication::WithGLContext<bool>(in_window->GetGLFWHandler(), [this, in_window, in_id, in_ownership]()
			{
				if (!glIsVertexArray(in_id)) // check for validity ... with the given context !
					return false;
				// initialize internals
				vertex_array_id = in_id;
				window = in_window;
				context = in_window->GetGLFWHandler();
				ownership = in_ownership;
				return true;
			});
		}
		return true;
	}	

}; // namespace chaos
