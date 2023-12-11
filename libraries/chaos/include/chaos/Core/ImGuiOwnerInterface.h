namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiRegistrationObject;
	class ImGuiOwnerInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiRegistrationObject: an entry to all registered huds
	*/

	class CHAOS_API ImGuiRegistrationObject : public ImGuiObject
	{
		friend class ImGuiOwnerInterface;

	public:

		/** destructor */
		~ImGuiRegistrationObject();

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
		chaos::shared_ptr<ImGuiObject> drawable_object;
		/** function to create the element */
		std::function<ImGuiObject* ()> creation_function;
		/** the show count */
		bool visible = false;
		/** owner */
		ImGuiOwnerInterface* owner = nullptr;
	};

	/**
	* ImGuiOwnerInterface: an entry to all registered huds
	*/

	class CHAOS_API ImGuiOwnerInterface : public ImGuiInterface
	{
	public:

		/** destructor */
		~ImGuiOwnerInterface();

		/** find a registered object */
		ImGuiRegistrationObject* FindRegisteredDrawable(char const* name);

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;

		/** insert a drawable */
		ImGuiRegistrationObject * RegisterDrawable(char const* name, std::function<ImGuiObject * ()> const& drawable_creation_function);
		/** insert a drawable */
		ImGuiRegistrationObject * RegisterDrawable(char const* name, SubClassOf<ImGuiObject> drawable_class);

		/** remove a registered object */
		void UnregisterDrawable(ImGuiRegistrationObject* registration);

	protected:

		/** the drawables registered by this owner */
		std::vector<shared_ptr<ImGuiRegistrationObject>> registered_drawable;
	};

#endif

}; // namespace chaos