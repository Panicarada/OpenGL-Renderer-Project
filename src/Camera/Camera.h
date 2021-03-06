//
// Created by 邱泽鸿 on 2020/12/13.
//

#pragma once

#include "Basic.h"

class Camera
{
public:
    void save(std::ostream& Out) const;
    void load(std::ifstream& In);
    Camera(const float& AngleOfView = 45.0f,
           const glm::mat4 &Projection = glm::perspective(glm::radians(45.0f), WINDOW_RATIO, ZNEAR, ZFAR),
           const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
           const glm::vec3 &InitUp = glm::vec3(0.0f, 1.0f, 0.0f), // 相机初始上方是y轴正方向
           const glm::vec3 &InitDirection = glm::vec3(0.0f, 0.0f, -1.0f), // 相机屏幕z轴负方向
           const float &Yaw = 0.0f, const float &Pitch = 0.0f);
    virtual ~Camera() = default;
    void OnKeyAction(GLFWwindow *Window, float deltaTime); // 键盘输入，需要移动
    inline glm::mat4 getViewMatrix() const
    {
        return m_ViewMatrix;
    }
    inline void setProjection(const glm::mat4 &Projection, const std::string& Tag)
    {
        m_Projection = Projection;
        m_Tag = Tag;
    }
    inline void resetZooming()
    {
        m_AngleOfView = InitAngleOfView;
        m_Projection = glm::perspective(glm::radians(InitAngleOfView), WINDOW_RATIO, ZNEAR, ZFAR);
    }
    inline glm::mat4 getProjectionMatrix() const
    {
        return m_Projection;
    }
    inline glm::vec3 getPosition() const
    {
        return m_Position;
    }
    inline glm::vec3 getDirection() const
    {
        return m_Direction;
    }
    void OnMouseAction(GLFWwindow* window, glm::vec2 Position);
    void OnKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods); // 相机移动
    void OnScrollAction(const glm::vec2&& Offset);
private:
    void updateCameraVectors(); // 俯仰角或者偏航角改变后，更新相机向量
public:
    bool isFPS; // 是否处于漫游模式（鼠标隐藏，鼠标的移动是镜头的移动）
    bool isOrbit; // 是否是Orbit模式（只能绕着观察物体转圈和zoom）
    glm::vec3 TargetPosition; // Orbit/Pan模式下，观察物体的坐标
protected:
    std::string m_Tag; // 描述相机的标签（比如Orthogonal / Perspective）
    glm::mat4 m_Projection; // 投影矩阵
    glm::mat4 m_ViewMatrix; // 视图矩阵
    glm::vec3 m_Position; // 相机位置

    /* 相机坐标系标准向量 */
    glm::vec3 m_InitDirection;
    glm::vec3 m_Direction;
    glm::vec3 m_InitUp;
    glm::vec3 m_Up;
    glm::vec3 m_InitRight;
    glm::vec3 m_Right;

    /* 旋转 */
    glm::quat m_qPitch; // 俯仰角对应四元数
    glm::quat m_qYaw; // 偏航角对应四元数
    float m_Pitch; // 俯仰角，角度制，相当于绕InitRight的旋转角
    float m_Yaw; // 偏航角，角度制，相当于绕InitUp的旋转角
    float InitAngleOfView; // 原始的视角大小，用于Reset
    float m_AngleOfView; // 视角大小，角度制（透视投影中）
};
