# Low Orbit Post Mortem
I'll start by going over a few aspects of this particular compo (my 19th), and then cover what went well and what went wrong.

## The Theme
I'll be honest: I voted against "A Small World", for the simple reason that that it is way too similar to the theme of LD #23, "Tiny Planet". I briefly considered making a sequel to my LD #23 [entry](http://quadtree.info/#/ld/), which I would have called "Space Imperator II". This just seemed to creatively bankrupt however, so I decided to find something else. I eventually settled on the idea of a racing game where the player raced around on a small planet, probably from pole to pole. Racing games are one genre I have never done in the LD, so it seemed nice to try something new.

## Feasability
Unreal Engine 4 uses PhysX which I guessed isn't designed to allow vehicles to drive on the bottom of things. My initial tests confirmed this, but eventually I was able to find an odd hack: If you leave gravity on, but add an extra force to all objects which negates it, vehicles work. However, I still suspect that the cars have better handling on the "top" of the planet rather than the "bottom". Either way, it isn't all that noticable (hopefully).

# What Went Well
## The Idea
I did have time to implement my original idea, and even throw in an extra level. The cars turned out how I expected, and I think each has its own distinct flavor.

## UE4 Graphics
I'm not a very good 3D artist, but UE4 has the ability to make my mediocre models look decent, at least. There is a problem with lighting, as the engine isn't designed to have objects lit and shadowed on all sides.

## The Minimap
I set up the top-view of the planet at the very start, as a simple render-to-texture. Considering how long it took to make, I think it really improved the look of the game and emphasized the planetary aspect of the game.

## Sound and Music
This is usually a weak spot for me, but I'm fairly happy with how it turned out. I recorded my electric range hood and used that as the engine noise, which worked well with the simulated RPM in the vehicle system. For the collisions, I used a slowed down version of me hitting butter knives together. The rocket engine sound is made in Bfxr.

As for the music, I made about 10 tracks in Abundant Music with a tempo of 300-350 and a synth as the lead. After I'd made about 10, I got one I really liked, although it's only about 88 seconds long.

# What Didn't Go Well
## UE4 Vehicles
The UE4 vehicle system is very complex. This is good in that it models stuff like engine RPM and transmissions, but it's bad in that there are a ton of things that can go wrong. I spent about 5 hours trying to export vehicle models as skeletal meshes from Blender, before giving up and using the same technique I used in LD #32: I just used the default car skeleton and attached wheel and body static meshes to it.

## The Name
The idea crystallized right off the bat, but the name didn't. Here are the names I rejected, from my notes: World Wide [Racer?], Equatorial Race, World Wide Circuit, Pole to Pole, Race the World, Race to the Poles, Circle the Globe, Globetrack, Ground Orbit, Planetary Challenge, Planetary Circuit, Polar Circuit, Polar Track, Planetary Speedway, Celestial Speedway, Solar Speedway, Speedway Planet, Latitude Circuit, Great Circuit, Extremely Low Orbit, Polar Speedway

## Performance
Final testing on my laptop revealed that the game runs poorly on low-spec machines even though it doesn't look all that good. I think the problem is the subdivision surfaces I used on the cars, which add a ton of largely redundant polygons.

## HTML5 Export
Yet again the HTML5 export failed to work flawlessly. It looks like the problem is that UE4.15 won't compile the PhysXVehicle plugin into a HTML5 build, and I'm not completely sure why. UE4.16 doesn't seem to have this problem, but I haven't had time to try a source build yet. The strangest part here is that vehicle physics used to work in HTML5 builds, as I used them in LD #32 which had an HTML5 version.