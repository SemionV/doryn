#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <spdlog/fmt/fmt.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Function to print a glm::mat4
void printMat4(const glm::mat4& mat) {
    std::cout << "glm::mat4(" << std::endl;
    for (int row = 0; row < 4; ++row) {
        std::cout << "    ";
        for (int col = 0; col < 4; ++col) {
            // Access the element at column 'col' and row 'row'
            std::cout << mat[col][row];
            if (col < 3)
                std::cout << ", ";
        }
        if (row < 3)
            std::cout << "," << std::endl;
        else
            std::cout << std::endl;
    }
    std::cout << ");" << std::endl;
}

void printVec4(const glm::vec4& vec) {
    std::cout << fmt::format("glm::vec4({0}, {1}, {2}, {3})", vec[0], vec[1], vec[2], vec[3]) << std::endl;
}

TEST(Camera, perspectiveProjection)
{
    unsigned int viewportWidth = 800;
    unsigned int viewportHeight = 600;

    float fov = 45.0f; // Field of View in degrees
    float aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    // Create the projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

    auto vecA = glm::vec4{10, 10, -30, 1};
    printMat4(projection);
    printVec4(vecA);

    auto vecAm = projection * vecA;
    printVec4(vecAm);
    printVec4(vecAm / vecAm.w);
}