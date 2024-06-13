# Development log

Tasks:
* Dynamic module loading(plugin system)
  * Configure and load plugins
  * Subscribe to the events and attach Controllers from plugins
  * Hot reload of plugins
  * Runtime CPP compilation(optional)
* Mouse picking(redirect event to the corresponding event hub of a view)
* Integrate C# dotnet core as scripting engine(https://learn.microsoft.com/en-us/dotnet/core/tutorials/netcore-hosting)
* OpenGL Renderer
  * Static mapping of Vertex Attributes
  * Fill Buffers with mapped data
  * Batch rendering
  * Render Meshes
* Multithreaded rendering
* Networking/Mutliplayer
* Camera Control
* Animation
* Collision detection
* Scene
* Serialization of scene state
* Spatial partition: https://youtube.com/watch?v=eED4bSkYCB8&si=uOxianrO-zNMLAG3, https://youtu.be/MzUxOe5x24w?si=3PLIEqPTq8w8RLet, https://youtu.be/4r_EvmPKOvY?si=iINNvyGsoYzdaftY, https://youtu.be/6BIfqfC1i7U?si=rJSwRj3xuANSJZ4o
* Path finding
* Build for MacOS
* Voice control

**12.06.24**
* Task done: Optimize compilation time

**07.06.24**
* Task done: I18n

**06.06.24**
* Task done: SerializationService bundle: set of multiple serialization services mapped to distinct file formats

**30.05.24**
* Task done: Support of dictionaries in YAML and JSON(needed to configure i18n and other resources)

**28.05.24**
* Task done: Configuration subsystem

**25.05.24**
* Task done: Error handling and logging

**09.05.24**
* Task done: YAML Serializer/Deserializer

**05.05.24**
* Task done: JSON Serializer/Deserializer
* YAML Serializer/Deserializer
  * Integrate rapid-yaml library

**07.04.24**
* Task done: Build for Linux
  * Cancel getchcar() on waiting
  * Do polling only if stdin is connected to a terminal
  * InputDevice per OS. Input/Output event hubs, concurrent polling, Open new Terminal
  * Generalize TerminalControllers

**30.03.24**
* Task done: Reimplement MatrixCalculator using  static polymorphism
* Task done: Generalize core type of Matricies, so that it can support also double

**29.03.24**
Update for last  months
* Task done: Reimplement EntityRepository using static polymorphism, no virtual class interfaces
* Task done: Reimplement ServiceContainer
* Task done: Rework project structure: engine should be a single static library unit, game logic can be a shared library(dll)
* Task done: Binary Layout Serializer unit tests

**29.12.23**
* Task done: Integrate Google Benchmark for microbenchmarking

**14.12.23**
* Task done: ObjectVisitor unit tests

**30.10.23**
* Task done: Return const& from EntityRepository
* Task done: Consider to allocate controller classes on stack(use const T& instead of std::shared_ptr/std::unique_ptr)
    * The conclusion is that heap allocation is needed for dynamic component instantiation(ViewControllers for instance, which are constracted on the fly)
    * Stack memory can be overused, what can lead to stack overflow(limit is usually from 1 to 8MB)

**27.10.23**
* Task done: Refactor Program and Uniform Blocks
* Task done: Move OpenglProcedures to a different namespace
* Task done: Destroy OpenGL buffers and recreate them when assigning data to an object

**25.10.23**
* Task done: Define virtual constructors and default move members for Interface classes

**17.10.23**
* Task done(some time ago): Do not include internal header files into dependencies.h(precompiled headers), because it increases compilation time a lot
* Task done(some time ago): Stop engine using Event and FrameService

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