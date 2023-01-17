// YOUR NAME: Connor Flenniken
//
// CS 4318, spring 2022
// Agent Challenge 5: Chocolate Russian roulette

#include "crr.h"
#include <utility>

namespace
{
   ChocolateSquareLocation firstMove(const ChocolateBar);
   pair<int, ChocolateSquareLocation> MinValue(const ChocolateBar, int, int, int, int, int);
   
   
   // Function that finds the maximum value of a certain action and it's immediate path states
   pair<int, ChocolateSquareLocation> MaxValue(const ChocolateBar bar, int alpha, int beta, int depth, int utilityA, int utilityB)
   {
	   
	   // Column and row of the action taken
	   int biteColumn, biteRow;
	   // The limit on how far down the tree to look
	   int depthLimit = 5;
	   
	   
	   // This checks whether the game has ended and returns the value that
	   // of the player's actions
	   if (!bar.isAnyLeft())
		   return (pair<int, ChocolateSquareLocation>(utilityA, NULL));
	   
	   
	   // Set the bestScore to the lowest score to ensure anything else is better
	   int bestScore = INT_MIN;
	   
	   
	   // Sets a base return to send if no other option is better
	   pair<int, ChocolateSquareLocation> bestChoice(bestScore, ChocolateSquareLocation());
	   
	   if (depth > depthLimit)
		   return (pair<int, ChocolateSquareLocation>(0, NULL));
	   
	   // Make sure that you only look a certain amount of times down the tree
	   if (depth <= depthLimit)
	   {
		    // Loop through every possible action
			for (biteRow = bar.getHeight() - 1; biteRow >= 0; biteRow -= 1)
			{
				for (biteColumn = bar.getWidth() - 1; biteColumn >= 0; biteColumn -= 1)
				{
					ChocolateSquareLocation bite = ChocolateSquareLocation(biteColumn, biteRow);
					
					// If there is a square at the bite location,
					// continue down the tree
					if (bar.isSquareThere(bite))
					{
						// Temporary bar to allow us to bite into it
						ChocolateBar tempBar = bar;
						
						// This line has 2 functions:
						// 1: It takes a bite out of the bar at the chosen spot
						// 2: It adds the number of squares eaten to the utility
						// A higher # of squares eaten is a better utility score
						utilityA += tempBar.takeBite(bite);
					
						
						// This calls the MinValue function to determine what the other player would play
						pair<int, ChocolateSquareLocation> nextMove = MinValue(tempBar, alpha, beta, depth + 1, utilityA, utilityB);
					
						// If the other player's option is better than our current best option,
						if (nextMove.first > bestScore)
						{
							// Then set this path to the best move and score
							bestScore = nextMove.first;
							bestChoice.first = bestScore;
							bestChoice.second = bite;
						
							// If your new best score is higher than the highest best score (alpha),
							// then set it as the highest best score (alpha)
							if (bestScore > alpha)
								alpha = bestScore;
						}
					
						// If your best score is greater than or equal to the lowest score (beta)
						// Return that choice
						if (bestScore >= beta)
							return bestChoice;
					}
				}
			}
		}
	   
	   // After iterating for the best option, return whatever your best choice is
	   return bestChoice;
   }
   
   // Function that finds the minimum value of a certain action and it's immediate path states
   pair<int, ChocolateSquareLocation> MinValue(const ChocolateBar bar, int alpha, int beta, int depth, int utilityA, int utilityB)
   {
	   
	   // Column and row of the action taken
	   int biteColumn, biteRow;
	   // The limit on how far down the tree to look
	   int depthLimit = 5;
	   
	   
	   
	   // This checks whether the game has ended and returns the value that
	   // of the player's actions
	   if (!bar.isAnyLeft())
		   return (pair<int, ChocolateSquareLocation>(utilityB, NULL));
	   
	   
	   
	   // Set the bestScore to the highest score to ensure anything else is better
	   int bestScore = INT_MAX;
	   
	   
	   // Sets a base return to send if no other option is better
	   pair<int, ChocolateSquareLocation> bestChoice(bestScore, ChocolateSquareLocation());
	   
	   
	   if (depth > depthLimit)
		   return (pair<int, ChocolateSquareLocation>(0, NULL));
	   
	   // Make sure you only look a certain amount of times down the tree
	   if (depth <= depthLimit)
	   {
		   // Loop through every possible action
		   for (biteRow = bar.getHeight() - 1; biteRow >= 0; biteRow -= 1)
		   {
				for (biteColumn = bar.getWidth() - 1; biteColumn >= 0; biteColumn -= 1)
				{
					ChocolateSquareLocation bite = ChocolateSquareLocation(biteColumn, biteRow);
					
					// If there is a square at the bite location,
					// continue down the tree
					if (bar.isSquareThere(bite))
					{
						// Temporary bar to allow us to bite into it
						ChocolateBar tempBar = bar;
						
						// This line has 2 functions:
						// 1: It takes a bite out of the bar at the chosen spot
						// 2: It subtracts the number of squares eaten to the utility
						// A lower # of squares eaten is a better utility score
						utilityB -= tempBar.takeBite(bite);
						
						
						// This calls the MaxValue function to determine what the other player would play
						pair<int, ChocolateSquareLocation> nextMove = MaxValue(tempBar, alpha, beta, depth + 1, utilityA, utilityB);
						
						// If the other player's option is better than our current best option,
						if (nextMove.first < bestScore)
						{
							// Then set this path to the best move and score
							bestScore = nextMove.first;
							bestChoice.first = bestScore;
							bestChoice.second = bite;
							
							// If your new best score is lower than the lowest best score (beta),
							// then set it as the lowest best score (beta)
							if (bestScore < beta)
								beta = bestScore;
						}
						
						// If your best score is less than or equal to the highest score (alpha)
						// Return that choice
						if (bestScore <= alpha)
							return bestChoice;
					}
				}
		   }
	   }
	   
	   // After iterating for the best option, return whatever your best choice is
	   return bestChoice;
   }
   
   
   // This function determines if you can make the best first move,
   // even if you are not the first player
   ChocolateSquareLocation firstMove(const ChocolateBar bar)
   {
	   
	   // Look at columns first
	   // KEY:
	   // Chocolate: O
	   // Poison Chocolate: X
	   // Best Move Spot: B
	   // Points to check: *
	   switch(bar.getWidth())
	   {
		   case 3:
				switch(bar.getHeight())
				{																					      // 3X4  3X5  3X6  
					case 4:																				  // *    O*   *  
						if (bar.isSquareThere(1,2) && bar.isSquareThere(0,3) && bar.isSquareThere(2,1))   // OB   OOB  O  
							return ChocolateSquareLocation(1,2);										  // OO*  OOO  OB 
						break;																			  // XOO  OOO  OO*
					case 5:																				  //      XOO  OOO
						if (bar.isSquareThere(2,3) && bar.isSquareThere(1,4))							  //           XOO
							return ChocolateSquareLocation(2,3);
						break;
					case 6:
						if (bar.isSquareThere(1,3) && bar.isSquareThere(0,5) && bar.isSquareThere(2,2))
							return ChocolateSquareLocation(1,3);
						break;
					case 7:																				  // 3X7  3X8  3X9
						if (bar.isSquareThere(2,4) && bar.isSquareThere(1,6))							  // O*   *    O* 
							return ChocolateSquareLocation(2,4);										  // OO   O    OO 
						break;																			  // OOB  O    OOB
					case 8:																				  // OOO  OB   OOO
						if (bar.isSquareThere(1,4) && bar.isSquareThere(0,7) && bar.isSquareThere(2,3))   // OOO  OO*  OOO
							return ChocolateSquareLocation(1,4);										  // OOO  OOO  OOO
						break;																			  // XOO  OOO  OOO
					case 9:																				  // 	  XOO  OOO
						if (bar.isSquareThere(2,6) && bar.isSquareThere(1,8))							  //		   XOO
							return ChocolateSquareLocation(2,6);
						break;
					default:
						return ChocolateSquareLocation(0,0);
						break;
				}
				break;
			case 4:																						// 4X5   4X6   4X7
				switch(bar.getHeight())																    // O*    *     O*  
				{																						// OO    O     OO  
					case 5:																				// OOB   O     OO  
						if (bar.isSquareThere(2,2) && bar.isSquareThere(1,4) && bar.isSquareThere(3,2)) // OOO*  OB    OOB 
							return ChocolateSquareLocation(2,2);										// XOOO  OOO*  OOO*
						break;																			//       XOOO  OOOO
					case 6:																				//		 	   XOOO
						if (bar.isSquareThere(1,2) && bar.isSquareThere(0,5) && bar.isSquareThere(3,1))
							return ChocolateSquareLocation(1,2);
						break;
					case 7:
						if (bar.isSquareThere(2,3) && bar.isSquareThere(1,6) && bar.isSquareThere(3,2))
							return ChocolateSquareLocation(2,3);
						break;
					case 8:																				// 4X8   4X9
						if (bar.isSquareThere(1,3) && bar.isSquareThere(0,7) && bar.isSquareThere(3,2)) // *     OO* 
							return ChocolateSquareLocation(1,3);										// O     OOOB
						break;																			// O     OOOO
					case 9:																				// O     OOOO
						if (bar.isSquareThere(3,7) && bar.isSquareThere(2,8))							// OB    OOOO
							return ChocolateSquareLocation(3,7);										// OOO*  OOOO
						break;																			// OOOO  OOOO
					default:																			// XOOO  OOOO
						return ChocolateSquareLocation(0,0);											//       XOOO
						break;
				}
				break;
			case 5:																						// 5X6    5X7
				switch(bar.getHeight())																	// O*     O*   
				{																						// OO     OO   
					case 6:																				// OOB    OO   
						if (bar.isSquareThere(2,3) && bar.isSquareThere(1,5) && bar.isSquareThere(4,3)) // OOOO*  OO   
							return ChocolateSquareLocation(2,3);										// OOOOO  OB   
						break;																			// XOOOO  OOOO*
					case 7:																				// 		  XOOOO
						if (bar.isSquareThere(1,2) && bar.isSquareThere(0,6) && bar.isSquareThere(4,1))
							return ChocolateSquareLocation(1,2);
						break;																			// 5X8    5X9
					case 8:																				// OOO*   O*   
						if (bar.isSquareThere(4,6) && bar.isSquareThere(3,7))							// OOOOB  OO   
							return ChocolateSquareLocation(4,6);										// OOOOO  OO   
						break;																			// OOOOO  OOB  
					case 9:																				// OOOOO  OOOO*
						if (bar.isSquareThere(2,5) && bar.isSquareThere(1,8) && bar.isSquareThere(4,4)) // OOOOO  OOOOO
							return ChocolateSquareLocation(2,5);										// OOOOO  OOOOO
						break;																			// XOOOO  OOOOO
					default:																			//        XOOOO
						return ChocolateSquareLocation(0,0);
						break;
				}
				break;
			case 6:																						// 6X7     6X8
				switch(bar.getHeight())																	// OOO*    O*    
				{																						// OOOO    OO    
					case 7:																				// OOOOB   OO    
						if (bar.isSquareThere(4,4) && bar.isSquareThere(3,6) && bar.isSquareThere(5,3)) // OOOOO*  OO    
							return ChocolateSquareLocation(4,4);										// OOOOOO  OOB   
						break;																			// OOOOOO  OOOOO*
					case 8:																				// XOOOOO  OOOOOO
						if (bar.isSquareThere(2,3) && bar.isSquareThere(1,7) && bar.isSquareThere(5,2)) // 		   XOOOOO
							return ChocolateSquareLocation(2,3);
						break;																			// 6X9
					case 9:																				// *     
						if (bar.isSquareThere(1,2) && bar.isSquareThere(0,8) && bar.isSquareThere(5,1)) // O     
							return ChocolateSquareLocation(1,2);										// O     
						break;																			// O     
					default:																			// O     
						return ChocolateSquareLocation(0,0);											// O     
						break;																			// OB    
				}																						// OOOOO*
				break;																					// XOOOOO
			default:
				return ChocolateSquareLocation(0,0);
				break;
	   }
	   
	   // If no case works, send out 0,0
	   return ChocolateSquareLocation(10,10);
   }
   
   
   ChocolateSquareLocation checkPatterns(const ChocolateBar &bar)
   {
	   int x, y;
	   for (x = bar.getWidth(); x > 0; x -= 1)
	   {
		   for (y = bar.getHeight(); y > 0; y -= 1)
		   {
			   // Check for two rows (if there are no more tiles)
			   if (!bar.isSquareThere(0, 2))
			   {
				   // If there are two equal rows, then eat the top rightmost square
				   if (bar.isSquareThere(x, 1) && bar.isSquareThere(x, 0) && !bar.isSquareThere(x + 1, 0))
					   return ChocolateSquareLocation(x, 1);
				   // If the top row is shorter than the bottom, then eat the square on the bottom row
				   // that is 2 over from the rightmost square in the second row
				   if (bar.isSquareThere(x, 1) && bar.isSquareThere(x + 1, 0) && bar.isSquareThere(x + 2, 0) && !bar.isSquareThere(x + 1, 1))
					   return ChocolateSquareLocation(x + 2, 0);
			   }
			   
			   // Check for two columns (if there are no more tiles)
			   if (!bar.isSquareThere(2, 0))
			   {
				   // If there are two equal columns, then eat the highest right square
				   if (bar.isSquareThere(1, y) && bar.isSquareThere(0, y) && !bar.isSquareThere(0, y + 1))
					   return ChocolateSquareLocation(1, y);
				   // If the right column is shorter than the left column, then eat the square on the
				   // left column that is 2 higher than the highest square in the second column
				   if (bar.isSquareThere(1, y) && bar.isSquareThere(0, y + 1) && bar.isSquareThere(0, y + 2) && !bar.isSquareThere(1, y + 1))
					   return ChocolateSquareLocation(0, y + 2);
			   }
			   
			   // Check for the 0 row and 0 column (if there are no more tiles)
			   if (!bar.isSquareThere(1, 1))
			   {
				   // If the column is shorter than the row
				   if (bar.isSquareThere(x, 0) && !bar.isSquareThere(0, x))
				   {
					   // Once you find the highest column value, eat the square that is
					   // 1 further right than that column value on the bottom row
					   if (bar.isSquareThere(0, y))
							return ChocolateSquareLocation(y + 1, 0);
				   }
				   // If the row is shorter than the column
				   if (bar.isSquareThere(0, y) && !bar.isSquareThere(y, 0))
				   {
					   // Once you find the highest row value, eat the square that is
					   // 1 higher up than that row value on the left column
					   if (bar.isSquareThere(x, 0))
						   return ChocolateSquareLocation(0, x + 1);
				   }
			   }
			   
			   // Check to create two row
			   // If can create the two row pattern, then bite (0, 2) to create it
			   // This is only if the two rows are already in the correct pattern
			   if (bar.isSquareThere(x, 0) && !bar.isSquareThere(x + 1, 0) && !bar.isSquareThere(x, 1) 
				   && bar.isSquareThere(x - 1, 1) && bar.isSquareThere(0, 2))
				   return ChocolateSquareLocation(0, 2);
			   // Check to create two column
			   // If you can create the two column pattern, then bite (2, 0) to create it
			   // This is only if the two columns are already in the correct pattern
			   if (bar.isSquareThere(0, y) && !bar.isSquareThere(0, y + 1) && !bar.isSquareThere(1, y) 
				   && bar.isSquareThere(1, y - 1) && bar.isSquareThere(2, 0))
				   return ChocolateSquareLocation(2, 0);
				   
			   // Check to create row/column
			   // If the first row and column are equal in length and (1, 1) is still there,
			   // Then bite (1, 1) to create the row and column of equal length pattern
			   if (y <= bar.getWidth() && bar.isSquareThere(1, 1) && !bar.isSquareThere(y + 1, 0) && !bar.isSquareThere(0, y + 1)
				   && bar.isSquareThere(y, 0) && bar.isSquareThere(0, y))
				   return ChocolateSquareLocation(1, 1);
		   }
	   }
	   
	   // If nothing is found, then return something outside the bounds of the bars
	   return ChocolateSquareLocation(10, 10);
   }
}


ChocolateSquareLocation crrAgentP(const ChocolateBar &bar)
{
   
   // Checks if we can make the best first move, even if we are B
   ChocolateSquareLocation firstMove1 = firstMove(bar);
   if (firstMove1.getColumn() != 10 && firstMove1.getRow() != 10)
	   return firstMove1;
   
   // Checks if we can make a cold pattern for the opponent
   ChocolateSquareLocation pattern = checkPatterns(bar);
   if (pattern.getColumn() != 10 && pattern.getRow() != 10)
	   return pattern;
   
   // Sets the alpha and beta to the lowest and highest ints respectively
   int alpha = INT_MIN;
   int beta = INT_MAX;
   
   // Call MaxValue to find the best move
   return MaxValue(bar, alpha, beta, 0, 0, 0).second;
}

/*

   CODE ANALYSIS
   My code follows relatively closely to the minimax algorithm pseudocode given in the textbook
   with some minor differences. 
   
   Agent Code:
   The agent code itself is short and is mainly there to set up parts to prepare for the minimax search.
   The first section begins by seeing if the best move still exists correctly. After this, it then checks
   a lookup table to see if the agent can create a cold pattern for the other agent. Finally, if neither
   option is found, it then moves on to a minimax agent that determines the best move.
   
   firstMove:
   In this function, the code searches for the size of the bar and determines the best first move. It
   makes sure that it is able to do so and that conflicting states are not confused as a start state.
   If it cannot create the best first move, then it moves on to checking whether it can force a
   losing pattern to give to the opponent.
   
   checkPatterns:
   This function begins by looping through each location to see if it is valid. If it is, it then tries to
   determine if any pattern is able to be given to the opponent. First, it checks whether the board is set
   up to be able to create the winning patterns discussed in class. If it cannot be created, it just
   returns an impossible value and the agent moves on to minimax.
   
   MaxValue:
   This function searches down a tree by a certain number of levels to determine the maximum score that
   the agent can receive. If the agent wins, the utility value of their score is determined by how many
   chocolate pieces they ate. If an action has a higher amount of chocolate eaten, then the agent will
   choose that path. The search is depth limited and uses alpha beta pruning to make sure that the agent
   runs smoothly and quickly while still choosing relatively smart options. Added in this iteration is a
   fix that passes a 0 if the agent cannot determine the value because of the depth limit. This sped up
   the program significantly.
   
   MinValue:
   This function searches down a tree by a certain number of levels to determine the minimum score that
   the agent can receive. If this agent wins, the utility value of their score is determined by how many
   chocolate pieces they ate. If an action has a higher amount of chocolate eaten, then the agent will
   choose that path. The search is depth limited and uses alpha beta pruning to make sure that the agent
   runs smoothly and quickly while still choosing relatively smart options. Added in this iteration is a
   fix that passes a 0 if the agent cannot determine the value because of the depth limit. This sped up
   the program significantly.
   
   
   HELP GIVEN
   I talked and gave some advice to Devoux this evening 03/21/22. During this conversation, we were able to
   work on improving our minimax algorithms and implement alpha-beta pruning and a few other time saves.
   Among these was the simple lookup table that we added to make the first move if available. We were trying
   to find out how to calculate the best location, but were unable to find out. Instead we ran a long run
   with the time saves we already had and determined the best options from our minimax.
   
   On 04/11/22, I found out that I had missed the returning of 0 if a value could not be determined by the
   depth value. I shared this information with Devoux.
   
   HELP RECEIVED
   Along with the help explained above with Devoux and I working together, I also received help from
   Julio earlier in the day of 03/21/22 on how to fix my minimax algorithm. While his approach 
   was slightly different, I was able to understand the differences and gleam insights to my code. 
   His method of limiting the search also was beneficial to my code and process.
   
   
   Process and testing:
   During the process of making this algorithm, I had made a few mistakes in coding my minimax approach.
   I did not take into account the end result of a depth limited search, and as such, my agent was just
   returning the best choice so far, which had a base location of (0, 0). Once I found that issue out,
   I was able to create a true minimax agent that was limited by its depth. While this did not immediately
   increase the number of wins, it did speed up the code a bit. After this, I worked on implementing code
   that can account for the patterns discussed in class.
   
   For the pattern recognition, I had a lot of trouble with accounting for each element that I need to
   look at to determine if a pattern could be created. I spent most of my morning today working on figuring
   out exactly what tiles needed to be there and which ones did not. I then created agents that forced me to
   be in situations to create those patterns and kept testing until my agent successfully used those
   strategies. I believe it is functioning correctly now, so I hope I did not miss anything!
   
   
   This challenge was extremely fun to work out and analyze my algorithm. I had a lot of fun making it!
   Have a great day!
   
   Connor Flenniken
*/
