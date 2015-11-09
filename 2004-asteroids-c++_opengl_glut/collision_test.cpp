#include<iostream>

using namespace std;
#include "collision.h"

#define DO(x,y) {cout<<(#x)<<endl<<"Expected: "<<y<<" and got "<<x<<endl;}

int main()
{
	cerr<<"starting"<<endl;
/*	// circle-circle: no, tangent, intersecting*3
	DO( collide_circle_circle(0,0,1,3,0,1),0);
	DO( collide_circle_circle(0,0,1,2,0,1),1);
	DO( collide_circle_circle(0,0,1,1,0,1),1);
	DO( collide_circle_circle(0,0,1,0,0,1),1);
	DO( collide_circle_circle(0,0,1,0,0,1),1);*/

/*	DO( collide_circle_point(0,0,1,2,0),0);
	DO( collide_circle_point(0,0,1,1,0),1);
	DO( collide_circle_point(0,0,1,0,0),1);
	DO( collide_circle_point(0,0,1,00.9,0),1);*/

	DO( collide_segment_circle( -1,1,1,1,  0,0,1), 1);
	DO( collide_segment_circle( -1,1,1,1,  0,0,0.9), 0);
	DO( collide_segment_circle( -1,1,1,1,  0,0,1.1), 1);
	DO( collide_segment_circle( -1,1,-2,1,  0,0,1.1), 0);
	float asdf[10][2] = {{0,0.93},{-0.2,0.63},{-0.7,0.73},{-0.65,0.23},{-0.15,0.23},{0,-0.07},{0.15,0.23},{0.65,0.23},{0.7,0.73},{0.2,0.63}};
	DO( collide_segment_circle(asdf[1][0],asdf[1][1],asdf[2][0],asdf[2][1],-5.54063,4.67934,1), 0);
	
/*	// parallel
	DO( collide_segment_segment(-1,0, 0,0, 1,0, 2,0),0);
	DO( collide_segment_segment(-1,0, 0,0, 1,0, 0,0),1);
	DO( collide_segment_segment(-1,0, 0,0, 1,0, -0.5,0),1);

	DO( collide_segment_segment(1,0,2,0, -1,0, 0,0),0);
	DO( collide_segment_segment(1,0, 0,0,-1,0, 0,0),1);
	DO( collide_segment_segment(1,0, -0.5,0,-1,0, 0,0),1);


	// not parallel
	DO( collide_segment_segment(0,0, 1,0, 1,1, 1,2),0);
	DO( collide_segment_segment(0,0, 1,0, 1,0.01, 1,2),0);
	DO( collide_segment_segment(0,0, 1,0, 1,0, 1,2),1);
	DO( collide_segment_segment(0,0, 2,0, 1,0, 1,2),1);
	DO( collide_segment_segment(0,0, 2,0, 1,-1, 1,2),1);

	DO( collide_segment_segment(1,1, 1,2,   0,0,1,0),0);
	DO( collide_segment_segment(1,0.01, 1,2,  0,0,1,0),0);
	DO( collide_segment_segment(1,0, 1,2,    0,0,1,0),1);
	DO( collide_segment_segment(1,0, 1,2,    0,0,2,0),1);
	DO( collide_segment_segment(1,-1, 1,2,   0,0,2,0),1);*/

/*	float square[4][2] = { {1,1}, {-1,1}, {-1,-1}, {1,-1}};
	DO( collide_point_polygon(2,0, square,4),0);
	DO( collide_point_polygon(1,0, square,4),1);
	DO( collide_point_polygon(0,0, square,4),1);
	DO( collide_point_polygon(1,1, square,4),1);
	DO( collide_point_polygon(0.999,0.999, square,4),1);
	DO( collide_segment_segment(0,0,1,0, 0,0,0,1),1);
	DO( collide_segment_segment(1,1,5,5, 1,1,1,0),1);
	DO( collide_segment_segment(1,1,5,5, 1,1,-1,1),1);*/
	
/*	float square[4][2] = { {1,1}, {-1,1}, {-1,-1}, {1,-1}};
	DO( collide_segment_polygon(0,0,0,5, square,4),1);
	DO( collide_segment_polygon(1,0,0,5, square,4),1);
	DO( collide_segment_polygon(1,1,0,5, square,4),1);
	DO( collide_segment_polygon(-2,0.99,2,0.99, square,4),1);
	DO( collide_segment_polygon(-2,1,2,1, square,4),1);
	DO( collide_segment_polygon(-2,1.01,2,1.01, square,4),0);*/

/*	float blah[10][2] = {{0,0.6},{-0.2,0.3},{-0.7,0.4},{-0.65,-0.1},{-0.15,-0.1},{0,-0.4},{0.15,-0.1},{0.65,-0.1},{0.7,0.4},{0.2,0.3}};
	DO( collide_polygon_circle(blah,10,-8.82333,3.4771,1), 0);
	DO( collide_polygon_circle(blah,10,6.82285,4.84775,1), 0);
	DO( collide_polygon_circle(blah,10,-3.8043,1.73174,1), 0);*/

//	float asdf[10][2] = {{0,0.93},{-0.2,0.63},{-0.7,0.73},{-0.65,0.23},{-0.15,0.23},{0,-0.07},{0.15,0.23},{0.65,0.23},{0.7,0.73},{0.2,0.63}};
/*	DO( collide_polygon_circle(asdf,10,-5.54063,4.67934,1), 0);*/

	
	return 0;
}
