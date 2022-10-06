#include "Renderer.h"

namespace Utils
{
    static uint32_t ConvertToRGBA(const glm::vec4& color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);

        return (a << 24) | (b << 16) | (g << 8) | r;
    }
}
void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage)
    {
        if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
        {
            return;
        }
        m_FinalImage->Resize(width, height);
    }
    else
    {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete m_ImageData;
    m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    Ray ray;

    ray.Origin = camera.GetPosition();

    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            ray.Direction = camera.GetRayDirections()[(y * m_FinalImage->GetWidth()) + x];

            glm::vec4 color = TraceRay(scene, ray);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[(y * m_FinalImage->GetWidth()) + x] = Utils::ConvertToRGBA(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
    if (scene.Spheres.size() == 0)
    {
        return glm::vec4(0, 0, 0, 1.0f);
    }

    const Sphere* closestSphere = nullptr;
    float hitDistance = FLT_MAX;
    for (const Sphere& sphere : scene.Spheres)
    {
        glm::vec3 origin = ray.Origin - sphere.Position;
        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(origin, ray.Direction);
        float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

        // Quadratic discriminate
        float d = b * b - 4.0f * a * c;

        if (d < 0.0f)
        {
            continue;
        }

        //float t0 = (-b + glm::sqrt(d)) / (2.0f * a);
        float closestT = (-b - glm::sqrt(d)) / (2.0f * a);

        if (closestT < hitDistance)
        {
            hitDistance = closestT;
            closestSphere = &sphere;
        }
    }

    if (closestSphere == nullptr)
    {
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
    glm::vec3 origin = ray.Origin - closestSphere->Position;
    glm::vec3 hitPoint = origin + ray.Direction * hitDistance;
    glm::vec3 normal = glm::normalize(hitPoint);

    float light = glm::max(glm::dot(normal, -lightDir), 0.0f);

    return glm::vec4(closestSphere->Albedo * light, 1.0f);
}