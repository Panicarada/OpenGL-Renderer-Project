//
// Created by 邱泽鸿 on 2020/12/14.
//

#include "TestShadowVSM.h"
#include "Sphere.h"
#include "Cube.h"
#include "Cylinder.h"
#include <unordered_map>

void test::TestShadowVSM::OnUpdate(GLFWwindow *Window, float deltaTime)
{
    m_Camera->OnKeyAction(Window, deltaTime);
}

void test::TestShadowVSM::OnRender()
{
    DebugCall(glClearColor(0.1f, 0.3f, 0.5f, 0.6f));
    // 渲染阴影
//    m_Shadow->renderShadow(m_GeometrySet, m_LightSet);

    // 清除z-buffer，用于深度测试；以及清除背景颜色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 启用深度图
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SoftShadow->getSoftShadowMap());

//    m_SoftShadow->b

    m_SoftShadow->render(m_GeometrySet, m_LightSet);

    m_Shader->bind();
    m_Shader->setUniform1f("u_zNear", ZNEAR);
    m_Shader->setUniform1f("u_zFar", ZFAR);

    if (m_Camera)
    { // 输入相机位置
        auto Position = m_Camera->getPosition();
        m_Shader->setUniform3f("u_CameraPosition", Position.x, Position.y, Position.z);
    }
    for (auto geometry : m_GeometrySet)
    {
        geometry->draw();
    }
}

void test::TestShadowVSM::OnImGuiRender()
{
    if (ImGui::ColorEdit4("Light Color", &selectedLight->m_Color[0]))
    {
        selectedLight->updateData();
    }
    if (ImGui::SliderFloat3("Light Position", &selectedLight->m_Position.x, -10.0f, 10.0f))
    {
        selectedLight->updateData();
        // 重新计算阴影
        m_SoftShadow->render(m_GeometrySet, m_LightSet);
        // 重新采样
        OnRender();
    }
    if (ImGui::SliderFloat("Light Brightness", &selectedLight->m_Brightness, 0.5f, 5.0f))
    {
        selectedLight->updateData();
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

test::TestShadowVSM::TestShadowVSM()
{
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    m_Shader = std::make_shared<Shader>("../resource/VSM/TestVSM.shader");

    m_Shader->bind();
    m_Camera = std::make_shared<Camera>();


    // 地板
    Floor = std::make_shared<Cube>(m_Camera, m_Shader);
    Floor->m_Scale = {20.0f, 20.0f, 0.0001f, 20.0f};
    Floor->m_Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);
    Floor->updateDrawData();
    Floor->m_Position = m_Camera->getPosition() + 10.0f * m_Camera->getDirection() - glm::vec3(0.0f, 2.0f, 0.0f);

    // 立方体
    auto cube = std::make_shared<Cube>(m_Camera, m_Shader);
    cube->m_Position = Floor->m_Position + glm::vec3(0.0f, 2.0f, -1.0f);
    m_GeometrySet.insert(Floor);
    m_GeometrySet.insert(cube);

    // 光源
    m_Shader->setUniform4f("u_Ambient", 0.2f, 0.2, 0.2f, 1.0f);
    selectedLight = std::make_shared<Light>(m_Shader);
    selectedLight->m_Position = cube->m_Position + glm::vec3(-2.0f, 0.4f, 1.0f);
    selectedLight->m_Brightness = 4.0f;
    m_LightSet.insert(selectedLight);
    selectedLight->updateData();

    // 阴影
    auto ShadowShader = std::make_shared<Shader>("../resource/VSM/Shadow.shader");
    m_Shader->setUniform1i("u_VSMShadowMap", 0); // 深度图是TEXTURE0
    m_SoftShadow = std::make_shared<SoftShadow>(ShadowShader);
    m_SoftShadow->render(m_GeometrySet, m_LightSet);
    m_SoftShadow->render(m_GeometrySet, m_LightSet);

}