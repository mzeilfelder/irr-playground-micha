#pragma once

#include "irrlicht.h"

// Load texture in hdr format
extern irr::video::ITexture* getHdrTexture(irr::video::IVideoDriver* driver, const irr::io::path& filename);
