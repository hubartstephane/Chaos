#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/ReferencedObject.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUFileResource.h>

namespace chaos
{

	/**
	* GPUProgramRenderMaterialProvider : this is a variable provider dedicated for RenderMaterials
	*/

	class GPUProgramRenderMaterialProvider : public GPUProgramProvider
	{
	public:

		/** constructor */
		GPUProgramRenderMaterialProvider(class GPURenderMaterial const * in_render_material, GPUProgramProviderBase const * in_other_provider) :
			render_material(in_render_material),
			other_provider(in_other_provider)
		{}

	protected:

		/** apply the actions */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override;

	protected:

		/** the render material as base for the chain */
		GPURenderMaterial const * render_material = nullptr;
		/** another provider (use a non intrusive reference !!!) */
		GPUProgramProviderBase const * other_provider = nullptr;
	};

	/**
	* GPURenderMaterial : this is the combinaison of some uniforms and a program
	*/

	class GPURenderMaterial : public GPUFileResource
	{
		friend class GPUProgramData;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** constructor */
		GPURenderMaterial();

		/** destructor */
		virtual ~GPURenderMaterial();

		/** prepare the rendering */
		bool UseMaterial(GPUProgramProviderBase const * in_uniform_provider) const;

		/** set the program */
		bool SetProgram(GPUProgram * in_program);
		/** set the parent material */
		bool SetParentMaterial(GPURenderMaterial * in_parent);
		/** go throw the hierary and search for the program */
		GPUProgram const * GetEffectiveProgram() const;

		/** get the uniform provider */
		GPUProgramProvider & GetUniformProvider() { return uniform_provider; }
		/** get the uniform provider */
		GPUProgramProvider const & GetUniformProvider() const { return uniform_provider; }

	protected:

		/** cleaning the resource */
		virtual bool DoRelease() override;

	protected:

		/** the program */
		boost::intrusive_ptr<GPUProgram> program;
		/** parent material */
		boost::intrusive_ptr<GPURenderMaterial> parent_material;
		/** some rendering states */
		GPUProgramProvider uniform_provider;
	};


}; // namespace chaos
