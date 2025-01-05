namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiContentObjectBase;

	template<typename T>
	class ImGuiContentObjectTypedBase;

	template<typename T>
	class ImGuiContentObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** 
	 * ImGuiContentObjectBase: a base class for generic content object 
	 */
	class ImGuiContentObjectBase : public ImGuiObject
	{
	public:

		/** constructor */
		ImGuiContentObjectBase(std::string in_message, ImGuiButtonType in_buttons);

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;

	protected:

		/** override */
		virtual void OnCloseWithCross() override;

		/** display the content of the widget */
		virtual void DisplayContent();

		/** call the notification function */
		virtual void CallNotification(ImGuiButtonType button_type);

		/** display all buttons */
		void DisplayButtons();

	protected:

		/** the optional message to display */
		std::string message;
		/** the buttons to display */
		ImGuiButtonType buttons;
	};

	/** 
	 * ImGuiContentObjectTypedBase: specialization of ImGuiContentObjectBase with some data within
	 * 
	 *   note: you can declare T has a reference !!! in that case the target object updates itself in realtime 
	 *   with no apply/validation required
	 */
	template<typename T>
	class ImGuiContentObjectTypedBase : public ImGuiContentObjectBase
	{
	public:

		using type = T;

		/** constructor (for non reference type) */
		ImGuiContentObjectTypedBase(
			std::string in_message,
			type in_value = {},
			std::function<void(ImGuiButtonType, type const&)> in_notification_function = {},
			ImGuiButtonType in_buttons = ImGuiButtonType::Close
		) requires !std::is_reference_v<type> :
			ImGuiContentObjectBase(std::move(in_message), in_buttons),
			value(std::move(in_value)),
			notification_function(std::move(in_notification_function))
		{}

		/** constructor (for reference type) */
		ImGuiContentObjectTypedBase(
			std::string in_message,
			type in_value,
			std::function<void(ImGuiButtonType, type const&)> in_notification_function = {},
			ImGuiButtonType in_buttons = ImGuiButtonType::Close
		) requires std::is_reference_v<type> :
			ImGuiContentObjectBase(std::move(in_message), in_buttons),
			value(in_value),
			notification_function(std::move(in_notification_function))
		{}

	protected:

		/** override */
		virtual void CallNotification(ImGuiButtonType button_type) override
		{
			if (notification_function)
				notification_function(button_type, value);
		}

		/** override */
		virtual void DisplayContent() override
		{
			if constexpr (ImGuiTools::CanDrawImGuiVariable<T>)
				DrawImGuiVariable(value);
		}

	protected:

		/** the value to display */
		type value;
		/** the notification callback */
		std::function<void(ImGuiButtonType, type const&)> notification_function;
	};

	/**
	 * ImGuiContentObjectTypedBase: specialization of ImGuiContentObjectBase with void data within
	 */
	template<>
	class ImGuiContentObjectTypedBase<void> : public ImGuiContentObjectBase
	{

	public:

		using type = void;

		/** constructor */
		ImGuiContentObjectTypedBase(
			std::string in_message,
			std::function<void(ImGuiButtonType)> in_notification_function = {},
			ImGuiButtonType in_buttons = ImGuiButtonType::Close
		) :
			ImGuiContentObjectBase(std::move(in_message), in_buttons),
			notification_function(std::move(in_notification_function))
		{
		}

	protected:

		/** override */
		virtual void CallNotification(ImGuiButtonType button_type)
		{
			if (notification_function)
				notification_function(button_type);
		}

	protected:

		/** the notification callback */
		std::function<void(ImGuiButtonType)> notification_function;
	};

	/**
	 * ImGuiContentObject: a generic ImGuiObject that can display user content, some buttons and have a notification callback
	 */
	template<typename T>
	class ImGuiContentObject : public ImGuiContentObjectTypedBase<T>
	{
	public:

		using ImGuiContentObjectTypedBase<T>::ImGuiContentObjectTypedBase;
	};

#endif

}; // namespace chaos