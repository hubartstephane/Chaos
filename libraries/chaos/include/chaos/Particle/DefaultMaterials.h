namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename GENERATOR>
	class DefaultMaterialBase;

	class DefaultParticleProgramSource;
	class DefaultScreenSpaceProgramGenerator;

	using DefaultParticleProgram = DefaultMaterialBase<DefaultParticleProgramSource>;
	using DefaultScreenSpaceProgram = DefaultMaterialBase<DefaultScreenSpaceProgramGenerator>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * DefaultMaterialBase : base class to have default material
	 */
	template<typename SOURCE>
	class DefaultMaterialBase
	{
	public:

		static GPUProgram* GetProgram()
		{
			DefaultMaterialBase<SOURCE>& instance = GetInstance();

			if (instance.program == nullptr)
			{
				GPUProgramGenerator program_generator;
				SOURCE().GetSources(program_generator);
				instance.program = program_generator.GenProgramObject();
			}
			return instance.program.get();
		}

		/** get the material */
		static GPURenderMaterial* GetMaterial()
		{
			DefaultMaterialBase<SOURCE>& instance = GetInstance();

			if (instance.material == nullptr)
			{
				GPUProgram* p = GetProgram();
				if (p != nullptr)
					instance.material = GPURenderMaterial::GenRenderMaterialObject(p);
			}
			return instance.material.get();
		}

	protected:

		/** gets the singleton */
		static DefaultMaterialBase<SOURCE>& GetInstance()
		{
			static DefaultMaterialBase<SOURCE> result;
			return result;
		}

	protected:

		/** the program */
		shared_ptr<GPUProgram> program;
		/** the material */
		shared_ptr<GPURenderMaterial> material;
	};

	/**
	 * DefaultParticleProgramGenerator : generator for default particle program/material
	 */

	class DefaultParticleProgramSource
	{
	public:

		/** get the sources */
		void GetSources(GPUProgramGenerator& program_generator);

	public:

		/** the vertex shader source */
		static char const* vertex_shader_source;
		/** the pixel shader source */
		static char const* fragment_shader_source;
	};

	/**
	 * DefaultScreenSpaceProgramGenerator : generator for particle in screen space
	 */

	class DefaultScreenSpaceProgramGenerator
	{
	public:

		/** get the sources */
		void GetSources(GPUProgramGenerator& program_generator);

	public:

		/** the vertex shader source */
		static char const* vertex_shader_source;
		/** the pixel shader source */
		static char const* fragment_shader_source;
	};

#endif


}; // namespace chaos