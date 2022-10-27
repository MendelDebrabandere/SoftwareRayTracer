//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

#include <future> // async
#include <ppl.h> // parallel

using namespace dae;

//#define ASYNC
#define PARALLEL_FOR

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	camera.CalculateCameraToWorld();

	const float aspectRatio{ float(m_Width) / m_Height };
	const float FOV{ tanf((camera.fovAngle * TO_RADIANS) / 2.f) };

	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const uint32_t numPixels{ uint32_t(m_Width * m_Height) };

#if defined(ASYNC)
	//Async execution
	const uint32_t numCores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> async_futures{};
	const uint32_t numPixelsPerTask = numPixels / numCores;
	uint32_t numUnassignedPixels = numPixels % numCores;
	uint32_t currPixelIndex = 0;

	for (uint32_t coreId{ 0 }; coreId < numCores; ++coreId)
	{
		uint32_t taskSize = numPixelsPerTask;
		if (numUnassignedPixels > 0)
		{
			++taskSize;
			--numUnassignedPixels;
		}

		async_futures.push_back(
			std::async(std::launch::async, [=, this]
				{
					const uint32_t pixelIndexEnd = currPixelIndex + taskSize;
					for (uint32_t pixelIndex{ currPixelIndex }; pixelIndex < pixelIndexEnd; ++pixelIndex)
					{
						RenderPixel(pScene, pixelIndex,  FOV, aspectRatio, camera, lights, materials);
					}
				}
			)
		);

		currPixelIndex += taskSize;
	}

	// wait till all tasks are finished
	for (const std::future<void>& f : async_futures)
	{
		f.wait();
	}

#elif defined(PARALLEL_FOR)
	//Parallel-for exec
	concurrency::parallel_for(0u, numPixels, [=, this](int i) {
		RenderPixel(pScene, i, FOV, aspectRatio, camera, lights, materials);
		});

#else
	//Syncronioyus exec
	for (uint32_t i{ 0 }; i < numPixels; ++i)
	{
		RenderPixel(pScene, i, FOV, aspectRatio, camera, lights, materials);
	}


#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void dae::Renderer::RenderPixel(Scene* pScene,
	uint32_t pixelIndex,
	float fov,
	float aspectRatio,
	const Camera& camera,
	const std::vector<Light>& lights,
	const std::vector<Material*>& materials) const
{
	const int px = pixelIndex % m_Width;
	const int py = pixelIndex / m_Width;


	Vector3 rayDirection{ (2 * ((px + 0.5f) / float(m_Width)) - 1) * aspectRatio * fov,
						(1 - 2 * ((py + 0.5f) / float(m_Height))) * fov,
						1 };

	rayDirection = camera.cameraToWorld.TransformVector(rayDirection);
	rayDirection.Normalize();

	//For each pixel ...
	//... Ray direction calculations above ...
	//Ray we are casting from the camera towards each pixel
	Ray viewRay{ camera.origin, rayDirection };

	//Color to write to the color buffer (default = black)
	ColorRGB finalColor{};

	//HitRecord containing more information about a potential hit
	HitRecord closestHit{};
	pScene->GetClosestHit(viewRay, closestHit);


	if (closestHit.didHit)
	{

		//finalColor = materials[closestHit.materialIndex]->Shade();

		bool ObservedArea{ false };
		bool Radiance{ false };
		bool BRDF{ false };

		switch (m_CurrentLightingMode)
		{
		case LightingMode::ObservedArea:
			ObservedArea = true;
			break;
		case LightingMode::Radiance:
			Radiance = true;
			break;
		case LightingMode::BRDF:
			BRDF = true;
			break;
		case LightingMode::Combined:
			ObservedArea = true;
			Radiance = true;
			BRDF = true;
			break;
		}


		for (const Light& light : lights)
		{
			ColorRGB tempCycleColor{ colors::White };

			Vector3 lightDirection{ -closestHit.origin + light.origin };
			float directionMagnitude{ lightDirection.Magnitude() };
			lightDirection = lightDirection / directionMagnitude;

			if (ObservedArea)
			{
				float dotProd{ Vector3::Dot(closestHit.normal, lightDirection) };
				if (dotProd < 0.f)
					continue;
				tempCycleColor *= ColorRGB{ dotProd, dotProd, dotProd };
			}
			if (Radiance)
			{
				tempCycleColor *= LightUtils::GetRadiance(light, closestHit.origin);
			}



			float offSet{ 0.01f };

			Ray ray{};
			ray.origin = light.origin;
			ray.direction = -lightDirection;
			ray.min = offSet;
			ray.max = directionMagnitude - offSet;

			if (m_ShadowsEnabled && pScene->DoesHit(ray))
			{
				continue;
			}



			if (BRDF)
			{
				tempCycleColor *= materials[closestHit.materialIndex]->Shade(closestHit, lightDirection, -rayDirection);
			}


			finalColor += tempCycleColor;
		}
	}


	//Update Color in Buffer
	finalColor.MaxToOne();

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));

}



bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::CycleLightingMode()
{
	switch (m_CurrentLightingMode)
	{
	case LightingMode::ObservedArea:
		m_CurrentLightingMode = LightingMode::Radiance;
		break;
	case LightingMode::Radiance:
		m_CurrentLightingMode = LightingMode::BRDF;
		break;
	case LightingMode::BRDF:
		m_CurrentLightingMode = LightingMode::Combined;
		break;
	case LightingMode::Combined:
		m_CurrentLightingMode = LightingMode::ObservedArea;
		break;
	}
}
