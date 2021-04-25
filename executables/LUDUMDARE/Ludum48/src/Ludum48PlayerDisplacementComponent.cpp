#include "Ludum48PlayerDisplacementComponent.h"

#include "Ludum48LevelInstance.h"

bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{

	return true;
}


bool LudumPlayerDisplacementComponent::SerializeIntoJSON(nlohmann::json& json_entry) const
{
	if (!chaos::PlayerDisplacementComponent::SerializeIntoJSON(json_entry))
		return false;
	chaos::JSONTools::SetAttribute(json_entry, "pawn_speed", pawn_speed);
	chaos::JSONTools::SetAttribute(json_entry, "push_speed", push_speed);
	return true;
}

bool LudumPlayerDisplacementComponent::SerializeFromJSON(nlohmann::json const& json_entry)
{
	if (!chaos::PlayerDisplacementComponent::SerializeFromJSON(json_entry))
		return false;

	chaos::JSONTools::GetAttribute(json_entry, "pawn_speed", pawn_speed);
	chaos::JSONTools::GetAttribute(json_entry, "push_speed", push_speed);
	return true;
}

