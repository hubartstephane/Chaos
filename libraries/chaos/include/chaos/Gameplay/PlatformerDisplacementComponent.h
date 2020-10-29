#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace PlatformerParticleFlags
	{
	};

	enum class PlatformerDisplacementState;

	namespace PlatformerDisplacementCollisionFlags
	{
	};

	class PlatformerDisplacementComponentInfo;
	class ComputePlatformerFlagProcessor;
	class PlatformerDisplacementComponent;

}; // namespace chaos

#else

namespace chaos
{

	// =========================================================
	// PlatformerParticleFlags
	// =========================================================

	// XXX : see TileParticleFlags to avoid flag collisions
namespace PlatformerParticleFlags
{
	static int constexpr LADDER = (1 << 9);
	static int constexpr BRIDGE = (1 << 10);
};

	// =========================================================
	// PlatformerDisplacementState
	// =========================================================

	/** the states for the displacement */
enum class PlatformerDisplacementState : int
{
	GROUNDED,     // player is on the ground
	FALLING,      // player in the air and going down
	JUMPING,      // player jump button has been pressed
	JUMPING_DOWN, // whether the player has jumped down through a bridge
	CLIMBING      // whether the player is on a ladder a goind up or down
};

	// =========================================================
	// PlatformerDisplacementCollisionFlags
	// =========================================================

	/** the possible collisions */
namespace PlatformerDisplacementCollisionFlags
{
	static constexpr int NOTHING = 0; // no collision of interrests
	static constexpr int TOUCHING_FLOOR = (1 << 0);
	static constexpr int TOUCHING_BRIDGE = (1 << 1); // a bridge is a kind of floor you can jump-down through
	static constexpr int TOUCHING_CEIL = (1 << 2);
	static constexpr int TOUCHING_WALL = (1 << 3);
	static constexpr int TOUCHING_LADDER = (1 << 4);
};

	// =========================================================
	// PlatformerDisplacementComponentInfo
	// =========================================================


	/** the settings for the component */
class PlatformerDisplacementComponentInfo
{
public:

	/** whether the player left stick value is exactly -1, 0, +1 or whether this may have intermediate values */
	bool discrete_stick_mode = true;
	/** the box extend for collision */
	glm::vec2 pawn_extend = glm::vec2(1.0f, 1.0f);
	/** clamping the velocity in both direction */
	glm::vec2 max_pawn_velocity = glm::vec2(250.0f, 500.0f);
	/** a factor to apply to max_velocity.x whenever pawn is running */
	float run_velocity_factor = 2.0f;
	/** IMPULSE mode : the impulse value for the pawn (immediate velocity given along X axis) */
	glm::vec2 pawn_impulse = glm::vec2(500.0f, 0.0f);
	/** the gravity to apply to the pawn */
	float gravity = 1000.0f;
	/** the maximum height jump */
	float max_jump_height = 80.0f;
	/** the maximum extra jump count */
	int max_extra_jump_count = 1;
	/** whenever the jump button is released during a jump, some ratio of the jump velocity is transmitted to smooth the animation */
	float jump_released_velocity_factor = 0.3f;
	/** the climb speed */
	glm::vec2 climp_velocity = glm::vec2(30.0f, 75.0f);
#if 0
	/** pawn do not advance forward anymore */
	float pawn_break_ratio = 0.01f;
	/** pawn is trying to change its direction */
	float pawn_hardturn_break_ratio = 0.005f;
#endif


	/** pawn do not advance forward anymore */
	float pawn_break_ratio = 0.1f;
	/** pawn is trying to change its direction */
	float pawn_hardturn_break_ratio = 0.05f;

	/** an horizontal velocity above which climb is forbidden */
	float climb_max_horizontal_velocity = 32.0f;
};

	// ==========================================
	// ComputePlatformerFlagProcessor
	// ==========================================

class ComputePlatformerFlagProcessor : public chaos::TiledMap::ComputeCustomFlagProcessor
{
	CHAOS_OBJECT_DECLARE_CLASS2(ComputePlatformerFlagProcessor, chaos::TiledMap::ComputeCustomFlagProcessor);

public:

	/** constructor */
	ComputePlatformerFlagProcessor();
	/** constructor */
	ComputePlatformerFlagProcessor(ComputePlatformerFlagProcessor const& src) = default;
};

	// =========================================================
	// PlatformerDisplacementComponent
	// =========================================================

	/** component for platformer */
class PlatformerDisplacementComponent : public PlayerDisplacementComponent
{

public:

	CHAOS_OBJECT_DECLARE_CLASS2(PlatformerDisplacementComponent, PlayerDisplacementComponent);

	/** constructor */
	using PlayerDisplacementComponent::PlayerDisplacementComponent;

	glm::vec2 GetPawnVelocity() const { return pawn_velocity; }

	PlatformerDisplacementComponentInfo const& GetDisplacementInfo() const { return displacement_info; }

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** compute the new displacement state */
	PlatformerDisplacementState ComputeDisplacementState(chaos::box2& pawn_box, bool jump_pressed, glm::vec2 const& stick_position, int collision_flags);
	/** get the offset from the jumping point when the player is jumping */
	float GetJumpRelativeHeight(float jump_time) const;
	/** get the duration of a jump according to gravity/max_height */
	float GetMaxJumpDuration() const;
	/** get the velocity of jumper at given time */
	float GetJumpVelocity(float jump_time) const;

	/** clamp the player velocity according to limits */
	glm::vec2 ClampPlayerVelocity(glm::vec2 velocity, bool running) const;

protected:


public:

	/** the rules for the displacement */
	PlatformerDisplacementComponentInfo displacement_info;

	/** the current state for the player */
	PlatformerDisplacementState displacement_state = PlatformerDisplacementState::GROUNDED;

	/** the current velocity of the pawn */
	glm::vec2 pawn_velocity = glm::vec2(0.0f, 0.0f);

	/** the current Y position where the jump started */
	float current_jump_start_y = 0.0f;
	/** the jump timer */
	float current_jump_timer = 0.0f;
	/** the current jump count */
	int current_jump_count = 0;

	/** the current Y position where the jump down started */
	float current_jumpdown_start_y = 0.0f;

	/** whether jump was pressed previous frame */
	bool was_jump_pressed = false;
};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION