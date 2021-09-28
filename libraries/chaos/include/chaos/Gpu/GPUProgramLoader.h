namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUProgramLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUProgramLoader : this class deserves to generate GPU programs from sources.
	*                    It is possible to use cache system and to add some definitions so we can generate multiple programs with small macro differences.
	*/

	class GPUProgramLoader : public ResourceManagerLoader<GPUProgram, GPUResourceManager>
	{
	public:

		/** constructor */
		GPUProgramLoader(GPUResourceManager* in_resource_manager = nullptr) :
			ResourceManagerLoader<GPUProgram, GPUResourceManager>(in_resource_manager) {}

		/** load an object from JSON */
		virtual GPUProgram* LoadObject(char const* name, nlohmann::json const& json, boost::filesystem::path const& config_path) const;
		/** program loading from path */
		virtual GPUProgram* LoadObject(FilePathParam const& path, char const* name = nullptr) const;

		/** Generate a program from a json content */
		virtual GPUProgram* GenProgramObject(nlohmann::json const& json, boost::filesystem::path const& config_path) const;
		/** Generate a program from an file */
		virtual GPUProgram* GenProgramObject(FilePathParam const& path) const;

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const& path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(ObjectRequest request) const override;
	};

#endif

}; // namespace chaos