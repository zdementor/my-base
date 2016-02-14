//|-------------------------------------------------------------------------
//| File:        Q3Helper.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "Q3Helper.h"

#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
namespace q3 {
//---------------------------------------------------------------------------

c8 ch[255];
static	char	com_token[MAX_TOKEN_CHARS];
static	int		com_lines;

//--------------------------------------------------------------

int Q_stricmpn (const char *s1, const char *s2, int n) 
{
	int		c1, c2;
	
	do 
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) 
		{
			return 0;		// strings are equal until end point
		}
		
		if (c1 != c2) 
		{
			if (c1 >= 'a' && c1 <= 'z') 
			{
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z') 
			{
				c2 -= ('a' - 'A');
			}
			if (c1 != c2) 
			{
				return c1 < c2 ? -1 : 1;
			}
		}
	} while (c1);
	
	return 0;		// strings are equal
}

//--------------------------------------------------------------

int Q_stricmp (const char *s1, const char *s2) 
{
	return Q_stricmpn (s1, s2, 99999);
} 

//--------------------------------------------------------------

static char *SkipWhitespace( char *data, qboolean *hasNewLines ) 
{
	int c;

	while( (c = *data) <= ' ') 
	{
		if( !c ) 
		{
			return NULL;
		}
		if( c == '\n' ) 
		{
			com_lines++;
			*hasNewLines = qtrue;
		}
		data++;
	}

	return data;
}

//--------------------------------------------------------------

//! Parse a token out of a string
//! Will never return NULL, just empty strings
//! If "allowLineBreaks" is qtrue then an empty
//! string will be returned if the next token is
//! a newline.
char *COM_ParseExt( char **data_p, qboolean allowLineBreaks )
{
	int c = 0, len;
	qboolean hasNewLines = qfalse;
	char *data;

	data = *data_p;
	len = 0;
	com_token[0] = 0;

	// make sure incoming data is valid
	if ( !data )
	{
		*data_p = NULL;
		return com_token;
	}

	while ( 1 )
	{
		// skip whitespace
		data = SkipWhitespace( data, &hasNewLines );
		if ( !data )
		{
			*data_p = NULL;
			return com_token;
		}
		if ( hasNewLines && !allowLineBreaks )
		{
			*data_p = data;
			return com_token;
		}

		c = *data;

		// skip double slash comments
		if ( c == '/' && data[1] == '/' )
		{
			data += 2;
			while (*data && *data != '\n') 
			{
				data++;
			}
		}
		// skip /* */ comments
		else if ( c=='/' && data[1] == '*' ) 
		{
			data += 2;
			while ( *data && ( *data != '*' || data[1] != '/' ) ) 
			{
				data++;
			}
			if ( *data ) 
			{
				data += 2;
			}
		}
		else
		{
			break;
		}
	}

	// handle quoted strings
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				*data_p = ( char * ) data;
				return com_token;
			}
			if (len < MAX_TOKEN_CHARS)
			{
				com_token[len] = c;
				len++;
			}
		}
	}

	// parse a regular word
	do
	{
		if (len < MAX_TOKEN_CHARS)
		{
			com_token[len] = c;
			len++;
		}
		data++;
		c = *data;
		if ( c == '\n' )
			com_lines++;
	} 
	while (c>32);

	if (len == MAX_TOKEN_CHARS)
	{
		sprintf(ch, "token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
		LOGGER.log(ch, io::ELL_WARNING);

		len = 0;
	}
	com_token[len] = 0;

	*data_p = ( char * ) data;

	return com_token;
}

//--------------------------------------------------------------

int COM_Compress( char *data_p ) 
{
	char *in, *out;
	int c;
	qboolean newline = qfalse, whitespace = qfalse;

	in = out = data_p;
	if (in) 
	{
		while ((c = *in) != 0) 
		{			
			if ( c == '/' && in[1] == '/' ) 
			// skip double slash comments
			{
				while (*in && *in != '\n') 
				{
					in++;
				}
			
			} 
			else if ( c == '/' && in[1] == '*' ) 
			// skip /* */ comments
			{
				while ( *in && ( *in != '*' || in[1] != '/' ) ) 
					in++;
				if ( *in ) 
					in += 2;        
			} 
			else if ( c == '\n' || c == '\r' ) 
			// record when we hit a newline
			{
				newline = qtrue;
                in++;
			
            } 
			else if ( c == ' ' || c == '\t') 
			// record when we hit whitespace
			{
				whitespace = qtrue;
                in++;            
			} 
			else 
			// an actual token
			{
				// if we have a pending newline, emit it (and it counts as whitespace)
				if (newline) 
				{
					*out++ = '\n';
                    newline = qfalse;
                    whitespace = qfalse;                            
				} 
				
				if (whitespace) 
				{
					*out++ = ' ';
					whitespace = qfalse;
                }
                            
                // copy quoted strings unmolested
                if (c == '"') 
				{
					*out++ = c;
                    in++;
                                    
					while (1) 
					{
						c = *in;
                        if (c && c != '"')
						{
							*out++ = c;
                            in++;
                        } 
						else 
						{                                            
							break;                                        
						}                                    
					}
                                    
					if (c == '"') 
					{                                        
						*out++ = c;                                        
						in++;                                    
					}                            
				} 
				else 
				{                                
					*out = c;                                
					out++;                                
					in++;                            
				}
			}
		}
	}
	*out = 0;

	return out - data_p;
}

//--------------------------------------------------------------

//! Strip Extension
void COM_StripExtension( const char *in, char *out ) 
{
	while ( *in && *in != '.' ) 
	{
		*out++ = *in++;
	}
	*out = 0;
}

//--------------------------------------------------------------

//! The next token should be an open brace.
//! Skips until a matching close brace is found.
//! Internal brace depths are properly skipped.
void SkipBracedSection (char **program) 
{
	char *token;
	int	  depth;

	depth = 0;
	do
	{
		token = COM_ParseExt( program, qtrue );

		if( token[1] == 0 )
		{
			if( token[0] == '{' ) 
			{
				depth++;
			}
			else if( token[0] == '}' ) 
			{
				depth--;
			}
		}
	} 
	while( depth && *program );
}

//--------------------------------------------------------------

void SkipRestOfLine ( char **data )
{
	char	*p;
	int		c;

	p = *data;
	while ( (c = *p++) != 0 ) 
	{
		if ( c == '\n' ) 
		{
			com_lines++;
			break;
		}
	}

	*data = p;
}

//--------------------------------------------------------------

vec_t VectorNormalize( vec3_t v ) 
{
	// NOTE: TTimo - Apple G4 altivec source uses double?
	float	length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = sqrt (length);

	if ( length )
	{
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}
		
	return length;
}

//--------------------------------------------------------------

unsigned NameToAFunc( const char *funcname )
{	
	if ( !Q_stricmp( funcname, "GT0" ) )
	{
		return GLS_ATEST_GT_0;
	}
	else if ( !Q_stricmp( funcname, "LT128" ) )
	{
		return GLS_ATEST_LT_80;
	}
	else if ( !Q_stricmp( funcname, "GE128" ) )
	{
		return GLS_ATEST_GE_80;
	}

	sprintf(ch, "invalid alphaFunc name '%s'", funcname);
	LOGGER.log(ch, io::ELL_WARNING);
	
	return 0;
}

//--------------------------------------------------------------

int NameToSrcBlendMode( const char *name )
{
	if ( !Q_stricmp( name, "GL_ONE" ) )
	{
		return GLS_SRCBLEND_ONE;
	}
	else if ( !Q_stricmp( name, "GL_ZERO" ) )
	{
		return GLS_SRCBLEND_ZERO;
	}
	else if ( !Q_stricmp( name, "GL_DST_COLOR" ) )
	{
		return GLS_SRCBLEND_DST_COLOR;
	}
	else if ( !Q_stricmp( name, "GL_ONE_MINUS_DST_COLOR" ) )
	{
		return GLS_SRCBLEND_ONE_MINUS_DST_COLOR;
	}
	else if ( !Q_stricmp( name, "GL_SRC_ALPHA" ) )
	{
		return GLS_SRCBLEND_SRC_ALPHA;
	}
	else if ( !Q_stricmp( name, "GL_ONE_MINUS_SRC_ALPHA" ) )
	{
		return GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA;
	}
	else if ( !Q_stricmp( name, "GL_DST_ALPHA" ) )
	{
		return GLS_SRCBLEND_DST_ALPHA;
	}
	else if ( !Q_stricmp( name, "GL_ONE_MINUS_DST_ALPHA" ) )
	{
		return GLS_SRCBLEND_ONE_MINUS_DST_ALPHA;
	}
	else if ( !Q_stricmp( name, "GL_SRC_ALPHA_SATURATE" ) )
	{
		return GLS_SRCBLEND_ALPHA_SATURATE;
	}

	sprintf(ch, " unknown blend mode '%s', substituting GL_ONE", name);
	LOGGER.log(ch, io::ELL_WARNING);

	return GLS_SRCBLEND_ONE;
}

//---------------------------------------------------------------------------

vid::E_SRC_BLEND_FUNCTION Q3ToMySrcBlendMode( int mode )
{
	if ( mode==GLS_SRCBLEND_ONE )
	{
		return vid::ESBF_ONE;
	}
	else if ( mode==GLS_SRCBLEND_ZERO )
	{
		return vid::ESBF_ZERO;
	}
	else if ( mode==GLS_SRCBLEND_DST_COLOR )
	{
		return vid::ESBF_DST_COLOR;
	}
	else if ( mode==GLS_SRCBLEND_ONE_MINUS_DST_COLOR )
	{
		return vid::ESBF_ONE_MINUS_DST_COLOR;
	}
	else if ( mode==GLS_SRCBLEND_SRC_ALPHA )
	{
		return vid::ESBF_SRC_ALPHA;
	}
	else if ( mode==GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA )
	{
		return vid::ESBF_ONE_MINUS_SRC_ALPHA;
	}
	else if ( mode==GLS_SRCBLEND_DST_ALPHA )
	{
		return vid::ESBF_DST_ALPHA;
	}
	else if ( mode==GLS_SRCBLEND_ONE_MINUS_DST_ALPHA )
	{
		return vid::ESBF_ONE_MINUS_DST_ALPHA;
	}
	else if ( mode==GLS_SRCBLEND_ALPHA_SATURATE )
	{
		return vid::ESBF_SRC_ALPHA_SATURATE;
	}

	return vid::ESBF_ONE;
}

//---------------------------------------------------------------------

int NameToDstBlendMode( const char *name )
{
	if ( !Q_stricmp( name, "GL_ONE" ) )
	{
		return GLS_DSTBLEND_ONE;
	}
	else if ( !Q_stricmp( name, "GL_ZERO" ) )
	{
		return GLS_DSTBLEND_ZERO;
	}
	else if ( !Q_stricmp( name, "GL_SRC_ALPHA" ) )
	{
		return GLS_DSTBLEND_SRC_ALPHA;
	}
	else if ( !Q_stricmp( name, "GL_ONE_MINUS_SRC_ALPHA" ) )
	{
		return GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA;
	}
	else if ( !Q_stricmp( name, "GL_DST_ALPHA" ) )
	{
		return GLS_DSTBLEND_DST_ALPHA;
	}
	else if ( !Q_stricmp( name, "GL_ONE_MINUS_DST_ALPHA" ) )
	{
		return GLS_DSTBLEND_ONE_MINUS_DST_ALPHA;
	}
	else if ( !Q_stricmp( name, "GL_SRC_COLOR" ) )
	{
		return GLS_DSTBLEND_SRC_COLOR;
	}
	else if ( !Q_stricmp( name, "GL_ONE_MINUS_SRC_COLOR" ) )
	{
		return GLS_DSTBLEND_ONE_MINUS_SRC_COLOR;
	}

	sprintf(ch, " unknown blend mode '%s', substituting GL_ONE", name);
	LOGGER.log(ch, io::ELL_WARNING);

	return GLS_DSTBLEND_ONE;
}

//---------------------------------------------------------------------------

vid::E_DST_BLEND_FUNCTION Q3ToMyDstBlendMode( int mode )
{
	if ( mode==GLS_DSTBLEND_ONE )
	{
		return vid::EDBF_ONE;
	}
	else if ( mode==GLS_DSTBLEND_ZERO )
	{
		return vid::EDBF_ZERO;
	}
	else if ( mode==GLS_DSTBLEND_SRC_ALPHA )
	{
		return vid::EDBF_SRC_ALPHA;
	}
	else if ( mode==GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA )
	{
		return vid::EDBF_ONE_MINUS_SRC_ALPHA;
	}
	else if ( mode==GLS_DSTBLEND_DST_ALPHA )
	{
		return vid::EDBF_DST_ALPHA;
	}
	else if ( mode==GLS_DSTBLEND_ONE_MINUS_DST_ALPHA )
	{
		return vid::EDBF_ONE_MINUS_DST_ALPHA;
	}
	else if ( mode==GLS_DSTBLEND_SRC_COLOR )
	{
		return vid::EDBF_SRC_COLOR;
	}
	else if ( mode==GLS_DSTBLEND_ONE_MINUS_SRC_COLOR )
	{
		return vid::EDBF_ONE_MINUS_SRC_COLOR;
	}

	return vid::EDBF_ONE;
}

//---------------------------------------------------------------------------

genFunc_t NameToGenFunc( const char *funcname )
{
	if ( !Q_stricmp( funcname, "sin" ) )
	{
		return GF_SIN;
	}
	else if ( !Q_stricmp( funcname, "square" ) )
	{
		return GF_SQUARE;
	}
	else if ( !Q_stricmp( funcname, "triangle" ) )
	{
		return GF_TRIANGLE;
	}
	else if ( !Q_stricmp( funcname, "sawtooth" ) )
	{
		return GF_SAWTOOTH;
	}
	else if ( !Q_stricmp( funcname, "inversesawtooth" ) )
	{
		return GF_INVERSE_SAWTOOTH;
	}
	else if ( !Q_stricmp( funcname, "noise" ) )
	{
		return GF_NOISE;
	}

	sprintf(ch, "invalid genfunc name '%s'", funcname);
	LOGGER.log(ch, io::ELL_WARNING);

	return GF_SIN;
}

//---------------------------------------------------------------------------
} // end namespace q3
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------