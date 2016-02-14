//|-------------------------------------------------------------------------
//| File:        list.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef listHPP
#define listHPP

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! Double linked list template.
template <class T>
class list
{
private:
    //! List element node with pointer to previous and next element in the list.
    struct SKListNode
    {
        SKListNode() : next(0), prev(0) {};

        SKListNode* next;
        SKListNode* prev;
        T element;
    };

public:

    //-----------------------------------------------------------------------
    //! List iterator.
    class iterator
    {
    public:

        iterator() : current(0) {};
        iterator(SKListNode* begin) : current(begin) {};

        inline iterator& operator ++() 
		{ 
			current = current->next; return *this; 
		}

        inline iterator& operator --() 
		{ 
			current = current->prev; return *this; 
		}

        inline iterator operator ++(s32) 
		{ 
			iterator tmp = *this; current = current->next; return tmp; 
		}

        inline iterator operator --(s32) 
		{ 
			iterator tmp = *this; current = current->prev; return tmp; 
		}

        inline iterator operator+(s32 num) const 
        { 
            iterator tmp = *this; 

            if (num >= 0) 
                while (num-- && tmp.current != 0) ++tmp; 
            else 
                while (num++ && tmp.current != 0) --tmp; 

            return tmp; 
        } 

        inline iterator& operator+=(s32 num) 
        { 
            if (num >= 0) 
                while (num-- && this->current != 0) ++(*this); 
            else 
                while (num++ && this->current != 0) --(*this); 

            return *this; 
        } 

        inline iterator operator-(s32 num) const  
		{ 
			return (*this)+(-num);          
		} 

        inline iterator operator-=(s32 num) const 
		{ 
			(*this)+=(-num);  return *this; 
		}

        inline bool operator ==(const iterator& other) const 
		{ 
			return current == other.current; 
		}

        inline bool operator !=(const iterator& other) const 
		{ 
			return current != other.current; 
		}

        inline T& operator *() 
		{ 
			return current->element; 
		}

    private:

        friend class list<T>;

        SKListNode* current;
    };

    //-----------------------------------------------------------------------

    //! constructor
    list()
        : root(0), last(0), list_size(0) {}

    //-----------------------------------------------------------------------

    //! destructor
    ~list()
    {
        clear();
    }

    //-----------------------------------------------------------------------

    //! Returns amount of elements in list.
    //! \return Returns amount of elements in the list. 
    inline u32 size() const
    {
        return list_size;
    }

    //-----------------------------------------------------------------------

    //! Clears the list, deletes all elements in the list. All existing 
    //! iterators of this list will be invalid.
    inline void clear()
    {
        SKListNode* node = root;
        while(node)
        {
            SKListNode* next = node->next;
            delete node;
            node = next;
        }

        root = 0;
        last = 0;
        list_size = 0;
    }

    //-----------------------------------------------------------------------

    //! Returns ture if the list is empty.
    //! \return Returns true if the list is empty and false if not.
    inline bool empty() const
    {
        return root == 0;
    }

    //-----------------------------------------------------------------------

    //! Adds an element at the end of the list.
    //! \param element: Element to add to the list.
    inline void push_back(const T& element)
    {
        SKListNode* node = new SKListNode;
        node->element = element;

        ++list_size;

        if (root == 0)
            root = node;

        node->prev = last;

        if (last != 0)
            last->next = node;

        last = node;
    }

    //-----------------------------------------------------------------------

    //! Adds an element at the begin of the list.
    //! \param element: Element to add to the list.
    inline void push_front(const T& element)
    {
        SKListNode* node = new SKListNode;
        node->element = element;

        ++list_size;

        if (root == 0)
        {
            last = node;
            root = node;
        }
        else
        {
            node->next = root;
            root->prev = node;
            root = node;
        }
    }

    //-----------------------------------------------------------------------

    //! Gets begin node.
    //! \return Returns a list iterator pointing to the begin of the list.
    inline iterator begin() const
    {
        return iterator(root);
    }

    //-----------------------------------------------------------------------

    //! Gets end node.
    //! \return Returns a list iterator pointing to null.
    inline iterator end() const
    {
        return iterator(0);
    }

    //-----------------------------------------------------------------------

    //! Gets last element.
    //! \return Returns a list iterator pointing to the end of the list.
    inline iterator get_last() const
    {
        return iterator(last);
    }

    //-----------------------------------------------------------------------

    //! Inserts an element after an element.
    //! \param it: iterator pointing to element after which the new element
    //! should be inserted.
    //! \param element: The new element to be insterted into the list.
    inline void insert_after(iterator& it, const T& element)
    {
        SKListNode* node = new SKListNode;
        node->element = element;

        node->next = it.current->next;

        if (it.current->next)
            it.current->next->prev = node;

        node->prev = it.current;
        it.current->next = node;
        ++list_size;
    }

    //-----------------------------------------------------------------------

    //! Inserts an element before an element.
    //! \param it: iterator pointing to element before which the new element
    //! should be inserted.
    //! \param element: The new element to be insterted into the list.
    inline void insert_before(iterator& it, const T& element)
    {
        SKListNode* node = new SKListNode;
        node->element = element;

        node->prev = it.current->prev;

        if (it.current->prev)
            it.current->prev->next = node;

        node->next = it.current;
        it.current->prev = node;
        ++list_size;
    }

    //-----------------------------------------------------------------------

    //! Erases an element	
    //! \param it: iterator pointing to the element which should be erased.
    //! \return Returns iterator pointing to next element.
    inline iterator erase(iterator& it)
    {
        iterator returniterator(it);
        ++returniterator;

        if (it.current == root)
            root = it.current->next;

        if (it.current == last)
            last = it.current->prev;

        if (it.current->next)
            it.current->next->prev = it.current->prev;

        if (it.current->prev)
            it.current->prev->next = it.current->next;

        delete it.current;
        it.current = 0;
        --list_size;

        return returniterator;
    }

private:

    SKListNode* root;
    SKListNode* last;
    u32 list_size;
};

//---------------------------------------------------------------------------
} // end namespace core
}// end namespace my
//---------------------------------------------------------------------------

#endif

