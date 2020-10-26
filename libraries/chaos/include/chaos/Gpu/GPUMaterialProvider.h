#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    class GPUMaterialProvider;
    class GPUConstantMaterialProvider;

}; // namespace chaos

#else 

namespace chaos
{

	// ========================================================
	// GPUMaterialProvider : used to override the material used
	// ========================================================	

	class GPUMaterialProvider : public Object
	{
	public:

		/** the material provider main method */
		virtual GPURenderMaterial const * GetMaterial(GPURenderable const * renderable, GPURenderMaterial const * default_material, GPURenderParams const & render_params) const;
	};	

    // ========================================================
    // GPUConstantMaterialProvider : a material provider that always returns the same material
    // ========================================================	

    class GPUConstantMaterialProvider : public GPUMaterialProvider
    {
    public:

        /** constructor */
        GPUConstantMaterialProvider() = default;
        /** copy constructor */
        GPUConstantMaterialProvider(GPUConstantMaterialProvider const & src) = default;
        /** initialization constructor */
        GPUConstantMaterialProvider(GPURenderMaterial const* in_render_material) : 
            render_material(in_render_material) {}

        /** override */
        virtual GPURenderMaterial const* GetMaterial(GPURenderable const* renderable, GPURenderMaterial const* default_material, GPURenderParams const& render_params) const override
        {
            return render_material;
        }

    protected:

        /** the render material to use */
        GPURenderMaterial const * render_material = nullptr;
    };

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
