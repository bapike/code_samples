#ifndef __gameobj_h_
#define __gameobj_h_

extern int screen_width,
           screen_height;

extern float bound_left,
	     bound_right,
	     bound_top,
	     bound_bottom;

class GameObject
{
public:
	float position[3];
	// rotation about x,y,z axes. They're orthogonal, so order
	//   shouldn't matter -- right?
	float orientation[3];
	
	float velocity[3];    // The velocity here should be in units/sec
	float rotvelocity[3]; // The rotational velocity is in degrees/sec
	float boundsphereradius;
	
	GameObject();
	virtual void draw_at_origin();
	virtual void update_custom(float time);
	void draw();
	void update(float time);
	virtual ~GameObject();
};

class PlayerShip : public GameObject
{
public:
	PlayerShip();
	void draw_at_origin();
	void update_custom(float time);
	
	unsigned char keydown_left;
	unsigned char keydown_right;
	unsigned char keydown_forward;
	unsigned char keydown_fire;
	float profile[10][2];
private:
	static float unrotatedprofile[10][2];
};

class EnemyShip : public GameObject
{
public:
	EnemyShip(bool whetherSmall);
	void draw_at_origin();
	void update_custom(float time);
	float profile[10][2];
	bool active;
	bool isSmall;
private:
	static float vertices[5][3];
	static int faces[6][3];
	static float normals[6][3];
	static float unrotatedprofile[10][2];
};

class Asteroid : public GameObject
{
public: 
	Asteroid(float size);
	void draw_at_origin();
private:
	float vertices[14][3];
	static int faces[24][3];
	float normals[24][3];
};

class Shot : public GameObject
{
public:
	Shot(GameObject *objectfiring, bool whetherEvil);
	Shot(GameObject *objectfiring, GameObject *towards, bool whetherEvil);
	void draw_at_origin();
	/* If a shot hits something, we don't want to have a callback delete a Shot a few milliseconds later.  We just disable it instead */
	bool active;
	bool evil;
};

class Explosion : public GameObject
{
public:
	Explosion(float x, float y);
	void draw_at_origin();
	void update_custom(float time);
	float size;
	static int EXPLOSION_LIFETIME_MSEC;
	static float EXPLOSION_MAX_SIZE;
};

float randf(float low, float high);

#endif /* __gameobj_h_ */
