#include "DataEntry.h"                                  // for DataEntry
#include "InnerNode.h"                                  // for InnerNode
#include "LeafNode.h"                                   // file-specific header
#include "TreeNode.h"                                   // for TreeNode
#include "Utilities.h"                                  // for size constants, print prefix, Key alias
#include <algorithm>                                    // for find
#include <cassert>                                      // for assert
#include <iostream>                                     // for ostream
#include <numeric>                                      // for numeric_limits
#include <string>                                       // for string
#include <vector>                                       // for vector

using std::find;
using std::vector;
using std::numeric_limits;
using std::ostream;
using std::string;


// constructor
LeafNode::LeafNode(InnerNode* parent)
: TreeNode{ parent }, entries{}, leftNeighbor{nullptr}, rightNeighbor{nullptr} {}

void LeafNode::setEntries(LeafNode *ln,vector<DataEntry>entriesIn){
    for(unsigned i = 0; i < entriesIn.size(); ++i){
        ln->entries.push_back(entriesIn[i]);
    }
}

void LeafNode::setNeighborsToNull(){
    this->rightNeighbor = nullptr;
    this->leftNeighbor = nullptr;
}




// print keys of data entries surrounded by curly braces, ending
// newline
void LeafNode::print(ostream& os, int indent) const {
    assert(indent >= 0);
    
    os << kPrintPrefix << string(indent, ' ') << "{ ";
    for (const auto& entry : entries) {
        if (entry != entries[0]) {
            os << " | ";
        }
        os << entry;
    }
    os << " }\n";
    
    assert(satisfiesInvariant());
}

// data entries are sorted; minimum is first entry's key
Key LeafNode::minKey() const {
    if (entries.empty()) {
        return numeric_limits<Key>::min();
    }
    return entries.front();
}

// data entries are sorted; maximum is last entry's key
Key LeafNode::maxKey() const {
    if (entries.empty()) {
        return numeric_limits<Key>::max();
    }
    return entries.back();
}

// TRUE if key is the key of any entry
bool LeafNode::contains(const Key& key) const {
    return (find(entries.cbegin(), entries.cend(), key) != entries.cend());
}

// TRUE if this node is the target
bool LeafNode::contains(const TreeNode* node) const {
    return (this == node);
}

// return the data entry with given key
const DataEntry& LeafNode::operator[](const Key& key) const {
    assert(contains(key));
    
    return *find(entries.cbegin(), entries.cend(), key);
}

vector<DataEntry> LeafNode::rangeFind(const Key& begin, const Key& end) const {
    // TO DO: implement this function
    assert(begin <= end);
    
    auto leaf = this;
    vector<DataEntry> vec;
    while (leaf) {
        for (auto& idx : leaf->entries) {
            Key key = Key(idx);
            
            if (key >= begin && end >= key){
                vec.push_back(idx);
            }
            if (end <= key){
                return vec;
            }
        }
        leaf = leaf->rightNeighbor;
    }
    return vec;
}

// use generic delete; height can't decrease
TreeNode* LeafNode::deleteFromRoot(const DataEntry& entryToRemove) {
    assert(contains(entryToRemove));
    assert(!getParent());
    
    deleteEntry(entryToRemove);
    assert(satisfiesInvariant());
    return this;
}

void LeafNode::insertEntry(const DataEntry& newEntry) {
    // TO DO: implement this function
    
    //check if entry is already in the tree
    if(this->contains(Key(newEntry))){
        return;
    }
    
    //case where leaf node is full
    if(entries.size() >= 2*kLeafOrder){
//        
//        LeafNode *newLeaf = new LeafNode(this->getParent());
        LeafNode *newLeaf = new LeafNode{nullptr};
        if(this->rightNeighbor != nullptr){
            this->rightNeighbor->leftNeighbor = newLeaf;
            newLeaf->rightNeighbor = this->rightNeighbor;
        }

        this->rightNeighbor = newLeaf;
        newLeaf->leftNeighbor = this;
        newLeaf->updateParent(this->getParent());
        
        vector<DataEntry>rightHalf_vector;
        //create second half
        for(unsigned long i = kLeafOrder; i < entries.size(); ++i){
            rightHalf_vector.push_back(entries[i]);
        }
        //split first half
        for(unsigned long i = kLeafOrder; i < 2*kLeafOrder; ++i){
            this->entries.pop_back();
        }
        
        //put DataEntry into correct spot
        if(newEntry >= rightHalf_vector[0]){
            auto upper_bound = std::upper_bound(rightHalf_vector.begin(),rightHalf_vector.end(),newEntry);
            rightHalf_vector.insert(upper_bound,newEntry);
        }
        else{
            auto upper_bound = std::upper_bound(entries.begin(),entries.end(),newEntry);
            entries.insert(upper_bound,newEntry);
        }
        
        setEntries(newLeaf,rightHalf_vector);
        
        if(this->getParent()){
            this->getParent()->insertChild(newLeaf,(Key)newLeaf->entries[0]);
        }
        else{
            InnerNode *newParent = new InnerNode(this,newLeaf->entries[0],newLeaf);
            this->updateParent(newParent);
            newLeaf->updateParent(newParent);
            
        }
    }
    
    //case where leaf node is not full
    else{
        auto upper_bound = std::upper_bound(entries.begin(),entries.end(),newEntry);
        entries.insert(upper_bound,newEntry);
    }
    
    
}
//PROBLEM!!!!!!!!! check piazza post 1110 for failed test case
//must update common ancestor during merge from a non-sibling
void LeafNode::deleteEntry(const DataEntry& entryToRemove) {
    // TO DO: implement this function
    
    if(!this->contains(entryToRemove)){
        return;
    }
    
    auto i = std::lower_bound(this->entries.begin(),this->entries.end(),entryToRemove);
    entries.erase(i);
    if(this->entries.size() < kLeafOrder && this->getParent() != nullptr){
        
        //check if we can borrow from right
        if(this->rightNeighbor != nullptr && this->rightNeighbor->entries.size() > kLeafOrder){
            
            unsigned long sizeDifference = this->rightNeighbor->entries.size() - this->entries.size();
            unsigned long numTransferred = sizeDifference/2;
            for(unsigned long i = 0; i < numTransferred; ++i){
                this->entries.push_back(this->rightNeighbor->entries[0]);
                this->rightNeighbor->entries.erase(this->rightNeighbor->entries.begin());
            }
            
            //update common ancestor
            TreeNode* position = nullptr;
            InnerNode* commonAncestor = this->getCommonAncestor(this->rightNeighbor);
            Key updateKey = this->rightNeighbor->minKey();
            for(unsigned long i = 0; i < commonAncestor->getChildren().size(); ++i){
                if(commonAncestor->getChildren()[i]->contains(this->rightNeighbor)){
                    position = commonAncestor->getChildren()[i];
                }
            }
            commonAncestor->updateKey(position,updateKey);
        }
        //check if we can borrow from left
        else if(this->leftNeighbor != nullptr && this->leftNeighbor->entries.size() > kLeafOrder){
            
            unsigned long sizeDifference = this->leftNeighbor->entries.size() - this->entries.size();
            unsigned long numTransferred = 0;
            
            if(sizeDifference % 2 == 1){
                numTransferred = (sizeDifference/2)+1;
            }
            else{
                numTransferred = sizeDifference/2;
            }
            
            for(unsigned long i = 0; i < numTransferred; ++i){
                this->entries.insert(this->entries.begin(),this->leftNeighbor->entries[this->leftNeighbor->entries.size()-1]);
                this->leftNeighbor->entries.pop_back();
            }
            
            //update common ancestor
            TreeNode* position = nullptr;
            InnerNode* commonAncestor = this->getCommonAncestor(this->leftNeighbor);
            Key updateKey = this->minKey();
            for(unsigned long i = 0; i < commonAncestor->getChildren().size(); ++i){
                if(commonAncestor->getChildren()[i]->contains(this)){
                    position = commonAncestor->getChildren()[i];
                }
            }
            commonAncestor->updateKey(position,updateKey);
        }
        //merge with right
        else if(this->rightNeighbor != nullptr && this->rightNeighbor->entries.size() == kLeafOrder){
            for(unsigned int i = 0; i < kLeafOrder; ++i){
                this->entries.push_back(this->rightNeighbor->entries[i]);
            }
            for(unsigned int i = 0; i < kLeafOrder; ++i){
                this->rightNeighbor->entries.pop_back();
            }
            if(this->rightNeighbor->rightNeighbor != nullptr){
                this->rightNeighbor->rightNeighbor->leftNeighbor = this;
            }
            auto temp = this->rightNeighbor->rightNeighbor;
            
            //if pulling from a non-sibling
            if(this->getCommonAncestor(this->rightNeighbor) != this->getParent()){
                //update common ancestor
                TreeNode* position = nullptr;
                InnerNode* commonAncestor = this->getCommonAncestor(this->rightNeighbor);
                Key updateKey = this->rightNeighbor->rightNeighbor->minKey();
                for(unsigned long i = 0; i < commonAncestor->getChildren().size(); ++i){
                    if(commonAncestor->getChildren()[i]->contains(this->rightNeighbor)){
                        position = commonAncestor->getChildren()[i];
                    }
                }
                commonAncestor->updateKey(position,updateKey);
                this->rightNeighbor->getParent()->deleteChild(this->rightNeighbor);
            }
            else{
                this->getParent()->deleteChild(this->rightNeighbor);
            }
            
            this->rightNeighbor = temp;
        }
        //merge with left and then delete this
        else{
            for(unsigned int i = 0; i < kLeafOrder-1; ++i){
                this->leftNeighbor->entries.push_back(this->entries[i]);
            }
            if(this->rightNeighbor != nullptr){
                this->rightNeighbor->leftNeighbor = this->leftNeighbor;
            }
            if(this->leftNeighbor != nullptr){
                this->leftNeighbor->rightNeighbor = this->rightNeighbor;
            }
            this->getParent()->deleteChild(this);
        }
    }
}
