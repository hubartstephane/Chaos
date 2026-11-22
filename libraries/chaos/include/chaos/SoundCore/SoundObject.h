namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API SoundObject : public Object, public NamedInterface
	{
		friend class SoundManager;

	public:

		/** getter on the irrklang engine */
		irrklang::ISoundEngine* GetIrrklangEngine();

		/** getter on the manager object */
		SoundManager* GetManager();
		/** getter on the manager object */
		SoundManager const* GetManager() const;

		/** blend the volume */
		bool StartBlend(BlendVolumeDesc const& desc, bool replace_older = false);
		/** start a fade out and pause or kill */
		bool FadeOut(float blend_time, bool kill_at_end = false, bool kill_when_paused = false);
		/** stop the object */
		void Stop();

		/** returns whether the object is attached to a manager */
		bool IsAttachedToManager() const;
		/** change the callbacks associated to this object */
		void SetCallbacks(SoundCallbacks* in_callbacks);
		/** get whether the sound is finished */
		bool IsFinished() const { return is_finished; }

		/** pause the object */
		void Pause(bool in_pause = true);
		/** get the own object pause state */
		bool IsPaused() const;
		/** get the final pause status for the object */
		virtual bool IsEffectivePaused() const;

		/** change the object volume */
		void SetVolume(float in_volume);
		/** get the own object volume */
		float GetVolume() const;
		/** get the final volume for the sound (category and blendings taken into account) */
		virtual float GetEffectiveVolume() const;

		/** returns true whether there is a blendout and waiting stop */
		bool IsPendingKill() const;
		/** returns true whether there is a blending */
		bool HasVolumeBlending() const;

	protected:

		/** internal tick the object */
		virtual void TickObject(float delta_time);
		/** unbind from manager */
		virtual void OnRemovedFromManager();
		/** remove element from manager list and detach it */
		virtual void RemoveFromManager();
		/** get whether the sound is finished */
		virtual bool ComputeFinishedState();

		/** called at terminaison of the object */
		void OnObjectFinished();
		/** update the flag finished and return it */
		bool UpdateFinishedState();
		/** called at blend terminaison */
		void OnBlendFinished();

		/** internal method called when effective volume has been changed */
		virtual void DoUpdateEffectiveVolume(float effective_volume);
		/** internal method called when effective paused state has been changed */
		virtual void DoUpdateEffectivePause(bool effective_pause);

		/** loading from a JSON object */
		virtual bool InitializeFromJSON(nlohmann::json const * json);

	protected:

		/** whether the object is finished */
		bool is_finished = false;
		/** whether the sound is paused */
		bool paused = false;
		/** the volume */
		float volume = 1.0f;
		/** the manager */
		SoundManager* sound_manager = nullptr;
		/** the blending description */
		BlendVolumeDesc blend_desc;
		/** the blend value */
		float           blend_value = 1.0f;
		/** the callbacks */
		shared_ptr<SoundCallbacks> callbacks;
	};

#endif

}; // namespace chaos