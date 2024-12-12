namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class PopupState;
	enum class PopupPlacementType;

	class PopupPlacement;

	namespace details
	{
		class ImGuiPopupBase;
	};

	template<typename T>
	class ImGuiPopup;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the current state of the popup */
	enum class PopupState
	{
		Closed,
		Opening,
		Opened
	};

	/** the reference point for placing a imgui popup */
	enum class PopupPlacementType
	{
		ScreenCenter,
		CursorPosition,
		FreePosition
	};

	/** how to place a imgui popup */
	class PopupPlacement
	{
	public:

		/** get an instance for a popup centered on screen */
		static PopupPlacement GetCenterOnScreenPlacement(bool in_movable = true, const glm::vec2& in_alignment = { 0.5f, 0.5f });
		/** get an instance for a popup placed on cursor position */
		static PopupPlacement GetCenterOnCursorPlacement(bool in_movable = true, const glm::vec2& in_alignment = { 0.0f, 0.0f });
		/** get an instance for a popup at a given position */
		static PopupPlacement GetCenterOnPositionPlacement(const glm::vec2& in_position, bool in_movable = true, const glm::vec2& in_alignment = { 0.5f, 0.5f });

	public:

		/** how to place the popup */
		PopupPlacementType placement_type = PopupPlacementType::ScreenCenter;
		/** whether the popup can be moved */
		bool movable = true;
		/** the position of the popup */
		glm::vec2 position = { 0.0f, 0.0f };
		/** the alignment of the popup */
		glm::vec2 alignment = { 0.5f, 0.5f };
	};

	/** base class for a popup */
	namespace details
	{
		class ImGuiPopupBase
		{
		public:

			/** get the current state of the popup */
			PopupState GetPopupState() const;

			/** end popup lifetime */
			void Close();

		protected:

			/** get the window flags */
			virtual int GetPopupFlags() const;

			/** internal method called to open the popup. Derived class has to implement a public Open method that initialize the popup and calls DoOpen */
			bool DoOpen(std::string in_popup_name, const PopupPlacement& in_placement);
			/** prepare imgui popup placement */
			void PrepareModalPlacement();

		protected:

			/** the title of the popup */
			std::string popup_name;
			/** the state of the popup */
			PopupState popup_state = PopupState::Closed;
			/** placement behaviour of the popup */
			PopupPlacement popup_placement;
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
			if (popup_state == PopupState::Closed)
			{
				if constexpr (!std::is_same_v<void, T>)
					return {};
				else
					return;
			}
			// set the window placement
			PrepareModalPlacement();

			// do open the imgui popup
			int flags = GetPopupFlags();
			if (!popup_placement.movable)
				flags |= ImGuiWindowFlags_NoMove;

			if (ImGui::BeginPopupModal(popup_name.c_str(), NULL, flags))
			{
				if constexpr (std::is_same_v<void, T>)
				{
					DoProcess();
					ImGui::EndPopup();
				}
				else
				{
					RESULT_TYPE result = DoProcess();
					ImGui::EndPopup();
					return result;
				}
			}

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