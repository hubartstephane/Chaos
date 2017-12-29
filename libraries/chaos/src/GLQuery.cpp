#include <chaos/GLQuery.h>
 
namespace chaos
{

  GLQuery::GLQuery(GLuint in_id, GLenum in_target) : 
    query_id(in_id), 
    query_target(in_target)
  {
  }

  GLQuery::GLQuery(GLenum in_target)
  {
    assert(
      (in_target == GL_SAMPLES_PASSED) ||
      (in_target == GL_ANY_SAMPLES_PASSED) ||
      (in_target == GL_ANY_SAMPLES_PASSED_CONSERVATIVE) ||
      (in_target == GL_PRIMITIVES_GENERATED) ||
      (in_target == GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN) ||
      (in_target == GL_TIME_ELAPSED) ||
      (in_target == GL_TIMESTAMP)
    );

    glGenQueries(1, &query_id);
    if (query_id != 0)
      query_target = in_target;
  }

  GLQuery::~GLQuery()
  {
    Release();
  }

  void GLQuery::Release()
  {
    assert(!query_started);
    if (query_id != 0)
    {			
      glDeleteQueries(1, &query_id);
      query_id = 0;
      query_started = false;
      query_ended = false;
    }  
  }

  void GLQuery::BeginQuery()
  {
    if (query_id == 0)
      return;
    assert(!query_started);
    query_started = true;
    query_ended = false;
    glBeginQuery(query_target, query_id);
  }

  void GLQuery::EndQuery()
  {
    if (query_id == 0)
      return;
    assert(query_started);
    query_started = false;
    query_ended = true;
    glEndQuery(query_target);
  }

  bool GLQuery::IsResultAvailable()
  {
    if (query_id == 0)
      return false;
    assert(query_ended);

    GLint result = 0;
    glGetQueryObjectiv(query_id, GL_QUERY_RESULT_AVAILABLE, &result);
    return (result > 0);
  }

  GLint GLQuery::GetResult(bool wait)
  {
    if (query_id == 0)
      return 0;
    assert(query_ended);

    GLint result = 0;
    glGetQueryObjectiv(query_id, wait? GL_QUERY_RESULT : GL_QUERY_RESULT_NO_WAIT, &result);
    return result;
  }

  GLint64 GLQuery::GetResult64(bool wait)
  {
    if (query_id == 0)
      return 0;
    assert(query_ended);

    GLint64 result = 0;
    glGetQueryObjecti64v(query_id, wait ? GL_QUERY_RESULT : GL_QUERY_RESULT_NO_WAIT, &result);
    return result;
  }

}; // namespace chaos
