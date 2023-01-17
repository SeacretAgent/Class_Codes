// YOUR NAME: Connor Flenniken
// CS 4318-010
// Agent Challenge B: Matching Strategy Game

#include "msg.h"
#include <utility>

namespace
{
   
   int findUtility(const MSGBoard&, TileLocation, bool);
   
   
   TileLocation chooseBestOuter(const MSGBoard &board)
   {
	   
	   // Keeeps track of the best move for the first choice
	   TileLocation bestMove = TileLocation();
	   
	   // Trackers to keep track of the utility
	   int utility = 0;
	   int tempUtility = 0;
	   
	   
	   // Loop through each tile
	   int whichColumn, whichRow;
	   for (whichColumn = 0; whichColumn < boardSize; whichColumn += 1)
	   {
		   for (whichRow = 0; whichRow < boardSize; whichRow += 1)
		   {
			   // Check if the tile is an edge tile as
			   // those are the only legal move for the first choice
			   TileLocation move = TileLocation(whichColumn, whichRow);
			   if (move.isEdgeTile())
			   {
				   // Calculate the utility of each edge choice
				   MSGBoard tempBoard = board.makeMove(move);
				   tempUtility = findUtility(tempBoard, move, false);
				   
				   // If the calculated utility is better than
				   // the best utility found so far,
				   if (tempUtility > utility)
				   {
					   // Then choose that as the best move and utility
					   utility = tempUtility;
					   bestMove = move;
				   }
			   }
		   }
	   }
	   
	   // Return the location of whichever edge tile is the best choice
	   return TileLocation(bestMove);
   }
   
   
   // Calculates the best move that the agent can make based on limiting
   // winning options for the enemy agent and creating winning options
   // for the agent
   int findUtility(const MSGBoard &board, TileLocation prevMove, bool next)
   {
	   int whichColumn, whichRow;
	   
	   // Start at 16
	   int utility = 16;
	   
	   // If the agent lost, heavily lean against choosing that option
	   // If the agent wins, heavily lean towards choosing that option
	   // (this takes care of the blockingly win)
	   if (!board.isGameOver())
	   {
		   if (next)
			   utility -= 100;
		   else
			   utility += 100;
	   }
	   
	   // Loop through all valid moves left
	   for (whichColumn = 0; whichColumn < boardSize; whichColumn += 1)
	   {
		   for (whichRow = 0; whichRow < boardSize; whichRow += 1)
		   {
			   TileLocation move = TileLocation(whichColumn, whichRow);
			   if (board.isLegalMove(move))
			   {
				   // If this is the first loop of the utility,
				   if (!next)
				   {
					   // If the enemy has a tile orthogonally to the move you just made.
					   // sub 1 from utility as they can block a winning option for you.
					   if (whichColumn == prevMove.getColumn() || whichRow == prevMove.getRow())
							utility -= 1;
				   
					   // If the enemy has a tile squarely to the move you just made,
					   // sub 1 from utility as they can block a winning option for you.
					   else if (prevMove.getColumn() < 3 && prevMove.getRow() < 3 &&
					   (whichColumn == prevMove.getColumn() + 1 && whichRow == prevMove.getRow() + 1))
							utility -= 1;
					   else if (prevMove.getColumn() > 0 && prevMove.getRow() > 0 &&
					   (whichColumn == prevMove.getColumn() - 1 && whichRow == prevMove.getRow() - 1))
							utility -= 1;
					   else if (prevMove.getColumn() < 3 && prevMove.getRow() > 0 &&
					   (whichColumn == prevMove.getColumn() + 1 && whichRow == prevMove.getRow() - 1))
							utility -= 1;
					   else if (prevMove.getColumn() > 0 && prevMove.getRow() < 3 &&
					   (whichColumn == prevMove.getColumn() - 1 && whichRow == prevMove.getRow() + 1))
							utility -= 1;
					   
					   // If the enemy has a tile diagonally to the move you just made,
					   // sub 1 from utility as they can block a winning option for you.
					   else if (((prevMove.getColumn() == 0 && prevMove.getRow() == 0) || 
					   (prevMove.getColumn() == 1 && prevMove.getRow() == 1) || (prevMove.getColumn() == 2 && prevMove.getRow() == 2) ||
					   (prevMove.getColumn() == 3 && prevMove.getRow() == 3)) && whichColumn == whichRow)
							utility -= 1;
					   else if (((prevMove.getColumn() == 0 && prevMove.getRow() == 3) ||
					   (prevMove.getColumn() == 1 && prevMove.getRow() == 2) || (prevMove.getColumn() == 2 && prevMove.getRow() == 1) ||
					   (prevMove.getColumn() == 3 && prevMove.getRow() == 0)) && ((whichColumn == 0 && whichRow == 3) ||
					   (whichColumn == 1 && whichRow == 2) || (whichColumn == 2 && whichRow == 1) || (whichColumn == 3 && whichRow == 0)))
							utility -= 1;
					   
					   // Redo the utility of the move you would make on the next round
					   MSGBoard tempBoard = board.makeMove(move); 
					   return findUtility(tempBoard, prevMove, true);
				   }
				   
				   // This section is when you choose a legal move two rounds in the future.
				   
				   // If the agent has a tile orthogonally to the move you just made,
				   // sub 1 from utility as they can block a winning option for you.
				   if (whichColumn == prevMove.getColumn() || whichRow == prevMove.getRow())
					   utility += 1;
				   
				   // If the agent has a tile squarely to the move you just made,
				   // sub 1 from utility as they can block a winning option for you.
				   else if (prevMove.getColumn() < 3 && prevMove.getRow() < 3 &&
					  (whichColumn == prevMove.getColumn() + 1 && whichRow == prevMove.getRow() + 1))
					   utility += 1;
				   else if (prevMove.getColumn() > 0 && prevMove.getRow() > 0 &&
					  (whichColumn == prevMove.getColumn() - 1 && whichRow == prevMove.getRow() - 1))
					   utility += 1;
				   else if (prevMove.getColumn() < 3 && prevMove.getRow() > 0 &&
					  (whichColumn == prevMove.getColumn() + 1 && whichRow == prevMove.getRow() - 1))
					   utility += 1;
				   else if (prevMove.getColumn() > 0 && prevMove.getRow() < 3 &&
					  (whichColumn == prevMove.getColumn() - 1 && whichRow == prevMove.getRow() + 1))
					   utility += 1;
				   
				   // If the agent has a tile diagonally to the move you just made,
				   // sub 1 from utility as they can block a winning option for you.
				   else if (((prevMove.getColumn() == 0 && prevMove.getRow() == 0) || 
				   (prevMove.getColumn() == 1 && prevMove.getRow() == 1) || (prevMove.getColumn() == 2 && prevMove.getRow() == 2) ||
				   (prevMove.getColumn() == 3 && prevMove.getRow() == 3)) && whichColumn == whichRow)
					   utility += 1;
				   else if (((prevMove.getColumn() == 0 && prevMove.getRow() == 3) ||
				   (prevMove.getColumn() == 1 && prevMove.getRow() == 2) || (prevMove.getColumn() == 2 && prevMove.getRow() == 1) ||
				   (prevMove.getColumn() == 3 && prevMove.getRow() == 0)) && ((whichColumn == 0 && whichRow == 3) ||
				   (whichColumn == 1 && whichRow == 2) || (whichColumn == 2 && whichRow == 1) || (whichColumn == 3 && whichRow == 0)))
					   utility += 1;
				   
				   // If nothing helps you, then sub 1 from the utility
				   else
					   utility -= 1;
			   }
		   }
	   }
	   
	   // Return the best utility found
	   return utility;
   }
   
   pair<TileLocation, int> minValue(const MSGBoard&, int, int, int, TileLocation, int, int);
   
   pair<TileLocation, int> maxValue(const MSGBoard &board, int depth, int utilityA, int utilityB, TileLocation lastMove, int alpha, int beta)
   {
	   // Column and row of the action taken
	   int whichColumn, whichRow;
	   // The limit on how far down the tree to look
	   int depthLimit = 8;
	   
	   // If the game is over,
	   // return the last move and the utility found
	   if (board.isGameOver())
		   return (pair<TileLocation, int>(lastMove, utilityA));
	   
	   // If you've reached the depth limit,
	   // just return 0 as you don't know the utility of that path
	   if (depth > depthLimit)
		   return (pair<TileLocation, int>(lastMove, 0));
	   
	   // Best score to keep track of if there is a better move
	   int bestScore = INT_MIN;
	   
	   // Choice to be returned
	   pair<TileLocation, int> bestChoice(lastMove, bestScore);
	   
	   // Make sure to go as far as the depth allows
	   if (depth <= depthLimit)
	   {
		   // Variables to store the best location and score.
		   TileLocation storeMove = TileLocation();
		   int storeScore = 0;
		   
		   for (whichColumn = 0; whichColumn < boardSize; whichColumn += 1)
		   {
			   for (whichRow = 0; whichRow < boardSize; whichRow += 1)
			   {
				   // Select the location to try to check
				   TileLocation move = TileLocation(whichColumn, whichRow);
				   
				   // Only continue if the move is legal
				   if (board.isLegalMove(move))
				   {
					   // If it is legal, store the legal move
					   storeMove = move;
					   
					   // Create a temporary board to use
					   MSGBoard tempBoard = board.makeMove(move);
					   
					   // Find the utility of the new board AFTER you have made your move.
					   utilityA += findUtility(board, move, false);
					   
					   // If it is legal, store the utility of that move
					   storeScore = utilityA;
					   
					   // Find the outcome of the other opponent's next move
					   pair<TileLocation, int> nextMove = minValue(tempBoard, depth + 1, utilityA, utilityB, move, alpha, beta);
					   
					   // If the opponent's move is legal,
					   if (board.isLegalMove(nextMove.first)) 
					   {
						   // if their best next move is better than your best score,
						   if (nextMove.second > bestScore)
						   {
								// then that becomes your best move and best score
								bestScore = nextMove.second;
								bestChoice.first = nextMove.first;
								bestChoice.second = bestScore;
								
								// If your best score is better than your alpha,
								// set alpha to that best score
								if (bestScore > alpha)
									alpha = bestScore;
						   }
						   
						   // If your best score is >= your beta,
						   // then just return your best choice then.
						   if (bestScore >= beta)
								return bestChoice;
					   }
				   }
			   }
		   }
		   
		   // If you did not find a better choice by the end of the loop, then choose
		   // the last legal move that you found.
		   if (bestChoice.first.getColumn() == 0 && bestChoice.first.getRow() == 0)
		   {
			   bestChoice.first = storeMove;
			   bestChoice.second = storeScore;
		   }
	   }
	   
	   // Return your best choice if you get to this point.
	   return bestChoice;
   }
   
   pair<TileLocation, int> minValue(const MSGBoard &board, int depth, int utilityA, int utilityB, TileLocation lastMove, int alpha, int beta)
   {
	   // Column and row of the action taken
	   int whichColumn, whichRow;
	   // The limit on how far down the tree to look
	   int depthLimit = 8;
	   
	   // If the game is over,
	   // return the last move and the utility found
	   if (board.isGameOver())
		   return (pair<TileLocation, int>(lastMove, utilityB));
	   
	   // If you've reached the depth limit,
	   // just return 0 as you don't know the utility of that path
	   if (depth > depthLimit)
		   return (pair<TileLocation, int>(lastMove, 0));
	   
	   // Best score to keep track of if there is a better move
	   int bestScore = INT_MAX;
	   
	   // Choice to be returned
	   pair<TileLocation, int> bestChoice(lastMove, bestScore);
	   
	   // Make sure to go as far as the depth allows
	   if (depth <= depthLimit)
	   {
		   // Variables to store the best location and score.
		   TileLocation storeMove = TileLocation();
		   int storeScore = 0;
		   
		   for (whichColumn = 0; whichColumn < boardSize; whichColumn += 1)
		   {
			   for (whichRow = 0; whichRow < boardSize; whichRow += 1)
			   {
				   // Select the location to try to check
				   TileLocation move = TileLocation(whichColumn, whichRow);
				   
				   // Only continue if the move is legal
				   if (board.isLegalMove(move))
				   {
					   // If it is legal, store the legal move
					   storeMove = move;
					   
					   // Create a temporary board to use
					   MSGBoard tempBoard = board.makeMove(move);
					   
					   // Find the utility of the new board AFTER you have made your move.
					   utilityB -= findUtility(board, move, false);
					   
					   // If it is legal, store the utility of that move
					   storeScore = utilityB;
					   
					   // Find the outcome of the other opponent's next move
					   pair<TileLocation, int> nextMove = maxValue(tempBoard, depth + 1, utilityA, utilityB, move, alpha, beta);
					   
					   // If the opponent's move is legal,
					   if (board.isLegalMove(nextMove.first)) 
					   {
						   // if their best next move is better than your best score,
						   if (nextMove.second < bestScore)
						   {
							   // then that becomes your best move and best score
								bestScore = nextMove.second;
								bestChoice.first = nextMove.first;
								bestChoice.second = bestScore;
								
								// If your best score is better than your beta,
								// set beta to that best score
								if (bestScore < beta)
									beta = bestScore;
						   }
						   
						   // If your best score is <= your alpha,
						   // then just return your best choice then.
						   if (bestScore <= alpha)
								return bestChoice;
					   }
				   }
			   }
		   }
		   
		   // If you did not find a better choice by the end of the loop, then choose
		   // the last legal move that you found.
		   if (bestChoice.first.getColumn() == 0 && bestChoice.first.getRow() == 0)
		   {
			   bestChoice.first = storeMove;
			   bestChoice.second = storeScore;
		   }
	   }
	   
	   // Return your best choice if your get to this point.
	   return bestChoice;
   }
}


TileLocation msgAgentP(const MSGBoard &board)
{
   
   // Variable for your best choice
   pair<TileLocation, int> bestChoice;
   
   // If no move has been made, then find the best outer choice
   if (board.getNumTilesLeft() == 16)
	   return chooseBestOuter(board);
   
   
   // If you are A, try to get max score
   if (board.getNextPlayerToMove() == 'A')
		bestChoice = maxValue(board, 0, 0, 0, TileLocation(), INT_MIN, INT_MAX);
	
   // If you are B, try to get min score
   if (board.getNextPlayerToMove() == 'B')
		bestChoice = minValue(board, 0, 0, 0, TileLocation(), INT_MIN, INT_MAX);
	
   // Return the best move found after minimax
   return bestChoice.first;
}

/*		
	In this code, I went for a minimax algorithm to determine which choice the agent should make.
	The minimax's shining feature is the utility function, which determines the utility based off
	of how likely the agent is to lose or win based off a certain choice.
	
	UTILITY: The utility has a few sections that form together to determine the best result.
	The first part determines if either the enemy or the agent has a win condition. If this is
	the case, it severly affects the utility score, as it pushes you to win and steer away from
	allowing the enemy to win.
	Next, the utility function then checks which legal moves are left. For each legal move, based on
	whether the enemy or the agent had the last move, it determines if any help towards a win has been
	achieved. If the enemy has a board that is closer to a win, then the utility decreases. If the agent
	has a board that is closer to a win, then the utility increases.
	
	MINIMAX: The minimax follows a simple minimax structure and is generally the same as my previous submission.
	However, I also added a few features. The first one is if the depth is reached, the minimax passes a utility
	of 0. This helps to speed up the program and make better choices. Next, I added a feature that makes sure that
	if no legal move is beneficial for a win, then it just chooses the last legal move found. This is to stop the
	program from returning (0,0), which is often already taken by this point. This means that the agent on this
	iteration does not forfeit any rounds.
	
	I helped Devoux a bit with this assignment by explaining how I planned to implement a minimax
	approach. We then discussed what ideas we had to improve on the approach after but were unable
	to implement in time. For the second iteration, I provided him with the solution to the forfeits
	that we were noticing in our codes.
	I also gave my code to Jordan to test, but he just used it to see how it stacked up to his, and
	his was always better than mine. I did not get his code, but I learned that my agent does decent,
	but is not a good as others who may have a more sophisticated code.
	
	Have a great week!
	This is the agent I will choose for my final writeup, so I will explain my approach in greater detail there.
*/
