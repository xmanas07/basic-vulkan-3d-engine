#pragma once

#include "lve_model.hpp"

//libs
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
//std
#include <memory>

namespace lve {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };

		// Public interface (Euler angles in radians)
		glm::vec3 rotation{ 0.f };

	private:
		// Internal representation
		glm::quat orientation = glm::quat(1.f, 0.f, 0.f, 0.f);

	public:
		glm::quat getOrientation() { 
			// sync quaternion from Euler
			orientation = glm::quat(rotation); 
			return orientation; }
		glm::mat4 mat4() {
			// sync quaternion from Euler
			orientation = glm::quat(rotation);
			glm::mat4 rot = glm::mat4_cast(orientation);
			glm::mat4 trans = glm::translate(glm::mat4{ 1.f }, translation);
			glm::mat4 scl = glm::scale(glm::mat4{ 1.f }, scale);
			return trans * rot * scl;
		}
	};

	class LveGameObject
	{
	public:
		using id_t = unsigned int;
		static LveGameObject createGameObject() {
			static id_t currentId = 0;
			return LveGameObject{ currentId++ };
		}

		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;

		id_t getId() { return id; }
		std::shared_ptr<LveModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};
		


	private:
		LveGameObject(id_t objId) :id{ objId } {}

		id_t id;
	};

}

