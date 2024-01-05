#pragma once
#include "Matrix.h"
#include "Vector3.h"

namespace dae
{
	class Camera
	{
	public:
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{

		}

		Vector3 origin{};
		float fovAngle{ 90.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};

		Matrix projectionMatrix{};

		Matrix GetViewMatrix()
		{
			return viewMatrix;
		}

		Matrix GetProjectionMatrix()
		{
			return projectionMatrix;
		}

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f, 0.f })
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CreateLookAtLH()
		{
			viewMatrix = Matrix::CreateLookAtLH(origin, forward, up);
		}

		void CreatePerspectiveFovLH()
		{

			float aspect = static_cast<float>(640) / static_cast<float>(480);

			float zn = 1.f;
			float zf = 1000.0f;

			projectionMatrix = Matrix::CreatePerspectiveFovLH(fov, aspect, zn, zf);
		}

		void Update(const Timer* pTimer)
		{
			Matrix rotationY = Matrix::CreateRotationY(totalYaw);
			Matrix rotationX = Matrix::CreateRotationX(totalPitch);

			Matrix finalRotation = rotationX * rotationY;
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalized();

			Vector3 localRight = Vector3::Cross(up, forward);
			localRight.Normalized();

			/*Vector3 localUp = Vector3::Cross(forward, localRight);
			localUp.Normalized();*/

			//Matrix finalTransform = scaleTransform * rotationTransform * translationTransform;
			//worldMatrix = finalTransform;


			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			float movementSpeed{ 20 };
			//Mouse Input

			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);



			//todo: W2
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				//origin.z += movementSpeed * deltaTime;
				origin += forward * deltaTime * movementSpeed;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * deltaTime * movementSpeed;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= localRight * deltaTime * movementSpeed;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += localRight * deltaTime * movementSpeed;
			}



			if (SDL_BUTTON_LMASK == mouseState)
			{
				origin -= forward * float(mouseY) * deltaTime * 10.f;
				totalYaw += float(mouseX) * deltaTime;
			}
			if (SDL_BUTTON_RMASK == mouseState)
			{

				totalPitch -= float(mouseY) * deltaTime * 2.f;
				totalYaw += float(mouseX) * deltaTime * 2.f;

			}
			if ((mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) && (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)))
			{
				origin -= up * float(mouseY) * deltaTime * 2.f;
			}

			//Update Matrices
			CreateLookAtLH();
			CreatePerspectiveFovLH(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}
	};
}

