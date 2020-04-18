<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.3.0" name="tileset" tilewidth="80" tileheight="80" tilecount="8" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="WALL">
  <image width="32" height="32" source="../sprites/Brick.png"/>
 </tile>
 <tile id="1" type="PlayerStart">
  <properties>
   <property name="BITMAP_NAME" value="Player"/>
   <property name="PARTICLE_CREATION" type="bool" value="false"/>
  </properties>
  <image width="32" height="32" source="../sprites/player.png"/>
 </tile>
 <tile id="2" type="Spike">
  <image width="32" height="32" source="../sprites/spike.png"/>
 </tile>
 <tile id="3" type="LADDER">
  <image width="32" height="32" source="../sprites/Ladder.png"/>
 </tile>
 <tile id="4" type="BRIDGE">
  <image width="32" height="32" source="../sprites/Bridge.png"/>
 </tile>
 <tile id="5" type="Spawner">
  <properties>
   <property name="PARTICLE_CREATION" type="bool" value="false"/>
   <property name="PARTICLE_DURATION" type="float" value="-1"/>
   <property name="PARTICLE_START_VELOCITY_X" type="float" value="500"/>
   <property name="SPAWNED_PARTICLE" value="Fire"/>
   <property name="SPAWN_PER_SECOND" type="float" value="5"/>
   <property name="TARGET_LAYER" value="Fire"/>
  </properties>
  <image width="80" height="80" source="../sprites/Bigger.png"/>
 </tile>
 <tile id="6">
  <image width="40" height="80" source="../sprites/Rectangular.png"/>
 </tile>
 <tile id="7" type="Fire">
  <image width="32" height="32" source="../sprites/fire.png"/>
 </tile>
</tileset>
