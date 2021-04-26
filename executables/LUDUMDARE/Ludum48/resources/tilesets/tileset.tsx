<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.5" tiledversion="1.5.0" name="tileset" tilewidth="128" tileheight="128" tilecount="13" columns="0" objectalignment="center">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="WALL">
  <properties>
   <property name="ObjectType" type="int" value="3"/>
  </properties>
  <image width="32" height="32" source="../sprites/Brick.png"/>
 </tile>
 <tile id="1" type="PlayerStart">
  <properties>
   <property name="BITMAP_NAME" value="PlayerAnim"/>
   <property name="ObjectType" type="int" value="5"/>
   <property name="PARTICLE_CREATION" type="bool" value="false"/>
  </properties>
  <image width="32" height="32" source="../sprites/player.png"/>
 </tile>
 <tile id="7" type="Fire">
  <properties>
   <property name="ObjectType" type="int" value="7"/>
  </properties>
  <image width="32" height="32" source="../sprites/Monster1.png"/>
 </tile>
 <tile id="14" type="BurningFire">
  <image width="128" height="96" source="BurningFire.png"/>
 </tile>
 <tile id="16">
  <image width="128" height="128" source="../sprites/WallBackground.png"/>
 </tile>
 <tile id="32">
  <properties>
   <property name="ObjectType" type="int" value="2"/>
  </properties>
  <image width="32" height="32" source="../sprites/diamond.png"/>
 </tile>
 <tile id="33">
  <properties>
   <property name="ObjectType" type="int" value="4"/>
  </properties>
  <image width="32" height="32" source="../sprites/eponge.png"/>
 </tile>
 <tile id="34">
  <properties>
   <property name="ObjectType" type="int" value="1"/>
  </properties>
  <image width="32" height="32" source="../sprites/stone.png"/>
 </tile>
 <tile id="35">
  <image width="96" height="96" source="../sprites/Gate.png"/>
 </tile>
 <tile id="36">
  <image width="96" height="96" source="../sprites/OpenGate.png"/>
 </tile>
 <tile id="37">
  <properties>
   <property name="ObjectType" type="int" value="8"/>
  </properties>
  <image width="32" height="32" source="../sprites/HardWall.png"/>
 </tile>
 <tile id="38">
  <properties>
   <property name="ObjectType" type="int" value="9"/>
  </properties>
  <image width="32" height="32" source="../sprites/Monster2.png"/>
 </tile>
 <tile id="39">
  <image width="32" height="32" source="../sprites/Smoke.png"/>
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
