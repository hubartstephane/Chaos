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

	Gamepad::Gamepad(GamepadManager* in_gamepad_manager, PhysicalGamepad* in_physical_gamepad, GamepadInputFilterSettings const* in_filter_settings, GamepadCallbacks* in_callbacks) :
		gamepad_manager(in_gamepad_manager),
		physical_gamepad(in_physical_gamepad),
		filter_settings(*in_filter_settings),
		callbacks(in_callbacks)
	{
		assert(in_gamepad_manager != nullptr);
		assert(in_filter_settings != nullptr);
	}

	Gamepad::~Gamepad()
	{
		RemoveFromSuperviser();
	}

	void Gamepad::RemoveFromSuperviser()
	{
		if (gamepad_manager == nullptr)
			return;
		gamepad_manager->RemoveGamepad(this);
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
#if _WIN64
		if (!IsPresent())
			return;
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

		size_t gamepad_index = GetGamepadIndex();
		GamepadManager::XInputSetStateFunc(DWORD(gamepad_index), &vibration);

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

		if (index != feedback_effects.size() - 1)
			std::swap(feedback_effects[index], feedback_effects.back()); // proper way to handle the last reference
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

}; // namespace chaos
