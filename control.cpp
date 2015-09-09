// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <stdio.h>

#include <cmath>
#include <iostream>

using namespace glm;
using namespace std;

#include "control.h"

glm::vec3 position = glm::vec3(0,0,1000);
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
/************************************About Point Light Rotation**********************/
glm::mat3 rotationMatrix = mat3(
                                1,0,0,
                                0,1,0,
                                0,0,1
                                );
float rotationAngle = 0;
bool rotationButton = false;
enum {xRotation,yRotation,zRotation};
int rotationStyle = 0;
glm::vec2 lightButtons = vec2(1,1);
/*************************************************************************************/
glm::mat4 getViewMatrix(){
    return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
    return ProjectionMatrix;
}
glm::mat3 getRotationMatrix(){
    return rotationMatrix;
}
glm::vec3 getCameraPosition(){
    return position;
}
glm::vec2 getlightButtons(){
    return lightButtons;
}
// Initial position : on +Z
//glm::vec3 position = glm::vec3( 0, 0, 1000 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float near =1.0f;
float far =3000.0f;

float speed = 100.0f; // 2.0:     3 units / second
float mouseSpeed = 0.01f;

//Rotation angle along Z-axis
float angle=0.0f;
float cos_angle = cos(angle);
float sin_angle = sin(angle);
glm::vec3 direction;
glm::vec3 getDirection(){
    return direction;
}

void computeMatricesFromInputs(){
    /**************************************RotationAngle Increment Automatically****************************/
    if(rotationButton == true){
        if(rotationAngle < 6.28)
            rotationAngle +=0.008;
        else rotationAngle = 0;
    }
    if (rotationStyle == xRotation)
        rotationMatrix = glm::mat3(
                                   1,0,0,
                                   0,cos(rotationAngle),-sin(rotationAngle),
                                   0,sin(rotationAngle),cos(rotationAngle)
                                   );
    else if(rotationStyle == yRotation)
        rotationMatrix = glm::mat3(
                                   cos(rotationAngle),0,-sin(rotationAngle),
                                   0,1,0,
                                   sin(rotationAngle),0,cos(rotationAngle)
                                   );
    else if(rotationStyle == zRotation)
        rotationMatrix = glm::mat3(
                                   cos(rotationAngle),-sin(rotationAngle),0,
                                   sin(rotationAngle),cos(rotationAngle),0,
                                   0,0,1
                                   );
    else rotationMatrix = glm::mat3(
                                    1,0,0,
                                    0,1,0,
                                    0,0,1
                                    );
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    
    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    printf("%f  %f\n",xpos,ypos);
    // Reset mouse position for next frame
    glfwSetCursorPos(window, 1200/2, 800/2);
    
    // Compute new orientation
    horizontalAngle += mouseSpeed *deltaTime* float(1200/2 - xpos );
    verticalAngle   += mouseSpeed *deltaTime* float(800/2 - ypos );
    /*******************Calculate CCS***********************************************/
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    direction = vec3(
                     cos(verticalAngle) * sin(horizontalAngle),
                     sin(verticalAngle),
                     cos(verticalAngle) * cos(horizontalAngle)
                     );
    
    // Right vector
    
    glm::vec3 right = glm::vec3(
                                sin(horizontalAngle - 3.14f/2.0f),
                                sin(angle),              //aaaaaaaaaaaaaaaaaaaaaaaaaaaa   have problem how to figure out correspond right vector for the direction
                                cos(horizontalAngle - 3.14f/2.0f)
                                );
    
    // Up vector
    glm::vec3 up = glm::cross( right, direction );
    
    /****************************************I/O input**********************************************/
    //directedLight control
    if(glfwGetKey(window,GLFW_KEY_0) == GLFW_PRESS){
        lightButtons[0] = !lightButtons[0];
    }
    //pointLight control
    if(glfwGetKey(window,GLFW_KEY_9) == GLFW_PRESS){
        lightButtons[1] = !lightButtons[1];
    }
    //push rotationButton
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        rotationButton = !rotationButton;
    }
    //set rotationStyle along X-axis
    if(glfwGetKey(window,GLFW_KEY_X) == GLFW_PRESS){
        rotationStyle = xRotation;
        rotationAngle = 0;
    }
    //set rotationStyel along Y-axis
    if(glfwGetKey(window,GLFW_KEY_Y) == GLFW_PRESS){
        rotationStyle = yRotation;
        rotationAngle = 0;
    }
    //set rotationStyle along Z-axis
    if(glfwGetKey(window,GLFW_KEY_Z) == GLFW_PRESS){
        rotationStyle = zRotation;
        rotationAngle = 0;
    }
    // Move forward
    if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position -= right * deltaTime * speed;
    }
    //set near and far clipping planes
    if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS){
        near += deltaTime*speed/500;
    }
    if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS){
        near -= deltaTime*speed/500;
    }
    if(glfwGetKey(window,GLFW_KEY_E) == GLFW_PRESS){
        far += deltaTime*speed;
    }
    if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS){
        far -= deltaTime*speed;
    }
    
    //set Rotation angle alone Z axis
    if(glfwGetKey(window,GLFW_KEY_U) == GLFW_PRESS){
        angle +=deltaTime*speed/10.0;
        
    }
    if(glfwGetKey(window,GLFW_KEY_I) == GLFW_PRESS){
        angle -=deltaTime*speed/10.0;
    }
    
    //reset camera position
    if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS){
        
        position = glm::vec3(0,0,1000);
        
        horizontalAngle = 3.14f;
        
        verticalAngle = 0.0f;
        
        near =1.0f;
        
        far=3000.0f;
        
        angle=0.0;
        cos_angle = cos(angle);
        sin_angle = sin(angle);
    }
    
    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
    
    /****************************Projection Matrix**************************************/
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(FoV, 3.0f / 2.0f, near, far);
    /************************set Camera coordinate system*******************************
     * ********************************************************************************/
    
    ViewMatrix       = glm::lookAt(
                                   position,           // Camera is here
                                   position+direction, // and looks here : at the same position, plus "direction"
                                   up                  // Head is up (set to 0,-1,0 to look upside-down)
                                   );
    
    /*******************************angle for Z axis rotation***************************/
    cos_angle = cos(angle);
    sin_angle = sin(angle);
    
    
    //set next frame`s start time
    lastTime = currentTime;
}
