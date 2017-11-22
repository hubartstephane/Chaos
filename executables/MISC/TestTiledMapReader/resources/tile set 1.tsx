<?xml version="1.0" encoding="UTF-8"?>
<tileset name="tile_1" tilewidth="32" tileheight="32" tilecount="49" columns="7" backgroundcolor="#010000">
 <grid orientation="isometric" width="32" height="32"/>
 <image source="brick.jpg" width="225" height="225"/>
 <terraintypes>
  <terrain name="Nouveau terrain" tile="20"/>
  <terrain name="Nouveau terrain" tile="22"/>
  <terrain name="Nouveau terrain" tile="31"/>
  <terrain name="Nouveau terrain" tile="12"/>
 </terraintypes>
 <tile id="0" terrain=",3,0,0"/>
 <tile id="1" terrain=",0,,0"/>
 <tile id="2" terrain=",0,0,0"/>
 <tile id="3" terrain=",,0,0"/>
 <tile id="7" terrain="0,0,,0"/>
 <tile id="10" terrain="0,,0,0"/>
 <tile id="11" terrain="0,,,"/>
 <tile id="14" terrain="0,0,,"/>
 <tile id="15" terrain=",,0,0"/>
 <tile id="17" terrain="0,0,,0"/>
 <tile id="19">
  <objectgroup draworder="index">
   <properties>
    <property name="prop 1" type="int" value="5"/>
   </properties>
  </objectgroup>
 </tile>
 <tile id="22" terrain="0,0,,"/>
 <tile id="23" terrain="0,0,0,0"/>
 <tile id="24" terrain="0,,0,"/>
 <tile id="26" terrain=",,0,0"/>
 <tile id="27">
  <objectgroup draworder="index">
   <object id="1" x="-2.54545" y="16.3636">
    <polyline points="0,0 14.7273,-14.3636 23.7273,9.54545 -0.272727,0.0909091"/>
   </object>
  </objectgroup>
 </tile>
 <tile id="29" terrain=",0,,0"/>
 <tile id="30" terrain="0,,0,"/>
 <tile id="32" terrain=",0,,0">
  <objectgroup draworder="index">
   <object id="2" x="12" y="-2.36364">
    <ellipse/>
   </object>
   <object id="3" x="-14.0909" y="-15.7273" width="0.181818" height="1.77636e-15">
    <ellipse/>
   </object>
   <object id="5" x="-7.09091" y="13.6364">
    <ellipse/>
   </object>
  </objectgroup>
 </tile>
 <tile id="33" terrain="0,,0,">
  <objectgroup draworder="index">
   <object id="1" x="-32" y="-68" width="88" height="93"/>
  </objectgroup>
 </tile>
 <tile id="36" terrain="0,0,,"/>
 <tile id="37" terrain=",,0,0"/>
 <tile id="39" terrain="0,0,,0"/>
 <tile id="44" terrain="0,0,,"/>
 <tile id="45" terrain="0,0,0,"/>
 <tile id="46" terrain="0,,,"/>
 <wangsets>
  <wangset name="New Wang Set" tile="-1">
   <wangedgecolor name="" color="#ff0000" tile="-1" probability="1"/>
   <wangedgecolor name="" color="#00ff00" tile="-1" probability="1"/>
   <wangtile tileid="16" wangid="0x1010102"/>
   <wangtile tileid="17" wangid="0x1010102"/>
   <wangtile tileid="22" wangid="0x1010102"/>
   <wangtile tileid="23" wangid="0x1010102"/>
   <wangtile tileid="24" wangid="0x1010102"/>
   <wangtile tileid="25" wangid="0x1010102"/>
   <wangtile tileid="26" wangid="0x1010101"/>
   <wangtile tileid="29" wangid="0x1010102"/>
   <wangtile tileid="30" wangid="0x1010102"/>
   <wangtile tileid="31" wangid="0x1010102"/>
   <wangtile tileid="32" wangid="0x1010102"/>
   <wangtile tileid="33" wangid="0x1010102"/>
   <wangtile tileid="35" wangid="0x1010102"/>
   <wangtile tileid="36" wangid="0x1010102"/>
   <wangtile tileid="37" wangid="0x1010102"/>
   <wangtile tileid="38" wangid="0x1010102"/>
   <wangtile tileid="39" wangid="0x1010102"/>
   <wangtile tileid="40" wangid="0x1010102"/>
   <wangtile tileid="41" wangid="0x1010102"/>
   <wangtile tileid="45" wangid="0x1010102"/>
   <wangtile tileid="46" wangid="0x1010102"/>
  </wangset>
 </wangsets>
</tileset>
