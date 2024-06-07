#include "dependencies.h"
#include "base/domain/services/configurationService.h"

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

enum class TestFormat
{
    test
};

class SerializationServiceBundleMock: public dory::domain::services::serialization::ISerializationServiceBundle<TestFormat, SerializationServiceBundleMock>
{
public:
    template<typename T>
    std::string serializeImpl(const TestFormat& format, T&& object)
    {
        return {};
    }

    template<typename T>
    T deserializeImpl(const TestFormat& format, const std::string& source)
    {
        return {};
    }

    template<typename T>
    void deserializeImpl(const TestFormat& format, const std::string& source, T& object)
    {
    }
};

class LoggerMock: public dory::domain::services::ILogService<LoggerMock>
{
};

class FileServiceMock: public dory::domain::services::IFileService<FileServiceMock>
{
};

class FormatKeyConverterMock: public dory::domain::services::serialization::IFormatKeyConverter<TestFormat, FormatKeyConverterMock>
{
public:
    using FormatType = TestFormat;
};

struct AppSettings
{
    std::string appName;
};
REFL_TYPE(AppSettings)
        REFL_FIELD(appName)
REFL_END

TEST_CASE( "Load settings", "[configuration]" )
{
    auto logger = LoggerMock{};
    auto fileService = FileServiceMock{};
    auto serializationServiceBundle = SerializationServiceBundleMock{};
    auto formatKeyConverter = FormatKeyConverterMock{};

    auto configurationService = dory::domain::services::configuration::ConfigurationService<LoggerMock,
        FileServiceMock, SerializationServiceBundleMock, FormatKeyConverterMock>{logger, fileService, serializationServiceBundle, formatKeyConverter};

    auto appSettings = AppSettings{};

    //configurationService.load("", appSettings);
}
