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
            sphere.radius = 0.2;
            sphere.Position = { 0.0f, 0.0f, 0.0f };
            sphere.Albedo = { 1.0f, 0.0f, 1.0f };
            m_Scene.Spheres.push_back(sphere);
        }
        {
            Sphere sphere;
            sphere.radius = 0.5;
            sphere.Position = { 1.0f, 0.0f, -5.0f };
            sphere.Albedo = { 0.0f, 1.0f, 1.0f };
            m_Scene.Spheres.push_back(sphere);
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
            ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Scene.Spheres[i].Albedo), 0.1f);
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