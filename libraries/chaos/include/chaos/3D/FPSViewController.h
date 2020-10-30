#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class FPSViewController;
	
}; // namespace chaos

#else

namespace chaos
{
	/** 
	* FPSViewController : this is a class to help handling FPS system with simple actions like go forward or strafe
	*/

	class FPSViewController
	{

	public:

		/** advance the camera */
		void GoBackward(float value);
		/** advance the camera */
		void GoForward(float value);
		/** strafe the camera */
		void StrafeRight(float value);
		/** strafe the camera (X axis, positive values on the right) */
		void StrafeLeft(float value);

		/** change the angles of the camera */
		void SetPitch(float value);
		/** change the angles of the camera */
		void SetYaw(float value);
		/** change the angles of the camera */
		void SetRoll(float value);

		/** increment angle of the camera */
		void IncrementPitch(float value);
		/** increment angle of the camera */
		void IncrementYaw(float value);
		/** increment angle of the camera */
		void IncrementRoll(float value);

		/** get transformation matrix */
		glm::mat4 GlobalToLocal() const;
		/** get transformation matrix */
		glm::mat4 LocalToGlobal() const;

	public:

		/** the position of the camera in its parent system */
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		/** Yaw (expressed in degree) */
		float yaw = 0.0f;
		/** Pitch (expressed in degree) */
		float pitch = 0.0f;
		/** Roll (expressed in degree) */
		float roll = 0.0f;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION