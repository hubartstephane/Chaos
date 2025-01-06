namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiChoiceObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiChoiceObject: a ImGuiObject used to display some choices. Usefull for a popup menu
	 */

	class ImGuiChoiceObject : public ImGuiObject
	{
	public:

		/** constructor */
		ImGuiChoiceObject(char const** in_choices, std::function<void(size_t)> in_choice_function);

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;

	protected:

		/** the choices to display */
		std::vector<std::string> choices;
		/** callback called whenever a choice has been made */
		std::function<void(size_t)> choice_function;

	};

#endif

}; // namespace chaos