#include "Ludum45Enemies.h"

#include <chaos/TiledMap.h>

bool EnemyPattern::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	enemy_count = def->FindPropertyInt("ENEMY_COUNT", 0);

	return true;
}








bool EnemyType::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	bitmap_name = def->FindPropertyString("BITMAP_NAME", "");
	enemy_life  = def->FindPropertyFloat("ENEMY_LIFE", enemy_life);
	enemy_damage  = def->FindPropertyFloat("ENEMY_DAMAGE", enemy_damage);

	return true;
}


