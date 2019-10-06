#include "Ludum45Enemies.h"

#include <chaos/TiledMap.h>

bool EnemyPattern::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{

	return true;
}








bool EnemyType::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	bitmap_name = def->FindPropertyString("BITMAP_NAME", "");





	return true;
}


