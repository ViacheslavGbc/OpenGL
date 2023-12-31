#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Math.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

void OnResize(GLFWwindow* window, int width, int height);
void OnInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

struct Color
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;
};

GLuint CreateShader(GLint type, const char* path)
{
    GLuint shader = 0;
    try
    {
        // Load text file
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path);

        // Interpret the file as a giant string
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        // Verify shader type matches shader file extension
        const char* ext = strrchr(path, '.');
        switch (type)
        {
            case GL_VERTEX_SHADER:
                assert(strcmp(ext, ".vert") == 0);
                break;

            case GL_FRAGMENT_SHADER:
                assert(strcmp(ext, ".frag") == 0);
                break;
        default:
            assert(false, "Invalid shader type");
            break;
        }

        // Compile text as a shader
        std::string str = stream.str();
        const char* src = str.c_str();
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        // Check for compilation errors
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Shader failed to compile: \n" << infoLog << std::endl;
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Shader (" << path << ") not found: " << e.what() << std::endl;
        // Alternatively, you can replace the above line with "throw;" to crash your program if a shader fails to load
        //throw;
    }
    return shader;
}

GLuint CreateProgram(GLuint vs, GLuint fs)
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        shaderProgram = GL_NONE;
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return shaderProgram;
}

enum State
{
    OBJ_1,
    OBJ_2,
    OBJ_3,
    OBJ_4,
    OBJ_5,

} state;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, OnResize);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vsDefault = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/Default.vert");
    GLuint fsDefault = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/Default.frag");

    GLuint vsAnimate = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/Animate.vert");
    GLuint vsTransform = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/Transform.vert");

    GLuint fsColor = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/Color.frag");
    GLuint fsColorFade = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/ColorFaded.frag");

    GLuint shaderDefault = CreateProgram(vsDefault, fsDefault);
    GLuint shaderDefaultColor = CreateProgram(vsDefault, fsColor);
    GLuint shaderDefaultFade = CreateProgram(vsDefault, fsColorFade);

    GLuint shaderAnimate = CreateProgram(vsAnimate, fsColor);
    GLuint shaderTransform = CreateProgram(vsTransform, fsColor);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };
    
    float rainbow[] = {
        1.0f, 0.0f, 0.0f,   // red
        0.0f, 1.0f, 0.0f,   // green
        0.0f, 0.0f, 1.0f    // blue
    };

    float white[] = {
        1.0f, 1.0f, 1.0f,   // white
        1.0f, 1.0f, 1.0f,   // white
        1.0f, 1.0f, 1.0f    // white
    };

    float red[] = {
        1.0f, 0.0f, 0.0f,   // red
        1.0f, 0.0f, 0.0f,   // red
        1.0f, 0.0f, 0.0f    // red
    };

    float blue[] = {
        0.0f, 0.0f, 1.1f,   // blue
        0.0f, 0.0f, 1.1f,   // blue
        0.0f, 0.0f, 1.1f    // blue
    };

    float black[] = {
        0.0f, 0.0f, 0.1f,   // blue
        0.0f, 0.0f, 0.1f,   // blue
        0.0f, 0.0f, 0.1f    // blue
    };

    float verticec[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    unsigned int ;
    
    
    // Generate vertex buffers (data)
    // Generate vertex arrays (collections + descriptions of vertex buffers)
    GLuint vaoRainbow, vaoWhite, vaoRed, vaoBlue, vaoBlack,VBO, VAO, EBO;
    GLuint vboPos, vboRainbow, vboWhite, vboRed, vboBlue, vboBlack;
    glGenVertexArrays(1, &vaoRainbow);
    glGenVertexArrays(1, &vaoWhite);
    glGenVertexArrays(1, &vaoRed);
    glGenVertexArrays(1, &vaoBlue);
    glGenVertexArrays(1, &vaoBlack);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &vboPos);
    glGenBuffers(1, &vboBlack);
    glGenBuffers(1, &vboRainbow);
    glGenBuffers(1, &vboWhite);
    glGenBuffers(1, &vboRed);
    glGenBuffers(1, &vboBlue);

    // Upload position
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Upload white
    glBindBuffer(GL_ARRAY_BUFFER, vboWhite);
    glBufferData(GL_ARRAY_BUFFER, sizeof(white), white, GL_STATIC_DRAW);

    // Upload red
    glBindBuffer(GL_ARRAY_BUFFER, vboRed);
    glBufferData(GL_ARRAY_BUFFER, sizeof(red), red, GL_STATIC_DRAW);

    // Upload blue
    glBindBuffer(GL_ARRAY_BUFFER, vboBlue);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blue), blue, GL_STATIC_DRAW);

    // Upload black
    glBindBuffer(GL_ARRAY_BUFFER, vboBlack);
    glBufferData(GL_ARRAY_BUFFER, sizeof(black), black, GL_STATIC_DRAW);

    // Upload rainbow
    glBindBuffer(GL_ARRAY_BUFFER, vboRainbow);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rainbow), rainbow, GL_STATIC_DRAW);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticec), verticec, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // Describe rainbow triangle:
    glBindVertexArray(vaoRainbow);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vboRainbow);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Describe white triangle:
    glBindVertexArray(vaoWhite);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vboWhite);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Describe red triangle:
    glBindVertexArray(vaoRed);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vboRed);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Describe blue triangle:
    glBindVertexArray(vaoBlue);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vboBlue);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Describe blak triangle:
    glBindVertexArray(vaoBlack);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vboBlack);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Unbind the vao to ensure no accidental associations are made
    glBindVertexArray(GL_NONE);
    float prev = glfwGetTime();
    float curr = prev;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float dt = curr - prev, tt = glfwGetTime();
        prev = curr;
        curr = tt;

        // input
        // -----
        OnInput(window);

        // Declair common render attribute beforehand, then assign based on object!
        GLuint shader = shaderDefault;
        GLuint vertexData = vaoWhite;
        Color bg{ 0.2f, 0.3f, 0.3f, 1.0f };
        Color tint{ 1.0f, 1.0f, 1.0f, 1.0f };

        switch (state)
        {
        case OBJ_1:
            shader = shaderDefault;
            vertexData = vaoWhite;

            // glVertexAttribPointer makes the association between the bound vbo and bound vao.
            // Once the association has been made, vbos can be modified independent of vaos.
            glBindBuffer(GL_ARRAY_BUFFER, vboWhite);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(white), white);
            break;

        case OBJ_2:
            shader = shaderDefault;
            vertexData = vaoRainbow;
            break;

        case OBJ_3:
            shader = shaderDefaultFade;
            vertexData = vaoRed;
            break;

        case OBJ_4:
            //shader = shaderDefault;
            vertexData = VAO;
           
        // render
        // ------
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            
            glClear(GL_COLOR_BUFFER_BIT);

            // draw our first triangle (?)
            //glBindVertexArray(VAO); 
            
            //glDrawArrays(GL_TRIANGLES, 0, 6);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //glBindVertexArray(0); // no need to unbind it every time 

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            //glfwPollEvents();


            /*shader = shaderAnimate;
            vertexData = vaoRainbow;
            tint.r = cosf(tt * 2.0f * PI) * 0.5f + 0.5f;
            tint.g = cosf(tt * 2.0f * PI * 0.333f) * 0.5f + 0.5f;
            tint.b = cosf(tt * 2.0f * PI * 0.666f) * 0.5f + 0.5f;
            */
            break;

        case OBJ_5:
            // uncomment this call to draw in wireframe polygons.
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            shader = shaderDefault;
            vertexData = vaoBlue;
            glBindBuffer(GL_ARRAY_BUFFER, vboBlue);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(blue), blue);
            //shader = shaderTransform;
            //vertexData = vaoRainbow;
            break;
        }

        GLint uColor = glGetUniformLocation(shader, "u_color");
        GLint uTime = glGetUniformLocation(shader, "u_time");
        GLint uTransform = glGetUniformLocation(shader, "u_transform");
        glUniform1f(uTime, tt);
        glUniform3f(uColor, tint.r, tint.g, tint.b);

        float ncos = cosf(tt) * 0.5f + 0.5f;
        float nsin = sinf(tt) * 0.5f + 0.5f;
        Matrix scale = Scale(ncos, ncos, 0.0f);
        Matrix rotation = RotateZ(tt * DEG2RAD * 100.0f);
        Matrix translation = Translate(cosf(tt), 0.0f, 0.0f);
        Matrix transform = scale * rotation * translation;
        glUniformMatrix4fv(uTransform, 1, GL_TRUE, &transform.m0);

        // render
        // ------
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shader);
        glBindVertexArray(vertexData);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &vaoRainbow);
    glDeleteVertexArrays(1, &vaoWhite);
    glDeleteBuffers(1, &vboPos);
    glDeleteBuffers(1, &vboRainbow);
    glDeleteBuffers(1, &vboWhite);
    glDeleteProgram(shaderDefault);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // Too many shaders to delete. No longer keeping track xD

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void OnInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        state = OBJ_1;
    
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        state = OBJ_2;
    
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        state = OBJ_3;
    
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        state = OBJ_4;
    
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        state = OBJ_5;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void OnResize(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
