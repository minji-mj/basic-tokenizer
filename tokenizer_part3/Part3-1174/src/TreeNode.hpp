//============================================================================
// Name        : TreeNode.hpp
// Description : Course Project Part 3 (Directory Tree)
//============================================================================

#ifndef SRC_TREENODE_HPP_
#define SRC_TREENODE_HPP_

#include <iostream>
#include <vector>

//#define USE_OSS

#ifndef USE_OSS
#define OUT std::cout
#else
#include <sstream>
extern std::ostringstream oss;
#define OUT oss
#endif

namespace ensc251 {

	static unsigned count = 0;

	class TreeNode
	{
		typedef std::vector<TreeNode*> TreeNodePVect;

		protected:
			TreeNodePVect childPVector;
			typedef void (TreeNode::*action_func)(void);
			typedef void (TreeNode::*traverse_func)(void);

		public:
			TreeNode() {}
			TreeNode(const TreeNode& nodeToCopy)
			{
				// ***** this needs work *****
				for (unsigned i = 0; i < nodeToCopy.childPVector.size(); i++)
				{
					add_childP(nodeToCopy.childPVector[i]->clone());
				}
			}

			virtual TreeNode* clone() const { return new TreeNode(*this); }

			virtual ~TreeNode() {
				// do not modify this insertion on OUT
				OUT << "Destroying TreeNode with " << childPVector.size() << " children."<< std::endl;

				// ***** Complete this member function *****
				for (unsigned i = 0; i < childPVector.size(); i++)
				{
					delete childPVector[i];
				}
			}

			friend void swap(TreeNode& first, TreeNode& second) // the swap function (should never fail!)
		    {
		    	// ***** fill this in and use this if desired *****
				std::swap(first.childPVector, second.childPVector);
		    }

		    TreeNode & operator = (TreeNode other) // note: argument passed by value, copy made!
		    {
				// ***** Complete this code *****
		    	swap(other,*this);
		        return *this; // by convention, always return *this
		    }

			void add_childP(TreeNode* child) { childPVector.push_back(child);/* ***** Complete this member function ***** */ }
			void add_children(const TreeNodePVect& childPV) { childPVector.insert(childPVector.end(), childPV.begin(), childPV.end()); }
			const TreeNodePVect& get_children() const { return childPVector; }

			void traverse_children_post_order(traverse_func tf, action_func af)
			{
			    for(auto childP : childPVector) {
			        (childP->*tf)(); // traverse child's children using tf
			    }
			    (this->*af)();
			}

			void traverse_children_pre_order(traverse_func tf, action_func af)
			{
				// ***** Complete this member function *****
				(this->*af)();
				for(auto childP : childPVector) {
					(childP->*tf)();
				}
			}

			void traverse_children_in_order(traverse_func tf, action_func af)
			{
				// ***** Complete this member function *****
				(childPVector[0]->*tf)();
				(this->*af)();
				(childPVector[1]->*tf)();
			}

			void count_action()
			{
				count++;
			}

			void count_traverse()
			{
				traverse_children_post_order(&ensc251::TreeNode::count_traverse, &ensc251::TreeNode::count_action);
			}
			void reset_count() { count = 0; }
			unsigned get_count() const { return count; }

	};
}

#endif /* SRC_TREENODE_HPP_ */