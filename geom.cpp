#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <stack>
#include <vector>

using namespace std; 

/* **************************************** */
/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
 */
int signed_area2D(point2d a, point2d b, point2d c) {
  int a_x = b.x - a.x;
  int a_y = b.y - a.y;
  int b_x = c.x - a.x;
  int b_y = c.y - a.y;
  int area = ((a_x * b_y) - (b_x * a_y));
  return area; 
}


/* **************************************** */
/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2d p, point2d q, point2d r) {
  return ~signed_area2D(p, q, r); 
}


/* **************************************** */
/* return 1 if c is  strictly left of ab; 0 otherwise */
int left_strictly(point2d a, point2d b, point2d c) {
  return (signed_area2D(a, b, c) > 0); 
}


/* return 1 if c is left of ab or on ab; 0 otherwise */
int left_on(point2d a, point2d b, point2d c) {
  return (left_strictly(a, b, c) | collinear(a, b, c)); 
}


point2d p0;
bool angle_comparator(point2d p1, point2d p2) {
  // printf("DOUBLE CHECK (%d, %d) \n", p0.x, p0.y);
  float angle1 = atan2((p1.y - p0.y), (p1.x - p0.x));
  float angle2 = atan2((p2.y - p0.y), (p2.x - p0.x));

  if (angle1 == angle2) {
    float distance1 = (float)((p1.x - p0.x) + (p1.y - p0.y));
    float distance2 = (float)((p2.x - p0.x) + (p2.y - p0.y));
    return distance1 < distance2;
  }

  if (angle1 < 0) {
    angle1 += (float) M_PI; // add twice the range of arctan, which is π/2, to offset negative angles such that they are greater than the positive values
  }
  if (angle2 < 0) {
    angle2 += (float) M_PI;
  }

  return angle1 < angle2;
}


// compute the convex hull of pts, and store the points on the hull in hull
void graham_scan(vector<point2d>& pts, vector<point2d>& hull) {
  if (pts.size() < 2) {
    printf("ERROR: There must be at least two points\n");
    exit(1);
  }

  printf("hull2d (graham scan): start\n"); 
  hull.clear(); //should be empty, but clear it to be safe

  //find point with lowest y value in pts to start graham scan.
  int smallest = std::numeric_limits<int>::max();
  point2d smallest_y;
  for (int i = 0; i < pts.size(); i++) {
    if (pts[i].y < smallest) {
      smallest = pts[i].y;
      smallest_y = pts[i];
    }
    else if (pts[i].y == smallest) { // if there are multiple points with least y-coord, choose point with largest x-coord
      if (pts[i].x > smallest_y.x) {
        smallest_y = pts[i];
      }
    }
  }
  p0 = smallest_y;

  sort(pts.begin(), pts.end(), angle_comparator);

  //removes duplicates of p0 so as not to consider them in the hull
  int i = 1;
  while (pts[i].x == p0.x && pts[i].y == p0.y) { // while there are duplicates (so we don't have to iterate through the whole vector)
    pts.erase(pts.begin() + i);
  }

  stack<point2d> s;
  s.push(pts[0]);
  s.push(pts[1]);
  point2d pt_a, pt_b;
  for (int i = 2; i < pts.size(); i++) { // starting from element 3 (e.g. 2 if points are labeled p0, 1, 2, etc.)
    if (!(s.top().x == pts[i].x && s.top().y == pts[i].y)) {
      while (!left_strictly(pt_a, pt_b, pts[i]) && s.size() > 2) {
        s.pop();
        pt_b = s.top();
        s.pop();
        pt_a = s.top();
        s.push(pt_b);
      }
      pt_a = s.top();
      s.push(pts[i]); 
      pt_b = s.top();
    }
  }

  // populate the hull with the points from the stack
  while (!s.empty()) {
    hull.push_back(s.top());
    s.pop();
  }

  /* Prints the list of points from p0. */
  // printf("Here is the list of pts\n");
  // for (int i = 0; i < pts.size(); i++) {
  //   printf("(%d, %d), ", pts[i].x, pts[i].y);
  // }
  // printf("\n");
  
  /* Prints convex hull points (backwards). */
  // printf("convex hull points are: \n");
  // for (int i = 0; i < hull.size(); i++) {
  //   printf("(%d, %d), ", hull[i].x, hull[i].y);
  // }
  // printf("\n");
  
  printf("hull2d (graham scan): end\n"); 
  return; 
}
