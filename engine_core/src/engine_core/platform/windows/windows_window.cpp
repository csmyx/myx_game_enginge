#include "windows_window.h"
#include "../../event/key_event.h"
#include "../../event/mouse_event.h"
#include "../../event/window_event.h"
#include "../../log.h"

#include <GLFW/glfw3.h>

namespace engine_core {

	static bool s_glfw_initialized = false;

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		init(props);
	}

	WindowsWindow::~WindowsWindow() {
		shutdown();
	}

	void WindowsWindow::init(const WindowProps& props) {
		m_props = props;

		ENGINE_CORE_INFO("Creating window: {} ({}x{})", props.title, props.width, props.height);

		if (!s_glfw_initialized) {
			int success = glfwInit();
			ENGINE_CORE_ASSERT(success, "glfwInit() failed");
			if (!success) {
				ENGINE_CORE_ERROR("Failed to initialize GLFW");
				return;
			}
			s_glfw_initialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context for now

		m_window = glfwCreateWindow(
			static_cast<int>(props.width),
			static_cast<int>(props.height),
			props.title.c_str(),
			nullptr,
			nullptr);

		ENGINE_CORE_ASSERT(m_window, "glfwCreateWindow() returned nullptr");
		if (!m_window) {
			ENGINE_CORE_ERROR("Failed to create GLFW window");
			return;
		}

		glfwSetWindowUserPointer(m_window, this);
		set_vsync(true);

		// Register GLFW callbacks
		glfwSetKeyCallback(m_window, key_callback);
		glfwSetMouseButtonCallback(m_window, mouse_button_callback);
		glfwSetCursorPosCallback(m_window, cursor_position_callback);
		glfwSetScrollCallback(m_window, scroll_callback);
		glfwSetWindowSizeCallback(m_window, window_size_callback);
		glfwSetWindowCloseCallback(m_window, window_close_callback);
	}

	void WindowsWindow::shutdown() {
		if (m_window) {
			glfwDestroyWindow(m_window);
			m_window = nullptr;
		}
		// Note: glfwTerminate() is called once at engine shutdown, not per window
	}

	void WindowsWindow::on_update() {
		ENGINE_CORE_ASSERT(m_window, "on_update() called on destroyed window");
		glfwPollEvents();
	}

	unsigned int WindowsWindow::get_width() const {
		return m_props.width;
	}

	unsigned int WindowsWindow::get_height() const {
		return m_props.height;
	}

	bool WindowsWindow::should_close() const {
		return m_window && glfwWindowShouldClose(m_window);
	}

	void WindowsWindow::set_event_callback(const std::function<void(Event&)>& callback) {
		m_event_callback = callback;
	}

	void WindowsWindow::set_vsync(bool enabled) {
		m_vsync = enabled;
		// No swap interval for NO_API window; placeholder for future renderer
	}

	bool WindowsWindow::is_vsync() const {
		return m_vsync;
	}

	void* WindowsWindow::get_native_window() const {
		return static_cast<void*>(m_window);
	}

	// --- Static GLFW callbacks ---

	void WindowsWindow::key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
		auto* self = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		ENGINE_CORE_ASSERT(self, "GLFW user pointer is null in key_callback");
		if (!self || !self->m_event_callback) return;

		switch (action) {
		case GLFW_PRESS: {
			KeyPressedEvent event(key, 0);
			self->m_event_callback(event);
			break;
		}
		case GLFW_RELEASE: {
			KeyReleasedEvent event(key);
			self->m_event_callback(event);
			break;
		}
		case GLFW_REPEAT: {
			KeyPressedEvent event(key, 1);
			self->m_event_callback(event);
			break;
		}
		default:
			break;
		}
	}

	void WindowsWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/) {
		auto* self = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		ENGINE_CORE_ASSERT(self, "GLFW user pointer is null in mouse_button_callback");
		if (!self || !self->m_event_callback) return;

		switch (action) {
		case GLFW_PRESS: {
			MouseButtonPressedEvent event(button);
			self->m_event_callback(event);
			break;
		}
		case GLFW_RELEASE: {
			MouseButtonReleasedEvent event(button);
			self->m_event_callback(event);
			break;
		}
		default:
			break;
		}
	}

	void WindowsWindow::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
		auto* self = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		ENGINE_CORE_ASSERT(self, "GLFW user pointer is null in cursor_position_callback");
		if (!self || !self->m_event_callback) return;

		MouseMovedEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
		self->m_event_callback(event);
	}

	void WindowsWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		auto* self = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		ENGINE_CORE_ASSERT(self, "GLFW user pointer is null in scroll_callback");
		if (!self || !self->m_event_callback) return;

		MouseScrolledEvent event(static_cast<float>(xoffset), static_cast<float>(yoffset));
		self->m_event_callback(event);
	}

	void WindowsWindow::window_size_callback(GLFWwindow* window, int width, int height) {
		auto* self = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		ENGINE_CORE_ASSERT(self, "GLFW user pointer is null in window_size_callback");
		if (!self) return;

		self->m_props.width = static_cast<unsigned int>(width);
		self->m_props.height = static_cast<unsigned int>(height);

		if (self->m_event_callback) {
			WindowResizeEvent event(
				static_cast<unsigned int>(width),
				static_cast<unsigned int>(height));
			self->m_event_callback(event);
		}
	}

	void WindowsWindow::window_close_callback(GLFWwindow* window) {
		auto* self = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		ENGINE_CORE_ASSERT(self, "GLFW user pointer is null in window_close_callback");
		if (!self || !self->m_event_callback) return;

		WindowCloseEvent event;
		self->m_event_callback(event);
	}

	// --- Factory ---

	Window* Window::create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

} // namespace engine_core
