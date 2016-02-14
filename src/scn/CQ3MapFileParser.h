//|-------------------------------------------------------------------------
//| File:        CQ3MapFileParser.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CQ3MapFileParser
{
private:

	struct SMapFileNode
	{
		SMapFileNode() : StartFrom(0), EndTo(0), StringData("") {}

		SMapFileNode(s32 start_from, s32 end_to) 
			: StartFrom(start_from), EndTo(end_to), StringData("") {}

		~SMapFileNode()
		{
			for (u32 ch=0; ch<Childs.size(); ch++)
			{
				SAFE_DELETE(Childs[ch]);
			}

			Childs.clear();
		}

		s32 StartFrom, EndTo;
		core::stringc StringData;
		core::array<SMapFileNode*> Childs;
	};

	static void printMapNode(SMapFileNode *node);
	static void parseMapFile(io::IReadFile *file, SMapFileNode *root);
	static void parseMapNode(
		s32 recurse_depth, const c8 *data, s32 start, s32 end, SMapFileNode *node
		);

public:

	static core::stringc extractEntitiesString(io::IReadFile *file);
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------