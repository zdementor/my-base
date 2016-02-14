//|-------------------------------------------------------------------------
//| File:        ISceneNodeCloner.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISceneNodeClonerHPP
#define ISceneNodeClonerHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Game Node Cloner interface
class __MY_SCN_LIB_API__ ISceneNodeCloner : public IUnknown
{
public:

    //! constructor
    ISceneNodeCloner() {}

    //! destructor
    virtual ~ISceneNodeCloner() {}

    //! Cloning Scene node (return new created scene node)
    virtual scn::ISceneNode* cloneSceneNode(scn::ISceneNode* node_to_clone, bool log_info=false) = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef ISceneNodeSaverHPP