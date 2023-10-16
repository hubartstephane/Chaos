namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiDrawableObjectRegistration;
	class ImGuiDrawableOwnerInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiDrawableObjectRegistration: an entry to all registered huds
	*/

	class CHAOS_API ImGuiDrawableObjectRegistration : public ImGuiDrawableObject
	{
		friend class ImGuiDrawableOwnerInterface;

	public:

		/** destructor */
		~ImGuiDrawableObjectRegistration();

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;

		/** increment or decrement the show counter */
		void Show(bool show = true);
		/** get the value of show counter */
		bool IsVisible() const;
		/** get the name of the registration */
		char const* GetName() const;

		/** override */
		virtual void SubReference() override;

	protected:

		/** the name of the drawable */
		std::string name;
		/** the cached drawable object */
		chaos::shared_ptr<ImGuiDrawableObject> drawable_object;
		/** function to create the element */
		std::function<ImGuiDrawableObject* ()> creation_function;
		/** the show count */
		bool visible = false;
		/** owner */
		ImGuiDrawableOwnerInterface* owner = nullptr;
	};

	/**
	* ImGuiDrawableOwnerInterface: an entry to all registered huds
	*/

	class CHAOS_API ImGuiDrawableOwnerInterface : public ImGuiDrawableInterface
	{
	public:

		/** destructor */
		~ImGuiDrawableOwnerInterface();

		/** find a registered object */
		ImGuiDrawableObjectRegistration* FindRegisteredDrawable(char const* name);

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;

		/** insert a drawable */
		ImGuiDrawableObjectRegistration * RegisterDrawable(char const* name, std::function<ImGuiDrawableObject * ()> drawable_creation_function);
		/** insert a drawable */
		ImGuiDrawableObjectRegistration * RegisterDrawable(char const* name, SubClassOf<ImGuiDrawableObject> drawable_class);

		/** remove a registered object */
		void UnregisterDrawable(ImGuiDrawableObjectRegistration* registration);

	protected:

		/** the drawables registered by this owner */
		std::vector<shared_ptr<ImGuiDrawableObjectRegistration>> registered_drawable;
	};

#endif

}; // namespace chaos