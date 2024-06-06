#include "dependencies.h"
#include "base/domain/services/configurationService.h"

struct AppSettings
{
    std::string appName;
};

class SerializationServiceMock: public dory::domain::services::serialization::ISerializationService<SerializationServiceMock>
{
public:
    template<typename T>
    static std::string serializeImpl(T&& object)
    {
        return "";
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

TEST_CASE( "Load settings", "[configuration]" )
{

}
