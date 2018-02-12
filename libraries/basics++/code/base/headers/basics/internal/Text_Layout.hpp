/*
 * TEXT LAYOUT
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 *
 * C1802030123
 */

#ifndef BASICS_TEXT_LAYOUT_HEADER
#define BASICS_TEXT_LAYOUT_HEADER

    #include <string>
    #include <vector>
    #include <basics/Font>
    #include <basics/Point>
    #include <basics/Size>

    namespace basics
    {

        class Text_Layout
        {
        public:

            struct Glyph
            {
                const Font::Character * character;
                Point2f position;
                Size2f  size;
            };

            typedef std::vector< Glyph > Glyph_List;

        private:

            const Font * font;
            Glyph_List   glyphs;

        public:

            Text_Layout(const Font & font, const std::wstring & text);

        public:

            const Glyph_List & get_glyphs () const
            {
                return glyphs;
            }

        };

    }

#endif
