# Lighting-and-Shading-of-OpenGL
Implement Phong Illumination Model in GLSL.
Support for two light sources: one being static and one being dynamic. 
The static one is a directional light with direction (0, -1, -1) in camera coordinate system. 
The dynamic light is a point source rotating around the center of the triangular model, it is initially placed at the location of the camera, (0, 0, 0) in camera coordinate system. 
The user can choose to rotate it around X, Y, or Z-axis of the world coordinate system, the rotation center is fixed. Light source can be turn on and off. 
When rotation is turned on, the light source will automatically rotate around the model. 
When rotation is turned off, the light source will stop rotating and be placed wherever it is. 
A “reset” option can reset the dynamic light to its initial location
