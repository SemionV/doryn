#include <dory/client/client.h>


#ifdef WIN32
#include <windows.h>
#include <winnt.h>
#endif

#ifdef __unix__
int main()
#endif
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    auto registry = client::Registry{};

    std::cout << "current dir: " << std::filesystem::current_path().string() << std::endl;

    auto libraryService = dory::domain::services::module::DynamicLibraryService{};
    auto library = libraryService.load("test module", "modules/testModule");
    if(library)
    {
        auto testModule2 = library->loadModule<client::Registry>("testModule2", registry);
        testModule2->attach(dory::LibraryHandle{ library });
    }

    return 0;
}