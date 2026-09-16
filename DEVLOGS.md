# September 11-12
Start of the project, very excited. Created the foundation of the raylib simulation (just a grid and padding and camera movement.) Afterwards, created the first basic unit which basically just runs towards the goals that it wants. They are conscious about strategic deceleration and changing directions. Within a certain range, the bots also intentionally avoid one another 

# September 13
Worked on the cuddling mechanism, and also changed the way the incentives are coded for future scalability and shared incentives. The system for incentives is more flexible is all im trying to say. The the toggling X for cuddling to exploration mechanism works well.

# September 14
Planned out and brainstormed ideas for how incentives and the environment will be structured. Figured the easiest thing to fix now would be the incentives system. Ended up being a very big day. Firstly, created incentives and their visual display (size meaning difficulty, number meaning interest), and marked as green when discovered. However, no influence of behavior just yet, that will come hopefully tomorrow or at least soon with the scouting->investigation cycle implemented. Incentives are placed randomly throughout the map.

Secondly, made the initial exploration burst behavior where each unit manages a certain slice (360/n degrees) of a certain radius in order to spread out work evenly and it randomly wanders around exploring. 

Lastly, implemented the vision tiles. Right now its just mainly for show and to demonstrate where the bots have went, but it will be useful in the future for if it gets turned into a game and theres unknown info.

Also in general just numerous small improvements to all sorts of systems very productive day.

# September 15
Today was definitely more of a conceptual work day, reason being after doing some more research regarding what Mars rovers actually do, I've realized that there are a LOT of issues with my representation of tasks being done. Primarily, that tasks done by a swarm of drones won't really be parallizable so whats needed is specialization in roles.

Because of this, I needed to change a lot of things, starting off with specialized role assignments (indicated by different unit colors) and different incentive requirements (indicated by the text box above when hovered). Each incentive would only need those # units to come together and work. Would probably also make planning easier.

Speaking of planning, also started basic work on a collective brain file that would determine priorities and assign them to the bots but no progress there yet.