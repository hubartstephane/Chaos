namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUDeviceResourceInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUDeviceResourceInterface: for all objects owned by a GPUDevice
	 */

	class CHAOS_API GPUDeviceResourceInterface
	{
	public:

		/** constructor */
		GPUDeviceResourceInterface(GPUDevice * in_gpu_device);

		/** getter on the device */
		GPUDevice * GetDevice() const { return gpu_device.get(); }

	protected:

		/** the device owning this object */
		weak_ptr<GPUDevice> gpu_device;
	};

#endif

}; // namespace chaos