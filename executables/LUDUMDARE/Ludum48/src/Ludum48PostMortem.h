#if 0


  ------------------------------------------------------

TMTileCollisionIterator TMLevelInstance::GetTileCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry)

->collision avec n importe quel type de primitive box/sphere...

class CollisionRequest()
{
	box2 / sphere2 / always;
	bool open_geometry;
}

reecriture de l iterator ... combinaison d iterator

	------------------------------------------------------

revoir DisplacementComponent qui est carrement vide

GetVelocity() ...
GetMaxVelocity() ...
GetMaxDistance() ... (pour les grosses chutes de framerate) ???

------------------------------------------------------

faire meilleure gestion du tile (pas forcement par particle libre)

------------------------------------------------------

revoir CollisionMask 

  permettre d ajouter ses propre flag plus facilement. Est ce que cela ne pourrait pas devenir des tag ?

------------------------------------------------------

certaines fonction sont virtual dans TMLevel, d autres dans TMLevelInstance. Faire le tri


	GetCollisionFlagByName

	------------------------------------------------------



#endif