/*
   CGUITTFont FreeType class for Irrlicht
   Copyright (c) 2009-2010 John Norman

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any
   purpose, including commercial applications, and to alter it and
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

   The original version of this class can be located at:
   http://irrlicht.suckerfreegames.com/

   John Norman
   john@suckerfreegames.com
*/

#include "CGUITTFont.h"

namespace irr
{
namespace gui
{

// --- SGUITTFace ---
// Manages the FT_Face cache.
struct SGUITTFace : public virtual irr::IReferenceCounted
{
	SGUITTFace() : FaceBuffer(0), FaceBufferSize(0)
	{
		memset((void*)&Face, 0, sizeof(FT_Face));
	}

	~SGUITTFace()
	{
		FT_Done_Face(Face);
		delete[] FaceBuffer;
	}

	FT_Face Face;
	FT_Byte* FaceBuffer;
	FT_Long FaceBufferSize;
};

// --- Static variables ---
FT_Library CGUITTFont::c_library;
core::map<io::path, SGUITTFace*> CGUITTFont::c_faces;
bool CGUITTFont::c_libraryLoaded = false;


// --- SGUITTGlyph ---

video::IImage* SGUITTGlyph::createGlyphImage(const FT_Bitmap& bits, video::IVideoDriver* driver) const
{
	// Determine what our texture size should be.
	// Add 1 because textures are inclusive-exclusive.
	core::dimension2du d(bits.width + 1, bits.rows + 1);
	core::dimension2du texture_size;
	//core::dimension2du texture_size(bits.width + 1, bits.rows + 1);

	// Create and load our image now.
	video::IImage* image = 0;
	switch (bits.pixel_mode)
	{
		case FT_PIXEL_MODE_MONO:
		{
			// Create a blank image and fill it with transparent pixels.
			texture_size = d.getOptimalSize(true, true);
			image = driver->createImage(video::ECF_A1R5G5B5, texture_size);
			image->fill(video::SColor(0, 255, 255, 255));

			// Load the monochrome data in.
			const u32 image_pitch = image->getPitch() / sizeof(u16);
#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)			
			u16* image_data = (u16*)image->getData();
#else
			u16* image_data = (u16*)image->lock();
#endif			
			u8* glyph_data = bits.buffer;
			for (unsigned int y = 0; y < bits.rows; ++y)
			{
				u16* row = image_data;
				for (unsigned int x = 0; x < bits.width; ++x)
				{
					// Monochrome bitmaps store 8 pixels per byte.  The left-most pixel is the bit 0x80.
					// So, we go through the data each bit at a time.
					if ((glyph_data[y * bits.pitch + (x / 8)] & (0x80 >> (x % 8))) != 0)
						*row = 0xFFFF;
					++row;
				}
				image_data += image_pitch;
			}
			break;
		}

		case FT_PIXEL_MODE_GRAY:
		{
			// Create our blank image.
			texture_size = d.getOptimalSize(!driver->queryFeature(video::EVDF_TEXTURE_NPOT), !driver->queryFeature(video::EVDF_TEXTURE_NSQUARE), true, 0);
			image = driver->createImage(video::ECF_A8R8G8B8, texture_size);
			image->fill(video::SColor(0, 255, 255, 255));

			// Load the grayscale data in.
			const float gray_count = static_cast<float>(bits.num_grays);
			const u32 image_pitch = image->getPitch() / sizeof(u32);
#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)			
			u32* image_data = (u32*)image->getData();
#else
			u32* image_data = (u32*)image->lock();
#endif
			u8* glyph_data = bits.buffer;
			for (unsigned int  y = 0; y < bits.rows; ++y)
			{
				u8* row = glyph_data;
				for (unsigned int  x = 0; x < bits.width; ++x)
				{
					image_data[y * image_pitch + x] |= static_cast<u32>(255.0f * (static_cast<float>(*row++) / gray_count)) << 24;
					//data[y * image_pitch + x] |= ((u32)(*bitsdata++) << 24);
				}
				glyph_data += bits.pitch;
			}
			break;
		}
		default:
			// TODO: error message?
			return 0;
	}
	return image;
}

void SGUITTGlyph::preload(u32 char_index, FT_Face face, video::IVideoDriver* driver, u32 font_size, const FT_Int32 loadFlags)
{
	if (IsLoaded) 
		return;

	// Set the size of the glyph.
	FT_Set_Pixel_Sizes(face, 0, font_size);

	// Attempt to load the glyph.
	if (FT_Load_Glyph(face, char_index, loadFlags) != FT_Err_Ok)
	{
		// TODO: error message?
		return;
	}

	FT_GlyphSlot glyph = face->glyph;
	FT_Bitmap bits = glyph->bitmap;

	// Setup the glyph information here:
	Advance = glyph->advance;
	Offset = core::vector2di(glyph->bitmap_left, glyph->bitmap_top);

	// Try to get the last page with available slots.
	CGUITTGlyphPage* page = Parent->getLastGlyphPage();

	// If we need to make a new page, do that now.
	if (!page)
	{
		page = Parent->createGlyphPage(bits.pixel_mode);
		if (!page)
		{
			// TODO: add error message?
			return;
		}
	}

	GlyphPage = Parent->getLastGlyphPageIndex();
	u32 texture_side_length = page->Texture->getOriginalSize().Width;
	core::vector2di page_position(
		(page->UsedSlots % (texture_side_length / font_size)) * font_size,
		(page->UsedSlots / (texture_side_length / font_size)) * font_size
		);
	SourceRect.UpperLeftCorner = page_position;
	SourceRect.LowerRightCorner = core::vector2di(page_position.X + bits.width, page_position.Y + bits.rows);

	++page->UsedSlots;
	--page->AvailableSlots;

	// We grab the glyph bitmap here so the data won't be removed when the next glyph is loaded.
	Surface = createGlyphImage(bits, driver);

	// Set our glyph as loaded.
	IsLoaded = true;
}

void SGUITTGlyph::unload()
{
	if (Surface)
	{
		Surface->drop();
		Surface = 0;
	}
	IsLoaded = false;
}


// --- CGUITTGlyphPage ---

bool CGUITTGlyphPage::createPageTexture(const u8& pixelMode, const core::dimension2du& textureSize)
{
	if ( Texture )
		return false;
	
	bool flgmip = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
	bool allowMemCpy = Driver->getTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY);
	Driver->setTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY, true);
#endif

	// Set the texture color format.
	switch (pixelMode)
	{
		case FT_PIXEL_MODE_MONO:
			Texture = Driver->addTexture(textureSize, Name, video::ECF_A1R5G5B5);
			break;
		case FT_PIXEL_MODE_GRAY:
		default:
			Texture = Driver->addTexture(textureSize, Name, video::ECF_A8R8G8B8);
			break;
	}

	// Restore our texture creation flags.
#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)				
	Driver->setTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY, allowMemCpy);
#endif				
	Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, flgmip);
	
	return Texture ? true : false;
}

void CGUITTGlyphPage::updateTexture()
{
	if (GlyphsToBePaged.empty())
		return;

	void* ptr = Texture->lock();
	video::ECOLOR_FORMAT format = Texture->getColorFormat();
	core::dimension2du size = Texture->getSize();

	video::IImage* pageholder = Driver->createImageFromData(format, size, ptr, true, false);

	for (u32 i = 0; i < GlyphsToBePaged.size(); ++i)
	{
		const SGUITTGlyph* glyph = GlyphsToBePaged[i];
		if (glyph && glyph->IsLoaded)
		{
			if (glyph->Surface)
			{
				glyph->Surface->copyTo(pageholder, glyph->SourceRect.UpperLeftCorner);
				glyph->Surface->drop();
				glyph->Surface = 0;
			}
			else
			{
				; // TODO: add error message?
				//currently, if we failed to create the image, just ignore this operation.
			}
		}
	}

	Texture->unlock();
	pageholder->drop();
	GlyphsToBePaged.clear();
}



// --- CGUITTFont ---

CGUITTFont* CGUITTFont::createTTFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fileSystem, const io::path& filename, u32 size, bool antialias, bool transparency, const wchar_t *invisibleChars, irr::ILogger* logger)
{
	if ( !driver )
		return 0;

	if (!c_libraryLoaded)
	{
		if (FT_Init_FreeType(&c_library))
			return 0;
		c_libraryLoaded = true;
	}

	CGUITTFont* font = new CGUITTFont(driver, fileSystem);
	font->Logger = logger;
	bool ret = font->load(filename, size, antialias, transparency);
	if (!ret)
	{
		font->drop();
		return 0;
	}
	
	font->setInvisibleCharacters(invisibleChars);

	return font;
}

//! Constructor.
CGUITTFont::CGUITTFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fileSystem)
: UseMonochrome(false), UseTransparency(true), UseHinting(false), UseAutoHinting(false),
BatchLoadSize(1), Driver(driver), FileSystem(fileSystem), Logger(0), GlobalKerningWidth(0), GlobalKerningHeight(0)
{
	#ifdef _DEBUG
	setDebugName("CGUITTFont");
	#endif
}

bool CGUITTFont::load(const io::path& filename, u32 size, bool antialias, bool transparency)
{
	// Some sanity checks.
	if (Driver == 0) return false;
	if (size == 0) return false;
	if (filename.size() == 0) return false;

	this->Size = size;
	this->Filename = filename;

	// Update the font loading flags when the font is first loaded.
	this->UseMonochrome = !antialias;
	this->UseTransparency = transparency;
	update_load_flags();

	// Log.
	if (Logger)
		Logger->log(L"CGUITTFont", core::stringw(core::stringw(L"Creating new font: ") + core::stringw(filename) + L" " + core::stringc(size) + L"pt " + (antialias ? L"+antialias " : L"-antialias ") + (transparency ? L"+transparency" : L"-transparency")).c_str(), irr::ELL_INFORMATION);

	// Grab the face.
	SGUITTFace* face = 0;
	core::map<io::path, SGUITTFace*>::Node* node = c_faces.find(filename);
	if (node == 0)
	{
		face = new SGUITTFace();
		c_faces.set(filename, face);

		if (FileSystem)
		{
			// Read in the file data.
			io::IReadFile* file = FileSystem->createAndOpenFile(filename);
			if (!file)
			{
				if (Logger) 
					Logger->log(L"CGUITTFont - failed to open the file.", core::stringw(filename).c_str(), irr::ELL_INFORMATION);

				c_faces.remove(filename);
				delete face;
				face = 0;
				return false;
			}
			face->FaceBuffer = new FT_Byte[file->getSize()];
			file->read(face->FaceBuffer, file->getSize());
			face->FaceBufferSize = file->getSize();
			file->drop();

			// Create the face.
			if (FT_New_Memory_Face(c_library, face->FaceBuffer, face->FaceBufferSize, 0, &face->Face))
			{
				if (Logger) 
					Logger->log(L"CGUITTFont", L"FT_New_Memory_Face failed.", irr::ELL_INFORMATION);

				c_faces.remove(filename);
				delete face;
				face = 0;
				return false;
			}
		}
		else
		{
			core::stringc converter(filename);
			if (FT_New_Face(c_library, converter.c_str(), 0, &face->Face))
			{
				if (Logger) 
					Logger->log(L"CGUITTFont", L"FT_New_Face failed.", irr::ELL_INFORMATION);

				c_faces.remove(filename);
				delete face;
				face = 0;
				return false;
			}
		}
	}
	else
	{
		// Using another instance of this face.
		face = node->getValue();
		face->grab();
	}

	// Store our face.
	TTface = face->Face;

	// Store font metrics.
	FT_Set_Pixel_Sizes(TTface, Size, 0);
	FontMetrics = TTface->size->metrics;

	// Allocate our glyphs.
	Glyphs.clear();
	Glyphs.set_used(TTface->num_glyphs);
	for (FT_Long i = 0; i < TTface->num_glyphs; ++i)
	{
		Glyphs[i] = SGUITTGlyph(this);
	}

	// Cache the first 127 ascii characters.
	u32 old_size = BatchLoadSize;
	BatchLoadSize = 127;
	getGlyphIndexByChar((wchar_t)0);
	BatchLoadSize = old_size;

	return true;
}

CGUITTFont::~CGUITTFont()
{
	// Delete the glyphs and glyph pages.
	reset_images();

	// We aren't using this face anymore.
	core::map<io::path, SGUITTFace*>::Node* n = c_faces.find(Filename);
	if (n)
	{
		SGUITTFace* f = n->getValue();

		// Drop our face.  If this was the last face, the destructor will clean up.
		if (f->drop())
			c_faces.remove(Filename);

		// If there are no more faces referenced by FreeType, clean up.
		if (c_faces.size() == 0)
		{
			FT_Done_FreeType(c_library);
			c_libraryLoaded = false;
		}
	}
}

void CGUITTFont::reset_images()
{
	// Delete the glyphs.
	for (u32 i = 0; i != Glyphs.size(); ++i)
		Glyphs[i].unload();

	// Unload the glyph pages from video memory.
	for (u32 i = 0; i != GlyphPages.size(); ++i)
		delete GlyphPages[i];
	GlyphPages.clear();

	// Always update the internal FreeType loading flags after resetting.
	update_load_flags();
}

void CGUITTFont::update_glyph_pages() const
{
	for (u32 i = 0; i != GlyphPages.size(); ++i)
	{
		GlyphPages[i]->updateTexture();
	}
}

CGUITTGlyphPage* CGUITTFont::getLastGlyphPage() const
{
	if (!GlyphPages.empty())
	{
		CGUITTGlyphPage* page = GlyphPages[getLastGlyphPageIndex()];
		if (page->AvailableSlots != 0)
			return page;
	}
	return 0;
}

CGUITTGlyphPage* CGUITTFont::createGlyphPage(const u8& pixel_mode)
{
	CGUITTGlyphPage* page = 0;

	// Name of our page.
	io::path name("TTFontGlyphPage_");
	name += TTface->family_name;
	name += ".";
	name += TTface->style_name;
	name += ".";
	name += Size;
	name += "_";
	name += GlyphPages.size(); // The newly created page will be at the end of the collection.

	// Create the new page.
	page = new CGUITTGlyphPage(Driver, name);

	// Determine our maximum texture size.
	// If we keep getting 0, set it to 1024x1024, as that number is pretty safe.
	core::dimension2du max_texture_size = MaxPageTextureSize;
	if (max_texture_size.Width == 0 || max_texture_size.Height == 0)
		max_texture_size = Driver->getMaxTextureSize();
	if (max_texture_size.Width == 0 || max_texture_size.Height == 0)
		max_texture_size = core::dimension2du(1024, 1024);

	// We want to try to put at least 144 glyphs on a single texture.
	core::dimension2du page_texture_size;
	if (Size <= 21) page_texture_size = core::dimension2du(256, 256);
	else if (Size <= 42) page_texture_size = core::dimension2du(512, 512);
	else if (Size <= 84) page_texture_size = core::dimension2du(1024, 1024);
	else if (Size <= 168) page_texture_size = core::dimension2du(2048, 2048);
	else page_texture_size = core::dimension2du(4096, 4096);

	if (page_texture_size.Width > max_texture_size.Width || page_texture_size.Height > max_texture_size.Height)
		page_texture_size = max_texture_size;

	if (!page->createPageTexture(pixel_mode, page_texture_size))
		// TODO: add error message?
		return 0;

	if (page)
	{
		// Determine the number of glyph slots on the page and add it to the list of pages.
		page->AvailableSlots = (page_texture_size.Width / Size) * (page_texture_size.Height / Size);
		GlyphPages.push_back(page);
	}
	return page;
}

void CGUITTFont::setTransparency(const bool flag)
{
	UseTransparency = flag;
	reset_images();
}

void CGUITTFont::setMonochrome(const bool flag)
{
	UseMonochrome = flag;
	reset_images();
}

void CGUITTFont::setFontHinting(const bool enable, const bool enable_auto_hinting)
{
	UseHinting = enable;
	UseAutoHinting = enable_auto_hinting;
	reset_images();
}

void CGUITTFont::draw(const core::stringw& text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	// Clear the glyph pages of their render information.
	for (u32 i = 0; i < GlyphPages.size(); ++i)
	{
		GlyphPages[i]->RenderPositions.set_used(0);
		GlyphPages[i]->RenderSourceRects.set_used(0);
	}

	// Set up some variables.
	core::dimension2d<s32> textDimension;
	core::position2d<s32> offset = position.UpperLeftCorner;

	// Determine offset positions.
	if (hcenter || vcenter)
	{
		textDimension = getDimension(text.c_str());

		if (hcenter)
			offset.X = ((position.getWidth() - textDimension.Width) >> 1) + offset.X;

		if (vcenter)
			offset.Y = ((position.getHeight() - textDimension.Height) >> 1) + offset.Y;
	}

	// Start parsing characters.
	u32 n;
	wchar_t previousChar = 0;
	const wchar_t * iter = text.c_str();
	while (*iter)
	{
		wchar_t currentChar = *iter;
		n = getGlyphIndexByChar(currentChar);
		bool visible = (Invisible.findFirst(currentChar) == -1);
		if (visible)
		{
			bool lineBreak=false;
			if (currentChar == L'\r') // Mac or Windows breaks
			{
				lineBreak = true;
				if (*(iter + 1) == L'\n')	// Windows line breaks.
					currentChar = *(++iter);
			}
			else if (currentChar == L'\n') // Unix breaks
			{
				lineBreak = true;
			}

			if (lineBreak)
			{
				previousChar = 0;
				offset.Y += FontMetrics.ascender / 64;
				offset.X = position.UpperLeftCorner.X;

				if (hcenter)
					offset.X += (position.getWidth() - textDimension.Width) >> 1;
				++iter;
				continue;
			}
			
			if ( n > 0 )
			{
				SGUITTGlyph& glyph = Glyphs[n-1];

				// Calculate the glyph offset.
				s32 offx = glyph.Offset.X;
				s32 offy = (FontMetrics.ascender / 64) - glyph.Offset.Y;

				// Apply kerning.
				core::vector2di k = getKerning(currentChar, previousChar);
				offset.X += k.X;
				offset.Y += k.Y;

				// Determine rendering information.
				CGUITTGlyphPage* const page = GlyphPages[glyph.GlyphPage];
				page->RenderPositions.push_back(core::position2di(offset.X + offx, offset.Y + offy));
				page->RenderSourceRects.push_back(glyph.SourceRect);
			}
		}
		offset.X += getWidthFromCharacter(currentChar);

		previousChar = currentChar;
		++iter;
	}

	// Draw now.
	update_glyph_pages();
	if (!UseTransparency)
		color.color |= 0xff000000;
	for (u32 i = 0; i < GlyphPages.size(); ++i)
	{
		CGUITTGlyphPage* page = GlyphPages[i];

		if ( !page->RenderPositions.empty() )
		{
			Driver->draw2DImageBatch(page->Texture, page->RenderPositions, page->RenderSourceRects, clip, color, true);
		}
	}
}

core::dimension2d<u32> CGUITTFont::getCharDimension(const wchar_t ch) const
{
	return core::dimension2d<u32>(getWidthFromCharacter(ch), getHeightFromCharacter(ch));
}

core::dimension2d<u32> CGUITTFont::getDimension(const wchar_t* text) const
{
	// Get the maximum font height.  Unfortunately, we have to do this hack as
	// Irrlicht will draw things wrong.  In FreeType, the font size is the
	// maximum size for a single glyph, but that glyph may hang "under" the
	// draw line, increasing the total font height to beyond the set size.
	// Irrlicht does not understand this concept when drawing fonts.  Also, I
	// add +1 to give it a 1 pixel blank border.  This makes things like
	// tooltips look nicer.
	s32 test1 = getHeightFromCharacter(L'g') + 1;
	s32 test2 = getHeightFromCharacter(L'j') + 1;
	s32 test3 = getHeightFromCharacter(L'_') + 1;
	s32 max_font_height = core::max_(test1, core::max_(test2, test3));

	core::dimension2d<u32> text_dimension(0, max_font_height);
	core::dimension2d<u32> line(0, max_font_height);

	wchar_t previousChar = 0;
	for(const wchar_t* iter = text; *iter; ++iter)
	{
		bool lineBreak = false;
		if (*iter == L'\r')	// Mac or Windows line breaks.
		{
			lineBreak = true;
			if (*(iter + 1) == L'\n')
			{
				++iter;
			}
		}
		else if (*iter == L'\n')	// Unix line breaks.
		{
			lineBreak = true;
		}

		// Kerning.
		core::vector2di k = getKerning(*iter, previousChar);
		line.Width += k.X;
		previousChar = *iter;

		// Check for linebreak.
		if (lineBreak)
		{
			previousChar = 0;
			text_dimension.Height += line.Height;
			if (text_dimension.Width < line.Width)
				text_dimension.Width = line.Width;
			line.Width = 0;
			line.Height = max_font_height;
			continue;
		}
		line.Width += getWidthFromCharacter(*iter);
	}
	if (text_dimension.Width < line.Width)
		text_dimension.Width = line.Width;

	return text_dimension;
}

inline u32 CGUITTFont::getWidthFromCharacter(wchar_t c) const
{
	// Set the size of the face.
	// This is because we cache faces and the face may have been set to a different size.
	//FT_Set_Pixel_Sizes(tt_face, 0, size);

	u32 n = getGlyphIndexByChar(c);
	if (n > 0)
	{
		int w = Glyphs[n-1].Advance.x / 64;
		return w;
	}
	if (c >= 0x2000)
		return (FontMetrics.ascender / 64);
	else 
		return (FontMetrics.ascender / 64) / 2;
}

inline u32 CGUITTFont::getHeightFromCharacter(wchar_t c) const
{
	// Set the size of the face.
	// This is because we cache faces and the face may have been set to a different size.
	//FT_Set_Pixel_Sizes(tt_face, 0, size);

	u32 n = getGlyphIndexByChar(c);
	if (n > 0)
	{
		// Grab the true height of the character, taking into account underhanging glyphs.
		s32 height = (FontMetrics.ascender / 64) - Glyphs[n-1].Offset.Y + Glyphs[n-1].SourceRect.getHeight();
		return height;
	}
	if (c >= 0x2000)
		return (FontMetrics.ascender / 64);
	else 
		return (FontMetrics.ascender / 64) / 2;
}

u32 CGUITTFont::getGlyphIndexByChar(wchar_t c) const
{
	// Get the glyph.
	u32 glyph = FT_Get_Char_Index(TTface, c);

	// If our glyph is already loaded, don't bother doing any batch loading code.
	if (glyph != 0 && Glyphs[glyph - 1].IsLoaded)
		return glyph;

	// Determine our batch loading positions.
	u32 half_size = (BatchLoadSize / 2);
	u32 start_pos = 0;
	if ((u32)c > half_size)
		start_pos = c - half_size;
	u32 end_pos = start_pos + BatchLoadSize;

	// Load all our characters.
	do
	{
		// Get the character we are going to load.
		u32 char_index = FT_Get_Char_Index(TTface, start_pos);

		// If the glyph hasn't been loaded yet, do it now.
		if (char_index)
		{
			SGUITTGlyph& glyph = Glyphs[char_index - 1];
			if (!glyph.IsLoaded)
			{
				glyph.preload(char_index, TTface, Driver, Size, LoadFlags);
				GlyphPages[glyph.GlyphPage]->pushGlyphToBePaged(&glyph);
			}
		}
	}
	while (++start_pos < end_pos);

	// Return our original character.
	return glyph;
}

s32 CGUITTFont::getCharacterFromPos(const wchar_t* text, s32 pixel_x) const
{
	s32 x = 0;

	u32 character = 0;
	wchar_t previousChar = 0;
	while (text[character])
	{
		wchar_t c = text[character];
		x += getWidthFromCharacter(c);

		// Kerning.
		core::vector2di k = getKerning(c, previousChar);
		x += k.X;

		if (x >= pixel_x)
			return character;

		previousChar = c;
		++character;
	}

	return -1;
}

void CGUITTFont::setKerningWidth(s32 kerning)
{
	GlobalKerningWidth = kerning;
}

void CGUITTFont::setKerningHeight(s32 kerning)
{
	GlobalKerningHeight = kerning;
}

s32 CGUITTFont::getKerningWidth(const wchar_t* thisLetter, const wchar_t* previousLetter) const
{
	if (TTface == 0)
		return GlobalKerningWidth;
	if (thisLetter == 0 || previousLetter == 0)
		return 0;

	// Return only the kerning width.
	return getKerning(*thisLetter, *previousLetter).X;
}

s32 CGUITTFont::getKerningHeight() const
{
	// FreeType 2 currently doesn't return any height kerning information.
	return GlobalKerningHeight;
}

core::vector2di CGUITTFont::getKerning(const wchar_t thisLetter, const wchar_t previousLetter) const
{
	if (TTface == 0 || thisLetter == 0 || previousLetter == 0)
		return core::vector2di();

	// Set the size of the face.
	// This is because we cache faces and the face may have been set to a different size.
	FT_Set_Pixel_Sizes(TTface, 0, Size);

	core::vector2di ret(GlobalKerningWidth, GlobalKerningHeight);

	// If we don't have kerning, no point in continuing.
	if (!FT_HAS_KERNING(TTface))
		return ret;

	// Get the kerning information.
	FT_Vector v;
	FT_Get_Kerning(TTface, getGlyphIndexByChar(previousLetter), getGlyphIndexByChar(thisLetter), FT_KERNING_DEFAULT, &v);

	// If we have a scalable font, the return value will be in font points.
	if (FT_IS_SCALABLE(TTface))
	{
		// Font points, so divide by 64.
		ret.X += (v.x / 64);
		ret.Y += (v.y / 64);
	}
	else
	{
		// Pixel units.
		ret.X += v.x;
		ret.Y += v.y;
	}
	return ret;
}

void CGUITTFont::setInvisibleCharacters(const wchar_t *s)
{
	Invisible = s;
}

video::ITexture* CGUITTFont::getPageTextureByIndex(u32 page_index) const
{
	if (page_index < GlyphPages.size())
		return GlyphPages[page_index]->Texture;
	return 0;
}

core::dimension2d<u32> CGUITTFont::getDimensionUntilEndOfLine(const wchar_t* p) const
{
	core::stringw s;
	for (const wchar_t* temp = p; temp && *temp != '\0' && *temp != L'\r' && *temp != L'\n'; ++temp )
		s.append(*temp);

	return getDimension(s.c_str());
}

} // end namespace gui
} // end namespace irr
