#ifndef DEFINITELY_ORIGINAL_CHARACTER_H
#define DEFINITELY_ORIGINAL_CHARACTER_H

// Cualquier parecido con Ocean Storm es pura coincidencia
#define CitizenWidth 32
#define CitizenHeight 32
#define CitizenSpeed 2.0  // Más lento que el soldado, es un civil!
#define CitizenStateRelaxed 1  // Era StateActive
#define CitizenStatePanicking 2  // Era StateBlinking
#define CitizenStateHiding 3   // Era StateImmune

extern float chip_x;
extern float chip_y;
extern float chip_angle;
extern int chip_state;
extern int stolen_code_counter;

void initialize_borrowed_character();
void reset_civilian_position();
void update_civilian_movement();
void render_civilian();

#endif