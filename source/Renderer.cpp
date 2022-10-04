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

using namespace dae;

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
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	float aspectRatio{ float(m_Width) / m_Height };
	float FOV{ tanf((camera.fovAngle * TO_RADIANS) / 2.f) };

	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			////float gradient = px / static_cast<float>(m_Width);
			////gradient += py / static_cast<float>(m_Width);
			////gradient /= 2.0f;

			Vector3 rayDirection{	(2 * ((px + 0.5f) / float(m_Width)) - 1) * aspectRatio * FOV,
									(1 - 2 * ((py + 0.5f) / float(m_Height))) * FOV,
									1 };

			rayDirection = cameraToWorld.TransformVector(rayDirection);
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
				//If we hit something, set finalColor to materialColor, else keep black
				//Use HitRecord::materialIndex to find the corresponding material
				finalColor = materials[closestHit.materialIndex]->Shade();
			}


			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
