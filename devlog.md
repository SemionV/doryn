# Development log

**14.09.23**
Current tasks:
* Instal gl3w library with build/install-dependencies.sh
* Develop WindowController instead of WindowSystem
* Use new View structure in ViewController for OpenGL
* Clean up the old WindowSystem code(remove)
* Add Groups of Controllers to Engine
* Move src/lib and scr/vendor dirs out of project folder(in order to make vs code intellisence work properly and light)

**19.06.23**
* I should swap roles of Engine and FrameService entities. FrameService should have a reference to Engine and should call update function of Engine. Engine should not know anything about FrameService, just recieve timeStep und manage update of game state via controllers.