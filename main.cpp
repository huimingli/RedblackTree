#include<iostream>
#include<queue>
#include <cstdlib>
#include<stack>
using namespace std;
enum class COLOR{ RED , BLACK};
class TreeNode {
private:
	TreeNode* left, * right, * parent;
public:
	int val;
	COLOR color;
	TreeNode(int val_)
		:val(val_),left(NULL),
		 right(NULL),parent(NULL),
		 color(COLOR::RED)
	{
	}
	friend class RBTree;
};

class RBTree {
public:
	RBTree() {
		root = NULL;
	}

	TreeNode* parent(TreeNode* n) {
		if (n == NULL)
			return NULL;
		return n->parent;
	}

	TreeNode* grandParent(TreeNode*n) {
		if (n == NULL)
			return NULL;
		return n->parent->parent;
	}

	TreeNode* uncle(TreeNode* n) {
		if (n == NULL)
			return NULL;
		if (parent(n) == grandParent(n)->left)
			return grandParent(n)->right;
		else
			return grandParent(n)->left;
	}

	TreeNode* sibling(TreeNode* n) {
		if (n == NULL)
			return NULL;
		if (n == parent(n)->left)
			return parent(n)->right;
		else
			return parent(n)->left;
	}

	//递归方式插入节点
	//TreeNode* tmp = NULL;
	//void insertRBTreeNode(TreeNode*&n, TreeNode* newNode) {
	//	if (n == NULL) {
	//		n = newNode;
	//		n->parent = tmp;
	//		tmp = NULL;
	//		insert_case1(n);
	//		verity_properties();
	//		return;
	//	}
	//	tmp = n;
	//	if (n->val < newNode->val) {
	//		insertRBTreeNode(n->right, newNode);
	//	}
	//	else if (n->val > newNode->val) {
	//		insertRBTreeNode(n->left, newNode);
	//	}
	//	else {
	//		//已经存在的节点不插入
	//		return;
	//	}
	//}

	void insertRBTreeNode(TreeNode* newNode) {
		if (root == NULL) {
			root = newNode;
		}
		else {
			TreeNode* t = root;
			while (true) {
				if (t->val < newNode->val) {
					if (t->right == NULL) {
						t->right = newNode;
						newNode->parent = t;
						break;
					}
					t = t->right;
				}
				else if (t->val > newNode->val) {
					if (t->left == NULL) {
						t->left = newNode;
						newNode->parent = t;
						break;
					}
					t = t->left;
				}
				else {
					//已经存在的节点不插入
					break;
				}
			}
		}
		insert_case1(newNode);
		verity_properties();
	}

	void printInLevel() {
		printInLevel(root);
	}

	TreeNode* lookup_node(int val) {
		return lookup_node(root, val);
	}
	
	void rotate_left(TreeNode* n) {
		TreeNode* y = n->right;
		replace_node(n, y);
		n->right = y->left;
		if (y->left != NULL)
			y->left->parent = n;
		y->left = n;
		n->parent = y;
	}

	void rotate_right(TreeNode* n) {
		TreeNode* y = n->left;
		replace_node(n, y);
		n->left = y->right;
		if (y->right != NULL)
			y->right->parent = n;
		y->right = n;
		n->parent = y;
	}

	void deleteRBTreeNode(int val) {
		TreeNode* child = NULL;
		TreeNode* target = lookup_node(val);
		if (target == NULL)
			return;
		if (target->left != NULL && target->right != NULL) {
			TreeNode* pred = max_node(target->left);
			target->val = pred->val;
			target = pred;
		}
		//此时最多有一个儿子
		child = target->right == NULL ? target->left : target->right;
		//1，红色节点，直接删除即可
		if (node_color(target) == COLOR::BLACK){
			target->color = node_color(child);
			delete_case1(target);
		}
		replace_node(target, child);
		delete target;
		verity_properties();
	}

	//2,如果是根节点就不用处理
	void delete_case1(TreeNode* n) {
		if (n->parent == NULL)
			return;
		else
			delete_case2(n);
	}

	//3，非根黑色节点,兄弟是红色，父节点变红，兄弟变黑，旋转父节点
	void delete_case2(TreeNode* n) {
		if (node_color(sibling(n)) == COLOR::RED) {
			sibling(n)->color = COLOR::BLACK;
			n->parent->color = COLOR::RED;
			if (n == n->parent->left) {
				rotate_left(n->parent);
			}
			else {
				rotate_right(n->parent);
			}
		 }
		delete_case3(n);
	}

	//4,父节点黑，兄弟黑，兄弟两个孩子黑，兄弟变红
	void delete_case3(TreeNode* n) {
	    if (node_color(n->parent)== COLOR::BLACK && node_color(sibling(n))== COLOR::BLACK 
			&& node_color(sibling(n)->left) == COLOR::BLACK && node_color(sibling(n)->right) == COLOR::BLACK) {
			sibling(n)->color = COLOR::RED;
			delete_case1(n->parent);
		}
		else
			delete_case4(n);
	}

	//5,父节点红，兄弟黑，兄弟两个孩子黑，兄弟变红，父亲变黑
	void delete_case4(TreeNode* n) {
		if (node_color(n->parent) == COLOR::RED && node_color(sibling(n)) == COLOR::BLACK &&
			node_color(sibling(n)->left) == COLOR::BLACK && node_color(sibling(n)->right) == COLOR::BLACK)
		{
			sibling(n)->color = COLOR::RED;
			n->parent->color = COLOR::BLACK;
		}
		else
			delete_case5(  n);
	}

	//6,目标节点是左孩子，兄弟是黑，兄弟左孩子红，右孩子黑，兄弟变红，兄弟左孩子变黑，兄弟右旋;目标是右孩子，兄弟黑，兄弟左孩子黑，右孩子红，
	void delete_case5(TreeNode* n) {
		if (n == n->parent->left && node_color(sibling(n)) ==COLOR::BLACK &&
			node_color(sibling(n)->left) == COLOR::RED && node_color(sibling(n)->right) == COLOR::BLACK)
		{
			sibling(n)->color = COLOR::RED;
			sibling(n)->left->color = COLOR::BLACK;
			rotate_right( sibling(n));
		}
		else if (n == n->parent->right && node_color(sibling(n)) == COLOR::BLACK &&
			node_color(sibling(n)->right) == COLOR::RED && node_color(sibling(n)->left) == COLOR::BLACK)
		{
			sibling(n)->color = COLOR::RED;
			sibling(n)->right->color = COLOR::BLACK;
			rotate_left( sibling(n));
		}
		delete_case6(  n);
	}

	//7,上一个case保证了sibling是黑色，目标节点是左孩子，兄弟的右孩子为红，目标节点是右孩子，兄弟左孩子为红
	//兄弟的颜色变为父亲的颜色，父亲变成黑色，目标为左孩子，兄弟右孩子变黑，父亲左旋；目标为右孩子，兄弟左孩子变黑，父亲右旋
	void delete_case6(TreeNode* n) {
		sibling(n)->color = node_color(n->parent);
		n->parent->color = COLOR::BLACK;
		if (n == n->parent->left)
		{
			sibling(n)->right->color = COLOR::BLACK;
			rotate_left(  n->parent);
		}
		else
		{
			sibling(n)->left->color = COLOR::BLACK;
			rotate_right( n->parent);
		}
	}

	~RBTree() {
		destoryRBTree(root);
	}

private:

	void destoryRBTree(TreeNode*n) {
		if (n == NULL)
			return;
		destoryRBTree(n->left);
		destoryRBTree(n->right);
		delete n;
		n = NULL;
	}

	bool verity_properties() {
		bool v1 = verity_property_1(root);
		bool v2 = verity_property_2(root);
		bool v4 = verity_property_4(root);
		bool v5 = verity_property_5(root);
		bool v6 = verity_property_6(root);
		if (!v1)
			cout << "verity_property_1(nodes are red or black) failed!" << endl;
		if (!v2)
			cout << "verity_property_2(tree root is black)  failed!" << endl;
		if (!v4)
			cout << "verity_property_4(no continuous red nodes)  failed!" << endl;
		if (!v5)
			cout << "verity_property_5(black paths are as far as each other)  failed!" << endl;
		if (!v6)
			cout << "verity_property_6(leftchild val less than node val while rightchild val more than node val)  failed!" << endl;
		bool result = v1 && v2 && v4 && v5 && v6;
		return result;
	}

	TreeNode* lookup_node(TreeNode* n, int val) {
		while (n != NULL) {
			if (n->val < val) {
				n = n->right;
			}
			else if (n->val > val) {
				n = n->left;
			}
			else {
				return n;
			}
		}
		return NULL;
	}

	//1，插入第一个节点，根节点，变成黑色即可
	void insert_case1(TreeNode* insertedNode) {
		if (insertedNode->parent == NULL)
			insertedNode->color = COLOR::BLACK;
		else
			insert_case2(insertedNode);
	}

	//2，插入的节点父节点是黑色，不用处理
	void insert_case2(TreeNode* insertedNode) {
		if (node_color(insertedNode->parent) == COLOR::BLACK)
			return;
		else
			insert_case3(insertedNode);
	}

	//3,插入的节点父亲为红色，叔叔也是红色，父亲和叔叔变成黑色，祖父变成红色，再调整祖父
	void insert_case3(TreeNode* insertedNode) {
		if (node_color(uncle(insertedNode)) == COLOR::RED) {
			insertedNode->parent->color = COLOR::BLACK;
			uncle(insertedNode)->color = COLOR::BLACK;
			grandParent(insertedNode)->color = COLOR::RED;
			insert_case1(grandParent(insertedNode));
		}
		else {
			insert_case4(insertedNode);
		}
	}

	//4,插入的节点父亲为红色，叔叔为黑色，插入的节点是右孩子，父节点是左孩子，左旋；插入的节点是左孩子，父节点是右孩子，右旋
	void insert_case4(TreeNode* insertedNode) {
		if (insertedNode == insertedNode->parent->right && insertedNode->parent == grandParent(insertedNode)->left) {
			rotate_left(insertedNode->parent);
			insertedNode = insertedNode->left;
		}
		else if (insertedNode == insertedNode->parent->left && insertedNode->parent == grandParent(insertedNode)->right) {
			rotate_right(insertedNode->parent);
			insertedNode = insertedNode->right;
		}
		insert_case5(insertedNode);
	}

	//5，插入的节点父亲为红色，叔叔为黑色，父亲变黑，祖父变红，插入的节点是左孩子，父亲也是左孩子，祖父右旋；插入的节点是右孩子，父亲也是右孩子，祖父左旋
	void insert_case5(TreeNode* insertedNode) {
		insertedNode->parent->color = COLOR::BLACK;
		grandParent(insertedNode)->color = COLOR::RED;
		if (insertedNode == insertedNode->parent->left && insertedNode->parent == grandParent(insertedNode)->left) {
			rotate_right(grandParent(insertedNode));
		}
		else if (insertedNode == insertedNode->parent->right && insertedNode->parent == grandParent(insertedNode)->right) {
			rotate_left(grandParent(insertedNode));
		}
	}

	

	bool verity_property_1(TreeNode* n) {
		if (n == NULL)
			return true;
		if (n->color != COLOR::RED && n->color != COLOR::BLACK)
			return false;
		return verity_property_1(n->left) && verity_property_1(n->right);
	}

	bool verity_property_2(TreeNode* n) {
		return node_color(n) == COLOR::BLACK;
	}

	COLOR node_color(TreeNode* n) {
		return n == NULL ? COLOR::BLACK : n->color;
	}

	bool verity_property_4(TreeNode* n) {
		if (node_color(n) == COLOR::RED) {
			return node_color(n->parent) == COLOR::BLACK && node_color(n->left) == COLOR::BLACK && node_color(n->right) == COLOR::BLACK;
		}
		if (n == NULL)
			return true;
		return verity_property_4(n->left) && verity_property_4(n->right);
	}

	bool verity_property_5(TreeNode* n) {
		int deep = -1;
		bool valid = true;
		verity_property_5_helper(n, 0, deep, valid);
		return valid;
	}

	void  verity_property_5_helper(TreeNode* n, int count, int& deep,bool &valid) {
		if (node_color(n) == COLOR::BLACK) {
			count++;
		}
		if (n == NULL) {
			if (deep == -1) {
				deep = count;
			}
			if (deep != count)
				valid = false;
			return;
		}
		verity_property_5_helper(n->left, count, deep, valid);
		verity_property_5_helper(n->right, count, deep, valid);
	}

	bool verity_property_6(TreeNode* n) {
		if (n == NULL)
			return true;
		stack<TreeNode*> s;
		TreeNode* t = n;
		int tmp = INT_MIN;
		while (!s.empty() || t != NULL) {
			while (t != NULL) {
				s.push(t);
				t = t->left;
			}

			t = s.top();
			if (t->val < tmp)
				return false;
			tmp = t->val;
			s.pop();
			t = t->right;
		}
		return true;
	}

	void printInLevel(TreeNode* n) {
		if (n == NULL) {
			cout << "empty tree" << endl;
			return;
		}
		cout << "printing tree:" << endl;
		queue<TreeNode*>q;
		q.push(n);
		while (!q.empty()) {
			int qSize = q.size();
			for (int i = 0; i < qSize; i++) {
				TreeNode* t = q.front();
				q.pop();
				cout << t->val << " ";
				if (t->left != NULL)
					q.push(t->left);
				if (t->right != NULL)
					q.push(t->right);
			}
			cout << endl;
		}
		cout << endl;
	}

	void replace_node(TreeNode* oldNode, TreeNode* newNode) {
		if (oldNode->parent == NULL) {
			root = newNode;
		}
		else {
			if (oldNode == oldNode->parent->left) {
				oldNode->parent->left = newNode;
			}
			else {
				oldNode->parent->right = newNode;
			}
		}
		if (newNode != NULL) {
			newNode->parent = oldNode->parent;
		}
	}

	TreeNode* min_node(TreeNode* n) {
		if (n == NULL)
			return NULL;
		while (n->left != NULL)
			n = n->left;
		return n;
	}

	TreeNode* max_node(TreeNode* n)
	{
		if (n == NULL)
			return NULL;
		while (n->right != NULL)
			n = n->right;
		return n;
	}

public:
	TreeNode* root;
};

int main() {
	RBTree tree;
	for (int i = 0; i < 12; i++)
	{
		int x = rand() % 12;
		TreeNode* t = new TreeNode(x);
		tree.insertRBTreeNode(t);

	}
	//tree.deleteRBTreeNode(4);
	tree.printInLevel();
	TreeNode* target = tree.lookup_node(33311);
	if (target != NULL) {
		cout << "target found val is " << target->val << endl<<endl;
	}
	else {
		cout << "target not found" << endl << endl;
	}
 

	tree.deleteRBTreeNode(11);
	tree.printInLevel();

	return 0;
}
 