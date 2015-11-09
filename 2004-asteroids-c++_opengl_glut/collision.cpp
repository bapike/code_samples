#include "collision.h"
#include <math.h>
#include<iostream>
using namespace std;

static inline float square(float a)
{
	return a*a;
}

static inline float distance_squared( float x1, float y1, float x2, float y2)
{
	return (square(x1-x2)+square(y1-y2));
}

bool collide_circle_circle(float centerx1, float centery1, float radius1, float centerx2, float centery2, float radius2)
{
	/*
	 * If the circles are close enough that the sum of their radii is greater
	 * than the distance between their centers, then we want to return true
	 * because they touch.  Otherwise, false.
	 */
	return (square(radius1+radius2)>=distance_squared(centerx1, centery1, centerx2, centery2));
}

bool collide_circle_point(float centerx, float centery, float radius, float pointx, float pointy)
{
	/*
	 * If the distance between the point and the center are small enough to
	 * be less than the radius, then we want to return true.  Otherwise false.
	 */
	return (square(radius)>=distance_squared(centerx, centery, pointx, pointy));
}

bool collide_segment_circle(
	float ax, float ay, float bx, float by,
	float centerx, float centery, float radius)
{
	float twoa,b,c,delta;

	/* TODO: check if endpoints are in circle? */

	twoa = 2.0f*(square(bx-ax)+square(by-ay));
	b = 2.0f*((bx-ax)*(ax-centerx)+(by-ay)*(ay-centery));
	c = square(ax-centerx)+square(ay-centery) - square(radius);
	delta = square(b)-2*twoa*c;
	if (delta<0)
		return false;
	else if (delta==0)
		return true;
	else  /* (delta>0) */
	{
		/* We have two possible intersection points */
		float t;
		
		delta = sqrtf(delta);
		t=(-b+delta)/twoa;
		if (0<=t && t<=1)
			return true;
		t=(-b-delta)/twoa;
		if (0<=t && t<=1)
			return true;
		return false;
	}
}

bool collide_segment_segment(float ax, float ay, float bx, float by, float sx, float sy, float tx, float ty)
{
	float den, t1, t2;
	den = (ty-sy)*(bx-ax)-(tx-sx)*(by-ay);
	/* Calculate the numerators */
	t1 = (tx-sx)*(ay-sy)-(ty-sy)*(ax-sx);
	t2 = (bx-ax)*(ay-sy)-(by-ay)*(ax-sx);
	if (fabsf(den)<0.00005)
	{
		/* We know the lines are parallel */
		if (fabsf(t1)<0.00005 || fabsf(t2)<0.00005)
		{
			/*
			 * We know the lines are coincident. Are the segments?
			 * This is actually a pretty hard problem.  Let's pick the longest
			 * segment, figure out its center and take half its length as the
			 * radius of a circle that just contains the longer segment.  If
			 * either of the endpoints of the shorter segment are in this length,
			 * then we have an intersection.  If we had used the shorter segment
			 * instead, then it's possible that the short segment could be
			 * completely enclosed in the longer segment, and this test would have
			 * failed.
			 */
			float centerx, centery, length1_squared, length2_squared;

			length1_squared = distance_squared(ax,ay,bx,by);
			length2_squared = distance_squared(sx,sy,tx,ty);
			if (length1_squared<length2_squared)
			{
				/* segment 2 is the longer one.  Use that */
				centerx = (sx+tx)/2.0f;
				centery = (sy+ty)/2.0f;
				return (collide_circle_point(centerx,centery,sqrtf(length2_squared),ax,ay)
				        || collide_circle_point(centerx,centery,sqrtf(length2_squared),bx,by));
			}
			else
			{
				/* segment 1 is the longer one.  Use that */
				centerx = (ax+bx)/2.0f;
				centery = (ay+by)/2.0f;
				return (collide_circle_point(centerx,centery,sqrtf(length1_squared),sx,sy)
				        || collide_circle_point(centerx,centery,sqrtf(length1_squared),tx,ty));
			}
		}
		else
			return false;
	}
	else
	{
		t1 /= den;
		t2 /= den;

		if (0<=t1 && t1<=1 && 0<=t2 && t2<=1)
			return true;
		else
			return false;
	}
}

bool collide_point_polygon(float sx, float sy, float vertices[][2], int numvertices)
{
	/*
	 * This algorithm can fail if you specify an endpoint of a vertex as the
	 * point.  I.e., a square with s=2 centered at (0,0) and testing (1,1) will
	 * show no collision since it's part of two different segments.  This should
	 * not happen in practice, one hopes.  And anyway, once the shot is inside,
	 * this test should start working again.
	 */
	int i, count;
	float outsidex, outsidey;

	// this is kind of arbitrary
	outsidex = sx+53;
	outsidey = sy+50;
	count=0;
	for (i=0;i<numvertices-1;i++)
	{
		if (collide_segment_segment(sx,sy,outsidex,outsidey,vertices[i][0],vertices[i][1],vertices[i+1][0],vertices[i+1][1]))
		{
			count++;
		}
	}
	if (collide_segment_segment(sx,sy,outsidex,outsidey,vertices[numvertices-1][0],vertices[numvertices-1][1],vertices[0][0],vertices[0][1]))
	{
		count++;
	}
	//return (count % 2);
	return (count & 0x0001);
}

bool collide_segment_polygon(float x1, float y1, float x2, float y2, float vert[][2], int numvert)
{
	int i;
	
	if (collide_point_polygon(x1,y1,vert,numvert)
	    || collide_point_polygon(x2,y2,vert,numvert))
	{
		return true;
	}
	for (i=0;i<(numvert-1);i++)
	{
		if (collide_segment_segment(x1,y1,x2,y2,vert[i][0],vert[i][1], vert[i+1][0],vert[i+1][1]))
			return true;
	}
	return (collide_segment_segment(x1,y1,x2,y2,vert[numvert-1][0],vert[numvert-1][1], vert[0][0],vert[0][1]));
}

bool collide_polygon_polygon(float va[][2], int numva, float vb[][2], int numvb)
{
	int i=0;

	while (i<(numva-1))
	{
		if (collide_segment_polygon(va[i][0],va[i][1],va[i+1][0],va[i+1][1],vb,numvb))
		{
			return true;
		}
		i++;
	}
	return (collide_segment_polygon(va[numva-1][0],va[numva-1][1],va[0][0],va[0][1],vb,numvb));
}

bool collide_polygon_circle(float va[][2], int numva, float centerx, float centery, float radius)
{
	int i;
	for (i=0;i<numva-1;i++)
	{
		if (collide_segment_circle(va[i][0],va[i][1],va[i+1][0],va[i+1][1], centerx, centery, radius))
		{
			return true;
		}
	}
	return (collide_segment_circle(va[numva-1][0],va[numva-1][1],va[0][0],va[0][1], centerx, centery, radius));
}
