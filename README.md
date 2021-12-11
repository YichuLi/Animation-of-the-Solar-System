# README



> Student name: Li, Yichu
>
> Date: 2020/12/12

## 1. Design outline

By completing this project, I get my familiar with transformations, texture mapping, lighting and window event handling. Then, I will introduce in detail how I completed this animation of the solar system.

### (1) Movement of Planets

In my program, the Sun is stationary and the eight planets is circulate around the sun and self-rotate at constant angular velocities. The Moon circulates around the Earth without self-rotation. That is the process of how I implement it: 

#### a. init()

* At first, I used ***createShaderProgram()*** to combine the vertex shader and fragment shader of planets. 

* Then, I create the perspective matrix according to window's width and height:

  ```c++
  pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);	//1.0472 radians = 60 degrees
  ```

#### b. setupVertices()

* First of all, I used the Sphere class to generate sphere model. At this time, I create VAO and VBOs and active the VAO:

  ```c++
  glGenVertexArrays(numVAOs, vao);
  glBindVertexArray(vao[0]);
  glGenBuffers(numVBOs, vbo);
  ```

* Then, I used ***glBindBuffer()*** and ***glBufferData()*** to copy the vertices' data into the buffer:

  ```c++
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
  ```

  

#### c. display()

* By using ***glClear()***, it can hide surface removal, both a color buffer and depth buffer.
*  Using ***glUseProgram()*** to enable the shader we've created. 
* I can get the locations of the mv_matrix and proj_matrix in the shader by using ***glGetUniformLocation()***
*  ***glm::lookAt()*** is used to get the view matrix, which can be used in every planet's model-view matrix. 
* For building a hierarchical scene, I used C++ STL "stack" class. I used ***push()*** to push the view matrix on the top of the stack. 
* Next to it, I send the perspective matrix's data  from C++ to a uniform by using ***glUniformMatrix4fv()***. 
* After that, I call ***planet()*** to draw the Sun, eight planets and the Moon.
* After every planet has finished being drawn, we call ***pop()*** twice to remove the Sun's model-view matrix and the view matrix from atop the matrix stack.

#### d. planet()

* In this function, every planet will use ***push(mvStack.top())*** at the beginning. 

* In this function, planet can use ***glm::translate()*** to set the location, revolution speed and revolution angle:

  ```c++
  mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime * Orbital_speed) * length, 0.0f, cos((float)currentTime * Orbital_speed) * length));
  ```

* Planet can use ***rotate()*** to change the self-rotate speed and self-rotate angle:

  ```c++
  mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime * self_rotate_speed, glm::vec3(rotateX, rotateY, rotateZ));
  ```

* Planet can use ***scale()*** to change its size:

  ```c++
  mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(size, size, size));
  ```

* Planet can also use ***glUniformMatrix4fv()*** to sent the model-view matrix' data from C++ to uniform mv_matrix or norm_matrix in the corresponding shader. 

* Also, planet can use ***glBindBuffer()***, ***glVertexAttribPointer()*** and ***glEnableVertexAttribArray()***, associate the active VBO with the  vertex attribute in the vertex shader:

  ```c++
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
  glEnableVertexAttribArray(0);
  ```

* Then, when we execute ***glDrawArrays()***, data in the  active VBO will be transmitted to the vertex attribute that has the corresponding layout qualifier, vertPos. It sends the vertices of  the planet to vertex shader. 

* Finally, when everything is finished, I used ***pop()*** to remove its model-view matrix from atop the matrix stack.

* Since I integrated the motion of all the planets into one function, the function will perform different behavior according to the specific planet. We need to note that:

  * Since the Sun is stationary in the whole time, the ***glm::translate()*** don't need to include current time for its revolution. And the ***rotate()*** is not been used.
  * The Sun's model-view matrix need to be retained, because the models of the eight planets and the Moon should base on it. 

  * After the Earth is drawn, its model-view matrix need to be retained, because the models of the Moon should base on it. 

  * The Moon has no self-rotation. Thus, ***rotate()*** is not been used in this fragment. And after the Moon is finished, ***pop()*** should be used twice, the second ***pop()*** is to remove the model-view matrix of the Earth.

### (2) Texture of Planets

The texture of planet is according to its real texture image. That is the process of how I implement it: 

#### a. init()

* Call the ***loadTexture()*** to get the texture image of each planet.

#### b. setupVertices()

* I used ***glBindBuffer()*** and ***glBufferData()*** to copy the texture's coordinates into the buffer:

  ```c++
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
  ```

  

#### c. planet()

* After ***glBindBuffer()***, ***glVertexAttribPointer()*** and ***glEnableVertexAttribArray()***, when executing ***glDrawArrays()***, data in the  active VBO will be transmitted to the vertex attribute that has the layout qualifier, tex_coord. It sends the texture coordinates of  the planet to vertex shader. 
* Then, it uses ***glActiveTexture(GL_TEXTURE0)*** to make the  texture unit 0 active. After that, it uses ***glBindTexture()*** to bind it to our texture object. 

### (3) Lighting

The light source is the Sun, I used the Blinn-Phong shader to get the color of ADS. And then, combine with the texture color to get the final displayed color. That is the process of how I implement it: 

#### a. setupVertices()

* I used ***glBindBuffer()*** and ***glBufferData()*** to copy the vertices' normal into the buffer:

  ```c++
  glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
  glBufferData(GL_ARRAY_BUFFER, pvalues2.size() * 4, &pvalues2[0], GL_STATIC_DRAW);
  ```

#### b. display()

* Using ***glUniformMatrix4fv()*** to copy normal matrix to the corresponding uniform variable. Also, input the current light's position. 

#### c. installLights()

* At the beginning, convert light’s position to view space, and save it in a float array in preparation for sending to the vertex shader. 
* And then, I get the locations of the light and material fields in the shader by using ***glGetUniformLocation()***. 
* Also, set the uniform light and material values in the shader by using ***glProgramUniform4fv()***.

#### d. planet()

* When the Sun is drawing, using ***glm::transpose()*** to inverse transpose of Model-View matrices. 
* Then, allocate ***glUniformMatrix4fv()*** to send the inverse transpose matrix's data from C++ to the norm_matrix uniform. 
* I used ***glBindBuffer()***, ***glVertexAttribPointer()*** and ***glEnableVertexAttribArray()***. The normal value will be transmitted to the vertex attribute that has a layout qualifier with location 2, vertNormal.

### (4) Background

The background of the solar system is a star-sphere, which is a collection of stars located on a surface of a sphere. This sphere has its self-rotate. It is big enough to contain the whole solar system. We create a new function to draw this sphere, called ***starSphere()***. 

#### a. init()

* I used ***createShaderProgram()*** to combine the vertex shader and fragment shader of the star-sphere.

#### b. display()

* Using ***glUseProgram()*** to enable the shader we've created. 
* Using ***glGetUniformLocation()*** to get the locations of the model-view matrix and perspective matrix of the background in the shader.

#### c. starSphere()

* This function is very similar to ***planet()***, except  at the end:

  ```C++
  glEnable(GL_CULL_FACE); //for skybox
  glCullFace(GL_FRONT);   //for skybox
  glFrontFace(GL_CCW);	// not CW, because we are viewing the inside
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
  glEnable(GL_DEPTH_TEST);
  ```

  It uses ***glCullFace(GL_FRONT)*** to disable the texture on the front of the star-sphere. It also disable the depth test before ***glDrawArrays()***, and enable it after.

### (5) 3D transformations

Users can use keyboard 'A', 'D', 'W', 'S', cursor and scroll of mouse to change the view location. 

#### a. keys_callback()

* By using deltaTime to calculate the keyboard press time, and then change the position of camera:

  ```c++
  float deltaTime = float(currentTime - lastTime);
  lastTime = currentTime;
  
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
      cameraPos.y += 1.0f * deltaTime * 10;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
      cameraPos.y -= 1.0f * deltaTime * 10;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
      cameraPos.x -= 1.0f * deltaTime * 10;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
      cameraPos.x += 1.0f * deltaTime * 10;
  }
  ```

  

#### b. glfwSetCursorPosCallback(window, mouse_callback) and mouse_callback()

* According to the location of your mouse cursor and the previous record, it will get a x-offset and a y-offset. Then, it changes its cameraFront:

  ```c++
  yaw += xoffset;
  pitch += yoffset;
  
  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
  ```

  After that, the changing will show at the ***display()*** to change the view matrix: 

  ```c++
  vMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  ```

#### c. glfwSetScrollCallback(window, ScrollCallback) and ScrollCallback()

* When the scroll rolled, it will get a y-offset and cause a changing in the z-axis of camera's position:

  ```c++
  cameraPos.z -= yoffset;
  ```

  

### (6) Shaders

Because the color of star-sphere background cannot be changed by the lighting and the planets can, we need to use two kinds of shaders for the color.

#### a. Vertex shaders 

##### i. For the star-sphere background

In this shader, only three layout qualifier, one for the vertex position, one for texture coordinates, one for binding texture:

```glsl
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 tex_coord;
layout (binding = 0) uniform sampler2D s;
```

The vertex position can be calculated as:

```glsl
gl_Position = proj_matrix * mv_matrix * vec4(vertPos,1.0);
```

The value of tc will be sent to the fragment shader:

```glsl
tc = tex_coord;
```

##### ii. For the Sun, the eight planets and the Moon

Since this shader need to calculate the lighting, except tc, it has other four values need to be sent to the fragment shader:

```glsl
varyingVertPos = (mv_matrix * vec4(vertPos,1.0)).xyz;
varyingLightDir = light.position - varyingVertPos;
varyingNormal = (norm_matrix * vec4(vertNormal,1.0)).xyz;
varyingHalfVector =
    normalize(normalize(varyingLightDir)
              + normalize(-varyingVertPos)).xyz;
```

#### b. Fragment shaders

##### i. For the star-sphere background

The color is fully decided by the texture:

```glsl
color = texture(s,tc);
```

##### ii. For the Sun, the eight planets and the Moon

By using Blinn-Phong shader algorithm, I can calculate the ambient, diffuse and specular respectively:

```glsl
// normalize the light, normal, and view vectors:
vec3 L = normalize(varyingLightDir);
vec3 N = normalize(varyingNormal);
vec3 V = normalize(-varyingVertPos);

// get the angle between the light and surface normal:
float cosTheta = dot(L,N);

// halfway vector varyingHalfVector was computed in the vertex shader,
// and interpolated prior to reaching the fragment shader.
// It is copied into variable H here for convenience later.
vec3 H = normalize(varyingHalfVector);

// get angle between the normal and the halfway vector
float cosPhi = dot(H,N);

// compute ADS contributions (per pixel):
vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta,0.0);
vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi,0.0), material.shininess*3.0);
```

And Then, I combined the lighting and texture and get the final displayed color:

```glsl
fragColor = 0.55 * vec4((ambient + diffuse + specular), 1.0) + 0.45 * texture(s,tc);
```



## 2. My feelings about this project

By completing this project, I have reviewed a lot of knowledge of this class and have some insights on learning methods. 

I was doing this project while reading textbook and watching the power point in the class to review the previous knowledge. I found that, this method is much better than I just read and watch. When I start to code, I can usually find some important knowledge I didn't notice before. I realize that in our subject, we should combine theory and practice, try to do some code exercise to absorb knowledge better.

At the very beginning, when doctor Wong gave us this project, I am a little scared. Because I am not so confident that I can complete this project by myself, since I was not fully grasp the knowledge teacher said in the class. To my surprise, I successfully finish this project in the end. Of curse, the process is not easy, in many cases, I have no idea why the window cannot be displayed as I wish. But I managed to overcome the difficulties and get the result I want. By finishing this project, I feel more confident. In the past, I always afraid of doing a project alone, because I was worried about whether I can finish this project. Now, I feel that if such a project that I was a little afraid of at the beginning can be successfully completed in the end, then what else is impossible? Often difficulty is not as difficult as you think. As long as you can face the difficulties and deal with them carefully, the final result will not be bad.



