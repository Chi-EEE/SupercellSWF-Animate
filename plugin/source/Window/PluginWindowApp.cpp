#include "Window/PluginWindowApp.h"
#include "Module/PluginContext.h"

#include "ResourcePublisher/ResourcePublisher.h"

#include <random>

namespace sc {
	namespace Adobe {
		PluginWindowApp::PluginWindowApp()
		{};

		bool PluginWindowApp::OnInit() {
			PluginContext& context = PluginContext::Instance();
			PluginSessionConfig& config = PluginSessionConfig::Instance();

			FCM::StringRep16 documentPathPtr;
			config.document->GetPath(&documentPathPtr);

			fs::path documentName = fs::path((const char16_t*)documentPathPtr).filename();
			context.falloc->Free(documentPathPtr);

			window = new PluginWindow(
				context.locale.GetString("TID_WINDOW_TITLE", documentName.u16string().c_str())
			);

			{
				wxIcon icon;
				fs::path iconPath = PluginContext::CurrentPath(PluginContext::PathType::Assets) / "window.ico";
				icon.LoadFile(iconPath.wstring(), wxBITMAP_TYPE_ICO);
				if (icon.IsOk()) {
					window->SetIcon(icon);
				}
			}

			{
				fs::path animFolder = PluginContext::CurrentPath(PluginContext::PathType::Assets) / "loading";
				std::vector<std::wstring> files;

				for (const auto& entry : fs::directory_iterator(animFolder)) {
					files.push_back(entry.path().wstring());
				}

				size_t animation_index = 0;

				{
					std::random_device seed;
					std::mt19937 generator(seed());
					std::uniform_int_distribution<> distr(0, files.size());

					animation_index = distr(generator);
				}

				wxAnimationCtrl* animation = new wxAnimationCtrl(window, wxID_ANY, wxNullAnimation, wxPoint(335, 5));
				animation->LoadFile(files[animation_index]);
				animation->Play();
			}

			window->Show();
			SetTopWindow(window);

			return true;
		}
	}
}