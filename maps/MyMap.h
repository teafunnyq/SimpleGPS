//
//  MyMap.h
//  maps
//
//  Created by Tiffany Kyu on 3/8/17.
//  Copyright © 2017 Tiffany Kyu. All rights reserved.
//

#ifndef MyMap_h
#define MyMap_h

template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap();
    ~MyMap();
    void clear();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
private:
    struct Node{
        KeyType m_key;
        ValueType m_val;
        Node *left, *right;
    };
    Node* m_root;
    int m_size;
    void FreeTree(Node* cur);   //Deletes the branchs of the tree
    Node* addNewNode(KeyType key, ValueType value); //Create a new Node
};

//Inline implementations

template<typename KeyType, typename ValueType>
inline
int MyMap<KeyType,ValueType>::size() const
{
    return m_size;
}

//Non-inline implementation
template <typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
: m_size(0)
{
    m_root = nullptr;
}

template <typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap()
{
    clear();
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::FreeTree(Node* cur)
{
    if ( cur == nullptr)
        return;
    FreeTree(cur->left);
    FreeTree(cur->right);
    
    delete cur;
}

template <typename KeyType, typename ValueType>
typename MyMap<KeyType,ValueType>::Node* MyMap<KeyType,ValueType>::addNewNode(KeyType key, ValueType value)
{
    Node* temp = new Node;
    temp -> m_key = key;
    temp -> m_val = value;
    temp -> left = nullptr;
    temp -> right = nullptr;
    return temp;
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType,ValueType>::clear()
{
    if(m_root == nullptr)
        return;
    
    FreeTree(m_root);
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    if ( m_root == nullptr)    //If BT is empty, add as root
    {
        Node* add = addNewNode(key, value);
        m_root = add;   //Point root
        m_size++;   //Increase size
        return;
    }
    
    Node* cur = m_root;
    
    for(;;) //Cyce through entire BT
    {
        if ( key == cur->m_key) //Key already here, so replace value
        {
            cur->m_val = value;
            return;
        }
        if ( key < cur->m_key)  //Key less than current data
        {
            if ( cur -> left != nullptr)    //If left child not empty, move to left
                cur = cur -> left;
            else    //If it is empty, add new node and point left pointer to it
            {
                Node* add = addNewNode(key, value);
                cur -> left = add;
                m_size++;
                return;
            }
        }
        //else if ( key > cur->m_key)
        else    //Key greater than current data
        {
            if ( cur -> right != nullptr)   //If right child not empty, move to right
                cur = cur -> right;
            else    //If it is empty, add new node and point right child to it
            {
                Node* add = addNewNode(key, value);
                cur -> right = add;
                m_size++;
                return;
            }
        }
    }
}

template <typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
    if ( m_root == nullptr) //BT is empty, nothing to return
        return nullptr;
    
    Node* cur = m_root;
    const ValueType* temp = nullptr;
    
    while( cur != nullptr)  //Go through entire tree until you hit the bottom branches
    {
        if ( cur -> m_key == key)   //If you find the key, return pointer to its value and exit loop
        {
            temp = &(cur->m_val);
            break;
        }
        if ( key < cur->m_key)  //Key less than current data, so move left
            cur = cur -> left;
        //else if ( key > cur->m_key)
        else    //Key greater than current data, so move right
            cur = cur -> right;
    }
    
    return temp;
}

#endif /* MyMap_h */