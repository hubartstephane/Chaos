namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUDevice;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUDevice: an object used to send commands and create resources from the GPU hardware
	 */

	class CHAOS_API GPUDevice : public Tickable, public ConfigurableInterface
	{
	public:

		/** initialize the device */
		virtual bool Initialize(JSONReadConfiguration config);
		/** finalize the device */
		virtual void Finalize();

	protected:

		/** internal method for initialization */
		virtual bool OnInitialize(JSONReadConfiguration config);
	};

#endif

}; // namespace chaos