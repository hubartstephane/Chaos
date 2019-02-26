#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Manager.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/GPUFileResource.h>
#include <chaos/FilePath.h>

namespace chaos
{

	/**
	* GPUResourceManager : a manager to store different kinds of (can be depend) resources
	**/

	class GPUResourceManager : public Manager, protected GPUFileResourceFriend
	{
		friend class GPURenderMaterialLoader;
		friend class RenderMaterialFromConfigLoader;

	public:

		/** destructor */
		virtual ~GPUResourceManager();
		/** release the resources */
		virtual void Release();

		/** load a texture (named deduced from path) */
		GPUTexture * LoadTexture(FilePathParam const & path);
		/** load a texture */
		GPUTexture * LoadTexture(FilePathParam const & path, char const * name);
		/** load a program (named deduced from path) */
		GPUProgram * LoadProgram(FilePathParam const & path);
		/** load a program */
		GPUProgram * LoadProgram(FilePathParam const & path, char const * name);
		/** load a material (named deduced from path) */
		GPURenderMaterial * LoadRenderMaterial(FilePathParam const & path);
		/** load a material */
		GPURenderMaterial * LoadRenderMaterial(FilePathParam const & path, char const * name);

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

		/** returns whether a texture with given name can be inserted */
		virtual bool CanAddTexture(char const * name) const;
		/** returns whether a program with given name can be inserted */
		virtual bool CanAddProgram(char const * name) const;
		/** returns whether a render material with given name can be inserted */
		virtual bool CanAddRenderMaterial(char const * name) const;

		/** load the textures from configuration */
		virtual bool LoadTexturesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
		/** load the programs from configuration */
		virtual bool LoadProgramsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
		/** load the materials from configuration */
		virtual bool LoadMaterialsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);

		/** add a texture from a JSON object */
		virtual GPUTexture * LoadTexture(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path);
		/** add a program from a JSON object */
		virtual GPUProgram * LoadProgram(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path);
		/** add a material from a JSON object (return the name of the parent material if any) */
		virtual GPURenderMaterial * LoadRenderMaterial(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name);

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
