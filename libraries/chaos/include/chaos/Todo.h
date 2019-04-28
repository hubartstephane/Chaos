
#pragma once

#if 0

	====================================================================
	== TODO
	====================================================================

		-- Mieux a  faire que d un coté
		
			chaos::ParticleDefault::ParticleTrait::ParticleToVertices

		  et 

			chaos::ParticleTools::GenerateBoxParticle(...) + copy couleur 

		 de l autre


		chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices, particle->rotation);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle->color;
	return 6;




	//return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, vertices, vertices_per_particle);



		- TiledMap :

	    properties string multiline

			<PROP value="mono ligne"/>

			<PROP>
			multi
			ligne
			</PROP>


		AddFontInfo .. AddColor  => ParticleTextGenerator : faudrait avoir quelque chose qui change la taille + la couleur ...

		

		Printf(\\[])=> resultat bizarre


		-particle_text_generator->AddColor("LEVELTITLE", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

		   => utiliser LEVELTITLE plutot que LEVEL_TITLE : a fixer




		- TiledMap : spawner + detection collision
		-  fadout









		- in game.json, better font management => "title_font_path", "font_path" ... on demand

			   verifier si ca supporte bien le DirectResourceFiles

		PerAllocationData : renommer ?

		=> Tick( ... )





	- REVOIR charger JSON pour faire une passe de commentaire

	- REVOIR LOADERS

		[recuse] : vraie recursion + filtre de fichiers

		[recurse] :"string"
		[recurse] : ["p1","p2"...]
		[recurse] : {"path:"..... + autre}


	- SHADER : faire un shader placeholder en cas d erreur ?





	- HUD component

	- PLAYER component

	- test platformer

	- ATLAS DYNAMIC

	- FENCE OPENGL a chaque frame

	- CHANGE MUSIC in game

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
		bool GameLevelInstance::IsLevelCompleted(bool & loop_levels) const;
		bool GameLevelInstance::CanCompleteLevel() const;

#endif