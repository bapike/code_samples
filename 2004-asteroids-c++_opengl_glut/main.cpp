/*
 * Author: Brian Pike
 * Class: CSC461-001 (Graphics), NC State University, Fall 2004, Dr. Healey
 * 
 * This program draws 3 wireframe objects on the screen.  On the left is the enemy spaceship; in the middle is the
 * player's spaceship; and on the right is a randomly generated asteroid.  These 3 objects rotate about the x-axis at a
 * rate of 90 degrees/second, to allow one to view the objects in their entirity.  In addition, there's a popup window that
 * comes up if you click the right mouse button, allowing you to quit.
 *
 * Implemented out of the requirements is generic keyboard and mouse handling code and some other debug messages which may
 * pollute the console.
 *
 * All game objects (asteroids, player ship, enemy ships, shots) will be
 * confined to [-1,1]x[-1,1]x{0} (i.e., a square in the x-y plane).  The camera
 * will be set at (0,0,z) so as to see this area.
 */

#include<iostream>
#include<stdlib.h>
#include<GL/glut.h>
#include<time.h>
#include<math.h>
/* For snprintf on windows (2 lines): */
#include<stdio.h>
#include<string.h>
//#define snprintf _snprintf

#include"gameobj.h"
#include"collision.h"
#include "flame.h"

using namespace std;

/*
 * TODO List:
 *  + Make alien ship appear and move around
 *  + Make alien ships shoot at stuff
 *  + Collision detection
 *  + Make alien ship appear at proper time
 *  + implement scoring
 *  + Track # of ships left
 *  + "New Game" on menu
 *  + heads up display
 *  + When starting a new level, leave ship where it is
 *  + care about # of ships left
 *  + flash messages on screen
 *  + Make easier for enemy ship to appear
 *  + Make level end only when no enemy ship and no asteroids
 *  - Make enemy ship fire more often on higher levels
 */

/* Call the timer function every this number of milliseconds */
const unsigned int CALL_TIMER_FUNC_EVERY_N_MILLISEC = 5;
const float CALL_TIMER_FUNC_EVERY_N_SEC = ((float)(CALL_TIMER_FUNC_EVERY_N_MILLISEC)) / 1000.0;
const int NUM_SHOTS=40;
const int SHOT_LIFETIME_MILLISEC=1000;
const int TIME_BETWEEN_SHOTS=20;
const int MAX_INIT_NUM_ASTEROIDS = 6;
const int NUM_ASTEROIDS=4*MAX_INIT_NUM_ASTEROIDS+1;
const int NUM_EXPLOSIONS = 20;
const int ENEMY_SHOOTS_EVERY_N_MS = 500;
const int MESSAGE_FLASH_LENGTH_MS = 2000;
const float SIZE_ASTEROID_LARGE = 1.0;
const float SIZE_ASTEROID_MED = 0.75;
const float SIZE_ASTEROID_SMALL = 0.5;
const int INIT_NUM_PLAYERSHIPS_LEFT = 5;
const int EXTRA_PLAYERSHIP_EVERY_N_POINTS = 10000;

int current_level;
int num_player_ships_left;
int current_score;
Asteroid *asteroids[NUM_ASTEROIDS] = {NULL};
int num_active_asteroids = 0;
Shot *shots[NUM_SHOTS] = {NULL};
Explosion *explosions[NUM_EXPLOSIONS] = {NULL};
PlayerShip *player_ship = NULL;
EnemyShip *enemy_ship = NULL;
bool has_seen_enemy_ship;
char current_message[30] = "";

void renderFrame(void);
void timer_callback(int value);
void clean_shot(int index);
void handle_keyboard(unsigned char key, int x, int y);
void handle_keyboard_up(unsigned char key, int x, int y);
void handle_keyboard_special(int key, int x, int y);
void handle_keyboard_special_up(int key, int x, int y);
void handle_mouse(int button, int state, int x, int y);
void window_reshape_callback(int width, int height);
void menu_setup(void);
void menu_callback(int value);
int shutdown(void);
void randomize_asteroid(int i);
void damage_asteroid(int i);
void damage_playership();
void damage_enemyship();
void increment_score(int points);
void clean_explosion(int index);
void init_next_level();
void start_game();
void end_game();
void render_status_info();
void enemy_ship_fire_and_move(int num_ms);
void test_collision_playership_asteroid(int i);
void test_collision_asteroid_shot(int i, int j);
void test_collision_enemyship_asteroid(int i);
void test_collision_playership_shot(int i);
void test_collision_enemyship_shot(int i);
void test_collision_playership_enemyship();
void make_explosion_at(GameObject *);
void start_enemy_ship_moving(bool is_small);
void zero_string(int blah);

int main(int argc, char **argv)
{
	// Let GLUT take parameters.
	glutInit(&argc,argv);

	// We want RGB colors and a double buffer
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(900,600);
	glutCreateWindow("Asteroids");

	// establish window callbacks, menus?
	glutKeyboardFunc(handle_keyboard);
	glutKeyboardUpFunc(handle_keyboard_up);
	glutSpecialFunc(handle_keyboard_special);
	glutSpecialUpFunc(handle_keyboard_special_up);
	glutMouseFunc(handle_mouse);

	// tells how to display the window 
	glutDisplayFunc(renderFrame);

	// Setup the timer function so that physics functions regardless of system load, approximately.
	glutTimerFunc(CALL_TIMER_FUNC_EVERY_N_MILLISEC, &timer_callback, 0);

	// called when window is reshaped -- use the default.
	glutReshapeFunc(window_reshape_callback);
	
	// sets the idle function, i.e. render 1 frame
	glutIdleFunc(&renderFrame);

	// set the background color
	glClearColor(0.0,0.0,0.0,0.0);
	glClearDepth(1.0);

	// Setup the projection
	window_reshape_callback(900,600);

	// Setup the camera.
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0, 0, 1, 0, 0, 0, 0, 1, 0 );

	// Setup the menu
	menu_setup();

	// Initialize random number generator
	srand(time(0));
	// Setup the level, with asteroids and a player ship
	start_game();

	// Setup OpenGL: lighting, type of shading, Z-buffer
	float amb[4] ={.2,.2,.2,1};
	float diff[4]={.8,.8,.8,1};
	float poss[4]={0,0,1,0};
	float spec[4]={0,0,0,1};
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, poss);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// Enable blending for just where we need it
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ZERO);
						 
	// This is the kernel of the program, calling any callback routines necessary
	glutMainLoop();
		  
	return 0;
}

void renderFrame(void)
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// draw stuff
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
		if (player_ship != NULL)
		{
			player_ship->draw();
		}
		if (enemy_ship != NULL)
		{
			enemy_ship->draw();
		}
		for (int i=0;i<NUM_ASTEROIDS;i++)
		{
			if (asteroids[i] != NULL)
			{
				asteroids[i]->draw();
			}
		}
		for (int i=0;i<NUM_SHOTS;i++)
		{
			if (shots[i] != NULL)
			{
				shots[i]->draw();
			}
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		for (int i=0;i<NUM_EXPLOSIONS; i++)
		{
			if (explosions[i] != NULL)
			{
				explosions[i]->draw();
			}
		}
		render_status_info();
		glBlendFunc(GL_ONE,GL_ZERO);
		glEnable(GL_DEPTH_TEST);
	glPopMatrix();

	// flush buffers and show the backbuffer
	glFlush();
	glutSwapBuffers();
}

void draw_string(char *str)
{
	while ((*str)!='\0')
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15,(*str));
		str++;
	}
}
		
void render_status_info()
{
	float mat[4] = { 1,1,1,0.9};
	char string[100];
	float sep = 0.3;
	float x = bound_left+sep, y = bound_top-(1.5*sep);

	glMatrixMode(GL_MODELVIEW);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mat);
	glNormal3f(0,0,1);
	
	glRasterPos2f(x,y);
	snprintf(string,100,"Level: %d",current_level);
	draw_string(string);
	
	glRasterPos2f(x,y-sep);
	snprintf(string,100,"Score: %d",current_score);
	draw_string(string);

	glRasterPos2f(x,y-(2*sep));
	snprintf(string,100,"Ships Remaining: %d",num_player_ships_left);
	draw_string(string);

	glRasterPos2f((((float)strlen(current_message))/2.0)*sep*-0.5,0);
	draw_string(current_message);
}

void timer_callback(int value)
{
	static int updates_since_fire=0;

	// If we don't have a player_ship, then we're just pausing.  Just return
	if (player_ship == NULL)
	{
		glutTimerFunc(CALL_TIMER_FUNC_EVERY_N_MILLISEC, &timer_callback, 0);
		return;
	}
	// Update the player; handle new shots.
	player_ship->update(CALL_TIMER_FUNC_EVERY_N_SEC);
	if (player_ship->keydown_fire)
	{
		if (updates_since_fire==0)
		{
			// OK, fire:
			// Find a free element of shots[]
			int i=0;
			while (i<NUM_SHOTS && shots[i]!=NULL)
			{
				i++;
			}
			if (i==NUM_SHOTS)
			{
				cerr<<"Alert!  NUM_SHOTS needs to be increased based on your shot rate\n";
			}
			else
			{
				shots[i] = new Shot(player_ship, false);
				glutTimerFunc(SHOT_LIFETIME_MILLISEC, &clean_shot, i);
			}
		}
		updates_since_fire++;
		updates_since_fire = updates_since_fire % TIME_BETWEEN_SHOTS;  
	}
	else
	{
		updates_since_fire=0;
	}

	if (enemy_ship != NULL && enemy_ship->active)
	{
		enemy_ship->update(CALL_TIMER_FUNC_EVERY_N_SEC);
	}

	// Update old shots
	for (int i=0;i<NUM_SHOTS;i++)
	{
		if (shots[i] != NULL)
		{
			shots[i]->update(CALL_TIMER_FUNC_EVERY_N_SEC);
		}
	}

	// Update the asteroids
	for (int i=0;i<NUM_ASTEROIDS;i++)
	{
		if (asteroids[i] != NULL)
		{
			asteroids[i]->update(CALL_TIMER_FUNC_EVERY_N_SEC);
		}
	}

	// Update the explosions
	for (int i=0;i<NUM_EXPLOSIONS;i++)
	{
		if (explosions[i] != NULL)
		{
			explosions[i]->update(CALL_TIMER_FUNC_EVERY_N_SEC);
		}
	}

	// Collision detection
	for (int i=0;i<NUM_ASTEROIDS;i++)
	{
		if (asteroids[i] != NULL)
		{
			/* shot-asteroid */
			for (int j=0; j<NUM_SHOTS; j++)
			{
				/* We check for NULL's in this function */
				test_collision_asteroid_shot(i,j);
			}

			/* ships-asteroid */
			/* We check for NULL's in this function */
			test_collision_playership_asteroid(i);
			test_collision_enemyship_asteroid(i);
		}
	}
	for (int i=0;i<NUM_SHOTS; i++)
	{
		if (shots[i] != NULL)
		{
			if (shots[i]->evil)
			{
				/* player ship-shot intersect? */
				test_collision_playership_shot(i);
			}
			else
			{
				/* enemy ship-shot intersect? */
				test_collision_enemyship_shot(i);
			}
		}
	}
	test_collision_playership_enemyship();

	// call again in CALL_TIMER_FUNC_EVERY_N_MILLISEC milliseconds
	glutTimerFunc(CALL_TIMER_FUNC_EVERY_N_MILLISEC, &timer_callback, 0);
}

void init_next_level()
{
	int i;

	/* First, clean up */
	for (i=0;i<NUM_ASTEROIDS; i++)
	{
		if (asteroids[i] != NULL)
		{
			delete asteroids[i];
			asteroids[i] = NULL;
		}
	}
	/* We can't just delete the shots (we'll have dangling callbacks), so let's make them invisible instead so that we don't delete any shots prematurely. */
	for (i=0; i<NUM_SHOTS; i++)
	{
		if (shots[i] != NULL)
		{
			shots[i]->active = false;
		}
	}
	/* Same with the enemy ship */
	if (enemy_ship != NULL)
	{
		enemy_ship->active = false;
	}

	/* Now initialize the new level */
	current_level++;
	cerr<<"Advancing to level "<<current_level<<endl;
	num_active_asteroids = current_level;
	if (current_level > MAX_INIT_NUM_ASTEROIDS)
		num_active_asteroids = MAX_INIT_NUM_ASTEROIDS;

	for (i=0;i<num_active_asteroids;i++)
	{
		asteroids[i]=new Asteroid(1.0);
		randomize_asteroid(i);
	}

	if (current_level == 1)
	{
		if (player_ship != NULL)
		{
			delete player_ship;
			player_ship = NULL;
			cerr<<"EEk"<<endl;
		}
		player_ship = new PlayerShip();
	}
	else
	{
		if (player_ship == NULL)
		{
			player_ship = new PlayerShip();
			cerr<<"EEk2"<<endl;
		}
	}
	has_seen_enemy_ship = false;
	snprintf(current_message,10,"Level %d",current_level);
	glutTimerFunc(MESSAGE_FLASH_LENGTH_MS,&zero_string,0);
}

void randomize_asteroid(int i)
{
	float direction = (rand() % 2) ? -1.0 : 1.0;
	switch (rand() % 4)
	{
		case 0: /* Put the asteroid on the left border; need x>0 */
			asteroids[i]->velocity[1]*=direction;
			asteroids[i]->position[0]=bound_left;
 			asteroids[i]->position[1]=randf(bound_bottom,bound_top);
			break;
		case 1: /* Put the asteroid on the right border; need x<0 */
			asteroids[i]->velocity[1]*=direction;
			asteroids[i]->velocity[0]*=-1;
			asteroids[i]->position[0]=bound_right;
			asteroids[i]->position[1]=randf(bound_bottom,bound_top);
			break;
		case 2: /* Put the asteroid on the top border; need y<0 */
			asteroids[i]->velocity[0]*=direction;
			asteroids[i]->velocity[1]*=-1;
			asteroids[i]->position[1]=bound_top;
			asteroids[i]->position[0]=randf(bound_left,bound_right);
			break;
		case 3: /* Put the asteroid on the bottom border; need y>0 */
			asteroids[i]->velocity[0]*=direction;
			asteroids[i]->position[1]=bound_bottom;
			asteroids[i]->position[0]=randf(bound_left,bound_right);
			break;
	}
}

void damage_asteroid(int i)
{
	int numtomake;
	float newsize;

	make_explosion_at(asteroids[i]);
	// Make new asteroids
	numtomake=2;
	newsize = SIZE_ASTEROID_LARGE;
	if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_LARGE)
		newsize = SIZE_ASTEROID_MED;
	else if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_MED)
		newsize = SIZE_ASTEROID_SMALL;
	else if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_SMALL)
		numtomake = 0;

	for (int k=0;k<NUM_ASTEROIDS && numtomake>0;k++)
	{
		if (asteroids[k]==NULL)
		{
			asteroids[k] = new Asteroid(newsize);
			asteroids[k]->position[0] = asteroids[i]->position[0];
			asteroids[k]->position[1] = asteroids[i]->position[1];
			asteroids[k]->position[2] = asteroids[i]->position[2];
			numtomake--;
			num_active_asteroids++;
		}
	}
	if (numtomake > 0)
		cerr<<"Need to increase NUM_ASTEROIDS!\n";
	
	// Delete this asteroid.
	delete asteroids[i];
	asteroids[i]=NULL;
	num_active_asteroids--;
	if (num_active_asteroids == 0)
	{
		// move to next level
		if (enemy_ship == NULL || enemy_ship->active==false)
		{
			init_next_level();
		}
	}
	else
	{
		// There's at least one left.  Let's think about putting the alien ship out there
		int asteroid_points = 0;
		for (int i=0;i<NUM_ASTEROIDS;i++)
		{
			if (asteroids[i] != NULL)
			{
				if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_LARGE)
					asteroid_points += 3;
				else if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_MED)
					asteroid_points += 2;
				else if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_SMALL)
					asteroid_points += 1;
			}
		}
		if (enemy_ship == NULL &&
			 asteroid_points < current_level &&
			 (rand() % 4) == 1)
		{
			if (has_seen_enemy_ship && ((rand() % 2)==0))
			{
				// Let's give them a small ship
				start_enemy_ship_moving(true);
			}
			else
			{
				// Let's give them a large ship
				start_enemy_ship_moving(false);
				has_seen_enemy_ship = true;
			}
		}
	}
}

void damage_playership()
{
	make_explosion_at(player_ship);
	delete player_ship;
	player_ship = NULL;
	if (num_player_ships_left == 0)
	{
		// what hapens when they run out of lives?
		// a hack:
		snprintf(current_message,10,"Game Over");
		glutTimerFunc(MESSAGE_FLASH_LENGTH_MS,&zero_string,0);
	}
	else
	{
		num_player_ships_left--;
		player_ship = new PlayerShip();
	}
}

void damage_enemyship()
{
	make_explosion_at(enemy_ship);
	enemy_ship->active = false;
	if (num_active_asteroids == 0)
	{
		// move to next level
		init_next_level();
	}
}

void make_explosion_at(GameObject *object)
{
	int i=0;
	while (i<NUM_EXPLOSIONS && explosions[i] != NULL)
	{
		i++;
	}
	if (i == NUM_EXPLOSIONS)
	{
		cerr<<"Need to increase NUM_EXPLOSIONS!"<<endl;
	}
	else if (explosions[i] == NULL)
	{
		explosions[i] = new Explosion(object->position[0], object->position[1]);
		glutTimerFunc(explosions[i]->EXPLOSION_LIFETIME_MSEC , &clean_explosion, i);
	}
}

void test_collision_asteroid_shot(int i, int j)
{
	if (asteroids[i] != NULL &&
	    shots[j] != NULL && 
		 shots[j]->active && 
		 collide_circle_point(
		 	asteroids[i]->position[0],
			asteroids[i]->position[1],
			asteroids[i]->boundsphereradius,
			shots[j]->position[0],
			shots[j]->position[1]))
	{
		cerr<<"asteroid "<<i<<" and shot "<<j<<" collided.\n";

		// Remove the shot
		shots[j]->active = false;
		// Increment the score if the shot was the player's
		if (shots[j]->evil == false)
		{
			if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_LARGE)
				increment_score(20);
			else if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_MED)
				increment_score(50);
			else if (asteroids[i]->boundsphereradius == SIZE_ASTEROID_SMALL)
				increment_score(100);
		}
		// Take care of whatever has to happen to the asteroids.
		damage_asteroid(i);
	}
}

void test_collision_playership_asteroid(int i)
{
	if (asteroids[i] != NULL &&
		 player_ship != NULL &&
		 collide_circle_circle(
		 	player_ship->position[0],
			player_ship->position[1],
			player_ship->boundsphereradius,
			asteroids[i]->position[0],
			asteroids[i]->position[1],
			asteroids[i]->boundsphereradius))
	{
		if (collide_polygon_circle(
			player_ship->profile,
			10,
			asteroids[i]->position[0],
			asteroids[i]->position[1],
			asteroids[i]->boundsphereradius))
		{
			cerr<<"asteroid "<<i<<" and player ship collided.\n";

			damage_playership();
			// Hurt the asteroid, causing an explosion if need be.
			damage_asteroid(i);
		}	
	}
}

void test_collision_enemyship_asteroid(int i)
{
	if (asteroids[i] != NULL &&
		 enemy_ship != NULL &&
		 enemy_ship->active &&
		 collide_circle_circle(
		 	enemy_ship->position[0],
			enemy_ship->position[1],
			enemy_ship->boundsphereradius,
			asteroids[i]->position[0],
			asteroids[i]->position[1],
			asteroids[i]->boundsphereradius))
	{
		if (collide_polygon_circle(
			enemy_ship->profile,
			10,
			asteroids[i]->position[0],
			asteroids[i]->position[1],
			asteroids[i]->boundsphereradius))
		{
			cerr<<"asteroid "<<i<<" and enemy ship collided.\n";
	
			// Hurt the enemy ship
			damage_enemyship();
			// Hurt the asteroid, causing an explosion if need be.
			damage_asteroid(i);
		}	
	}
}

void test_collision_playership_shot(int i)
{
	if (player_ship != NULL &&
		 shots[i] != NULL &&
		 shots[i]->evil == true &&
		 shots[i]->active &&
		 collide_circle_point(
		 	player_ship->position[0],
		 	player_ship->position[1],
		 	player_ship->boundsphereradius,
			shots[i]->position[0],
			shots[i]->position[1]))
	{
		if (collide_point_polygon(
			 	shots[i]->position[0],
				shots[i]->position[1],
				player_ship->profile,
				10))
		{
			cerr<<"playership and shot "<<i<<" collided.\n";
			damage_playership();
			shots[i]->active = false;
		}
	}
}

void test_collision_enemyship_shot(int i)
{
	if (enemy_ship != NULL &&
		 enemy_ship->active &&
		 shots[i] != NULL &&
		 shots[i]->evil == false &&
		 shots[i]->active &&
		 collide_circle_point(
		 	enemy_ship->position[0],
		 	enemy_ship->position[1],
		 	enemy_ship->boundsphereradius,
			shots[i]->position[0],
			shots[i]->position[1]))
	{
		if (collide_point_polygon(
			 	shots[i]->position[0],
				shots[i]->position[1],
				enemy_ship->profile,
				10))
		{
			cerr<<"enemyship and shot "<<i<<" collided.\n";
			if (enemy_ship->isSmall)
				increment_score(1000);
			else
				increment_score(200);
			damage_enemyship();
			shots[i]->active = false;
		}
	}
}

void test_collision_playership_enemyship()
{
	if (enemy_ship != NULL &&
		 enemy_ship->active &&
		 player_ship != NULL &&
		 collide_circle_circle(
		 	enemy_ship->position[0],
		 	enemy_ship->position[1],
		 	enemy_ship->boundsphereradius,
			player_ship->position[0],
			player_ship->position[1],
			player_ship->boundsphereradius))
	{
		if (collide_polygon_polygon(
				enemy_ship->profile,
				10,
				player_ship->profile,
				10))
		{
			cerr<<"playership and enemyship collided.\n";
			damage_playership();
			damage_enemyship();
		}
	}
}

void start_enemy_ship_moving(bool is_small)
{
	if (enemy_ship != NULL)
	{
		cerr<<"BEWARE OF A MEMORY LEAK!!!"<<endl;
	}
	enemy_ship = new EnemyShip(is_small);
	enemy_ship->position[0]=0;
	enemy_ship->position[1]=randf(bound_bottom,bound_top);
	enemy_ship->position[2]=0;
	enemy_ship->velocity[0]=randf(5,10);
	enemy_ship->velocity[1]=randf(-10,10);
	enemy_ship->velocity[2]=0;
	switch (rand() % 2)
	{
		case 0: /* Put the enemy ship on the left border; need x>0 */
			enemy_ship->velocity[0]*=1;
			enemy_ship->position[0]=bound_left;
			break;
		case 1: /* Put the enemy_ship on the right border; need x<0 */
			enemy_ship->velocity[0]*=-1;
			enemy_ship->position[0]=bound_right;
			break;
	}
	// Make enemy ship fire and move randomly every ENEMY_SHOOTS_EVERY_N_MS ms
	enemy_ship_fire_and_move(ENEMY_SHOOTS_EVERY_N_MS);
}

void increment_score(int points)
{
	int old_score = current_score;
	current_score += points;
	cerr<<" Score += "<<points<<" to make "<<current_score<<endl;
	if (((current_score / EXTRA_PLAYERSHIP_EVERY_N_POINTS) > (old_score / EXTRA_PLAYERSHIP_EVERY_N_POINTS))
		&& (old_score > 0))
	{
		cerr<<"Awarding an extra ship"<<endl;
		num_player_ships_left++;
	}
}

void clean_shot(int index)
{
	if (shots[index] == NULL)
	{
		cerr<<"This shot has already been deleted!\n";
	}
	else
	{
		delete shots[index];
		shots[index]=NULL;
	}
}

void clean_explosion(int index)
{
	if (explosions[index] == NULL)
	{
		cerr<<"This Explosion has already been deleted!\n";
	}
	else
	{
		delete explosions[index];
		explosions[index]=NULL;
	}
}

void enemy_ship_fire_and_move(int num_ms)
{
	if (enemy_ship == NULL)
	{
		cerr<<"Why is enemy_ship null in enemy_ship_fire_and_move?"<<endl;
		// this is OK; it means that the enemy_ship has reached the other side and has been
		// culled.
		return;
	}
	else if (enemy_ship->active == false)
	{
		delete enemy_ship;
		enemy_ship = NULL;
		return;
	}
	// Make y movement random
	enemy_ship->velocity[1]=randf(-10,10);

	// make ship maybe fire
	float prob_asteroid, prob_playership, rand_val;
	if (enemy_ship->isSmall)
	{
		// Then it should be hard
		prob_asteroid = 0.4;
		prob_playership = 0.5;
	}
	else
	{
		// Make it a little easier
		prob_asteroid = 0.3;
		prob_playership = 0.2;
	}
	rand_val = randf(0,1);
	if (rand_val < prob_asteroid && (num_active_asteroids > 0))
	{
		// Fire towards a random asteroid
		int which_asteroid = rand() % num_active_asteroids;
		int j = 0;
		do {
			if (asteroids[j] != NULL)
			{
				if (which_asteroid==0)
				{
					// Fire towards this asteroid
					// Find a free element of shots[]
					int i=0;
					while (i<NUM_SHOTS && shots[i]!=NULL)
					{
						i++;
					}
					if (i==NUM_SHOTS)
					{
						cerr<<"Alert!  NUM_SHOTS needs to be increased based on your shot rate\n";
					}
					else
					{
						shots[i] = new Shot(enemy_ship, asteroids[j], true);
						glutTimerFunc(SHOT_LIFETIME_MILLISEC, &clean_shot, i);
					}
				}
				which_asteroid--;
			}
			j++;
		} while (which_asteroid >= 0 && j<NUM_ASTEROIDS);
		
	}
	else if (rand_val < (prob_asteroid + prob_playership) && (player_ship != NULL))
	{
		// Fire towards the player ship
		if (player_ship != NULL)
		{
			// Find a free element of shots[]
			int i=0;
			while (i<NUM_SHOTS && shots[i]!=NULL)
			{
				i++;
			}
			if (i==NUM_SHOTS)
			{
				cerr<<"Alert!  NUM_SHOTS needs to be increased based on your shot rate\n";
			}
			else
			{
				shots[i] = new Shot(enemy_ship, player_ship, true);
				glutTimerFunc(SHOT_LIFETIME_MILLISEC, &clean_shot, i);
			}
		}
	}
	else
	{
		// Fire randomly; we'll just fire in whatever direction the ship is pointing
		// Find a free element of shots[]
		int i=0;
		while (i<NUM_SHOTS && shots[i]!=NULL)
		{
			i++;
		}
		if (i==NUM_SHOTS)
		{
			cerr<<"Alert!  NUM_SHOTS needs to be increased based on your shot rate\n";
		}
		else
		{
			shots[i] = new Shot(enemy_ship, true);
			glutTimerFunc(SHOT_LIFETIME_MILLISEC, &clean_shot, i);
		}
	}

	// Set to run again
	glutTimerFunc(num_ms, &enemy_ship_fire_and_move, num_ms);
}

void zero_string(int blah)
{
	strcpy(current_message,"");
	if (player_ship == NULL)
	{
		start_game();
	}
}

/*
 * key = ASCII code of key pressed.
 * (x,y) = window coordinates of mouse at time of key press
 */
void handle_keyboard(unsigned char key, int x, int y)
{
	if (player_ship == NULL)
		return;
	switch(key)
	{
		case 'x':
		case 'X':
			player_ship->keydown_forward=1;
			break;
		case 'z':
		case 'Z':
			player_ship->keydown_fire=1;
			break;
		case 'q':
			shutdown();
			break;
		default:
			cerr<<"Unrecognized Key Pressed: '"<<key<<"' (ascii code "<<(int)(key)<<")"<<endl;
	}
}

void handle_keyboard_up(unsigned char key, int x, int y)
{
	if (player_ship == NULL)
		return;
	switch(key)
	{
		case 'x':
		case 'X':
			player_ship->keydown_forward=0;
			break;
		case 'z':
		case 'Z':
			player_ship->keydown_fire=0;
			break;
		case 'q':
			break;
		default:
			cerr<<"Unknown Key Unpressed: '"<<key<<"' (ascii code "<<(int)(key)<<")"<<endl;
	}
}

void handle_keyboard_special(int key, int x, int y)
{
	if (player_ship == NULL)
		return;
	switch(key)
	{
		case GLUT_KEY_LEFT:
			player_ship->keydown_left=1;
			player_ship->keydown_right=0;
			break;
		case GLUT_KEY_RIGHT:
			player_ship->keydown_left=0;
			player_ship->keydown_right=1;
			break;
		default:
			cerr<<"Other special key ("<<key<<") pressed"<<endl;
	}
}

void handle_keyboard_special_up(int key, int x, int y)
{
	if (player_ship == NULL)
		return;
	switch(key)
	{
		case GLUT_KEY_LEFT:
			player_ship->keydown_left=0;
			break;
		case GLUT_KEY_RIGHT:
			player_ship->keydown_right=0;
			break;
		default:
			cerr<<"Other special key ("<<key<<") unpressed"<<endl;
	}
}

void handle_mouse(int button, int state, int x, int y)
{
	cerr<<"Mouse Button Event: button="<<button<<", state="<<state<<", x="<<x<<", y="<<y<<endl;
}

void menu_setup()
{
	glutCreateMenu(&menu_callback);
	glutAddMenuEntry("New Game",0);
	glutAddMenuEntry("Quit",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menu_callback(int value)
{
	cerr<<"menu_callback with value="<<value<<endl;
	switch (value)
	{
		case 0:
			start_game();
			break;
		case 1:
			shutdown();
			break;
		default:
			cerr<<"menu_callback: unknown return value!!"<<endl;
	}
}

void window_reshape_callback(int width, int height)
{
	// Set bounds
	float size=50.0;
	bound_left=-(width/2.0)*(1.0/size);
	bound_right=(width/2.0)*(1.0/size);
	bound_bottom=-(height/2.0)*(1.0/size);
	bound_top=(height/2.0)*(1.0/size);

	// If they changed the shape of the window, change the perspective to adapt.
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// Orthogonal projection instead of perspective projection
	glOrtho(bound_left,bound_right,bound_bottom,bound_top,-10,10);
	glViewport(0,0,width,height);
	screen_width = width;
	screen_height = height;
	cerr<<"Adjusting projection matrix to adapt to new window size ("<<width<<","<<height<<")"<<endl;
}

void start_game()
{
	end_game();
	current_level = 0;
	num_player_ships_left = INIT_NUM_PLAYERSHIPS_LEFT;
	current_score = 0;
	init_next_level();
}

void end_game()
{
	if (player_ship != NULL)
	{
		delete player_ship;
		player_ship = NULL;
	}
	if (enemy_ship != NULL)
	{
		delete enemy_ship;
		enemy_ship = NULL;
	}
	for (int i=0;i<NUM_ASTEROIDS;i++)
	{
		if (asteroids[i] != NULL)
		{
			delete asteroids[i];
			asteroids[i]=NULL;
		}
	}
	for (int i=0;i<NUM_SHOTS;i++)
	{
		if (shots[i] != NULL)
		{
			delete shots[i];
			shots[i]=NULL;
		}
	}
	for (int i=0;i<NUM_EXPLOSIONS; i++)
	{
		if (explosions[i] != NULL)
		{
			delete explosions[i];
			explosions[i] = NULL;
		}
	}
}

int shutdown(void)
{
	end_game();
	glutDestroyWindow(1);
	cerr<<"Exiting..."<<endl;
	exit(0);
}
