namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUDevice;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUDevice: an object used to send commands and create resources from the GPU hardware
	 */

	class CHAOS_API GPUDevice : public Tickable, public ConfigurationUserInterface
	{
	public:

		/** initialize the device */
		virtual bool Initialize(JSONReadConfiguration config);
		/** finalize the device */
		virtual void Finalize();

		/** create a texture */
		GPUTexture * CreateTexture(TextureDescription const& in_texture_description = {});

	protected:

		/** internal method for initialization */
		virtual bool OnInitialize(JSONReadConfiguration config);
	};

#endif

}; // namespace chaos