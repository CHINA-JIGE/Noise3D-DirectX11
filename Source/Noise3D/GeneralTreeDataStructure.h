
/***********************************************************************

								 h£ºGeneralTreeNode
			 n-ary Tree. Including several 

************************************************************************/
#pragma once

namespace Noise3D
{
	//general n-ary tree node's implementation. but 'value' field is not contained in this base class
	//need to derived and implement a more specific kind of node.
	class GeneralTreeNode
	{
	public:

		GeneralTreeNode();

		GeneralTreeNode(const GeneralTreeNode& rhs);

		GeneralTreeNode* GetParentNode();

		void	AttachToParentNode(GeneralTreeNode* pNode);//attach current node to parent

		void	AttachChildNode(GeneralTreeNode* pNode);//attach to child node

		bool IsAttachedToParentNode();

		GeneralTreeNode* CreateChildNode();//directly create a new scene node and attach to current node

		GeneralTreeNode* GetChildNode(int index);

		uint32_t GetChildNodeCount();

	protected:

		friend class GeneralTree;

		//only general tree has the permission to delete node(like scene graph's root can't be deleted directly)
		~GeneralTreeNode();

		std::vector<GeneralTreeNode*> mChildNodeList;

		GeneralTreeNode* m_pParentNode;

	};

	//general n-ary tree's implementation. based on GeneralTreeNode class
	//template <typename object_t>
	class GeneralTree
	{
	public:

		GeneralTree();

		//delete all nodes
		~GeneralTree();

		//start from given node. Append to traverse result to ref output(doesn't clear the list)
		void Traverse_PreOrder(GeneralTreeNode* pNode,std::vector<GeneralTreeNode*>& outResult);

		void Traverse_PostOrder(GeneralTreeNode* pNode, std::vector<GeneralTreeNode*>& outResult);

		void Traverse_LayerOrder(GeneralTreeNode* pNode, std::vector<GeneralTreeNode*>& outResult);

		//start from root. Append to traverse result to ref output(doesn't clear the list)
		//identical to Traverse_XXX(root, outResult)
		void Traverse_PreOrder(std::vector<GeneralTreeNode*>& outResult);

		void Traverse_PostOrder(std::vector<GeneralTreeNode*>& outResult);

		void Traverse_LayerOrder(std::vector<GeneralTreeNode*>& outResult);

		//recursively delete nodes(current and its childrens). root can't be removed
		void Remove(GeneralTreeNode* pNode);

		GeneralTreeNode* GetRoot();

	private:

		//recursively delete nodes(current and its childrens). root can't be removed
		template<bool permittedToRemoveRoot>
		void mFunc_Remove(GeneralTreeNode* pNode);

		void mFunc_FindAndDeleteRefToChildNode(GeneralTreeNode* pFather, GeneralTreeNode* pChild);

		GeneralTreeNode* m_pRoot;

	};
}
