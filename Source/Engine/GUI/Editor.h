#pragma once

namespace neu {
	class Editor {
	public:
		void Begin();
		void UpdateGui(class Scene& scene);

	private:
		bool m_active = true;
		class Object* m_selected = nullptr;
	};
}