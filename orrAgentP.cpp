// YOUR NAME: Connor Flenniken
// CS 4318, spring 2022
// Agent Challenge 3: Off-road rally

#include "orr.h"
#include <cmath>

vector<drivingDirection> orrAgentP(TerrainMap &map)
{
   
   // Keeps track of the number of hexes in each board
   int numHex = map.getSize() * map.getSize();
   
   // Keeps track of the start and finish hexe #'s in each board
   const int start = map.getStartHex();
   const int finish = map.getFinishHex();
   
   
   //************
   //* STAGE 1: *
   //*   h(n)   *
   //************
   // h(n) calculated using the manhattan distance from each hex to the finish hex.

   // Sets the current distance to 0
   int distance = 0;
   
   // Creates a vector to keep track of each distance to the finish goal
   vector<int> distances(numHex);
   
   // Sets the correct row and column # of the finish hex
   int finishRow = finish % map.getSize();
   int finishColumn = static_cast<int>(finish / map.getSize());
   
   // Find the distance from the current hex to the finish hex (h(n))
   for (int i = 0; i < numHex; i += 1)
   {
	   // Row # calculated by dividing the current hex by the map size and taking the remainder.
	   // Column # is calculated by dividing the current hex by the map size and rounding down.
	   int row = i % map.getSize();
	   int column = static_cast<int>(i / map.getSize());
	   
	   // Distance is calculated by adding both distances together.
	   // You need to subtract the current row/column
	   // from the finish row to get the correct #. Absolute value because the # should not be negative.
	   // Multiply distance by 2 (since you cannot have anything less than 2).
	   distance = abs(finishRow - row) + abs(finishColumn - column);
	   distance *= 2;
	   
	   // Check if the hex is any distance diagonally from the hex in the K direction
	   while (column > finishColumn && row < finishRow)
	   {
		   // Reduce distance by 2 until either the row is above the finish
		   // or the column is to the left of the finish
		   distance -= 2;
		   row += 1;
		   column -= 1;
	   }
	   
	   // Check if the hex is any distance diagonally from the hex in the X direction
	   while (column < finishColumn && row > finishRow)
	   {
		   // Reduce distance by 2 until either the row is below the finish
		   // or the column is to the right of the finish
		   distance -= 2;
		   row -= 1;
		   column += 1;
	   }
	   
	   // Set distances[i] to its correct #
	   distances.at(i) = distance;
   }
   
   //**********************
   //*      STAGE 2:      *
   //* f(n) = h(n) + g(n) *
   //**********************
   // Calculate the cost of each move, only when needing to,
   // and add the heuristic to determine which item to take
   // off the frontier. Use the frontier to find the next
   // direction to add to the path.
   
   
   // A drivingDirection to keep track of which direction you move in.
   drivingDirection moveToTry;
   
   
   // This vector keeps track of the best path found so far from the start
   // to a given hex. It is the size of the # of hexes and can store each
   // of their path values.
   vector<vector<drivingDirection>> bestPaths(numHex);
   // This vector keeps track of the current cost (driving time) of each path.
   // It is essentially the g(n) of the A* search.
   // Set to INT_MAX so that practically any cost is less than the start cost.
   vector<int> cost(numHex, INT_MAX);
   
   // The vector newHexes keeps track of whether the hex
   // has ever been on the frontier. If it is a new hex that has not
   // been on the frontier before, it is set to false to signify that
   // it is now on the frontier.
   vector<bool> newHexes(numHex, true);
   // The vector visitedHexes keeps track of whether the hex
   // has been taken off the frontier. If it has, it is set to true.
   // This is used to see if the cost to that path has already been found.
   // If it has, the driving time does not need to be calculated.
   vector<bool> visitedHexes(numHex, false);
   
   
   // The vector frontier is the frontier of the A* search.
   // It keeps track of which hexes can be searched from next in the search.
   vector<int> frontier;
   // Clear the frontier just to be sure there is nothing in it.
   frontier.clear();
   
   // Add the start hex to the frontier, set its cost to 0.
   // Also set its newHexes value to false, as it is not new any more.
   frontier.push_back(start);
   cost.at(start) = 0;
   newHexes.at(start) = false;
   
   
   // Keep searching unless there is no more hexes to search from.
   while (!frontier.empty())
   {
	   
	   // This for loop finds the hex with the lowest f(n) value.
	   // f(n) = h(n) + g(n).
	   // In order to calculate this, I take the cost for each hex on the frontier
	   // and add the heuristic value calculated earlier for the same hex.
	   //
	   // As stated in you office hours, it is best if I explain the difference between
	   // the distances vector and the frontier vector and i.
	   // The frontier.at(i) returns the number of the hex in succession in the frontier.
	   // The cost then takes this value, which is a hex number, and uses this value as
	   // an index number. This is because the frontier carries only the current hexes
	   // that are being looked at, but the cost contains every hex index in succession.
	   int temp = 0;
	   for (uint i = 1; i < frontier.size(); i += 1)
	   {
		   if (cost.at(frontier.at(i)) + distances.at(frontier.at(i)) < 
		       cost.at(frontier.at(temp)) + distances.at(frontier.at(temp)))
		   {
			   temp = i;
		   }
	   }
	   
	   // Set the currentHex as the hex with the lowest f(n) value.
	   int currentHex = frontier.at(temp);
	   
	   // Remove this current hex from the frontier.
	   frontier.erase(frontier.begin() + temp);
	   
	   // If the current hex is removed from the frontier, return the best path.
	   // You are done.
	   if (currentHex == finish)
		   return bestPaths.at(finish);
	   
	   // Loop through every direction you can go to.
	   for (moveToTry = driveW; moveToTry <= driveE; moveToTry = static_cast<drivingDirection>(moveToTry + 1))
	   {
		   // Set the next hex as the hex if the direction you are moving from.
		   int nextHex = map.getNeighborHex(currentHex, moveToTry);   
		
		
		   // If the hex has never been on the frontier, add the hex
		   // to the frontier and set its newHexes value to false;
		   if (newHexes.at(nextHex))
		   {
			   frontier.push_back(nextHex);
			   newHexes.at(nextHex) = false;
		   }
		   
		   // Set the currentMoveTime to 0
		   int currentMoveTime = 0;
		   
		   // If you have not visited the nex hex (aka not looked in all directions from that hex)
		   // and you are not travelling back to the hex you are currently on,
		   // then calculate the new move time and look at the map.
		   if (!visitedHexes.at(nextHex) && nextHex != currentHex)
			   currentMoveTime = map.getMoveTime(currentHex, moveToTry);
		   
		   // If you are not travelling back to the hex you are currently on and
		   // the cost of the current path plus the new move time is less than the
		   // cost of the new hex's current best time, this is the direction you
		   // should add to the next hex's path.
		   if (nextHex != currentHex && cost.at(currentHex) + currentMoveTime < cost.at(nextHex))
		   {
			   
			   // Set the nextHex's path to the current path you have
			   bestPaths.at(nextHex) = bestPaths.at(currentHex);
			   // Add the new direction to get to the nextHex
			   bestPaths.at(nextHex).push_back(moveToTry);
			   
			   // Set the new cost at the nextHex as the currentHex's time plus
			   // the new currentMoveTime
			   cost.at(nextHex) = cost.at(currentHex) + currentMoveTime;
		   }
	   }
	   
	   // After you have explored from the current hex,
	   // set it as being visited.
	   visitedHexes.at(currentHex) = true;
   }
   
   // If, for some reason, you do not find the end at all,
   // return whatever is in the best path to the finish hex.
   return bestPaths.at(finish);
}


/*

	In this challenge, I have successfully implemented an A* search algorithm
	that can reliably find its way to the goal for each game board.
	I have also double checked that the program compiles, so hopefully there
	should be no problems.
	
	h(n):
	For the heuristic algorithm (h(n)), I stayed with the manhattan method that
	seems to work well for my program. In earlier iterations, I was not accounting
	for the diagonal moves K and X in my manhattan calculations. This was causing
	my h(n) to not factor in quicker moves diagonally northwest and southeast.
	However, I was able to find a solution. 
	
	Now, my algorithm checks whether it is in the same row and column 
	as the solution and moves accordingly in the correct direction, 
	subtracting 2 from the distance each time. This pushes the distance to the 
	nearest row or column of the finish row/column and finds the distance from there.
	This accounts for differences in the manhattan distance in the diagonal directions.
	
	
	g(n):
	To determine the g(n) for each hex, my algorithm just finds the cost of the
	cheapest path to that node. To do this, my code takes the cost of the current
	path you are on and adds the cost of the next path. If this cost is less than
	the cost of the hex you are trying to drive to, then the new path direction is
	added onto the end of the new path. To cut down on map looks, my code also checks
	to see if you have already checked the path from the new hex. If you have, then
	you do not add the current drive time.
	
	
	f(n) = g(n) + h(n):
	To determine which hex to take off the frontier, my code adds both the heuristic
	value and the cost value together. This is in my for loop at the beginning of the
	frontier while loop. When the cost of the g(n) + h(n) is the least number in the
	entire frontier, the algorithm chooses that hex as the next hex and continues the search.
	
	
	HELP:
	I received some helpful tips from a few different sources for this agent challenge.
	Firstly, I received some help from both Devoux (Nemo I believe) and Joel, and you during office hours.
	
	Devoux: Devoux helped me to make my code a little bit more understandable and discussed with me options
	to improve my heuristic function. When I went to him, my heuristic was not functioning properly. 
	He found a temporary solution that involved removing part of the manhattan heuristic. 
	This allowed me to have my first working heuristic and A* algorithm.
	In return, I helped him in procuring his first UCS algorithm by helping him find a way to store his
	best paths and times and a few general tips on how to create a UCS in a similar way to how I made mine.
	
	Joel: When I was in one of your office hours, Joel found a mistake in my code that was causing my
	output to produce incorrect results and loop in certain situations. After this fix, my code ran
	smoothly and I was able to work on improving my heuristic function.
	
	Dr. Legrand: In multiple office hours, I was able to learn the steps to create my first UCS and improve
	my heuristic values. In particular, the help received on how to reduce map looks in the heuristic and in
	the actual map look section helped to reduce my looks at the map significantly. The most major drop
	in map looks came by implementing a system that catches if you have been to a certain hex in the past.
	This cut my map looks in about half. Thank you for your help in debugging the code.
	
	ONLINE SOURCES:
	I had a few online sources, some which I ended up using, and a few which I didn't, but still proved
	beneficial in learning how to create my heuristic.
	
	https://stackoverflow.com/questions/5084801/manhattan-distance-between-tiles-in-a-hexagonal-grid
	This link is what helped me to learn how to implement manhattan distance. It also explained how
	you would implement a hex version of manhattan distance which I was unfortunately not able to
	figure out, but I was able to find a solution.
	
	https://stackoverflow.com/questions/11821899/how-to-get-row-and-column-from-index
	I needed to separate my hexes by row and column for my heuristic. To do this, I found how to
	use the modulo operator for my columns, which I did not think about at the time of writing my
	formula out. This article helped me to learn how to use this in my program.

*/