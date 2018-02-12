/*
 * Intro scene
 */

#include "Menu_Scene.hpp"
#include "Help_Scene.hpp"

#include <basics/Canvas>
#include <basics/Director>

using namespace basics;
using namespace std;

namespace example{

    Help_Scene::Texture_Data Help_Scene::textures_data[] =
            {
                    { ID(help1),     "help-scene/start.png" },
                    { ID(help4), "help-scene/collision.png" },  //La segunda ID no me funcionaba aun cambiandola, por eso tengo 4 sin utilizar la 2ª
                    { ID(help2), "help-scene/moving.png" },
                    { ID(help3), "help-scene/collision.png" }
            };

    unsigned Help_Scene::textures_count = sizeof(textures_data) / sizeof(Texture_Data);

    Help_Scene::Help_Scene()
    {
        state         = UNINITIALIZED;
        suspended     = true;
        canvas_width  = 720;
        canvas_height = 1280;
    }

    // ---------------------------------------------------------------------------------------------

    bool Help_Scene::initialize ()
    {
        if (state == UNINITIALIZED)
        {
            help_state = START;
            state = LOADING;
        }
        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Help_Scene::handle(basics::Event &event)
    {
        if (state == SHOWING)
        {
            switch (event.id)
                {
                    case ID(touch-ended):
                    {
                        switch (help_state)
                        {
                            case START:
                                help_state = MOVE;
                                break;
                            case MOVE:
                                help_state = COLLISION;
                                break;
                            case COLLISION:
                                return_main_menu();
                                break;
                        }
                        break;
                    }
                }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Help_Scene::update (float time)
    {
        if (!suspended) switch (state)
            {
                case LOADING:    load_textures    ();   break;
                case SHOWING:
                    switch (help_state)
                    {
                        case START:
                            image_to_show = image;
                            break;
                        case MOVE:
                            image_to_show = image2;
                            break;
                        case COLLISION:
                            image_to_show = image3;
                            break;
                    }

                case ERROR:      break;
            }
    }

    // ---------------------------------------------------------------------------------------------

    void Help_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            if (!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            if (canvas)
            {
                canvas->clear ();

                switch (state)
                {
                    case LOADING: render_loading   (*canvas); break;
                    case SHOWING: render_showing   (*canvas); break;
                    case ERROR:   break;
                }

            }

        }
    }

    void Help_Scene::render_loading(Canvas & canvas)
    {
        Texture_2D * loading_texture = textures[ID(loading)].get ();

        if (loading_texture)
        {
            canvas.fill_rectangle
                    (
                            { canvas_width * .5f, canvas_height * .5f },
                            { loading_texture->get_width (), loading_texture->get_height () },
                            loading_texture
                    );
        }
    }

    void Help_Scene::render_showing(Canvas & canvas)
    {
        image_to_show->render(canvas);
    }


    void Help_Scene::load_textures ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {
            // Se carga las texturas de las imagenes

            Texture_Data   & texture_data = textures_data[textures.size ()];
            Texture_Handle & texture      = textures[texture_data.id] = Texture_2D::create (texture_data.id, context, texture_data.path);

            // Se comprueba si la textura se ha podido cargar correctamente:
            if (texture) context->add (texture); else state = ERROR;

            if (textures.size () == textures_count)
            {
                create_sprite ();
                state = SHOWING;

            }

        }
    }

    void Help_Scene::create_sprite()
    {
        //Creacion de sprites y su referenciacion
        Sprite_Handle         image_handle(new Sprite( textures[ID(help1)].get()));
        image = image_handle.get();
        image->set_position({canvas_width * .5f, canvas_height * .5f});

        Sprite_Handle         image_handle2(new Sprite( textures[ID(help2)].get()));
        image2 = image_handle2.get();
        image2->set_position({canvas_width * .5f, canvas_height * .5f});

        Sprite_Handle         image_handle3(new Sprite( textures[ID(help3)].get()));
        image3 = image_handle3.get();
        image3->set_position({canvas_width * .5f, canvas_height * .5f});

        //Referencia a la imagen que se va a mostrar
        image_to_show = image;
    }


    void Help_Scene::return_main_menu()
    {
        state = FINISHED;

        director.run_scene(shared_ptr< Scene >(new Menu_Scene));
    }

}