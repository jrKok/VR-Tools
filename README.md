# VR-Tools
A freeware set of tools intended to be used as a plugin in the flight simulator X Plane.

Its purpose is to implement functionnality specific for the VR aspect of the simulator.

It components now enable :
1 -  to view some datarefs in temporary, small sized windows (which can be made persistent if the user requires it).
     Some of the datarefs shown :
     
       -  FPS (1/time between frames)
       
       -  Angle of attack
       
       -  Accelerations (components of the acceleration vector, mainly "g-force"
       
       -  simulated weather parameters (qnh, cloud base, wind direction&speed...)

2 - Filter out commands mapped to VR controlers to not have them triggered inadvertently

3 - view/edit text files (with a virtual keyboard, optionnaly the physical keyboard can also be used).

4 - edit a plane's "vrconfig.txt" file with a graphic interface, logging the view position in 3D space

Since it is now known (begin 2020) that X Plane in its next version will use the Vulkan API for rendering but keep an Open-GL bridge for plugins to render -and plugins not having direct access to vulkan rendering- VR Tools will now be optimized for OpenGL rendering. Until now its rendering was inefficient because every element in its window was redrawn at every frame. Now the window's content will be pre-rendered into a texture (in fact several RGBA int bitmaps directly maintained by the plugin). Those bitmaps will be rendered to an XPlane custom window in a 1:1 fashion with modern OpenGL. This will speed up the rendering speed by quite a factor. Font management is done with freetype. Advanced openGL is implemented via GLEW. This will eventually allow to use bigger fonts if such things are wished by the users.This new rendering might enable also to use more graphic background in the future. Multithreading could also be enabled now that the rendering process is centralized and mostly independant of data processing. 

ETA : end of April 2020 with added functionality to use a stack of text files to speed up switching between related files, linux if glew and freetype will link without too much hassle in my build :). 
