#pragma once

#if 0

	====================================================================
	== TODO
	====================================================================

	- Game::GetLevel(int level_index) => maybe some search to do to have not sequential levels

  -GameLevel => have level_index. We should add a NAME (NamedObject) so we can acces levels by name. Non sequencial level acces

	-GameLevelInstance::camera_box : very poor API. We could have more than one camera. Apply effects/post process ...
		                               we could have a class Camera (like player) and swap from one to another

	-Player::OnLevelStarted(...) : could be created OR NOT ... see GameLevelInstance::OnPlayerLevelStarted(...)

	-Add method to check whether the player loose one life



	Game::DEATH_FIND_RENDERABLE_CHILD

		- FindRenderableLayer, FindParticleLayer unsued
		- Music management : CreateAllMusic ?? bad idea if we want to change music per level ...


	HUD
	  -create HUD components to maximize reusability



	bool Game::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		ResetGameVariables(); <=== now there is a GameInstance for that, see if this can be removed


	}


	=> create an INSTRUCTION screen

	=> Game::Initial_Life



	====================================================================
	== NOTES
	====================================================================

		bool GameLevelInstance::CheckGameOverCondition();
		bool GameLevelInstance::IsLevelCompleted() const;
		bool GameLevelInstance::CanCompleteLevel() const;

#endif