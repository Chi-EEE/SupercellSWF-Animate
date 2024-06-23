#include "Module/Localization.h"
#include "Module/PluginContext.h"

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <codecvt>
#include <locale>

namespace sc {
	namespace Adobe {
		void Localization::Load(std::string LanguageCode) {
			PluginContext& context = PluginContext::Instance();

			std::filesystem::path localesPath = PluginContext::CurrentPath(PluginContext::PathType::Locale);
			context.logger->info("Locales storage: {}", localesPath.string());

			std::filesystem::path currentLocalePath = localesPath / (LanguageCode + ".json");

			if (!std::filesystem::exists(currentLocalePath)) {
				currentLocalePath = localesPath / "en_US.json";
			}

			context.logger->info("Active locale file: {}", currentLocalePath.string());
			std::ifstream file(currentLocalePath);

			try {
				m_locale = nlohmann::json::parse(file, nullptr, true, true);
			}
			catch (const nlohmann::json::exception& exception)
			{
				context.logger->error("Failed to parse locale file: {}", exception.what());
				context.Trace("Failed to load localization");
				context.Trace(exception.what());
			}
		}

		std::string Localization::GetUTF(const std::string& TID) {
			auto value = m_locale[TID];
			if (value.is_string()) {
				return value;
			}
			else {
				return TID;
			}
		}

		std::u16string Localization::ToUtf16(const std::string& string) {
			static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			return convert.from_bytes(string);
		}

		std::string Localization::ToUtf8(const std::u16string& string) {
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			return convert.to_bytes(string);
		}
	}
}
