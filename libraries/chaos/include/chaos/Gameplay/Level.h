#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class Level;

}; // namespace chaos

#else

namespace chaos
{
	// =====================================
	// Level : a simple game level
	// =====================================

	class Level : public chaos::Object, public chaos::FileResource, public chaos::NamedObject
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(Level, chaos::Object);

	public:

		/** create a level instance for that level */
		LevelInstance * CreateLevelInstance(Game * in_game);

		/** returns the level timeout */
		float GetLevelTimeout() const { return level_timeout; }
		/** returns the title for that level */
		std::string const & GetLevelTitle() const { return level_title; }
		/** the index of that level */
		int GetLevelIndex() const { return level_index; }

	protected:

		/** create a level instance for that level user specified function */
		virtual class LevelInstance * DoCreateLevelInstance();

	protected:

		/** get the level timeout */
		float level_timeout = -1.0f;
		/** the title of the level*/
		std::string level_title;
		/** the index of the level (maybe a key or an ordered index) */
		int level_index = 0;

		/** the level instance class */
		chaos::SubClassOf<LevelInstance> level_instance_class;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION