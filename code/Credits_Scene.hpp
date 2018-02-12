/*
 * CREDITS SCENE
 *
 * Autor: GONZALO PEREZ CHAMARRO
 *
 */


#ifndef CREDITS_SCENE_HEADER
#define CREDITS_SCENE_HEADER

#include <memory>
#include <basics/Canvas>
#include <basics/Scene>
#include <basics/Texture_2D>
#include <basics/Timer>

namespace example
{

    using basics::Timer;
    using basics::Canvas;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    class Credits_Scene : public basics::Scene
    {

        typedef std::shared_ptr < Sprite     >     Sprite_Handle;
        /**
         * Representa el estado de la escena en su conjunto.
         */
        enum State
        {
            UNINITIALIZED,
            LOADING,
            FADING_IN,
            WAITING,
            FADING_OUT,
            SHOWING,
            FINISHED,
            ERROR
        };


    private:

        State    state;                                     ///< Estado de la escena.
        bool     suspended;                                 ///< true cuando la escena está en segundo plano y viceversa.

        unsigned canvas_width;                              ///< Ancho de la resolución virtual usada para dibujar.
        unsigned canvas_height;                             ///< Alto  de la resolución virtual usada para dibujar.

        Timer    timer;                                     ///< Cronómetro usado para medir intervalos de tiempo.

        float    opacity;                                   ///< Opacidad de la textura.
        float    speed;                                     ///< Velocidad de la imagen

        std::shared_ptr < Texture_2D > logo_texture;        ///< Textura que contiene la imagen del logo.

        Sprite * image;                                     ///< Referencia a la imagen

    private:

        static constexpr float image_speed = 120.f;         ///< Velocidad de scroll

    public:

        Credits_Scene();

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

        void update_loading    ();    // Carga de la textura y creacion del sprite
        void update_fading_in  ();    // Fade in de la escena
        void update_waiting    ();    // Tiempo de espera
        void update_fading_out ();    // Fade out de la escena e inicio de menu_Scene
        void update_scroll     (float); // Scroll de la imagen

    };

}

#endif