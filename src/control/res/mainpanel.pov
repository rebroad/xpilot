// Persistence of Vision Ray Tracer Scene Description File
// File: mainpanel.pov
// Vers: 3.5
// Desc: for creating the xpilot5 main panel buttons
// Date: 20040224
// Auth: Dick Balaska
//  ©  : 2004 BuckoSoft, Inc.  All rights reserved.
//
// $Id: mainpanel.pov,v 1.3 2004/05/10 12:42:22 dick Exp $
// 
// $Log: mainpanel.pov,v $
// Revision 1.3  2004/05/10 12:42:22  dick
// Add a 5th button (private server), so the others get shuffled a bit.
//
// Revision 1.2  2004/02/28 07:13:09  dick
// Build a panel of Up, Down, or Disabled buttons.
// Build the text and icon into the button.
//
// Revision 1.1  2004/02/26 03:33:25  dick
// A povray file for making buttons
//

#include "colors.inc"
#include "textures.inc"
#include "metals.inc"
#include "glass.inc"
#include "stones.inc"

#declare True=1;
#declare False=0;
#declare feet=12;
#declare foot=12;
#declare inches=1;
#declare inch=1;
#declare pi2=(pi*2);

///////////////////////////////////////////////////////////////////////////////
// Select one of the 3 types to make; Up, Down, or X-disabled
#declare bsU=0;
#declare bsD=1;
#declare bsX=0;
///////////////////////////////////////////////////////////////////////////////

#if (bsX = 1)
	#declare ambientMul			=0.6;
	#declare ambientTextMul	=0.5;
	#declare ambientIconMul	=0.24;
#else
	#declare ambientMul			=1;
	#declare ambientTextMul	=1;
	#declare ambientIconMul	=1;
#end

// 223x566
// setup to be rendered at 223x566
	camera {
		location <0.0*inches,0*inches,-12.9*inches>
		right (223/566)*x
		look_at  <0.0*inches,0,0>
	}

global_settings {
		ambient_light rgb 1
		assumed_gamma 1.70
}

///////////////////////////////////////////////////////////////////////////////
#if (bsX != 1)
	light_source { < -270*inches, 60*inches, -400*inches > color rgb 1}
#else
	light_source { < -270*inches, 60*inches, -400*inches > color rgb 1}
#end
//light_source { < 270*inches, 60*inches, -400*inches > color rgb 1}
//light_source { < -270*inches, -40*inches, -450*inches > color rgb 1.5}
//light_source { < 0*inches, 3*inches, -150*inches > color rgb 1.5}

		background { Black }

#if (0)
	#include "skies.inc"
	sky_sphere { 
		S_Cloud5
		rotate y * 45
		//rotate y*(clock*360)	// rotate once every 30 seconds
	}
#end

#if (0)
sphere {
	<0,1.5,0> 1
	texture {Polished_Chrome}
}
#end

#declare xpblue = rgb<78/256,124/256,255/256>;

///////////////////////////////////////////////////////////////////////////////
#declare TextTexture=
texture {
	pigment {xpblue*0.8}
	finish {ambient ambientTextMul}
}

///////////////////////////////////////////////////////////////////////////////

#declare buttT0=<0,0,0>;
#declare buttV0=<0,5.0,0>;
#declare buttV1=<0,4.0,0>;
#declare buttV2=<0,3,0>;
#declare buttV3=<0,2.0,0>;
#declare buttV4=<0,0.0,0>;

#declare Round=0.5;
#declare Rounded_Box = 
superellipsoid { 
	<Round, Round> 
	scale <2.0,0.7,0.5>
};

#declare ButtonBox=
object {
	Rounded_Box
	#if (bsX != 1)
		texture {Polished_Chrome}
	#else
		//texture {pigment {Gray70}}
		material {M_Glass}
	#end
	//material {M_Glass}
}

#macro Button(_v, _icon, _text0, _text1)
union {
#if (1)
	object {
		ButtonBox
	}
#end
	#local _xofs=0.1;
	#local _row0=-0.14;
	#if (strlen(_text1) != 0)
		#local _row0=0.10;
	#end
	intersection {
		text {
			ttf
			"arial.ttf",
			_text0,
			1.5,
			0
			scale 0.4
			translate <_xofs,_row0,-1>
		}
		object {
			Rounded_Box
		}
		translate z*-0.01
		texture {TextTexture}
	}
	#if (strlen(_text1) != 0)
		intersection {
			text {
				ttf
				"arial.ttf",
				_text1,
				1.5,
				0
				scale 0.4
				translate <_xofs,_row0-0.4,-1>
			}
			object {
				Rounded_Box
			}
			translate z*-0.01
			texture {TextTexture}
		}
	#end
	box {
	  <-1, -1, -1>  // one corner position <X1 Y1 Z1>
	  < 1,  1,  1>  // other corner position <X2 Y2 Z2>
		texture {
			pigment {
				image_map {
				png concat("../buttons/", _icon, ".png")
				map_type 0
				interpolate 2     // 0=none, 1=linear, 2=bilinear, 4=normalized distance
				 once           // for no repetitive tiling
				// [filter N V]   // N=all or color index # (0...N), V= value (0.0...1.0)
				// [transmit N V] // N=all or color index # (0...N), V= value (0.0...1.0)
				// [use_color | use_index]
				transmit 0, 1.0
				}
			}
			finish { ambient ambientIconMul}
		}
		scale 0.7
		translate <-1.1,-0.3,0.2>
	}
//	translate z*-0.1
	translate <_v.x,_v.y*1.6,_v.z> + <-0.3,-2.7,0+bsD*0.4>
}
#end
		
Button(buttV0, "xpilots", "Local", "Server")
Button(buttV1, "xpwhere", "Internet", "Server")
Button(buttV2, "nethood", "Lan", "Server")
Button(buttV3, "nethood1", "Private", "Server")
Button(buttV4, "xpilotpencil", "Editors", "")


#if (1)
difference {
	// create a box that extends between the 2 specified points
	box {
	  <-10, -10, 0>  // one corner position <X1 Y1 Z1>
	  < 10,  20, 10>  // other corner position <X2 Y2 Z2>
	}

/*	object {
		Rounded_Box
		scale <2.0,0.5,0.7>
		translate <-1.4,0,5.5>
	}
	object {
		Rounded_Box
		scale <2.0,0.5,0.7>
		translate <-1.4,0,2.5>
	}

	object {
		Rounded_Box
		scale <2.0,0.7,0.5>
		translate <0,1,0.01>
	}
*/
			texture { 
				Apocalypse 
				finish { ambient 1 }
				scale 2
				translate <-1.5,2,0>
				}

}
#end
