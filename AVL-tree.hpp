#include <cstdlib>
#include <algorithm>
#include <iostream>

class AVL_tree {

public:
	AVL_tree();
	~AVL_tree();

	void insert(int e);
	void remove(int e);
	bool contains(int e);
	void clear();

	int top() {
		int result = 100000;
		if (_root != NULL)
			result = _root->val;
		return result;
	}

protected:
	struct Node {
		int val;
		int height;

		Node *parent;
		Node *left_child;
		Node *right_child;
	};

	Node *_root;
	
	void balance(Node *node);
	int balance_index(Node *node);
	void rotate_left(Node *node);
	void rotate_right(Node *node);
	void update_height(Node *node);
	void update_heights(Node *node);
	int height(Node *node);
	Node* find(int e);
	Node* find_predecessor(Node* node);

};

AVL_tree::AVL_tree() {
	_root = NULL;
}

AVL_tree::~AVL_tree() {
	clear();
}

/*
*	Insert a new node containing the given value.
*/
void AVL_tree::insert(int e) {
	Node *new_node = (Node *) malloc(sizeof(Node));
	new_node->val = e;
	new_node->height = 1;
	new_node->left_child = NULL;
	new_node->right_child = NULL;

	if (_root == NULL) {
		new_node->parent = NULL;
		_root = new_node;
	}
	else {
		Node *cur = _root;

		while (true) {
			if (e < cur->val) {
				if (cur->left_child != NULL) {
					cur = cur->left_child;
					continue;
				}
				else {
					cur->left_child = new_node;
					new_node->parent = cur;
					break;
				}
			}
			else {
				if (cur->right_child != NULL) {
					cur = cur->right_child;
					continue;
				}
				else {
					cur->right_child = new_node;
					new_node->parent = cur;
					break;
				}
			}
		}

		balance(new_node);
	}
}

/*
*	Remove a node containing given element.
*/
void AVL_tree::remove(int e) {
	Node *node = find(e);
	Node *tmp = NULL;

	if (node == NULL)
		return;

	while (true) {
		if (node->left_child == NULL 
			&& node->right_child == NULL) {
			// No children. Just remove the node.

			if (node == _root) {
				_root = NULL;
			}
			else {
				tmp = node->parent;
				if (tmp->left_child == node)
					tmp->left_child = NULL;
				else
					tmp->right_child = NULL;
				balance(tmp);
			}
			free(node);

			break;
		}
		else if (node->left_child != NULL
			&& node->right_child == NULL) {
			// No right child. Put left subtree's root 
			// instead of the current node.

			if (node == _root) {
				_root = node->left_child;
				_root->parent = NULL;
			}
			else {
				tmp = node->parent;
				if (tmp->left_child == node)
					tmp->left_child = node->left_child;
				else
					tmp->right_child = node->left_child;
				node->left_child->parent = tmp;
				balance(tmp);
			}
			free(node);
			break;
		}
		else if (node->left_child == NULL
			&& node->right_child != NULL) {
			// No left child. Put rigth subtree's root 
			// insted of the current node.

			if (node == _root) {
				_root = node->right_child;
				_root->parent = NULL;
			}
			else {
				tmp = node->parent;
				if (tmp->left_child == node)
					tmp->left_child = node->right_child;
				else
					tmp->right_child = node->right_child;
				node->right_child->parent = tmp;
				balance(tmp);
			}
			free(node);
			break;
		}
		else {
			// Has both children. Swap with the right most predecessor.
			// And delete the predecessor.

			tmp = find_predecessor(node);
			node->val = tmp->val;
			node = tmp;
		}
	}
}

/*
*	Check whether the tree contains a given value.
*/
bool AVL_tree::contains(int e) {
	if (find(e) != NULL)
		return true;
	
	return false;
}

/*
*	Clear the tree.
*/
void AVL_tree::clear() {
	Node *node = _root, *tmp = NULL;

	while (node != NULL) {
		if (node->left_child == NULL
			&& node->right_child == NULL) {
			tmp = node->parent;

			free(node);
			node = tmp;
		}
		else if (node->left_child != NULL) {
			tmp = node;
			node = node->left_child;
			tmp->left_child = NULL;
		}
		else {
			tmp = node;
			node = node->right_child;
			tmp->right_child = NULL;
		}
	}

	_root = NULL;
}

/*
*	Return a reference to a node containing a given value.
*	If no such node found, return NULL.
*/
AVL_tree::Node* AVL_tree::find(int e) {
	Node *node = _root;
	while (node != NULL) {
		if (node->val == e)
			return node;
		else if (node->val < e)
			node = node->right_child;
		else
			node = node->left_child;
	}
	
	return NULL;
}

/*
*	Return the right-most node in the left subtree of the given node.
*	
*	If the left subtree is empty, then return the node itself.
*/
AVL_tree::Node* AVL_tree::find_predecessor(AVL_tree::Node* node) {
	if (node->left_child == NULL)
		return node;

	node = node->left_child;
	while (node->right_child != NULL) {
		node = node->right_child;
	}

	return node;
}

/*
*	Return balance index, difference between left child's
*	and right child's subtrees heights.
*	
*	The balance index of an empty tree is considered 0.
*/
int AVL_tree::balance_index(AVL_tree::Node *node) {
	int result = 0;
	if (node != NULL)
		result = height(node->left_child) 
			     - height(node->right_child);
	return result;
}

/*
*	Balance the tree. Start balancing from the given node.
*/
void AVL_tree::balance(AVL_tree::Node *node) {
	Node *tmp = NULL;
	int diff = 0, diff2 = 0;;

	while (node != NULL) {
		diff = balance_index(node);

		if (diff == 2) { 
			tmp = node->left_child;

			diff2 = balance_index(tmp);

			// Double rotation situation.
			if (diff2 == -1) {
				rotate_left(tmp->right_child);
			}

			rotate_right(node->left_child);
			// node->parent is a new root of the subtree.
			// Update heights to the root.
			update_heights(node->parent);

			break;
		}
		else if (diff == -2) {
			tmp = node->right_child;

			diff2 = balance_index(tmp);

			// Double rotation situation.
			if (diff2 == 1) {
				rotate_right(tmp->left_child);
			}

			rotate_left(node->right_child);
			// node->parent is a new root of the subtree.
			// Update heights to the root.
			update_heights(node->parent);

			break;
		}
		
		// If subtree is balanced, just update node's height.
		// Check parent subtree.
		node->height = 1 + std::max(height(node->left_child),
								 height(node->right_child));
		node = node->parent;
	}
}

/*
*	Rotate the given node left.
*		A                 A
*       |                 |
*		B        =>       C
*	   / \               / \
*	 T0   C             B   T2
*        / \           / \
*      T1   T2       T0   T1
*
*	C - given node.
*/
void AVL_tree::rotate_left(Node *node) {
	Node *parent = node->parent;
	Node *pparent = NULL;

	// If B == NULL then there is nowhere to rotate
	if (parent == NULL)
		return;

	// This is a reference to A
	pparent = parent->parent;

	// Left rotation
	parent->right_child = node->left_child;
	if (parent->right_child != NULL)
		parent->right_child->parent = parent;
	
	node->parent = pparent;
	node->left_child = parent;
	parent->parent = node;

	// Update heights of B anc C after rotation.
	// Update B _first_ because C's height depends on B's.
	update_height(parent);
	update_height(node);


	// If A != NULL restore correct reference from A to C.
	// Notice, reference from C to A has already been restored.
	// 
	// If A == NULL then B was a root of the tree. Then C becomes
	// a new root after rotation. 
	if (pparent != NULL) {
		if (pparent->left_child == parent)
			pparent->left_child = node;
		else
			pparent->right_child = node;

		update_height(pparent);
	}
	else {
		_root = node;
	}
}

/*
*	Rotate the given node right.
*		A                A
*       |                |
*		B       =>       C
*	   / \              / \
*	  C   T0          T1   B
*    / \                  / \
*  T1   T2              T2   T0
*
*	C - given node.
*/
void AVL_tree::rotate_right(Node *node) {
	Node *parent = node->parent;
	Node *pparent = NULL;

	// If B == NULL then there is nowhere to rotate
	if (parent == NULL)
		return;

	// This is a reference to A
	pparent = parent->parent;

	// Right rotation
	parent->left_child = node->right_child;
	if (parent->left_child != NULL)
		parent->left_child->parent = parent;
	
	node->parent = pparent;
	node->right_child = parent;
	parent->parent = node;

	// Update heights of B anc C after rotation.
	// Update B _first_ because C's height depends on B's.
	update_height(parent);
	update_height(node);


	// If A != NULL restore correct reference from A to C.
	// Notice, reference from C to A has already been restored.
	// 
	// If A == NULL then B was a root of the tree. Then C becomes
	// a new root after rotation. 
	if (pparent != NULL) {
		if (pparent->left_child == parent)
			pparent->left_child = node;
		else
			pparent->right_child = node;

		update_height(pparent);
	}
	else {
		_root = node;
	}
}

/*
*	Update height of a subtree rooted in the given node.
*/
void AVL_tree::update_height(AVL_tree::Node *node) {
	node->height = 1 + std::max(height(node->left_child),
						   height(node->right_child));
}

/*
*	Update heights of subtrees rooted in nodes on a
*	path from the given node to the root, inclusively.
*/
void AVL_tree::update_heights(AVL_tree::Node *node) {
	while (node != NULL) {
		update_height(node);
		node = node->parent;
	}
}

/*
*	Return subtree height rooted in the given node.
*	The height of an empty tree is considered to be 0.
* 	The height of a tree consisting of a single given node
*	is considered to be 1.
*/
int AVL_tree::height(AVL_tree::Node *node) {
	int result = 0;
	if (node != NULL)
		result = node->height;

	return result;
}