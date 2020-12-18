//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once
#include "Geometry.h"
#include "Renderer.h"

class Sphere : public Geometry
{
public:
    Sphere(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
           Rotation rotation = {0.0f, 0.0f, 0.0f}, Scale Scale = {1.0f, 1.0f, 1.0f, 1.0f});
    void updateSubdivision(int VerticalSteps, int HorizontalSteps);
    void updateDrawData() override;
    inline std::string getClassName() override
    {
        return "Geometry::Sphere";
    }
    inline int getVerticalSteps() const
    {
        return m_VerticalSteps;
    }
    inline int getHorizontalSteps() const
    {
        return m_HorizontalSteps;
    }
private:
    int m_VerticalSteps;
    int m_HorizontalSteps;
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec4 Color;
    };
};
