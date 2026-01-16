namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ModifiersInputCondition;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ModifiersInputCondition: a class that checks for ALT, SHIFT and CONTROL state
	 */

	class ModifiersInputCondition : public InputConditionBase
	{
	public:
		
		/** constructor */
		ModifiersInputCondition(KeyModifier in_modifiers, bool in_wanted_value):
			modifiers(in_modifiers),
			wanted_value(in_wanted_value)
		{
		}

		/** copy constructor */
		ModifiersInputCondition(ModifiersInputCondition const& src) = default;

		/** override */
		virtual InputConditionResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override;

		/** override */
		virtual char const* GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const override;

	protected:

		/** check whether one of the 2 keys is down */
		bool GetModifierKeyPairValue(InputDeviceInterface const* in_input_device, Key key1, Key key2) const;

	protected:

		/** the modifiers to search for */
		KeyModifier modifiers = KeyModifier::None;
		/** the wanted value for the modifiers */
		bool wanted_value = false;
	};

	/**
	* Some request getters
	**/

	template<InputRequestType REQUEST_TYPE> 
	auto RequireModifiers(KeyModifier in_modifiers, REQUEST_TYPE const& in_condition)
	{
		return And(in_condition, ModifiersInputCondition(in_modifiers, true));
	}

	template<InputRequestType REQUEST_TYPE>
	auto RequireModifiers(KeyModifier in_modifiers, REQUEST_TYPE && in_condition)
	{
		return And(std::move(in_condition), ModifiersInputCondition(in_modifiers, true));
	}

	template<InputRequestType REQUEST_TYPE>
	auto ForbidModifiers(KeyModifier in_modifiers, REQUEST_TYPE const& in_condition)
	{
		return And(in_condition, ModifiersInputCondition(in_modifiers, false));
	}

	template<InputRequestType REQUEST_TYPE>
	auto ForbidModifiers(KeyModifier in_modifiers, REQUEST_TYPE&& in_condition)
	{
		return And(std::move(in_condition), ModifiersInputCondition(in_modifiers, false));
	}

#endif

}; // namespace chaos