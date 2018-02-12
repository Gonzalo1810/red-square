/*
 * RASTER FONT
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 *
 * C1802030037
 */

#ifndef BASICS_RASTER_FONT_HEADER
#define BASICS_RASTER_FONT_HEADER

    #include <unordered_map>
    #include <basics/Atlas>
    #include <basics/Font>
    #include <basics/Vector>

    namespace basics
    {

        class Raster_Font : public Font
        {
        public:

            struct Character : public Font::Character
            {
                Atlas::Slice * slice;
                Vector2f       offset;
                float          advance;
            };

        private:

            typedef std::unordered_map< uint32_t, Character > Character_Map;

        private:

            Character_Map character_map;

            Atlas atlas;

            struct
            {
                float line_height;
                float base_height;
            }
            metrics;

        public:

            Raster_Font(const std::string & path, Graphics_Context::Accessor & context);

        };

    }

#endif
