//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// tree.cpp - checks the tree class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_tree_.h>
#include <_string_.h>

using namespace soige;

void check_tree();

int main(int argc, char* argv[])
{
	printf("Checking _tree_\n");
	check_tree();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// tree tests
void check_tree()
{
	_tree_<_string_> str_tree;
	typedef _tree_<_string_>::_node_ str_node;
	_string_ path;
	str_node node;
	ulong c;

	path = str_tree.insertNode(str_node(_string_("Zabba"))).getPath();
	path = str_tree.insertNode(str_node(_string_("Abba"))).getPath();
	path = str_tree.insertNode(str_node(_string_("Labba"))).getPath();
	path = str_tree.insertNode(str_node(_string_("Babba"))).getPath();
	
	str_tree.getRoot().sortChildren();
	str_node& rnode = str_tree.getNode(_string_("//"));
	path = rnode.getChild(0).getPath();
	path = rnode.getChild(1).getPath();
	path = rnode.getChild(2).getPath();
	path = rnode.getChild(3).getPath();
	str_tree.getRoot().reverseChildren();
	path = rnode.getChild(0).getPath();
	path = rnode.getChild(1).getPath();
	path = rnode.getChild(2).getPath();
	path = rnode.getChild(3).getPath();
	node.value = _string_("He-he-he");
	str_tree.getNode(path).appendChild(node);
	node.value = _string_("Blah-blah-blah");
	str_tree.getNode(path).insertChild(node, 0);
	str_tree.getNode(path).reverseChildren();
	node.value = _string_("Labba");
	path = str_tree.getRoot().findChild(node, true);
	if(path.length())
		str_tree.removeNode(path);
	c = str_tree.totalCount();
	node.value = _string_("Blah-blah-blah");
	path = str_tree.getRoot().findChild(node, true);
	if(path.length())
		str_tree.removeNode(path);
	c = str_tree.totalCount();
	node = str_tree.getNode(_string_("//2"));
	str_tree.getNode(_string_("//2/0")).appendChild(node);
	c = str_tree.totalCount();
	c = str_tree.getNode(_string_("//2/0")).getChildCount();
	c = str_tree.getNode(_string_("//2/0")).getHierarchyChildCount();
	path = str_tree.getNode(_string_("//2/0")).findChild(str_node(_string_("He-he-he")), false);
	path = str_tree.getNode(_string_("//2/0")).findChild(str_node(_string_("He-he-he")), true);
	bool b = str_tree.getNode(_string_("//2/0/0")).isChildOf( str_tree.getNode(_string_("//2/0")) );
	b = str_tree.getNode(_string_("//2/0/0")).isParentOf( str_tree.getNode(_string_("//2/0")) );
	b = str_tree.getNode(_string_("//0")).isSiblingOf( str_tree.getNode(_string_("//2")) );
	str_tree = str_tree;
	c = str_tree.totalCount();
	str_tree.removeAllNodes();
}

