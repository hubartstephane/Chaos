#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUVertexArray;

}; // namespace chaos

#else 

namespace chaos
{

	class GPUVertexArray : public GPUResource
	{
	public:

		/** constructor (create its own resource) */
		GPUVertexArray(Window * in_window = nullptr);
		/** constructor (reference a given resource). Call this function with 0 if you do not want to create resource at all */
		GPUVertexArray(Window* in_window, GLuint in_id, bool in_ownership);

		/** destructor */
		virtual ~GPUVertexArray();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return vertex_array_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const;

		/** create an OpenGL resource */
		bool CreateResource(Window* in_window);
		/** Initialize from GL Resource */
		bool SetResource(Window* in_window, GLuint in_id, bool in_ownership);

	protected:

		/** cleaning the object */
		virtual void Release() override;

	protected:

		/** the resource id */
		GLuint vertex_array_id = 0;
		/** whether the object has ownership of the GL resource */
		bool ownership = true;
		/** the window owning the vertex array (VA are not shared between context) */
		weak_ptr<Window> window;
		/** the corresponding context at moment of creation (used to know whether the window has been destroyed the recreated) */
		GLFWwindow* context = nullptr;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


