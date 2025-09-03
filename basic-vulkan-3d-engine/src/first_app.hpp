#pragma  once

#include "bve_device.hpp"
#include "bve_game_object.hpp"
#include "bve_renderer.hpp"
#include "bve_window.hpp"
#include "bve_descriptors.hpp"

//std
#include <memory>
#include <vector>

namespace bve {
	class FirstApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 800;

		FirstApp();
		~FirstApp();
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();
		void addGameObject(bve::BveGameObject gameObject);

		BveWindow bveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		BveDevice bveDevice{ bveWindow };
		BveRenderer bveRenderer{ bveWindow, bveDevice };

		// note: order of declarations matters
		std::unique_ptr<BveDescriptorPool> globalPool{};
		BveGameObject::Map gameObjects;
	};
} //namespace bve