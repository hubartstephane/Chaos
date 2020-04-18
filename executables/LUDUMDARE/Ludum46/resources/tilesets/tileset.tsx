<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.3.0" name="tileset" tilewidth="80" tileheight="80" tilecount="12" columns="0">
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
 <tile id="6">
  <image width="40" height="80" source="../sprites/Rectangular.png"/>
 </tile>
 <tile id="7" type="Fire">
  <image width="32" height="32" source="../sprites/fire.png"/>
 </tile>
 <tile id="8" type="Spawner">
  <properties>
   <property name="PARTICLE_START_VELOCITY_Y" type="float" value="-100"/>
   <property name="SPAWNED_PARTICLE" value="Soul"/>
   <property name="SPAWNER_TYPE" value="SoulSpawner"/>
   <property name="TARGET_LAYER" value="Souls"/>
  </properties>
  <image width="64" height="64" source="../sprites/SoulSpawner.png"/>
 </tile>
 <tile id="9" type="Spawner">
  <properties>
   <property name="PARTICLE_DURATION" type="float" value="6"/>
   <property name="PARTICLE_START_VELOCITY_X" type="float" value="100"/>
   <property name="SPAWNED_PARTICLE" value="Fire"/>
   <property name="SPAWNER_TYPE" value="FireSpawner"/>
   <property name="TARGET_LAYER" value="Fire"/>
  </properties>
  <image width="80" height="80" source="../sprites/FireSpawner.png"/>
 </tile>
 <tile id="10" type="SoulTrigger">
  <image width="32" height="32" source="../sprites/Trigger.png"/>
 </tile>
 <tile id="11" type="SpikeBar">
  <image width="64" height="64" source="../sprites/SpikeBar.png"/>
 </tile>
 <tile id="12" type="SoulTrigger">
  <properties>
   <property name="TRIGGER_TYPE" value="BurnTrigger"/>
  </properties>
  <image width="64" height="64" source="../sprites/SoulBurnTrigger.png"/>
 </tile>
</tileset>
