namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiYesNoObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiYesNoObject: an ImGuiObject to ask for a question
	 */

	CHAOS_API class ImGuiYesNoObject : public ImGuiObject
	{
	public:

		/** constructor */
		ImGuiYesNoObject(std::string in_message, std::function<void(bool)> in_response_function);

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;

	protected:

		/** the message to display */
		std::string message;
		/** the callback to call once response is made */
		std::function<void(bool)> response_function;
	};

#endif

}; // namespace chaos