namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class GPUProgramType;

	class GPUProgram;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class GPUProgramType
	{
		RENDER,
		COMPUTE
	};

	class GPUProgram : public GPUFileResource
	{
		friend class GPUResourceManager;
		friend class GPUProgramGenerator;

	public:

		/** constructor */
		GPUProgram(GLuint in_id = 0, GPUProgramType in_type = GPUProgramType::RENDER);
		/** destructor */
		virtual ~GPUProgram();

		/** use the program */
		bool UseProgram(class GPUProgramProviderBase const* uniform_provider) const;

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return program_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsProgram(program_id) == GL_TRUE; }
		/** get the program data */
		GPUProgramData const& GetProgramData() const { return program_data; }
		/** get the type of the program */
		GPUProgramType GetProgramType() const { return type; }
		
		/** the default material */
		GPURenderMaterial* GetDefaultMaterial() const;

		/** override */
		virtual void Release() override;

	protected:

		/** the resource id */
		GLuint program_id = 0;
		/** the type of program */
		GPUProgramType type = GPUProgramType::RENDER;
		/** the program data */
		GPUProgramData program_data;
		/** the default material */
		shared_ptr<GPURenderMaterial> default_material;
	};

#endif

}; // namespace chaos