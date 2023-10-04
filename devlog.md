# Development log

Tasks:
* Define virtual constructors and default move members for Interface classes
* Return const& from EntityRepository and build a method for mutations
* Generalize core type of Matricies, so that it can support also double
* Do not include internal header files into dependencies.h(precompiled headers), because it increases compilation time a lot
* Stop engine using Event and FrameService
* Render Meshes
* Camera Control
* Mouse picking
* Animation
* Collision detection
* Dynamic module loading(plugin system)
* Integrate C# dotnet core as scripting engine(https://learn.microsoft.com/en-us/dotnet/core/tutorials/netcore-hosting)
* Scene
* Spatial partition
* Path finding
* Build for Linux
* Build for MacOS
* Networking/Mutliplayer
* Vioce control

**25.09.23**
* Task done: Compile with Visual C++ compiler

**24.09.23**
* Task done: Refactor Project class, make it just a service container, develop separate class to hold application logic
* Task done: Move src/lib out of project folder(in order to make vs code intellisence work properly and light)

**23.09.23**
* Task done: Implement dynamic window open/close with views

**22.09.23**
* Task done: Generalize DataContext type for all the types in the engine, so that it is possible to work with derived classes of DataContext in the Project class  without casting
* Task done: Make order with engine namespaces, folders and files
* Task done: Group all the engine components, repositories, factories, etc in Project class

**20.09.23**
* Task done: Develop Unit Tests for EntitiyRepository
* Task done: Introduce logic level Service to manage complex data like Window and View(in order to delete window and views properly)
* Task done: Add Groups of Controllers to Engine
* Task done: Assign ComponentId to every Controller, attach view to controller inderectly with view-controller entity
* Task done: Remove Engine::dataContext and pass it in the Engine::update as parameter instead
* Task done: Introduce Engene EventHub and invoke "after initialize event"
* Task done: Implement dynamic Controller creating and deletion on Engine

**16.09.23**
* Task done: Enhance EntityStorage:
    * introduce a template parameter for entity id(might be int, uint, ulong, etc)
    * always return a pointer from all the search functions instead of optional or optional to pointer

**15.09.23**
* Task done: Develop WindowController instead of WindowSystem
* Task done: Delete Window and View entities attached to GlfwWindow when iut is closed
* Task done: Use new View structure in ViewController for OpenGL
* Task done: Clean up the old WindowSystem code(remove)
* Task done: Implement SystemConsole and Win32Window systems as Controllers

**14.09.23**
* Task done: Instal gl3w library with build/install-dependencies.sh

**19.06.23**
* I should swap roles of Engine and FrameService entities. FrameService should have a reference to Engine and should call update function of Engine. Engine should not know anything about FrameService, just recieve timeStep und manage update of game state via controllers.