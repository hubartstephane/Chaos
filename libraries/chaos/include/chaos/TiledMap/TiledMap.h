#ifdef CHAOS_FORWARD_DECLARATION

	// all classes in this file
#define CHAOS_TILEDMAP_CLASSES \
(BaseObject) \
(Property) \
(PropertyOwner) \
(TypedObject) \
(GeometricObject) \
(GeometricObjectPoint) \
(GeometricObjectSurface) \
(GeometricObjectRectangle) \
(GeometricObjectEllipse) \
(GeometricObjectPolygon) \
(GeometricObjectPolyline) \
(GeometricObjectText) \
(GeometricObjectTile) \
(Wangset) \
(WangEdgeColor) \
(WangTile) \
(WangEdgeColor) \
(GroundData) \
(TileInfo) \
(TileData) \
(TileFlagProcessor) \
(ComputeNeighbourFlagProcessor) \
(ComputeCustomFlagProcessorEntry)\
(ComputeCustomFlagProcessor)\
(LayerBase) \
(ImageLayer) \
(ObjectLayer) \
(TileLayer) \
(GroupLayer) \
(ManagerObject) \
(ObjectTypeDefinition) \
(ObjectTypeSet) \
(TileSet) \
(TileSetData)\
(Map) \
(Manager)

// forward declaration
#define CHAOS_TILEDMAP_FORWARD_DECL(r, data, elem) class elem;

// friendship macro
#define CHAOS_TILEDMAP_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_TILEDMAP_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_TILEDMAP_FRIEND_DECL, _, CHAOS_TILEDMAP_CLASSES)	

namespace chaos
{
	namespace TiledMap
	{
		BOOST_PP_SEQ_FOR_EACH(CHAOS_TILEDMAP_FORWARD_DECL, _, CHAOS_TILEDMAP_CLASSES);

	}; // namespace TiledMap
		
}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

#include "Chaos/TiledMap/TiledMapBaseObject.h"
#include "Chaos/TiledMap/TiledMapPropertyOwner.h"
#include "Chaos/TiledMap/TiledMapGeometricObject.h"
#include "Chaos/TiledMap/TiledMapLayer.h"
#include "Chaos/TiledMap/TiledMapTileFlagProcessor.h"
#include "Chaos/TiledMap/TiledMapManagerObject.h"
#include "Chaos/TiledMap/TiledMapObjectTypeSet.h"
#include "Chaos/TiledMap/TiledMapTileSet.h"
#include "Chaos/TiledMap/TiledMapMap.h"
#include "Chaos/TiledMap/TiledMapManager.h"