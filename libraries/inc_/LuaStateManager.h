#ifndef CHAOS_LUASTATEMANAGER_H
#define CHAOS_LUASTATEMANAGER_H

#include <map>
#include <vector>
#include <assert.h>

#include <chaos/Buffer.h>
#include <chaos/LuaTools.h>
#include <chaos/SparseWriteBuffer.h>

namespace chaos
{

// ==============================================================================================
// LuaStateProviderConfig : an object to configure LuaStateManager
// ==============================================================================================

class LuaStateManagerConfig
{
public:

    /** the constructor */
    LuaStateManagerConfig() :
      bCacheStates(true)
      {
      }
    
public:

    /** indicates whether state must be encoded into a binary buffer an cached for later use */
    bool bCacheStates;
};


// ==============================================================================================
// LuaStateManager : an object to generate LUA states (from cache or note)
// ==============================================================================================

class LuaStateManager
{

public:

    /** the ID for empty state */
    static int const STATE_NONE_ID = -1;
    /** the ID for state with a basic initialization */
    static int const STATE_BASE_ID =  0;

    /** the reserved buffer size when caching states */
    static size_t const ENCODING_BUFFER_SIZE = 2048;

    /** the defines the type of data used for caching */
    typedef UnsafeBuffer<char> StateCache;

    /** a Map that make a correspondance between a State ID and a cache */
    typedef std::map<int, StateCache> StateCacheMap;

    /** a Map, that make relation between State (inherit of) */
    typedef std::map<int, int> StateInheritanceMap;

    /** the constructor */
    LuaStateManager();

    /** the destructor */
    ~LuaStateManager();

    /** Destroy all caches */
    void ClearCaches();

    /** a getters on the configuration */
    LuaStateManagerConfig & GetConfiguration(){ return config;}
    /** a getters on the configuration */
    LuaStateManagerConfig const & GetConfiguration() const { return config;}

    /** return the inheritance for a given state type */
    int GetStateInheritance(int StateID) const;

    /** Create a state of a given Type */
    lua_State * CreateState(int StateID = STATE_BASE_ID);

    /** declare en dependancy between 2 states */
    bool DeclareStateInheritance(int StateID, int ParentStateID);

protected:

    /** restore a state from a buffer */
    bool RestoreStateFromCache(lua_State * L, StateCache const & buffer);

    /** store the state into a buffer */
    bool StoreStateIntoCache(lua_State * L, StateCache & buffer);

    /** the recursive initialization of a state */
    bool RecursiveInitializeState(lua_State * L, int StateID);

    /** the initialization of the default state */
    static bool DoInitializeBaseState(lua_State * L);

    /** an initialization method that depends on the requested StateType */
    static bool DoInitializeState(lua_State *L, int StateID);

protected:

    /** the configuration of the provider */
    LuaStateManagerConfig config;
    /** a tree that contains a buffer for states */
    StateCacheMap state_caches;
    /** a map to make relation between state types */
    StateInheritanceMap inheritance_map;
    /** utility buffer for consequent caching */
    SparseWriteBuffer<> encode_buffer;
};

}; // namespace chaos

#endif // CHAOS_LUASTATEMANAGER_H