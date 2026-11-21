namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class PhysicalGamepad;
	
#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* PhysicalGamepad : the physical device. Client do not directly use it
	*/
	class CHAOS_API PhysicalGamepad : public InputDeviceInterface
	{
		friend class Gamepad;
		friend class GamepadManager;

	public:

		/** get a reference on the state */
		GamepadState const* GetGamepadState() const { return &gamepad_state; }

		/** returns the gamepad index */
		size_t GetGamepadIndex() const { return gamepad_index; }
		/** returns whether the gamepad is allocated for a user */
		bool IsAllocated() const { return (user_gamepad != nullptr); }
		/** returns true whether the gamepad is connected */
		bool IsPresent() const { return is_present; }
		/** return the name of gamepad */
		char const* GetName() const;
		/** return the GUID of gamepad model */
		char const * GetGUID() const;

		/** take a physical device, and create a logical device if possible */
		class Gamepad* CaptureDevice(GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks);

	protected:

		/** override */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const override;

		/** update all the values for the axis and buttons */
		void UpdateAxisAndButtons(GamepadInputFilterSettings const & in_filter_settings);
		/** called at disconnection to be sure input cannot be consulted anymore */
		void ClearInputs();

	protected:

		/** the manager */
		class GamepadManager* gamepad_manager = nullptr;
		/** the current gamepad index */
		size_t gamepad_index = 0;
		/** indicates whether the gamepad is present */
		bool is_present = false;
		/** indicates whether the gamepad is allocated to a client */
		class Gamepad* user_gamepad = nullptr;
		/** the device state */
		GamepadState gamepad_state;
	};

#endif

}; // namespace chaos