# Instruction

This repository is my final project for ECE 6122 Advanced Programming Techniques for Engineering Applications at Georgia Tech in Fall 2023. It is a C++ application that uses a custom classes with OpenGL and a third-party library to load and display animated 3D objects in OBJ files. This program load and render the objects in the 3D screen. The animated 3D objects are frozen in place until the user presses the “g” key and then the objects start moving around at random speeds and rotating randomly about an axis. The objects are able to collide and bounce off each other. The objects are also confined to the space around the center of the scene and cannot just float off into space.

"model_loading.cpp" is the main application file. "1.model_loading.vs" is the  vertex shader. "1.model_loading.fs" is the fragment shader. "camera.h" is the header file for camera class. "model.h" is the header file for model class. "mesh.h" is the header file for mesh class. "shader_m.h" and "shader.h" are the header files for shader class. "pumpkin.png" and "container.jpg" are the texture files. "pumpkin_sphere.obj" and "pumpkin_sphere.mtl" are the 3D model files. The model file is downloaded from this website: https://www.turbosquid.com/. "FinalProjectVideo.mp4" is a video of the application running.

The project refers to the tutorial website: https://learnopengl.com/. I'd like to thank this tutorial for being an enormous help on the project. The easiest way to run the program is to download the source code from this website (https://github.com/JoeyDeVries/LearnOpenGL) and replace the corresponding files. The paths of the model files and texture files should be replaced with your own file path before running the program. 

This application has the following features: 

1. Create an oversized floor with a textured image. 
2. The four objects are rendered correctly with lighting and material properties. Have
   some general ambient and diffuse lighting effects.
3. The four objects do not move and only start to move when the user presses the “g”
   key.
4. The four objects start to move and rotate randomly around the area. The object
   shall collide and bounce off each other and the floor.
5. Each of the four 3D objects has an internal light that randomly changes intensity.
6. The camera view should always point towards the center of the scene.
7. Pressing the up/down arrow keys should zoom in and out.
8. Pressing the left/right arrow keys rotate either the camera view or the model left
   and right.
9. Pressing the “u” and “d” keys causes the camera to rotate up or down. Pressing
   escape key ends the application.
10. Add extra static objects to give the scene depth.

I also uploaded a video of the application running and demonstrated the features above.