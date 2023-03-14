#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include <iostream>

class OpenGLObject {
public:
    // Constructor para el objeto de OpenGL
    OpenGLObject() {
        // Inicializar las variables miembro
        model = glm::mat4(1.0f);
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        scale = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    // Funci�n para establecer el modelo del objeto
    void setModel(glm::mat4 model) {
        this->model = model;
    }

    // Funci�n para establecer la posici�n del objeto
    void setPosition(glm::vec3 position) {
        this->position = position;
    }

    // Funci�n para establecer la rotaci�n del objeto
    void setRotation(glm::vec3 rotation) {
        this->rotation = rotation;
    }

    // Funci�n para establecer la escala del objeto
    void setScale(glm::vec3 scale) {
        this->scale = scale;
    }

    // Funci�n para dibujar el objeto en su posici�n y con su escala y rotaci�n actuales
    void draw(GLuint shaderProgram) {
        // Calcular la matriz modelo del objeto
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, scale);
        modelMatrix = model * modelMatrix;

        // Obtener las ubicaciones de las variables uniformes en el shader
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

        // Enviar las matrices al shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

        // Dibujar el objeto
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        glBindVertexArray(0);
    }

private:
    glm::mat4 model;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};