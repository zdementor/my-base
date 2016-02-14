//|-------------------------------------------------------------------------
//| File:        ISceneNodeCreator.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISceneNodeCreatorHPP
#define ISceneNodeCreatorHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Game Node Creator interface
class __MY_SCN_LIB_API__ ISceneNodeCreator : public IUnknown
{
public:

    //! constructor
    ISceneNodeCreator() {}

    //! destructor
    virtual ~ISceneNodeCreator() {}

    //! creating Scene node (return new created scene node)
    virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info=false) = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef ISceneNodeCreatorHPP