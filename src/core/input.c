#include "input.h"
#include "../utils/definitions.h"
#include "../entities/character/Totally_not_stolen_character.h"
#include "../entities/character/Totally_not_stolen_character.c"

void initialize_input() {
    // Si necesitamos alguna inicialización de input
}

void process_input() {
    // Procesar input global
    select_gamepad(0);
    
    // Por ahora solo procesamos el input del personaje
    update_civilian_movement();
    
    // En el futuro:
    // if(is_player_in_vehicle)
    //    update_car();
    // else
    //    update_civilian_movement();
}