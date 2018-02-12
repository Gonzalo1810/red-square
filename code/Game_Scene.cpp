/*
 * GAME SCENE
 *
 * Autor: GONZALO PEREZ CHAMARRO
 *
 */

#include "Game_Scene.hpp"
#include "Menu_Scene.hpp"

#include <cstdlib>
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Log>
#include <basics/Scaling>
#include <basics/Rotation>
#include <basics/Translation>

using namespace basics;
using namespace std;

namespace example
{

    Game_Scene::Texture_Data Game_Scene::textures_data[] =
    {
        { ID(loading),    "game-scene/loading.png"        },
        { ID(vbar),       "game-scene/horizontal-bar.png" },
        { ID(hbar),       "game-scene/vertical-bar.png"   },
        { ID(enemy),      "game-scene/ball.png"    },
        { ID(enemy2),      "game-scene/ball2.png"    },
        { ID(enemy3),      "game-scene/ball3.png"    },
        { ID(enemy4),      "game-scene/ball4.png"    },
        { ID(title),      "game-scene/square-logo.png"    },
        { ID(player),     "game-scene/red-square.png"           },
        { ID(dead),     "game-scene/dead.png"           },
        { ID(reiniciar),     "game-scene/restart.png"           },
        { ID(volver),     "game-scene/menu.png"           },
    };

    unsigned Game_Scene::textures_count = sizeof(textures_data) / sizeof(Texture_Data); // tamaño del array anterior

    // ---------------------------------------------------------------------------------------------

    Game_Scene::Game_Scene(size_t mode)
    {
        canvas_width  = 720;
        canvas_height =  1280;

        srand (unsigned(time(nullptr)));

        // Segun el modo recibido por parametro cambia la dificultad del juego. Por defecto esta en FACIL
        switch(mode){
            case 0:
                increment = 5;
                break;
            case 1:
                increment = 10;
                break;
            case 2:
                increment = 15;
                break;
            default:
                increment = 5;
                break;
        }

        initialize ();
    }

    // ---------------------------------------------------------------------------------------------

    bool Game_Scene::initialize ()
    {
        state    = LOADING;
        paused   = true;
        gameplay = WAITING_TO_START;

        timer.reset();

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::suspend ()
    {
        paused = true;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::resume ()
    {
        paused = false;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::handle (Event & event)
    {
        if (state == RUNNING)
        {
            if (gameplay == WAITING_TO_START)
            {
                start_playing();
                ai_start_move(actual_speed);
            }
            else if(gameplay == PLAYING)
            {
                switch (event.id)
                {
                    case ID(touch-started):
                    {
                        //Solo se moverá el player, si el usuario toca en el player
                        if(*event[ID(y)].as< var::Float > () < player->get_top_y() &&
                           *event[ID(y)].as< var::Float > () > player->get_bottom_y() &&
                           *event[ID(x)].as< var::Float > () < player-> get_right_x() &&
                           *event[ID(x)].as< var::Float > () > player-> get_left_x())
                        {

                            follow_target = true;
                            user_target_y = *event[ID(y)].as< var::Float > ();
                            user_target_x = *event[ID(x)].as<var::Float>();

                        }


                        break;
                    }
                    case ID(touch-moved):
                    {
                        user_target_y = *event[ID(y)].as< var::Float > ();
                        user_target_x = *event[ID(x)].as<var::Float>();

                        break;
                    }

                    case ID(touch-ended):
                    {
                        follow_target = false;
                        break;
                    }
                }
            }else if(gameplay == DEAD)
            {
                switch (event.id)
                {
                    case ID(touch-ended):
                    {
                        // Click en reiniciar
                        if(*event[ID(y)].as< var::Float > () < restart->get_top_y() &&
                           *event[ID(y)].as< var::Float > () > restart->get_bottom_y() &&
                           *event[ID(x)].as< var::Float > () < restart-> get_right_x() &&
                           *event[ID(x)].as< var::Float > () > restart-> get_left_x())
                        {
                            restart_game();
                        }
                        //click en volver al menu principal
                        else if(*event[ID(y)].as< var::Float > () < menu->get_top_y() &&
                                *event[ID(y)].as< var::Float > () > menu->get_bottom_y() &&
                                *event[ID(x)].as< var::Float > () < menu-> get_right_x() &&
                                *event[ID(x)].as< var::Float > () > menu-> get_left_x())
                        {
                            director.run_scene(shared_ptr<Scene> (new Menu_Scene));
                        }
                        break;
                    }
                }
            }

        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::update (float time)
    {
        if (!paused) switch (state)
        {
            case LOADING: load_textures  ();     break;
            case RUNNING: run_simulation (time); break;
            case ERROR:   break;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render (basics::Graphics_Context::Accessor & context)
    {
        if (!paused)
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
                    case RUNNING:
                        render_playfield (*canvas);
                        if(gameplay ==DEAD)
                            render_pause(*canvas);
                        break;
                    case ERROR:   break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::load_textures ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {
            // Se carga la siguiente textura:

            Texture_Data   & texture_data = textures_data[textures.size ()];
            Texture_Handle & texture      = textures[texture_data.id] = Texture_2D::create (texture_data.id, context, texture_data.path);

            // Se comprueba si la textura se ha podido cargar correctamente:

            if (texture) context->add (texture); else state = ERROR;


            // Se han terminado de cargar todas las texturas:

            if (textures.size () == textures_count)
            {
                create_sprites ();
                restart_game   ();

                state = RUNNING;

            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::create_sprites ()
    {
        // Se crean y configuran los sprites del fondo:


        Sprite_Handle         player_handle(new Sprite( textures[ID(player)      ].get () ));
        sprites.push_back (        player_handle);

        Sprite_Handle enemy_handle(new Sprite(textures[ID(enemy)].get()));
        sprites.push_back(enemy_handle);

        Sprite_Handle enemy2_handle(new Sprite(textures[ID(enemy2)].get()));
        sprites.push_back(enemy2_handle);

        Sprite_Handle enemy3_handle(new Sprite(textures[ID(enemy3)].get()));
        sprites.push_back(enemy3_handle);

        Sprite_Handle enemy4_handle(new Sprite(textures[ID(enemy4)].get()));
        sprites.push_back(enemy4_handle);

        //Sprites de las paredes

        Sprite_Handle top_handle(new Sprite(textures[ID(vbar)].get()));
        sprites.push_back(top_handle);

        Sprite_Handle bottom_handle(new Sprite(textures[ID(vbar)].get()));
        sprites.push_back(bottom_handle);

        Sprite_Handle left_handle(new Sprite(textures[ID(hbar)].get()));
        sprites.push_back(left_handle);

        Sprite_Handle right_handle(new Sprite(textures[ID(hbar)].get()));
        sprites.push_back(right_handle);

        //Sprite del titutlo

        Sprite_Handle title_handle(new Sprite(textures[ID(title)].get()));
        sprites.push_back(title_handle);
        title_handle.get()->set_position({canvas_width * .5f, canvas_height - title_handle.get()->get_height()});

        //Sprite del menu de muerte

        Sprite_Handle pause_menu_handle(new Sprite(textures[ID(dead)].get()));
        pause_sprites.push_back(pause_menu_handle);
        pause_menu_handle.get()->set_position({canvas_width * .5f, canvas_height* .5f});

        Sprite_Handle restart_handle(new Sprite(textures[ID(reiniciar)].get()));
        pause_sprites.push_back(restart_handle);
        restart_handle.get()->set_position({canvas_width * .5f,
                                            pause_menu_handle.get()->get_position_y() + restart_handle.get()->get_height()});
        restart = restart_handle.get();

        Sprite_Handle return_menu_handle(new Sprite(textures[ID(volver)].get()));
        pause_sprites.push_back(return_menu_handle);
        return_menu_handle.get()->set_position({restart_handle.get()->get_position_x(),
                                                restart_handle.get()->get_position_y() - restart_handle.get()->get_height()*1.5f});
        menu = return_menu_handle.get();



        // Se guardan punteros a los sprites que se van a usar frecuentemente:

        player          =         player_handle.get ();
        enemy1          =         enemy_handle.get();
        enemy2 = enemy2_handle.get();
        enemy3 = enemy3_handle.get();
        enemy4 = enemy4_handle.get();

        top_border      =         top_handle.get();
        bottom_border   =         bottom_handle.get();
        left_border     =         left_handle.get();
        right_border    =         right_handle.get();

        enemies.push_back(enemy1);
        enemies.push_back(enemy2);
        enemies.push_back(enemy3);
        enemies.push_back(enemy4);

    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::restart_game()
    {
        // Posicionamiento de los sprites en el inico de la escena
        player->set_position ({ canvas_width / 2.f, canvas_height / 2.f - 300});

        enemy1->set_position({canvas_width/3.f, canvas_height / 3.f +100});
        enemy2->set_position({canvas_width/3.f *2, canvas_height/3.f +100});
        enemy3->set_position({canvas_width/3.f, canvas_height/9.f});
        enemy4->set_position({canvas_width/3.f *2 , canvas_height/9.f });


        bottom_border->set_position({canvas_width/2.f,                        bottom_border->get_height() / 2.f});
        left_border->set_position({left_border->get_width(),                  left_border->get_height() / 2.f});
        right_border->set_position({canvas_width - right_border->get_width(), right_border->get_height() /2.f});
        top_border->set_position({canvas_width / 2.f,                         left_border->get_height() - top_border->get_height()/ 2.f });

        for(auto enemy:enemies)
        {
            enemy->set_speed({0,0});
        }

        actual_speed = initial_speed;

        follow_target = false;

        gameplay = WAITING_TO_START;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::start_playing ()
    {
        gameplay = PLAYING;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::run_simulation (float time)
    {
        // Se actualiza el estado de todos los sprites:
        if(gameplay == PLAYING)
        {
            for (auto & sprite : sprites)
            {
                sprite->update (time);
            }

            //Incremento continuo de la velocidad
            increment_velocity();

            //Actualizacion de enemigo y player
            update_ai   ();
            update_user ();

            // Se comprueban las posibles colisiones de la bola con los bordes y con los players:
            if(check_collisions () || enemy_collision())
            {
                gameplay = DEAD;
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::update_ai ()
    {
        //Interseccion de los enemigos con las paredes y su rebote
        for(size_t i = 0; i< enemies.size();i++)
        {
                if (enemies[i]->intersects (*top_border))
                {
                    enemies[i]->set_position_y (top_border->get_bottom_y () - enemies[i]->get_height() / 2.f);
                    enemies[i]->set_speed_y    (-enemies[i]->get_speed_y ());
                }
                if (enemies[i]->intersects (*bottom_border))
                {
                    enemies[i]->set_position_y (bottom_border->get_top_y () + enemies[i]->get_height() / 2.f);
                    enemies[i]->set_speed_y    (-enemies[i]->get_speed_y ());
                }
                if (enemies[i]->intersects (*left_border))
                {
                    enemies[i]->set_position_x (left_border->get_right_x () + enemies[i]->get_width() / 2.f);
                    enemies[i]->set_speed_x    (-enemies[i]->get_speed_x ());
                }
                if (enemies[i]->intersects (*right_border))
                {
                    enemies[i]->set_position_x (right_border->get_left_x () - enemies[i]->get_width() / 2.f);
                    enemies[i]->set_speed_x    (-enemies[i]->get_speed_x ());
                }

        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::update_user ()
    {
        //Seguimiento del dedo del jugador
        if (follow_target)
        {
            player->set_position_x(user_target_x);
            player->set_position_y(user_target_y);
        }

    }

    // ---------------------------------------------------------------------------------------------

    bool Game_Scene::check_collisions ()
    {
        //Se comprueba si el jugador se choca con las paredes y se devuelve true en tal caso
        return (player->intersects(*top_border) || player->intersects(*bottom_border)
                || player->intersects(*left_border) || player->intersects(*right_border));
    }

    //----------------------------------------------------------------------------------------------

    bool Game_Scene::enemy_collision()
    {
        //Devuelve true si el player cholisiona con algun enemigo
        bool temp = false;
        for(size_t i = 0; i<enemies.size() && !temp;i++)
        {
            if(player->intersects(*enemies[i]))
                temp = true;
        }

        return temp;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render_loading (Canvas & canvas)
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

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render_playfield (Canvas & canvas)
    {
        for (auto & sprite : sprites)
        {
            sprite->render (canvas);
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render_pause(Canvas &canvas)
    {
        for (auto & sprite : pause_sprites)
        {
            sprite->render (canvas);
        }

    }


    //----------------------------------------------------------------------------------------------

    void Game_Scene::ai_start_move(float speed)
    {
        enemy1->set_speed({speed,-speed});

        enemy2->set_speed({-speed,-speed});

        enemy3->set_speed({speed, speed});

        enemy4->set_speed({-speed,speed});
    }

    //----------------------------------------------------------------------------------------------

    void Game_Scene::increment_velocity()
    {
        if(timer.get_elapsed_seconds() > 1.f)  // Al paso de 1 segundo se incrementa la velocidad
        {
            timer.reset();
            actual_speed += increment;
        }

        for(auto enemy:enemies)
        {
            enemy->set_global_speed(actual_speed);
        }
    }


}
