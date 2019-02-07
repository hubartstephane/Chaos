#pragma once

#if 0

	====================================================================
	== TODO
	====================================================================

  -GameLevel => have level_index. We should add a NAME (NamedObject) so we can acces levels by name. Non sequencial level acces

	-GameLevelInstance::camera_box : very poor API. We could have more than one camera. Apply effects/post process ...
		                               we could have a class Camera (like player) and swap from one to another

	-Player::OnLevelStarted(...) : could be created OR NOT ... see GameLevelInstance::OnPlayerLevelStarted(...)

	-Add method to check whether the player loose one life






	HUD
	  -create HUD components to maximize reusability


	====================================================================
	== NOTES
	====================================================================

		bool GameLevelInstance::CheckGameOverCondition();
		bool GameLevelInstance::IsLevelCompleted() const;
		bool GameLevelInstance::CanCompleteLevel() const;

#endif