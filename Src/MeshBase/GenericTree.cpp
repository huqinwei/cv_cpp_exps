// #include "MeshBase/GenericTree.h"
// // =======================================================================
// //   Implementation section
// // =======================================================================
// 
// // Notes about checking whether pointers are null:
// 
// // When checking whether a pointer is null or not, several pieces of syntax
// // have the same effect. If you have a pointer called ptr, both of these
// // expressions check if the pointer is null:
// //   if (nullptr == ptr)
// //   if (!ptr)
// // Both of these expressions check if the pointer is NOT null, meaning it
// // has any nonzero value:
// //   if (nullptr != ptr)
// //   if (ptr)
// 
// // Notes about implementing templated member functions:
// 
// // Since this is a templated class, there are special considerations
// // about where to write the implementation of the member functions.
// // The compiler uses templates to generate code at compile time, and so it
// // needs to have a complete definition of the template in each compilation
// // unit. (Those are the separate .cpp files that are compiled into .o files
// // before being linked together into an executable). One way to ensure this
// // is to implement the templated functions right here in the header file
// // itself, instead of using a cpp file for the class implementation.
// 
// // There are other ways to deal with this issue. We could write a separate
// // header file with the implementation and include it here with some extra
// // preprocessor directives. That would at least keep the implementation in
// // a separate file for easy editing, but it wouldn't speed up recompilation
// // at all. Another way to deal with it is to place the implementation in
// // a cpp file, but then we must explicitly declare ahead of time in that cpp
// // file all of the versions of our template that may be used. That makes it
// // harder for other people to reuse the library without editing the library
// // files.
// 
// // Another issue is that our inner TreeNode class type is dependent
// // on a specific instance of the GenericTree<T> class template. Within these
// // function definitions, we can simply refer to "TreeNode" and the
// // compiler will know what we mean. But to use TreeNode as a return
// // type at global scope, we have to certify that GenericTree<T>::TreeNode
// // is a type by writing "typename" before it as well.
// // template <typename T>
// // GenericTree<T>::TreeNode::TreeNode() : parentPtr(nullptr) {}
// // template <typename T>
// // GenericTree<T>::TreeNode::TreeNode(const T& dataArg) : parentPtr(nullptr), data(dataArg) {}
// // template <typename T>
// // GenericTree<T>::TreeNode::~TreeNode() {}
// 
// template <typename T>
// typename GenericTree<T>::TreeNode* GenericTree<T>::createRoot(const T& rootData) {
// 
// 	// If the rootNodePtr member variable already has a nonzero value assigned,
// 	// then the root node already exists, and it's an error to try to recreate it.
// 	if (nullptr != rootNodePtr) {
// 		// We don't always need to import <string> just to reuse a short error message.
// 		// We can store short strings as constant arrays of char type at compile time.
// 		// (A constexpr is similar to const, but must be entirely evaluated at compile time.)
// 		constexpr char ERROR_MESSAGE[] = "Tried to createRoot when root already exists";
// 		// Display the error message to the standard error stream.
// 		std::cerr << ERROR_MESSAGE << std::endl;
// 		// Throw an exception containing the error message.
// 		// We won't catch the exception, so this terminates the program.
// 		throw std::runtime_error(ERROR_MESSAGE);
// 	}
// 
// 	rootNodePtr = new TreeNode(rootData);
// 
// 	// Return a copy of the root node pointer.
// 	return rootNodePtr;
// }
// 
// template <typename T>
// typename GenericTree<T>::TreeNode* GenericTree<T>::TreeNode::addChild(const T& childData) {
// 
// 	// We prepare a new child node with the given data.
// 	TreeNode* newChildPtr = new TreeNode(childData);
// 
// 	// The "this" pointer in C++ always points to the current instance of the
// 	//  class for which we are defining a function body.
// 	// Here, "this" is a pointer to the current TreeNode.
// 
// 	// Assign this current node as the new child's parent pointer.
// 	newChildPtr->parentPtr = this;
// 
// 	// This node (the parent) already has a data structure to keep track
// 	// of its children pointers. We add the new child to the list.
// 	childrenPtrs.push_back(newChildPtr);
// 
// 	// Return a copy of the pointer to the new child.
// 	return newChildPtr;
// }
// 
// template <typename T>
// void GenericTree<T>::deleteSubtree(TreeNode* targetRoot) {
// 
// 	// Deleting a subtree requires deallocating the memory used by the nodes,
// 	// but since the pointers are stored in the tree itself, we need to
// 	// traverse the subtree to identify those pointers. We have to be careful
// 	// about the order in which we do this so that we can reach all the nodes
// 	// that need to be deleted.
// 
// 	// We could have implemented this function recursively, since every node
// 	// in the tree is like the root of another subtree. When traversing the
// 	// tree recursively, each call to the recursive function has to wait for
// 	// the recursive calls it makes to return, creating a "call stack". In
// 	// essence, the recursive calls act like an implicit data structure
// 	// themselves, based on the order in which they are called and return.
// 	// That is a very elegant way to write this in some languages, and this
// 	// course's lectures on binary search trees show how you can do that.
// 
// 	// However, recursion in C++ can also cause problems if too many layers
// 	// of recursion happen in succession before returning, because of the
// 	// finite amount of stack memory allocated for each C++ program. There
// 	// are special design considerations to think about when you want to
// 	// ensure that recursion will be optimized by the compiler.
// 
// 	// Here is a way to do it iteratively, using loops instead of recursion.
// 	// We need to use an explicit data structure (such as a stack) to
// 	// keep track of nodes that still need to be traversed, since we won't
// 	// rely on the call stack to do that implicitly.
// 
// 	// If the targeted subtree root doesn't exist, then there is nothing to do.
// 	if (nullptr == targetRoot) {
// 		return;
// 	}
// 
// 	// Check that the specified node to delete is in the same tree as this
// 	// class instance that's calling the function.
// 	{
// 		TreeNode* walkBack = targetRoot;
// 		while (walkBack->parentPtr) {
// 			// Walk back from the targeted node to its ultimate parent, the root.
// 			// (The root has no parent, so the walk ends there.)
// 			walkBack = walkBack->parentPtr;
// 		}
// 		// The ultimate root found must be this tree's root. Otherwise we're in
// 		// a different tree.
// 		if (walkBack != rootNodePtr) {
// 			throw std::runtime_error("Tried to delete a node from a different tree");
// 		}
// 	}
// 
// 	// We'll take note whether this is the root of the entire tree.
// 	bool targetingWholeTreeRoot = (rootNodePtr == targetRoot);
// 
// 	// If the subtree root node has a parent, then the parent should no longer
// 	// list it as a child. (Otherwise, targetRoot is actually the root of the
// 	// whole tree, so it has no parent, and we can skip this section.)
// 	if (targetRoot->parentPtr) {
// 
// 		// A flag for error checking: We need to find the target node
// 		// listed as a child of its parent. We will keep track as we search.
// 		bool targetWasFound = false;
// 
// 		// Loop through the parent's listed children using a reference variable
// 		// in a range-based for loop. (Yes, currentChildPtr is a pointer, but
// 		// we're accesssing each pointer directly by reference this way, so we
// 		// can change the original pointers stored in targetRoot->parentPtr->childrenPtrs
// 		// that we are iterating over, instead of acting on temporary copies.)
// 		// If the child is found under its parent as expected, overwrite it
// 		// in-place with nullptr.
// 		for (auto& currentChildPtr : targetRoot->parentPtr->childrenPtrs) {
// 			if (currentChildPtr == targetRoot) {
// 				// We found where the parent node is pointing to the target
// 				// node as its child. Replace that pointer with a null pointer.
// 				currentChildPtr = nullptr;
// 				// Flag that our search succeeded, for error checking.
// 				targetWasFound = true;
// 				// Stop looping early. The "break" statement exits the current "for"
// 				// loop and moves on to the next statement outside.
// 				break;
// 			}
// 		}
// 
// 		// If the target node was not found, our tree is malformed somehow.
// 		if (!targetWasFound) {
// 			// If this flag is still false, we have some kind of bug.
// 			// The target should have been listed as a child of its parent.
// 			constexpr char ERROR_MESSAGE[] = "Target node to delete was not listed as a child of its parent";
// 			std::cerr << ERROR_MESSAGE << std::endl;
// 			throw std::runtime_error(ERROR_MESSAGE);
// 		}
// 	}
// 
// 	// Now, we need to make sure all the descendents get deleted. We have to
// 	// think ahead about how to do this. Is there a specific order we must use
// 	// to delete items? For some class designs, if we delete elements in the
// 	// wrong order, we will break the internal structure of our class and
// 	// lose our record of where the disconnected pieces are. For example, if
// 	// we deleted our root node before recording where its children were, the
// 	// rest of the tree would remain in memory, but without any pointers
// 	// pointing to it. There would be no way to access the data or free the
// 	// memory until the entire program ends. That's a memory leak.
// 
// 	// On the other hand, even if we record where the root node's children are
// 	// before deleting the root node, we still might not want to immediately
// 	// delete the children. If the delete operation triggers a subtree removal
// 	// rooted at each child, as we have our destructor set up currently, this
// 	// would end up being recursive. This goes back to the question of whether
// 	// we are okay with using a recursive algorithm to traverse our entire tree.
// 	// In this function, we're trying to avoid that and use iteration instead.
// 
// 	// For our current data structure, let's try this method:
// 	// First, let's traverse the entire tree without deleting anything,
// 	// while writing down all the pointers that we find in a certain order.
// 	// For the ordering, we want to be able to delete the nodes from the bottom
// 	// of the tree upwards, so that children are deleted before their parents.
// 	// For example, a post-order traversal of the nodes would accomplish this.
// 
// 	// We can do this iteratively using two stack data structures. We'll use
// 	// one stack to record which nodes to "explore" next as we descend the tree,
// 	// one branch at a time; we will actually descend the rightmost branches
// 	// first, because if a given node's children are pushed onto the explore
// 	// stack from left to right, the rightmost child will be popped and explored
// 	// next. Meanwhile, each time we "explore" a node, we'll push it onto
// 	// a stack of nodes to be deleted at the very end. The leftmost branch's
// 	// leftmost child will be explored last and deleted first. This way, the
// 	// delete operations happen in post order from left to right.
// 
// 	// Our stack of nodes that still need to be explored (and then deleted)
// 	std::stack<TreeNode*> nodesToExplore;
// 
// 	// We also need a stack for the pointers that need to be deleted:
// 	std::stack<TreeNode*> nodesToDelete;
// 
// 	// To begin with, we'll record that the target node, which is the root of
// 	// the subtree, needs to be explored:
// 	nodesToExplore.push(targetRoot);
// 
// 	// Keep looping as long as there are nodes left to explore and delete.
// 	while (!nodesToExplore.empty()) {
// 
// 		// Get a copy of the top pointer on the explore stack.
// 		TreeNode* curNode = nodesToExplore.top();
// 
// 		// Now that we've retrieved the top pointer, we can pop it from the explore stack.
// 		nodesToExplore.pop();
// 
// 		if (showDebugMessages) {
// 			std::cerr << "Exploring node: ";
// 			if (curNode) {
// 				// if curNode isn't null, we can show what it contains
// 				std::cerr << curNode->data << std::endl;
// 			}
// 			else {
// 				std::cerr << "[null]" << std::endl;
// 			}
// 		}
// 
// 		// If nullptr...
// 		if (!curNode) {
// 			// The "continue" statement jumps to the top of the next iteration
// 			// of the while loop.
// 			continue;
// 		}
// 
// 		// Record that we need to delete this node later, by pushing it onto the delete stack.
// 		nodesToDelete.push(curNode);
// 
// 		// Loop through the current node's children pointers from first to last,
// 		// which we interpret as left to right
// 		for (auto childPtr : curNode->childrenPtrs) {
// 			// Push a copy of the child pointer onto the stack of children to explore
// 			nodesToExplore.push(childPtr);
// 		}
// 
// 	} // End of explore loop.
// 
// 	  // We're done exploring all the nodes in the tree now, so now we need
// 	  // to delete the nodes one at a time from the delete stack.
// 	while (!nodesToDelete.empty()) {
// 
// 		// Get a copy of the top pointer on the delete stack.
// 		TreeNode* curNode = nodesToDelete.top();
// 
// 		// Now that we've retrieved the top pointer, we can pop it from the stack.
// 		nodesToDelete.pop();
// 
// 		if (showDebugMessages) {
// 			std::cerr << "Deleting node: ";
// 			if (curNode) {
// 				// if curNode isn't null, we can show what it contains
// 				std::cerr << curNode->data << std::endl;
// 			}
// 			else {
// 				std::cerr << "[null]" << std::endl;
// 			}
// 		}
// 
// 		// Delete the current node pointer.
// 		delete curNode;
// 
// 		// As a good practice, we'll try to set pointers to nullptr after we
// 		// delete them. This is somewhat unnecessary here, because this local
// 		// variable is about to go out of scope when the while loop iterates,
// 		// but it can help you catch mistakes in many other situations.
// 		curNode = nullptr;
// 
// 		// You want to avoid deleting the same memory address more than once
// 		// by mistake, or dereferencing an address that is no longer allocated.
// 		// After setting a pointer to nullptr:
// 		// If you "delete" a nullptr, it's safe; nothing will happen.
// 		// If you dereference a nullptr, the program will crash, and
// 		// you'll quickly discover the source of the problem.
// 	}
// 
// 	// If we deleted the root node of this class instance,
// 	//  then reset the root pointer.
// 	if (targetingWholeTreeRoot) {
// 		rootNodePtr = nullptr;
// 	}
// 
// 	return;
// }
// 
// template <typename T>
// void GenericTree<T>::compress() {
// 
// 	// We'll use an iterative approach to traversing the tree here.
// 	// In this function, we don't ever want to push null pointers onto the
// 	// exploration queue. Some exploration techniques do push null pointers, as
// 	// long as they have a robust base case to handle "exploring" a null pointer,
// 	// but this time, whenever we explore a new node, we're going to make a
// 	// compressed copy of its vector of children pointers that has all the null
// 	// pointers removed.
// 
// 	// If there's no root, return.
// 	if (!rootNodePtr) return;
// 
// 	// Queue of node pointers that we still need to explore (constructed empty)
// 	std::queue<TreeNode*> nodesToExplore;
// 
// 	// Begin by pushing our root pointer onto the queue
// 	nodesToExplore.push(rootNodePtr);
// 
// 	// Loop while there are still nodes to explore
// 	while (!nodesToExplore.empty()) {
// 
// 		// Make a copy of the front pointer on the queue, then pop it to decrease the queue
// 		TreeNode* frontNode = nodesToExplore.front();
// 		nodesToExplore.pop();
// 
// 		if (!frontNode) {
// 			// The front node pointer should not be null, because we're designing this
// 			// function so that no null pointers should ever get pushed onto the exploration queue.
// 			throw std::runtime_error("Error: Compression exploration queued a null pointer");
// 		}
// 
// 		// If the node exists, it may have children pointers. Let's make
// 		// an empty vector of children node pointers and get ready to make
// 		// a compressed copy of this node's children pointers.
// 		std::vector<TreeNode*> compressedChildrenPtrs;
// 		// Now loop through the currently recorded children pointers...
// 		for (auto childPtr : frontNode->childrenPtrs) {
// 			if (childPtr) {
// 				// If this child pointer is not null, then push it onto the back
// 				// of our new, compressed pointers vector.
// 				compressedChildrenPtrs.push_back(childPtr);
// 				// Also put this child pointer onto the end of the exploration queue.
// 				nodesToExplore.push(childPtr);
// 			}
// 		}
// 
// 		// Here's a little trick: the std::vector::swap() function lets us replace
// 		// the node's actual children pointer vector with the new one, even
// 		// though the compressed one is a local variable here. The standard
// 		// template library swaps the internals of the two structures so that
// 		// the old one expires here at local scope, while the new one lives on
// 		// with our node.
// 		frontNode->childrenPtrs.swap(compressedChildrenPtrs);
// 	}
// 
// }
// 
// template <typename T>
// std::ostream& GenericTree<T>::Print(std::ostream& os) const {
// 
// 	// 层级打印
// 	//实现：preorder+STL
// 	const TreeNode* rootNodePtr = this->rootNodePtr;    // Now that the pointer is const, if we tried to do this, the compiler would give an error and stop us: rootNodePtr->data = "This won't compile";
// 
// 	if (nullptr == rootNodePtr) {
// 		return os << "[empty tree]" << std::endl;
// 	}
// 
// 	std::stack<const TreeNode*> nodesToExplore;//打印栈
// 	nodesToExplore.push(rootNodePtr);
// 	std::stack<int> depthStack;//深度关联
// 	depthStack.push(0);
// 
// 
// 	//图形显示用的，边界条框// true是边界，false是空格
// 	std::stack< std::vector<bool> > curMarginStack;
// 	curMarginStack.push(std::vector<bool>());// 0层什么都不用
// 	// Each node's margin graphics will be based on the trailing lines that are still running parallel in the margin.
// 	std::stack< std::vector<bool> > trailingMarginStack;
// 	trailingMarginStack.push(std::vector<bool>());
// 
// 	//下边是explore过程：个人理解：前序遍历，根据深度打印出不一样的条形和空格
// 	//利用栈完成遍历
// 	//是栈，不是队列，所以先推进去右下角
// 	// 所以，怎么逆向迭代
// 	//[]的越界问题，用.at()可以解决，但是还可以用iterator和reverse iterator，rbegin()， rend()
// 	// // 反向遍历孩子
// //     auto curNode = rootNodePtr;
// //     for (int i = curNode->childrenPtrs.size() - 1; i>=0; i--) {
// //         auto res = curNode->childrenPtrs[i];
// //         std::cout << res->data << std::endl;
// //     }
// 
// 
// 	while (!nodesToExplore.empty()) {
// 		const TreeNode* curNode = nodesToExplore.top();
// 		nodesToExplore.pop();
// 		int curDepth = depthStack.top();//深度联动获得
// 		depthStack.pop();
// 
// 		std::vector<bool> curMargin = curMarginStack.top();//画线
// 		curMarginStack.pop();
// 		std::vector<bool> trailingMargin = trailingMarginStack.top();//空格？
// 		trailingMarginStack.pop();
// 
// 		if (showDebugMessages) {
// 			// Simplified numerical output for debugging.
// 			os << "Depth: " << curDepth;
// 			std::cerr << " Data: ";
// 			if (curNode) {
// 				// if curNode isn't null, we can show what it contains
// 				std::cerr << curNode->data << std::endl;
// 			}
// 			else {
// 				std::cerr << "[null]" << std::endl;
// 			}
// 		}
// 		else {
// 			constexpr int LAST_ROW = 2;
// 
// 			for (int row = 1; row <= LAST_ROW; row++) {
// 				// Iterate forward through the margin display flags to fill in the margin.
// 				for (auto stemIt = curMargin.begin(); stemIt != curMargin.end(); stemIt++) {
// 
// 					bool showStem = *stemIt;
// 					std::string stemSymbol = "|";
// 					if (!showStem) {
// 						stemSymbol = " ";
// 					}
// 
// 					bool isLastCol = false;
// 					if (stemIt + 1 == curMargin.end()) {
// 						isLastCol = true;
// 					}
// 
// 					if (isLastCol) {
// 						if (LAST_ROW == row) {
// 							// The stem before the data item should always be "|_ " in effect:
// 							os << stemSymbol << "_ ";
// 						}
// 						else if (showStem) {
// 							// Display a stem and a newline
// 							os << stemSymbol << std::endl;
// 						}
// 						else {
// 							// Don't bother displaying trailing spaces before the newline
// 							os << std::endl;
// 						}
// 					}
// 					else {
// 						// Display a stem (or a blank) and some padding spaces
// 						os << stemSymbol << "  ";
// 					}
// 
// 					// Bottom of loop for margin stems
// 				}
// 
// 				// Bottom of loop for multi-row display
// 			}
// 
// 			// At the end of the second row, output the data. The root node data
// 			// is displayed alone on the first line correctly.)
// 			if (curNode) {
// 				os << curNode->data << std::endl;
// 			}
// 			else {
// 				os << "[null]" << std::endl;;
// 			}
// 
// 		}
// 
// 		// If this node is non-null and has any children...
// 		if (curNode && curNode->childrenPtrs.size() > 0) {
// 			for (auto it = curNode->childrenPtrs.rbegin(); it != curNode->childrenPtrs.rend(); it++) {//反向++
// 				const TreeNode* childPtr = *it;
// 				nodesToExplore.push(childPtr);//子节点指针入栈
// 				depthStack.push(curDepth + 1);//深度随动
// 
// 				auto nextMargin = trailingMargin;// Prepare a working copy of the margin for the node we're pushing.
// 
// 				nextMargin.push_back(true);// All nodes get an extra stem glyph next to their printout.
// 				curMarginStack.push(nextMargin);
// 
// 				// But for the trailing margin, we need to leave the rightmost child with a blank trailing in the margin, because it's displayed lowest.
// 				auto nextTrailingMargin = trailingMargin;
// 				if (curNode->childrenPtrs.rbegin() == it) {
// 					// This is the rightmost child. Leave a blank trailing.
// 					nextTrailingMargin.push_back(false);
// 				}
// 				else {
// 					// Other children leave a vertical stem symbol trailing in the margin.
// 					nextTrailingMargin.push_back(true);
// 				}
// 				trailingMarginStack.push(nextTrailingMargin);
// 			}
// 		}
// 	}
// 	return os;
// }
// 
// template <typename T>
// int GenericTree<T>::node_segmentation(std::string input_file_name, int segment_level) {
// 
// 	quad_tree_node_data s;
// 	s.file_name_obj = input_file_name;
// 	createRoot(s);
// 
// 	Segmenter segmenter;
// 
// 	TreeNode* rootNodePtr = this->getRootPtr();
// 
// 	if (nullptr == rootNodePtr || segment_level <= 0) {
// 		return -1;
// 	}
// 	if (0)//todo:删除
// 		rootNodePtr->data.file_name_obj = input_file_name;//初始化
// 
// 	std::queue<std::pair<TreeNode*, int>> node_to_process;
// 	node_to_process.push(std::make_pair(rootNodePtr, segment_level));
// 
// 	while (!node_to_process.empty()) {
// 		auto curr_data = node_to_process.front();
// 		auto curr_node = curr_data.first;
// 		int level = curr_data.second;
// 		node_to_process.pop();
// 
// 		auto segment = [](std::string& str) {
// 			std::vector<std::string> res;
// 			for (int i = 0; i < 4; ++i) {
// 				res.push_back(str + std::to_string(i));
// 			}
// 			return res;
// 		};
// 		std::string pathExport = "F:/handwork_data/data_out/27_MJSJHY/";//todo:暴露接口，或者在树的节点内储存信息
// 		//std::vector<std::string> res
// 		std::unordered_map<std::string, std::vector<double>> res = segmenter.segmentMeshWithSpatialBBox_new(curr_node->data.file_name_obj, pathExport);//上一次的文件名
// 		//std::vector<std::string> res = segment(curr_node->data);
// 		for (auto it = res.begin(); it != res.end(); it++) {
// 			//for (int i = 0; i < res.size(); ++i) {
// 			quad_tree_node_data s;
// 			s.file_name_obj = it->first;
// 
// 			auto node_inserted = curr_node->addChild(s);
// 			if (level > 1)
// 				node_to_process.push(std::make_pair(node_inserted, level - 1));
// 		}
// 
// 	}
// 
// 
// 
// }