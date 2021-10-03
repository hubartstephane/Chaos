#pragma once

#include "Ludum49.h"
#include "Ludum49Game.h"
#include "Ludum49Level.h"

// =================================================
// Landscape
// =================================================

class Landscape;

class LandscapeMorph : public Object
{
	CHAOS_DECLARE_OBJECT_CLASS2(LandscapeMorph, Object);

public:

	bool Tick(Landscape* landscape,float delta_time);

	float GetInternalTime() const;

protected:

	virtual bool DoTick(Landscape* landscape, float delta_time);

protected:

	float internal_time = 0.0f;


};

// ----------------

class LandscapeMorphCircle : public LandscapeMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LandscapeMorphCircle, LandscapeMorph);

protected:

	virtual bool DoTick(Landscape* landscape, float delta_time) override;
};


// =================================================
// Landscape
// =================================================

class Landscape : public TMObject
{
	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(Landscape, TMObject);

public:

	Landscape();

	virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;

	virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params) override;

	virtual bool DoTick(float delta_time) override;

	void BuildMesh();

	int smooth_count = 0;
	float smooth_factor = 0.3f;

	float internal_t = 0.0f;

	box2 ori_bounding_box;

	std::vector<shared_ptr<LandscapeMorph>> morphs;

	shared_ptr<GPUDynamicMesh> mesh;
	shared_ptr<GPUDynamicMesh> debug_mesh;

	std::vector<glm::vec2> points;
	std::vector<glm::vec2> smoothed_points;
};


// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public TMLevelInstance
{
public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumLevelInstance, TMLevelInstance);

	LudumLevelInstance();


protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual bool Initialize(Game * in_game, Level * in_level) override;
	/** override */
	virtual void CreateCameraComponents(Camera* camera, TMCameraTemplate* camera_template) override;

	/** override */
	virtual bool IsPlayerDead(Player* player) override;

	virtual bool CheckLevelCompletion() const override;

	virtual bool CanCompleteLevel() const override;

	virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params) override;

	virtual uint64_t GetCollisionFlagByName(char const* name) const override;





public:

	/** pointer on game */
	class LudumGame * game = nullptr;


	mutable float completion_timer = -1.0f; // shu49 : due to Checkcomplete constness !!
	float completion_delay = 5.0f;

};
