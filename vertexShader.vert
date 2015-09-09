#version 120
/*********************************variables will fetch data from main.cpp*************************************/
attribute vec3 vertexPosition;              //something wrong body does not display,now seems fixed
attribute vec3 vertexAmbientColor;          //global Ambient Light
attribute vec3 vertexDiffuseColor;
attribute vec3 vertexSpecularColor;
attribute vec3 vertexNorm;
attribute vec3 colorCoeficients;           //(Ka,Kd,Ks) working correct
attribute float shineCoeficient;             //working correct
attribute vec3 pointLightPosition;              //something wrong always red,seems fixed by change buffer data size
//attribute vec3 viewPosition;         //
/*********************************variables will be send to fragmentShader************************************/
varying vec3 fragmentColor;

/*********************************variables will be modified by main.cpp**************************************/
uniform mat4 MVP;
uniform vec3 cameraLocation;             //seems right
uniform vec3 viewDirection;
uniform mat3 rotationMatrix;
uniform vec2 lightButtons;             //directed,point
uniform vec3 globalAmbient;
uniform vec4 colorCoeficientChange;
/*************************************************************************************************************/
void main(){
    float Ka,Kd,Ks;//unvalued
    vec3 directedLightIntensity,directedLighthalfVector,directedLightDirection;
    vec3 pointLightIntensity,pointLightDirection,pointLightPositionTemp;
    float distanceToPointLight;
    vec3 RGBcolor = vec3 (1.0,0.6,0.6);
    float directedLightButton,pointLightButton;
    /**************************Assign Coeficients to 3 different colors********************************************/
    if(colorCoeficientChange[3]!=0)
    {
        Ka = colorCoeficientChange[0];
        Kd = colorCoeficientChange[1];
        Ks = colorCoeficientChange[2];
    }
    else
    {
        Ka = colorCoeficients[0];
        Kd = colorCoeficients[1];
        Ks = colorCoeficients[2];
    }
    /**************************Assign light on/off control******************************************************/
    directedLightButton = lightButtons[0];
    pointLightButton = lightButtons[1];
    /*********************************send vertex/per to GPU via gl_Position**************************************/
    gl_Position =  MVP * vec4(vertexPosition,1);
    
    
    //        viewDirection = normalize(cameraLocation - vertexPosition);
    directedLightDirection=normalize(viewDirection*vec3(0,-1,-1) );
    distanceToPointLight = length(vertexPosition - pointLightPosition);              //big problem
    directedLighthalfVector = normalize(directedLightDirection+viewDirection);
    
    pointLightPositionTemp = rotationMatrix * pointLightPosition;
    pointLightDirection = normalize(vertexPosition-pointLightPositionTemp );
    
    /*********************************Phong Illumination Model Calculation****************************************/
    directedLightIntensity = (Kd*vertexDiffuseColor*dot(vertexNorm,directedLightDirection)+Ks*vertexSpecularColor*pow(dot(vertexNorm,directedLighthalfVector),shineCoeficient));   //unfinished
    pointLightIntensity = ((Kd*vertexDiffuseColor*dot(vertexNorm,pointLightDirection))+Ks*vertexSpecularColor*pow(dot(vertexNorm,directedLighthalfVector),shineCoeficient))/(distanceToPointLight*distanceToPointLight/1700000);
    //fragmentColor = PointLightPosition/1000;
    // if(directedLightButton != 0)
    
    
    fragmentColor = min(vec3(1.0),0.7*directedLightIntensity*directedLightButton+0.4*pointLightIntensity*pointLightButton+Ka*vertexAmbientColor+globalAmbient);
    //      else fragmentColor = vec3(0,1,0);
    
}
