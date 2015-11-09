#ifndef _COLLISION_H_
#define _COLLISION_H_

/*
 * collide_row_column
 * 1 = yes; r=reverse yes
 *
 *        |cir|point|poly|segment 
 * circle | 1 |  1  |  r |  r   
 * point  | r |     |  1 |       
 * poly   | 1 |  r  |  1 |  r    
 * segment| 1 |     |  1 |  1    
 */

bool collide_circle_circle(float centerx1, float centery1, float radius1, float centerx2, float centery2, float radius2);
bool collide_circle_point(float centerx, float centery, float radius, float pointx, float pointy);
bool collide_segment_circle(float x1, float y1, float x2, float y2, float cx, float cy, float r);
bool collide_segment_segment(float ax, float ay, float bx, float by, float sx, float sy, float tx, float ty);
bool collide_point_polygon(float sx, float sy, float vertices[][2], int numvertices);
bool collide_segment_polygon(float x1, float y1, float x2, float y2, float vert[][2], int numvert);

// Not really tested, but pretty simple
bool collide_polygon_polygon(float va[][2], int numva, float vb[][2], int numvb);
bool collide_polygon_circle(float va[][2], int numva, float centerx, float centery, float radius);
#endif /* _COLLISION_H_ */
