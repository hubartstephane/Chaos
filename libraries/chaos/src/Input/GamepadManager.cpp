#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
#if _DEBUG
	namespace GlobalVariables
	{
		CHAOS_GLOBAL_VARIABLE(bool, ForceFeedbackEnabled, true);
	};
#endif

	//
	// GamepadInputFilterSettings functions
	//

	bool DoSaveIntoJSON(nlohmann::json* json, GamepadInputFilterSettings const& src)
	{
		JSONTools::SetAttribute(json, "length_interpolation_type", src.length_interpolation_type);
		JSONTools::SetAttribute(json, "dead_zone", src.dead_zone);
		JSONTools::SetAttribute(json, "max_zone", src.max_zone);
		JSONTools::SetAttribute(json, "sector_snap_angle_ratio", src.sector_snap_angle_ratio);
		JSONTools::SetAttribute(json, "sector_snap_count", src.sector_snap_count);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, GamepadInputFilterSettings& dst)
	{
		JSONTools::GetAttribute(config, "length_interpolation_type", dst.length_interpolation_type);
		JSONTools::GetAttribute(config, "dead_zone", dst.dead_zone);
		JSONTools::GetAttribute(config, "max_zone", dst.max_zone);
		JSONTools::GetAttribute(config, "sector_snap_angle_ratio", dst.sector_snap_angle_ratio);
		JSONTools::GetAttribute(config, "sector_snap_count", dst.sector_snap_count);
		return true;
	}

	//
	// PhysicalGamepad functions
	//

	void PhysicalGamepad::ClearInputs()
	{
		gamepad_state.Clear();
	}

	bool PhysicalGamepad::EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const
	{
		if (IsPresent())
			if (gamepad_state.EnumerateDeviceHierarchy(func))
				return true;
		return InputDeviceInterface::EnumerateDeviceHierarchy(func);
	}

	void PhysicalGamepad::UpdateAxisAndButtons(GamepadInputFilterSettings const& in_filter_settings)
	{
		if (!IsPresent())
			return;
		gamepad_state.UpdateAxisAndButtons(gamepad_index, in_filter_settings);
	}

	Gamepad* PhysicalGamepad::CaptureDevice(GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks)
	{
		return gamepad_manager->DoCaptureDevice(this, in_filter_settings, in_callbacks);
	}

	//
	// ForceFeedbackEffect functions
	//

	void ForceFeedbackEffect::SubReference()
	{
		// the object is handled as usual
		if (gamepad == nullptr)
			Object::SubReference();
		// the last reference is the one from the gamepad. Destroy it
		else if (--shared_count == 1)
		{
			assert(gamepad != nullptr);
			RemoveFromGamepad();
		}
	}

	bool ForceFeedbackEffect::TickAndGetMotorValues(float delta_time, ForceFeedbackMotorValues& out_motor_values)
	{
		out_motor_values = {};
		return false; // do not destroy the effect
	}

	void ForceFeedbackEffect::RemoveFromGamepad()
	{
		if (gamepad == nullptr)
			return;
		gamepad->RemoveForceFeedbackEffect(this);
	}

	//
	// DefaultForceFeedbackEffect functions
	//

	bool DefaultForceFeedbackEffect::TickAndGetMotorValues(float delta_time, ForceFeedbackMotorValues& out_motor_values)
	{
		if (duration > 0.0f)
		{
			timer += delta_time;
			if (timer >= duration)
			{
				out_motor_values = {};
				return true; // end of the effect
			}
		}
		out_motor_values = motor_values;
		return false; // do not destroy the effect
	}

	//
	// Gamepad functions
	//

	Gamepad::Gamepad(GamepadManager* in_gamepad_manager, PhysicalGamepad* in_physical_gamepad, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks) :
		gamepad_manager(in_gamepad_manager),
		physical_gamepad(in_physical_gamepad),
		filter_settings(*in_filter_settings),
		callbacks(in_callbacks)
	{
		assert(in_gamepad_manager != nullptr);
		assert(in_filter_settings != nullptr);
	}

	void Gamepad::SubReference()
	{
		// the object is handled as usual
		if (gamepad_manager == nullptr)
			Object::SubReference();
		// the last reference is the one from the manager. Destroy it
		else if (--shared_count == 1)
		{
			assert(gamepad_manager != nullptr);
			gamepad_manager->RemoveGamepad(this);
		}
	}

	bool Gamepad::EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const
	{
		if (physical_gamepad != nullptr)
			if (physical_gamepad->EnumerateDeviceHierarchy(func))
				return true;
		return InputDeviceInterface::EnumerateDeviceHierarchy(func);
	}

	void Gamepad::ClearInputs()
	{
		if (physical_gamepad == nullptr)
			return;
		physical_gamepad->ClearInputs();
	}

	bool Gamepad::IsPresent() const
	{
		if (physical_gamepad == nullptr)
			return false;
		return physical_gamepad->IsPresent();
	}

	size_t Gamepad::GetGamepadIndex() const
	{
		if (physical_gamepad == nullptr)
			return 0;
		return physical_gamepad->GetGamepadIndex();
	}

	void Gamepad::SetCallbacks(GamepadCallbacks* in_callbacks)
	{
		callbacks = in_callbacks;
	}

	GamepadState const* Gamepad::GetGamepadState() const
	{
		if (physical_gamepad == nullptr)
			return nullptr;
		return physical_gamepad->GetGamepadState();
	}

	void Gamepad::ClearForceFeedbackEffects()
	{
		while (feedback_effects.size() > 0)
			RemoveForceFeedbackAt(feedback_effects.size() - 1);
		TickForceFeedbackEffects(0.0f);
	}

	bool Gamepad::ShouldReduceForceFeedbackToZero() const
	{
		return (force_feedback_paused || force_feedback_muted || !force_feedback_enabled);
	}

	void Gamepad::SetForceFeedbackPaused(bool in_paused)
	{
		force_feedback_paused = in_paused;
		if (ShouldReduceForceFeedbackToZero()) // immediate update. because not sure, the Gamepad will ever be ticked
			TickForceFeedbackEffects(0.0f);
	}

	void Gamepad::SetForceFeedbackMuted(bool in_muted)
	{
		force_feedback_muted = in_muted;
		if (ShouldReduceForceFeedbackToZero()) // immediate update. because not sure, the Gamepad will ever be ticked
			TickForceFeedbackEffects(0.0f);
	}

	void Gamepad::SetForceFeedbackEnabled(bool in_enabled)
	{
		force_feedback_enabled = in_enabled;
		if (ShouldReduceForceFeedbackToZero()) // immediate update. because not sure, the Gamepad will ever be ticked
			TickForceFeedbackEffects(0.0f);
	}

	void Gamepad::DoUpdateForceFeedbackDevice(ForceFeedbackMotorValues in_motor_values)
	{
		if (!IsPresent())
			return;
#if _WIN64
		if (GamepadManager::XInputSetStateFunc == nullptr)
			return;
#if _DEBUG
		if (!GlobalVariables::ForceFeedbackEnabled.Get())
			in_motor_values = {};
#endif // _DEBUG

		in_motor_values.left_value  = std::clamp(in_motor_values.left_value, 0.0f, 1.0f);
		in_motor_values.right_value = std::clamp(in_motor_values.right_value, 0.0f, 1.0f);

		XINPUT_VIBRATION vibration;
		vibration.wLeftMotorSpeed = (WORD)(in_motor_values.left_value * 65535.0f);
		vibration.wRightMotorSpeed = (WORD)(in_motor_values.right_value * 65535.0f);
		GamepadManager::XInputSetStateFunc(DWORD(GetGamepadIndex()), &vibration);
#endif // _WIN64
	}

	void Gamepad::TickForceFeedbackEffects(float delta_time)
	{
		ForceFeedbackMotorValues final_motor_values;

		// compute force feedback intensity
		if (!force_feedback_paused)
		{
			size_t count = feedback_effects.size();
			for (size_t i = count; i > 0; --i) // in reverse order for better removal
			{
				size_t index = i - 1;

				ForceFeedbackMotorValues motor_values;
				if (feedback_effects[index]->TickAndGetMotorValues(delta_time, motor_values))
					RemoveForceFeedbackAt(index);

				final_motor_values.left_value  = std::max(final_motor_values.left_value, motor_values.left_value);
				final_motor_values.right_value = std::max(final_motor_values.right_value, motor_values.right_value);
			}
		}
		// reduce feedback to 0 (do not skip TickAndGetMotorValues(...) calls so the effects tick themselves even if muted
		if (ShouldReduceForceFeedbackToZero())
			final_motor_values = {};
		// update the device
		DoUpdateForceFeedbackDevice(final_motor_values);
	}

	void Gamepad::AddForceFeedbackEffect(ForceFeedbackEffect* effect)
	{
		assert(effect != nullptr);
		assert(effect->gamepad == nullptr);
		effect->gamepad = this;
		feedback_effects.push_back(effect);
	}

	bool Gamepad::RemoveForceFeedbackEffect(ForceFeedbackEffect* effect)
	{
		assert(effect != nullptr);
		assert(effect->gamepad == this);

		size_t count = feedback_effects.size();
		for (size_t i = count; i > 0; --i) // from end to beginning because ... maybe a future DetachAllFeedback(...) function will be from end to beginning
		{
			size_t index = i - 1;
			if (feedback_effects[index] == effect)
				return RemoveForceFeedbackAt(index);
		}
		return false;
	}

	bool Gamepad::RemoveForceFeedbackAt(size_t index)
	{
		ForceFeedbackEffect * effect = feedback_effects[index].get();
		assert(effect != nullptr);
		assert(effect->gamepad == this);

		effect->gamepad = nullptr;

		feedback_effects[index] = feedback_effects[feedback_effects.size() - 1];
		feedback_effects.pop_back();

		return true;
	}

	GamepadInputFilterSettings const& Gamepad::GetInputFilterSettings() const
	{
		return filter_settings;
	}
	void Gamepad::SetInputFilterSettings(GamepadInputFilterSettings const& in_filter_settings)
	{
		filter_settings = in_filter_settings;
	}

	//
	// GamepadPollInterface functions
	//

	bool GamepadPollInterface::DoPollGamepad(PhysicalGamepad * physical_gamepad)
	{
		assert(physical_gamepad != nullptr);
		return true;
	}

	//
	// GamepadManager functions
	//

#if _WIN64
	GamepadManager::XINPUT_SET_STATE_FUNC GamepadManager::XInputSetStateFunc = nullptr;
#endif

	GamepadManager::GamepadManager(GamepadInputFilterSettings const & in_gamepad_filter_settings):
		gamepad_filter_settings(in_gamepad_filter_settings)
	{
		// load manually module so that this work on windows 7 & windows +
#if _WIN64
		if (XInputSetStateFunc == nullptr)
		{
			HMODULE hModule = LoadLibrary("XInput1_4.dll"); // try to load both library
			if (hModule == NULL)
				hModule = LoadLibrary("xinput9_1_0.dll"); // try to load both library
			if (hModule != NULL)
				XInputSetStateFunc = (XINPUT_SET_STATE_FUNC)GetProcAddress(hModule, "XInputSetState");
		}
#endif
		// initialize all physical gamepads
		for (size_t i = 0; i < MAX_SUPPORTED_GAMEPAD_COUNT; ++i)
		{
			PhysicalGamepad& physical_gamepad = physical_gamepads[i];
			physical_gamepad.gamepad_index   = i;
			physical_gamepad.gamepad_manager = this;

			int physical_index = int(i);

			if (glfwJoystickPresent(physical_index) && glfwJoystickIsGamepad(physical_index))
			{
				present_physical_gamepads |= uint32_t(1 << i);
				
				physical_gamepad.is_present = true;
				physical_gamepad.UpdateAxisAndButtons(gamepad_filter_settings);
			}
		}
	}

	GamepadManager::~GamepadManager()
	{
		while (user_gamepads.size() > 0)
			RemoveGamepadAt(user_gamepads.size() - 1); // remove last gamepad
	}

	bool GamepadManager::RemoveGamepadAt(size_t index)
	{
		Gamepad* gamepad = user_gamepads[index].get();
		assert(gamepad != nullptr);
		assert(gamepad->gamepad_manager == this);

		// remove all force feedback effect that could remains on physical device after its destruction
		gamepad->ClearForceFeedbackEffects();

		// give physical device back to the manager		
		if (PhysicalGamepad* physical_gamepad = gamepad->physical_gamepad)
		{
			allocated_physical_gamepads &= ~uint32_t(1 << physical_gamepad->gamepad_index);

			physical_gamepad->user_gamepad = nullptr;
			gamepad->physical_gamepad = nullptr;

			if (gamepad->callbacks != nullptr)
				gamepad->callbacks->OnGamepadDisconnected(gamepad); // notify that the gamepad is loosing its physical device
		}

		gamepad->gamepad_manager = nullptr;

		user_gamepads[index] = user_gamepads[user_gamepads.size() - 1];
		user_gamepads.pop_back();
		return true;
	}

	bool GamepadManager::RemoveGamepad(Gamepad* in_gamepad)
	{
		size_t count = user_gamepads.size();
		for (size_t i = count; i > 0; --i)
		{
			size_t index = i - 1;

			Gamepad* gamepad = user_gamepads[index].get();
			if (gamepad == in_gamepad)
				return RemoveGamepadAt(index);
		}
		return true;
	}

	size_t GamepadManager::GetPhysicalGamepadCount() const
	{
		return size_t(BitTools::popcount(present_physical_gamepads));
	}

	void GamepadManager::TickForceFeedbackEffects(float delta_time)
	{
		for (auto & gamepad : user_gamepads)
			if (gamepad->IsPresent())
				gamepad->TickForceFeedbackEffects(delta_time);
	}

	void GamepadManager::Tick(float delta_time, GamepadPollInterface* in_poll_target)
	{
		if (!enabled)
			return;
		// update physical gamepad state / handle disconnection
		UpdateAndHandlePhysicalDeviceDisconnection();
		// try to give all logical device a physical device
		AssociateUnallocatedPhysicalDevicesToGamepads();
		// uncatched inputs polling
		PollInputs(in_poll_target);
		// tick the force feedback effects
		TickForceFeedbackEffects(delta_time);
	}

	void GamepadManager::PollInputs(GamepadPollInterface * in_poll_target)
	{
		if (in_poll_target == nullptr || !polling_enabled)
			return;

		uint32_t unallocated_present_physical_gamepads = present_physical_gamepads & ~allocated_physical_gamepads; // only poll unallocated physical gamepads

		BitTools::ForEachBitForward(unallocated_present_physical_gamepads, [&](uint32_t gamepad_index)
		{
			PhysicalGamepad & physical_gamepad = physical_gamepads[size_t(gamepad_index)];
			assert(physical_gamepad.IsPresent());
			assert(!physical_gamepad.IsAllocated());

			if (physical_gamepad.IsAnyInputActive())
				in_poll_target->DoPollGamepad(&physical_gamepad);
		});
	}

	void GamepadManager::UpdateAndHandlePhysicalDeviceDisconnection()
	{
		for (PhysicalGamepad & physical_gamepad : physical_gamepads)
		{
			uint32_t physical_gamepad_bit = uint32_t(1 << physical_gamepad.gamepad_index);

			int  physical_index = int(physical_gamepad.gamepad_index);
			bool is_present     = glfwJoystickPresent(physical_index) && glfwJoystickIsGamepad(physical_index);
			bool was_present    = physical_gamepad.IsPresent();

			physical_gamepad.is_present = is_present;
			
			if (is_present)
			{
				present_physical_gamepads |= physical_gamepad_bit;

				physical_gamepad.UpdateAxisAndButtons((physical_gamepad.user_gamepad != nullptr) ? physical_gamepad.user_gamepad->filter_settings : gamepad_filter_settings);

				if (physical_gamepad.user_gamepad != nullptr && physical_gamepad.user_gamepad->callbacks != nullptr)
					physical_gamepad.user_gamepad->callbacks->OnGamepadStateUpdated(physical_gamepad.gamepad_state); // notify the user of the update
			}
			else if (was_present)
			{
				present_physical_gamepads   &= ~physical_gamepad_bit; // gamepad is not present anymore
				allocated_physical_gamepads &= ~physical_gamepad_bit; // gamepad is no allocated anymore (if it was)

				physical_gamepad.ClearInputs();

				if (Gamepad* user_gamepad = physical_gamepad.user_gamepad)
				{
					user_gamepad->physical_gamepad = nullptr; // unbind physical and logical devices
					physical_gamepad.user_gamepad = nullptr;
					if (user_gamepad->callbacks != nullptr)
						user_gamepad->callbacks->OnGamepadDisconnected(user_gamepad);
				}
			}
		}
	}

	Gamepad* GamepadManager::DoCaptureDevice(PhysicalGamepad* in_physical_gamepad, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks)
	{
		assert(in_physical_gamepad != nullptr);

		// the physical device is not present
		if (!in_physical_gamepad->IsPresent())
			return nullptr;
		// the device is already allocated
		if (in_physical_gamepad->IsAllocated())
			return nullptr;

		return DoAllocateGamepad(in_physical_gamepad, in_filter_settings, in_callbacks);
	}

	Gamepad* GamepadManager::AllocateGamepad(bool want_present, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks) // user explicitly require a gamepad
	{
		PhysicalGamepad* physical_gamepad = FindUnallocatedPhysicalDevice(in_callbacks);
		if (want_present && physical_gamepad == nullptr) // all physical device in use or not present ?
			return nullptr;

		return DoAllocateGamepad(physical_gamepad, in_filter_settings, in_callbacks);
	}

	PhysicalGamepad* GamepadManager::FindUnallocatedPhysicalDevice(GamepadCallbacks* in_callbacks)
	{
		uint32_t unallocated_present_physical_gamepads = present_physical_gamepads & ~allocated_physical_gamepads; // only interrested in present unallocated devices

		PhysicalGamepad* best_physical_gamepad = nullptr;

		// the 1st best is to find a physical PRESENT gamepad and that has any input
		// the 2nd best is to find a physical PRESENT gamepad (even if no input)
		PhysicalGamepad * result = BitTools::ForEachBitForward(unallocated_present_physical_gamepads, [&](uint32_t gamepad_index) -> PhysicalGamepad *
		{
			PhysicalGamepad & physical_gamepad = physical_gamepads[size_t(gamepad_index)];
			assert(physical_gamepad.IsPresent());
			assert(!physical_gamepad.IsAllocated());

			if (in_callbacks != nullptr && !in_callbacks->AcceptPhysicalDevice(&physical_gamepad)) // user filter
				return nullptr;

			if (physical_gamepad.IsAnyInputActive()) // no better choice can be expected
				return &physical_gamepad;

			if (best_physical_gamepad == nullptr)
				best_physical_gamepad = &physical_gamepad; // keep the first that fits

			return nullptr;
		});

		return (result != nullptr) ? result : best_physical_gamepad;
	}

	Gamepad* GamepadManager::DoAllocateGamepad(PhysicalGamepad* in_physical_gamepad, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks)
	{
		if (in_filter_settings == nullptr)
			in_filter_settings = &gamepad_filter_settings;

		Gamepad* result = new Gamepad(this, in_physical_gamepad, in_filter_settings, in_callbacks);
		if (result == nullptr)
			return nullptr;

		user_gamepads.push_back(result);

		if (in_physical_gamepad != nullptr)
		{
			assert(in_physical_gamepad->IsPresent());
			assert(!in_physical_gamepad->IsAllocated());

			result->ever_connected = true;

			in_physical_gamepad->user_gamepad = result;

			allocated_physical_gamepads |= uint32_t(1 << in_physical_gamepad->gamepad_index);

			if (in_callbacks != nullptr)
				in_callbacks->OnGamepadConnected(result);
		}
		return result;
	}

	void GamepadManager::AssociateUnallocatedPhysicalDevicesToGamepads()
	{
		for (bool require_active_inputs : {true, false}) // 2 steps algorithm : 1 - physical device with any inputs  2 - any physical device (even without inputs)
		{
			uint32_t unallocated_present_physical_gamepads = present_physical_gamepads & ~allocated_physical_gamepads;

			BitTools::ForEachBitForward(unallocated_present_physical_gamepads, [&](uint32_t gamepad_index)
			{
				PhysicalGamepad & physical_gamepad = physical_gamepads[size_t(gamepad_index)];
				assert(physical_gamepad.IsPresent());
				assert(!physical_gamepad.IsAllocated());

				if (require_active_inputs && !physical_gamepad.IsAnyInputActive()) // in step 0, ignore gamepads that have no inputs
					return;
				DoGivePhysicalDeviceToAnyGamepad(&physical_gamepad);
			});
		}
	}

	Gamepad * GamepadManager::DoGivePhysicalDeviceToAnyGamepad(PhysicalGamepad * in_physical_gamepad)
	{
		assert(in_physical_gamepad != nullptr);

		Gamepad* result = FindBestGamepadCandidateForPhysicalDevice(in_physical_gamepad);
		if (result == nullptr)
			return nullptr;

		allocated_physical_gamepads |= uint32_t(1 << in_physical_gamepad->gamepad_index);

		in_physical_gamepad->user_gamepad = result;
		result->physical_gamepad = in_physical_gamepad;
		result->ever_connected   = true;

		if (result->callbacks != nullptr)
			result->callbacks->OnGamepadConnected(result);

		return result;
	}

	Gamepad* GamepadManager::FindBestGamepadCandidateForPhysicalDevice(PhysicalGamepad* in_physical_gamepad)
	{
		assert(in_physical_gamepad != nullptr);

		Gamepad* best_gamepad = nullptr;

		size_t count = user_gamepads.size();
		for (auto & gamepad : user_gamepads)
		{
			if (gamepad->physical_gamepad != nullptr) // already bound
				continue;
			if (gamepad->callbacks != nullptr && !gamepad->callbacks->AcceptPhysicalDevice(in_physical_gamepad)) // user filter
				continue;
			if (gamepad->IsEverConnected()) // the best is to use a gamepad that has already received a physical device (more likely to have been really used)
				return gamepad.get();
			if (best_gamepad == nullptr)
				best_gamepad = gamepad.get(); // in a second step, we will return any gamepad (by order of allocation)
		}
		return best_gamepad;
	}

	bool GamepadManager::HasAnyInputs(size_t gamepad_index, float dead_zone)
	{
		int physical_index = int(gamepad_index);

		if (glfwJoystickPresent(physical_index) == FALSE || glfwJoystickIsGamepad(physical_index) == GLFW_FALSE)
			return false;

		GLFWgamepadstate state;
		if (glfwGetGamepadState(physical_index, &state) == GLFW_FALSE)
			return false;

		for (size_t i = 0; i < GamepadState::BUTTON_COUNT; ++i)
			if (state.buttons[i])
				return true;

		for (size_t i = 0; i < GamepadState::AXIS_COUNT; ++i)
			if (state.axes[i] > dead_zone || state.axes[i] < -dead_zone)
				return true;

		return false;
	}

	bool GamepadManager::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		bool is_enabled = true;
		JSONTools::GetAttribute(config, "enabled", is_enabled, true);
		SetEnabled(is_enabled);

		bool is_polling_enabled = true;
		JSONTools::GetAttribute(config, "polling_enabled", is_polling_enabled, true);
		SetPollingEnabled(is_polling_enabled);

		return true;
	}

	bool GamepadManager::IsEnabled() const
	{
		return enabled;
	}

	void GamepadManager::SetEnabled(bool in_enabled)
	{
		if (enabled != in_enabled)
		{
			enabled = in_enabled;
			if (!enabled)
			{
				for (auto& gamepad : user_gamepads)
				{
					gamepad->ClearInputs();
					gamepad->ClearForceFeedbackEffects();
				}
			}
		}
	}

	void GamepadManager::SetPollingEnabled(bool in_polling_enabled)
	{
		polling_enabled = in_polling_enabled;
	}

	bool GamepadManager::IsPollingEnabled() const
	{
		return polling_enabled;
	}

}; // namespace chaos
