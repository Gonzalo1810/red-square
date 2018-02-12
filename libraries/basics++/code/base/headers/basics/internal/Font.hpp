/*
 * FONT
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 *
 * C1802030037
 */

#ifndef BASICS_FONT_HEADER
#define BASICS_FONT_HEADER

    #include <string>

    namespace basics
    {

        class Font
        {
        public:

            struct Character
            {
            };

        protected:

            std::string name;

        public:

            Font() = default;
            virtual ~Font() = default;

        public:

            const std::string & get_name () const
            {
                return name;
            }

        };

    }

#endif
