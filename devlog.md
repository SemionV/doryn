# Development log

**19.06.23**
* I should swap roles of Engine and FrameService entities. FrameService should have a reference to Engine and should call update function of Engine. Engine should not know anything about FrameService, just recieve timeStep und manage update of game state via controllers.