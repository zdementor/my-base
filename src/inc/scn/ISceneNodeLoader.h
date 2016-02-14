//|-------------------------------------------------------------------------
//| File:        ISceneNodeLoader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISceneNodeLoaderHPP
#define ISceneNodeLoaderHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Game Node Loader interface
class __MY_SCN_LIB_API__ ISceneNodeLoader : public IUnknown
{
public:

    //! constructor
    ISceneNodeLoader() {}

    //! destructor
    virtual ~ISceneNodeLoader() {}

    //! loading scene node from the xml-file
    virtual scn::ISceneNode* loadSceneNode(io::IXMLReader *xml) = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef ISceneNodeLoaderHPP