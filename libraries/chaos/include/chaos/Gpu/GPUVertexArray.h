namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUVertexArray;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GPUVertexArray : public GPUResource
	{
		friend class GPUVertexArrayCache;

	public:

		/** destructor */
		virtual ~GPUVertexArray();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return vertex_array_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const;

	protected:

		/** constructor */
		GPUVertexArray(GLuint in_id);
		/** cleaning the object */
		virtual void Release() override;

	protected:

		/** the resource id */
		GLuint vertex_array_id = 0;
	};

#endif

}; // namespace chaos