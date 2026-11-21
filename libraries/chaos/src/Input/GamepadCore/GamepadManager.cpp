#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
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
		for (size_t i = 0; i < CHAOS_SUPPORTED_GAMEPAD_COUNT; ++i)
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

		if (index != user_gamepads.size() - 1)
			std::swap(user_gamepads[index], user_gamepads.back()); // proper way to handle the last reference
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

	size_t GamepadManager::GetPresentPhysicalGamepadCount() const
	{
		return size_t(BitTools::popcount(present_physical_gamepads));
	}

	bool GamepadManager::ForEachPresentPhysicalGamepad(LightweightFunction<bool(PhysicalGamepad const &)> in_func) const
	{	
		return BitTools::ForEachBitForward(present_physical_gamepads, [&](uint32_t index)
		{
			return in_func(GetPhysicalGamepad(index));
		});
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
