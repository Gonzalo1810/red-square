/*
 * MODE SCENE
 *
 * Autor: GONZALO PEREZ CHAMARRO
 *
 */

#include "Game_Scene.hpp"
#include "Mode_Scene.hpp"

#include <basics/Canvas>
#include <basics/Director>

using namespace basics;
using namespace std;

namespace example
{

    Mode_Scene::Mode_Scene()
    {
        state         = LOADING;
        suspended     = true;
        canvas_width  = 720;
        canvas_height =  1280;
    }

    // ---------------------------------------------------------------------------------------------

    bool Mode_Scene::initialize ()
    {
        for (auto & option : options)
        {
            option.is_pressed = false;
        }

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Mode_Scene::handle (basics::Event & event)
    {
        if (state == READY)                     // Se descartan los eventos cuando la escena está LOADING
        {
            switch (event.id)
            {
                case ID(touch-started):         // El usuario toca la pantalla
                case ID(touch-moved):
                {
                    // Se determina qué opción se ha tocado:

                    Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };
                    int     option_touched = option_at (touch_location);

                    // Solo se puede tocar una opción a la vez (para evitar selecciones múltiples),
                    // por lo que solo una se considera presionada (el resto se "sueltan"):

                    for (int index = 1; index < number_of_options; ++index)
                    {
                        options[index].is_pressed = index == option_touched;
                    }

                    break;
                }

                case ID(touch-ended):           // El usuario deja de tocar la pantalla
                {
                    // Se "sueltan" todas las opciones:

                    for (auto & option : options) option.is_pressed = false;

                    // Se determina qué opción se ha dejado de tocar la última y se actúa como corresponda:

                    Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                    // Inicia Game_Scene pasandole un parametro segun la dificultad
                    if (option_at (touch_location) == FACIL)
                    {
                        director.run_scene (shared_ptr< Scene >(new Game_Scene{0}));
                    }
                    if (option_at (touch_location) == NORMAL)
                    {
                        director.run_scene (shared_ptr< Scene >(new Game_Scene{1}));
                    }
                    if (option_at (touch_location) == DIFICIL)
                    {
                        director.run_scene (shared_ptr< Scene >(new Game_Scene{2}));
                    }

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Mode_Scene::update (float time)
    {
        if (!suspended) if (state == LOADING)
            {
                Graphics_Context::Accessor context = director.lock_graphics_context ();

                if (context)
                {
                    // Se carga el atlas:

                    atlas.reset (new Atlas("mode-scene/modes.sprites", context));

                    // Si el atlas se ha podido cargar el estado es READY y, en otro caso, es ERROR:

                    state = atlas->good () ? READY : ERROR;

                    // Si el atlas está disponible, se inicializan los datos de las opciones del menú:

                    if (state == READY)
                    {
                        configure_options ();
                    }
                }
            }
    }

    // ---------------------------------------------------------------------------------------------

    void Mode_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            // El canvas se puede haber creado previamente, en cuyo caso solo hay que pedirlo:

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            // Si no se ha creado previamente, hay que crearlo una vez:

            if (!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // Si el canvas se ha podido obtener o crear, se puede dibujar con él:

            if (canvas)
            {
                canvas->clear ();

                if (state == READY)
                {
                    // Se dibuja el slice de cada una de las opciones del menú:

                    for (auto & option : options)
                    {
                        canvas->set_transform
                                (
                                        scale_then_translate_2d
                                                (
                                                        option.is_pressed ? 0.75f : 1.f,              // Escala de la opción
                                                        { option.position[0], option.position[1] }      // Traslación
                                                )
                                );

                        canvas->fill_rectangle ({ 0.f, 0.f }, { option.slice->width, option.slice->height }, option.slice, CENTER | TOP);
                    }

                    // Se restablece la transformación aplicada a las opciones para que no afecte a
                    // dibujos posteriores realizados con el mismo canvas:

                    canvas->set_transform (Transformation2f());
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Mode_Scene::configure_options ()
    {
        // Se asigna un slice del atlas a cada opción del menú según su ID:

        options[FACIL   ].slice = atlas->get_slice (ID(FACIL)   );
        options[NORMAL ].slice = atlas->get_slice (ID(NORMAL) );
        options[DIFICIL   ].slice = atlas->get_slice (ID(DIFICIL)   );


        // Se calcula la altura total del menú:

        float menu_height = 0;

        for (auto & option : options) menu_height += option.slice->height;

        // Se calcula la posición del borde superior del menú en su conjunto de modo que
        // quede centrado verticalmente:

        float option_top = canvas_height / 2.f + menu_height / 2.f;

        // Se establece la posición del borde superior de cada opción:

        for (unsigned index = 0; index < number_of_options; ++index)
        {
            options[index].position = Point2f{ canvas_width / 2.f, option_top };

            option_top -= options[index].slice->height;
        }

        // Se restablece la presión de cada opción:

        initialize ();
    }

    // ---------------------------------------------------------------------------------------------

    int Mode_Scene::option_at (const Point2f & point)
    {
        for (int index = 0; index < number_of_options; ++index)
        {
            const Option & option = options[index];

            if
                    (
                    point[0] > option.position[0] - option.slice->width  &&
                    point[0] < option.position[0] + option.slice->width  &&
                    point[1] > option.position[1] - option.slice->height &&
                    point[1] < option.position[1] + option.slice->height
                    )
            {
                return index;
            }
        }

        return -1;
    }


}