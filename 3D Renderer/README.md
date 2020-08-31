# Information
	I have been working on this project in free time since summer 2019. Most of my 
projects before this one have been small 2D games so I wanted to branch out and 
learn what I could about 3D graphics. It is written in C/C++ with rendering in 
OpenGL, windowing/input using SDL and SoLoud for the audio library. In the scene 
above I was going for a spooky space station kind of vibe.
	I started with Blinn–Phong shading model and recently converted to a 
physically-based implementation. The models for the 3D scene were part of a 
kitbash set that I assembled into different scenes in blender. I used Assimp 
to export all the models into the several scene parts so that I could occlude 
them. The soft shadows are sampled from a jitter map and there is support for 
dynamic and static directional/spot lights. The scene has a reflection probe 
in the center that is used to sample colors for the diffuse and specular IBL. 
There is also 4x MSAA, HDR and bloom.
	The AI is A* where the nodes are created from a binary file. Animations 
can be loaded in from blender, the enemy was created and rigged by an artist 
and loaded directly in with little modification. The view point is primarily
a first-person camera but there is also support for a more cinematic spline 
camera that follows a list of points. There is some support for python scripting. 
Right now the audio just has support for 2D SFX and BGM but i plan to implement 
3D audio. This project has morphed over time and is more like an engine. When I 
find something I am interested in I find that the best way to help me understand 
it is to implement it.