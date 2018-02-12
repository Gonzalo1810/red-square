/*
 * GAME SCENE
 *
 * Autor: GONZALO PEREZ CHAMARRO
 *
 *
 */


#ifndef GAME_SCENE_HEADER
#define GAME_SCENE_HEADER

    #include <map>
    #include <list>
    #include <memory>
    #include <basics/Canvas>
    #include <basics/Id>
    #include <basics/Scene>
    #include <basics/Texture_2D>
    #include "Sprite.hpp"
    #include <basics/Timer>
#include <basics/internal/Timer.hpp>

namespace example
    {

        using basics::Id;
        using basics::Canvas;
        using basics::Texture_2D;
        using basics::Timer;
        using basics::Atlas;

        class Game_Scene : public basics::Scene
        {

            typedef std::shared_ptr < Sprite     >     Sprite_Handle;
            typedef std::list< Sprite_Handle     >     Sprite_List;
            typedef std::shared_ptr< Texture_2D  >     Texture_Handle;
            typedef std::map< Id, Texture_Handle >     Texture_Map;
            typedef basics::Graphics_Context::Accessor Context;

            /**
             * Estado de la escena
             */
            enum State
            {
                LOADING,
                RUNNING,
                ERROR
            };
            /**
             * Estado del gameplay
             */
            enum Gameplay_State
            {
                WAITING_TO_START,
                PLAYING,
                DEAD
            };

        private:

            static struct   Texture_Data { Id id; const char * path; } textures_data[]; // Array de texturas
            static unsigned textures_count;  // Numero de texturas del array anterior

        public:

            static constexpr float initial_speed = 90.f;  // Velocidad inicial predeterminada

        private:

            State          state;     // Estado de la escena
            Gameplay_State gameplay;  //Estado del gameplay
            bool           paused;    // Indica si esta pausada la escena
            size_t         increment;  // Incremento de velocidad

            unsigned       canvas_width;  // ancho del canvas
            unsigned       canvas_height;  // alto del canvas

            Texture_Map    textures;  // mapa de texturas
            Sprite_List    sprites;   // lista de sprites a pintar
            Sprite_List    pause_sprites;  // lista de sprites del menu de muerte

            //Referencias a sprites autoexplicativas
            Sprite       * top_border;
            Sprite       * bottom_border;
            Sprite       * left_border;
            Sprite       * right_border;

            Sprite       * player;
            Sprite       * enemy1;
            Sprite       * enemy2;
            Sprite       * enemy3;
            Sprite       * enemy4;

            Sprite       * restart;
            Sprite       * menu;

            std::vector <Sprite*>    enemies;  // Vector que agrupara los enemigos

            bool           follow_target;  // Indica si el jugador tiene que moverse
            float          user_target_y;  // Posicion Y de tocado en pantalla
            float          user_target_x;  // Posicion X de tocado en pantalla
            float          actual_speed;   // Velocidad actual de los enemigos

            Timer timer;  // temporizador



        public:

            Game_Scene(size_t);

            basics::Size2u get_view_size () override
            {
                return { canvas_width, canvas_height };
            }

            bool initialize () override;
            void suspend    () override;
            void resume     () override;

            void handle     (basics::Event & event) override;
            void update     (float time) override;
            void render     (Context & context) override;

        private:

            void load_textures    ();  // Carga de texturas
            void create_sprites   ();  // Creacion de los sprites

            void restart_game     ();  // reinicio del juego
            void start_playing    ();  // comienzo del gameplay

            void run_simulation   (float time);  //recoge toda la accion del juego
            void update_ai        ();  // movimiento y colision de la IA
            void update_user      ();  // movimiento y colision del player
            bool check_collisions ();  // comprobacion de las colisiones
            bool enemy_collision();    // colisiones del player con el enemigo
            void ai_start_move(float);  // comienzo del movimiento de la IA
            void increment_velocity();  // incremento de la velocidad actual

            void render_loading   (Canvas & canvas);  // render de sprites en el canvas
            void render_playfield (Canvas & canvas);  // render continnuo de sprites
            void render_pause (Canvas & canvas);  // render del menu de muerte

        };

    }

#endif
