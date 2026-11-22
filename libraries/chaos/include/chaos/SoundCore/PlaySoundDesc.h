namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class PlaySoundDesc;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API PlaySoundDesc
	{
	public:

		/** returns whether the sound is in 3D dimension */
		bool IsSound3D() const;
		/** set the 3D flag */
		void Enable3D(bool enable);

		/** set the position of the sound (this enables the 3D feature) */
		void SetPosition(glm::vec3 const& in_position, bool update_3D_sound = true);
		/** set the velocity of the sound (this enables the 3D feature) */
		void SetVelocity(glm::vec3 const& in_velocity, bool update_3D_sound = true);

	public:

		/** whether the sound is paused at the beginning */
		bool paused = false;
		/** whether the sound is looping */
		bool looping = false;

		/** the initial volume of the object */
		float volume = 1.0f;
		/** the blend in time of the object */
		float blend_in_time = 0.0f;

		/** true whether the sound is in 3D */
		bool is_3D_sound = false;
		/** the position of the sound in 3D */
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		/** the velocity of the sound in 3D */
		glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };
		/** the minimal distance for sound in 3D */
		float min_distance = 0.0f;
		/** the maximum distance for sound in 3D */
		float max_distance = 0.0f;
		/** timer for far 3D sound before entering pause */
		float pause_timer_when_too_far = 0.0f;

		/** the name of the sound object to create */
		std::string sound_name;

		/** the names of the categories ... */
		std::vector<std::string> category_names;
		/** ... or a pointer on the category */
		std::vector<SoundCategory*> categories;
	};

#endif

}; // namespace chaos