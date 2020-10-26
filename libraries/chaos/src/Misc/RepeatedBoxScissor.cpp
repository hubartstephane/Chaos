
#include <chaos/Chaos.h>

namespace chaos
{

	RepeatedBoxScissor::RepeatedBoxScissor(chaos::box2 const& in_target_box, chaos::box2 const& in_scissor_box, bool in_wrap_x, bool in_wrap_y)
	{
		// copy the parameters
		target_box = in_target_box;
		scissor_box = in_scissor_box;
		wrap_x = in_wrap_x;
		wrap_y = in_wrap_y;

		// First,
		//   Considere that there is always wrap_x = wrap_y = true
		//   Search along X and Y, the instance index that must collides with the scissor box
		// Then,
		//   whenever there are no wrap, we can skip the whole rendering if the instancee (0, 0) is not in the visible range

		glm::vec2 target_bottomleft = GetBoxCorners(in_target_box).first;
		glm::vec2 scissor_bottomleft = GetBoxCorners(in_scissor_box).first;

		glm::vec2 target_size = 2.0f * in_target_box.half_size;
		glm::vec2 scissor_size = 2.0f * in_scissor_box.half_size;

		// clamp out all non intersecting box when no WRAP
		if (!wrap_x)
		{
			if ((target_bottomleft.x + target_size.x < scissor_bottomleft.x) || (target_bottomleft.x > scissor_bottomleft.x + scissor_size.x))
			{
				start_instance = last_instance = glm::ivec2(0, 0); // nothing to render at all
				return;
			}
		}

		if (!wrap_y)
		{
			if ((target_bottomleft.y + target_size.y < scissor_bottomleft.y) || (target_bottomleft.y > scissor_bottomleft.y + scissor_size.y))
			{
				start_instance = last_instance = glm::ivec2(0, 0); // nothing to render at all
				return;
			}
		}

		// number of time to decal the layer box, to be directly left of the scissor box
		glm::ivec2 offset_count = glm::ivec2(
			(int)std::ceil((scissor_bottomleft.x - target_bottomleft.x - target_size.x) / target_size.x),
			(int)std::ceil((scissor_bottomleft.y - target_bottomleft.y - target_size.y) / target_size.y)
		);

		// the bottomleft corner of the decaled box
		glm::vec2  virtual_target_bottomleft = target_bottomleft + chaos::RecastVector<glm::vec2>(offset_count) * target_size;
		// competition of the number of repetition
		glm::vec2  tmp = ((scissor_bottomleft - virtual_target_bottomleft + scissor_size) / target_size);

		glm::ivec2 repetition_count = glm::ivec2(
			(int)std::ceil(tmp.x),
			(int)std::ceil(tmp.y)
		);

		// unwrap case, then only visible instance is 0 (we previsously tested for visibility)
		if (!wrap_x)
		{
			offset_count.x = 0;
			repetition_count.x = 1;
		}
		// unwrap case, then only visible instance is 0 (we previsously tested for visibility)
		if (!wrap_y)
		{
			offset_count.y = 0;
			repetition_count.y = 1;
		}

		// finalize the initialization
		start_instance = offset_count;
		last_instance = offset_count + repetition_count;
	}

	glm::vec2 RepeatedBoxScissor::GetInstanceOffset(glm::ivec2 const& index) const
	{
		return 2.0f * target_box.half_size * chaos::RecastVector<glm::vec2>(index);
	}

}; // namespace chaos
