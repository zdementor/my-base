//|-------------------------------------------------------------------------
//| File:        IUnknown.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IUnknownHPP
#define IUnknownHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include "MemAudit.h" 

//----------------------------------------------------------------------------
namespace my {    
//----------------------------------------------------------------------------

//! Base class of most objects of the MyEngine.
//! This class provides reference counting through the methods grab() and drop().
//! It also is able to store a debug string for every instance of an object.
//! Most objects of the MyEngine are derived from IUnknown, 
//! and so they are reference counted.
class __MY_SCN_LIB_API__ IUnknown
{
	friend void getUObjectsNamesList(core::stringc &nameList);

public:
    //! Constructor.
    IUnknown() : ReferenceCounter(1), locked(false)
    {
#if MY_DEBUG_MODE  
		ClassName = NULL;
		addUnknownObjectToList((void*)this);
#endif
    }
    //! Destructor.
    virtual ~IUnknown()      
    {	
#if MY_DEBUG_MODE  
		remUnknownObjectFromList((void*)this);
#endif
    }
    //! Grabs the object. Increments the reference counter by one.
    //! Someone who calls grab() to an object, should later also call
    //! drop() to it. If an object never gets as much drop() as grab()
    //! calls, it will never be destroyed.
    //! The IUnknown class provides a basic reference counting mechanism
    //! with its methods grab() and drop(). Most objects of the Irrlicht
    //! Engine are derived from IUnknown, and so they are reference counted.
    //!
    //! When you create an object in the Irrlicht engine, calling a method
    //! which starts with 'create', an object is created, and you get a pointer
    //! to the new object. If you no longer need the object, you have
    //! to call drop(). This will destroy the object, if grab() was not called
    //! in another part of you program, because this part still needs the object.
    //! Note, that you only need to call drop() to the object, if you created it,
    //! and the method had a 'create' in it.
    //!
    //! A simple example:
    //!
    //! If you want to create a texture, you may want to call an imaginable method
    //! IDriver::createTexture. You call
    //! ITexture* texture = driver->createTexture(dimension2d<s32>(128, 128));
    //! If you no longer need the texture, call texture->drop().
    //! If you want to load a texture, you may want to call imaginable method
    //! IDriver::loadTexture. You do this like
    //! ITexture* texture = driver->loadTexture("example.jpg");
    //! You will not have to drop the pointer to the loaded texture, because
    //! the name of the method does not start with 'create'. The texture
    //! is stored somewhere by the driver.
    void grab() 
	{ 
		if (locked) return; 
		
		++ReferenceCounter; 
	}

    //! Drops the object. Decrements the reference counter by one.
    //! Returns true, if the object was deleted.
    //! The IUnknown class provides a basic reference counting mechanism
    //! with its methods grab() and drop(). Most objects of the Irrlicht
    //! Engine are derived from IUnknown, and so they are reference counted.
    //!
    //! When you create an object in the Irrlicht engine, calling a method
    //! which starts with 'create', an object is created, and you get a pointer
    //! to the new object. If you no longer need the object, you have
    //! to call drop(). This will destroy the object, if grab() was not called
    //! in another part of you program, because this part still needs the object.
    //! Note, that you only need to call drop() to the object, if you created it,
    //! and the method had a 'create' in it.
    //!
    //! A simple example:
    //!
    //! If you want to create a texture, you may want to call an imaginable method
    //! IDriver::createTexture. You call
    //! ITexture* texture = driver->createTexture(dimension2d<s32>(128, 128));
    //! If you no longer need the texture, call texture->drop().
    //! If you want to load a texture, you may want to call imaginable method
    //! IDriver::loadTexture. You do this like
    //! ITexture* texture = driver->loadTexture("example.jpg");
    //! You will not have to drop the pointer to the loaded texture, because
    //! the name of the method does not start with 'create'. The texture
    //! is stored somewhere by the driver.
    bool drop()
    {   
		#if MY_DEBUG_MODE && MY_COMPILER != MY_COMPILER_BORL
		if (ReferenceCounter<0)
			_asm int 3 // access violation		
		#endif 

		if (locked) return false;

        --ReferenceCounter;

        if (ReferenceCounter==0)
        {   
			delete this;
            return true;
        }

        return false;
    }
    
    // returns ReferenceCounter
    s32 getReferenceCounter()
    {   
		return ReferenceCounter;
    }

#if MY_DEBUG_MODE  
	//! Returns the name of the object. 
    //! \return Returns a string, previously set by setName();
    const c8* getClassName() const
    {   
		return ClassName;
    }
#endif

	//! return true if this object locked by some other object
	bool isLocked()
	{
		return locked;
	}

	//! lock this object
	void lockObject()
	{
		locked = true;
	}

	//! lock this object
	void unlockObject()
	{
		locked = false;
	}

protected:

#if MY_DEBUG_MODE  
    //! Sets the name of the object. The ClassName may only be set and
    //! changed by the object itself. 
    void setClassName(const c8* newName)
    {   
		ClassName = newName;
    }
#endif

private:

    s32 ReferenceCounter;

#if MY_DEBUG_MODE  
	const c8* ClassName;
#endif

	bool locked;
};

//----------------------------------------------------------------------------
} // end namespace my
//----------------------------------------------------------------------------

#endif //IUnknownHPP

