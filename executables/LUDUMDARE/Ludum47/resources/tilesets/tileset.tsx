<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.4" tiledversion="1.4.2" name="tileset" tilewidth="32" tileheight="32" tilecount="1" columns="0" objectalignment="center">
 <grid orientation="orthogonal" width="1" height="1"/>
 <terraintypes>
  <terrain name="Terrain1" tile="0"/>
  <terrain name="Terrain2" tile="25"/>
  <terrain name="Nouveau Terrain" tile="25"/>
  <terrain name="Nouveau Terrain" tile="25"/>
 </terraintypes>
 <tile id="1" type="PlayerStart" terrain="0,0,,">
  <properties>
   <property name="BITMAP_NAME" value="PlayerAnim"/>
   <property name="PARTICLE_CREATION" type="bool" value="false"/>
  </properties>
  <image width="32" height="32" source="../sprites/player.png"/>
 </tile>
 <wangsets>
  <wangset name="CollisionPlatformer" tile="-1">
   <wangedgecolor name="col1" color="#ff0000" tile="-1" probability="1"/>
   <wangedgecolor name="" color="#00ff00" tile="-1" probability="1"/>
   <wangtile tileid="0" wangid="0x1010101"/>
   <wangtile tileid="17" wangid="0x2010201"/>
   <wangtile tileid="18" wangid="0x1020201"/>
   <wangtile tileid="19" wangid="0x2020102"/>
   <wangtile tileid="20" wangid="0x2010202"/>
   <wangtile tileid="21" wangid="0x1020202"/>
   <wangtile tileid="22" wangid="0x2020201"/>
   <wangtile tileid="23" wangid="0x2020202"/>
   <wangtile tileid="24" wangid="0x1010102"/>
   <wangtile tileid="25" wangid="0x2010101"/>
   <wangtile tileid="26" wangid="0x1020101"/>
   <wangtile tileid="27" wangid="0x1010201"/>
   <wangtile tileid="28" wangid="0x2010102"/>
   <wangtile tileid="29" wangid="0x1020102"/>
   <wangtile tileid="30" wangid="0x1010202"/>
   <wangtile tileid="31" wangid="0x2020101"/>
  </wangset>
 </wangsets>
</tileset>
