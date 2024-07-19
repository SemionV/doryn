#include <any>

#include <dory/engine/domain/services/scriptService.h>

namespace services = dory::domain::services;

TEST_CASE("Run script", "ScriptService")
{
    using DataContextType = int;

    auto scriptService = services::ScriptService<DataContextType>{};

    bool flag = false;

    scriptService.addScript("testScript", [&flag](DataContextType& context, const services::ScriptService<DataContextType>::ScriptParametersPackType& arguments)
    {
        if(arguments.contains("param1"))
        {
            auto param1 = arguments.at("param1");
            REQUIRE(std::any_cast<int>(param1) == 1);

            if(arguments.contains("param2"))
            {
                auto param2 = arguments.at("param2");
                REQUIRE(std::any_cast<std::string>(param2) == "hello!");

                flag = true;
            }
        }
    });

    int context;
    scriptService.runScript(context, "testScript", {{"param1", 1}, {"param2", std::make_any<std::string>("hello!")}});

    REQUIRE(flag == true);
}