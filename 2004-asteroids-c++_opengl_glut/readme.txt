Assignment 2: Asteroids: Lighting and Motion
CSC461-001 Fall 2004 - Dr. Healey
Brian Pike
bapike@unity.ncsu.edu

Description:
This program draws 3 shaded and lighted asteroids, along with a
shaded and lighted spaceship on the screen.  The spaceship can be
controlled with the left and right arrow keys and can use 'x' to
move forward.  The spaceship can also fire its weapon using the 'z'
key.  After a period of time, the bullets disappear.

The motion of the spaceship is fairly realistic, although turning
is exact (i.e., no rotational momentum), and space here has friction.
The spaceship is limited to a maximum speed.  The spaceship also
has a very ugly paint job.  The spaceship cannot shoot constantly;
there is some delay if you just hold down the button.  Thus the
number of shots is limited by the lifetime of the shots and the rate
at which they can be produced.

To exit, use the popup window that comes up if you click the right
mouse button.

Special Notes:
There's some code that spits debug messages to the console, mostly
from generic keyboard and mouse-handling functions.  I hope this
isn't a problem.

Compiling:
By following the directions from:
http://www.csc.ncsu.edu/faculty/healey/csc461/assn_1/opengl.pdf
to create a project and then importing *.cpp, *.h, one can compile
to an executable.  I didn't include all of the files in the
project because I don't know which ones are important or not.  Besides,
if the reason you want the source code is to use MOSS, then it would
be better to include only the C++ code.
