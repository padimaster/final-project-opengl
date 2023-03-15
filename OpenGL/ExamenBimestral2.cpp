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
    glm::vec3 position;
    float scale;
    float angle;
};

struct PointLight {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight : PointLight {
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
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

float cameraSensivity = 0.01f;
float sensivity = 0.0075f;

int activeObject = 0;

//POSICIONES DE LOS MODELOS
//POSICIONES DE LOS MODELOS
ModelProps modelProps[] = {
    // position, factor scale, angle
    // Joel 0
    { 
        // Posicion
        glm::vec3(5.0f, -28.2f, 17.0f), 
        // Factor de Escala
        1.0f, 
        // Angulo de Rotacion
        0.0f 
    },
    // Zombie 1
    { 
        glm::vec3(10.0f, -21.4f, 9.0f), 
        1.0f, 
        0.0f 
    },
    // Moon 2
    { 
        glm::vec3(15.0f, 25.0f, 15.0f), 
        0.05f, 
        0.0f 
    },
    // Castle 3
    { 
        glm::vec3(0.0f, 40.0f, 0.0f ), 
        3.5f, 
        0.0f 
    },
    // Lampara cerca joel 4
    { 
        glm::vec3(4.0f, -28.2f, 15.0f), 
        0.075f, 
        0.0f 
    },
    // Lampara cerca zombie 5
    { 
        glm::vec3(12.0f, -21.4f, 9.0f), 
        0.075f, 
        0.0f 
    },
    // Lampara en terreno 6
    { 
        glm::vec3(20.0f, -30.0f, 15.0f), 
        0.075f, 
        0.0f 
    },
    // Helicopter 7
    { 
        glm::vec3(-10.0f, 15.0f, 15.0f), 
        0.005f, 
        0.0f 
    },
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
        ourJoel,
        ourZombie,
        ourMoon,
        ourCastle,
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

    // PROPIEDADES DE LOS POINT LIGHTS
    // point light cerca a joel
    PointLight lampNearJoelPointLight;
    lampNearJoelPointLight.position = modelProps[4].position;
    lampNearJoelPointLight.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    lampNearJoelPointLight.diffuse = glm::vec3(0.49f, 0.73f, 0.91f);
    lampNearJoelPointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lampNearJoelPointLight.constant = 1.0f;
    lampNearJoelPointLight.linear = 0.22f;
    lampNearJoelPointLight.quadratic = 0.20f;

    // point light cerca a zombie
    PointLight lampNearZombiePointLight;
    lampNearZombiePointLight.position = modelProps[5].position;
    lampNearZombiePointLight.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    lampNearZombiePointLight.diffuse = glm::vec3(0.49f, 0.73f, 0.91f);
    lampNearZombiePointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lampNearZombiePointLight.constant = 1.0f;
    lampNearZombiePointLight.linear = 0.22f;
    lampNearZombiePointLight.quadratic = 0.20f;


    // point light 3 cescep del castillo
    PointLight castlePointLight;
    castlePointLight.position = modelProps[6].position;
    castlePointLight.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    castlePointLight.diffuse = glm::vec3(0.49f, 0.73f, 0.91f);
    castlePointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    castlePointLight.constant = 1.0f;
    castlePointLight.linear = 0.22f;
    castlePointLight.quadratic = 0.20f;

    // point light 4 LUNA
    PointLight moonPointLight;
    moonPointLight.position = glm::vec3(12.5f, 25.0f, 15.0f);
    moonPointLight.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    moonPointLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    moonPointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    moonPointLight.constant = 1.0f;
    moonPointLight.linear = 0.014f;
    moonPointLight.quadratic = 0.0007;

    PointLight pointLights[] = {
        lampNearJoelPointLight,
        lampNearZombiePointLight,
        castlePointLight, 
        moonPointLight 
    };
    
    const int numberOfModels = sizeof(modelProps) / sizeof(ModelProps);
    const int pointLightNumber = sizeof(pointLights) / sizeof(PointLight);

    SpotLight helicopterSpotLight;
    helicopterSpotLight.position = modelProps[numberOfModels - 1].position;
    helicopterSpotLight.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    helicopterSpotLight.diffuse = glm::vec3(10.0f, 10.0f, 10.0f);
    helicopterSpotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    helicopterSpotLight.constant = 1.0f;
    helicopterSpotLight.linear = 0.014f;
    helicopterSpotLight.quadratic = 0.0007f;
    helicopterSpotLight.direction = glm::vec3(0.0f, -0.2f, -1.0f);
    helicopterSpotLight.cutOff = glm::cos(glm::radians(6.5f));
    helicopterSpotLight.outerCutOff = glm::cos(glm::radians(8.0f));
    // 

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

        //ASIGNACIÓN DE LOS VALORES PARA LA LUZ DIRECCIONAL
        /* posible eliminacion
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.005f, 0.005f, 0.005f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);*/


        //ASIGNACIÓN DE VALORES PARA LAS DIFERENTES POINTLIGHTS
        //SE UTILIZA LAS POSICIONES DEFINIDAS EN EL ARREGLO DECLARADO ANTERIORMENTE
        
        for (int i = 0; i < pointLightNumber; i++) {
            std::string lightStr = "pointLights[" + std::to_string(i) + "].";
            ourShader.setVec3(lightStr + "position", pointLights[i].position);
            ourShader.setVec3(lightStr + "ambient", pointLights[i].ambient);
            ourShader.setVec3(lightStr + "diffuse", pointLights[i].diffuse);
            ourShader.setVec3(lightStr + "specular", pointLights[i].specular);
            ourShader.setFloat(lightStr + "constant", pointLights[i].constant);
            ourShader.setFloat(lightStr + "linear", pointLights[i].linear);
            ourShader.setFloat(lightStr + "quadratic", pointLights[i].quadratic);
        }

        //CAMBIO DE DIRECCIÓN DE DIRECCIÓN HACIA A DONDE APUNTA LA SPOTLIGHT (LINTERNA)
        float radius = 0.3f;
        float camX = (sin(glfwGetTime()) / 2) * radius;
        float camZ = (cos(glfwGetTime()) / 2) * radius;
        
        //DEFINICIÓN DE LOS VALORES PARA EL SPOTLIGHT DEL HELICOPTERO
        ourShader.setVec3("spotLight.ambient", helicopterSpotLight.ambient);
        ourShader.setVec3("spotLight.diffuse", helicopterSpotLight.diffuse);
        ourShader.setVec3("spotLight.specular", helicopterSpotLight.specular);
        ourShader.setFloat("spotLight.constant", helicopterSpotLight.constant);
        ourShader.setFloat("spotLight.linear", helicopterSpotLight.linear);
        ourShader.setFloat("spotLight.quadratic", helicopterSpotLight.quadratic);
        ourShader.setFloat("spotLight.cutOff", helicopterSpotLight.cutOff);
        ourShader.setFloat("spotLight.outerCutOff", helicopterSpotLight.outerCutOff);
        //Cambio de direccion del spotlight
        helicopterSpotLight.position = modelProps[numberOfModels - 1].position;
        ourShader.setVec3("spotLight.position", helicopterSpotLight.position);
        //Cambio de direccion del spotlight
        helicopterSpotLight.direction = glm::vec3(camX, -1.0f, camZ);
        ourShader.setVec3("spotLight.direction", helicopterSpotLight.direction);

        // DEFINICIÓN DE LAS MATRICES DE VISTA Y PROYECCIÓN
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        //                                      GRAFICACIÓN DE MODELOS

    //SE ESTABLECE LA MATRIZ MODEL PARA CADA MODELO CON DIFERENTES MOVIMIENTOS DE TRASLACIÓN, ROTACIÓN Y ESCALAMIENTO
    // 

        for (int i = 0; i < numberOfModels; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, modelProps[i].position);
            model = glm::scale(model, glm::vec3(modelProps[i].scale, modelProps[i].scale, modelProps[i].scale));
            model = glm::rotate(model, modelProps[i].angle, glm::vec3(0.0f, 1.0f, 0.0f));
            ourShader.setMat4("model", model);
            ourModels[i].Draw(ourShader);

            // Helicopter
            if (i == numberOfModels - 1) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, modelProps[i].position);
                model = glm::scale(model, glm::vec3(modelProps[i].scale, modelProps[i].scale, modelProps[i].scale));

                //ANGULO DE ROTACION CON SU VELOCIDAD
                float angle = currentFrame * 10.0f;

                // Rotar la hélice
                model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
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
        camera.ProcessKeyboard(FORWARD, cameraSensivity);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, cameraSensivity);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, cameraSensivity);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, cameraSensivity);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.Position.y = camera.Position.y + sensivity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.Position.y = camera.Position.y - sensivity;
    // Movimiento de los Objetos
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        modelProps[activeObject].position.z = modelProps[activeObject].position.z + sensivity;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        modelProps[activeObject].position.z = modelProps[activeObject].position.z - + sensivity;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        modelProps[activeObject].position.x = modelProps[activeObject].position.x + sensivity;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        modelProps[activeObject].position.x = modelProps[activeObject].position.x - sensivity;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        modelProps[activeObject].position.y = modelProps[activeObject].position.y + sensivity;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        modelProps[activeObject].position.y = modelProps[activeObject].position.y - sensivity;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        modelProps[activeObject].angle = modelProps[activeObject].angle + sensivity;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        modelProps[activeObject].angle = modelProps[activeObject].angle - sensivity;
    // Change active object
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        activeObject = 0;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        activeObject = 7;
}

// ADAPTAR EL VIEWPORT AL TAMAÑO DE LA VENTANA
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



// CAPTURAR EL MOVIMIENTO DEL MOUSE
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