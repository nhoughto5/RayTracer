#pragma once
#include "Walnut/Image.h"
#include <memory>
#include "Walnut/Random.h"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
public:
    Renderer() = default;

    void Render(const Scene& scene, const Camera& camera);
    void OnResize(uint32_t width, uint32_t height);
    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:

    struct HitPayload
    {
        float HitDistance;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;
        int ObjectIndex;
    };

    HitPayload TraceRay(const Ray& ray);
    HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
    HitPayload Miss(const Ray& ray);
    glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;
    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;
};

