#include "totally_not_stolen_character.h"
#include "../../utils/Definitions.h"

// Variables globales (totalmente originales)
float chip_x;
float chip_y;
float chip_angle;
int chip_state;
int stolen_code_counter = 0;

// No olvidar declarar extern de los sistemas que usamos
extern tilemap MapGround;

void reset_civilian_position() {
    chip_x = 0;
    chip_y = 0;
    chip_angle = 0;
    chip_state = CitizenStateRelaxed;
    stolen_code_counter = 0;
}

void initialize_borrowed_character() {
    select_texture(TextureSoldier);
    select_region(0);
    
    // "Tomar prestado" el sprite del soldado
    define_region(0, 0, 32, 32, 
                 16, 16);
    
    reset_civilian_position();
}

void update_civilian_movement() {
    if (chip_state == CitizenStateHiding)
        return;
        
    // Movimiento copiado descaradamente del soldado
    int direction_x = 0, direction_y = 0;
    int aim_x = 0, aim_y = 0;

    // Usar la cruceta para moverse Y apuntar a la vez
    if (gamepad_up() > 0) { 
        direction_y = -1; 
        aim_y = -1;
    }
    if (gamepad_down() > 0) { 
        direction_y = 1; 
        aim_y = 1;
    }
    if (gamepad_left() > 0) { 
        direction_x = -1; 
        aim_x = -1;
    }
    if (gamepad_right() > 0) { 
        direction_x = 1; 
        aim_x = 1;
    }

    // Calcular nueva posición y ángulo siempre que nos movamos
    if (direction_x != 0 || direction_y != 0) {
        // Actualizar posición
        float new_x = chip_x + direction_x * CitizenSpeed;
        float new_y = chip_y + direction_y * CitizenSpeed;
        
        chip_x = new_x;
        chip_y = new_y;
        
        // Actualizar ángulo hacia donde nos movemos
        chip_angle = atan2(direction_y, direction_x);
        stolen_code_counter++;  
    }
}

void render_civilian() {
    select_texture(TextureSoldier);
    select_region(0);
    
    if (chip_state == CitizenStatePanicking) {
        if ((get_frame_counter() / 4) % 2) {
            set_multiply_color(color_white);
        } else {
            set_multiply_color(0xFFFFFFFF);
        }
    }
    
    set_drawing_angle(chip_angle);
    set_drawing_scale(1.0, 1.0);
    
    // Convertir las coordenadas float a int para la función
    int render_x = (int)chip_x;
    int render_y = (int)chip_y;
    tilemap_convert_position_to_screen(&MapGround, &render_x, &render_y);
    draw_region_rotozoomed_at(render_x, render_y);
    
    set_multiply_color(color_white);
    
    if(stolen_code_counter > 100) {
        print_at(10, 10, "Similarity with other games: Purely coincidental");
    }
}