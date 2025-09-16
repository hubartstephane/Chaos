namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<int COUNT> 
	class ComposedInputDevice; 

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ComposedInputDevice: compose multiple InputDevice into a single one
	*/

	template<int COUNT> 
	class CHAOS_API ComposedInputDevice : public InputDeviceInterface
	{

	public:

		/** constructor */
		template<typename... PARAMS>
		ComposedInputDevice(PARAMS... params)
			: child_devices{ { static_cast<InputDeviceInterface const*>(std::forward<PARAMS>(params))... } }
		{
			static_assert(sizeof...(PARAMS) == COUNT);
			static_assert((std::is_convertible_v<std::remove_reference_t<PARAMS>, InputDeviceInterface const*> && ...));
		}

		/** override */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const override
		{
			for (InputDeviceInterface const * input_device : child_devices)
				if (input_device != nullptr)
					if (input_device->EnumerateDeviceHierarchy(func))
						return true;
			return false;
		}

	protected:

		/** the devices */
		std::array<InputDeviceInterface const *, COUNT> child_devices;
	};

	/**
	 * Utility functions
	 **/

	template<typename... PARAMS>
	auto MakeComposedInputDevices(PARAMS&&... params)
	{
		return ComposedInputDevice<sizeof...(PARAMS)>(std::forward<PARAMS>(params)...);
	}

#endif

}; // namespace chaos