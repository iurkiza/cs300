/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
Project: cs300_i.urkiza_1
Author: Ibon Urkiza, i.urkiza, 54000320
Creation date: 15/04/2022
----------------------------------------------------------------------------------------------------------*/

-How to run the program: The program is in x64. When I first execute it It takes time to load it. I dont know why it happens

-How to use the program: 
W: move the camera towards the top of the target.
S: move thecamera towards the bottom of the target.
A: move the camera towards the left rotating around the target.
D: move the camera towards the left rotating around the target.
E: move the camera away from the target.
Q: move the camera closer to the target.
N: Toggle normal rendering
T: Toggle texture-mapping on/off
F: Toggle face/averaged normal
M: Toggle wireframe on/off
O/P: Increase/reduce number of slices (4 is the minimum number of slices)

-Important parts of the code: The cpp which contains most of the functions is Vertex cpp. I have a class called Mesh which contains a transform, handles for the VBO and VAOs a vector of Vertexes(Position, normal and UV) and a vector of normals. The rest of the functions are to Load the meshes, generate the normal vector (averaged, or normal) and 
other helper functions.
-Known issues and problems: The sphere i generated does not look well. Also the averaged normals are wrong.