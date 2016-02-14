//|-------------------------------------------------------------------------
//| File:        queue.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef queueHPP
#define queueHPP
//----------------------------------------------------------------------------

#include "list.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------   

template <class T>
class queue
{
public:

	bool erase(T obj)
	{
		core::list<T>::iterator iter = queue_objects.begin();

		for (; iter!=queue_objects.end(); ++iter)
		{
			if (*iter==obj)
			{
				queue_objects.erase(iter);

				return true;
			}
		}	

		return false;
	}

	void push_back(T obj)
	{
		core::list<T>::iterator iter = queue_objects.begin();

		for (; iter!=queue_objects.end(); ++iter)
		{
			if (*iter==obj)
			{
				// already in list
				return;
			}
		}

		queue_objects.push_back(obj);		
	}

	bool is_ready(T obj)
	{
		return *(queue_objects.begin())==obj;
	}
	
	void switch_next()
	{
		queue_objects.erase(queue_objects.begin());
	}

private:

	core::list<T> queue_objects;
};

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef queueHPP
