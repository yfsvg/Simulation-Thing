# September 11-12
Start of the project, very excited. Created the foundation of the raylib simulation (just a grid and padding and camera movement.) Afterwards, created the first basic unit which basically just runs towards the goals that it wants. They are conscious about strategic deceleration and changing directions. Within a certain range, the bots also intentionally avoid one another 

# September 13
Worked on the cuddling mechanism, and also changed the way the incentives are coded for future scalability and shared incentives. The system for incentives is more flexible is all im trying to say. The the toggling X for cuddling to exploration mechanism works well.

# September 14
Planned out and brainstormed ideas for how incentives and the environment will be structured. Figured the easiest thing to fix now would be the incentives system. Ended up being a very big day. Firstly, created incentives and their visual display (size meaning difficulty, number meaning interest), and marked as green when discovered. However, no influence of behavior just yet, that will come hopefully tomorrow or at least soon with the scouting->investigation cycle implemented. Incentives are placed randomly throughout the map.

Secondly, made the initial exploration burst behavior where each unit manages a certain slice (360/n degrees) of a certain radius in order to spread out work evenly and it randomly wanders around exploring. 

Lastly, implemented the vision tiles. Right now its just mainly for show and to demonstrate where the bots have went, but it will be useful in the future for if it gets turned into a game and theres unknown info.

Also in general just numerous small improvements to all sorts of systems very productive day.