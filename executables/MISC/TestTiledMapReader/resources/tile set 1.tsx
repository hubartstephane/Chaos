<?xml version="1.0" encoding="UTF-8"?>
<tileset name="tile_1" tilewidth="32" tileheight="32" spacing="6" margin="5" tilecount="43" columns="5" backgroundcolor="#010000">
 <grid orientation="orthogonal" width="32" height="33"/>
 <image source="brick.jpg" trans="e278ff" width="225" height="225"/>
 <terraintypes>
  <terrain name="groud1" tile="21">
   <properties>
    <property name="toto" type="int" value="15"/>
   </properties>
  </terrain>
 </terraintypes>
 <tile id="21" terrain="0,0,,0">
  <objectgroup draworder="index" opacity="0.5">
   <object id="4" x="0.0795455" y="-4.5625" width="29.4375" height="31.4034">
    <ellipse/>
   </object>
   <object id="5" x="-34.4243" y="31.0083">
    <polygon points="0,0 38.3242,23.5883 56.305,6.4839 0.661577,-14.6994 -0.094511,-15.3151 -0.496183,-14.3723"/>
   </object>
  </objectgroup>
 </tile>
 <tile id="24">
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
 <tile id="30">
  <properties>
   <property name="proprietefloat" type="float" value="33.3"/>
  </properties>
 </tile>
 <tile id="33" type="xxx" probability="7"/>
 <wangsets>
  <wangset name="wangset1" tile="-1">
   <wangedgecolor name="" color="#ff0000" tile="-1" probability="1"/>
   <wangedgecolor name="" color="#00ff00" tile="-1" probability="1"/>
   <wangedgecolor name="" color="#0000ff" tile="-1" probability="1"/>
   <wangedgecolor name="" color="#ff7700" tile="-1" probability="1"/>
   <wangtile tileid="0" wangid="0x1010202"/>
   <wangtile tileid="5" wangid="0x1010202"/>
   <wangtile tileid="10" wangid="0x1010202"/>
   <wangtile tileid="11" wangid="0x1010202"/>
   <wangtile tileid="12" wangid="0x1010202"/>
   <wangtile tileid="13" wangid="0x1010202"/>
   <wangtile tileid="16" wangid="0x1010102"/>
   <wangtile tileid="17" wangid="0x1010102"/>
   <wangtile tileid="18" wangid="0x1010202"/>
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
  <wangset name="wangset2" tile="-1">
   <wangedgecolor name="" color="#ff0000" tile="-1" probability="1"/>
   <wangedgecolor name="" color="#00ff00" tile="-1" probability="1"/>
   <wangtile tileid="0" wangid="0x1010101"/>
  </wangset>
 </wangsets>
</tileset>
