/*
 * HELP SCENE
 *
 * Autor: GONZALO PEREZ CHAMARRO
 *
 *
 */


#ifndef HELP_SCENE_HEADER
#define HELP_SCENE_HEADER

#include <memory>
#include <basics/Canvas>
#include <basics/Scene>
#include <basics/Id>
#include <basics/Texture_2D>
#include <basics/Timer>
#include "Sprite.hpp"

namespace example
{
    using basics::Id;
    using basics::Canvas;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    class Help_Scene : public basics::Scene
    {

        typedef std::shared_ptr < Sprite     >     Sprite_Handle;
        typedef std::shared_ptr< Texture_2D  >     Texture_Handle;
        typedef std::map< Id, Texture_Handle >     Texture_Map;

        /**
         * Representa el estado de la escena en su conjunto.
         */
        enum State
        {
            UNINITIALIZED,
            LOADING,
            SHOWING,
            FINISHED,
            ERROR
        };
        /**
         * Estado de visual de la escena
         */
        enum Help
        {
            START,
            MOVE,
            COLLISION
        };

    private:

        static struct   Texture_Data { Id id; const char * path; } textures_data[];  // Array de texturas
        static unsigned textures_count;  // tamaño del array anterior


    private:

        State    state;                                     ///< Estado de la escena.
        Help     help_state;                                ///< Estado de visual de la escena
        bool     suspended;                                 ///< true cuando la escena está en segundo plano y viceversa.

        Texture_Map    textures;                            ///< Mapa de texturas

        // Referencias a sprites
        Sprite * image;
        Sprite * image2;
        Sprite * image3;

        Sprite * image_to_show;                             ///< La imagen que se va a mostrar en la escena

        unsigned canvas_width;                              ///< Ancho de la resolución virtual usada para dibujar.
        unsigned canvas_height;                             ///< Alto  de la resolución virtual usada para dibujar.

        std::shared_ptr < Texture_2D > logo_texture;        ///< Textura que contiene la imagen del logo.



    public:

        Help_Scene();

        /**
         * Este método lo llama Director para conocer la resolución virtual con la que está
         * trabajando la escena.
         * @return Tamaño en coordenadas virtuales que está usando la escena.
         */
        basics::Size2u get_view_size () override
        {
            return { canvas_width, canvas_height };
        }

        /**
         * Aquí se inicializan los atributos que deben restablecerse cada vez que se inicia la escena.
         * @return
         */
        bool initialize () override;

        /**
         * Este método lo invoca Director automáticamente cuando el juego pasa a segundo plano.
         */
        void suspend () override
        {
            suspended = true;
        }

        /**
         * Este método lo invoca Director automáticamente cuando el juego pasa a primer plano.
         */
        void resume () override
        {
            suspended = false;
        }

        void handle     (basics::Event & event) override;

        /**
         * Este método se invoca automáticamente una vez por fotograma para que la escena
         * actualize su estado.
         */
        void update (float time) override;

        /**
         * Este método se invoca automáticamente una vez por fotograma para que la escena
         * dibuje su contenido.
         */
        void render (Graphics_Context::Accessor & context) override;

    private:

        void load_textures     ();  // Carga de las texturas
        void create_sprite        (); // creacion de los sprites

        void render_loading(Canvas &);  // render de sprites en la scena
        void render_showing(Canvas &);  // render continuo de la imagen en la escena

        void return_main_menu  ();    // vuelta al menu principal
    };

}

#endif