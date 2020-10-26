#include <chaos/Chaos.h>

namespace chaos
{

	GPUQuery::GPUQuery(GLenum in_target)
	{
		CreateResource(in_target);	
	}

	GPUQuery::GPUQuery(GLuint in_id, bool in_ownership)
	{
		SetResource(in_id, in_ownership);
	}

	GPUQuery::~GPUQuery()
	{
		DoRelease();
	}

	bool GPUQuery::IsValid() const 
	{ 
		return (query_id != 0);
	}

	
	bool GPUQuery::CreateResource(GLenum in_target)
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
		glCreateQueries(in_target, 1, &query_id);
		if (query_id == 0)
			return false;
		// initialize internals
		query_target = in_target;
		ownership = true;		
		return false;
	}

	
	bool GPUQuery::SetResource(GLuint in_id, bool in_ownership)
	{
		// early exit
		if (query_id == in_id)
		{
			ownership = in_ownership;
			return true;
		}

		// release previous resource
		Release();

		// reference new resource (if exisiting)
		if (in_id != 0)
		{
			// bad incomming resource
			if (!glIsQuery(in_id)) 
				return false;
			// get the resource type
			glGetQueryObjectuiv(in_id, GL_QUERY_TARGET, &query_target);
			// initialize internals
			query_id = in_id;
			ownership = in_ownership;
		}
		return true;
	}

	bool GPUQuery::DoRelease()
	{
		assert(!query_started);
		assert(!conditional_rendering_started);  // do not release resource, if something has been started (and would never been stopped)  

		if (query_id == 0)
			return false;
		if (ownership)
			glDeleteQueries(1, &query_id);
		query_id = 0;
		query_started = false;
		query_ended = false;
		conditional_rendering_started = false;
		return true;
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
