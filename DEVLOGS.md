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

# September 16-17th
Combined into one session because of how massive the change is. The primary difference that I'm tryign to build up towards basically is that instead of randomly going to points, the bots will fall into a predictable cycle of exploring, finding incentives, and then coming back, making a priority list, assigning tasks, and then going back out to visit them. 

I've been working on the incentive list for a long time and its hard as hell!! I made some very difficult to change architectural decisions and hard coded a lot of the movemeent decision making to be totally random at first because I just wanted to see something happen like 5 days ago. But thats coming back and biting me in the ass once i actually need the bots go to somewhere worthwhile.

I was having a lot of issues with desyncing between the units and the group as a whole, so to end off the two days I made a debug state tracker that shows the priority scores of each incentive and also the state/type of each unit. Its this decoupling thats really messing with me so hopefully this quick tool is gonna help fix that over the weekend.

# September 18th
Came to the realization that one of the biggest things that was biting my ass while working on this was the fact that I didn't actually need a unitState for the bot, since they're all shared across each bot. So there is no need for an individual state, and it was one of the biggest things that was holding me down. I struggled with this a LOT and even with the help of Claude to help me debug, so I eventually just tore everything out and created a new centralized groupstate system that takes in commands directly from main and assigns to all members. This helps remove the decoupling risk since how centralized it is. 

Also another thing was updating the debug screen to be more simple and also include scrolling functionality. 