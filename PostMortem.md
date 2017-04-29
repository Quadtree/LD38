# [Low Orbit](https://ldjam.com/events/ludum-dare/38/low-orbit) Post Mortem
I'll start by going over a few aspects of this particular compo (my 19th), and then cover what went well and what went wrong.

## The Theme
I'll be honest: I voted against "A Small World", for the simple reason that that it is way too similar to the theme of LD #23, "Tiny Planet". I briefly considered making a sequel to my LD #23 [entry](http://quadtree.info/#/ld/), which I would have called "Space Imperator II". This just seemed too creatively bankrupt however, so I decided to find something else. I eventually settled on the idea of a racing game where the player raced around on a small planet, probably from pole to pole. Racing games are one genre I have never done in the LD, so it seemed nice to try something new.

## Feasability
Unreal Engine 4 uses PhysX which I guessed isn't designed to allow vehicles to drive on the bottom of things. My initial tests confirmed this: disabling gravity and using a radial attractor to simulate gravity prevents vehicles from getting any traction. Eventually I was able to find an odd hack: If you leave gravity on, but add an extra force to all objects which negates it, vehicles work. However, I still suspect that the cars have better handling on the "top" of the planet rather than the "bottom". Either way, it isn't all that noticable (hopefully).

# What Went Well
## The Idea
I did have time to implement my original idea, and even throw in an extra level. The cars turned out how I expected, and I think each has its own distinct flavor.

## UE4 Graphics
I'm not a very good 3D artist, but UE4 has the ability to make my mediocre models look decent, at least. There is a problem with lighting, as the engine isn't designed to have objects lit and shadowed on all sides.

## The Minimap
I set up the top-view of the planet at the very start, as a simple render-to-texture. Considering how long it took to make, I think it really improved the look of the game and emphasized the planetary aspect of the game.

## Sound and Music
This is usually a weak spot for me, but I'm fairly happy with how it turned out. I recorded my electric range hood and used that as the engine noise, which worked well with the simulated RPM in the vehicle system. For the collisions, I used a slowed down version of me hitting butter knives together. The rocket engine sound is made in Bfxr.

As for the music, I made about 10 tracks in [Abundant Music](http://www.abundant-music.com/) with a tempo of 300-350 and a synth as the lead. After I'd made about 10, I got one I really liked, although it's only about 88 seconds long.

## Opponent AI
My past experiences with writing vehicle AI were not very encouraging. The only vehicle AI I've ever written is the fairly stupid AI that drives the other vehicles in my LD #32 entry. Even then, it only had to worry about what is essentially a 2D plane with no obstacles. I got fairly lucky here in that my first idea worked out pretty well. The AI chooses the next destination, either as the next checkpoint, or from a numbered set of waypoints that proceeds each checkpoint. It then projects everything onto a plane perpendicular to a line drawn from the center of the planet. It then takes the dot product of the vector to the destination and another vector that faces off to the left, and uses this to determine steering. As the AI gets more on course, the dot product decreases and the AI increases its maximum speed. It also decreases maximum speed as it approaches the destination.

The result, while not particularly smart, can actually beat me on occasion, which I think is good enough.

# What Didn't Go Well
## UE4 Vehicles
The UE4 vehicle system is very complex. This is good in that it models stuff like engine RPM and transmissions, but it's bad in that there are a ton of things that can go wrong. I spent about 5 hours trying to export vehicle models as skeletal meshes from Blender, before giving up and using the same technique I used in LD #32: I just used the default car skeleton and attached wheel and body static meshes to it.

## The Name
The idea crystallized right off the bat, but the name didn't. Here are the names I rejected, from my notes: World Wide [Racer?], Equatorial Race, World Wide Circuit, Pole to Pole, Race the World, Race to the Poles, Circle the Globe, Globetrack, Ground Orbit, Planetary Challenge, Planetary Circuit, Polar Circuit, Polar Track, Planetary Speedway, Celestial Speedway, Solar Speedway, Speedway Planet, Latitude Circuit, Great Circuit, Extremely Low Orbit, Polar Speedway

## Performance
Final testing on my laptop revealed that the game runs poorly on low-spec machines even though it doesn't look all that good. I think the problem is the subdivision surfaces I used on the cars, which add a ton of largely redundant polygons.

## HTML5 Export
Yet again the HTML5 export failed to work flawlessly. It looks like the problem is that UE4.15 won't compile the PhysXVehicle plugin into a HTML5 build, and I'm not completely sure why. UE4.16 doesn't seem to have this problem, so my only option was to build 4.16 from source. There were a couple other issues around lighting and weird model distortion, but I do have a [HTML5](https://quadtree.info/ld/ld38/HTML5/LD38-HTML5-Shipping.html) build up now, with only one bug that doesn't exist in the Win32 version: for some reason, the car's wheels are rotationally 1 frame behind the body. This is only noticable when the car is spinning at high speed, so hopefully it'll only come up rarely.

All in all, it was an enjoyable experience. See you all in LD #39!
