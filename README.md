# Getting Started

## Build
```
mkdir build
cd build
cmake ..
make -j8
```

## Execution
```
cd build
./bin/skinning
```

## Project Features
<a id="characterswapping"></a>
### Character Swapping
<a href="#characterswapping"><img src="/assets/gifs/character_swap.gif"/></a>
```
We construct different render passes for each Character, and store all relevant information in a Character Class, and the input handler can switch between different character models without losing the stored character information.
```
<a id="gravity"></a>
### Gravity Attempt
<a href="#gravity"><img src="/assets/gifs/gravity-1.gif"/></a>
```
We have the character jump directly upwards on the y-axis, using simple physics simulation.
Velocity = Initial Velocity + (Acceleration * Time)
If we’re in the air, we check every frame to make sure if we are equal to height of the land or below it, and if so, set the height to the floor of the map.
```

<a id="water"></a>
### Water Animation
<a href="#water"><img src="/assets/gifs/water-1.gif"/></a>
```
We have a set water level and render water at that height.
We use 3D Perlin Noise to simulate the shifting tides of the water.
```

<a id="perlinlevels"></a>
### Perlin Levels and Moisture Map
<a href="#perlinlevels"><img src="/assets/gifs/water2snow-1.gif"/></a>
```
We use Perlin Noise to generate 2 maps: Elevation and Moisture.
We use the elevation values to determine what height the map will be, and use the elevation and moisture values in order to determine different biomes, represented by a blended color.
```

<a id="freecam"></a>
### Free-Cam Mode
<a href="#freecam"><img src="/assets/gifs/fps_mode-2.gif"/></a>
```
You can change to free cam mode in order to fly and explore the map. This way, you can see the entire map in all of its radius-bounded glory and creation.
```

# Acknowledgement 

Made by Drew Dearing, Jesus Martinez, and Eric Lee.

This bone animation code is based on the skinning project written by
Randall Smith for 2015 Spring Graphics Course.

The PMD parser library is written by
[itsuhane](https://github.com/itsuhane/libmmd), licensed under Boost Software
License.

The author of PMD models is Animasa, you can download the model with the
official editor from his webpage [VPVP](http://www.geocities.jp/higuchuu4/index_e.htm).
