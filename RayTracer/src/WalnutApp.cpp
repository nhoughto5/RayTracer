#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;
class ExampleLayer : public Walnut::Layer
{
public:
    ExampleLayer()
        : m_Camera(45.0f, 0.1f, 100.0f)
    {
        {
            Sphere sphere;
            sphere.radius = 1.0f;
            sphere.Position = { 0.0f, 0.0f, 0.0f };
            sphere.MaterialIndex = 0;
            m_Scene.Spheres.push_back(sphere);
        }
        {
            Sphere sphere;
            sphere.radius = 100.0f;
            sphere.Position = { 0.0f, -101.0f, 0.0f };
            sphere.MaterialIndex = 1;
            m_Scene.Spheres.push_back(sphere);
        }

        {
            Material material;
            material.Albedo = { 1.0f, 0.0f, 1.0f };
            material.Roughness = 0.05;
            material.Metallic = 1.0;
            m_Scene.Materials.push_back(material);
        }
        {
            Material material;
            material.Albedo = { 0.2f, 0.30f, 1.0f };
            material.Roughness = 0.05;
            material.Metallic = 1.0;
            m_Scene.Materials.push_back(material);
        }
    }

    virtual void OnUpdate(float ts) override
    {
        m_Camera.OnUpdate(ts);
    }

    virtual void OnUIRender() override
    {
        ImGui::Begin("Settings");
        ImGui::Text("Last render: %.3fms", m_LastRenderTime);
        if (ImGui::Button("Render"))
        {
            Render();
        }
        ImGui::End();

        ImGui::Begin("Scene");
        auto pos = m_Camera.GetPosition();
        ImGui::Text("Camera Pos: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
        for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::DragFloat3("Position", glm::value_ptr(m_Scene.Spheres[i].Position), 0.1f);
            ImGui::DragFloat("Radius", &m_Scene.Spheres[i].radius, 0.1f);
            ImGui::DragInt("Material", &m_Scene.Spheres[i].MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size()-1);
            ImGui::PopID();
            ImGui::Separator();
        }

        for (size_t i = 0; i < m_Scene.Materials.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Scene.Materials[i].Albedo), 0.1f);
            ImGui::DragFloat("Roughness", &m_Scene.Materials[i].Roughness, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", &m_Scene.Materials[i].Metallic, 0.05f, 0.0f, 1.0f);
            ImGui::PopID();
            ImGui::Separator();
        }

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");
        m_ViewPortWidth = ImGui::GetContentRegionAvail().x;
        m_ViewPortHeight = ImGui::GetContentRegionAvail().y;

        if (m_Renderer.GetFinalImage())
        {
            ImGui::Image(m_Renderer.GetFinalImage()->GetDescriptorSet(), { (float)m_Renderer.GetFinalImage()->GetWidth(), (float)m_Renderer.GetFinalImage()->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();
        ImGui::PopStyleVar();

        Render();
    }

    void Render()
    {
        Timer timer;

        m_Renderer.OnResize(m_ViewPortWidth, m_ViewPortHeight);
        m_Camera.OnResize(m_ViewPortWidth, m_ViewPortHeight);
        m_Renderer.Render(m_Scene, m_Camera);

        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    Renderer m_Renderer;
    Camera m_Camera;
    Scene m_Scene;
    uint32_t m_ViewPortWidth = 0, m_ViewPortHeight = 0;

    float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Ray Tracer";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<ExampleLayer>();
    app->SetMenubarCallback([app]()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                app->Close();
            }
            ImGui::EndMenu();
        }
    });
    return app;
}