#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{

	GPUQuery::GPUQuery(Window * in_window, GLenum in_target)
	{
		CreateResource(in_window, in_target);	
	}

	GPUQuery::GPUQuery(Window* in_window, GLuint in_id, bool in_ownership)
	{
		SetResource(in_window, in_id, in_ownership);
	}

	GPUQuery::~GPUQuery()
	{
		Release();
	}

	bool GPUQuery::IsValid() const 
	{ 
		return (query_id != 0 && window != nullptr && window->GetGLFWHandler() == context);
	}

	bool GPUQuery::CreateResource(Window* in_window, GLenum in_target)
	{
		assert(
			(in_target == GL_SAMPLES_PASSED) ||
			(in_target == GL_ANY_SAMPLES_PASSED) ||
			(in_target == GL_ANY_SAMPLES_PASSED_CONSERVATIVE) ||
			(in_target == GL_PRIMITIVES_GENERATED) ||
			(in_target == GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN) ||
			(in_target == GL_TIME_ELAPSED) ||
			(in_target == GL_TIMESTAMP));

		// release previous resource
		Release();
		// create new resource
		if (in_window != nullptr && in_window->GetGLFWHandler() != nullptr)
		{
			return WindowApplication::WithGLContext<bool>(in_window->GetGLFWHandler(), [this, in_window, in_target]()
			{
				glCreateQueries(in_target, 1, &query_id);
				if (query_id == 0)
					return false;
				// initialize internals
				window = in_window;
				context = in_window->GetGLFWHandler();
				ownership = true;
				query_target = in_target;
				return true;
			});
		}
		return false;
	}
	
	bool GPUQuery::SetResource(Window* in_window, GLuint in_id, bool in_ownership)
	{
		// early exit
		if (query_id == in_id && window != nullptr && context != nullptr && window == in_window && context == in_window->GetGLFWHandler())
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
				// bad incomming resource
				if (!glIsQuery(in_id))
					return false;
				// get the resource type
				glGetQueryObjectuiv(in_id, GL_QUERY_TARGET, &query_target);
				// initialize internals
				query_id = in_id;
				window = in_window;
				context = in_window->GetGLFWHandler();
				ownership = in_ownership;
				return true;
			});
		}
		return true;
	}

	void GPUQuery::Release()
	{
		assert(!query_started);
		assert(!conditional_rendering_started);  // do not release resource, if something has been started (and would never been stopped)  

		if (window != nullptr && context != nullptr && window->GetGLFWHandler() == context) // ensure window has not be destroyed/recreated
		{
			if (query_id != 0 && ownership)
			{
				WindowApplication::WithGLContext<void>(context, [this]()
				{
					glDeleteQueries(1, &query_id);
				});
			}
		}
		// reset the members
		query_id = 0;
		window = nullptr;
		context = nullptr;
		ownership = false;
		query_started = false;
		query_ended = false;
		conditional_rendering_started = false;
	}

	bool GPUQuery::BeginQuery()
	{
		if (query_id == 0)
			return false;
		if (query_started) // do not start query ...
			return false;
		if (conditional_rendering_started) // ... if something is already started
			return false;

		query_started = true;
		query_ended = false;
		glBeginQuery(query_target, query_id);
		return true;
	}

	bool GPUQuery::EndQuery()
	{
		if (query_id == 0)
			return false;
		if (!query_started) // query must have been started before
			return false;
		assert(!conditional_rendering_started); // logical error

		query_started = false;
		query_ended = true;
		glEndQuery(query_target);
		return true;
	}

	bool GPUQuery::IsResultAvailable()
	{
		if (query_id == 0)
			return false;
		if (!query_ended) // never been started/stopped => nothing to get
			return false;

		GLint result = 0;
		glGetQueryObjectiv(query_id, GL_QUERY_RESULT_AVAILABLE, &result);
		return (result > 0);
	}

	GLint GPUQuery::GetResult(bool wait)
	{
		if (query_id == 0)
			return 0;
		if (!query_ended) // never been started/stopped => nothing to get
			return 0;

		GLint result = 0;
		glGetQueryObjectiv(query_id, wait? GL_QUERY_RESULT : GL_QUERY_RESULT_NO_WAIT, &result);
		return result;
	}

	GLint64 GPUQuery::GetResult64(bool wait)
	{
		if (query_id == 0)
			return 0;
		if (!query_ended) // never been started/stopped => nothing to get
			return 0;

		GLint64 result = 0;
		glGetQueryObjecti64v(query_id, wait ? GL_QUERY_RESULT : GL_QUERY_RESULT_NO_WAIT, &result);
		return result;
	}

	bool GPUQuery::BeginConditionalRendering(bool query_wait, bool inverted)
	{
		if (query_id == 0)
			return false;
		if (conditional_rendering_started) // some conditional rendering in progress => ignore
			return false;
		if (query_started) // some query record in progress => ignore
			return false;
		if (!query_ended) // query must at least has been started/stopped once
			return false;

		conditional_rendering_started = true;

		GLenum query_mode = GL_NONE;
		if (!inverted)
			query_mode = query_wait ? GL_QUERY_WAIT : GL_QUERY_NO_WAIT;
		else
			query_mode = query_wait ? GL_QUERY_WAIT_INVERTED : GL_QUERY_NO_WAIT_INVERTED;

		glBeginConditionalRender(query_id, query_mode);
		return true;
	}

	bool GPUQuery::EndConditionalRendering()
	{
		if (query_id == 0)
			return false;
		if (!conditional_rendering_started) // condtional rendering must have been started once
			return false;
		assert(!query_started); // logical error

		conditional_rendering_started = false;
		glEndConditionalRender();
		return true;
	}  

}; // namespace chaos
