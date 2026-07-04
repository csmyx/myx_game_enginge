#pragma once

#include "../../window.h"

struct GLFWwindow;

namespace engine_core {

	class WindowsWindow : public Window {
	public:
		explicit WindowsWindow(const WindowProps& props);
		~WindowsWindow() override;

		void on_update() override;
		unsigned int get_width() const override;
		unsigned int get_height() const override;
		bool should_close() const override;
		void set_event_callback(const std::function<void(Event&)>& callback) override;
		void set_vsync(bool enabled) override;
		bool is_vsync() const override;
		void set_clear_color(float r, float g, float b, float a) override;
		void clear() override;
		void swap_buffers() override;
		void* get_native_window() const override;

	private:
		void init(const WindowProps& props);
		void shutdown();

		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void window_size_callback(GLFWwindow* window, int width, int height);
		static void window_close_callback(GLFWwindow* window);

		GLFWwindow* m_window = nullptr;
		WindowProps m_props;
		bool m_vsync = true;
		std::function<void(Event&)> m_event_callback;
	};

} // namespace engine_core
