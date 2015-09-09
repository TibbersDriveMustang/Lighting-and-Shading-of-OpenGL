// ---------------------------------------------------------------------------
// LoadShader.cpp
// Quick and dirty LoadShader function for the OpenGL Programming Guide 4.3
// Red Book.
//
// Author: Qoheleth
// http://www.opengl.org/discussion_boards/showthread.php/180175-Redbook-8th-sample-code?p=1245842#post1245842
// ---------------------------------------------------------------------------
#include "shader.h"

GLuint LoadShaders( ShaderInfo shaderInfo )
{
    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;
    vertexShader   = glCreateShader( GL_VERTEX_SHADER );	// create a vertex shader object
    fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );	// create a fragment shader object
    
    // load and compile vertex shader
    string shaderProgramText;
    const char* text = getShaderProgram( shaderInfo.vShaderFile, shaderProgramText );
    GLint length = shaderProgramText.size();
    glShaderSource( vertexShader, 1, &text, NULL );//associate the source code of the shader created by glCreateShader()
    glCompileShader( vertexShader );//compiles the source code for "vertexShader",the results of the compilation can be
    //queried by calling glGetShaderiv() with an argument of GL_COMPILE_STATUS
    
    for ( int i = 0; i < length; i++ )
    {
        cout << text[ i ];
    }
    
    GLint status;
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &status );
    
    if ( !( status == GL_TRUE ) )
        cerr << "\nVertex Shader compilation failed..." << '\n';
    
    char *infoLog = new char[ 100 ];
    GLsizei bufSize = 100;
    glGetShaderInfoLog( vertexShader, bufSize, NULL, infoLog );
    for ( int i = 0; i < bufSize; i++ )
        cout << infoLog[ i ];
    delete [] infoLog;
    
    // load and compile fragment shader
    shaderProgramText = "";
    text = getShaderProgram( shaderInfo.fShaderFile, shaderProgramText );
    glShaderSource( fragmentShader, 1, &text, NULL );
    glCompileShader( fragmentShader );
    length=shaderProgramText.size();
    for(int i=0;i<length;i++)
    {
        cout<<text[i];
    }
    
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &status );//with an argument of GL_COMPILE_STATUS will return the status
    //of the compilation phase
    
    if ( !( status == GL_TRUE ) )
        cerr << "\nFragment Shader compilation failed..." << '\n';
    
    infoLog = new char[10];
    bufSize = 0;
    glGetShaderInfoLog( fragmentShader, bufSize, NULL, infoLog );
    for ( int i = 0; i < bufSize; i++ )
        cout << infoLog[ i ] << endl;
    delete [] infoLog;
    
    // create the shader program
    program = glCreateProgram();  //need to create an executable shader program to link all the compiled shader objects
    
    // attach the vertex and fragment shaders to the program
    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );//attach "shader programs" to the "program"
    
    // link the objects for an executable program
    glLinkProgram( program );
    
    glGetProgramiv( program, GL_LINK_STATUS, &status );
    if ( !( status == GL_TRUE ) )
        cout << "Link failed..." << endl;
    
    else
        cout<<"Successfully link to the Program"<<endl;
    // return the program
    return program;//then we can use the linked program in main.cpp by glUseProgram()
}

const char* getShaderProgram( const char *filePath, string &shader )
{
    fstream shaderFile( filePath, ios::in );
    
    if ( shaderFile.is_open() )
    {
        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        shader = buffer.str();
        buffer.clear();
    }
    shaderFile.close();
    
    return shader.c_str();
}
