namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Sound;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API Sound : public SoundObject
	{
		friend class SoundSource;
		friend class SoundManager;

	public:

		/** returns true whether this sound belongs to given category */
		bool IsOfCategory(SoundCategory const* category) const;

		/** set the position of the sound */
		void SetPosition(glm::vec3 const& in_position);
		/** set the velocity of the sound */
		void SetVelocity(glm::vec3 const& in_velocity);

		/** returns whether the sound is effectively paused */
		virtual bool IsEffectivePaused() const override;
		/** returns the effective volume of the sound */
		virtual float GetEffectiveVolume() const override;

		/** returns whether the sound is in 3D dimension */
		bool IsSound3D() const;
		/** get the position of the sound */
		glm::vec3 GetPosition() const;
		/** get the velocity of the sound */
		glm::vec3 GetVelocity() const;
		/** get whether the sound is looping */
		bool IsLooping() const;

		// XXX : while we want to manager ourself the sound volume depending on distance, remove this
#if 0
		/** the distance after which the sound is no more heared */
		void SetMaxDistance(float distance);
		/** the distance below which the sound is at its max volume */
		void SetMinDistance(float distance);
#endif

		/** change the position of the sound track */
		void SetSoundTrackPosition(int position);

		/** returns the source */
		SoundSource* GetSource() { return source; }
		/** returns the source */
		SoundSource const* GetSource() const { return source; }

	protected:

		/** override */
		virtual void TickObject(float delta_time);
		/** override */
		virtual void DoUpdateEffectiveVolume(float effective_volume);
		/** override */
		virtual void DoUpdateEffectivePause(bool effective_pause);

		/** get volume modifier due to distance for 3D sounds */
		float Get3DVolumeModifier() const;

		/** update irrklang state */
		void DoUpdateIrrklangPause(bool effective_pause);

		/** the sound method (returns true whether it is immediatly finished) */
		virtual bool DoPlaySound(PlaySoundDesc const& play_desc);
		/** unbind from manager */
		virtual void OnRemovedFromManager() override;
		/** remove element from manager list and detach it */
		virtual void RemoveFromManager() override;

		/** get whether the sound is finished */
		virtual bool ComputeFinishedState() override;

	protected:

		/** returns true whether the sound is in 3D */
		bool is_3D_sound = false;
		/** the min distance below which the sound volume is MAX (3D consideration only) */
		float min_distance = 0.0f;
		/** the max distance after which the sound volume is 0.0f (3D consideration only) (different from irrklang consideration) */
		float max_distance = 0.0f;
		/** timer for far 3D sound before entering pause */
		float pause_timer_when_too_far = 0.0f;
		/** value of the timer  */
		float pause_timer_value = 0.0f;

		/** whether the sound is looping */
		bool looping = false;

		/** the position of the sound in 3D */
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		/** the velocity of the sound in 3D */
		glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };

		/** the categories of the sound */
		std::vector<SoundCategory*> categories;
		/** the source that generated this object */
		SoundSource* source = nullptr;

		/** the irrklang sound */
		shared_ptr<irrklang::ISound> irrklang_sound;

		/** the volume that has been sent to irrklang (by default, irrklang creates sounds with volume = 1.0) */
		float cached_effective_volume = 1.0f;
	};

#endif

}; // namespace chaos