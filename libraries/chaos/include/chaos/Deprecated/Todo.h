
#pragma once

#if 0

	===========================================================================================
	== A faire pour LD47
	===========================================================================================

	-rendu des TMObjects

	-Allocation provisoire : detruit a la fin du rendu

	-utiliser des droites comme ligne de mouvement pour des acteurs



	- Verifier les Loaders avec les NAME+TAG

	- Filtre d image pour ajouter de l alpha sous certaine condition

	- Jeu de platforme a faire

	- TiledMap => Generer des vrais objets sur la map (qui auront un ParticleAllocation) => plus facile pour la serialization

	- Faire des Portail => Next Level contient nom de level + nom de player start

	- TiledMap => supporter Object Template (etudier l interet)

	
https://www.mapeditor.org/2020/08/05/tiled-1-4-2-released.html
https://www.mapeditor.org/2020/06/25/tiled-1-4-1-released.html
https://www.mapeditor.org/2020/05/27/tiled-1-4-beta-released.html
https://www.mapeditor.org/2020/06/20/tiled-1-4-0-released.html

#endif


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
	=>revoir box/obox pour fixer les soucis de camera dans TMLevel
	
	
	
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
		
			ParticleDefaultLayerTrait::ParticleToPrimitives

		  et 

			ParticleTools::GenerateBoxParticle(...) + copy couleur 

		 de l autre


		ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices, particle->rotation);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle->color;
	return 6;




	//return ParticleDefaultLayerTrait::ParticleToPrimitives(particle, vertices, vertices_per_particle);



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

  -Level => have level_index. We should add a NAME (NamedObject) so we can acces levels by name. Non sequencial level acces

	-LevelInstance::camera_box : very poor API. We could have more than one camera. Apply effects/post process ...
		                               we could have a class Camera (like player) and swap from one to another

	-Player::OnLevelStarted(...) : could be created OR NOT ... see LevelInstance::OnPlayerEntered(...)

	-Add method to check whether the player loose one life



	Game::CHAOS_FIND_RENDERABLE_CHILD

		- FindRenderableLayer, FindParticleLayer unsued
		- Music management : CreateAllMusic ?? bad idea if we want to change music per level ...



	bool Game::OnEnterGame(PhysicalGamepad * in_physical_gamepad)
	{
		ResetGameVariables(); <=== now there is a GameInstance for that, see if this can be removed


	}
	=> create an INSTRUCTION screen

	=> Game::Initial_Life

	=> In GameFramework.h, the HUD is expected to be called PlayingHUD. We should remove any class difference and use HUDComponent instead
		 This would make more reusable code
















class toto : public chaos::Object, public chaos::JSONSerializable
{
public:

	CHAOS_DECLARE_OBJECT_CLASS(toto, chaos::Object)

	virtual bool SerializeFromJSON(nlohmann::json const& json) override
	{
		chaos::JSONTools::GetAttribute(json, "myvalue", myvalue);

		return true;
	}

	int myvalue = 12345;
};

#if 0


	ExecutionContext BuildChildSequence(std::function<void(ExecutionContext context_sequence)> build_func)
	{
		Lock();
		ExecutionContext result = AddChildSequence();
		result.Lock();
		build_func(result);
		result.Unlock();
		Unlock();
		return result;
	}



	// prevent the dispatch of completion event
	void Lock()
	{
		ConditionalCreateData();
		++context_data->lock_count;

	}

	void Unlock()
	{
		ConditionalCreateData();
		if (--context_data->lock_count == 0)
		{

		}
	}


#endif

// ============================================================================

class ExecutionContextData;

class ExecutionContext
{
	friend class ExecutionContextData;

protected:

	/** constructor with data */
	ExecutionContext(ExecutionContextData* in_context_data): 
		context_data(in_context_data){}

public:

	/** default constructor */
	ExecutionContext() = default;
	/** copy constructor */
	ExecutionContext(ExecutionContext const &) = default;
	/** default constructor */
	ExecutionContext(ExecutionContext &&) = default;


	/** set the task to be completed */
	void CompleteTask();
	/** create a child task */
	ExecutionContext AddChildTask();
	/** create a child sequence task */
	void AddChildSequenceTask(std::function<void(ExecutionContext)> func);

	/** add (or call directly) a delegate whenever the task is being finished */
	void AddCompletionDelegate(std::function<void()> func);

	/** returns whether the task is already completed */
	bool IsCompleted() const;
	/** returns whether the task is already completed */
	operator bool() const;

	/** prevent the dispatch of completion event */
	void Lock();
	/** enable the dispatch of completion event */
	void Unlock();

protected:

	/** the context data */
	chaos::shared_ptr<ExecutionContextData> context_data;
};



// ======================================================

class ExecutionContextData : public chaos::Object
{
	friend class ExecutionContext;

protected:

	/** constructor */
	ExecutionContextData(ExecutionContextData* in_parent_context = nullptr) :
		parent_context(in_parent_context) {}
	/** check whether the task is completed */
	bool IsCompleted() const;
	/** create a child task */
	ExecutionContextData * AddChildTask();
	/** create a sequence pending task */
	void AddChildSequenceTask(std::function<void(ExecutionContext)> func);
	/** lock the context */
	void Lock();
	/** unlock the context (pending actions started) */
	void Unlock();
	/** complete leaf task */
	void CompleteTask();
	/** called to continue processing */
	void OnCompletion();
	/** called whenever a child task is being completed */
	void OnChildTaskCompleted(ExecutionContextData* child_task);
	/** add a delegate at completion */
	void AddCompletionDelegate(std::function<void()> func);

protected:

	/** a mutex to protect for concurrency actions */
	mutable std::recursive_mutex critical_section;
	/** the parent of the task */
	ExecutionContextData* parent_context = nullptr;
	/** count the number of time the object has been locked */
	int lock_count = 0;
	/** whenever the task has been completed during its lock */
	bool completed_during_lock = false;
	/** all child pending tasks */
	int child_task_count = 0;
	/** all sequencial execution */
	std::vector < std::function<void(ExecutionContext)>> pending_execution;
	/** delegates to call whenever the task is completed */
	std::vector<std::function<void()>> completion_functions;
};

// ======================================================

ExecutionContext ExecutionContext::AddChildTask()
{
	if (context_data == nullptr)
		context_data = new ExecutionContextData;
	return ExecutionContext(context_data->AddChildTask());
}

void ExecutionContext::AddChildSequenceTask(std::function<void(ExecutionContext)> func)
{
	if (context_data == nullptr)
		context_data = new ExecutionContextData;
	context_data->AddChildSequenceTask(func);
}

void ExecutionContext::AddCompletionDelegate(std::function<void()> func)
{
	if (context_data == nullptr)
		func();
	else
		context_data->AddCompletionDelegate(func);
}

void ExecutionContext::CompleteTask()
{
	if (context_data != nullptr)
		context_data->CompleteTask();
}

bool ExecutionContext::IsCompleted() const
{
	if (context_data == nullptr)
		return true;
	return context_data->IsCompleted();
}

ExecutionContext::operator bool() const
{
	return IsCompleted();
}

void ExecutionContext::Lock()
{
	if (context_data == nullptr)
		context_data = new ExecutionContextData;
	context_data->Lock();
}

void ExecutionContext::Unlock()
{
	assert(context_data != nullptr);
	context_data->Unlock();
}

// ======================================================

void ExecutionContextData::AddCompletionDelegate(std::function<void()> func)
{
	std::unique_lock lock(critical_section);
	if (IsCompleted())
		func();
	else
		completion_functions.push_back(func);
}

void ExecutionContextData::OnCompletion()
{
	std::unique_lock lock(critical_section);
	if (child_task_count > 0)
		return;
	// start new sequence execution
	if (pending_execution.size() > 0)
	{
		auto next_execution = *pending_execution.begin();
		pending_execution.erase(pending_execution.begin());

		ExecutionContext other_context = ExecutionContext(AddChildTask());
		next_execution(other_context);
		return;
	}
	// or really complete
	else
	{
		// the completion delegates
		for (auto& f : completion_functions)
			f();
		completion_functions.clear();
		// notify parent for the end
		if (parent_context != nullptr)
			parent_context->OnChildTaskCompleted(this);
	}
}

void ExecutionContextData::Lock()
{
	std::unique_lock lock(critical_section);
	++lock_count;
}

void ExecutionContextData::Unlock()
{
	std::unique_lock lock(critical_section);
	if (--lock_count == 0 && completed_during_lock)
	{
		completed_during_lock = false;
		OnCompletion();
	}
}

void ExecutionContextData::CompleteTask()
{
	std::unique_lock lock(critical_section);
	// should only be called manually on a leaf task
	assert(child_task_count == 0);
	assert(pending_execution.size() == 0);
	assert(parent_context != nullptr);

	if (lock_count > 0)
		completed_during_lock = true;
	else
		OnCompletion();
}


ExecutionContextData * ExecutionContextData::AddChildTask()
{
	std::unique_lock lock(critical_section);
	++child_task_count;
	return new ExecutionContextData(this);
}

void ExecutionContextData::AddChildSequenceTask(std::function<void(ExecutionContext)> func)
{
	std::unique_lock lock(critical_section);
	if (IsCompleted()) // include lock
	{
		ExecutionContext other_context = ExecutionContext(AddChildTask());
		func(other_context);
	}
	else
	{
		pending_execution.push_back(func);
	}
}

bool ExecutionContextData::IsCompleted() const
{
	std::unique_lock lock(critical_section);
	if (child_task_count > 0)
		return false;
	if (pending_execution.size() > 0)
		return false;
	if (lock_count > 0)
		return false;
	return true;
}

void ExecutionContextData::OnChildTaskCompleted(ExecutionContextData* child_task)
{
	std::unique_lock lock(critical_section);
	--child_task_count;
	if (lock_count > 0)
		completed_during_lock = true;
	else
		OnCompletion();
}












#if 1


// quand on detruit le future d un async, c 'est bloquant


ExecutionContext TestSimple1()
{
	ExecutionContext result;
	
	for (int i = 0; i < 2; ++i)
	{
		ExecutionContext child = result.AddChildTask();
		std::thread t([child, i]() mutable
		{
			std::this_thread::sleep_for(std::chrono::seconds(3 * i + 3));
			child.CompleteTask();
		});
		t.detach(); // because thread is on stack and assert
	}
	return result;
}





#endif





#if 0

  int workgroup_count[3];
  int workgroup_size[3];
  int workgroup_invocations;

  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);

glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workgroup_size[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workgroup_size[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workgroup_size[2]);

  // glBindImageTexture 
  // glDispatchCompute -> number of workgroup
  // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); -- bloque le CPU
  // glFinish block until all GL execution is complete

  // layout (local_size_x = 16, local_size_y = 16) in; --> size of a work group

  // gl_LocalInvocationID.xy * gl_WorkGroupID.xy == gl_GlobalInvocationID



  gl_NumWorkGroups
	  This variable contains the number of work groups passed to the dispatch function.
	  gl_WorkGroupID
	  This is the current work group for this shader invocation.Each of the XYZ components will be on the half - open range[0, gl_NumWorkGroups.XYZ).
	  gl_LocalInvocationID
	  This is the current invocation of the shader within the work group.Each of the XYZ components will be on the half - open range[0, gl_WorkGroupSize.XYZ).
	  gl_GlobalInvocationID

	  int size[3];
  glGetProgramiv(ComputeShaderID, GL_COMPUTE_WORK_GROUP_SIZE, size);

#endif






#endif