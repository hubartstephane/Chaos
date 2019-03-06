
#pragma once

#if 0

	====================================================================
	== TODO
	====================================================================

	- HOT RELOAD RESOURCES

	- REVOIR LOADERS

	- PARTICLE ALLOCATION : extra data

	- HUD component

	- PLAYER component

	- test platformer

	- ATLAS DYNAMIC

	- FENCE OPENGL a chaque frame

	- CHANGE MUSIC in game

	- BACKGROUND pour les particles de text

	- ANIMATION pour sprites (shaders ?)

	- OPENGL STATE CACHE

	- MATERIAL AUTOMATIQUE :    PROGRAMS => MATERIAL

	- LOAD LEVELS on demand only (and not at all at the beginning)

	- TITRE LEVELS (voir game over / pause)

	- MATERIAL COMPOSITE/RICH


		name -----> MATERIAL CHILD


	- FULLSCREEN TOGGLE

	- TILEDMAP : revoir collision player (a la demande seulement ?)

	- HUD debug menu => menu de base

	- stack des inputs

	- chargement resources par extension

	- ajouter des program sources par defaut

	- fix Doxygen

	- compilation Linux

	- repenser post process

	- fix BIND TEXTURE dans les shaders

	- A REAL CAMERA SYSTEM

	- RENDERER CAMERA+SCENE + extra
















	- Game::GetLevel(int level_index) => maybe some search to do to have not sequential levels

  -GameLevel => have level_index. We should add a NAME (NamedObject) so we can acces levels by name. Non sequencial level acces

	-GameLevelInstance::camera_box : very poor API. We could have more than one camera. Apply effects/post process ...
		                               we could have a class Camera (like player) and swap from one to another

	-Player::OnLevelStarted(...) : could be created OR NOT ... see GameLevelInstance::OnPlayerEntered(...)

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

	=> In GameFramework.h, the HUD is expected to be called PlayingHUD. We should remove any class difference and use HUDComponent instead
		 This would make more reusable code

	====================================================================
	== NOTES
	====================================================================

		bool GameLevelInstance::CheckGameOverCondition();
		bool GameLevelInstance::IsLevelCompleted() const;
		bool GameLevelInstance::CanCompleteLevel() const;

#endif