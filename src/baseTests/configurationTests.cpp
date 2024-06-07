#include "dependencies.h"
#include "base/domain/services/configurationService.h"

enum class TestFormat
{
    unknown,
    test,
    test2
};

struct UserPreferences
{
    dory::configuration::RecursiveSection section;
    std::string language;
    int difficulty = 0;
};
REFL_TYPE(UserPreferences)
        REFL_FIELD(section)
        REFL_FIELD(language)
        REFL_FIELD(difficulty)
REFL_END

struct AppSettings
{
    dory::configuration::RecursiveSection section;
    std::string appName;
    std::string description;
    UserPreferences userPreferences;
};
REFL_TYPE(AppSettings)
        REFL_FIELD(section)
        REFL_FIELD(appName)
        REFL_FIELD(description)
        REFL_FIELD(userPreferences)
REFL_END

struct Constants
{
    struct AppSettings
    {
        static const constexpr std::string_view appSettingsFileName = "appSettingsFileName.test";
        static const constexpr std::string_view appSettingsFileContent = "appSettingsFileContent";
        static const constexpr std::string_view appSettingsUserFileName = "appSettingsFileName.user.test2";
        static const constexpr std::string_view appSettingsUserFileContent = "appSettingsUserFileContent";
        static const constexpr std::string_view appName = "app";
        static const constexpr std::string_view description = "description";
    };

    struct UserPreferences
    {
        static const constexpr std::string_view userPreferencesFileName = "preferences.test";
        static const constexpr std::string_view userPreferencesFileContent = "preferencesContent";
        static const constexpr std::string_view userPreferencesUserFileName = "preferences.user.test";
        static const constexpr std::string_view userPreferencesUserFileContent = "preferencesUserContent";
        static const constexpr std::string_view language = "japan";
        static const constexpr int difficulty = 3;
    };
};

class SerializationServiceMock: public dory::domain::services::serialization::ISerializationService<SerializationServiceMock>
{
public:
    template<typename T>
    static std::string serializeImpl(T&& object)
    {
        return {};
    }

    template<typename T>
    static T deserializeImpl(const std::string& source)
    {
        return {};
    }

    template<typename T>
    static void deserializeImpl(const std::string& source, T&& object)
    {
    }
};

class SerializationServiceBundleMock: public dory::domain::services::serialization::ISerializationServiceBundle<TestFormat, SerializationServiceBundleMock>
{
public:
    template<typename T>
    static std::string serializeImpl(const TestFormat& format, T&& object)
    {
        return {};
    }

    template<typename T>
    static T deserializeImpl(const TestFormat& format, const std::string& source)
    {
        return {};
    }

    static void deserializeImpl(const TestFormat& format, const std::string& source, AppSettings& object)
    {
        if(format == TestFormat::test)
        {
            if(source == Constants::AppSettings::appSettingsFileContent)
            {
                object.appName = Constants::AppSettings::appName;
                object.section.loadFrom.emplace_back( Constants::AppSettings::appSettingsUserFileName );
                object.userPreferences.section.loadFrom.emplace_back( Constants::UserPreferences::userPreferencesFileName );
            }
        }
        else if(format == TestFormat::test2)
        {
            if(source == Constants::AppSettings::appSettingsUserFileContent)
            {
                object.description = Constants::AppSettings::description;
            }
        }
    }

    static void deserializeImpl(const TestFormat& format, const std::string& source, UserPreferences& object)
    {
        if(format == TestFormat::test)
        {
            if(source == Constants::UserPreferences::userPreferencesFileContent)
            {
                object.language = Constants::UserPreferences::language;
                object.section.loadFrom.emplace_back( Constants::UserPreferences::userPreferencesUserFileName );
            }
            else if(source == Constants::UserPreferences::userPreferencesUserFileContent)
            {
                object.difficulty = Constants::UserPreferences::difficulty;
            }
        }
    }
};

class FileServiceMock: public dory::domain::services::IFileService<FileServiceMock>
{
public:
    static std::string readImpl(const std::filesystem::path& filePath)
    {
        if(filePath == Constants::AppSettings::appSettingsFileName)
        {
            return std::string{ Constants::AppSettings::appSettingsFileContent };
        }
        else if(filePath == Constants::AppSettings::appSettingsUserFileName)
        {
            return std::string{ Constants::AppSettings::appSettingsUserFileContent };
        }
        else if(filePath == Constants::UserPreferences::userPreferencesFileName)
        {
            return std::string{ Constants::UserPreferences::userPreferencesFileContent };
        }
        else if(filePath == Constants::UserPreferences::userPreferencesUserFileName)
        {
            return std::string{ Constants::UserPreferences::userPreferencesUserFileContent };
        }

        return {};
    }

    static void writeImpl(const std::filesystem::path& filePath, const std::string& content)
    {
    }
};

class FormatKeyConverterMock: public dory::domain::services::serialization::IFormatKeyConverter<TestFormat, FormatKeyConverterMock>
{
public:
    using FormatType = TestFormat;

    static TestFormat getFormatImpl(const std::filesystem::path& path)
    {
        if(path == Constants::AppSettings::appSettingsFileName
            || path == Constants::UserPreferences::userPreferencesFileName
            || path == Constants::UserPreferences::userPreferencesUserFileName)
        {
            return TestFormat::test;
        }
        else if(path == Constants::AppSettings::appSettingsUserFileName)
        {
            return TestFormat::test2;
        }

        return TestFormat::unknown;
    }
};

TEST_CASE( "Load settings", "[configuration]" )
{
    auto logger = dory::domain::services::MultiSinkLogService{};
    auto fileService = FileServiceMock{};
    auto serializationServiceBundle = SerializationServiceBundleMock{};
    auto formatKeyConverter = FormatKeyConverterMock{};

    auto configurationService = dory::domain::services::configuration::ConfigurationService<decltype(logger),
        FileServiceMock, SerializationServiceBundleMock, FormatKeyConverterMock>{logger, fileService, serializationServiceBundle, formatKeyConverter};

    auto appSettings = AppSettings{};

    configurationService.load(Constants::AppSettings::appSettingsFileName, appSettings);

    REQUIRE(appSettings.section.loadFrom.empty());
    REQUIRE(appSettings.appName == Constants::AppSettings::appName);
    REQUIRE(appSettings.description == Constants::AppSettings::description);
    REQUIRE(appSettings.userPreferences.section.loadFrom.empty());
    REQUIRE(appSettings.userPreferences.language == Constants::UserPreferences::language);
    REQUIRE(appSettings.userPreferences.difficulty == Constants::UserPreferences::difficulty);
}
