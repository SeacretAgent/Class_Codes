// YOUR NAME: Connor Flenniken
//
// CS 4318, spring 2022
// Agent Challenge 3: Off-road rally

#include "orr.h"

namespace
{

}

vector<drivingDirection> orrAgentP(TerrainMap &map)
{
   
   // Used later to set the direction used in the for loop to check each move
   drivingDirection moveToTry;
   
   // Vector that holds the final route passed to the system
   vector<drivingDirection> route;

   // Integers that store the location of the start and finish hex;
   const int start = map.getStartHex();
   const int finish = map.getFinishHex();
   
   //************
   //* STAGE 1: *
   //*   h(n)   *
   //************
   // h(n) calculated using the manhattan distance from each hex to the finish hex.
   
   // Integer that stores n, the number in the matrix n X n;
   const int mapSize = map.getSize();

   // Sets the current distance and finish numbers to 0
   int distance = 0, finishRow = 0, finishColumn = 0;
   
   // Creates a vector to keep track of each distance to the finish goal
   vector<int> distances(mapSize * mapSize);
   
   // Make sure that you are not dividing by 0, 
   // Otherwise, keep distance of distances[0] at 0
   if (finish != 0)
   {
	   // Row # calculated by dividing the current hex by the map size and taking the remainder.
	   // Column # is calculated by dividing the current hex by the map size and rounding down.
	   finishRow = finish % mapSize;
	   finishColumn = static_cast<int>(floor(finish / mapSize));
	   
	   // Distance is calculated by adding both distances together.
	   // Multiply distance by 2 (since you cannot have anything less than 2).
	   // Since both row and column for distances[0] is 0, you do not need to subtract them.
	   distance = finishRow + finishColumn;
	   distance *= 2;
   }
   
   // Set distances[0] to its correct #
   distances.at(0) = distance;
   
   // Repeat for the rest of the numbers.
   for (int i = 1; i < (mapSize * mapSize); i += 1)
   {
	   // Row # calculated by dividing the current hex by the map size and taking the remainder.
	   // Column # is calculated by dividing the current hex by the map size and rounding down.
	   int row = i % mapSize;
	   int column = static_cast<int>(floor(i / mapSize));
	   
	   
	   // Distance is calculated by adding both distances together.
	   // Since distances[0] is NOT 0, you need to subtract the current row/column
	   //  from the finish row to get the correct #. Absolute value because the # should not be negative.
	   // Multiply distance by 2 (since you cannot have anything less than 2).
	   distance = abs(finishRow - row) + abs(finishColumn - column);
	   distance *= 2;
	   
	   // Set distances[i] to its correct #
	   distances.at(i) = distance;
   }
   
   //******************
   //*    STEP 2:     *
   //* Route Creation *
   //******************
   
   // Clear the route so that you have a blank slate
   route.clear();

   // Set the current location to the start
   // Set the bestDistance to the maximum int to be replaced
   // Choose a random direction for the bestOption, will most likely change
   int next = start;
   int bestDistance = INT_MAX;
   drivingDirection bestOption = driveW;
   
   // While you have not reached the finish line, keep searching
   while (next != finish)
   {
	   // Loop through all driving options
	   for (moveToTry = driveW; moveToTry <= driveE; moveToTry = static_cast<drivingDirection>(moveToTry + 1))
	   {
		   
		  // If the next move is the finish line, move there and return the route.
	      if (map.getNeighborHex(next, moveToTry) == finish)
          {
		      route.push_back(moveToTry);
		      return route;
	      }
		  
		  // Else, check the current direction to see if the distance from the goal is less than the current distance.
		  if (distances.at(map.getNeighborHex(next, moveToTry)) < bestDistance)
		  {
			  // Set the best distance to the hex you will move to and set the best driving option to that direction.
			  bestDistance = distances.at(map.getNeighborHex(next, moveToTry));
			  bestOption = moveToTry;
		  }
	   }
	   
	   // Add the new best driving direction and set the next location to the location in that direction.
	   route.push_back(bestOption);
	   next = map.getNeighborHex(next, bestOption);
   }

   // If, for some reason, you end up out here, return whatever route you have so far.
   return route;
}

/*
	In this program, I went with a Greedy Best First Search (GBFS) approach. I have not successfully been able to implement a UCS or A* search.
	For the heuristic algorithm, I went with an algorithm that determines the distance from each node and the goal node. The algorithm calculates
	the manhattan distance of the node to the goal node. I made sure to multiply this value by 2 as the lowest number you can have in this state.
	
	For the manhattan distance, I was unsure of how to calculate the manhattan distances. I found out some information from an online source
	on how to calculate the manhattan distance given a one dimensional array instead of 2 dimensional x and y coordinates.
	https://stackoverflow.com/questions/66616376/how-to-find-manhattan-distance-in-one-dimensional-array
	I understood on how to get the column nuimber, but I didn't think about using the % remainder funciton to find the row.
	Once I was able to find the row amount, I was able to calculate the manhattan distance correctly.
	
	If I have time to look at this problem before sunday, then I will continue to try to figure out how to implement UCS.
	Once I get UCS, hopefully I can figure out how to combine these two agents, which should hopefully be semi easy to implement.
*/