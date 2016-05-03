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

/*
	This class is discussed in the following forum-thread: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=6&t=37296

	Changes by Michael Zeilfelder to adapt it to HCraft:
	- Remove ustring (otherwise I risk converting strings twice in my project as they are UCS-2 or UCS-4 already)
	- Remove const-ref in places where it makes no sense
	- Remove additional create functions and keep just one. Pass only the parameters actually needed to it.
	- Removing allocater as I get memory-leaks and don't understand what the allocater was meant for.
	- Glyph_Pages no longer mutable (doesn't seem to need that)
	- remove textscenenodes. Probably shouldn't be in this class and make understanding it harder.
	- remove createTextureFromChar (it also had a memory leak probably as it wasn't dropping pageholder)
	- remove Glyphs.set_free_when_destroyed(false). That function prevents clearing the array - it's not about pointers. Rather fix SGUITTGlyph (which didn't care about rule of three).
	- don't drop the driver (not grabbed - but I changed that stuff a lot so might not have been there originally)
	- remove a few memory re-allocations in drawing
	- CGUITTGlyphPage::updateTexture replaced dirtyflag by check for glpyhs to handle
	- CGUITTGlyphPage::updateTexture works with ::texture->getSize() instead of getOriginalSize. Same result in this case, but more correct.
	
	TODO:
	- Hinting should be one enum with explanation (have to figure it out first, results are strange currently when I enable it)
	- max_font_height shouldn't have to be calculated each time. Set test-characters when font-size can change. And update each time getHeightFromCharacter is called.
*/

#ifndef __C_GUI_TTFONT_H_INCLUDED__
#define __C_GUI_TTFONT_H_INCLUDED__

#include <ft2build.h>
#include FT_FREETYPE_H	// official way to include freetype.h correct according to freetype documenation
#include <irrlicht.h>

// TODO: Shouldn't be in irr namespace as long as it's not in the engine (but leaving it for now in the hope that will change)
namespace irr
{
namespace gui
{
	struct SGUITTFace;
	class CGUITTFont;

	//! Structure representing a single TrueType glyph.
	struct SGUITTGlyph
	{
		//! Constructor.
		SGUITTGlyph() : isLoaded(false), glyph_page(0), surface(0), parent(0) {}

		//! Destructor.
		~SGUITTGlyph() { unload(); }

		SGUITTGlyph(const SGUITTGlyph &other): isLoaded(false), glyph_page(0), surface(0), parent(0)
		{
			*this = other;
		}

		SGUITTGlyph& operator=(const SGUITTGlyph &other)
		{
			if ( &other != this )
			{
				isLoaded = other.isLoaded;
				glyph_page = other.glyph_page;

				if ( other.surface )
					other.surface->grab();
				if ( surface )
					surface->drop();
				surface = other.surface;
				parent = other.parent;
			}
			return *this;
		}

		//! Preload the glyph.
		//!	The preload process occurs when the program tries to cache the glyph from FT_Library.
		//! However, it simply defines the SGUITTGlyph's properties and will only create the page
		//! textures if necessary.  The actual creation of the textures should only occur right
		//! before the batch draw call.
		void preload(u32 char_index, FT_Face face, video::IVideoDriver* driver, u32 font_size, const FT_Int32 loadFlags);

		//! Unloads the glyph.
		void unload();

		//! Creates the IImage object from the FT_Bitmap.
		video::IImage* createGlyphImage(const FT_Bitmap& bits, video::IVideoDriver* driver) const;

		//! If true, the glyph has been loaded.
		bool isLoaded;

		//! The page the glyph is on.
		u32 glyph_page;

		//! The source rectangle for the glyph.
		core::recti source_rect;

		//! The offset of glyph when drawn.
		core::vector2di offset;

		//! Glyph advance information.
		FT_Vector advance;

		//! This is just the temporary image holder.  After this glyph is paged,
		//! it will be dropped.
		mutable video::IImage* surface;

		//! The pointer pointing to the parent (CGUITTFont)
		CGUITTFont* parent;
	};

	//! Holds a sheet of glyphs.
	class CGUITTGlyphPage
	{
		public:
			CGUITTGlyphPage(video::IVideoDriver* Driver, const io::path& texture_name) :texture(0), available_slots(0), used_slots(0), driver(Driver), name(texture_name) {}
			~CGUITTGlyphPage()
			{
				if (texture)
				{
					if (driver)
						driver->removeTexture(texture);
					else texture->drop();
				}
			}

			//! Create the actual page texture,
			bool createPageTexture(const u8& pixel_mode, const core::dimension2du& texture_size)
			{
				if( texture )
					return false;
				
				bool flgmip = driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
				driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
				bool allowMemCpy = driver->getTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY);
				driver->setTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY, true);
#endif

				// Set the texture color format.
				switch (pixel_mode)
				{
					case FT_PIXEL_MODE_MONO:
						texture = driver->addTexture(texture_size, name, video::ECF_A1R5G5B5);
						break;
					case FT_PIXEL_MODE_GRAY:
					default:
						texture = driver->addTexture(texture_size, name, video::ECF_A8R8G8B8);
						break;
				}

				// Restore our texture creation flags.
#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)				
				driver->setTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY, allowMemCpy);
#endif				
				driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, flgmip);
				return texture ? true : false;
			}

			//! Add the glyph to a list of glyphs to be paged.
			//! This collection will be cleared after updateTexture is called.
			void pushGlyphToBePaged(const SGUITTGlyph* glyph)
			{
				glyph_to_be_paged.push_back(glyph);
			}

			//! Updates the texture atlas with new glyphs.
			void updateTexture()
			{
				if (glyph_to_be_paged.empty())
					return;

				void* ptr = texture->lock();
				video::ECOLOR_FORMAT format = texture->getColorFormat();
				core::dimension2du size = texture->getSize();

				video::IImage* pageholder = driver->createImageFromData(format, size, ptr, true, false);

				for (u32 i = 0; i < glyph_to_be_paged.size(); ++i)
				{
					const SGUITTGlyph* glyph = glyph_to_be_paged[i];
					if (glyph && glyph->isLoaded)
					{
						if (glyph->surface)
						{
							glyph->surface->copyTo(pageholder, glyph->source_rect.UpperLeftCorner);
							glyph->surface->drop();
							glyph->surface = 0;
						}
						else
						{
							; // TODO: add error message?
							//currently, if we failed to create the image, just ignore this operation.
						}
					}
				}

				texture->unlock();
				pageholder->drop();
				glyph_to_be_paged.clear();
			}

			video::ITexture* texture;
			u32 available_slots;
			u32 used_slots;

			core::array<core::vector2di> render_positions;
			core::array<core::recti> render_source_rects;

		private:
			core::array<const SGUITTGlyph*> glyph_to_be_paged;
			video::IVideoDriver* driver;
			io::path name;
	};

	//! Class representing a TrueType font.
	class CGUITTFont : public irr::gui::IGUIFont
	{
		public:
			//! Creates a new TrueType font and returns a pointer to it.  The pointer must be drop()'ed when finished.
			//! \param driver Irrlicht video driver
			//! \param fileSystem Irrlicht filesystem
			//! \param filename The filename of the font.
			//! \param size The size of the font glyphs in pixels.  Since this is the size of the individual glyphs, the true height of the font may change depending on the characters used.
			//! \param antialias set the use_monochrome (opposite to antialias) flag
			//! \param transparency set the use_transparency flag
			//! \param invisibleChars Set characters which don't need drawing (speed optimization)
			//! \param logger Irrlicht logging, for printing out additinal warnings/errors
			//! \return Returns a pointer to a CGUITTFont.  Will return 0 if the font failed to load.
			static CGUITTFont* createTTFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fileSystem, const io::path& filename, u32 size, bool antialias = true, bool transparency = true, const wchar_t *invisibleChars = L" ", irr::ILogger* logger = 0);

			//! Destructor
			virtual ~CGUITTFont();
		

			//! Draws some text and clips it to the specified rectangle if wanted.
			virtual void draw(const core::stringw& text, const core::rect<s32>& position,
				video::SColor color, bool hcenter=false, bool vcenter=false,
				const core::rect<s32>* clip=0);

			//! Returns the dimension of a character produced by this font.
			virtual core::dimension2d<u32> getCharDimension(const wchar_t ch) const;

			//! Returns the dimension of a text string.
			virtual core::dimension2d<u32> getDimension(const wchar_t* text) const;

			//! Calculates the index of the character in the text which is on a specific position.
			virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x) const;

			//! Sets global kerning width for the font.
			virtual void setKerningWidth(s32 kerning);

			//! Sets global kerning height for the font.
			virtual void setKerningHeight(s32 kerning);

			//! Gets kerning values (distance between letters) for the font. If no parameters are provided,
			virtual s32 getKerningWidth(const wchar_t* thisLetter=0, const wchar_t* previousLetter=0) const;

			//! Returns the distance between letters
			virtual s32 getKerningHeight() const;
		
			//! Define which characters should not be drawn by the font.
			/** This is a speed optimization. For example spaces don't draw anything in most fonts.
			So making them invisible save the render-time for those. Instead an empty space with 
			their width is added to the output. */
			virtual void setInvisibleCharacters(const wchar_t *s);



			//! Sets the amount of glyphs to batch load.
			void setBatchLoadSize(u32 batch_size) { batch_load_size = batch_size; }

			//! Sets the maximum texture size for a page of glyphs.
			void setMaxPageTextureSize(const core::dimension2du& texture_size) { max_page_texture_size = texture_size; }

			//! Get the font size.
			u32 getFontSize() const { return size; }

			//! Check the font's transparency.
			bool isTransparent() const { return use_transparency; }

			//! Check if the font auto-hinting is enabled.
			//! Auto-hinting is FreeType's built-in font hinting engine.
			bool useAutoHinting() const { return use_auto_hinting; }

			//! Check if the font hinting is enabled.
			bool useHinting()	 const { return use_hinting; }

			//! Check if the font is being loaded as a monochrome font.
			//! The font can either be a 256 color grayscale font, or a 2 color monochrome font.
			bool useMonochrome()  const { return use_monochrome; }

			//! Tells the font to allow transparency when rendering.
			//! Default: true.
			//! \param flag If true, the font draws using transparency.
			void setTransparency(const bool flag);

			//! Tells the font to use monochrome rendering.
			//! Default: false.
			//! \param flag If true, the font draws using a monochrome image.  If false, the font uses a grayscale image.
			void setMonochrome(const bool flag);

			//! Enables or disables font hinting.
			//! Default: Hinting and auto-hinting true.
			//! \param enable If false, font hinting is turned off. If true, font hinting is turned on.
			//! \param enable_auto_hinting If true, FreeType uses its own auto-hinting algorithm.  If false, it tries to use the algorithm specified by the font.
			void setFontHinting(const bool enable, const bool enable_auto_hinting = true);
			
			//! Get the last glyph page if there's still available slots.
			//! If not, it will return zero.
			CGUITTGlyphPage* getLastGlyphPage() const;

			//! Create a new glyph page texture.
			//! \param pixel_mode the pixel mode defined by FT_Pixel_Mode
			//should be better typed. fix later.
			CGUITTGlyphPage* createGlyphPage(const u8& pixel_mode);

			//! Get the last glyph page's index.
			u32 getLastGlyphPageIndex() const { return Glyph_Pages.size() - 1; }

			//! This function is for debugging mostly. If the page doesn't exist it returns zero.
			//! \param page_index Simply return the texture handle of a given page index.
			video::ITexture* getPageTextureByIndex(u32 page_index) const;

		protected:
			bool use_monochrome;
			bool use_transparency;
			bool use_hinting;
			bool use_auto_hinting;
			u32 size;
			u32 batch_load_size;
			core::dimension2du max_page_texture_size;

		private:
			// Manages the FreeType library.
			static FT_Library c_library;
			static core::map<io::path, SGUITTFace*> c_faces;
			static bool c_libraryLoaded;

			CGUITTFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fileSystem);
			bool load(const io::path& filename, u32 size, bool antialias, bool transparency);
			void reset_images();
			void update_glyph_pages() const;
			void update_load_flags()
			{
				// Set up our loading flags.
				load_flags = FT_LOAD_DEFAULT | FT_LOAD_RENDER;
				if (!useHinting()) load_flags |= FT_LOAD_NO_HINTING;
				if (!useAutoHinting()) load_flags |= FT_LOAD_NO_AUTOHINT;
				if (useMonochrome()) load_flags |= FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO | FT_RENDER_MODE_MONO;
				else load_flags |= FT_LOAD_TARGET_NORMAL;
			}
			u32 getWidthFromCharacter(wchar_t c) const;
			u32 getHeightFromCharacter(wchar_t c) const;
			u32 getGlyphIndexByChar(wchar_t c) const;
			core::vector2di getKerning(const wchar_t thisLetter, const wchar_t previousLetter) const;
			core::dimension2d<u32> getDimensionUntilEndOfLine(const wchar_t* p) const;

			irr::video::IVideoDriver* Driver;
			irr::io::IFileSystem* FileSystem;
			irr::ILogger* Logger;

			io::path filename;
			FT_Face tt_face;
			FT_Size_Metrics font_metrics;
			FT_Int32 load_flags;

			core::array<CGUITTGlyphPage*> Glyph_Pages;
			mutable core::array<SGUITTGlyph> Glyphs;

			s32 GlobalKerningWidth;
			s32 GlobalKerningHeight;
			core::stringw Invisible;
	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_TTFONT_H_INCLUDED__
