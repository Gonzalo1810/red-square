/*
 * Credits Scene
 *
 * Autor: GONZALO PEREZ CHAMARRO
 */

#include "Game_Scene.hpp"
#include "Credits_Scene.hpp"
#include "Menu_Scene.hpp"

#include <basics/Canvas>
#include <basics/Director>

using namespace basics;
using namespace std;

namespace example
{
    Credits_Scene::Credits_Scene()
    {
        state         = UNINITIALIZED;
        suspended     = true;
        canvas_width  = 720;
        canvas_height = 1280;
    }

    // ---------------------------------------------------------------------------------------------

    bool Credits_Scene::initialize ()
    {
        if (state == UNINITIALIZED)
        {
            speed = image_speed;
            state = LOADING;
        }
        else
        {
            timer.reset ();

            opacity = 0.f;
            state   = FADING_IN;
        }
        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Credits_Scene::update (float time)
    {
        if (!suspended) switch (state)
            {
                case LOADING:    update_loading    (); break;
                case FADING_IN:  update_fading_in  (); break;
                case WAITING:    update_waiting    (); break;
                case SHOWING:    update_scroll     (time); image->update(time); break;
                case FADING_OUT: update_fading_out (); break;
            }
    }

    // ---------------------------------------------------------------------------------------------

    void Credits_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            //Se busca el canvas si existe, o se crea uno
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            if (!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            if (canvas)
            {
                canvas->clear ();

                if(logo_texture)
                    image->render(*canvas);

            }

        }
    }

    // ---------------------------------------------------------------------------------------------

    void Credits_Scene::update_loading ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {
            // Se carga la textura del logo:

            logo_texture = Texture_2D::create (0, context, "creditos.png");

            // Se comprueba si la textura se ha podido cargar correctamente:

            if (logo_texture)
            {
                context->add (logo_texture);

                timer.reset ();

                opacity = 0.f;
                state   = FADING_IN;
            }
            else
                state   = ERROR;

            //Carga del sprite

            Sprite_Handle image_handle(new Sprite(logo_texture.get()));
            image = image_handle.get();
            image->set_speed_y(speed);
            image->set_position({canvas_width * .5f, -image->get_height() *.5f});
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Credits_Scene::update_fading_in ()
    {
        float elapsed_seconds = timer.get_elapsed_seconds ();

        if (elapsed_seconds < 1.f)
        {
            opacity = elapsed_seconds;      // Se aumenta la opacidad del logo a medida que pasa el tiempo
        }
        else
        {
            timer.reset ();

            opacity = 1.f;

            state   = SHOWING;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Credits_Scene::update_waiting ()
    {
        // Se esperan dos segundos sin hacer nada:

        if (timer.get_elapsed_seconds () > 2.f)
        {
            timer.reset ();


            state = SHOWING;


        }
    }

    // ---------------------------------------------------------------------------------------------

    void Credits_Scene::update_fading_out ()
    {
        float elapsed_seconds = timer.get_elapsed_seconds ();

        if (elapsed_seconds < 1.2f)
        {
            opacity = 1.f - elapsed_seconds * 2.f; // Se reduce la opacidad de 1 a 0 en 1.2 segundos
        }
        else
        {


            state = FINISHED;
            director.run_scene (shared_ptr< Scene >(new Menu_Scene)); //Se inicia Menu_Scene

        }
    }

    // ---------------------------------------------------------------------------------------------

    void Credits_Scene::update_scroll(float time)
    {

        if(image->get_bottom_y() > canvas_height)
        {
            state = FADING_OUT;

        }
    }

}