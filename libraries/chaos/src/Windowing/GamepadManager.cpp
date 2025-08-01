#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// PhysicalGamepad functions
	//

	PhysicalGamepad::PhysicalGamepad(GamepadManager* in_gamepad_manager, int in_stick_index) :
		gamepad_manager(in_gamepad_manager),
		stick_index(in_stick_index)
	{
		assert(in_gamepad_manager != nullptr);
	}

	void PhysicalGamepad::ClearInputs()
	{
		if (!IsPresent())
			return;
		gamepad_state.Clear();
	}

	bool PhysicalGamepad::EnumerateDeviceHierarchy(LightweightFunction<bool(InputDeviceUserInterface const*)> func) const
	{
		if (IsPresent())
			if (gamepad_state.EnumerateDeviceHierarchy(func))
				return true;
		return InputDeviceUserInterface::EnumerateDeviceHierarchy(func);
	}

	void PhysicalGamepad::UpdateAxisAndButtons(float dead_zone)
	{
		if (!IsPresent())
			return;
		gamepad_state.UpdateAxisAndButtons(stick_index, dead_zone);
	}

	Gamepad* PhysicalGamepad::CaptureDevice(GamepadCallbacks* in_callbacks)
	{
		return gamepad_manager->DoCaptureDevice(this, in_callbacks);
	}

	//
	// ForceFeedbackEffect functions
	//

	void ForceFeedbackEffect::SubReference()
	{
		// the ParticleAllocation is handled as usual
		if (gamepad == nullptr)
			Object::SubReference();
		// the last reference is the one from the layer. Destroy it
		else if (--shared_count == 1)
			RemoveFromGamepad();
	}

	bool ForceFeedbackEffect::GetForceFeedbackValues(float delta_time, float& result_left_value, float& result_right_value)
	{
		result_left_value = 0.0f;
		result_right_value = 0.0f;
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

	bool DefaultForceFeedbackEffect::GetForceFeedbackValues(float delta_time, float& result_left_value, float& result_right_value)
	{
		timer += delta_time;
		if (timer >= duration)
			return true; // end of the effect
		result_left_value = left_value;
		result_right_value = right_value;
		return false; // do not destroy the effect
	}

	//
	// Gamepad functions
	//

	Gamepad::Gamepad(GamepadManager* in_gamepad_manager, PhysicalGamepad* in_physical_device) :
		gamepad_manager(in_gamepad_manager),
		physical_device(in_physical_device)
	{
		assert(gamepad_manager != nullptr);
		if (physical_device != nullptr)
		{
			assert(physical_device->IsPresent());
			physical_device->user_gamepad = this;
			ever_connected = true;
		}
	}

	Gamepad::~Gamepad()
	{
		if (gamepad_manager != nullptr)
			gamepad_manager->OnGamepadDestroyed(this);
		if (physical_device != nullptr)
			physical_device->user_gamepad = nullptr;
	}

	bool Gamepad::EnumerateDeviceHierarchy(LightweightFunction<bool(InputDeviceUserInterface const*)> func) const
	{
		if (physical_device != nullptr)
			if (physical_device->EnumerateDeviceHierarchy(func))
				return true;
		return InputDeviceUserInterface::EnumerateDeviceHierarchy(func);
	}

	bool Gamepad::IsPresent() const
	{
		if (physical_device == nullptr)
			return false;
		return physical_device->IsPresent();
	}

	int Gamepad::GetGamepadIndex() const
	{
		if (physical_device == nullptr)
			return -1;
		return physical_device->GetGamepadIndex();
	}

	void Gamepad::SetCallbacks(GamepadCallbacks* in_callbacks)
	{
		callbacks = in_callbacks;
	}

	GamepadState const* Gamepad::GetGamepadState() const
	{
		if (physical_device == nullptr)
			return nullptr;
		return physical_device->GetGamepadState();
	}

	void Gamepad::ClearForceFeedbackEffects()
	{
		if (feedback_effects.size() > 0)
		{
			feedback_effects.clear();
			TickForceFeedbackEffects(0.0f);
		}
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

	namespace GlobalVariables
	{
#if _DEBUG
		CHAOS_GLOBAL_VARIABLE(bool, NoForceFeedback, false);
#endif
	};

	void Gamepad::DoUpdateForceFeedbackDevice(float max_left_value, float max_right_value)
	{
#if _DEBUG
		if (GlobalVariables::NoForceFeedback.Get())
		{
			max_left_value = 0.0f;
			max_right_value = 0.0f;
		}
#endif
		if (!IsPresent())
			return;
#if _WIN64
		if (GamepadManager::XInputSetStateFunc == nullptr)
			return;

		max_left_value = std::clamp(max_left_value, 0.0f, 1.0f);
		max_right_value = std::clamp(max_right_value, 0.0f, 1.0f);

		XINPUT_VIBRATION vibration;
		vibration.wLeftMotorSpeed = (WORD)(max_left_value * 65535.0f);
		vibration.wRightMotorSpeed = (WORD)(max_right_value * 65535.0f);
		GamepadManager::XInputSetStateFunc(GetGamepadIndex(), &vibration);
#endif
	}

	void Gamepad::TickForceFeedbackEffects(float delta_time)
	{
		// compute the max left & right values, destroy finished values
		float max_left_value = 0.0f;
		float max_right_value = 0.0f;

		if (!force_feedback_paused)
		{
			size_t count = feedback_effects.size();
			for (size_t i = count; i > 0; --i)
			{
				size_t index = i - 1;

				ForceFeedbackEffect* effect = feedback_effects[index].get();
				if (effect == nullptr)
					continue;

				float left_value = 0.0f;
				float right_value = 0.0f;
				if (effect->GetForceFeedbackValues(delta_time, left_value, right_value))
				{
					feedback_effects.erase(feedback_effects.begin() + index);
				}
				else
				{
					max_left_value = std::max(max_left_value, left_value);
					max_right_value = std::max(max_right_value, right_value);
				}
			}
		}
		// reduce feedback to 0 (do not skip GetForceFeedbackValues(...) calls so the effects tick themselves even if muted
		if (ShouldReduceForceFeedbackToZero())
		{
			max_left_value = 0.0f;
			max_right_value = 0.0f;
		}
		// update the device
		DoUpdateForceFeedbackDevice(max_left_value, max_right_value);
	}

	void Gamepad::AddForceFeedbackEffect(ForceFeedbackEffect* effect)
	{
		assert(effect != nullptr);
		assert(effect->gamepad == nullptr);
		effect->gamepad = this;
		feedback_effects.push_back(effect);
	}

	void Gamepad::RemoveForceFeedbackEffect(ForceFeedbackEffect* effect)
	{
		assert(effect != nullptr);
		assert(effect->gamepad == this);

		for (size_t i = feedback_effects.size(); i > 0; --i) // from end to beginning because ... maybe a future DetachAllFeedback(...) function will be from end to beginning
		{
			size_t index = i - 1;
			if (feedback_effects[index] == effect)
			{
				feedback_effects.erase(feedback_effects.begin() + index);
				return;
			}
		}
	}


	//
	// GamepadManager functions
	//

#if _WIN64
	GamepadManager::XINPUT_SET_STATE_FUNC GamepadManager::XInputSetStateFunc = nullptr;
#endif

	GamepadManager::GamepadManager(float in_dead_zone) : dead_zone(in_dead_zone)
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
		// allocate a PhysicalGamepad for all supported inputs
		physical_gamepads.reserve(MAX_SUPPORTED_GAMEPAD_COUNT);
		for (int i = 0; i < MAX_SUPPORTED_GAMEPAD_COUNT; ++i)
		{
			PhysicalGamepad* physical_gamepad = new PhysicalGamepad(this, i);
			if (physical_gamepad != nullptr)
			{
				physical_gamepad->is_present = (glfwJoystickPresent(i) > 0);
				if (physical_gamepad->is_present)
					physical_gamepad->UpdateAxisAndButtons(dead_zone);
			}
			physical_gamepads.push_back(physical_gamepad);
		}
	}

	GamepadManager::~GamepadManager()
	{
		for (PhysicalGamepad* physical_gamepad : physical_gamepads) // destroy all physical gamepads
			if (physical_gamepad != nullptr)
				delete(physical_gamepad);
		physical_gamepads.clear();

		for (Gamepad* gamepad : user_gamepads) // notify all user gamepads about the destruction
		{
			if (gamepad != nullptr)
			{
				gamepad->gamepad_manager = nullptr;
				gamepad->physical_device = nullptr;
				if (gamepad->callbacks != nullptr)
					gamepad->callbacks->OnManagerDestroyed(gamepad);
			}
		}
		user_gamepads.clear();
	}

	void GamepadManager::EnableInputPooling(bool in_pooling_enabled)
	{
		pooling_enabled = in_pooling_enabled;
	}

	bool GamepadManager::IsInputPoolingEnabled() const
	{
		return pooling_enabled;
	}

	bool GamepadManager::OnGamepadDestroyed(Gamepad* gamepad)
	{
		assert(gamepad != nullptr);

		// remove all force feedback effect that could remains on physical device after its destruction
		gamepad->ClearForceFeedbackEffects();
		// remove the gamepad for list
		size_t count = user_gamepads.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (user_gamepads[i] == gamepad) // remove the gamepad from the list
			{
				if (i != count - 1)
					user_gamepads[i] = user_gamepads[count - 1]; // remove swap
				user_gamepads.pop_back();
				break;
			}
		}
		return true;
	}

	// user explicitly require a physical gamepad
	PhysicalGamepad* GamepadManager::FindUnallocatedPhysicalGamepad(GamepadCallbacks* in_callbacks)
	{
		PhysicalGamepad* best_physical_gamepad = nullptr;

		size_t count = physical_gamepads.size();
		for (size_t i = 0; i < count; ++i)
		{
			PhysicalGamepad* physical_gamepad = physical_gamepads[i];
			if (physical_gamepad == nullptr)
				continue;
			if (!physical_gamepad->IsPresent())
				continue;
			if (physical_gamepad->IsAllocated()) // we want an entry that is owned by nobody
				continue;
			if (in_callbacks != nullptr && !in_callbacks->AcceptPhysicalDevice(physical_gamepad)) // user filter
				continue;

			// the 1st best is to find a physical PRESENT gamepad and that has any input
			// the 2nd best is to find a physical PRESENT gamepad (even if no input)

			if (physical_gamepad->IsAnyAction())
				return physical_gamepad; // no better choice can be expected => immediate returns

			if (best_physical_gamepad == nullptr)
				best_physical_gamepad = physical_gamepad; // keep the first in allocation order that fits (XXX : not really allocation order due to remove swap)
		}
		return best_physical_gamepad;
	}

	Gamepad* GamepadManager::AllocateGamepad(bool want_present, GamepadCallbacks* in_callbacks) // user explicitly require a gamepad
	{
		PhysicalGamepad* physical_gamepad = FindUnallocatedPhysicalGamepad(in_callbacks);
		if (want_present && physical_gamepad == nullptr) // all physical device in use or not present ?
			return nullptr;

		return DoAllocateGamepad(physical_gamepad, in_callbacks);
	}

	Gamepad* GamepadManager::DoAllocateGamepad(PhysicalGamepad* physical_gamepad, GamepadCallbacks* in_callbacks)
	{
		Gamepad* result = new Gamepad(this, physical_gamepad);
		if (result != nullptr)
		{
			user_gamepads.push_back(result);
			if (in_callbacks != nullptr)
			{
				result->SetCallbacks(in_callbacks);
				if (physical_gamepad != nullptr)
					result->callbacks->OnGamepadConnected(result);
			}
		}
		return result;
	}

	void GamepadManager::Tick(float delta_time)
	{
		// update physical stick state / handle disconnection
		int unallocated_present_physical_device_count = 0;
		UpdateAndUnconnectPhysicalGamepads(unallocated_present_physical_device_count); // get the number of physical devices to bind

		// try to give all logical device a physical device
		if (unallocated_present_physical_device_count > 0)
			GiveGamepadPhysicalDevices(unallocated_present_physical_device_count);

		// uncatched inputs pooling
		if (unallocated_present_physical_device_count > 0 && pooling_enabled)
			PoolInputs(unallocated_present_physical_device_count);

		// tick the force feedback effects
		TickForceFeedbackEffects(delta_time);
	}

	void GamepadManager::PoolInputs(int& unallocated_present_physical_device_count)
	{
		size_t count = physical_gamepads.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (unallocated_present_physical_device_count == 0) // early exit
				return;

			PhysicalGamepad* physical_gamepad = physical_gamepads[i];
			if (physical_gamepad == nullptr)
				continue;
			if (!physical_gamepad->IsPresent())
				continue;
			if (physical_gamepad->IsAllocated())
				continue;
			if (!physical_gamepad->IsAnyAction())
				continue;

			--unallocated_present_physical_device_count;
			if (!DoPoolGamepad(physical_gamepad))
				break;
		}
	}

	void GamepadManager::TickForceFeedbackEffects(float delta_time)
	{
		size_t count = user_gamepads.size();
		for (size_t i = 0; i < count; ++i)
		{
			Gamepad* gamepad = user_gamepads[i];
			if (gamepad == nullptr)
				continue;
			if (!gamepad->IsPresent())
				continue;
			gamepad->TickForceFeedbackEffects(delta_time);
		}
	}

	bool GamepadManager::DoPoolGamepad(PhysicalGamepad* physical_gamepad)
	{
		return true;
	}

	void GamepadManager::UpdateAndUnconnectPhysicalGamepads(int& unallocated_present_physical_device_count)
	{
		size_t count = physical_gamepads.size();
		for (size_t i = 0; i < count; ++i)
		{
			PhysicalGamepad* physical_gamepad = physical_gamepads[i];
			if (physical_gamepad == nullptr)
				continue;

			bool is_present = (glfwJoystickPresent(int(i)) > 0);
			bool was_present = physical_gamepad->IsPresent();

			physical_gamepad->is_present = is_present; // update presence flag

			if (is_present)
			{
				physical_gamepad->UpdateAxisAndButtons(dead_zone);

				Gamepad* user_gamepad = physical_gamepad->user_gamepad;
				if (user_gamepad != nullptr && user_gamepad->callbacks != nullptr)
					user_gamepad->callbacks->OnGamepadStateUpdated(physical_gamepad->gamepad_state); // notify the user of the update

				if (!physical_gamepad->IsAllocated())
					++unallocated_present_physical_device_count; // count the number of physical device not owned by a logical device
			}
			else if (was_present)
			{
				physical_gamepad->ClearInputs();

				Gamepad* user_gamepad = physical_gamepad->user_gamepad;
				if (user_gamepad != nullptr)
				{
					user_gamepad->physical_device = nullptr; // unbind physical and logical device
					physical_gamepad->user_gamepad = nullptr;
					if (user_gamepad->callbacks != nullptr)
						user_gamepad->callbacks->OnGamepadDisconnected(user_gamepad);
				}
			}
		}
	}

	void GamepadManager::GiveGamepadPhysicalDevices(int& unallocated_present_physical_device_count)
	{
		size_t count = physical_gamepads.size();

		for (int step = 0; step < 2; ++step) // 2 steps algorithm : 1 - physical device with any inputs  2 - any physical device (even without inputs)
		{
			for (size_t i = 0; i < count; ++i)
			{
				if (unallocated_present_physical_device_count == 0) // no more present physical device ?
					return;

				PhysicalGamepad* physical_gamepad = physical_gamepads[i];
				if (physical_gamepad == nullptr)
					continue;
				if (!physical_gamepad->IsPresent())
					continue;
				if (physical_gamepad->IsAllocated()) // want unbound physical device
					continue;

				if (step == 0 && !physical_gamepad->IsAnyAction()) // in step 0, ignore sticks that have no inputs
					continue;

				if (!DoGiveGamepadPhysicalDevice(physical_gamepad)) // returns false if no logical gamepad wants a physical device
					return;

				--unallocated_present_physical_device_count;
			}
		}
	}

	Gamepad* GamepadManager::FindBestGamepadToBeBoundToPhysicalDevice(PhysicalGamepad* physical_gamepad)
	{
		Gamepad* best_gamepad = nullptr;

		size_t count = user_gamepads.size();
		for (size_t i = 0; i < count; ++i)
		{
			Gamepad* gamepad = user_gamepads[i];
			if (gamepad == nullptr)
				continue;
			if (gamepad->physical_device != nullptr) // logical device does not need to be bound to a physical device
				continue;
			if (gamepad->callbacks != nullptr && !gamepad->callbacks->AcceptPhysicalDevice(physical_gamepad)) // user filter
				continue;
			if (gamepad->IsEverConnected()) // the best is to use a gamepad that has already received a physical device (more likely to have been really used)
				return gamepad;
			if (best_gamepad == nullptr)
				best_gamepad = gamepad; // in a second step, we will return any gamepad (by order of allocation)
		}
		return best_gamepad;
	}

	bool GamepadManager::DoGiveGamepadPhysicalDevice(PhysicalGamepad* physical_gamepad)
	{
		Gamepad* best_gamepad = FindBestGamepadToBeBoundToPhysicalDevice(physical_gamepad);
		if (best_gamepad == nullptr)
			return false;               // no orphan logical device

		physical_gamepad->user_gamepad = best_gamepad;
		best_gamepad->physical_device = physical_gamepad;
		best_gamepad->ever_connected = true;

		if (best_gamepad->callbacks != nullptr)
			best_gamepad->callbacks->OnGamepadConnected(best_gamepad);

		return true;
	}

	bool GamepadManager::HasAnyInputs(int stick_index, float dead_zone)
	{
		if (glfwJoystickPresent(stick_index)) // ensure any input is triggered
		{
			int buttons_count = 0;
			int axis_count = 0;

			float const* axis_buffer = glfwGetJoystickAxes(stick_index, &axis_count);
			if (axis_buffer != nullptr)
				for (size_t i = 0; i < (size_t)axis_count; ++i)
					if (axis_buffer[i] > dead_zone || axis_buffer[i] < -dead_zone) // is axis valid
						return true;

			unsigned char const* buttons_buffer = glfwGetJoystickButtons(stick_index, &buttons_count);
			if (buttons_buffer != nullptr)
				for (size_t i = 0; i < (size_t)buttons_count; ++i)
					if (buttons_buffer[i]) // is button pressed ?
						return true;
		}
		return false;
	}

	Gamepad* GamepadManager::DoCaptureDevice(PhysicalGamepad* in_physical_gamepad, GamepadCallbacks* in_callbacks)
	{
		// the physical device is not present
		if (!in_physical_gamepad->IsPresent())
			return nullptr;
		// the device is already allocated
		if (in_physical_gamepad->user_gamepad != nullptr)
			return nullptr;

		return DoAllocateGamepad(in_physical_gamepad, in_callbacks);
	}

}; // namespace chaos
