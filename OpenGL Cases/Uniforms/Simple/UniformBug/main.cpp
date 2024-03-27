#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp> 

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void loop(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

const char* COMPUTE_SHADER_SOURCE =
    "#version 460 core\n"
    "layout(local_size_x = 8, local_size_y = 8) in;\n"
    "uniform float f;\n"
    "layout (binding = 0, r32f) restrict writeonly uniform image2D outBuffer;\n"
    "void main()\n"
    "{\n"
    "   imageStore(outBuffer, ivec2(gl_GlobalInvocationID.xy), vec4(f, 0, 0, 0));\n"
    "}\0";

unsigned int shaderProgram1, shaderProgram2;
unsigned int uniform1, uniform2;
bool useOneShader = true;
GLuint texture, fbo;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Press SPACEBAR to use 2 shaders", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    // build and compile our shader program
    // ------------------------------------
    
    unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &COMPUTE_SHADER_SOURCE, NULL);
    glCompileShader(shader);
    
    // check for shader compile errors
    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    
    {
        shaderProgram1 = glCreateProgram();
        glAttachShader(shaderProgram1, shader);
        glLinkProgram(shaderProgram1);

        // check for linking errors
        glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        uniform1 = glGetUniformLocation(shaderProgram1, "f");
        if (uniform1 == -1) {
            std::cout << "WARNING: Uniform f is not defined!\n" << std::endl;
        }
        
    }

    {
        shaderProgram2 = glCreateProgram();
        glAttachShader(shaderProgram2, shader);
        glLinkProgram(shaderProgram2);

        // check for linking errors
        glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        uniform2 = glGetUniformLocation(shaderProgram2, "f");
        if (uniform2 == -1) {
            std::cout << "WARNING: Uniform f is not defined!" << std::endl;
        }
    }



    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureStorage2D(texture, 1, GL_R32F, SCR_WIDTH, SCR_HEIGHT);


    GLenum buffer = GL_COLOR_ATTACHMENT0;
    glCreateFramebuffers(1, &fbo);
    glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texture, 0);
    glNamedFramebufferDrawBuffers(fbo, 1, &buffer);
    
    glfwSwapInterval(0);

    // render loop
    // -----------
    loop(window);

    // de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteProgram(shaderProgram1);
    glDeleteProgram(shaderProgram2);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
    {
        useOneShader = false;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        useOneShader = true;
    }
 
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void loop(GLFWwindow* window) {
    double angle = 0;
    while (!glfwWindowShouldClose(window))
    {

        // input
        // -----
        processInput(window);

        // render
        // ------
        angle += 0.05;
        if (angle > 2.0 * glm::pi<float>()) {
            angle = 0;
        }

        float f = (float)glm::cos(angle) * 0.5f + 0.5f;
        {
            glProgramUniform1f(shaderProgram1, uniform1, f);
            glUseProgram(shaderProgram1);
            glBindImageTexture(0, texture, 0, false, 0, GL_WRITE_ONLY, GL_R32F);
            glDispatchCompute(SCR_WIDTH / 8, SCR_HEIGHT / 8, 1);

            if (!useOneShader) {
                glProgramUniform1f(shaderProgram2, uniform2, f);
                glUseProgram(shaderProgram2);
                glBindImageTexture(0, texture, 0, false, 0, GL_WRITE_ONLY, GL_R32F);
                glDispatchCompute(SCR_WIDTH / 8, SCR_HEIGHT / 8, 1);
            }


        }
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glBlitNamedFramebuffer(fbo, 0, 0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        float value;
        glGetTextureSubImage(texture, 0, 0, 0, 0, 1, 1, 1, GL_RED, GL_FLOAT, GL_UNSIGNED_BYTE, &value);

        if (useOneShader)
        {
            std::cout << "USING 1 SHADER - " << "Wrote " << f << ", value read back was " << value << std::endl;
        }
        else
        {
            std::cout << "USING 2 SHADERS - " << "Wrote " << f << ", value read back was " << value << std::endl;
        }



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}