# Development log

Tasks:
* Profiling
  * Frame stream
  * Conditional compilation of Profiling feature into the resulting binaries
  * Measure duration of the pipeline steps
  * Analyze the profiling data
* Run the whole pipeline in the LoopService, but control the subsystems withing the controllers, who are treating time in different ways. For instance,
  run window system event polling each iteration, as well as rendering subsystem, but run animation system and game logic only every 16.6 ms, and so on.
* Job System(Thread pool)
* Camera
  * Handle user input to a window
  * Free camera
  * Followup camera 
* Window view layout(automatic calculation of the views inside a window), update view with layout service on window-resize event
* Edge-rendering(render edges of a geometric shape before tesselation), polygon offset
* Stencil buffer and others
* Rendering 
  * Utility to quickly setup material and mesh 
  * Highlight edges of a mesh(use polygon offset)
* Scene
  * Load mesh assets
  * Load materialId assets
  * Load scene graph
  * Unload scene and assets
  * Unbind assets from GPU
* Render Meshes
* Ray casting(for mouse picking and other features)
* Mouse picking(redirect event to the corresponding event hub of a view)
* Multithreaded rendering
* Animation
* Collision detection
* Serialization of scene state
* Networking/Mutliplayer
* Spatial partition: https://youtube.com/watch?v=eED4bSkYCB8&si=uOxianrO-zNMLAG3, https://youtu.be/MzUxOe5x24w?si=3PLIEqPTq8w8RLet, https://youtu.be/4r_EvmPKOvY?si=iINNvyGsoYzdaftY, https://youtu.be/6BIfqfC1i7U?si=rJSwRj3xuANSJZ4o
* Path finding
* Integrate C# dotnet core as scripting engine(https://learn.microsoft.com/en-us/dotnet/core/tutorials/netcore-hosting)
* Build for MacOS
* Voice control
* CI/CD setup with GitLab
  * Setup CPack
  * Setup pipeline for build and tests on windows
  * Setup pipeline for releases
  * Deploy assembled packages to some network storage

**04.01.25**
* Task done: Scene entities with model and world transformation components

**02.12.24**
* Task done: Move all glfw specific code to GlfwWindowSystemDevice

**02.12.24**
* Task done: Test binding of multiple uniform blocks
* Task done: Improve opengl error detection(add information about location in the code to the error message)
* Task done: Bind static and dynamic uniforms separately

**01.12.24**
* Task done: build a type trait to detect uniform types in a general way
* Task done: Test and fix binding of multiple uniform blocks

**30.11.24**
* Task done: Organize OpenglGpuDevice code better
* Task done: Get uniform-block names and find out if it is a named instance or not

**25.11.24**
* Task done: render geometry

**25.11.24**
* Task done: Scene graph
* Task done: Static mapping of Vertex Attributes
* Task done: Fill Buffers with mapped data

**04.11.24**
* Task done: Hot reload of plugins with file watchers(see library https://github.com/SpartanJ/efsw)
* Task done: Configure and load plugins

**02.11.24**
* Task done: Move engine to a static shared library
* Task done: Switch from dynamic library to static(cmake setup and library embedding in the source code(factory))

**29.10.24**
* Task done: Flatten controller pipeline: store entities in a sorted flat array

**17.09.24**
* Task done: Move generic components like EventHub system, Reflection/Serialization, etc to header only libraries, each per component/system

**10.09.24**
* Task done: Setup memory usage analysis with Valgrind
* Task done: Setup static analysis tool
* Task done: Setup pipeline for tests(build, validate, analyze)

**04.09.24**
* Task done: Setup test coverage checks

**03.09.24**
* Task done: Setup CTest

**28.08.24**
* Task done: Organize cmake targets for convenient development
* Task done: Cleanup install directory from google-benchmark files
* Task done: Put test executable to a separate folder in install directory

**19.07.24**
* Task done: boost problem solved. Now instead of whole boost library a small adapter library is used instead, which contains extract of used boost libraries 

**19.07.24**
* Task done: Review and rework header includes(include only headers which are required be the source file), possibly use forward declaration
* Task done: Install boost preliminary

**18.07.24**
* Working on hot reload of extensions and plugins. It is basically implemented, but needs some polishing
* Task done: Test multi-module libraries
* Task done: lean up old ModuleService code
* Task done: Restrict events and controllers API to use ILibrary handler for all subscriptions
* Task done: Deployment(install) of extensions and plugins during development
* Task done: Debug modules
  * The problem was in mismatch in the relative paths in the debug symbols of the dll. A temporary solution is to either load the module from the 
  * destination where it was built(using absolute or relative path like /home/semion/dev/doryn/build/cmake-artifacts-ninja-clion/examples/sandbox/modules/extension/)
  * or set gdb symbols search path like:
  * set solib-search-path /home/semion/dev/doryn/build/cmake-artifacts-ninja-clion/examples/sandbox/modules/extension/

**05.07.24**
* Task done: Rework project structure

**03.07.24**
* Task done: Manage external dependencies with CMake

**23.06.24**
* Task done: Multi-module libraries

**22.06.24**
* Task done: Resource handling and reference counting utility classes. The problem is mitigated in the way, that Resources will be allocated in a library
  and will be deallocated by the library itself on library destruction

**21.06.24**
* Task done: Resource handling and reference counting utility  classes

**19.06.24**
* Task done: Template based class mocking with FakeIt

**18.06.24**
* Task done: Test reload logic

**17.06.24**
* Task done: Safe lock mechanism to execute code from a library

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
* Task done: Rework project structure: mainController should be a single static library unit, game logic can be a shared library(dll)
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
* Task done(some time ago): Stop mainController using Event and LoopService

**25.09.23**
* Task done: Compile with Visual C++ compiler

**24.09.23**
* Task done: Refactor Project class, make it just a service container, develop separate class to hold application logic
* Task done: Move src/lib out of project folder(in order to make vs code intellisence work properly and light)

**23.09.23**
* Task done: Implement dynamic window open/close with views

**22.09.23**
* Task done: Generalize DataContext type for all the types in the mainController, so that it is possible to work with derived classes of DataContext in the Project class  without casting
* Task done: Make order with mainController namespaces, folders and files
* Task done: Group all the mainController components, repositories, factories, etc in Project class

**20.09.23**
* Task done: Develop Unit Tests for EntitiyRepository
* Task done: Introduce logic level Service to manage complex data like Window and View(in order to delete window and views properly)
* Task done: Add Groups of Controllers to MainController
* Task done: Assign ComponentId to every Controller, attach view to controller inderectly with view-controller entity
* Task done: Remove MainController::dataContext and pass it in the MainController::update as parameter instead
* Task done: Introduce Engene EventHub and invoke "after load event"
* Task done: Implement dynamic Controller creating and deletion on MainController

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
* I should swap roles of MainController and LoopService entities. LoopService should have a reference to MainController and should call update function of MainController. MainController should not know anything about LoopService, just recieve timeStep und manage update of game state via controllers.