namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUQuery;

	template<GLenum QUERY_TARGET>
	class GLTypedQuery;

	using GLSamplesPassedQuery = GLTypedQuery<GL_SAMPLES_PASSED>;
	using GLAnySamplesPassedQuery = GLTypedQuery<GL_ANY_SAMPLES_PASSED>;
	using GLAnySamplesPassedConservativeQuery = GLTypedQuery<GL_ANY_SAMPLES_PASSED_CONSERVATIVE>;
	using GLPrimitieGeneratedQuery = GLTypedQuery<GL_PRIMITIVES_GENERATED>;
	using GLTransformFeedbackPrimitiveWrittenQuery = GLTypedQuery<GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN>;
	using GLTimeElapsedQuery = GLTypedQuery<GL_TIME_ELAPSED>;
	using GLTimeStampQuery = GLTypedQuery<GL_TIMESTAMP>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class GPUQuery : public GPUResource
	{
	public:

		/** constructor (create its own resource) */
		GPUQuery(Window* in_window = nullptr, GLenum in_target = GL_NONE);
		/** constructor (reference a given resource). Call this function with 0 if you do not want to create resource at all */
		GPUQuery(Window* in_window, GLuint in_id, bool in_ownership);

		/** destructor */
		virtual ~GPUQuery();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return query_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const;

		/** create an OpenGL resource */
		bool CreateResource(Window* in_window, GLenum in_target);
		/** Initialize from GL Resource */
		bool SetResource(Window* in_window, GLuint in_id, bool in_ownership);

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

		/** override */
		virtual void Release() override;

	protected:

		/** the resource id */
		GLuint query_id = 0;
		/** whether the object has ownership of the GL resource */
		bool ownership = true;
		/** the window owning the query (queries are not shared between context) */
		weak_ptr<Window> window;
		/** the corresponding context at moment of creation (used to know whether the window has been destroyed the recreated) */
		GLFWwindow* context = nullptr;

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
	class GLTypedQuery : public GPUQuery
	{
	public:

		GLTypedQuery(Window* in_window = nullptr) : GPUQuery(in_window, QUERY_TARGET) {}
	};

#endif

}; // namespace chaos