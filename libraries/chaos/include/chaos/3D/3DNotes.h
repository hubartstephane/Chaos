namespace chaos
{
	// Both OPENGL and VULKAN
	// ----------------------
	// 
	//   - Right Handed
	// 
	//   - The projection matrix is expected to look at -Z direction
	// 
	//
	//                    Y
	//
	//                    ^
	//                    |        view direction (the camera is looking to the -Z direction)
	//                    |     ^
	//                    |    .
	//                    |   .
	//                    |  .
	//                    | .  
	//                    |.
	//                    +------------------> X
	//                   /
	//                  /
	//                 /
	//                /
	//               v
	//              Z
	//
	//  For OpenGL, the device expects the vertex to be clipped inside Z e [-1 .. +1] space after projection
	//  For Vulkan, the device expects the vertex to be clipped inside Z e [ 0 .. +1] space after projection
	// 
	// 
	//                 Z = +1                                                Z = +1
	//           +-----------------+                    	           +-----------------+
	//          /                 /|                                  /                 /|
	//         /     Z = 0       / |                                 /                 / |
	//        +-----------------+  |                                /                 /  |
	//       /                 /   |                               /                 /   |
	//      /     Z = -1      /    |                              /       Z = 0     /    |
	//     +-----------------+     +                             +-----------------+     +         projection space
	//     |                 |    /                              |                 |    /
	//     |                 |   /                               |                 |   /
	//     |                 |  /                                |                 |  /
	//     |                 | /                                 |                 | /
	//     |                 |/                                  |                 |/
	//     +-----------------+                                   +-----------------+     
	//           OPENGL                                                VULKAN
	// 
	// 
	//  - GLM is designed to fit OPENGL conventions, not VULKAN's
	// 
	//    -> glm::frustum(...) 
	//    -> glm::perspectiveFov(...)  are designed to generate a matrix to look at negative Z order and get in projection space with Z e [-1 .. -1]
	//    -> glm::ortho(...)
	//    -> ...
	//
	// you can use macro     GLM_CONFIG_CLIP_CONTROL      to change these functions behaviour
	//
	//                          ->GLM_CLIP_CONTROL_LH_ZO           LH = left-handed       RH = right-handed
	//                          ->GLM_CLIP_CONTROL_LH_NO            
	//                          ->GLM_CLIP_CONTROL_RH_ZO           ZO = Z e [0..+1]       NO = Z e [-1..+1]
	//                          ->GLM_CLIP_CONTROL_RH_NO
	//
	// If you really want a specific behaviour you can use the following functions instead
	//
	//         glm::frustum     | ZO
	//         glm::perspective | NO
	//         glm::ortho       | LH
	//                          | RH
	//                          | LH_NO
	//                            ...
	//         
	// There is another macro that maybe worth looking at: GLM_FORCE_LEFT_HANDED
	//
	// During rasterization, the fragment coordinates Y are different from OPENGL to VULKAN
	//
	//     Y
	//     ^
	//     |
	//     |                             0
	//   1 +-------------+                +-------------+--> X
	//     |             |                |             |
	//     |             |                |             |
	//     |             |                |             |   gl_FragCoord
	//     |             |                |             |
	//     |             |                |             |
	//     +-------------+-->           1 +-------------+
	//    0              1                |
	//         OPENGL                     |    VULKAN
	//                                    v
	//                                    Y
	//
	// It's possible to change OPENGL behavior with glClipControl
	// 
	//    glClipControl(   GL_LOWER_LEFT -> default                 GL_NEGATIVE_ONE_TO_ONE -> default        )
	//                     GL_UPPER_LEFT -> like vulkan             GL_ZERO_TO_ONE         -> like vulkan
	//
	// There is no equivalence inside Vulkan API

}; // namespace chaos