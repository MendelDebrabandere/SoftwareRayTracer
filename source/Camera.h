#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include "SDL.h"

#include <iostream>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			//todo: W2 COMPLETED

			right = Vector3::Cross(Vector3::UnitY, forward);
			right.Normalize();
			up = Vector3::Cross(forward, right);
			up.Normalize();

			Vector4 rightVector4{ right, 0 },
				upVector4{ up, 0 },
				forwardVector4{ forward, 0 },
				originVector4{ origin, 1 };


			Matrix ONB{ rightVector4, upVector4, forwardVector4, originVector4 };
			cameraToWorld = ONB;
			return ONB;
		}

		void Update(Timer* pTimer)
		{
			//todo: W2 COMPLETED

			//const float deltaTime = pTimer->GetElapsed();

			const float movementSpeed = 5.f;
			const float rotationSpeed = 0.003f;

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			if (pKeyboardState[SDL_SCANCODE_W] )
			{
				this->origin += movementSpeed * pTimer->GetElapsed() * this->forward;
			}
			if (pKeyboardState[SDL_SCANCODE_S] )
			{
				this->origin -= movementSpeed * pTimer->GetElapsed() * this->forward;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				this->origin -= movementSpeed * pTimer->GetElapsed() * this->right;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				this->origin += movementSpeed * pTimer->GetElapsed() * this->right;
			}
			if (pKeyboardState[SDL_SCANCODE_Q])
			{
				this->origin -= movementSpeed * pTimer->GetElapsed() * this->up;
			}
			if (pKeyboardState[SDL_SCANCODE_E])
			{
				this->origin += movementSpeed * pTimer->GetElapsed() * this->up;
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
			
/*			if (mouseState &  SDL_BUTTON_RMASK && mouseState & SDL_BUTTON_LMASK)
			{
				this->origin -= movementSpeed / 15.f * pTimer->GetElapsed() * Vector3{0,1,0} * static_cast<float>(mouseY);
			}
			else */if (mouseState & SDL_BUTTON_RMASK)
			{
				totalPitch -= rotationSpeed * mouseX;
				totalYaw -= rotationSpeed * mouseY;
				if (abs(totalYaw) >= float(M_PI) / 2.f)
				{
					if (totalYaw < 0)
					{
						totalYaw = -float(M_PI) / 2.001f;
					}
					else
					{
						totalYaw = float(M_PI) / 2.001f;
					}
				}
			}
			//else if (mouseState & SDL_BUTTON_LMASK)
			//{
			//	totalPitch -= rotationSpeed * mouseX;
			//	this->origin -= movementSpeed / 15.f * pTimer->GetElapsed() * this->forward * static_cast<float>(mouseY);
			//}

			Matrix finalRotation{ Matrix::CreateRotation(totalPitch, totalYaw, 0) };

			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();

		}
	};
}
