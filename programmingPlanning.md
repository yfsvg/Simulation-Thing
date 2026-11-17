Things to do in the near future:
1. Lay down the groundwork for easier game dev
    - Use a game struct that can be accessed instead of using global variables.
    - Draw only what is visible instead of using drawBG to draw 160,000 fog rectangles every time.
    - Adding scroll wheel zoom

2. Selection and move orders
    - Dragging over an area to select multiple bots
    - Select mode like in Geometry dash where you can just click on them as well and you select more. They're highlighted to let you know.
    - For now, just picking a point and they will travel there and then go back to their idle decision.
    - I liked my old idea of using an enum state, idle moving working and dead should be the new one.
    - Keep existing steering because we have no environment yet

3. Map and resources
    - Using the perlin noise, make a realistic tilemap with martian coloring
    - I think one reason why I wasn't doing shit was because the resources were just blocks and not interesting. If I start working on art along with it I'll be less likely to be burnt out because that is fun.
    - Regolith can be mined literally anywhere, and raw iron is scattered for now.

4. Mining and hauling
    - Connecting #2 and #3 so you can direct bots to do certain things.