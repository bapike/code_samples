/  Copyright 2004, 2016 Brian Pike
|
|  You may redistribute this work under the terms of the GNU General Public
|  License as published by the Free Software Foundation, either version 2 of
\  the License, or any later version.

This is my final version of a project from a graphics course
at North Carolina State University (CSC461-001, Fall 2004, Prof. Healey).

Description:
This is a rough clone of the game `Asteroids', written in C++ with
OpenGL and GLUT.

It draws a spaceship that can turn, move forward, and
fire a particle weapon, along with asteroids that move around
the screen, and (eventually) an enemy spaceship.
Everything has shading and lighting.  The playing surface is
a flat torus, as objects wrap around between the top/bottom
and left/right.  When shot, large asteroids break into smaller
asteroids.

It was written on Linux, then ported to Windows before turning it in.

The motion of the spaceship is somewhat realistic, although turning
is exact (i.e., no rotational momentum), and (for playability)
the spaceship experiences friction from space.
The spaceship is limited to a maximum speed.  The spaceship also
has a very ugly paint job.  The spaceship cannot shoot constantly;
there is some delay if you just hold down the button.  Thus the
number of shots is limited by the lifetime of the shots and the rate
at which they can be produced.  The shots disappear after a certain
amount of time.

Note to any students:
If this assignment is still assigned, then the professor is undoubtedly
using some form of cheating detection.  Use this code as inspiration,
but don't copy. 

Controls:
 'x' moves forward
 'z' fires
 'q' quits
 left arrow key rotates left
 right arrow key rotates right
 right clicking brings up a menu to restart or quit

Known Bugs:
 - the spaceship can try to respawn too close to an asteroid
 - the physics is not exact (I think nonphysical behavior was part of the assignment :-/)
 - something should happen when asteroids collide
 - when a shot hits an asteroid, the broken pieces of the asteroid should
   have velocities that are 'fair' (e.g., use elastic collisions)
 - it should be rewritten to use more modern C++ 
 - the performance was adequate, but never very good for something so simple

Compiling on Windows:
Install the dependencies.  Follow the directions from:
http://www.csc.ncsu.edu/faculty/healey/csc461/assn_1/opengl.pdf
to create a project and then import the *.cpp, *.h files.

Compiling on Linux:
Install the dependencies and use 'make'.

