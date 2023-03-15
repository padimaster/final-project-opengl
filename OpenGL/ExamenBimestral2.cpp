#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <string>
#include <ctime>
#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>

//ESTRUCTURA DE VARIABLES
struct ModelProps {
    float x;
    float y;
    float z;
    float scale;
};

//DEFINICIÓN DEL PROTOTIPO DE FUNCIONES
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// TAMAÑO DE LA VENTANA
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

//CONFIGURACIÓN DE LA CÁMARA
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float sensivity = 0.005f;

//POSICIONES DE LOS MODELOS
ModelProps modelProps[] = {
    // x, y, z, factor scale
    // Castle
    { 0.0f, 40.0f, 0.0f , 3.5f },
    // Zombie
    { 10.0f, -21.4f, 9.0f, 1.0f },
    // Joel
    { 5.0f, -28.2f, 17.0f, 1.0f },
    // Moon
    { 15.0f, 25.0f, 15.0f, 0.05f},
    // Lampara cerca joel
    { 4.0f, -28.2f, 15.0f, 0.075f},
    // Lampara cerca zombie
    { 12.0f, -21.4f, 9.0f, 0.075f},
    // Lampara en terreno
    { 20.0f, -30.0f, 15.0f, 0.075f},
    // Helicopter
    { -10.0f, 15.0f, 15.0f, 0.005f },
};

int main()
{
    // INICIALIZAR GLFW 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // CREACIÓN DE VENTANA
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "EXAMEN BIMESTRE II", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //CAPTURAR EL MOUSE
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // INICIALIZAR GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //ACTIVAR EL ZBUFFER
    glEnable(GL_DEPTH_TEST);

    //CARGA DE SHADERS PARA LOS DIFERENTES PROGRAMAS
    Shader ourShader("shaders/shader_models.vs", "shaders/shader_models.fs");
    Shader lightCubeShader("shaders/shader_lightcube.vs", "shaders/shader_lightcube.fs");

    //CARGA DE MODELOS
    Model ourCastle("./model/castillo/castillo.obj");
    Model ourZombie("./model/zombie/zombie.obj");
    Model ourJoel("./model/joel/joel.obj");
    Model ourMoon("./model/luna/luna.obj");
    Model ourHelicopterWithoutBlades("./model/helicopter/helicopter_without_blades.obj");
    Model ourBlades("./model/helicopter/blades.obj");
    Model ourTailBlades("./model/helicopter/tail_blades.obj");
    Model ourLampara("./model/lampara/lampara.obj");

    //ARREGLO DE MODELOS
    Model ourModels[] = {
        ourCastle,
        ourZombie,
        ourJoel,
        ourMoon,
        ourLampara,
        ourLampara,
        ourLampara,
        ourHelicopterWithoutBlades,
        ourBlades
    };

    //VERTICES PARA DIBUJAR LUZ
    float vertices[] = {

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    //POSICIONES DE LOS POINT LIGHTS
    glm::vec3 lightPositions[] = {
        glm::vec3(3.5f, -28.2f, 15.0f), //cerca a joel
        glm::vec3(11.5f, -21.4f, 9.0f), //cerca zombie
        glm::vec3(19.5f, -30.0f, 15.0f), //cesped castillo
        glm::vec3(12.5f, 25.0f, 15.0f) //luna
    };


    //MODIFICACIÓN DE LA VELOCIDAD DE MOVIMIENTO DE LA CÁMARA
    camera.MovementSpeed = 10;

    //GENERACIÓN DE VAO Y VBO
    unsigned int lightCubeVAO, VBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //MANERA DE LEER LOS DATOS (SOLO POSICIONES, POR TANTO STRIDE = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Obtener el tiempo de inicio
    float startTime = glfwGetTime();
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        //VARIABLES PARA CONFIGURAR LA VELOCIDAD DE LA CÁMARA INDEPENDIENTE DEL HARDWARE
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //PROCESAR INPUT
        processInput(window);

        //COLOR DE FONDO
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //ACTIVACIÓN DEL SHADER PARA LOS MODELOS
        ourShader.use();

        //ASIGNACIÓN DE VALORES PARA LA VISUALIZACIÓN DESDE LA CÁMARA
        ourShader.setVec3("viewPos", camera.Position);

        //ASIGNACIÓN DEL VALOR DE SHININESS PARA LOS OBJETOS
        ourShader.setFloat("material.shininess", 32.0f);

                
        //CAMBIO DE DIRECCIÓN DE DIRECCIÓN HACIA A DONDE APUNTA LA SPOTLIGHT (LINTERNA)
        float radius = 0.3f;
        float camX = (sin(glfwGetTime()) / 2) * radius;
        float camZ = (cos(glfwGetTime()) / 2) * radius;

        //ASIGNACIÓN DE LOS VALORES PARA LA LUZ DIRECCIONAL
        /* posible eliminacion
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.005f, 0.005f, 0.005f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);*/


        //ASIGNACIÓN DE VALORES PARA LAS DIFERENTES POINTLIGHTS
        //SE UTILIZA LAS POSICIONES DEFINIDAS EN EL ARREGLO DECLARADO ANTERIORMENTE

        // point light 1 cerca a joel
        ourShader.setVec3("pointLights[0].position", lightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("pointLights[0].diffuse", 0.49f, 0.73f, 0.91f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.22);
        ourShader.setFloat("pointLights[0].quadratic", 0.20);

        // point light 2 cerca a zombie
        ourShader.setVec3("pointLights[1].position", lightPositions[1]);
        ourShader.setVec3("pointLights[1].ambient", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("pointLights[1].diffuse", 0.49f, 0.73f, 0.91f);
        ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.22);
        ourShader.setFloat("pointLights[1].quadratic", 0.20);

        // point light 3 cescep del castillo
        ourShader.setVec3("pointLights[2].position", lightPositions[2]);
        ourShader.setVec3("pointLights[2].ambient", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("pointLights[2].diffuse", 0.49f, 0.73f, 0.91f);
        ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[2].constant", 1.0f);
        ourShader.setFloat("pointLights[2].linear", 0.22);
        ourShader.setFloat("pointLights[2].quadratic", 0.20);

        // point light 4 LUNA
        ourShader.setVec3("pointLights[3].position", lightPositions[3]);
        ourShader.setVec3("pointLights[3].ambient", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("pointLights[3].diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[3].constant", 1.0f);
        ourShader.setFloat("pointLights[3].linear", 0.014);
        ourShader.setFloat("pointLights[3].quadratic", 0.0007);

        //DEFINICIÓN DE LOS VALORES PARA EL SPOTLIGHT DEL HELICOPTERO

        ourShader.setVec3("spotLight.position", glm::vec3(-10.0f, 15.0f, 15.0f));
        ourShader.setVec3("spotLight.direction", glm::vec3(camX, -1.0f, camZ));
        ourShader.setVec3("spotLight.ambient", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("spotLight.diffuse", 10.0f, 10.0f, 10.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.014);
        ourShader.setFloat("spotLight.quadratic", 0.0007);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(6.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(7.5f)));

        // DEFINICIÓN DE LAS MATRICES DE VISTA Y PROYECCIÓN
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        //                                      GRAFICACIÓN DE MODELOS

    //SE ESTABLECE LA MATRIZ MODEL PARA CADA MODELO CON DIFERENTES MOVIMIENTOS DE TRASLACIÓN, ROTACIÓN Y ESCALAMIENTO
    // 

        const int numberOfModels = sizeof(modelProps) / sizeof(ModelProps);
        for (int i = 0; i < numberOfModels; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(modelProps[i].x, modelProps[i].y, modelProps[i].z));
            model = glm::scale(model, glm::vec3(modelProps[i].scale, modelProps[i].scale, modelProps[i].scale));
            ourShader.setMat4("model", model);
            ourModels[i].Draw(ourShader);

            // Helicopter
            if (i == numberOfModels - 1) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(modelProps[i].x, modelProps[i].y, modelProps[i].z));
                model = glm::scale(model, glm::vec3(modelProps[i].scale, modelProps[i].scale, modelProps[i].scale));

                // Guardar la matriz de modelo original
                glm::mat4 originalModel = model;

                //ANGULO DE ROTACION CON SU VELOCIDAD
                float angle = glfwGetTime() * 10.0f;
                
                // Rotar la hélice
                glm::vec3 bladePos = glm::vec3(modelProps[i].x, modelProps[i].y + 0.5f, modelProps[i].z);
                model = glm::translate(model, bladePos);
                model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, -bladePos);

                ourShader.setMat4("model", model);
                ourModels[i + 1].Draw(ourShader);
            }
        }

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        //SE ACTIVA EL NUEVO VAO
        glBindVertexArray(lightCubeVAO);


        // LIMPIRAR BUFFERS Y PROCESAR CAPTURAS PENDIENTES
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    //Limpiar los buffers de la GPU y terminar de usar glfw
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

    //SE PROCESA EL INPUT DEL MOVIMIENTO DE LA CÁMARA 
    //W -> HACIA ADELANTE
    //A-> HACIA LA IZQUIERDA
    //D -> HACIA LA DERECHA
    //S-> HACIA ATRÁS
    //SPACE->HACIA ARRIBA
    //LEFT SHIFT -> HACIA ABAJO
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.Position.y = camera.Position.y + sensivity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.Position.y = camera.Position.y - sensivity;
}

// ADAPTAR EL VIEWPORT AL TAMAÑO DE LA VENTANA
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



// CPATURAR EL MOVIMIENTO DEL MOUSE
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//CAPTURAR EL SCROLL DEL MOUSE
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
