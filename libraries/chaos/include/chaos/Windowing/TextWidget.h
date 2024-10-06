namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class TextWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Widget : some kind of "virtual sub-window"
	*/

	class CHAOS_API TextWidget : public Widget
	{
		TextWidget(TextWidget, Widget);

	public:

		/** override */
		virtual bool OnDraw(GPURenderer* renderer, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params) override;

		/** initialize widget content */
		virtual void Initialize(char const* str);

	protected:

		/** the mesh owning the text */
		shared_ptr<GPUMesh> mesh;
		/** the bounding box of the text */
		ParticleCorners bounding_box;
	};

#endif

}; // namespace chaos