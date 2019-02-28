#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <chrono>
#include <iostream>
using namespace std;

const GLchar* loadVertexShader(){
    return "#version 150\n"
            "in vec2 position;"
            "void main(){"
                "gl_Position = vec4(position, 0.0, 1.0);"//Equivalent line: gl_Position = vec4(position.x, position.y, 0.0, 1.0);
            "}";
}

const GLchar* loadFragmentShader(){
    return "#version 150\n"
            "uniform vec3 triangleColor;" //Instead of hard coding the color, make it an attribute that can be alterated at any time later on.
            "out vec4 outColor;"
            "void main(){"
                "outColor = vec4(triangleColor, 1.0);"
            "}";
}

int main(){
    //Initiliaze window setting values
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    //settings.antialiasingLevel = 2; // Optional

    //Create window
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Close, settings);

    glewExperimental = GL_TRUE;//Force GLEW to use a modern OpenGL method for checking if a function is available.
    glewInit();

    //Prepare to draw shape
    float vertices[] = {
        -0.25f,  0.25f, //Top
        0.0f, -0.25f, //Left
        -0.5f, -0.25f, //Right

        0.25f,  0.25f,
        0.0f, -0.25f,
        0.5f, -0.25f,

        0.0f, 0.75f,
        0.25f, 0.25f,
        -0.25f, 0.25f

    };//A triangle.

    //Make buffer (Vertex Buffer Object, "vbo") to store data
    GLuint vbo; //GL unsigned int to store pointer to object
    glGenBuffers(1, &vbo); //Generate 1 buffer

    //Make Vertex Buffer Object the active array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //Copy vertices to the active buffer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //GL_STATIC_DRAW => The vertex data will be uploaded once and drawn many times
    //Determines in what kind of memory the data is stored on the graphics card for the highest efficiency

    //Create a vertex array object (VOA). It stores information everytime glVertexAttribPointer is called.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);//When switching to different vertex data and vertex formats, just need to bind the right VOA.

    //Compiling the shaders:
    const GLchar* vertexSource = loadVertexShader();//Load vertex shader; Duty is to output the final vertex position in device coordinates and to output any data the fragment shader requires
    const GLchar* fragmentSource = loadFragmentShader();//Load fragment shader

    //Create vertex shader and load data into it
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);//Compile the vertex shader now

    //Create fragment shader and load data into it
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);//Compile the fragment shader now

    //Check if the shaders succesfully compiled
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if(status == GL_TRUE){
        cout << "Vertex shader compiled succesfully." << endl;
    }
    else{
        cout << "***Vertex shader did not compile.***" << endl;
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if(status == GL_TRUE){
        cout << "Fragment shader compiled succesfully." << endl;
    }
    else{
        cout << "***Fragment shader did not compile.***" << endl;
    }

    //Create program out of the two shaders.
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);//Only one vertex buffer can be active at a time.
    glAttachShader(shaderProgram, fragmentShader);

    //This needs to happen before linking the program, but is not necessary if there is only one output.
    glBindFragDataLocation(shaderProgram, 0, "outColor");//The fragment shader is allowed to write to multiple buffers. Therefore, must explicitly specify which output is written to which buffer.

    glLinkProgram(shaderProgram);//After attaching the shaders, they must be linked toegther.

    glUseProgram(shaderProgram);//Start using the program. Only one program can be active at a time.

    //Need to define how attributes are formatted and ordered in the vertex shader.
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");//Reference to the position input in the vertex shader
    //Since position is the only input for the vertex shader, posAttrib has a position of zero.

    //With the position of the attribute known, can now specify how the data for the shader is to be retrieved.
    //2 is the number of input values (size of the position vec). Float is the input value type.
    //Boolean is whether input values should be normalized. Last two values are stride and offset.
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);//Info is now also stored in the VOA created at the start.

    glEnableVertexAttribArray(posAttrib);//Enable the vertex attribute array that was just finalized.

    auto t_start = std::chrono::high_resolution_clock::now();
    GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor");//Get the location of the the fragment shader attribute.
    glUniform3f(uniColor, 1.0f, 1.0f, 0.0f); //Set the fragment shader color after it has already been compiled.
    //glUniformXY(...);  X = 3 = numer of compents (RGB), Y = f = float type

    //Run main window loop
    bool running = true;
    while (running){
        sf::Event windowEvent;

        while (window.pollEvent(windowEvent)){
            switch (windowEvent.type){
                case sf::Event::Closed:
                    running = false;
                    break;
                case sf::Event::KeyPressed:
                    if (windowEvent.key.code == sf::Keyboard::Escape){
                        running = false;
                    }
                    break;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        glUniform3f(uniColor, (sin(time * 4.0f) + 1.0f) / 2.0f, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f);

        glDrawArrays(GL_TRIANGLES, 0, 9);//Primitive type (usually point, line or triangle), # vertices to skip at start, # of vertices
        window.display();
    }

    //Clean up
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}
