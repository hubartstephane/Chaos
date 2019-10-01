
out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

in vec2 vs_position;
in vec4 vs_color;

in vec3 vs_texcoord0;
in vec3 vs_texcoord1;
in vec2 vs_texcoord2;

// while all PowerZone in all levels have same size we can use
// a constant (among each particle) texture scaling
uniform vec2 texcoord_scale;
uniform vec4 mycolor1;
uniform vec4 mycolor2;

uniform float game_time;
uniform float main_time;

// we use different values for scrolling because upgrade zone goes 
// in the same direction than scrolling and would appear static elsewhere
uniform vec2 scroll_speed;

uniform sampler2D powerup_texture;
uniform sampler2DArray material;

void main()
{

	// get the zone color (BLUE or RED)
	vec3 zone_texcoord;
	zone_texcoord.xy = 0.5 * (vs_texcoord0.xy + vs_texcoord1.xy);
	zone_texcoord.z = vs_texcoord0.z;
	vec4 zone_color = texture(material, zone_texcoord);

	float direction = -((zone_color.b * 2.0) - 1.0);

	// get the chevron-patterned
	vec2 chevron_texcoord = vs_texcoord2 * texcoord_scale;
	chevron_texcoord.y *= direction;
	chevron_texcoord.y -= scroll_speed[int(zone_color.b)] * game_time;
	vec4 chevron_color = texture(powerup_texture, chevron_texcoord);

	// the original texture is RED and WHITE. Green channel is a good indicator to select between mycolor1 & mycolor2
	vec4 alpha = mix(mycolor1, mycolor2, chevron_color.g); 
	output_color = vec4(zone_color.r, zone_color.g, zone_color.b, 1.0) * vec4(1.0, 1.0, 1.0, alpha.a);
}
