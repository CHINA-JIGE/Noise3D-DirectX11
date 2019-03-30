
/***********************************************************************

								 h£ºGeneralTreeNode
			 n-ary Tree data structure template. only header is needed.

************************************************************************/
#pragma once

namespace Noise3D
{
	//a more detailed node like SceneNode or BvhNode should be derived from TreeNodeTemplate<derivedNode_t>
	//a compile-time check is implemented using std::is_base_of
	//if derivedNode_t is not derived from TreeNodeTemplate<derivedNode_t>, then there will be an LINKING error

	//general n-ary tree node's implementation. but 'value' field is not contained in this base class
	//can be derived and let the derived node type have those common operation of tree.
	template <class derivedNode_t, class derivedTree_t>
	class TreeNodeTemplate
	{
	public:

		TreeNodeTemplate() :
			m_pFatherNode(nullptr),
			m_pHostTree(nullptr)
		{
			//a more detailed node like SceneNode or BvhNode should be derived from TreeNodeTemplate<derivedNode_t>
			//try to instantiate a template. if is_base_of return false, then instantiation failed, compile error
			//(more specifically, a LINK error)

			mFunc_CompileTime_NodeTypeInheritanceCheck<std::is_base_of<TreeNodeTemplate<derivedNode_t,derivedTree_t>,derivedNode_t>::value>();
			mFunc_CompileTime_NodeTypeInheritanceCheck<std::is_base_of<TreeTemplate<derivedNode_t, derivedTree_t>, derivedTree_t>::value>();

			/*								   management
				derivedNode_t		-------------------  derivedTree_t
						|														|
						| inherit											|
			TreeNodeTemplate<>						TreeTemplate<>
			
			*/


			//(2019.3.15)perhaps using static_assert() is also a good choice.
			/*static_assert(std::is_base_of<TreeNodeTemplate<derivedNode_t, derivedTree_t>, derivedNode_t>::value,
			"TreeNodeTemplate : derivedNode_t should inherit from TreeNodeTemplate.");

			static_assert(std::is_base_of<TreeTemplate<derivedNode_t, derivedTree_t>, derivedTree_t>::value,
			"TreeNodeTemplate : derivedTree_t should inherit from TreeTemplate.");*/

		}

		TreeNodeTemplate(const TreeNodeTemplate& rhs)
		{
			this->mChildNodeList = rhs.mChildNodeList;//deep copy
			this->m_pFatherNode = rhs.m_pFatherNode;
			this->m_pHostTree = rhs.m_pHostTree;
		}

		//father node
		derivedNode_t* GetFatherNode()const
		{
			return m_pFatherNode;
		}

		//attach current node to parent
		void	AttachToFatherNode(derivedNode_t* pNode)
		{
			//if it's not polymorphic, then shit could happen and dynamic_cast will fail
			//if(derivedNode_t* pDerivedNode = dynamic_cast<derivedNode_t*>(this))
			//{
				pNode->AttachChildNode((derivedNode_t*)this);
			//}

		};

		//attach to child node
		void	AttachChildNode(derivedNode_t* pNode)
		{
			if (pNode != nullptr)
			{
				//delete pNode's original father's ref to pNode
				derivedNode_t* pOriginalParent = pNode->m_pFatherNode ;
				if (pOriginalParent != nullptr)
				{
					auto iter_ChildNodeRef = std::find(pOriginalParent->mChildNodeList.begin(), pOriginalParent->mChildNodeList.end(), pNode);
					if (iter_ChildNodeRef != pOriginalParent->mChildNodeList.end())
					{
						pOriginalParent->mChildNodeList.erase(iter_ChildNodeRef);
					}
				}

				pNode->m_pFatherNode = (derivedNode_t* )this;
				mChildNodeList.push_back(pNode);
			}
		};

		//...
		bool IsAttachedToFatherNode()const 
		{
			return (m_pFatherNode != nullptr);
		}

		//directly create a new scene node and attach to current node
		derivedNode_t* CreateChildNode() 
		{
			//if it's not polymorphic, then this expansion will fail
			derivedNode_t* pNewChild = new derivedNode_t;
			pNewChild->m_pHostTree = this->m_pHostTree;
			TreeNodeTemplate::AttachChildNode(pNewChild);
			return pNewChild;
		}

		//get child node via index
		derivedNode_t* GetChildNode(uint32_t index) const 
		{
			if (index < mChildNodeList.size())
			{
				return mChildNodeList[index];
			}
			return nullptr;
		}

		//how many attached child nodes
		uint32_t GetChildNodeCount() const
		{
			return mChildNodeList.size();
		}

		//get the tree it belongs to
		derivedTree_t* GetHostTree() const
		{
			return m_pHostTree;
		}

		//determine if it's leaf node
		bool IsLeafNode() const
		{
			return mChildNodeList.empty();
		}

	protected:

		//only general tree has the permission to delete node(like scene graph's root can't be deleted directly)
		~TreeNodeTemplate() {};

		//friend all template instance and friend derived type
		template<typename node_t, typename tree_t> friend class TreeTemplate;
		friend derivedTree_t;

		 //a more detailed node like SceneNode or BvhNode should be derived from TreeNodeTemplate<derivedNode_t>
		//mFunc_CompileTime_NodeTypeInheritanceCheck<false> shall NOT be implemented to
		//DELIBERATELY cause a LINKING ERROR
		template <bool isDerivedTypeActuallyDerivedFromTreeNodeTemplate> 
		void mFunc_CompileTime_NodeTypeInheritanceCheck();

		template <>
		void mFunc_CompileTime_NodeTypeInheritanceCheck<true>() {};

		//template <>
		//void mFunc_CompileTime_NodeTypeInheritanceCheck<false>() {};

		std::vector<derivedNode_t*> mChildNodeList;

		derivedNode_t* m_pFatherNode;

		//the tree it belongs to (initialized at creation (but not at constructor))
		derivedTree_t* m_pHostTree;

	};

	//general n-ary tree's implementation. based on TreeNodeTemplate class
	template <class derivedNode_t, class derivedTree_t>
	class TreeTemplate
	{
	public:

		TreeTemplate() :
			m_pRoot(new derivedNode_t)
		{
			m_pRoot->m_pHostTree = static_cast<derivedTree_t*>(this);
		}

		//destructor, delete all nodes
		~TreeTemplate()
		{
			//this remove function instance allowed to remove root
			mFunc_Remove<true>(m_pRoot);
		}

		//start from given node. Append to traverse result to ref output(doesn't clear the list)
		void Traverse_PreOrder(derivedNode_t* pNode,std::vector<derivedNode_t*>& outResult)const
		{
			if (pNode == nullptr)return;

			outResult.push_back(pNode);
			for (auto pn : pNode->mChildNodeList)
			{
				// nullptr's check will be done within the recursive call
				TreeTemplate::Traverse_PreOrder(pn, outResult);
			}
		}

		//start from given node. Append to traverse result to ref output(doesn't clear the list)
		void Traverse_PostOrder(derivedNode_t* pNode, std::vector<derivedNode_t*>& outResult)const
		{
			if (pNode == nullptr)return;

			for (auto pn : pNode->mChildNodeList)
			{
				// nullptr's check will be done within the recursive call
				TreeTemplate::Traverse_PostOrder(pn, outResult);
			}
			outResult.push_back(pNode);
		}

		//start from given node. Append to traverse result to ref output(doesn't clear the list)
		void Traverse_LayerOrder(derivedNode_t* pNode, std::vector<derivedNode_t*>& outResult)const
		{
			if (pNode == nullptr)return;

			//Breath First Search(BFS), stack needed, no recursive solution(?)
			std::queue<derivedNode_t*> queue;
			queue.push(pNode);

			//search until all nodes are popped up
			while (!queue.empty())
			{
				derivedNode_t* pStackTopNode = queue.front();
				outResult.push_back(pStackTopNode);
				queue.pop();//pop the head of queue

							//push child to queue tail
				for (auto pn : pStackTopNode->mChildNodeList)
				{
					if (pn != nullptr)queue.push(pn);
				}
			}
		}

		//start from root. Append to traverse result to ref output(doesn't clear the list)
		//identical to Traverse_XXX(root, outResult)
		void Traverse_PreOrder(std::vector<derivedNode_t*>& outResult)const
		{
			TreeTemplate::Traverse_PreOrder(m_pRoot, outResult);
		}

		//start from root. Append to traverse result to ref output(doesn't clear the list)
		void Traverse_PostOrder(std::vector<derivedNode_t*>& outResult)const
		{
			TreeTemplate::Traverse_PostOrder(m_pRoot, outResult);
		}

		//start from root. Append to traverse result to ref output(doesn't clear the list)
		void Traverse_LayerOrder(std::vector<derivedNode_t*>& outResult)const
		{
			TreeTemplate::Traverse_LayerOrder(m_pRoot, outResult);
		}

		//traverse the path from current node to root (inclusive)(can be used in scenario like scene graph world transform eval)
		void TraversePathToRoot(derivedNode_t* pStartNode, std::vector<derivedNode_t*>& outResult)const
		{
			if (pStartNode == nullptr)return;
			if (pStartNode->GetHostTree() != this)
			{
				ERROR_MSG("TreeTemplate : given node doesn't belong to current tree.");
				return;
			}

			derivedNode_t* pTmpNode = pStartNode;
			try
			{
				//when pTmpNode==root, continue. then ptr will become nullptr, exit.
				do
				{
					outResult.push_back(pTmpNode);
					pTmpNode = pTmpNode->GetFatherNode();
				} while (pTmpNode != nullptr);
			}
			catch (std::exception e)
			{
				ERROR_MSG(std::string("TreeTemplate: father node access violation. current node") + std::to_string((uint32_t)pTmpNode));
			}
		}

		//recursively delete nodes(current and its childrens). root can't be removed
		void Remove(derivedNode_t* pNode)
		{
			//not allowed to remove root
			mFunc_Remove<false>(pNode);
		}

		//delete all nodes except root. and reset the root
		void Reset()
		{
			mFunc_Remove<false>(m_pRoot);
			m_pRoot->mChildNodeList.clear();
			m_pRoot->m_pFatherNode = nullptr;
		}

		//get the root of the tree
		derivedNode_t* GetRoot()const
		{
			return m_pRoot;
		}

	protected:
		//friend all template instance and friend derived type
		template<typename node_t, typename tree_t> friend class TreeTemplate;
		friend derivedTree_t;

		//recursively delete nodes(current and its childrens). root can't be removed in run-time
		template<bool permittedToRemoveRoot>
		void mFunc_Remove(derivedNode_t* pNode)
		{
			//root can be removed only in destructor
			if (!permittedToRemoveRoot)
			{
				if (pNode == m_pRoot)
				{
					//WARNING_MSG("Not allowed to manually delete Tree's root.");
					return;
				}
			}

			std::vector<derivedNode_t*> list;

			//remove pNode's ref from its father's 'ChildNodeList'
			derivedNode_t* pFatherNode = pNode->GetFatherNode();
			if (pFatherNode != nullptr)
			{
				auto iter_ChildNodeRef = std::find(pFatherNode->mChildNodeList.begin(), pFatherNode->mChildNodeList.end(), pNode);
				pFatherNode->mChildNodeList.erase(iter_ChildNodeRef);
			}

			//traverse sub-tree starting from pNode
			TreeTemplate::Traverse_PostOrder(pNode, list);

			//delete current and all children nodes including pNode
			//and remember to remove them from 'childNodeList'
			for (auto pn : list)delete pn;
		}

		//remove father's reference to child node (remove record in father's ChildNodeList)
		void mFunc_FindAndDeleteRefToChildNode(derivedNode_t* pFather, derivedNode_t* pChild)
		{
			if (pFather != nullptr)
			{
				//search child in father's ref list
				auto iter_ChildNodeRef = std::find(pFather->mChildNodeList.begin(), pFather->mChildNodeList.end(), pChild);
				pFather->mChildNodeList.erase(iter_ChildNodeRef);
			}
		}

		derivedNode_t* m_pRoot;

	};
}
