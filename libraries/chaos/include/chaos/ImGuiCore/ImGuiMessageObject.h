namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiMessageObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiMessageObject: an ImGuiObject to just display a text
	 */

	CHAOS_API class ImGuiMessageObject : public ImGuiObject
	{
	public:

		/** constructor */
		ImGuiMessageObject(std::string in_message);

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;

	protected:

		/** the message to display */
		std::string message;
	};

#endif

}; // namespace chaos