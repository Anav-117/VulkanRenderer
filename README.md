# VulkanRenderer
A Vulkan Renderer that aims to provide users with a way to render and interact with 3D models using multiple shaders, allowing users to move both the model and the camera to view their model from different viewpoints.

The renderer allows users to load any arbitrary number of models (specified in a text file at runtime). 6 shaders provide distinct styles of viewing the model. These shaders are - 
Phong lighting
Cel shading
Gooch shading
Flat shading
“Exploded” view
Wireframe view

Credits

The following additional libraries were used to perform certain tasks inside the program:
GLFW - for window creation and input handling
GLM - for matrix and vector mathematics
Chrono - for timekeeping
irrKlang - for sound
The models showcased in the program are - 
Game-ready food asset pack - by Meerschaum Digital (https://sketchfab.com/3d-models/game-ready-food-asset-pack-822fa185ff9d492ca5999dd4df18e0d0)
Baked Goods - by Rigsters (https://sketchfab.com/3d-models/baked-goods-d1ae09e3cb8343bc8790b15928452906)
