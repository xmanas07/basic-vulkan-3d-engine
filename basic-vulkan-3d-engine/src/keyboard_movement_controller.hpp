#pragma once


#include "lve_game_object.hpp"
#include "lve_window.hpp"
namespace lve {
	class KeyboardMovementController {
		static constexpr float MAX_ALOWABLE_FRAMETIME = .067f;

	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_SPACE;
			int moveDown = GLFW_KEY_C;
			int rotateRight = GLFW_KEY_E;
			int rotateLeft = GLFW_KEY_Q;
			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
			int toggleLookCenter = GLFW_KEY_L;
		};

		void moveInSpace(GLFWwindow* window, float dt, LveGameObject& gameObject);
		void moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);

		KeyMappings keys{};
		float moveSpeed{ 1.5f };
		float lookSpeed{ 1.5f };

	};
}