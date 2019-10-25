// Some idea to handle the "float's on unaligned memory positions" troubles on some big endian platforms.
// http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=52538

#ifndef __IRR_TPACK1_H_INCLUDED__
#define __IRR_TPACK1_H_INCLUDED__

#include "irrTypes.h"

// byte-align structures
#include "irrpack.h"

namespace irr
{

//! Used to create versions of structs with packing set to 1 (no gaps)
//! This is mainly useful for binary serialization
template <typename T>
struct TPack1
{
    T Packed;
} PACK_STRUCT;

//! To use instead of f32 inside structs for binary serializing.
/**  The problem with floats on certain (big endian) platforms
is that they can't be used when they are on non-aligned memory positions.
Accessing a float like that creates an alignment exception.
But when loading them with packed structs we can't avoid in some formats 
which didn't care enough about alignment that this happens.
So instead we have to load them into this union and copy them later to the 
unpacked version.
*/
union UnalignedFloat32 
{
	u32 uval;
    u8 cval[4];
    f32 fval;
};

} // end namespace irr

// Default alignment
#include "irrunpack.h"

#endif
