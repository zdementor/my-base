//|-------------------------------------------------------------------------
//| File:        rlepack.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef rlepackHPP
#define rlepackHPP

//-----------------------------------------------------------
namespace my {
namespace core {
//-----------------------------------------------------------

__MY_CORE_LIB_API__ int rle_encode (
    unsigned char *in_buf,  int in_buf_size,
    unsigned char *out_buf, int out_buf_size
    );

__MY_CORE_LIB_API__ int rle_decode (
    unsigned char *in_buf,  int in_buf_size,
    unsigned char *out_buf, int out_buf_size
    );

//-----------------------------------------------------------
} // end namespace core
} // end namespace my
//-----------------------------------------------------------

#endif