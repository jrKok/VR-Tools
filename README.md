# VR-Tools
A freeware set of tools intended to be used as a plugin in the flight simulator X Plane.

Its purpose is to implement functionnality specific for the VR aspect of the simulator.

It components now enable :
1 -  to view some datarefs in temporary, small sized windows (which can be made persistent if the user requires it).
     Some of the datarefs shown :
       -  FPS (1/time between frames)
       -  Angle of attack
       -  Accelerations (components of the acceleration vector, mainly "g-force"
       -  simulated weather parameters

2 - Filter out commands mapped to VR controlers to not have them triggered inadvertently
3 - view/edit text files (with a virtual keyboard, optionnaly the physical keyboard can also be used).
4 - edit a plane's "vrconfig.txt" file with a graphic interface, logging the view position in 3D space

Since it is now known (begin 2020) that X Plane in its next version will use the Vulkan API for rendering but keep an Open-GL bridge for plugins to render -and plugins not having direct access to vulkan rendering- VR Tools will now be optimized for OpenGL rendering. Until now its rendering was inefficient because every element in its window was redrawn at every frame. Now the window's content will be pre-rendered to a texture (a RGBA int bitmap) and it will be only necesseray to render the texture to the window in a 1:1 fashsion speeding up the rendering speed by quite a factor. Building this texture doesn't require openGL. Only font management is done with freetype with the only hassle being that it has to be included but the plugin is delivered with the freetype.dll since its licence does permit it (credits to the freetype dev team, thanks a lot). This will eventually allow to use bigger fonts if such things are wished by the users.

This new rendering might enable also to use more graphic backgrounds. 

ETA : end of April 2020 with added functionality to use a stack of text files to speed up switching between related files. 
