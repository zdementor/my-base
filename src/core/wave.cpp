//|-------------------------------------------------------------------------
//| File:        wave.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

//---------------------------------------------------------------------------
namespace my {
namespace core { 
//---------------------------------------------------------------------------

// waves stuff

static const s32 FUNCTABLE_SIZE	 = 8192;
static const s32 FUNCTABLE_MASK	 = (FUNCTABLE_SIZE-1);

#define	WAVEVALUE( time, table, base, amplitude, phase, freq ) \
((base) + table[ (s32)( ( ( (phase) + time * (freq) ) * FUNCTABLE_SIZE ) ) & FUNCTABLE_MASK ] * (amplitude))

//! functions tables
static f32 sinTable[FUNCTABLE_SIZE];
static f32 squareTable[FUNCTABLE_SIZE];
static f32 triangleTable[FUNCTABLE_SIZE];
static f32 sawToothTable[FUNCTABLE_SIZE];
static f32 inverseSawToothTable[FUNCTABLE_SIZE];

//---------------------------------------------------------------------------

// noise stuff

static const s32 NOISE_SIZE = 256;
static const s32 NOISE_MASK = ( NOISE_SIZE - 1 );

#define VAL( a ) s_noise_perm[ ( a ) & ( NOISE_MASK )]
#define INDEX( x, y, z, t ) VAL( x + VAL( y + VAL( z + VAL( t ) ) ) )
#define LERP( a, b, w ) ( a * ( 1.0f - w ) + b * w )

static f32 s_noise_table[NOISE_SIZE];
static s32 s_noise_perm[NOISE_SIZE];

//---------------------------------------------------------------------------

static int randSeed = 0;

//---------------------------------------------------------------------------

void srand( unsigned seed ) 
{
	randSeed = seed;
}

//---------------------------------------------------------------------------

int	rand( void ) 
{
	randSeed = (69069 * randSeed + 1);
	return randSeed & 0x7fff;
}

//---------------------------------------------------------------------------

class CFuncTablesInitializator
{
public:
	CFuncTablesInitializator()
	{
		s32 i;

		// init function tables

		for ( i = 0; i < FUNCTABLE_SIZE; i++ )
		{
			sinTable[i] = sin( 
				core::DEG2RAD * ( i*360.0f / ( (f32)(FUNCTABLE_SIZE-1) ) ) 
				);
			squareTable[i]	 = ( i < FUNCTABLE_SIZE/2 ) ? 1.0f : -1.0f;
			sawToothTable[i] = (float)i / FUNCTABLE_SIZE;
			inverseSawToothTable[i] = 1.0f - sawToothTable[i];

			if ( i < FUNCTABLE_SIZE / 2 )
			{
				if ( i < FUNCTABLE_SIZE / 4 )
				{
					triangleTable[i] = ( float ) i / ( FUNCTABLE_SIZE / 4 );
				}
				else
				{
					triangleTable[i] = 1.0f - triangleTable[i-FUNCTABLE_SIZE / 4];
				}
			}
			else
			{
				triangleTable[i] = -triangleTable[i-FUNCTABLE_SIZE/2];
			}
		}

		// Noise Init

		srand( 1001 );

		for ( i = 0; i < NOISE_SIZE; i++ )
		{
			s_noise_table[i] = ( float ) ( ( ( rand() / ( float ) RAND_MAX ) * 2.0 - 1.0 ) );
			s_noise_perm[i] = ( unsigned char ) ( rand() / ( float ) RAND_MAX * 255 );
		}
	}
}
FuncTablesInitializator;

//---------------------------------------------------------------------------

static float GetNoiseValue( int x, int y, int z, int t )
{
	int index = INDEX( ( int ) x, ( int ) y, ( int ) z, ( int ) t );

	return s_noise_table[index];
}

//---------------------------------------------------------------------------

float R_NoiseGet4f( float x, float y, float z, float t )
{
	int i;
	int ix, iy, iz, it;
	float fx, fy, fz, ft;
	float front[4];
	float back[4];
	float fvalue, bvalue, value[2], finalvalue;

	ix = ( int ) floor( x );
	fx = x - ix;
	iy = ( int ) floor( y );
	fy = y - iy;
	iz = ( int ) floor( z );
	fz = z - iz;
	it = ( int ) floor( t );
	ft = t - it;

	for ( i = 0; i < 2; i++ )
	{
		front[0] = GetNoiseValue( ix, iy, iz, it + i );
		front[1] = GetNoiseValue( ix+1, iy, iz, it + i );
		front[2] = GetNoiseValue( ix, iy+1, iz, it + i );
		front[3] = GetNoiseValue( ix+1, iy+1, iz, it + i );

		back[0] = GetNoiseValue( ix, iy, iz + 1, it + i );
		back[1] = GetNoiseValue( ix+1, iy, iz + 1, it + i );
		back[2] = GetNoiseValue( ix, iy+1, iz + 1, it + i );
		back[3] = GetNoiseValue( ix+1, iy+1, iz + 1, it + i );

		fvalue = LERP( LERP( front[0], front[1], fx ), LERP( front[2], front[3], fx ), fy );
		bvalue = LERP( LERP( back[0], back[1], fx ), LERP( back[2], back[3], fx ), fy );

		value[i] = LERP( fvalue, bvalue, fz );
	}

	finalvalue = LERP( value[0], value[1], ft );

	return finalvalue;
}

//---------------------------------------------------------------------------

//! Evaluates a given wave, referencing backEnd.refdef.time directly
static float EvalWaveForm(u32 time, const wave *wf)
{
	f32 ftime = 0.001f*time;

	if (wf->func==EFT_TRIANGLE)
	{
		return WAVEVALUE( ftime, triangleTable, wf->base, wf->amplitude, wf->phase, wf->frequency );
	}
	else
	if (wf->func==EFT_SQUARE)
	{
		return WAVEVALUE( ftime, squareTable, wf->base, wf->amplitude, wf->phase, wf->frequency );
	}
	else
	if (wf->func==EFT_SAWTOOTH)
	{
		return WAVEVALUE( ftime, sawToothTable, wf->base, wf->amplitude, wf->phase, wf->frequency );
	}
	else
	if (wf->func==EFT_INVERSE_SAWTOOTH)
	{
		return WAVEVALUE( ftime, inverseSawToothTable, wf->base, wf->amplitude, wf->phase, wf->frequency );
	}
	else
	if (wf->func==EFT_SIN)
	{
		return WAVEVALUE( ftime, sinTable, wf->base, wf->amplitude, wf->phase, wf->frequency );
	}
	else
	if  (wf->func==EFT_NOISE)
	{
		return wf->base + R_NoiseGet4f( 0, 0, 0, ( ftime + wf->phase ) * wf->frequency ) * wf->amplitude;
	}

	return 0.0f;
}

//---------------------------------------------------------------------------

static float EvalWaveFormClamped( u32 time, const wave *wf )
{
	float glow  = EvalWaveForm( time, wf );

	if ( glow < 0 )
	{
		return 0;
	}

	if ( glow > 1 )
	{
		return 1;
	}

	return glow;
}

//---------------------------------------------------------------------------

//! getting current wave value
inline f32 wave::get_value(u32 curr_time_ms) const
{
	return EvalWaveForm(curr_time_ms, this);
}

//---------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------


