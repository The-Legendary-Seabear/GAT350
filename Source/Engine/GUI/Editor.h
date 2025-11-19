#pragma once
#include "Resources/ResourceManager.h"
#include"ImGuiFileDialog.h"

namespace neu {
	class Editor {
	public:
		void Begin();
		void UpdateGui(class Scene& scene);

		static void ShowTexture(const Texture& texture, float width, float height);
		template<typename T>
		static bool GetDialogResource(res_t<T>& resource, const std::string& dialogName, const std::string& title, const std::string& filetype);


	private:
		bool m_active = true;
		class GUI* m_selected = nullptr;
	};

	template<typename T>
	inline bool Editor::GetDialogResource(res_t<T>& resource, const std::string& dialogName, const std::string& title, const std::string& filetype)
	{
		if (ImGui::IsItemClicked(0))
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.flags = ImGuiFileDialogFlags_Modal;
			ImGuiFileDialog::Instance()->OpenDialog(dialogName, title, filetype.c_str(), config);
		}
		if (ImGuiFileDialog::Instance()->Display(dialogName)) {
			if (ImGuiFileDialog::Instance()->IsOk()) {
				std::string filename = ImGuiFileDialog::Instance()->GetFilePathName();
				resource = Resources().Get<T>(file::GetRelativePath(filename));
			}
			else {
				resource = res_t<T>();
			}

			ImGuiFileDialog::Instance()->Close();
		}

		return true;
	}

}