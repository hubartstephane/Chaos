
#pragma once

#if 0

	====================================================================
	== TODO
	====================================================================
	
	

	- BUG FIX

	1/ certains attributs dans les shaders peuvent disparaitre au moment du build si ils ne sont pas utiliser
	2/ si on fait du HOT RELOAD de shader et que des attributs disparaissent/apparaissent, le systeme de cache de VertexArray par programme semble perdu
	   (a verifier)
	3/ pour que le HOT RELOAD refonctionne (contournement), on peut recharger le niveau
		
		F4->Resoure RELOAD
		F5->Level RELOAD


	   une solution pourrait etre de mettre le cache dans le GPUProgram (a verifier)




















	- revoir CreateBackgroundImage(...) : SetBackground(...) + SetBackgroundMaterial(...)
	
	  sortir ca du ParticleManager. Creer un renderable dedié "background_renderable"
	
	
	
	
	- SoundManager : multi-category
	=> Pause -> BlendPause category=INGAME
	  
	=> travailler sur les musiques. MENU-PAUSE-IN GAME
	  probablement mieux a faire
	  
	  
	  
	  
	  
	- HUD Component
	
	=> recuperer le titre du niveau courant
	
	=> faire HUD tutorial
	
	=> configuration des components par JSON ???
	
	
	
	
	- MATERIAL
	
	=> Revoir SubMaterials. Utiliser ca dans LD43 (quantic paouf) pour le multipass
	
	
	
	- PROGRAMS
	=> ajouter missing VERTEX SHADER si non specifié
	=> Chargement d un program
		-> on peut charger un fichier       'myprogram.pgm'
		-> ou on peut charger le repertoire 'myprogram' (si le .pgm n'existe pas, le generer a la volée)
		
	
	
	- MANAGER
	
	[recurse] : a revoir. il devrait y avoir un niveau [files]: [...]
	eventuellement PATH
	
	
	
	
	- COLLISIONS
	=>revoir box/obox pour fixer les soucis de camera dans TiledMapLevel
	
	
	
	- LEVELS
	=>un fichier levels.json pour decrire les fichiers, eventuellement une map pour le menu
	
	
	
	
	
	
	-ParticleSystem
	=>utiliser des VERTEX_BUFFER tournant + GPUFence
	
	====================================================================
	== TODO
	====================================================================
	
	
	
	
	
	
	
	
	
	
	
	
	
	

		-- animated bitmap in atlas

		explosion_4x4 est refernce dans JSON, pas "explosion" => a changer non ?



		-- Mieux a  faire que d un coté
		
			chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives

		  et 

			chaos::ParticleTools::GenerateBoxParticle(...) + copy couleur 

		 de l autre


		chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices, particle->rotation);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle->color;
	return 6;




	//return chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, vertices, vertices_per_particle);



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


	- PLAYER component

	- test platformer

	- ATLAS DYNAMIC

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



#endif