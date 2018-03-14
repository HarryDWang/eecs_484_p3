#include "BTree.h"                                      // file-specific header
#include "DataEntry.h"                                  // for DataEntry
#include "LeafNode.h"                                   // for LeafNode
#include "TreeNode.h"                                   // for TreeNode
#include "Utilities.h"                                  // for Key alias
#include <cassert>                                      // for assert
#include <iostream>                                     // for ostream
#include <vector>                                       // for vector

using std::vector;
using std::ostream;


// constructor; root begins as empty leaf node
BTree::BTree()
    : root{ new LeafNode{} }, height{ 0 }, size{ 0 } {}

// destructor
BTree::~BTree() {
    delete root;
}

// return height
size_t BTree::getHeight() const {
    return height;
}

// return number of entries
size_t BTree::getSize() const {
    return size;
}

void BTree::insertEntry(const DataEntry& newEntry) {
    // TO DO: implement this function
    TreeNode *node;
    if(!this->root->contains(Key(newEntry))){
        node = this->root->insertIntoRoot(newEntry);
        if(this->root != node){
            this->root = node;
            this->height++;
        }
        
        this->size++;
    }    
}

void BTree::deleteEntry(const DataEntry& entryToRemove) {
    // TO DO: implement this function
}

vector<DataEntry> BTree::rangeFind(const Key& begin, const Key& end) const {
    // TO DO: implement this function
    return vector<DataEntry>{};
}

// print tree
void BTree::print(ostream& os) const {
    os << kPrintPrefix << "Height = " << height << "  |  Size = " << size << "\n";
    os << kPrintPrefix << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    root->print(os);
}
