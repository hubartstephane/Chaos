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

dans 

	chaos::ParticleLayerBase* LudumLevel::DoCreateParticleLayer(chaos::TMLayerInstance* layer_instance)

	on creer des layers de particles avec  
	
	   new chaos::ParticleLayer<ParticleAnimatedLayerTrait>();

    on est donc obligé de creer de LayerTrait alors qu on pourrait faire de l inplace


		new chaos::ParticleLayer<PARTICLE, VERTEX>();

	------------------------------------------------------

dans 

bool LudumLevel::FinalizeLayerParticles(chaos::TMLayerInstance* layer_instance, chaos::ParticleAllocationBase* allocation)

for (GameObjectParticle& particle : accessor)

on a particle.bitmap_info   ... mais on ne trouve pas l information du tileset qui a permis sa creation

il ya 

TMParticle::gid ... mais un acces directement au TileData serait il pas mieux ?


------------------------------------------------------

ne devrait on pas s 'affranchir de GetTiledMap() depuis TMLayer/Level Instance

------------------------------------------------------

les TMparticle ne savent pas ou elles sont sur la grille. Passer en mode grille est tres fastidieux


------------------------------------------------------

chaos::TMTileCollisionIterator it = GetTileCollisionIterator(GetBoundingBox(), COLLISION_GAMEOBJECT, false);
	while (it)

	reflechir a avoir un begin() et end() pour des meilleurs iterations

		------------------------------------------------------

numeric_limits for glm ??

		glm::vec2 min_position = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	glm::vec2 max_position = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

	------------------------------------------------------

		glm::vec2 std::min(v1, v2); --> il y a glm::min(...)

	------------------------------------------------------

	ParticleTools devrait disparaitre

		------------------------------------------------------

		DrawInterface / GetDebugDrawInterface();

	pourquoi ne pas avoir des parametres par defaut

	DI->SetColor(...)
	DI->SetFill(...)

		------------------------------------------------------

		shared_ptr -> ref_ptr  (pour eviter le conflit avec STL)

		------------------------------------------------------


autorecenter camera : le recentrage automatique fait des marches d escaliers visibles sur dans grandes zone de camra

meilleur parametrage du recentrage

------------------------------------------------------

s 'affranchir de chaos::' dans les projets ludums ???

------------------------------------------------------

quelques ennumeration de couleur YELLOW, GREEN ...

------------------------------------------------------

Dans GameHUD la notion de text/format n'est pas tres claire'










#endif


