namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Gamepad;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Gamepad : this is a logical gamepad .. may change the physical gamepad it is bound on
	*/
	class CHAOS_API Gamepad : public Object, public InputDeviceInterface, public SupervisableInterface
	{
	public:

		friend class GamepadManager;

	protected:

		/** constructor */
		Gamepad(GamepadManager* in_gamepad_manager, PhysicalGamepad* in_physical_gamepad, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks);
		/** destructor */
		~Gamepad();

	public:

		/** get a reference on the state (if connected) */
		GamepadState const* GetGamepadState() const;

		/** returns the gamepad index */
		size_t GetGamepadIndex() const;
		/** returns true whether the gamepad is connected */
		bool IsPresent() const;
		/** returns whether the gamepad has already been connected once */
		bool IsEverConnected() const { return ever_connected; }

		/** give a callback object to the gamepad */
		void SetCallbacks(GamepadCallbacks* in_callbacks);

		/** returns whether the force feedback is enabled */
		bool IsForceFeedbackEnabled() const { return force_feedback_enabled; }
		/** set whether the force feedback is enabled */
		void SetForceFeedbackEnabled(bool in_enabled);

		/** returns whether the force feedback is muted */
		bool IsForceFeedbackMuted() const { return force_feedback_muted; }
		/** set whether the force feedback is muted */
		void SetForceFeedbackMuted(bool in_muted);

		/** returns whether the force feedback is paused */
		bool IsForceFeedbackPaused() const { return force_feedback_paused; }
		/** set whether the force feedback is paused */
		void SetForceFeedbackPaused(bool in_paused);

		/** add a force feedback effect */
		void AddForceFeedbackEffect(ForceFeedbackEffect* effect);
		/** remove a forcefeedback effect */
		bool RemoveForceFeedbackEffect(ForceFeedbackEffect* effect);
		/** remove force feedback effects */
		void ClearForceFeedbackEffects();

		/** clear all inputs of the gamepad */
		void ClearInputs();

		/** get the filter settings */
		GamepadInputFilterSettings const& GetInputFilterSettings() const;
		/** set the filter settings */
		void SetInputFilterSettings(GamepadInputFilterSettings const& in_filter_settings);

	protected:

		/** override */
		virtual void RemoveFromSuperviser() override;

		/** override */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const override;

		/** tick force feedback effects */
		void TickForceFeedbackEffects(float delta_time);
		/** setting the left & right values to the device */
		void DoUpdateForceFeedbackDevice(ForceFeedbackMotorValues in_motor_values);
		/** returns whether forcefeedback should be reduced to 0 */
		bool ShouldReduceForceFeedbackToZero() const;

		/** remove an effect at a given index */
		bool RemoveForceFeedbackAt(size_t index);

	protected:

		/** the manager */
		class GamepadManager* gamepad_manager = nullptr;
		/* the device */
		PhysicalGamepad* physical_gamepad = nullptr;
		/** configuration to handle input1D & input2D */
		GamepadInputFilterSettings filter_settings;
		/** the callbacks */
		shared_ptr<GamepadCallbacks> callbacks;
		/** indicates whether the gamepad has already be connected to a physical device */
		bool ever_connected = false;
		/** indicates whether the force feedback is enabled */
		bool force_feedback_enabled = true;
		/** indicates whether the force feedback is muted */
		bool force_feedback_muted = false;
		/** indicates whether the force feedback must be ticked or not */
		bool force_feedback_paused = false;

		/** the forcefeedback effects */
		std::vector<shared_ptr<ForceFeedbackEffect>> feedback_effects;
	};

#endif

}; // namespace chaos