// CS 3352, fall 2021
// Honors Any Segments Intersect

#include "bst.h"


int BSTNode::yValue (pair<int, int> start, pair<int, int> end, int x)
{
	int m = (end.second - start.second) / (end.first - start.first);
	int b = start.second - (m * start.first);
	int y = (m * x) + b;
	return y;
}

// Returns a node's key as a string.
string BSTNode::getKeyString()
{
   stringstream strstr;
   strstr << key;
   return strstr.str();
}

// Destructor:  Deallocate all nodes in the tree.
BinarySearchTree::~BinarySearchTree()
{
   BSTNode *oldRoot;
   while (root != NULL)
   {
      oldRoot = root;
      treeDelete(oldRoot);
      delete oldRoot;
   }
}

// Walks the subtree rooted at node x and returns a string that represents
// that subtree.
string BinarySearchTree::inorderTreeWalk(BSTNode *x)
{
   string result;
   result = "";
   if (x != NULL)
   {
      result += "(" + inorderTreeWalk(x->getLeftChild());
      result += x->getKeyString();
      result += inorderTreeWalk(x->getRightChild()) + ")";
   }
   return result;
}

// Finds and returns the node with the smallest key in the subtree rooted at
// node x.  Returns NULL if no such node is found.
BSTNode *BinarySearchTree::treeMinimum(BSTNode *x)
{
   while (x->getLeftChild() != NULL)
   {
      x = x->getLeftChild();
   }
   return x;
}

// Finds and returns the node with the smallest key larger than the key of
// node x.  Returns NULL if no such node is found.
BSTNode *BinarySearchTree::treeSuccessor(BSTNode *x)
{
   BSTNode *y;
   if (x->getRightChild() != NULL)
   {
      return treeMinimum(x->getRightChild());
   }
   y = x->getParent();
   while (y != NULL && x == y->getRightChild())
   {
      x = y;
      y = y->getParent();
   }
   return y;
}

// Inserts node z (already allocated) into the binary search tree.
void BinarySearchTree::treeInsert(BSTNode *z)
{
   BSTNode *x, *y;
   y = NULL;
   x = root;
   while (x != NULL)
   {
      y = x;
      if (z->getKey() < x->yValue(x->p1.point, x->p2.point, z->p1.point.first))
      {
         x = x->getLeftChild();
      }
      else
      {
         x = x->getRightChild();
      }
   }
   z->setParent(y);
   if (y == NULL)
   {
      root = z;
   }
   else if (z->getKey() < y->yValue(y->p1.point, y->p2.point, z->p1.point.first))
   {
      y->setLeftChild(z);
   }
   else
   {
      y->setRightChild(z);
   }
}

// Finds and returns the node with key k in the subtree rooted at node x.
// Returns NULL if no such node is found.
BSTNode *BinarySearchTree::treeSearch(BSTNode *x, int k)
{
	// If there is no node x in the tree or the key of x
	//  is the key we are looking for, then return x.
	if ((x == NULL) || (k == x->getKey())) 
		return x;
	
	// If the key we are looking for is less than the key of x,
	//  then search for the key starting at the left child of x.
	if (k < x->getKey())
		return treeSearch(x->getLeftChild(), k);
	
	// If the key we are looking for is greater than the key of x,
	//  then search for the key starting at the right child of x.
	return treeSearch(x->getRightChild(), k);
}

// Finds and returns the node with the largest key in the subtree rooted at
// node x.  Returns NULL if no such node is found.
BSTNode *BinarySearchTree::treeMaximum(BSTNode *x)
{
	// Iterate through the right children starting at node x
	//  until the child you are on does not have a right child.
	while (x->getRightChild() != NULL)
		x = x->getRightChild();
	
	// Return the last right child from the starting node x.
	return x;
}

// Finds and returns the node with the largest key smaller than the key of
// node x.  Returns NULL if no such node is found.
BSTNode *BinarySearchTree::treePredecessor(BSTNode *x)
{
	// Set node y as the predecessor.
	BSTNode *y;
	
	// If node x does not have any left children,
	//  then find the highest value child smaller than x.
	if (x->getLeftChild() != NULL)
		return treeMaximum(x->getLeftChild());
	
	// If node x does have a left child, then set y as the parent of x.
	y = x->getParent();
	
	// While node y is not the root and node x is the left child of node y
	while (y != NULL && x == y->getLeftChild())
	{
		// Move up the tree with y and set x as one of its children.
		x = y;
		y = y->getParent();
	}
	// Return the highest value of y that is smaller than x.
	return y;
}

// Replaces subtree rooted at node u with the subtree rooted at node v.
// Should be used by the treeDelete function.
void BinarySearchTree::transplant(BSTNode *u, BSTNode *v)
{
	// If u is the root of the tree, set v as the root of the tree.
	if (u->getParent() == NULL)
		root = v;
	
	// If u is the left child of its parent,
	//  then replace u as the left child of its parent with v.
	else if (u == u->getParent()->getLeftChild())
		u->getParent()->setLeftChild(v);
	// If u is the right child of its parent,
	//  then replace u as the right child of its parent with v.
	else
		u->getParent()->setRightChild(v);
	
	// If v is not null, then set v's parent as the parent of u
	//  in order to connect both parent and child pointers.
	if (v != NULL)
		v->setParent(u->getParent());
}

// Removes node z from the binary search tree.  (Does not deallocate node
// z.)
void BinarySearchTree::treeDelete(BSTNode *z)
{
	// If z node's left child does not exist,
	//  then replace z with the right child.
	if (z->getLeftChild() == NULL)
		transplant(z, z->getRightChild());
	// if z node's right child does not exist,
	//  then replace z with the left child.
	else if (z->getRightChild() == NULL)
		transplant(z, z->getLeftChild());
	// If z node has both a left and right child
	else
	{
		// a new node y will be the minimum value of the right child.
		BSTNode *y = treeMinimum(z->getRightChild());
		
		// If node y is not a child of z
		if (y->getParent() != z)
		{
			// set y node's right child as y in the tree.
			transplant(y, y->getRightChild());
			
			// Link node y to its new right child, node z's right child.
			y->setRightChild(z->getRightChild());
			// Link node y as y node's right child's new parent.
			y->getRightChild()->setParent(y);
		}
		
		// Even if y is not a child of z,
		//  the rest of the code is called.
		
		// Set y node as z in the tree.
		transplant(z, y);
		
		// Link node y to its new left child, node z's left child.
		y->setLeftChild(z->getLeftChild());
		// Link node y as the left child's new parent.
		y->getLeftChild()->setParent(y);
	}
}

