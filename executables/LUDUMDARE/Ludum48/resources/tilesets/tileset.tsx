<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.5" tiledversion="1.5.0" name="tileset" tilewidth="128" tileheight="128" tilecount="33" columns="0" objectalignment="center">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="WALL">
  <image width="32" height="32" source="../sprites/Brick.png"/>
 </tile>
 <tile id="1" type="PlayerStart">
  <properties>
   <property name="BITMAP_NAME" value="PlayerAnim"/>
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
   <property name="PARTICLE_CREATION" type="bool" value="false"/>
   <property name="TRIGGER_TYPE" value="BurnTrigger"/>
  </properties>
  <image width="64" height="64" source="../sprites/SoulBurnTrigger.png"/>
 </tile>
 <tile id="14" type="BurningFire">
  <image width="128" height="96" source="BurningFire.png"/>
 </tile>
 <tile id="15">
  <image width="64" height="64" source="Banner.png"/>
 </tile>
 <tile id="16">
  <image width="128" height="128" source="../sprites/WallBackground.png"/>
 </tile>
 <tile id="17" type="WALL">
  <image width="32" height="32" source="../sprites/Brick9.png"/>
 </tile>
 <tile id="18" type="WALL">
  <image width="32" height="32" source="../sprites/Brick10.png"/>
 </tile>
 <tile id="19" type="WALL">
  <image width="32" height="32" source="../sprites/Brick11.png"/>
 </tile>
 <tile id="20" type="WALL">
  <image width="32" height="32" source="../sprites/Brick12.png"/>
 </tile>
 <tile id="21" type="WALL">
  <properties>
   <property name="OBJECT_REFERENCE" type="object" value="0"/>
  </properties>
  <image width="32" height="32" source="../sprites/Brick13.png"/>
 </tile>
 <tile id="22" type="WALL">
  <image width="32" height="32" source="../sprites/Brick14.png"/>
 </tile>
 <tile id="23" type="WALL">
  <image width="32" height="32" source="../sprites/Brick15.png"/>
 </tile>
 <tile id="24" type="WALL">
  <image width="32" height="32" source="../sprites/Brick1.png"/>
 </tile>
 <tile id="25" type="WALL">
  <image width="32" height="32" source="../sprites/Brick2.png"/>
 </tile>
 <tile id="26" type="WALL">
  <image width="32" height="32" source="../sprites/Brick3.png"/>
 </tile>
 <tile id="27" type="WALL">
  <image width="32" height="32" source="../sprites/Brick4.png"/>
 </tile>
 <tile id="28" type="WALL">
  <image width="32" height="32" source="../sprites/Brick5.png"/>
 </tile>
 <tile id="29" type="WALL">
  <image width="32" height="32" source="../sprites/Brick6.png"/>
 </tile>
 <tile id="30" type="WALL">
  <image width="32" height="32" source="../sprites/Brick7.png"/>
 </tile>
 <tile id="31" type="WALL">
  <image width="32" height="32" source="../sprites/Brick8.png"/>
 </tile>
 <tile id="32">
  <image width="32" height="32" source="../../../Ludum48/resources/sprites/diamond.png"/>
 </tile>
 <tile id="33">
  <image width="32" height="32" source="../../../Ludum48/resources/sprites/eponge.png"/>
 </tile>
 <tile id="34">
  <image width="32" height="32" source="../../../Ludum48/resources/sprites/stone.png"/>
 </tile>
 <wangsets>
  <wangset name="Terrains" type="corner" tile="-1">
   <wangcolor name="Terrain1" color="#ff0000" tile="0" probability="1"/>
   <wangcolor name="Terrain2" color="#00ff00" tile="25" probability="1"/>
   <wangcolor name="Nouveau Terrain" color="#0000ff" tile="25" probability="1"/>
   <wangcolor name="Nouveau Terrain" color="#ff7700" tile="25" probability="1"/>
   <wangtile tileid="0" wangid="0,1,0,1,0,1,0,1"/>
   <wangtile tileid="1" wangid="0,1,0,0,0,0,0,1"/>
   <wangtile tileid="3" wangid="0,2,0,3,0,2,0,2"/>
   <wangtile tileid="8" wangid="0,0,0,2,0,0,0,0"/>
  </wangset>
  <wangset name="CollisionPlatformer" type="edge" tile="-1">
   <wangcolor name="col1" color="#ff0000" tile="-1" probability="1"/>
   <wangcolor name="" color="#00ff00" tile="-1" probability="1"/>
   <wangtile tileid="0" wangid="1,0,1,0,1,0,1,0"/>
   <wangtile tileid="17" wangid="1,0,2,0,1,0,2,0"/>
   <wangtile tileid="18" wangid="1,0,2,0,2,0,1,0"/>
   <wangtile tileid="19" wangid="2,0,1,0,2,0,2,0"/>
   <wangtile tileid="20" wangid="2,0,2,0,1,0,2,0"/>
   <wangtile tileid="21" wangid="2,0,2,0,2,0,1,0"/>
   <wangtile tileid="22" wangid="1,0,2,0,2,0,2,0"/>
   <wangtile tileid="23" wangid="2,0,2,0,2,0,2,0"/>
   <wangtile tileid="24" wangid="2,0,1,0,1,0,1,0"/>
   <wangtile tileid="25" wangid="1,0,1,0,1,0,2,0"/>
   <wangtile tileid="26" wangid="1,0,1,0,2,0,1,0"/>
   <wangtile tileid="27" wangid="1,0,2,0,1,0,1,0"/>
   <wangtile tileid="28" wangid="2,0,1,0,1,0,2,0"/>
   <wangtile tileid="29" wangid="2,0,1,0,2,0,1,0"/>
   <wangtile tileid="30" wangid="2,0,2,0,1,0,1,0"/>
   <wangtile tileid="31" wangid="1,0,1,0,2,0,2,0"/>
  </wangset>
 </wangsets>
</tileset>
