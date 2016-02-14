//|-------------------------------------------------------------------------
//| File:        array.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef arrayHPP
#define arrayHPP
//----------------------------------------------------------------------------

#include "sort.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! Self reallocating template array (like stl vector) with additional features.
template < class T >
class array
{
public:

    // Constructors/destructors
    array() 
		: data(0), used(0), allocated(0),
		free_when_destroyed(true), is_sorted(true) { }

    array(T* newPointer, u32 size, bool own_data = false) 
		: data(0), used(0), allocated(0),
		free_when_destroyed(true), is_sorted(true)
    { set_pointer(newPointer, size, own_data);}

    //! Constructs a array and allocates an initial chunk of memory.
    //! \param start_count: Amount of elements to allocate.
    array(u32 start_count)
		: data(0), used(0), allocated(0),
		free_when_destroyed(true),  is_sorted(true)
    { reallocate(start_count); }

    //! Copy constructor
    array(const array<T>& other)
        : data(0)
    { *this = other; }

    //! Destructor. Frees allocated memory, if set_free_when_destroyed
    //! was not set to false by the user before.
    ~array()
    {
        if (free_when_destroyed)
            delete [] data;
    }

	//! Reallocates the array, make it bigger or smaller.
    //! \param new_size: New size of array.
    void reallocate(u32 new_size)
    {
        T* old_data = data;

        data = new T[new_size];
        allocated = new_size;
        
        s32 end = used < new_size ? used : new_size;
        for (s32 i=0; i<end; ++i)
            data[i] = old_data[i];

        if (allocated < used)
            used = allocated;
        
        delete [] old_data;

		is_sorted=false;
    }

    //! Adds an element at back of array. If the array is to small to 
    //! add this new element, the array is made bigger.
    //! \param element: Element to add at the back of the array.
    u32 push_back(const T& element)
    {
        if (used + 1 > allocated)
            reallocate(used * 2 +1);

        data[used++] = element;
        is_sorted = false;

		return used - 1;
    }

    //! Adds an element at front of array. If the array is to small to 
    //! add this new element, the array is made bigger.
    //! \param element: Element to add at the front of the array.
    void push_front(const T& element)
    {
        if (used + 1 > allocated)
            reallocate(used * 2 +1);        
		used++;
        for (u32 i=(used-1); i>0; i--)
            data[i] = data[i-1];
		data[0] = element;
        is_sorted = false;
    }

    //! Clears the array and deletes all allocated memory.
    void clear()
    {
		if (free_when_destroyed)
			delete [] data;
        data = 0;
        used = 0;
        allocated = 0;
        is_sorted = true;
    }

    //! Clears the array and deletes all allocated memory.
    void clear_ptr()
    {
		for (u32 i=0; i<used; ++i)
            delete data[i];
		clear();
    }

	//! Sets pointer to new array, using this as new workspace.
    //! \param newPointer: Pointer to new array of elements.
    //! \param size: Size of the new array.
    void set_pointer(T* newPointer, u32 size, bool own_data)
    {
		if (free_when_destroyed)
			delete [] data;
        data = newPointer;
        allocated = size;
        used = size;
        is_sorted = false;
		free_when_destroyed = own_data;
    }

    //! Return true if the array should delete the memory it used.
    bool is_own_data() const
    {
        return free_when_destroyed;
    }

    //! Sets the size of the array.
    //! \param usedNow: Amount of elements now used.
    void set_used(u32 usedNow)
    {
        if (allocated < usedNow)
            reallocate(usedNow);	
        used = usedNow;
    }

	//! Assignement operator
    void operator = (const array<T>& other)
    {
        if (free_when_destroyed)
            delete [] data;
        if (other.allocated_size() == 0)
            data = 0;
        else
            data = new T[other.allocated_size()];
        used = other.size();
        free_when_destroyed = other.is_own_data();
        is_sorted = other.sorted();
        allocated = other.allocated_size();
        for (u32 i=0; i<used; ++i)
            data[i] = other[i];
    }

    //! Direct access operator
    T& operator [](u32 index)
    {
#if MY_DEBUG_MODE && MY_COMPILER != MY_COMPILER_BORL
        if (index>=used)
			_asm int 3 // access violation
#endif   
        return data[index];
    }

    //! Direct access operator
    const T& operator [](u32 index) const
    {
#if MY_DEBUG_MODE && MY_COMPILER != MY_COMPILER_BORL
        if (index>=used)
            _asm int 3 // access violation
#endif
        return data[index];
    }

    //! == operator
    bool operator==(const array<T>& other) const
    {
        if (used!=other.size())
            return false; 
		for (u32 i=0; i<used; i++)
			if (data[i]!=other[i])
				return false;
		return true;
    }

    //! != operator
    bool operator != (const array<T>& other) const
    {
		if (used != other.size())
            return true; 
		for (u32 i=0; i<used; i++)
			if (data[i]!=other[i])
				return true;
		return false;
    }

    //! Gets last frame
    const T& get_last() const
    {
#if MY_DEBUG_MODE && MY_COMPILER != MY_COMPILER_BORL
        if (!used)
            _asm int 3 // access violation
#endif
        return data[used-1];
    }

    //! Gets last frame
    T& get_last()
    {
#if MY_DEBUG_MODE && MY_COMPILER != MY_COMPILER_BORL
        if (!used)
            _asm int 3 // access violation
#endif
        return data[used-1];
    }

    //! Returns a pointer to the array.
    //! \return Pointer to the array.
    T* pointer() { return data; }

    //! Returns a const pointer to the array.
    //! \return Pointer to the array.
    const T* const_pointer() const { return data; }

    //! Returns size of used array.
    //! \return Size of elements in the array.
    u32 size() const { return used; }

    //! Returns amount memory allocated.
    //! \return Returns amount of memory allocated. The amount of bytes
    //! allocated would  be allocated_size() * sizeof(ElementsUsed);
    u32 allocated_size() const { return allocated; }

    //! Returns true if array is empty
    //! \return True if the array is empty, false if not.
    bool empty() const { return used == 0; }

    //! Sorts the array
	template < class Sorter >
    void sort()
    {
        if (is_sorted || used<2)
            return;
        Sorter().sort(data, used);
        is_sorted = true;
    }

    //! Sorts the ureferenced array
	template < class Sorter >
    void sort_ptr()
    {
        if ( is_sorted || used < 2 )
            return;
        Sorter().sort_ptr ( data, used );
        is_sorted = true;
    }

    //! Sorts the array with deault sorter
    void sort()
    { sort <quicksort> (); }

    void sort_ptr()
    { sort_ptr <quicksort> (); }

    //! Performs a binary search for an element, returns -1 if not found.
    //! The array will be sorted before the binary search if it is not
    //! already sorted.
    //! \param element: Element to search for.
    //! \return Returns position of the searched element if it was found,
    //! otherwise -1 is returned.
	template <class S>
    s32 binary_search(const S &element)
    { return binary_search(element, 0, used - 1); }

    //! Performs a binary search for an element, returns -1 if not found.
    //! The array will be sorted before the binary search if it is not
    //! already sorted.
    //! \param element: Element to search for.
    //! \param left: First left index
    //! \param right: Last right index.
    //! \return Returns position of the searched element if it was found,
    //! otherwise -1 is returned.
	template <class S>
    s32 binary_search(const S & element, s32 left, s32 right)
    {   
		if (!used)
            return -1;
        sort();
        s32 m;
        do
        {   m = (left+right)>>1;
            if (element < data[m])
                right = m - 1;
            else
                left = m + 1;
        } 
		while((element < data[m] || data[m] < element) && left<=right);
        // this last line equals to:
        // " while((element != array[m]) && left<=right);"
        // but we only want to use the '<' operator.
        // the same in next line, it is "(element == array[m])"
        if (!(element < data[m]) && !(data[m] < element))
            return m;
        return -1;
    }

    //! Finds an element in linear time, which is very slow. Use
    //! binary_search for faster finding. Only works if =operator is implemented.
    //! \param element: Element to search for.
    //! \return Returns position of the searched element if it was found,
    //! otherwise -1 is returned.
	template <class S>
    s32 linear_search(S &element)
    {
        for ( u32 i = 0; i < used; ++i )
            if ( data[i] == element )
                return (s32)i;
        return -1;
    }

	template < class S >
    s32 linear_search_ptr(S & element)
    {
        for ( u32 i = 0; i < used; ++i )
            if ( *data[i] == element )
                return (s32)i;
        return -1;
    }

    //! Finds an element in linear time, which is very slow. Use
    //! binary_search for faster finding. Only works if =operator is implemented.
    //! \param element: Element to search for.
    //! \return Returns position of the searched element if it was found,
    //! otherwise -1 is returned.
	template <class S>
    s32 linear_reverse_search(S &element)
    {
        for ( s32 i = used - 1; i >= 0; --i )
            if ( data[i] == element )
                return (s32)i;
        return -1;
    }

    //! Erases an element from the array. May be slow, because all elements
    //! following after the erased element have to be copied.
    //! \param index: Index of element to be erased.
    void erase(u32 index)
    {
#if MY_DEBUG_MODE && MY_COMPILER != MY_COMPILER_BORL
		if (index>=used || index<0)
			_asm int 3 // access violation
#endif
        for (u32 i=index+1; i<used; ++i)
            data[i-1] = data[i];
        --used;
    }

    //! Erases some elements from the array. may be slow, because all elements
    //! following after the erased element have to be copied.
    //! \param index: Index of the first element to be erased.
    //! \param count: Amount of elements to be erased.
    void erase(u32 index, s32 count)
    {
#if MY_DEBUG_MODE && MY_COMPILER != MY_COMPILER_BORL
		if (index>=used || index<0 || count<1 || index+count>used)
			_asm int 3 // access violation
#endif
        for (u32 i=index+count; i<used; ++i)
            data[i-count] = data[i];
        used -= count;
    }

    //! Sets if the array is sorted
    void set_sorted(bool _is_sorted) { is_sorted = _is_sorted; }

    //! Return true if the array is sorted
    bool sorted() const { return is_sorted; }

private:

	T		*data;
	u32		allocated;
	u32		used;
	bool	free_when_destroyed;
	bool	is_sorted;
};

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif

