#include <chaos/Chaos.h>

#include "Ludum46Game.h"

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

	auto p = 1.0f / glm::vec2(3.0f, 2.0f);

	chaos::SceneNode n;
	n.SetPosition({ 10.0f, 100.0f });
	n.SetRotator(0.1f);
	n.SetScale({1.0f, 2.0f});

	auto m1 = n.GetLocalToParent();
	auto m2 = n.GetParentToLocal();
	auto m3 = m1 * m2;

	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
