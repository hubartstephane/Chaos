#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GraphicResource.h>

namespace chaos
{

class GLQuery : public GraphicResource
{
public:

  /** constructor */
  GLQuery(GLuint in_id, GLenum in_target);
  /** constructor (generate the query itself) */
  GLQuery(GLenum target);

  /** destructor */
  virtual ~GLQuery();

  /** cleaning the object */
  virtual void Release() override;

  /** returns the GL name of the resource */
  GLuint GetResourceID() const { return query_id; }
  /** returns true whether the resource is valid */
  bool IsValid() const { return glIsQuery(query_id) == GL_TRUE;}

  /** start the query */
  bool BeginQuery();
  /** end the query */
  bool EndQuery();

  /** returns true whether the query is started */
  bool IsStarted() const { return query_started; }
  /** returns true whether the query is started */
  bool IsEnded() const { return query_ended; }
  /** returns true whether a conditional rendering is beeing started */
  bool IsConditionalRenderingStarted() const { return conditional_rendering_started; }

  /** returns true whether the result is availabled */
  bool IsResultAvailable();
  /** returns the result of the query */
  GLint GetResult(bool wait);
  /** returns the result of the query */
  GLint64 GetResult64(bool wait);

  /** start conditional rendering */
  bool BeginConditionalRendering(bool query_wait, bool inverted = false);
  /** end conditional rendering */
  bool EndConditionalRendering();

protected:

  /** the resource id */
  GLuint query_id;
  /** indicates whether the query has been already started */
  bool query_started = false;
  /** indicates whether the query has been already been begun/ended */
  bool query_ended = false;
  /** indicates whether a condtional rendering is being started */
  bool conditional_rendering_started = false;
  /** the target of the query */
  GLenum query_target = GL_NONE;
};


template<GLenum QUERY_TARGET>
class GLTypedQuery : public GLQuery
{
public:

  GLTypedQuery() : GLQuery(QUERY_TARGET) {}
};

using GLSamplesPassedQuery = GLTypedQuery<GL_SAMPLES_PASSED>;
using GLAnySamplesPassedQuery = GLTypedQuery<GL_ANY_SAMPLES_PASSED>;
using GLAnySamplesPassedConservativeQuery = GLTypedQuery<GL_ANY_SAMPLES_PASSED_CONSERVATIVE>;
using GLPrimitieGeneratedQuery = GLTypedQuery<GL_PRIMITIVES_GENERATED>;
using GLTransformFeedbackPrimitiveWrittenQuery = GLTypedQuery<GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN>;
using GLTimeElapsedQuery = GLTypedQuery<GL_TIME_ELAPSED>;
using GLTimeStampQuery = GLTypedQuery<GL_TIMESTAMP>;

}; // namespace chaos
