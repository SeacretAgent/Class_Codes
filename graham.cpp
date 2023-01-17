// Connor Flenniken
// CS 3352, Fall 2021
// Graham Scan Honors Assignment

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <math.h>

using namespace std;

/**
 The running time for my GRAHAM-SCAN algorithm is O(n^2). This is because of both my sort and the popping from the stack.
 My sort uses one for loop, but erase is linear, which means the sort is O(n^2). My GRAHAM-SCAN also checks the polar angles
 of each point in the stack from each point in the vector. This means that the running time is O(n^2). Together, the running time
 is equal to O(n^2).
**/

/**
 I received help from both Jordan Wade and Cameron Ley on the Graham Scan. I thought I had tested every case, but one of their cases
 showed that my scan did not account for points on the same vertical line, as I was not checking if the polar angle was equal
 near the end of my code.
Here is where they helped in my code:
 Line 117: while (direction(secondTop(convexHull), convexHull.top(), points.at(i)) <= 0), I originally had < instead of <=.
**/

//************************************************************************
//*                       FIND-SHORTEST-DISTANCE                         *
//* Calculates the distance to two different points from an origin point *
//*         and returns the point that is closer to the origin.          *
//*  The point that is returned is subsequently deleted in Graham Scan.  *
//************************************************************************
// WCRT: O(1)
// Space: O(1)
int findShortestDistance(pair<int, int> origin, pair<int, int> p1, pair<int, int> p2)
{
	
	int distX1 = p1.first - origin.first;
	int distY1 = p1.second - origin.second;
	
	int distX2 = p2.first - origin.first;
	int distY2 = p2.second - origin.second;
	
	int distP1 = sqrt(pow(distX1, 2) + pow(distY1, 2));
	int distP2 = sqrt(pow(distX2, 2) + pow(distY2, 2));
	
	if (distP1 < distP2)
		return 0;
	else
		return 1;
}

//********************************************************
//*                      DIRECTION                       *
//* Finds the cross product to determine which direction *
//*   the line segment is compared to the origin point   *
//********************************************************
// WCRT: O(1)
// Space: O(1)
int direction(pair<int, int> pi, pair<int, int> pj, pair<int, int> pk)
{
	return (((pi.first - pk.first) * (pj.second - pk.second)) - ((pj.first - pk.first) * (pi.second - pk.second)));
}

//*************************************************************
//*                       SORT-POINTS                         *
//* sorts points by determining the polar angle of each point *
//*    and swaps each point if the polar angle is negative    *
//*      If the polar angle is the same for two points,       *
//*   then it deletes the point that is closer to the origin  *
//*************************************************************
// WCRT: O(n^2)
// Space: O(n)
void sortPoints(vector<pair<int, int> > &points)
{
	int polarAngle = 0;
	
	// Sort by direction
	for (uint i = 1; i < points.size() - 1; i += 1)
	{	
		polarAngle = direction(points[i], points[i + 1], points[0]);
		if (polarAngle < 0)
		{
			swap(points[i], points[i + 1]);
			
			if (i > 1)
			{
				i -= 2;
				continue;
			}
		}
		else if (polarAngle == 0)
		{
			
			// vector.erase = O(n)
			if (findShortestDistance(points[0], points[i], points[i + 1]) == 0)
				points.erase(points.begin() + i);
			else
				points.erase(points.begin() + i + 1);
		}
	}
}

//******************************************************
//*                    SECOND-TOP                      *
//* Determines the second value of the stack passed in *
//******************************************************
// WCRT: O(1)
// Space: O(n)
pair<int, int> secondTop(stack<pair<int, int> > &points)
{
	pair<int, int> tempPoint = points.top();
	points.pop();
	pair<int, int> secondPoint = points.top();
	points.push(tempPoint);
	return secondPoint;
}

//************************************************************************************
//*                                  GRAHAM-SCAN                                     *
//* Finds the convex hull of a set of points by starting at the lowest x and y point *
//*            and moving around counter-clockwise, making sure to choose            *
//*                  the first point in a counter-clockwise motion                   *
//************************************************************************************
// WCRT: O(n^2)
// Space: O(n)
stack<pair<int, int> > grahamScan(vector<pair<int, int> > &points)
{
	int lowestIndex = 0;
	
	// O(n)
	for (uint i = 1; i < points.size(); i += 1)
	{
		if (points[i].second < points[lowestIndex].second || (points[i].second == points[lowestIndex].second && points[i].first < points[lowestIndex].first))
			lowestIndex = i;
	}
	if (lowestIndex != 0)
		swap(points[0], points[lowestIndex]);
	
	// O(n^2)
	sortPoints(points);
	
	int m = points.size() - 1;
	stack<pair<int, int> > convexHull;
	if (m < 2)
		return convexHull;
	else
	{
		convexHull.push(points[0]);
		convexHull.push(points[1]);
		convexHull.push(points[2]);
		
		// O(n)
		for (int i = 3; i <= m; i += 1)
		{	
			// O(n)
			//direction is not negative, pop
			while (direction(secondTop(convexHull), convexHull.top(), points.at(i)) <= 0)
			{
				convexHull.pop();
			}
			
			convexHull.push(points.at(i));
		}
	}
	
	return convexHull;
}

//****************************************************************************
//*                               PRINT-HULL                                 *
//* Prints the stack in reverse order in order to print the points correctly *
//****************************************************************************
// WCRT: O(n)
// Space: O(n)
void printHull(stack<pair<int, int> > hull)
{
	if (hull.empty())
		return;
	
	pair<int, int> x = hull.top();
	
	hull.pop();
	
	printHull(hull);
	
	cout << x.first << " " << x.second << endl;
}

//**********************************************************************************
//*                                    MAIN                                        *
//* Takes in all input and assigns the input into the correct variables to be used *
//*           Calls grahamScan and checks if there is a convex hull.               *
//*   If there is, calls PRINT-HULL in order to print the hull in reverse order.   *
//**********************************************************************************
// WCRT: O(n^2)
// Space: O(n)
int main()
{
	
	int numLines;
	
	while(cin >> numLines)
	{
	
		vector<pair<int, int> > points(numLines);
	
		for (int i = 0; i < numLines; i += 1)
		{
			cin >> points[i].first;
			cin >> points[i].second;
		}
		
		stack<pair<int, int> > hull = grahamScan(points);
		if (hull.size() == 0)
			cout << "Convex hull is empty." << endl;
		else
		{
			cout << hull.size() << endl;
			printHull(hull);
		}
	}

	return 0;
}