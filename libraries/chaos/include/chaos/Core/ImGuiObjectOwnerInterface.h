namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiObjectProxy;
	class ImGuiObjectOwnerInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiObjectProxy: a proxy for an ImGuiObject (responsible for creation/destruction)
	*/

	class CHAOS_API ImGuiObjectProxy : public Object
	{
		friend class ImGuiObjectOwnerInterface;

	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiObjectProxy, Object);

		/** destructor */
		~ImGuiObjectProxy();

		/** display the owned ImGuiObject */
		virtual void DrawImGui();

		/** change visibility */
		void Show(bool show = true);
		/** get visibility */
		bool IsVisible() const;
		/** get the name */
		char const* GetName() const;

		/** request to close this interface */
		void RequestClosing();
		/** check whether closing is requested */
		bool IsClosingRequested() const;

		/** override */
		virtual void SubReference() override;

	protected:

		/** owner */
		ImGuiObjectOwnerInterface* owner = nullptr;
		/** the name of the drawable */
		std::string name;
		/** the cached imGui object */
		chaos::shared_ptr<ImGuiObject> imgui_object;
		/** function to create the ImGui object */
		std::function<ImGuiObject* ()> imgui_object_create_function;
		/** the show count */
		bool visible = false;
		/** indicates whether closing this UI is requested */
		bool closing_request = false;
	};

	/**
	* ImGuiObjectOwnerInterface: an interface for an object owning some ImGuiObject's
	*/

	class CHAOS_API ImGuiObjectOwnerInterface
	{
	public:

		/** destructor */
		~ImGuiObjectOwnerInterface();

		/** draw all ImGui objects */
		virtual void DrawImGuiObjects();
		/** draw all ImGui objects menu entries */
		virtual void DrawImGuiObjectsMenu(ImGuiInterface::DrawImGuiMenuFunc func);

		/** find a registered object */
		ImGuiObjectProxy* FindImGuiObjectProxy(char const* name);

		/** insert a drawable */
		ImGuiObjectProxy * RegisterImGuiObjectProxy(char const* name, std::function<ImGuiObject * ()> imgui_object_create_function);
		/** insert a drawable */
		ImGuiObjectProxy * RegisterImGuiObjectProxy(char const* name, SubClassOf<ImGuiObject> drawable_class);

		/** remove a registered object */
		void UnregisterImGuiObjectProxy(ImGuiObjectProxy* proxy);

	protected:

		/** the proxies registered by this owner */
		std::vector<shared_ptr<ImGuiObjectProxy>> proxies;
	};

#endif

}; // namespace chaos