/*********************************************************************************************
 *
 *
 *                                        Version 2.1
 *
 *                              Directed Light Implement Completed
 *                              Point Light Implement Completed
 *                              Light Rotation Implemented
 *                              Using "phone.in" documents
 *                              implemented interactive changing of Global ambient Correctly
 *                              implemented color coeficient changing
 *                              implemented Shading Model
 *                              Needing Z-buffer
 *
 * ********************************************************************************************/



// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include GLEW
//#include "GL/glew.h"

// Include GLFW
#include <GLFW/glfw3.h>

#include "control.h"
GLFWwindow* window;

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;

#include "shader.h"

/*****************************Structure and How to give values efficiently****************************/
struct v{
    float x,y,z;
    v(float a,float b,float c):x(a),y(b),z(c){}
    v(){}
};

struct Int{
    int x,y,z;
};

struct tris{
    v v0,v1,v2;
    v Norm[3];
    v face_normal;
    GLfloat Color[3];
};
/****************triangle data structure***************/
struct trisPosition{
    v  vec[3];
};

struct trisColor{
    v vec[3];
};

struct trisNorm{
    v vec[3];
};

struct faceNormal{
    float x,y,z;
};


/******************************************************/
int MAX_MATERIAL_COUNT;
int NumTris;
GLenum shadeMode = GL_SMOOTH;

trisPosition* TrisPosition;
trisColor* TrisAmbientColor;
trisColor* TrisDiffuseColor;
trisColor* TrisSpecularColor;
trisNorm* TrisNorm;
faceNormal* face_normal;
float* shineCoeficientValue;
v* coeficientscombined;
glm::vec3 *lightPositionValue;
glm::vec3 globalAmbient = vec3(0.1,0.1,0.1);
glm::vec4 colorCoeficientChange = vec4(0,0,0,0);
void ReadInput(char *FileName);

/*********************************Light Rotation****************************/
float lightRotationAngle = 0;
glm::mat3 zRotationMatrix = glm::mat3(
                                      cos(lightRotationAngle),-sin(lightRotationAngle),0,
                                      sin(lightRotationAngle),cos(lightRotationAngle),0,
                                      0,0,1
                                      );
glm::mat3 yRotationMatrix = glm::mat3(
                                      cos(lightRotationAngle),-sin(lightRotationAngle),0,
                                      0,1,0,
                                      sin(lightRotationAngle),cos(lightRotationAngle),0
                                      );
glm::mat3 xRotationMatrix = glm::mat3(
                                      1,0,0,
                                      cos(lightRotationAngle),-sin(lightRotationAngle),0,
                                      sin(lightRotationAngle),cos(lightRotationAngle),0
                                      );
/************main**************
 * ***************************/

int main(int argc,char **argv)
{
    /* Current Path*/
    char the_path[256];
    
    getcwd(the_path, 255);
    strcat(the_path, "/");
    strcat(the_path, argv[0]);
    printf("Current Path:%s\n", the_path);
    
    
    
    char buf[50]="cow_up.in";
    char *pc=buf;
    /*************************************************Read Input File*************************************************/
    ReadInput(pc);
    /******** Initialise GLFW************************
     * *********************************************/
    GLenum renderType=GL_TRIANGLES;
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    /********Open a window and create its OpenGL context*********
     * **********************************************************/
    window = glfwCreateWindow( 1200, 800, "Object", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    /************** Initialize GLEW******************************
     * *********************************************************/
    if (glfwInit() != 0) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        //return -1;
    }
    
    /********************* Ensure we can capture the escape key being pressed below**************
     * ******************************************************************************************/
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                           // Black background
    /**********************Z-Buffer Enabled*******************************************************/
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);                                           // Accept fragment if it closer to the camera than the former one
    
    ShaderInfo shaders= {
        GL_VERTEX_SHADER,"vertexShader.vert",
        GL_FRAGMENT_SHADER,"fragmentShader.frag",
    };                                                              //link the Shader program for vertex and fragment
    
    /*=============================using LoadShaders.cpp====================================*/
    GLuint programID = LoadShaders(shaders);                        //Create and compile our GLSL program from the shaders
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");       // Get a handle for our "MVP" uniform
    GLuint cameraLocationID = glGetUniformLocation(programID,"cameraLocation");
    GLuint viewDirectionID = glGetUniformLocation(programID,"viewDirection");
    GLuint rotationMatrixID = glGetUniformLocation(programID,"rotationMatrix");
    GLuint lightButtonsID = glGetUniformLocation(programID,"lightButtons");
    GLuint globalAmbientColorID = glGetUniformLocation(programID,"globalAmbient");
    GLuint colorCoeficientChangeID = glGetUniformLocation(programID,"colorCoeficientChange");
    /********************Change Global Ambient Color************************************************/
    cout<<"change global ambient color(Y/N)?"<<endl;
    float R,G,B;
    char temp;
    cin >> temp;
    if(temp == 'Y')
    {
        cout<<"Enter R G B value"<<endl;
        cin>>R>>G>>B;
        globalAmbient[0] = R;
        globalAmbient[1] = G;
        globalAmbient[2] = B;
        
    }
    
    /******************Change Color Coeficients******************************************************/
    cout<<"change color coeficiences?(Y/N)"<<endl;
    char temp2;
    cin >> temp2;
    if(temp2 == 'Y')
    {
        float A,D,S;
        cout<<"Ra Rd Rs"<<endl;
        cin>>A>>D>>S;
        colorCoeficientChange[0] = A;
        colorCoeficientChange[1] = D;
        colorCoeficientChange[2] = S;
        colorCoeficientChange[3] = 1;
    }
    
    /******************Change Shade Mode:GL_FLAT  GL_SMOOTH******************************************/
    cout<<"change Shading Mode to GL_FLAT(Default:GL_SMOOTH)(Y/N)"<<endl;
    char temp3;
    cin>>temp3;
    if(temp3 =='Y')
    {
        shadeMode = GL_FLAT;
    }
    /******************* Get shader variables from shader programs*******************
     *********************************************************************************/
    GLuint vertexPosition = glGetAttribLocation(programID, "vertexPosition");
    GLuint vertexAmbientColor = glGetAttribLocation(programID,"vertexAmbientColor");
    GLuint vertexDiffuseColor = glGetAttribLocation(programID, "vertexDiffuseColor");
    GLuint vertexSpecularColor = glGetAttribLocation(programID,"vertexSpecularColor");
    GLuint vertexNorm = glGetAttribLocation(programID,"vertexNorm");
    GLuint colorCoeficients = glGetAttribLocation(programID,"colorCoeficients");
    GLuint shineCoeficient = glGetAttribLocation(programID,"shineCoeficient");
    GLuint lightPosition = glGetAttribLocation(programID,"pointLightPosition");
    
    // Draw the triangles !
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 9*NumTris*sizeof(TrisPosition[0].vec[0].x), TrisPosition, GL_STATIC_DRAW);
    
    GLuint ambientColorbuffer;
    glGenBuffers(1,&ambientColorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,ambientColorbuffer);
    glBufferData(GL_ARRAY_BUFFER,NumTris*sizeof(TrisAmbientColor[0]),TrisAmbientColor,GL_STATIC_DRAW);
    
    GLuint diffuseColorbuffer;
    glGenBuffers(1, &diffuseColorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, diffuseColorbuffer);
    glBufferData(GL_ARRAY_BUFFER, NumTris*sizeof(TrisDiffuseColor[0]), TrisDiffuseColor, GL_STATIC_DRAW);//watch out!!!!!!!!!!!!!!
    
    GLuint specularColorbuffer;
    glGenBuffers(1,&specularColorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,specularColorbuffer);
    glBufferData(GL_ARRAY_BUFFER,NumTris*sizeof(TrisSpecularColor[0]),TrisSpecularColor,GL_STATIC_DRAW);
    
    GLuint normbuffer;
    glGenBuffers(1,&normbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,normbuffer);
    glBufferData(GL_ARRAY_BUFFER,NumTris*sizeof(TrisNorm[0]),TrisNorm,GL_STATIC_DRAW);
    
    GLuint colorCoeficientsbuffer;
    glGenBuffers(1,&colorCoeficientsbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,colorCoeficientsbuffer);
    glBufferData(GL_ARRAY_BUFFER,3*NumTris*sizeof(coeficientscombined[0]),coeficientscombined,GL_STATIC_DRAW);//??can we just buffer once????
    
    GLuint shineCoeficientbuffer;
    glGenBuffers(1,&shineCoeficientbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,shineCoeficientbuffer);
    glBufferData(GL_ARRAY_BUFFER,3*NumTris*sizeof(shineCoeficientValue[0]),shineCoeficientValue,GL_STATIC_DRAW);
    
    GLuint lightPositionbuffer;
    glGenBuffers(1,&lightPositionbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,lightPositionbuffer);
    glBufferData(GL_ARRAY_BUFFER,9*NumTris*sizeof(lightPositionValue[0]),lightPositionValue,GL_STATIC_DRAW);
    
    /*********************************Main Loop***************************************/
    do{
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use our shader
        glUseProgram(programID);
        
        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        //        glm::mat4 RotationMatrix=getRotationMatrix();
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::vec3 cameraPosition = getCameraPosition();
        glm::vec3 viewDirection = getDirection();
        glm::mat3 rotationMatrix = getRotationMatrix();
        glm::vec2 lightButtons =getlightButtons();
        
        /***********************send MVP Transformation Matrix and cameraLocation to currently bound shader*****************/
        glm::mat4 MVP = ProjectionMatrix *ViewMatrix *  ModelMatrix;
        // in the "MVP" uniform type variable and cameraLocation
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniform3f(cameraLocationID,cameraPosition[0],cameraPosition[1],cameraPosition[2]);
        glUniform3f(viewDirectionID,viewDirection[0],viewDirection[1],viewDirection[2]);
        glUniformMatrix3fv(rotationMatrixID,1,GL_FALSE,&rotationMatrix[0][0]);
        glUniform2f(lightButtonsID,lightButtons[0],lightButtons[1]);
        glUniform3f(globalAmbientColorID,globalAmbient[0],globalAmbient[1],globalAmbient[2]);
        glUniform4f(colorCoeficientChangeID,colorCoeficientChange[0],colorCoeficientChange[1],colorCoeficientChange[2],colorCoeficientChange[3]);
        // 1rst attribute buffer : vertices                                    XXXXXXXXXXXXXXXXXXXXXXXXXX
        glEnableVertexAttribArray(vertexPosition);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              vertexPosition,              // The attribute we want to configure
                              3,                           // size of components to be updated per vertex
                              GL_FLOAT,                    // type
                              GL_FALSE,                    // normalized?
                              0,                           // stride
                              (void*)0                     // array buffer offset
                              );
        
        // 2nd attribute buffer : ambientColor
        glEnableVertexAttribArray(vertexAmbientColor);
        glBindBuffer(GL_ARRAY_BUFFER,ambientColorbuffer);
        glVertexAttribPointer(
                              vertexAmbientColor,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              (void*)0
                              );
        // 3rd attribute buffer : diffuseColor
        glEnableVertexAttribArray(vertexDiffuseColor);
        glBindBuffer(GL_ARRAY_BUFFER, diffuseColorbuffer);
        glVertexAttribPointer(
                              vertexDiffuseColor,                 // The attribute we want to configure
                              3,                           // size of components to be updated per vertex
                              GL_FLOAT,                    // type
                              GL_FALSE,                    // normalized?
                              0,                           // stride
                              (void*)0                     // array buffer offset
                              );
        //4th attribute buffer : specularColor
        glEnableVertexAttribArray(vertexSpecularColor);
        glBindBuffer(GL_ARRAY_BUFFER,specularColorbuffer);
        glVertexAttribPointer(
                              vertexSpecularColor,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              (void*)0
                              );
        //5th attribute buffer : norms
        glEnableVertexAttribArray(vertexNorm);
        glBindBuffer(GL_ARRAY_BUFFER,normbuffer);
        glVertexAttribPointer(
                              vertexNorm,                     //The attribute we want to configure
                              3,                              //size of components to be updated per vertex
                              GL_FLOAT,                       //type
                              GL_FALSE,                       //normalized?
                              0,                              //stride
                              (void*)0                        //array buffer offset
                              );
        //6th attribute buffers : colorCoeficients(Ka,Kd,Ks)
        glEnableVertexAttribArray(colorCoeficients);
        glBindBuffer(GL_ARRAY_BUFFER,colorCoeficientsbuffer);
        glVertexAttribPointer(
                              colorCoeficients,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              (void*)0
                              );
        //7th attribute buffers : shineCoeficients
        glEnableVertexAttribArray(shineCoeficient);
        glBindBuffer(GL_ARRAY_BUFFER,shineCoeficientbuffer);
        glVertexAttribPointer(
                              shineCoeficient,
                              1,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              (void*)0
                              );
        //8th attribute buffers : lightPosition
        glEnableVertexAttribArray(lightPosition);
        glBindBuffer(GL_ARRAY_BUFFER,lightPositionbuffer);
        glVertexAttribPointer(
                              lightPosition,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              (void*)0
                              );
        //************************************CW CCW Model Control************************************/
        glFrontFace(GL_CW);
        
        //************************************
        glShadeModel(shadeMode);
        glDrawArrays(renderType, 0, 3*NumTris); // 12*3 indices starting from 0 -> 36 vertices
        
        glDisableVertexAttribArray(vertexPosition);
        glDisableVertexAttribArray(vertexAmbientColor);
        glDisableVertexAttribArray(vertexDiffuseColor);
        glDisableVertexAttribArray(vertexSpecularColor);
        glDisableVertexAttribArray(vertexNorm);
        glDisableVertexAttribArray(colorCoeficients);
        glDisableVertexAttribArray(shineCoeficient);
        glDisableVertexAttribArray(lightPosition);
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    /*****************************************************************************************/
    /********************************** Cleanup VBO and shader********************************/
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1,&ambientColorbuffer);
    glDeleteBuffers(1, &diffuseColorbuffer);
    glDeleteBuffers(1,&specularColorbuffer);
    glDeleteBuffers(1,&normbuffer);
    glDeleteBuffers(1,&colorCoeficientsbuffer);
    glDeleteBuffers(1,&shineCoeficientbuffer);
    glDeleteBuffers(1,&lightPositionbuffer);
    glDeleteProgram(programID);
    
    
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

/*************************Read meshes' data from input file************************/

void ReadInput(char *FileName)
{
    v   ambient[10],
    diffuse[10],
    specular[10];
    
    float shine[MAX_MATERIAL_COUNT];
    int material_count,color_index[3], i;
    char ch;
    

    
    
    FILE* fp=fopen(FileName,"r");
    if(fp==NULL){
        printf("ERROR:unable to open TriObj[%s]!\n",FileName);
        exit(1);
    }
    
    fscanf(fp,"%c",&ch);
    while(ch!='\n')     //skip the first line - object`s name
        fscanf(fp,"%c",&ch);
    
    fscanf(fp,"# triangles = %d\n",&NumTris);   //read # of triangles
    fscanf(fp,"Material count = %d\n",&material_count); //read material count
    
    for(i=0;i<material_count;i++){
        fscanf(fp, "ambient color %f %f %f\n", &(ambient[i].x), &(ambient[i].y), &(ambient[i].z));
        fscanf(fp, "diffuse color %f %f %f\n", &(diffuse[i].x), &(diffuse[i].y), &(diffuse[i].z));
        fscanf(fp, "specular color %f %f %f\n", &(specular[i].x), &(specular[i].y), &(specular[i].z));
        fscanf(fp, "material shine %f\n", &(shine[i]));
        
    }
    
    fscanf(fp, "%c", &ch);
    while(ch!= '\n')    // skip documentation line
        fscanf(fp, "%c", &ch);
    
    /*************************Allocating triangle**************************/
    
    printf ("Reading in %s (%d triangles). . .\n", FileName, NumTris);
    TrisPosition = new trisPosition[NumTris];
    TrisAmbientColor = new trisColor[NumTris];
    TrisDiffuseColor = new trisColor[NumTris];
    TrisSpecularColor = new trisColor[NumTris];
    TrisNorm = new trisNorm[NumTris];
    face_normal = new faceNormal[NumTris];
    shineCoeficientValue =new float[NumTris*3];
    coeficientscombined = new v[NumTris*3];
    lightPositionValue = new glm::vec3[NumTris*3*3];
    /***********************************************************Set Shine Coeficients*************************/
    for(int i=0;i<NumTris*3;i++)
        shineCoeficientValue[i] = shine[material_count - 1];
    /***********************************************************Set Ka Kd Ks**********************************/
    for(int i=0;i<NumTris*3;i++)
        coeficientscombined[i] = v (0.1,0.3,0.3);  //Ka,Kd,Ks
    /********************Set Light Position and ************************************/
    for(int i=0;i<3*NumTris;i++)
        lightPositionValue[i] = vec3(0,0,-1000);
    /*************************Reading vertex informations******************/
    for (i=0; i<NumTris; i++)
    {
        fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
               &(TrisPosition[i].vec[0].x), &(TrisPosition[i].vec[0].y), &(TrisPosition[i].vec[0].z),
               &(TrisNorm[i].vec[0].x), &(TrisNorm[i].vec[0].y), &(TrisNorm[i].vec[0].z),
               &(color_index[0]));
        fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
               &(TrisPosition[i].vec[1].x), &(TrisPosition[i].vec[1].y), &(TrisPosition[i].vec[1].z),
               &(TrisNorm[i].vec[1].x), &(TrisNorm[i].vec[1].y), &(TrisNorm[i].vec[1].z),
               &(color_index[1]));
        fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
               &(TrisPosition[i].vec[2].x), &(TrisPosition[i].vec[2].y), &(TrisPosition[i].vec[2].z),
               &(TrisNorm[i].vec[2].x), &(TrisNorm[i].vec[2].y), &(TrisNorm[i].vec[2].z),
               &(color_index[2]));
        fscanf(fp, "face normal %f %f %f\n",
               &(face_normal[i].x), &(face_normal[i].y),&(face_normal[i].z));
        /********************************Appoint 3 kinds of colors.RGB separately to each vertex********************/
        TrisAmbientColor[i].vec[0].x = /*(unsigned char)(int)(255**/(ambient[color_index[0]].x);
        TrisAmbientColor[i].vec[0].y = /*(unsigned char)(int)(255**/(ambient[color_index[0]].y);
        TrisAmbientColor[i].vec[0].z = /*(unsigned char)(int)(255**/(ambient[color_index[0]].z);
        TrisAmbientColor[i].vec[1].x = /*(unsigned char)(int)(255**/(ambient[color_index[1]].x);
        TrisAmbientColor[i].vec[1].y = /*(unsigned char)(int)(255**/(ambient[color_index[1]].y);
        TrisAmbientColor[i].vec[1].z = /*(unsigned char)(int)(255**/(ambient[color_index[1]].z);
        TrisAmbientColor[i].vec[2].x = /*(unsigned char)(int)(255**/(ambient[color_index[2]].x);
        TrisAmbientColor[i].vec[2].y = /*(unsigned char)(int)(255**/(ambient[color_index[2]].y);
        TrisAmbientColor[i].vec[2].z = /*(unsigned char)(int)(255**/(ambient[color_index[2]].z);
        
        TrisDiffuseColor[i].vec[0].x = /*(unsigned char)(int)(255**/(diffuse[color_index[0]].x);
        TrisDiffuseColor[i].vec[0].y = /*(unsigned char)(int)(255**/(diffuse[color_index[0]].y);
        TrisDiffuseColor[i].vec[0].z = /*(unsigned char)(int)(255**/(diffuse[color_index[0]].z);
        TrisDiffuseColor[i].vec[1].x = /*(unsigned char)(int)(255**/(diffuse[color_index[1]].x);
        TrisDiffuseColor[i].vec[1].y = /*(unsigned char)(int)(255**/(diffuse[color_index[1]].y);
        TrisDiffuseColor[i].vec[1].z = /*(unsigned char)(int)(255**/(diffuse[color_index[1]].z);
        TrisDiffuseColor[i].vec[2].x = /*(unsigned char)(int)(255**/(diffuse[color_index[2]].x);
        TrisDiffuseColor[i].vec[2].y = /*(unsigned char)(int)(255**/(diffuse[color_index[2]].y);
        TrisDiffuseColor[i].vec[2].z = /*(unsigned char)(int)(255**/(diffuse[color_index[2]].z);
        
        TrisSpecularColor[i].vec[0].x = /*(unsigned char)(int)(255**/(specular[color_index[0]].x);
        TrisSpecularColor[i].vec[0].y = /*(unsigned char)(int)(255**/(specular[color_index[0]].y);
        TrisSpecularColor[i].vec[0].z = /*(unsigned char)(int)(255**/(specular[color_index[0]].z);
        TrisSpecularColor[i].vec[1].x = /*(unsigned char)(int)(255**/(specular[color_index[1]].x);
        TrisSpecularColor[i].vec[1].y = /*(unsigned char)(int)(255**/(specular[color_index[1]].y);
        TrisSpecularColor[i].vec[1].z = /*(unsigned char)(int)(255**/(specular[color_index[1]].z);
        TrisSpecularColor[i].vec[2].x = /*(unsigned char)(int)(255**/(specular[color_index[2]].x);
        TrisSpecularColor[i].vec[2].y = /*(unsigned char)(int)(255**/(specular[color_index[2]].y);
        TrisSpecularColor[i].vec[2].z = /*(unsigned char)(int)(255**/(specular[color_index[2]].z);
        
    }
    fclose(fp);
    
    
}

