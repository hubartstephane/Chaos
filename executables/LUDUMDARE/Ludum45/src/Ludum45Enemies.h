#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/BitmapAtlas.h>
#include <chaos/ReferencedObject.h>
#include <chaos/NamedObject.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>


class EnemyPattern : public chaos::ReferencedObject
{

public:

	virtual bool Initialize(chaos::TiledMap::ObjectTypeDefinition const * def);



public:

	std::string name;
};

class EnemyType : public chaos::ReferencedObject
{
public:

	virtual bool Initialize(chaos::TiledMap::ObjectTypeDefinition const * def);


public:

	std::string name;

	std::string bitmap_name;
};