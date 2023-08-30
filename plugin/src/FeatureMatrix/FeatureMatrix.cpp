#include "FeatureMatrix/FeatureMatrix.h"

namespace sc {
	namespace Adobe {
		FeatureMatrix::FeatureMatrix()
		{
		}

		FeatureMatrix::~FeatureMatrix()
		{
		}

		void FeatureMatrix::Init(FCM::IFCMCallback* callback)
		{
			ASSERT(callback != nullptr);
			m_callback = callback;

			fs::path featuresPath = Utils::CurrentPath() / "../res/Features.json";

			std::ifstream file(featuresPath);
			json features = json::parse(file, nullptr, true, true);

			if (features.is_array()) {
				for (auto feature : features) {
					ReadFeature(feature);
				}
			}
		}

		void FeatureMatrix::ReadFeature(json& feature) {
			Feature featureItem = Feature(feature["supported"]);
			std::string featureName = feature["name"];
			auto featureProperties = feature["properties"];

			if (featureProperties.is_array()) {
				for (auto& featureProperty : featureProperties) {
					if (featureProperty.is_object()) {
						ReadProperty(featureItem, featureProperty);
					}
				}
			}

			m_features.insert(std::pair(featureName, featureItem));
		}

		void FeatureMatrix::ReadProperty(Feature& feature, json& property) {
			std::string propertyDefault = "";
			bool isSupported = false;

			if (property["default"].is_string()) {
				propertyDefault = property["default"];
			}

			if (property["supported"].is_boolean()) {
				isSupported = property["supported"];
			}

			Property propertyItem = Property(propertyDefault, isSupported);

			if (property["values"].is_array()) {
				for (auto value : property["values"]) {
					ReadValue(propertyItem, value);
				}
			}

			std::string propertyName = property["name"];
			feature.AddProperty(propertyName, propertyItem);
		}

		void FeatureMatrix::ReadValue(Property& property, json& value) {
			property.AddValue(value["name"], Value(value["supported"]));
		}

		FCM::Result FeatureMatrix::IsSupported(CStringRep16 featureName, FCM::Boolean& isSupported)
		{
			std::string featureNameStr = Utils::ToUtf8((const char16_t*)featureName);

			if (featureNameStr.empty()) {
				isSupported = true;
				return FCM_SUCCESS;
			}

			Feature* feature = FindFeature(featureNameStr);
			if (feature == NULL)
			{
				isSupported = false;
			}
			else
			{
				isSupported = feature->IsSupported();
			}
			return FCM_SUCCESS;
		}

		FCM::Result FeatureMatrix::IsSupported(
			CStringRep16 featureName,
			CStringRep16 propertyName,
			FCM::Boolean& isSupported)
		{
			std::string featureNameStr = Utils::ToUtf8((const char16_t*)featureName);

			if (featureNameStr.empty()) {
				isSupported = true;
				return FCM_SUCCESS;
			}

			Feature* feature = FindFeature(featureNameStr);
			if (feature == NULL)
			{
				isSupported = false;
			}
			else
			{
				if (!feature->IsSupported())
				{
					isSupported = false;
				}
				else
				{
					std::string propertyNameStr = Utils::ToUtf8((const char16_t*)propertyName);

					if (propertyNameStr.empty()) {
						isSupported = true;
						return FCM_SUCCESS;
					}

					Property* property = feature->FindProperty(propertyNameStr);
					if (property == NULL)
					{
						isSupported = false;
					}
					else
					{
						isSupported = property->IsSupported();
					}
				}
			}
			return FCM_SUCCESS;
		}

		FCM::Result FeatureMatrix::IsSupported(
			CStringRep16 inFeatureName,
			CStringRep16 propertyName,
			CStringRep16 valueName,
			FCM::Boolean& isSupported)
		{
			std::string featureNameStr = Utils::ToUtf8((const char16_t*)inFeatureName);

			if (featureNameStr.empty()) {
				isSupported = true;
				return FCM_SUCCESS;
			}

			Feature* feature = FindFeature(featureNameStr);
			if (feature == NULL)
			{
				isSupported = false;
			}
			else
			{
				if (!feature->IsSupported())
				{
					isSupported = false;
				}
				else
				{
					std::string propertyNameStr(Utils::ToUtf8((const char16_t*)propertyName));

					if (propertyNameStr.empty()) {
						isSupported = true;
						return FCM_SUCCESS;
					}

					Property* pProperty = feature->FindProperty(propertyNameStr);
					if (pProperty == NULL)
					{
						isSupported = false;
					}
					else
					{
						if (!pProperty->IsSupported())
						{
							isSupported = false;
						}
						else
						{
							std::string valueNameStr(Utils::ToUtf8((const char16_t*)valueName));

							if (valueNameStr.empty()) {
								isSupported = false;
								return FCM_SUCCESS;
							}

							Value* value = pProperty->FindValue(valueNameStr);
							if (value == NULL)
							{
								isSupported = false;
							}
							else
							{
								isSupported = value->IsSupported();
							}
						}
					}
				}
			}
			return FCM_SUCCESS;
		}

		FCM::Result FeatureMatrix::GetDefaultValue(
			CStringRep16 featureName,
			CStringRep16 propertyName,
			FCM::VARIANT& output)
		{
			FCM::Result res = FCM_INVALID_PARAM;

			std::string featureNameStr = Utils::ToUtf8((const char16_t*)featureName);
			std::string propertyNameStr = Utils::ToUtf8((const char16_t*)propertyName);

			Property* property = NULL;
			Feature* feature = FindFeature(featureNameStr);
			if (feature != NULL && feature->IsSupported())
			{
				property = feature->FindProperty(propertyNameStr);
				if (property != NULL && property->IsSupported())
				{
					std::string stringValue = property->GetDefault();
					if (stringValue.empty()) {
						return FCM_INVALID_PARAM;
					}

					std::istringstream iss(stringValue);
					res = FCM_SUCCESS;
					switch (output.m_type) {
					case kFCMVarype_UInt32:
						iss >> output.m_value.uVal;
						break;
					case kFCMVarype_Float:
						iss >> output.m_value.fVal;
						break;
					case kFCMVarype_Bool:
						output.m_value.bVal = (stringValue == "true");
						break;
					case kFCMVarype_CString:
					{
						std::u16string u16StringValue = Utils::ToUtf16(stringValue);
						output.m_value.strVal = (FCM::StringRep16)u16StringValue.c_str();
					}
					break;
					case kFCMVarype_Double:
						iss >> output.m_value.dVal;
						break;
					default:
						ASSERT(0);
						res = FCM_INVALID_PARAM;
						break;
					}
				}
			}

			return res;
		}

		Feature* FeatureMatrix::FindFeature(const std::string& inFeatureName)
		{
			FeatureMap::iterator itr = m_features.find(inFeatureName);
			if (itr != m_features.end())
			{
				return &itr->second;
			}
			return NULL;
		}
	}
}