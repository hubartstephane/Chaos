#include <chaos/Chaos.h>

namespace chaos
{
	// =============================================
	// SoundListenerCameraComponent
	// =============================================

	bool SoundListenerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		if (camera != nullptr)
		{
			Game * game = camera->GetGame();
			if (game != nullptr)
			{
				SoundManager * sound_manager = game->GetSoundManager();
				if (sound_manager != nullptr)					
				{
					glm::vec3 position = glm::vec3(camera->GetCameraBox(false).position, 0.0f);
					glm::vec3 speed    = glm::vec3(0.0f, 0.0f, 0.0f);
					sound_manager->SetListenerPosition(position, speed);
				}
			}
		}
		return true;
	}

}; // namespace chaos
