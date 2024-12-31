namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImGuiPopupState;
	enum class ImGuiPopupFlags;

	namespace details
	{
		class ImGuiPopupBase;
	};

	template<typename T>
	class ImGuiPopup;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the current state of the popup */
	enum class ImGuiPopupState
	{
		Closed,
		Opening,
		Opened
	};

	/** some flags concerning the popup */
	enum class ImGuiPopupFlags
	{
		Modal = 1,
		NonModal = 2,
		CloseWithCross = 4, // modal only
		CloseWithEscape = 8 // modal only
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(ImGuiPopupFlags, CHAOS_API)

	/** base class for a popup */
	namespace details
	{
		class ImGuiPopupBase
		{
		public:

			/** get the current state of the popup */
			ImGuiPopupState GetPopupState() const;
			
			/** get the window flags */
			int GetPopupFlags() const;
			/** get the window flags */
			void SetPopupFlags(int in_flags);

			/** end popup lifetime */
			void Close();

		protected:


			/** internal method called to open the popup. Derived class has to implement a public Open method that initialize the popup and calls DoOpen */
			bool DoOpen(std::string in_popup_name, ImGuiPopupFlags in_popup_flags, const ImGuiWindowPlacement& in_placement);

		protected:

			/** the title of the popup */
			std::string popup_name;
			/** the state of the popup */
			ImGuiPopupState popup_state = ImGuiPopupState::Closed;
			/** placement behaviour of the popup */
			ImGuiWindowPlacement popup_placement;
			/** the window flags */
			int imgui_flags = ImGuiWindowFlags_AlwaysAutoResize;
			/** the popup flags */
			ImGuiPopupFlags popup_flags = ImGuiPopupFlags::Modal;
		};

	}; // namespace details

	/** base class for a popup */
	template<typename T>
	class ImGuiPopup : public details::ImGuiPopupBase
	{
	public:

		/** the return type of the popup */
		using RESULT_TYPE = std::conditional_t<
			std::is_same_v<T, void>,
			void,
			std::optional<T>
		>;

		/** entry point of the window loop */
		virtual RESULT_TYPE Process()
		{
			// early exit
			if (popup_state == ImGuiPopupState::Closed)
			{
				if constexpr (!std::is_same_v<void, T>)
					return {};
				else
					return;
			}
			// set the window placement
			if (popup_state == ImGuiPopupState::Opening)
			{
				ImGui::OpenPopup(popup_name.c_str(), ImGuiPopupFlags_None);
				popup_placement.PrepareNextWindowPlacement();
				popup_state = ImGuiPopupState::Opened;
			}

			// open the popup
			bool  opened = true;
			bool* opened_ptr = (HasAnyFlags(popup_flags, ImGuiPopupFlags::CloseWithCross)) ? &opened : nullptr;

			bool open_result = (HasAnyFlags(popup_flags, ImGuiPopupFlags::NonModal)) ?
				ImGui::BeginPopup(popup_name.c_str(), imgui_flags) :
				ImGui::BeginPopupModal(popup_name.c_str(), opened_ptr, imgui_flags); // modal by default

			if (open_result)
			{
				if constexpr (std::is_same_v<void, T>)
				{
					DoProcess();

					if (HasAnyFlags(popup_flags, ImGuiPopupFlags::CloseWithEscape))
						if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape))
							Close();

					ImGui::EndPopup();
				}
				else
				{
					RESULT_TYPE result = DoProcess();

					if (HasAnyFlags(popup_flags, ImGuiPopupFlags::CloseWithEscape))
						if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape))
							Close();

					ImGui::EndPopup();					
					return result;
				}
			}

			// reset the flag if popup has been closed somehow
			if (!ImGui::IsPopupOpen(popup_name.c_str()))
				popup_state = ImGuiPopupState::Closed;

			if constexpr (!std::is_same_v<void, T>)
				return {};
		}

	protected:

		/** this method is to be override. It describes the popup content and a return value when the popup lifetime is over */
		virtual RESULT_TYPE DoProcess()
		{
			if constexpr (!std::is_same_v<T, void>)
				return {};
		}
	};

#endif

}; // namespace chaos