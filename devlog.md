# Development log

Tasks:
* Implement SystemConsole and Win32Window systems as Controllers
* Assign ComponentId to every Controller, attach view to controller inderectly with view-controller entity
* Call initialize method of Controller in Engine::initialize
* Introduce logic level Service to manage complex data like Winow and View(in order to delete window and views properly)
* Implement dynamic window open/close with views
* Add Groups of Controllers to Engine
* Move src/lib and scr/vendor dirs out of project folder(in order to make vs code intellisence work properly and light)
* Integrate some scripting engine

**15.09.23**
* Task done: Develop WindowController instead of WindowSystem
* Task done: Delete Window and View entities attached to GlfwWindow when iut is closed
* Task done: Use new View structure in ViewController for OpenGL
* Task done: Clean up the old WindowSystem code(remove)

**14.09.23**
* Task done: Instal gl3w library with build/install-dependencies.sh

**19.06.23**
* I should swap roles of Engine and FrameService entities. FrameService should have a reference to Engine and should call update function of Engine. Engine should not know anything about FrameService, just recieve timeStep und manage update of game state via controllers.