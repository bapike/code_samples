/*
 * Author: Brian Pike
 * Class: CSC461-001 (Graphics), NC State University, Fall 2004, Dr. Healey
 */

#include<stdlib.h>
#include<GL/glut.h>
#include<iostream>
#include<math.h>
#include "gameobj.h"
#include "flame.h"

using namespace std;

int screen_width,
    screen_height;

float bound_left,
      bound_right,
      bound_top,
      bound_bottom;

/* Apparently math.h on VC++ doesn't have M_PI.  Grr... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif /* M_PI */

static const float DEG2RAD=M_PI/180.0;

#define DRAW_TRIANGLE(a,b,c) { glBegin(GL_POLYGON); glVertex3fv((a)); glVertex3fv((b)); glVertex3fv((c)); glEnd(); }

static void calcNormal(float normal[3], float a[3], float b[3], float c[3])
{
	float d0[3],d1[3];
	float length;

	for (int i=0;i<3; i++)
	{
		d0[i]=b[i]-a[i];
		d1[i]=c[i]-b[i];
	}
	normal[0]=d0[1]*d1[2]-d0[2]*d1[1];
	normal[1]=d0[2]*d1[0]-d0[0]*d1[2];
	normal[2]=d0[0]*d1[1]-d0[1]*d1[0];
	length = sqrtf(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
	normal[0] /= length;
	normal[1] /= length;
	normal[2] /= length;
}

float randf(float lower, float upper)
{
	float retvalue;

	retvalue = (rand())/(RAND_MAX+1.0)*(upper-lower);
	/*
	 * So retvalue is a float between 0 and (upper-lower)
	 * If we add lower to both sides, we get [lower,upper]
	 */
	retvalue += lower;
	return retvalue;
}

GameObject::GameObject()
{
	for (int i=0;i<3;i++)
	{
		position[i]=0.0;
		orientation[i]=0.0;
		velocity[i]=0.0;
		rotvelocity[i]=0.0;
	}
	boundsphereradius = 0.0;
}

GameObject::~GameObject() {}

void GameObject::draw_at_origin() {}
void GameObject::update_custom(float time) {}

void GameObject::draw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslatef(position[0],position[1],position[2]);
		glRotatef(orientation[0],1,0,0);
		glRotatef(orientation[1],0,1,0);
		glRotatef(orientation[2],0,0,1);
		draw_at_origin();
	glPopMatrix();
}

void GameObject::update(float time)
{
	update_custom(time);

	for (int i=0;i<3;i++)
	{
		position[i] += time*velocity[i];
		orientation[i] += time*rotvelocity[i];
		
		// Prevent winding.
		if (orientation[i] < 0)
		{
			orientation[i] += 360.0;
		} else if (orientation[i] > 360.0)
		{
			orientation[i] -= 360.0;
		}
	}

	// Wrap for Left and Right sides of screen
	if (position[0] < bound_left)
		position[0]=bound_right;
	else if (position[0] > bound_right)
		position[0]=bound_left;
	// And for top and bottom
	if (position[1] < bound_bottom)
		position[1]=bound_top;
	else if (position[1] >bound_top)
		position[1]=bound_bottom;
}

/*
 *                       PlayerShip
 */

PlayerShip::PlayerShip()
 : GameObject()
{
	keydown_left = 0;
	keydown_right = 0;
	keydown_forward = 0;
	keydown_fire = 0;
	boundsphereradius = 0.7;
	/* Setup for this game specifically */
	orientation[0]=270;
}

float PlayerShip::unrotatedprofile[10][2] = {
		{  0.0,  0.5},
		{-0.20, 0.2},
		{-0.7, 0.3},
		{-0.65,-0.2},
		{-0.15,-0.2},
		{  0.0,-0.5},
		{0.15,-0.2},
		{0.65,-0.2},
		{0.7,0.3},
		{0.20,0.2}};

void PlayerShip::draw_at_origin()
{
	float mats[4][4] = {
		{ 0, 1, 0, 0},
		{ 1, 0, 0, 0},
		{ 0, 0, 1, 0},
		{ 1, 1, 1, 0}};
	glMatrixMode(GL_MODELVIEW);

	// Draw the body of the thing
	glPushMatrix();
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[0]);
		glScalef(0.25,0.25,1);
		glutSolidSphere(0.5,8,5);
	glPopMatrix();
	
	// Draw a crossbar piece that connects, almost like a wing but a squashed cube
	glPushMatrix();
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[1]);
		glScalef(1,0.05,0.1);
		glutSolidCube(1);
	glPopMatrix();

	// Now on either side, draw a sphere surrounding the tips of the crossbar piece and a cone that represents
	// the weapons.
	glPushMatrix();
		glTranslatef(0.5,0,0);
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[2]);
		glutSolidSphere(0.2,5,5);
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[3]);
		glutSolidCone(0.1,.4,8,4);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-0.5,0,0);
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[2]);
		glutSolidSphere(0.2,5,5);
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[3]);
		glutSolidCone(0.1,.4,8,4);
	glPopMatrix();

/* To overlay the collision detection polygon: */
/*	glNormal3f(0,0,1);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[3]);
	glBegin(GL_POLYGON);
	for (int i=0;i<10;i++)
		glVertex3f(unrotatedprofile[i][0],-4,unrotatedprofile[i][1]);
	glEnd();*/
}

void PlayerShip::update_custom(float time)
{
	static float ROT_AMT=900.0;
	static float FWD_AMT=500.0;
	static float MAX_SPEED=50.0;
	static float SLOW_AMT=100;
	float speed;
	
	// Use keystrokes for something
	if (keydown_left)
	{
		// Using orientation[] instead of rotvelocity[] is unrealistic but easy
		orientation[1] -= ROT_AMT*time;
	}
	if (keydown_right)
	{
		orientation[1] += ROT_AMT*time;
	}
	if (keydown_forward)
	{
		velocity[0] += sin(DEG2RAD*orientation[1])*FWD_AMT*time;
		velocity[1] += cos(DEG2RAD*orientation[1])*FWD_AMT*time;
	}

	// Slow down.
	speed=sqrtf(velocity[0]*velocity[0]+velocity[1]*velocity[1]);
	if (speed > SLOW_AMT*time)
	{
		float newspeed=speed - SLOW_AMT*time;
		velocity[0] *=(newspeed/speed);
		velocity[1] *=(newspeed/speed);

		speed=sqrtf(velocity[0]*velocity[0]+velocity[1]*velocity[1]);
	} else
	{
		velocity[0]=0;
		velocity[1]=0;
		speed=0;
	}

	// Cap speed
	if (speed > MAX_SPEED)
	{
		velocity[0]*=(MAX_SPEED/speed);
		velocity[1]*=(MAX_SPEED/speed);
	}

	// Update collision detection poly;
	float sinrot, cosrot;
	sinrot = sin(DEG2RAD * orientation[1]);
	cosrot = cos(DEG2RAD * orientation[1]);

	for (int i=0;i<10;i++)
	{
		profile[i][0]= (unrotatedprofile[i][0]*cosrot) + (unrotatedprofile[i][1]*sinrot) + position[0];
		profile[i][1]= (unrotatedprofile[i][1]*cosrot) - (unrotatedprofile[i][0]*sinrot) + position[1];
	}
}

/*
 *                       EnemyShip
 */

/* 
 * How to draw a tetrahedron (this information from mathworld.wolfram.com):
 * Given side length a, let:
 *   x=(1/3)*sqrt(3)*a
 *   h=sqrt(2)*x
 *   d=(1/2)*x
 * then the vertices are located at:
 *   (x,0,0), (-d,0,a/2), (-d,0,-a/2), (0,h,0)
 * We also multiply the y-coordinate by 1/2 to snub the nose of the tetrahedron.
 */
float EnemyShip::vertices[5][3]={
	{(1.0/3.0)*sqrtf(3.0),0,0},
	{-(1.0/6.0)*sqrtf(3.0),0,1.0/2.0},
	{-(1.0/6.0)*sqrtf(3.0),0,-1.0/2.0},
	{0,0.2*(1.0/3.0)*sqrtf(6.0),0},
	{0,0.2*-(1.0/3.0)*sqrtf(6.0),0}};

int EnemyShip::faces[6][3]={
		{0,2,3},
		{1,0,3},
		{2,1,3},
		{4,2,0},
		{4,0,1},
		{4,1,2}};

/* These would be messy to do here, so we set these in the constructor */
float EnemyShip::normals[6][3]={{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

EnemyShip::EnemyShip(bool whetherSmall)
 : GameObject()
{
	for (int i=0;i<6;i++)
	{
		calcNormal(normals[i],vertices[ faces[i][0] ],
		                      vertices[ faces[i][1] ],
		                      vertices[ faces[i][2] ]);
	}
	orientation[0] = 270;
	rotvelocity[1] = 360*5;
	active = true;
	isSmall = whetherSmall;
	if (isSmall)
	{
		boundsphereradius = 0.5;
	}
	else
	{
		boundsphereradius = 1.0;
	}
}

float EnemyShip::unrotatedprofile[10][2] = {
		  {0.15,0.5},
		  {-0.15,0.5},
		  {-0.15,0.15},
		  {-0.25,0},
		  {-0.15,-0.15},
		  {-0.15,-0.5},
		  {0.15,-0.5},
		  {0.15,-0.15},
		  {0.25,0},
		  {0.15,0.15}};

void EnemyShip::draw_at_origin()
{
	float mats[2][4]={
		{0,1,0,0},
		{1,0,0,0}};
	if (active == false)
		return;
	glMatrixMode(GL_MODELVIEW);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[0]);
	glPushMatrix();
		//glutSolidSphere(boundsphereradius,10,10);
		if (isSmall)
			glScalef(0.4,0.4,1);
		else
			glScalef(0.8,0.8,2);

		for (int i=0;i<6; i++)
		{
			glNormal3fv(normals[i]);
			DRAW_TRIANGLE(vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]] );
		}
	
		glPushMatrix();
			glRotatef(60,0,1,0);
			for (int i=0;i<6; i++)
			{
				glNormal3fv(normals[i]);
				DRAW_TRIANGLE(vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]] );
			}
		glPopMatrix();

		glTranslatef(0,0.1,0);
		glutSolidSphere(0.2,5,5);
	glPopMatrix();

	/* To overlay the collision detection polygon: */
/*	glNormal3f(0,0,1);
	float scale = 2.0;
	if (isSmall)
		scale = 1.0;
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mats[1]);
	glBegin(GL_POLYGON);
	for (int i=0;i<10;i++)
	{
		glVertex3f(scale*unrotatedprofile[i][0],-4,scale*unrotatedprofile[i][1]);
	}
	glEnd();*/
}

void EnemyShip::update_custom(float time)
{
	// Update collision detection poly;
	float sinrot, cosrot;
	sinrot = sin(DEG2RAD * orientation[1]);
	cosrot = cos(DEG2RAD * orientation[1]);

	if (velocity[0]>0)
	{
		// They're moving to the right
		if (fabs(bound_right - position[0])<0.1)
		{
			// It's close enough to the border that we should delete it; set a flag
			active = false;
		}
	}
	else
	{
		// They're moving to the left 
		if (fabs(bound_left - position[0])<0.1)
		{
			// It's close enough to the border that we should delete it; set a flag
			active = false;
		}
	}

	float scale = 2.0;
	if (isSmall)
		scale = 1.0;
	
	for (int i=0;i<10;i++)
	{
		profile[i][0] = scale * ((unrotatedprofile[i][0]*cosrot) + (unrotatedprofile[i][1]*sinrot)) + position[0];
		profile[i][1] = scale * ((unrotatedprofile[i][1]*cosrot) - (unrotatedprofile[i][0]*sinrot)) + position[1];
	}
}

int Asteroid::faces[][3]={
	/* Between the top point and the second layer */
	{ 0, 1, 2},
	{ 0, 2, 3},
	{ 0, 3, 4},
	{ 0, 4, 5},
	{ 0, 5, 6},
	{ 0, 6, 1},
	/* Between the 2nd layer and the 3rd layer */
	{ 2, 1, 7},
	{ 2, 7, 8},
	{ 3, 2, 8},
	{ 3, 8, 9},
	{ 4, 3, 9},
	{ 4, 9,10},
	{ 5, 4,10},
	{ 5,10,11},
	{ 6, 5,11},
	{ 6,11,12},
	{ 1, 6,12},
	{ 1,12, 7},
	/* Between the 3rd layer and the bottom point */
	{13, 8, 7},
	{13, 9, 8},
	{13,10, 9},
	{13,11,10},
	{13,12,11},
	{13, 7,12} };

Asteroid::Asteroid(float size)
 : GameObject()
{
	double pi=M_PI;
	double scale;

	vertices[0][0]=0;
	vertices[0][1]=1;
	vertices[0][2]=0;
	for (int i=1; i<7; i++)
	{
		// In the middle two slices, the x and z coordinates should be basically the roots of unity, scaled so that
		// the length of the vector (including the y coordinate which is the same through each slice) is ~1.0.
		vertices[i][0]=(sqrt(1-(1.0/3.0)*(1.0/3.0)))*cos((2.0/6.0)*(pi)*(i-1));
		vertices[i][1]=(1.0/3.0);
		vertices[i][2]=(sqrt(1-(1.0/3.0)*(1.0/3.0)))*-sin((2.0/6.0)*(pi)*(i-1));
		vertices[i+6][0]=vertices[i][0];
		vertices[i+6][1]=(-1.0/3.0);
		vertices[i+6][2]=vertices[i][2];
	}
	vertices[13][0]=0;
	vertices[13][1]=-1;
	vertices[13][2]=0;

	// All of the vertices should be 1 unit from the origin.
	// Now we will obfuscate them by scaling each vertex by a certain amount.
	for (int i=0;i<14;i++)
	{
		/*
		 * This sucks.  Why doesn't MS support SVID random number functions
		 * that would give me a random float?  Get a random integer, take it
		 * modulo a large number, cast it to a float, divide by that large
		 * number, and get a number in (0.0,1.0).  Let's make this (0.5,1.0)
		 * by adding 0.5. Then we scale by this amount, remembering to take
		 * into account the 'size' parameter.
		 */
		 scale = size*((((float)(rand() % 100000))/100000.0)+0.25);
		 vertices[i][0]*=scale;
		 vertices[i][1]*=scale;
		 vertices[i][2]*=scale;
	}
	for (int i=0;i<24; i++)
	{
		calcNormal(normals[i],vertices[ faces[i][0] ],
		                      vertices[ faces[i][1] ],
									 vertices[ faces[i][2] ]);
	}
	boundsphereradius = size;

	/* Setup the velocity and random rotation */
	rotvelocity[0]=randf(-150,150.0);
	rotvelocity[1]=randf(-150,150.0);
	rotvelocity[2]=randf(-150,150.0);
	velocity[0]=randf(0,10.0);
	velocity[1]=randf(0,10.0);
}

void Asteroid::draw_at_origin()
{
	float mat[4]={.6,.3,.2,0};

	// We have 4 layers in the spherelike blob.
	glMatrixMode(GL_MODELVIEW);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mat);

	for (int i=0;i<24;i++)
	{
		glNormal3fv(normals[i]);
		DRAW_TRIANGLE(vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]]);
	}
}

/*
 * Shot
 */
Shot::Shot(GameObject *objectfiring, bool whetherEvil)
 : GameObject()
{
	static float shot_speed = 30.0;

	velocity[0] = (shot_speed * sin(DEG2RAD * objectfiring->orientation[1])) + objectfiring->velocity[0];
	velocity[1] = (shot_speed * cos(DEG2RAD * objectfiring->orientation[1])) + objectfiring->velocity[1];
	velocity[2] = 0 + objectfiring->velocity[2];

	position[0] = objectfiring->position[0];
	position[1] = objectfiring->position[1];
	position[2] = objectfiring->position[2];

	orientation[0] = 0;
	orientation[1] = 0;
	orientation[2] = 0;

	active = true;
	evil = whetherEvil;
}

Shot::Shot(GameObject *objectfiring, GameObject *towards, bool whetherEvil)
	: GameObject()
{
	static float shot_speed = 30.0;

	position[0] = objectfiring->position[0];
	position[1] = objectfiring->position[1];
	position[2] = objectfiring->position[2];

	orientation[0] = 0;
	orientation[1] = 0;
	orientation[2] = 0;

	active = true;
	evil = whetherEvil;

	/*
	 * We want to fire in the direction of 'towards' at the normal shot_speed,
	 * adjusted by the velocity of the object firing
	 */
	velocity[0] = towards->position[0] - objectfiring->position[0];
	velocity[1] = towards->position[1] - objectfiring->position[1];
	velocity[2] = 0;
	// Normalize the vector
	float length = sqrt((velocity[0] * velocity[0]) + (velocity[1] * velocity[1]));
	velocity[0] *= (shot_speed/length);
	velocity[1] *= (shot_speed/length);
	velocity[0] += objectfiring->velocity[0];
	velocity[1] += objectfiring->velocity[1];
	velocity[2] += objectfiring->velocity[2];
}

void Shot::draw_at_origin()
{
	float mats[2][4] = {
		{1,1,0,0}, // yellow for good
		{1,0,0,0}};// red for evil
	float *mat;
	if (evil)
		mat = mats[1];
	else
		mat = mats[0];

	if (active)
	{
		glMatrixMode(GL_MODELVIEW);

		glPointSize(4);
		glBegin( GL_POINTS );
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mat);
			glNormal3f(0,0,1);
			/* Since GameObject::draw() already translated for us, we don't need to! */
			glVertex3f(0,0,0);
		glEnd();

	/*	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float blah = 4.0; randf(0,2.0);
		glPixelZoom(blah,blah);
		//glRasterPos2f(0,0);
		glRasterPos2f(-1,-1);
		glDrawPixels(50,50,GL_RGBA,GL_UNSIGNED_BYTE,flame_data);
		glBlendFunc(GL_ONE,GL_ZERO);*/
	}
}

/*
 * Explosion 
 */
Explosion::Explosion(float x, float y)
 : GameObject()
{
	position[0] = x;
	position[1] = y;
	position[2] = 5;

	EXPLOSION_LIFETIME_MSEC = 500;
	EXPLOSION_MAX_SIZE = 3.0f;

	size = 0.1;
	// TODO: Give random rotation?
}

int Explosion::EXPLOSION_LIFETIME_MSEC = 0;
float Explosion::EXPLOSION_MAX_SIZE = 0;

void Explosion::draw_at_origin()
{
	float posx, posy;
	
	glMatrixMode(GL_MODELVIEW);
	posx = size*50*(bound_top-bound_bottom)/(screen_height);
	posy = size*50*(bound_right-bound_left)/(screen_width);
	glPixelZoom(size,size);
	glRasterPos2f(-posx/2,-posy/2);
	glDrawPixels(50,50,GL_RGBA,GL_UNSIGNED_BYTE,flame_data);
}

void Explosion::update_custom(float time)
{
	size += time* ((EXPLOSION_MAX_SIZE*1000.0) / EXPLOSION_LIFETIME_MSEC);
}
