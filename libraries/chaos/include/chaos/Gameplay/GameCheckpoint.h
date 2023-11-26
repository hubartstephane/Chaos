namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class CheckpointObject;

	class GameCheckpoint;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =============================================
	// CheckpointObject
	// =============================================

	template<typename T>
	class CheckpointObject
	{
	public:

		/** the saving entry point */
		virtual T* SaveIntoCheckpoint() const
		{
			T* result = DoCreateCheckpoint();
			if (result == nullptr)
				return nullptr;
			if (!DoSaveIntoCheckpoint(result))
			{
				delete(result);
				return nullptr;
			}
			return result;
		}

		/** the loading entry point */
		virtual bool LoadFromCheckpoint(T const* checkpoint)
		{
			if (checkpoint == nullptr)
				return false;
			return DoLoadFromCheckpoint(checkpoint);
		}

	protected:

		/** checkpoint instanciation method */
		virtual T* DoCreateCheckpoint() const
		{
			return new T();
		}
		/** internal method for saving */
		virtual bool DoSaveIntoCheckpoint(T* checkpoint) const
		{
			return true;
		}
		/** internal method for loading */
		virtual bool DoLoadFromCheckpoint(T const* checkpoint)
		{
			return true;
		}
	};

	// =============================================
	// GameCheckpoint
	// =============================================

	class CHAOS_API GameCheckpoint : public Object
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(GameCheckpoint, Object);

	public:

		/** the player encoded into a JSON */
		nlohmann::json player_save;
		/** the game state encoded into a JSON */
		nlohmann::json level_save;

		/** the game state encoded into a JSON */
		nlohmann::json main_clock_save;
		/** the game state encoded into a JSON */
		nlohmann::json game_clock_save;
	};

#endif

}; // namespace chaos