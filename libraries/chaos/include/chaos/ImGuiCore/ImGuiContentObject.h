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
	 */
	template<typename T>
	class ImGuiContentObjectTypedBase : public ImGuiContentObjectBase
	{
	public:

		/** constructor */
		ImGuiContentObjectTypedBase(
			std::string in_message,
			T in_value = {},
			std::function<void(ImGuiButtonType, T const&)> in_notification_function = {},
			ImGuiButtonType in_buttons = ImGuiButtonType::Close
		) :
			ImGuiContentObjectBase(std::move(in_message), in_buttons),
			value(std::move(in_value)),
			notification_function(std::move(in_notification_function)) {}

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
		T value;
		/** the notification callback */
		std::function<void(ImGuiButtonType, T const&)> notification_function;
	};

	/**
	 * ImGuiContentObjectTypedBase: specialization of ImGuiContentObjectBase with void data within
	 */
	template<>
	class ImGuiContentObjectTypedBase<void> : public ImGuiContentObjectBase
	{

	public:

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