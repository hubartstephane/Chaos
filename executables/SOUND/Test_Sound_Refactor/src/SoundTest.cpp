#include "SoundTest.h"

#include <chaos/ReferencedObject.h>
#include <chaos/RenderMaterial.h>

class SpriteLayer
{
public:

protected:

	/** the order of the layer in the manager */
	int order = 0;
	/** the material used to render the layer */
	boost::intrusive_ptr<chaos::RenderMaterial> render_material;
};




class SpriteManager : public chaos::ReferencedObject
{

};