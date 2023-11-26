namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	using SafeZone = std::pair<glm::vec2, glm::vec2>;

	class CameraTools;
	class Camera;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =============================================
	// CameraTools
	// =============================================

	class CHAOS_API CameraTools
	{
	public:

		/** returns the transformation matrix from a camera obox */
		static glm::mat4x4 GetCameraTransform(obox2 const& obox);
		/** returns the projection matrix from a camera obox */
		static glm::mat4x4 GetProjectionMatrix(obox2 const& obox);

		/** apply safe zone to a camera box */
		static box2 GetSafeCameraBox(box2 const& camera_box, SafeZone const& safe_zone);
	};

	// =============================================
	// Camera
	// =============================================

	class CHAOS_API Camera : public Tickable, public JSONSerializableInterface
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(Camera, Tickable);

	public:

		CHAOS_DECLARE_GAMEPLAY_GETTERS();

		/** initialization method */
		bool Initialize(LevelInstance* in_level_instance);

		/** set the camera box */
		void SetCameraBox(box2 const& in_box) { camera_box = in_box; }

		/** get the camera box */
		box2  GetCameraBox(bool apply_modifiers = true) const;
		/** get the camera OBox */
		obox2 GetCameraOBox(bool apply_modifiers = true) const;


		/** get the camera OBox when level instance is started */
		obox2 const& GetInitialCameraOBox() const { return initial_camera_obox; }
		/** set the camera initial OBox */
		void SetInitialCameraOBox(obox2 const& in_obox) { initial_camera_obox = in_obox; }

		/** get the safe zone of the camera */
		SafeZone const& GetSafeZone() const { return safe_zone; }
		/** set the safe zone of the camera */
		void SetSafeZone(SafeZone const& in_safe_zone);
		/** set the centered safe zone of the camera */
		void SetSafeZone(glm::vec2 const& in_safe_zone);

		/** apply the safe zone the the camera box */
		box2 GetSafeCameraBox(bool apply_modifiers = true) const;

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(nlohmann::json const * json) override;

		/** Camera is a CameraComponent owner */
		CHAOS_DECLARE_COMPONENT_OWNER(CameraComponent, Component, components)

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the camera bounding box */
		box2 camera_box;
		/** the initial camera obox (at level startup) */
		obox2 initial_camera_obox;
		/** the safe zone of the camera */
		SafeZone safe_zone = { {0.1f, 0.1f}, {0.9f, 0.9f} };

		/** the game instance owning the camera */
		LevelInstance* level_instance = nullptr;

		/** the components */
		std::vector<shared_ptr<CameraComponent>> components;

	};

#endif

}; // namespace chaos