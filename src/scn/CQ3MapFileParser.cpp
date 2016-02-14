//|-------------------------------------------------------------------------
//| File:        CQ3MapFileParser.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CQ3MapFileParser.h"
#include <io/IFileSystem.h>
#include <io/IReadFile.h>
#include <io/ILogger.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

core::stringc CQ3MapFileParser::extractEntitiesString(io::IReadFile *file)
{ 
	core::stringc ent_str = "";

	if (!file)
		return ent_str;
	
	SMapFileNode *root = new SMapFileNode(0, file->getSize()-1);

	parseMapFile(file, root);

	for (u32 ch=0; ch<root->Childs.size(); ch++)
	{
		SMapFileNode *child = root->Childs[ch];

		if (child && child->StringData.size()>0)
		{
			ent_str.append("{\n");
			ent_str.append(child->StringData.c_str());
			ent_str.append("\n}\n");
		}
	}

	SAFE_DELETE(root);

	LOGGER.log("Extracted entities:");
	LOGGER.log(ent_str.c_str());

	return ent_str; 
}

//----------------------------------------------------------------------------

void CQ3MapFileParser::printMapNode(SMapFileNode *node)
{
	if (!node)
		return;

	LOGGER.log("{");

	LOGGER.log(node->StringData.c_str());

	for (u32 ch=0; ch<node->Childs.size(); ch++)
	{
		printMapNode(node->Childs[ch]);	
	}		

	LOGGER.log("}");
}

//----------------------------------------------------------------------------

void CQ3MapFileParser::parseMapNode(
	s32 recurse_depth, const c8 *data, s32 start, s32 end, SMapFileNode *node
	)
{
	if (!data || !node)
		return;

	recurse_depth++;

	s32 start_from=start, end_to=end;
	bool first_token_finded = false;

	s32 depth = 0;

	for (s32 i=start; i<=end; i++)
	{
		if (data[i] == '{')
		{
			first_token_finded = true;

			if (depth==0)
			{
				start_from = i;
			}

			depth++;
		}
		else if (data[i] == '}')
		{
			depth--;

			if (depth==0)
			{
				end_to = i;
			}
		}

		if (depth==0 && first_token_finded)
		{
			first_token_finded = false;

			node->Childs.push_back(new SMapFileNode(start_from+1, end_to-1));
		}
	}

	core::stringc str="";

	if (node->Childs.size()>0)
	{
		for (u32 ch=0; ch<=node->Childs.size(); ch++)
		{
			SMapFileNode *child      = (ch<node->Childs.size()) ? node->Childs[ch] : NULL;
			SMapFileNode *prev_child = (ch>0) ? node->Childs[ch-1] : NULL;

			if (ch==0)
			{
				s32 s = child->StartFrom-2 - (node->StartFrom);

				if (s>0)
				{
					s++;

					str.reserve(s+1);

					s32 i=0;

					for (i=0; i<=s; i++)
					{
						str[i]=0;
					}

					for (i=0; i<s; i++)
					{
						str[i] = data[node->StartFrom + i];
					}
				}
			}
			else if (ch==(node->Childs.size()))
			{
				s32 s = node->EndTo - (prev_child->EndTo+2) ;

				if (s>0)
				{
					s++;

					str.reserve(s+1);

					s32 i=0;

					for (i=0; i<=s; i++)
					{
						str[i]=0;
					}

					for (i=0; i<s; i++)
					{
						str[i] = data[prev_child->EndTo+2 + i];
					}
				}
			}
			else
			{
				s32 s = child->StartFrom-2 - (prev_child->EndTo+2);

				if (s>0)
				{
					s++;					

					str.reserve(s+1);

					s32 i=0;

					for (i=0; i<=s; i++)
					{
						str[i]=0;
					}

					for (i=0; i<s; i++)
					{
						str[i] = data[prev_child->EndTo+2 + i];
					}
				}
			}		

			node->StringData.append(str.c_str());

			if (child)
				parseMapNode(recurse_depth, data, child->StartFrom, child->EndTo, child);		
		}
	}
	else
	{
		s32 s = node->EndTo - (node->StartFrom);

		if (s>0)
		{
			s++;

			str.reserve(s+1);

			s32 i=0;

			for (i=0; i<=s; i++)
			{
				str[i]=0;
			}

			for (i=0; i<s; i++)
			{
				str[i] = data[node->StartFrom + i];
			}
			
			node->StringData.append(str.c_str());			
		}		
	}

	recurse_depth--;
}

//----------------------------------------------------------------------------

void CQ3MapFileParser::parseMapFile(io::IReadFile *file, SMapFileNode *root)
{
	if (!file || !root)
		return;

	LOGGER.log("Parsing map file ", file->getFileName() );

	s32 fsize = file->getSize();

	c8 *data = new c8[fsize];

	file->seek(0);
	file->read(data, fsize);

	s32 start=0, end=fsize-1;
	s32 recurse_depth = 0;

	parseMapNode( recurse_depth, data, start, end, root );
	
	SAFE_DELETE_ARRAY(data);
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
