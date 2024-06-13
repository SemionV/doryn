#include <iostream>
#include "client.h"

#include <boost/dll/import.hpp>

int main()
{
    boost::dll::fs::path shared_library_path("testPlugin.dll");

    /*auto pluginFactory = boost::dll::import_alias<std::unique_ptr<dory::plugin::Plugin<client::Registry>>()>(             // type of imported symbol must be explicitly specified
            shared_library_path,                                            // path to library
            "pluginFactory",                                                // symbol to import
            boost::dll::load_mode::append_decorations                       // do append extensions and prefixes
    );*/

    auto registry = client::Registry{};
    /*auto plugin = pluginFactory();
    plugin->initialize(registry);*/
    auto cpp11_func = boost::dll::import_symbol<int(std::string&&)>(
            "testPlugin.dll", "i_am_a_cpp11_function"
    );


    std::cout << "call test function from plugin: " << cpp11_func("") << "\n";
    return 0;
}