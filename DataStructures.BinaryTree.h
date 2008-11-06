#ifndef BINARY_TREE_H
#define BINARY_TREE_H

namespace DataStructures
{
  template <class T>
  class BinaryTree
  {
  protected:
    class Node
    {
    public:
      Node()
      {
        this->parent = this->left = this->right = NULL;
      }

      Node( const T& value, Node* parent )
      {
        this->value  = value;
        this->parent = parent;
        this->left   = this->right = NULL;
      }

      T     value;
      Node* parent;
      Node* left;
      Node* right;
    };

    Node* root;

    bool r_insert( const T& value, Node*& cur, Node* parent = NULL )
    {
      if( cur == NULL )
      {
        cur = new Node( value, parent );
        return( true );
      }
      else if( value < cur->value )
        this->r_insert( value, cur->left, cur );
      else if( value > cur->value )
        this->r_insert( value, cur->right, cur );

      return( false );
    }

    Node* find_next_highest( Node* cur )
    {
      if( cur->right != NULL )
        return( this->r_find_next_highest( cur->right ) );
      else
        return( NULL );
    }

    Node* r_find_next_highest( Node* cur )
    {
      if( cur->left != NULL )
        return( this->r_find_next_highest( cur->left ) );
      else
        return( cur );
    }

    Node* find( T value )
    {
      return( this->r_find( value, this->root ) );
    }

    Node* r_find( T value, Node* cur )
    {
      if( cur == NULL )
        return( NULL );
      else if( value == cur->value )
        return( cur );
      else if( value < cur->value )
        return( this->r_find( value, cur->left ) );
      else
        return( this->r_find( value, cur->right ) );
    }

  public:
    BinaryTree()
    {
      this->root = NULL;
    }

    bool insert( const T& value )
    {
      return( this->r_insert( value, this->root ) );
    }

    bool remove( const T& value )
    {
      Node* old = NULL;
      Node* tmp = this->find( value );

      if( tmp == NULL )
        return( false );
      else if( tmp->left == NULL && tmp->right == NULL )
      {
        // delete tmp;
      }
      else if( tmp->right == NULL )
      {
        if( tmp == this->root )
          this->root = tmp->left;
        else if( tmp == tmp->parent->left )
          tmp->parent->left = tmp->left;
        else
          tmp->parent->right = tmp->left;
        
        if( tmp->left != NULL )
          tmp->left->parent = tmp->parent;
      }
      else
      {
        old = tmp;
        tmp = this->find_next_highest( old );

        old->value = tmp->value;

        if( tmp == tmp->parent->left )
          tmp->parent->left = tmp->right;
        else
          tmp->parent->right = tmp->right;

        if( tmp->right != NULL )
          tmp->right->parent = tmp->parent;
      }

      delete tmp;
      tmp = NULL;

      return( true );
    }
  };
}

#endif