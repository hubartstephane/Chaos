#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Manager.h>
#include <chaos/GPUTexture.h>
#include <chaos/GLTextureTools.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/FileResource.h>
#include <chaos/FilePath.h>

namespace chaos
{

	/**
	* GPUResourceManager : a manager to store different kinds of (can be depend) resources
	**/

	class GPUResourceManager : public Manager, protected FileResourceFriend
	{
		friend class GPUTextureLoader;
		friend class GPUProgramLoader;
		friend class GPURenderMaterialLoader;

	public:

		/** destructor */
		virtual ~GPUResourceManager();
		/** release the resources */
		virtual void Release();

		/** load a texture */
		GPUTexture * LoadTexture(FilePathParam const & path, char const * name = nullptr, GenTextureParameters const & texture_parameters = GenTextureParameters());
		/** load a program */
		GPUProgram * LoadProgram(FilePathParam const & path, char const * name = nullptr, class GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions());
		/** load a material */
		GPURenderMaterial * LoadRenderMaterial(FilePathParam const & path, char const * name = nullptr);

		/** find a texture by its name */
		GPUTexture * FindTexture(char const * name);
		/** find a texture by its name */
		GPUTexture const * FindTexture(char const * name) const;
		/** find a texture by its path */
		GPUTexture * FindTextureByPath(FilePathParam const & path);
		/** find a texture by its path */
		GPUTexture const * FindTextureByPath(FilePathParam const & path) const;

		/** find a program by its name */
		GPUProgram * FindProgram(char const * name);
		/** find a program by its name */
		GPUProgram const * FindProgram(char const * name) const;
		/** find a program by its path */
		GPUProgram * FindProgramByPath(FilePathParam const & path);
		/** find a program by its path */
		GPUProgram const * FindProgramByPath(FilePathParam const & path) const;

		/** find a render material by its name */
		GPURenderMaterial * FindRenderMaterial(char const * name);
		/** find a render material by its name */
		GPURenderMaterial const * FindRenderMaterial(char const * name) const;
		/** find a render material by its path */
		GPURenderMaterial * FindRenderMaterialByPath(FilePathParam const & path);
		/** find a render material by its path */
		GPURenderMaterial const * FindRenderMaterialByPath(FilePathParam const & path) const;

		/** initialize the manager from a file */
		virtual bool LoadManager(FilePathParam const & path);
		/** loading from a JSON object */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);

	protected:

		/** load the textures from configuration */
		virtual bool LoadTexturesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
		/** load the programs from configuration */
		virtual bool LoadProgramsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
		/** load the materials from configuration */
		virtual bool LoadMaterialsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);

		/** search (if necessary the parent of the material) */
		void SetRenderMaterialParent(GPURenderMaterial * render_material, std::string const & parent_name);

	protected:

		/** the textures */
		std::vector<shared_ptr<GPUTexture>> textures;
		/** the programs */
		std::vector<shared_ptr<GPUProgram>> programs;
		/** the render materials */
		std::vector<shared_ptr<GPURenderMaterial>> render_materials;
	};
}; // namespace chaos
