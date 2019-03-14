#include <iostream>
#include <vector>
#include "GeneralTreeDataStructure.h"

using namespace Noise3D;

int main()
{
	GeneralTree t;
	GeneralTreeNode* a = t.GetRoot()->CreateChildNode();
		GeneralTreeNode* aa = a->CreateChildNode();
		GeneralTreeNode* ab = a->CreateChildNode();
		GeneralTreeNode* ac = a->CreateChildNode();
			GeneralTreeNode* aba = ab->CreateChildNode();
			GeneralTreeNode* ad = a->CreateChildNode();
				GeneralTreeNode* ada = ad->CreateChildNode();
					GeneralTreeNode* adaa = ada->CreateChildNode();
					GeneralTreeNode* adb = ad->CreateChildNode();
	GeneralTreeNode* b = t.GetRoot()->CreateChildNode();
		GeneralTreeNode* ba = b->CreateChildNode();
		GeneralTreeNode* bb = b->CreateChildNode();
			GeneralTreeNode* bba = bb->CreateChildNode();
				GeneralTreeNode* bbaa = bba->CreateChildNode();

	const uint32_t modNum = 6234;
	std::vector<GeneralTreeNode*> list;

	/*
	std::cout << "root(pre):" <<std::endl;
	t.Traverse_PreOrder(list);
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_PreOrder(a, list);
	std::cout << "a(pre):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_PreOrder(ad, list);
	std::cout << "ad(pre):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_PreOrder(b, list);
	std::cout << "b(pre):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_PostOrder(list);
	std::cout << "root(post):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_PostOrder(a, list);
	std::cout << "a(post):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_PostOrder(ad, list);
	std::cout << "ad(post):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_PostOrder(b, list);
	std::cout << "b(post):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_LayerOrder(list);
	std::cout << "root(layer):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_LayerOrder(a, list);
	std::cout << "a(layer):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	t.Traverse_LayerOrder(bb, list);
	std::cout << "bb(layer):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();*/

	t.Traverse_LayerOrder(list);
	std::cout << "root(layer):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	//remove
	t.Remove(ad);
	t.Traverse_LayerOrder(list);
	std::cout << "ad removed(layer):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	//remove root
	t.Remove(t.GetRoot());
	t.Traverse_LayerOrder(list);
	std::cout << "try to remove root(layer):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();

	//re-attach
	b->AttachToParentNode(a);
	t.Traverse_LayerOrder(list);
	std::cout << "b attach to a(layer):" << std::endl;
	for (auto pn : list)std::cout << uint32_t(pn) % modNum << " ";
	std::cout << std::endl;
	list.clear();



	system("pause");
}