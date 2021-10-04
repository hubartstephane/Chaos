#pragma once

#include "Ludum49.h"
#include "Ludum49Game.h"
#include "Ludum49Level.h"

// =================================================
// Landscape
// =================================================

class Landscape;
class LPMorph;

using MORPH_TREE_IT = std::vector<shared_ptr<LPMorph>>::iterator;

using MORPH_DATA_MAP = std::map<std::string, float>;

class LPMorph : public Object
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph, Object);

public:

	bool Tick(Landscape* landscape,float delta_time);

	float GetInternalTime() const;

	virtual bool ComposeTree(MORPH_TREE_IT& it, MORPH_TREE_IT end);

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver);

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2>& mutable_points);

	virtual float GetStrength(Landscape* landscape);

protected:

	virtual bool DoTick(Landscape* landscape, float delta_time);

protected:

	float internal_time = 0.0f;
};

class LPMorph_ID : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_ID, LPMorph);


};

// =================================================

class LPMorph_Unary : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Unary, LPMorph);

public:

	virtual bool ComposeTree(MORPH_TREE_IT& it, MORPH_TREE_IT end);

	virtual bool DoTick(Landscape* landscape, float delta_time) override;

	shared_ptr<LPMorph> arg1;
};

// =================================================

class LPMorph_Binary : public LPMorph_Unary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Binary, LPMorph_Unary);

public:

	virtual bool ComposeTree(MORPH_TREE_IT& it, MORPH_TREE_IT end);

	virtual bool DoTick(Landscape* landscape, float delta_time) override;

	shared_ptr<LPMorph> arg2;
};

// =================================================

class LPMorph_Ternary : public LPMorph_Binary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Ternary, LPMorph_Binary);

public:

	virtual bool ComposeTree(MORPH_TREE_IT& it, MORPH_TREE_IT end);

	virtual bool DoTick(Landscape* landscape, float delta_time) override;

	shared_ptr<LPMorph> arg3;
};

// =================================================

class LPMorph_Circle : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Circle, LPMorph);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points) override;

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	float radius = 100;

	int vertice_count = 0;
};

// =================================================

class LPMorph_Rectangle : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Rectangle, LPMorph);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points) override;

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	float width = 100;
	float height = 100;

	int vertice_count = 0;
};

// =================================================

class LPMorph_Gear : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Gear, LPMorph);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points) override;

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	float r1 = 100;
	float r2 = 50;

	int gear_count = 7;
};



// =================================================

class LPMorph_Rotate : public LPMorph_Binary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Rotate, LPMorph_Binary);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2>& mutable_points) override;
};

// =================================================

class LPMorph_Translate : public LPMorph_Ternary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Translate, LPMorph_Ternary);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2>& mutable_points) override;
};



// =================================================

class LPMorph_Function : public LPMorph_Unary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Function, LPMorph_Unary);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points) override;

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	virtual float GetHeightValue(float x, float strength) { return 0; }

	float width = 100;
	float height = 100;

	int vertice_count = 0;
};

// =================================================

class LPMorph_Wave : public LPMorph_Function
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Wave, LPMorph_Function);

protected:

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	virtual float GetHeightValue(float x, float strength) override;

	float amplitude = 50.0f;
	float wave_length = 1.0f;
};


// =================================================

class LPMorph_Mix : public LPMorph_Ternary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Mix, LPMorph_Ternary);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points) override;

};




// =================================================

class LPMorph_Scale : public LPMorph_Binary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Scale, LPMorph_Binary);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points) override;
	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	int ignore_x = 0;
	int ignore_y = 0;

};

// =================================================

class LPMorph_BaseMesh : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_BaseMesh, LPMorph);

protected:

	virtual bool GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points) override;

};

// =================================================

class LPMorph_Time : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Time, LPMorph);

public:

	virtual float GetStrength(Landscape * landscape) override;
};

// =================================================

class LPMorph_Mod : public LPMorph_Unary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Mod, LPMorph_Unary);

public:

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	virtual float GetStrength(Landscape * landscape) override;

	float A = 0.0f;
	float B = 1.0f;
};

// =================================================

class LPMorph_LinearStep : public LPMorph_Unary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_LinearStep, LPMorph_Unary);

public:

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	virtual float GetStrength(Landscape * landscape) override;

	float A = 0.0f;
	float B = 1.0f;
};

class LPMorph_LinearStepTwice : public LPMorph_Unary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_LinearStepTwice, LPMorph_Unary);

public:

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	virtual float GetStrength(Landscape * landscape) override;

	float A = 0.0f;
	float B = 1.0f;
	float C = 2.0f;
	float D = 3.0f;
};

// =================================================

class LPMorph_Const : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Const, LPMorph);

protected:

	virtual float GetStrength(Landscape * landscape) override;

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	float value = 0.0f;

};

class LPMorph_One : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_One, LPMorph);

protected:

	virtual float GetStrength(Landscape* landscape) override { return 1.0f; }
};

class LPMorph_Zero : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Zero, LPMorph);

protected:

	virtual float GetStrength(Landscape* landscape) override { return 0.0f; }
};

class LPMorph_PI : public LPMorph
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_PI, LPMorph);

protected:

	virtual float GetStrength(Landscape* landscape) override { return (float)M_PI; }
};

// =================================================

class LPMorph_Neg : public LPMorph_Unary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Neg, LPMorph_Unary);

protected:

	virtual float GetStrength(Landscape* landscape) override;
};

// =================================================

class LPMorph_Cos : public LPMorph_Unary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Cos, LPMorph_Unary);

protected:

	virtual float GetStrength(Landscape * landscape) override;

	virtual bool Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver) override;

	float speed = 1.0f;
	float radius = 1.0f;
	float offset = 0.0f;
	float time_offset = 0.0f;

};

// =================================================

class LPMorph_Add : public LPMorph_Binary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Add, LPMorph_Binary);

protected:

	virtual float GetStrength(Landscape * landscape) override;
};

class LPMorph_Mul : public LPMorph_Binary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Mul, LPMorph_Binary);

protected:

	virtual float GetStrength(Landscape * landscape) override;
};

class LPMorph_Sub : public LPMorph_Binary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Sub, LPMorph_Binary);

protected:

	virtual float GetStrength(Landscape * landscape) override;
};

class LPMorph_Div : public LPMorph_Binary
{
	CHAOS_DECLARE_OBJECT_CLASS2(LPMorph_Div, LPMorph_Binary);

protected:

	virtual float GetStrength(Landscape * landscape) override;
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

	virtual box2 GetBoundingBox(bool world_system) const override;

	void BuildMesh(std::vector<glm::vec2> const & src);

	int smooth_count = 0;
	float smooth_factor = 0.3f;

	float internal_t = 0.0f;

	box2 ori_bounding_box;

	box2 point_bounding_box;

	shared_ptr<LPMorph> morph;

	//std::vector<shared_ptr<LPMorph>> morphs;

	shared_ptr<GPUDynamicMesh> mesh;
	shared_ptr<GPUDynamicMesh> debug_mesh;

	std::vector<glm::vec2> points; // do not modified this, keep track of original
	std::vector<glm::vec2> smoothed_points;

	glm::vec4 color = { 1.0f, 0.0f, 0.0f, 1.0f };

	float polygon_orientation = 1.0f;
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
