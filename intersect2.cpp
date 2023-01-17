// Connor Flenniken
// CS 3352, Fall 2021
// Any-Segments-Intersect Honors Assignment

#include <iostream>
#include <algorithm>
#include <vector>
#include "bst.h"

using namespace std;


/**
 The running time for my Any-Segments-Intersect algorithm itself is O(n^2). The running time can be found for 
 a multitude of different reasons. First, I am using an exchange sort, which has a running time of o(n^2).
 I chose exchange sort as the sort is easy to understand and implement and I soon found out that my running time
 outside of the sort was n^2, so this sort did not fully affect the running time by increasing the worst case running time.
 
 Because of the way that I created and stored my segments, I had to create two functions to find the left and right endpoints
 in different situations. Both of these functions were O(n), but are in a for loop, which adds another O(n^2) to the running time.
 Next, since I am using a binary search tree, my tree insert and tree delete functions are O(n) as well, which are also in the same
 for loop. this adds one more O(n^2). All other functions and variables add up to O(1), so the overall running time is O(n^2)
**/

/**
 I received help from Shelby Cagle on a few time and space complexities that I did not fully understand.
 
 Thank you for your help as well on questions that I had! I especially was helped on how to get the key 
 in my binary search tree by using the y value and the insert functions that have to 
 compare the key to the y value of other lines at the current line segment's y value. 
 Thank you again for your help!
**/

//**********************************************************
//*                       DIRECTION                        *
//* Returns the cross product of the direction of one line * 
//*     relative to another line and a starting origin     *
//**********************************************************
// WCRT: O(1)
// Space Complexity: O(1)
int direction(pair<int, int> pi, pair<int, int> pj, pair<int, int> pk)
{
	return (((pk.first - pi.first) * (pj.second - pi.second)) - ((pj.first - pi.first) * (pk.second - pi.second)));
}

//************************************************************************
//*                            ON-SEGMENT                                *
//*          Returns true if a point colinear with the segment           *
//*  lies on the segment, false if the point does not lie on the segment *
//************************************************************************
// WCRT: O(1)
// Space Complexity: O(1)
bool onSegment(pair<int, int> pi, pair<int, int> pj, pair<int, int> pk)
{
	if (((min(pi.first, pj.first) <= pk.first) && (pk.first <= max(pi.first, pj.first)))
		&& ((min(pi.second, pj.second) <= pk.second) && (pk.second <= max(pi.second, pj.second))))
		return true;
	return false;
}

//*****************************************
//*         SEGMENTS-INTERSECT            *
//* Takes in two segments and determines  *
//* whether the lines will ever intersect *
//*****************************************
// WCRT: O(1)
// Space Complexity: O(1)
bool segmentsIntersect(pair<int , int> p1, pair<int , int> p2, pair<int , int> p3, pair<int , int> p4)
{
	int d1 = direction(p3, p4, p1);
	int d2 = direction(p3, p4, p2);
	int d3 = direction(p1, p2, p3);
	int d4 = direction(p1, p2, p4);
	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
		return true;
	else if (d1 == 0 && onSegment(p3, p4, p1))
		return true;
	else if (d2 == 0 && onSegment(p3, p4, p2))
		return true;
	else if (d3 == 0 && onSegment(p1, p2, p3))
		return true;
	else if (d4 == 0 && onSegment(p1, p2, p4))
		return true;
	return false;
}

//**************************************************
//*                  SORT-POINTS                   *
//*  Sorts every endpoint in the list of segments  *
//* by placing smaller x values first and breaking *
//*     ties by placing smaller y values first     *
//**************************************************
// WCRT: O(n^2)
// Space Complexity: O(n)
void sortPoints(vector<point> &points)
{
	for (uint i = 0; i < points.size() - 1; i += 1)
	{
		for (uint j = i + 1; j < points.size(); j += 1)
		{
			if (points[i].point.first > points[j].point.first)
			{
				swap(points[i], points[j]);
			}
			else if (points[i].point.first == points[j].point.first)
			{
				if (points[i].point.second > points[j].point.second)
				{
					swap(points[i], points[j]);
				}
				else if ((points[i].point.second == points[j].point.second) && 
						!points[i].isLeftEndpoint && points[j].isLeftEndpoint)
					swap(points[i], points[j]);
			}
		}
	}
}

//*************************************************
//*               findRightEndpoint               *
//*     Finds the right endpoint of a segment     *
//* given the left endpoint of a specific segment *
//*************************************************
// WCRT: O(n)
// Space Complexity: O(n)
point findRightEndpoint(vector<point> &points, point pi)
{
	for (uint i = 0; i < points.size(); i += 1)
	{
		if (!points[i].isLeftEndpoint && (pi.segmentNum == points[i].segmentNum))
		{
			return points[i];
		}
	}
	
	return pi;
}

//**************************************************
//*                findLeftEndpoint                *
//*      Finds the left endpoint of a segment      *
//* given the right endpoint of a specific segment *
//**************************************************
// WCRT: O(n)
// Space Complexity: O(n)
point findLeftEndpoint(vector<point> &points, point pi)
{
	for (uint i = 0; i < points.size(); i += 1)
	{
		if (points[i].isLeftEndpoint && (pi.segmentNum == points[i].segmentNum))
		{
			return points[i];
		}
	}
	
	return pi;
}

//*****************************************************************
//*                     ANY-SEGMENTS-INTERSECT                    *
//*      Checks if any two line segments intersect by checking    *
//*       whether there is a line above or below the current      *
//* line segment in a constantly sweeping line from left to right *
//*****************************************************************
// WCRT: O(n^2)
// Space Complexity: O(n)
bool anysegmentsIntersect(vector<point> points)
{	
	// O(1)
	BinarySearchTree bst;
	
	// O(n^2)
	sortPoints(points);
	
	// O(n)
	for (uint i = 0; i < points.size(); i += 1)
	{
		// O(1)
		if (points[i].isLeftEndpoint)
		{
			// O(n)
			point rightEndpoint = findRightEndpoint(points, points[i]);
			
			// O(1)
			pair<point, point> segment;
			segment.first = points[i];
			segment.second = rightEndpoint;
			BSTNode *node = new BSTNode(segment);
			
			// O(n)
			bst.treeInsert(node);
			
			// O(n)
			if ((bst.treeSuccessor(node) != NULL 
			&& segmentsIntersect(segment.first.point, segment.second.point, bst.treeSuccessor(node)->p1.point, bst.treeSuccessor(node)->p2.point)) ||
			(bst.treePredecessor(node) != NULL 
			&& segmentsIntersect(segment.first.point, segment.second.point, bst.treePredecessor(node)->p1.point, bst.treePredecessor(node)->p2.point)))
				return true;
		}
		else
		{
			// O(n)
			point leftEndpoint = findLeftEndpoint(points, points[i]);
			
			// O(1)
			pair<point, point> segment;
			segment.first = leftEndpoint;
			segment.second = points[i];
			BSTNode *node = new BSTNode(segment);
			
			// O(n)
			if (bst.treePredecessor(node) != NULL && bst.treeSuccessor(node) != NULL
			&& segmentsIntersect(bst.treePredecessor(node)->p1.point, bst.treePredecessor(node)->p2.point, 
			bst.treeSuccessor(node)->p1.point, bst.treeSuccessor(node)->p2.point))
				return true;
				
			// O(n)
			bst.treeDelete(node);
		}
	}
	
	
	return false;
}

//***************************************************
//*                       MAIN                      *
//*        Takes in the input and inserts the       *
//*       variables into the correct locations      *
//* Prints the answer to the Any-Segments-Intersect *
//***************************************************
// WCRT: O(n^2)
// Space Complexity: O(n)
int main()
{
	
	int numLines;
   
	while(cin >> numLines)
	{
		vector<point> points(numLines * 2);
	
		int lineNum = 0;
		for (int i = 0; i < numLines * 2; i += 2)
		{		
			cin >> points[i].point.first;
			cin >> points[i].point.second;
			cin >> points[i + 1].point.first;
			cin >> points[i + 1].point.second;
			points[i].segmentNum = lineNum;
			points[i + 1].segmentNum = lineNum;
			if (points[i].point.first < points[i + 1].point.first || 
			points[i].point.first == points[i + 1].point.first)
			{
				points[i].isLeftEndpoint = true;
				points[i + 1].isLeftEndpoint = false;
			}
			else
			{
				points[i].isLeftEndpoint = false;
				points[i + 1].isLeftEndpoint = true;
			}
			lineNum += 1;
		}
	
		if (anysegmentsIntersect(points))
			cout << "true\n";
		else
			cout << "false\n";
	}
	
	return 0;
}