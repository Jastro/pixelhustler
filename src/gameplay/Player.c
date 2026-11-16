// *****************************************************************************
    // include Vircon32 headers
    #include "input.h"
    #include "math.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
    #include "../../include/gameplay/Weapons.h"
    #include "../../include/gameplay/Traffic.h"
    #include "../../include/gameplay/Police.h"
    #include "../../include/gameplay/Corpses.h"
    #include "../../include/gameplay/Explosions.h"
// *****************************************************************************


// ---------------------------------------------------------
//   PLAYER VARIABLES
// ---------------------------------------------------------

float PlayerX;
float PlayerY;
float PlayerAngle;
bool IsPlayerMoving = false;
int PlayerFrame = 0;
int PlayerAnimTimer = 0;

// soldier_health, soldier_immunity_timer, soldier_blink_timer from OceanStorm.
// soldier_armor and soldier_stamina were left in the ocean.
int PlayerHealth = PLAYER_MAX_HEALTH;
int PlayerImmunityTimer = 0;
int PlayerDownTimer = 0;
int PlayerDownReason = DownWasted;
int PlayerStunTimer = 0;
float PlayerKnockVX = 0;
float PlayerKnockVY = 0;


bool PlayerHitsCar( float x, float y )
{
    float min_distance2 = (CAR_CIRCLE_RADIUS + PED_RADIUS) * (CAR_CIRCLE_RADIUS + PED_RADIUS);

    for( int i = 0; i < MaxCars; i++ )
    {
        float fx = sin( CarAngle[ i ] ) * CAR_CIRCLE_SPACING;
        float fy = -cos( CarAngle[ i ] ) * CAR_CIRCLE_SPACING;

        for( int c = -1; c <= 1; c++ )
        {
            float dx = x - (CarX[ i ] + c * fx);
            float dy = y - (CarY[ i ] + c * fy);

            if( dx * dx + dy * dy < min_distance2 )
              return true;
        }
    }

    return false;
}

bool PlayerCanMoveTo( float x, float y )
{
    if( IsSolidAt( x, y ) )
      return false;

    if( PlayerHitsCar( PlayerX, PlayerY ) )
      return true;

    return !PlayerHitsCar( x, y );
}


// ---------------------------------------------------------
//   INITIALIZATION
// ---------------------------------------------------------
// reset_soldier() from OceanStorm, "original" edition

void InitializePlayer()
{
    // PlayerX = StartingX;   // Jastro: copied from OceanStorm. StartingX was the aircraft carrier.
    //                        // Player spawned in the ocean. There is no ocean. Player spawned in the void.
    PlayerX = EntranceX[ EntranceHome ];
    PlayerY = EntranceY[ EntranceHome ] + 12;
    PlayerAngle = pi / 2;
    IsPlayerMoving = false;
    PlayerFrame = 0;
    PlayerAnimTimer = 0;
    IsPlayerInCar = false;
    TargetFloorZ = FLOOR_Z_ON_FOOT;
    PlayerHealth = PLAYER_MAX_HEALTH;
    PlayerImmunityTimer = 0;
    PlayerDownTimer = 0;
    PlayerStunTimer = 0;
    PlayerKnockVX = 0;
    PlayerKnockVY = 0;
}


// ---------------------------------------------------------
//   UPDATE
// ---------------------------------------------------------
// update_soldier() from OceanStorm, minus swimming and bombs.
// Removed features changelog (Jastro):
//   - swimming  ("it's a city")
//   - bombs     ("coming back as DLC")
//   - drowning  ("ok this one was Carra")
// Restored features changelog (Jastro):
//   - bullets   (uncommented 14 lines. "I didn't have to touch much. It's called being efficient")

void UpdatePlayer()
{
    select_gamepad( 0 );

    if( PlayerStunTimer > 0 )
    {
        PlayerStunTimer--;

        if( !IsSolidAt( PlayerX + PlayerKnockVX, PlayerY ) ) PlayerX += PlayerKnockVX;
        else PlayerKnockVX = 0;

        if( !IsSolidAt( PlayerX, PlayerY + PlayerKnockVY ) ) PlayerY += PlayerKnockVY;
        else PlayerKnockVY = 0;

        PlayerKnockVX *= 0.85;
        PlayerKnockVY *= 0.85;
        UpdateCamera( PlayerX, PlayerY );
        return;
    }

    // BUG #7 (closed): "pressing B enters and exits the car in the same frame"
    //   Reported by: Jastro
    //   Jastro's analysis: "the car is haunted"
    //   Root cause (Carra): player and car were both updated in the same
    //   frame and both read the same B press. Fixed with the return below.
    //   Jastro's reaction: "so it WASN'T haunted?" (visibly disappointed)
    if( gamepad_button_b() == 1 )
    {
        if( TryEnterNearestCar() )
          return;
    }

    int direction_x, direction_y;
    gamepad_direction( &direction_x, &direction_y );

    // (this time the else branch is different; reviewed by Carra, twice)
    float new_x = PlayerX + direction_x * CHAR_SPEED;
    if( PlayerCanMoveTo( new_x, PlayerY ) )
      PlayerX = new_x;

    float new_y = PlayerY + direction_y * CHAR_SPEED;
    if( PlayerCanMoveTo( PlayerX, new_y ) )
      PlayerY = new_y;

    IsPlayerMoving = (direction_x != 0 || direction_y != 0);

    if( IsPlayerMoving )
    {
        PlayerAngle = atan2( direction_y, direction_x );

        // Jastro's first attempt reused the heli rotor animation:
        //   PlayerFrame = 1 - PlayerFrame;   // toggles 0/1 every frame
        // Result: the character vibrated at 60 Hz. QA filed it as "possessed".
        PlayerAnimTimer++;
        if( PlayerAnimTimer >= PED_ANIM_SPEED )
        {
            PlayerAnimTimer = 0;
            PlayerFrame = (PlayerFrame + 1) % FramesPerPed;
        }
    }
    else
    {
        PlayerFrame = 0;
        PlayerAnimTimer = 0;
    }

    // L/R to change weapon, same as OceanStorm.
    // (L+R together is the CPU meter. Jastro found out by switching to the shotgun mid-benchmark.)
    if( gamepad_button_l() == 1 && gamepad_button_r() <= 0 )
      SwitchWeaponPrevious();
    else if( gamepad_button_r() == 1 && gamepad_button_l() <= 0 )
      SwitchWeaponNext();

    UpdateWeapon();

    if( gamepad_button_x() == 1 )
    {
        if( IsPlayerAtShop() )
          BuyAmmo();
        else
          ReloadWeapon();
    }

    // A to shoot. OceanStorm's original comment, preserved like a museum piece:
    // "Create the bullet a bit in front of the soldier
    //  instead of at its center to avoid unwanted
    //  collisions with enemies"
    // Jastro's summary for the sprint review: "bullet go front"
    if( gamepad_button_a() == 1 && WeaponCurrentAmmo[ CurrentWeapon ] <= 0 && WeaponReserveAmmo[ CurrentWeapon ] <= 0 )
      PlaySfx( SoundCancel );

    if( gamepad_button_a() > 0 )
    {
        float firing_x = PlayerX + 10 * cos( PlayerAngle );
        float firing_y = PlayerY + 10 * sin( PlayerAngle );
        FireWeapon( firing_x, firing_y, PlayerAngle );
    }

    UpdateCamera( PlayerX, PlayerY );
}


// ---------------------------------------------------------
//   DAMAGE, WASTED AND BUSTED
// ---------------------------------------------------------
// soldier_take_damage() from OceanStorm.
// In OceanStorm the enemies shot you. Here a taxi does it. Progress.

void PlayerTakeDamageFrom( int damage, int death_reason )
{
    if( PlayerImmunityTimer > 0 || PlayerDownTimer > 0 )
      return;

    PlaySfx( SoundHurt );

    // if( soldier_armor > 0 ) soldier_armor--;
    // ^ Chip does not wear armor. He wears an orange bomber. Same thing, according to Jastro.
    PlayerHealth -= damage;

    if( PlayerHealth <= 0 )
    {
        PlayerHealth = 0;
        StartPlayerDown( death_reason );
    }
    else
      PlayerImmunityTimer = PLAYER_IMMUNITY_FRAMES / 3;
}

void PlayerTakeDamage( int damage )
{
    PlayerTakeDamageFrom( damage, DownWasted );
    PlayerImmunityTimer = PLAYER_IMMUNITY_FRAMES;
}

void CheckPlayerRunOver()
{
    if( PlayerImmunityTimer > 0 || PlayerStunTimer > 0 )
      return;

    for( int i = 0; i < MaxCars; i++ )
    {
        float speed = fabs( CarSpeed[ i ] );

        if( speed < PED_KNOCK_SPEED ) continue;
        if( fabs( PlayerX - CarX[ i ] ) > 60 || fabs( PlayerY - CarY[ i ] ) > 60 ) continue;

        float fx = CarFwdX[ i ] * CAR_CIRCLE_SPACING;
        float fy = CarFwdY[ i ] * CAR_CIRCLE_SPACING;

        for( int k = -1; k <= 1; k++ )
        {
            float dx = PlayerX - (CarX[ i ] + k * fx);
            float dy = PlayerY - (CarY[ i ] + k * fy);

            if( dx * dx + dy * dy >= (CAR_CIRCLE_RADIUS + 9) * (CAR_CIRCLE_RADIUS + 9) )
              continue;

            // v1 (Jastro): the player was pushed 8 times in one frame. It looked like teleporting.
            // Jastro: "it's a very fast push". v2 (Carra): you fly, you land, you lie there. Like in GTA2.
            PlayerKnockVX = CarVelX[ i ] * 1.3 + dx * 0.08;
            PlayerKnockVY = CarVelY[ i ] * 1.3 + dy * 0.08;
            PlayerAngle = atan2( PlayerKnockVY, PlayerKnockVX );
            PlayerStunTimer = PLAYER_FLAT_FRAMES;
            PlaySfx( SoundThud );

            if( CarIsAI[ i ] && CarMood[ i ] == MoodAttack )
              StartCarMood( i, MoodFlee );

            if( CarIsAI[ i ] && CarMood[ i ] == MoodPolice )
              StartPoliceBackoff( i );

            PlayerTakeDamage( 15 + speed * 6 );
            PlayerImmunityTimer = PLAYER_FLAT_FRAMES + 30;
            return;
        }
    }
}

void UpdatePlayerHazards()
{
    if( PlayerImmunityTimer > 0 )
      PlayerImmunityTimer--;

    if( IsPlayerInCar || PlayerDownTimer > 0 )
      return;

    CheckPlayerRunOver();
}

void StartPlayerDown( int reason )
{
    if( PlayerDownTimer > 0 )
      return;

    PlayerDownReason = reason;
    PlayerDownTimer = PLAYER_DOWN_FRAMES;

    if( reason == DownBusted ) PlaySfx( SoundBusted );
    else PlaySfx( SoundDeath );
    CarThrottle = 0;
    CarSteer = 0;
    CarHandbrake = false;

    if( reason == DownWasted && !IsPlayerInCar )
      SpawnCorpse( PlayerX, PlayerY, PlayerAngle + pi / 2, PedChip, color_white );
}

// kill_player() + reset_soldier() from OceanStorm.
// OceanStorm respawned you on the aircraft carrier. Here, at home. Your mum is not asking questions either.

void RespawnPlayer()
{
    if( IsPlayerInCar )
    {
        CarIsAI[ CurrentCar ] = false;
        CurrentCar = -1;
        IsPlayerInCar = false;
    }

    if( PlayerDownReason == DownBusted )
      PlayerMoney /= 2;

    // OceanStorm's kill_player() reset everything. Jastro's port reset everything except
    // the crimes still "in flight": the taxi you set on fire exploded after you respawned,
    // and you got two stars for dying. Carra: "the police have a very long memory".
    InitializePlayer();
    ResetWeapons();
    ResetWanted();
    ResetCops();
    ClearCarCrimeCredit();
    ClearPedestrianCrimeCredit();
    StartCrimeGrace();
    FloorZ = FLOOR_Z_ON_FOOT;
}

void UpdatePlayerDown()
{
    PlayerDownTimer--;

    if( !IsPlayerInCar )
      UpdateCamera( PlayerX, PlayerY );

    if( PlayerDownTimer <= 0 )
    {
        PlayerDownTimer = 0;
        RespawnPlayer();
    }
}


// ---------------------------------------------------------
//   RENDERING
// ---------------------------------------------------------

void RenderPlayer()
{
    if( IsPlayerInCar )
      return;

    if( PlayerDownTimer > 0 && PlayerDownReason == DownWasted )
      return;

    // "Parpadeo rápido" from render_soldier() used to be here.
    // Playtest feedback: "why is Chip a ghost now?". Removed. Jastro misses it.

    // run over or pulled out of the car. Jastro: "he's doing push-ups". Nobody believed him.
    if( PlayerStunTimer > 0 )
    {
        select_texture( TextureSprites );
        select_region( RegionFirstCorpse + PedChip );
        set_multiply_color( color_white );
        DrawSpriteInMap( PlayerX, PlayerY, PlayerAngle + pi / 2, 1.0 );
        return;
    }

    select_texture( TextureSprites );
    select_region( RegionFirstPed + PedChip * FramesPerPed + PlayerFrame );

    //   float draw_angle = PlayerAngle;            // v1: walks sideways like a crab
    //   float draw_angle = PlayerAngle - pi / 2;   // v2: moonwalks. Jastro wanted to keep it.
    float draw_angle = PlayerAngle + pi / 2;     // v3: Carra

    set_multiply_color( make_color_rgba( 0, 0, 0, 100 ) );
    DrawSpriteInMap( PlayerX + 2, PlayerY + 2, draw_angle, 1.0 );

    set_multiply_color( color_white );
    DrawSpriteInMap( PlayerX, PlayerY, draw_angle, 1.0 );
}


void PrintZoomedAt( int x, int y, int* text, float scale )
{
    select_texture( -1 );
    set_drawing_scale( scale, scale );

    while( *text )
    {
        select_region( *text );
        draw_region_zoomed_at( x, y );
        x += 10 * scale;
        text++;
    }
}

void RenderPlayerStatus( int y )
{
    DrawRect( 122, y, 110, 28, make_color_rgba( 12, 10, 22, 190 ) );
    DrawRect( 128, y + 9, 98, 10, make_color_rgb( 50, 20, 30 ) );

    int color = make_color_rgb( 80, 255, 120 );
    if( PlayerHealth <= 50 ) color = make_color_rgb( 255, 200, 50 );
    if( PlayerHealth <= 25 ) color = make_color_rgb( 255, 50, 60 );

    DrawRect( 128, y + 9, PlayerHealth * 98 / PLAYER_MAX_HEALTH, 10, color );

    if( IsPlayerInCar && CurrentCar >= 0 )
    {
        int car_health = CarHealth[ CurrentCar ];
        if( car_health < 0 ) car_health = 0;

        int car_color = make_color_rgb( 120, 200, 255 );
        if( car_health < CAR_FIRE_HEALTH && (get_frame_counter() / 6) % 2 ) car_color = make_color_rgb( 255, 120, 30 );

        DrawRect( 128, y + 21, car_health * 98 / CAR_MAX_HEALTH, 3, car_color );
    }

    if( PlayerDownTimer <= 0 )
      return;

    int* text = "WASTED";
    int text_color = make_color_rgb( 255, 40, 60 );

    if( PlayerDownReason == DownBusted )
    {
        text = "BUSTED";
        text_color = make_color_rgb( 60, 130, 255 );
    }

    int alpha = 255;
    if( PlayerDownTimer > PLAYER_DOWN_FRAMES - 30 )
      alpha = (PLAYER_DOWN_FRAMES - PlayerDownTimer) * 255 / 30;

    DrawRect( 0, 140, screen_width, 80, make_color_rgba( 0, 0, 0, alpha * 160 / 255 ) );
    set_multiply_color( make_color_rgba( 0, 0, 0, alpha ) );
    PrintZoomedAt( 204, 152, text, 4 );
    set_multiply_color( (text_color & 0x00FFFFFF) | (alpha << 24) );
    PrintZoomedAt( 200, 148, text, 4 );
    set_multiply_color( color_white );
}


// ---------------------------------------------------------
//   DEV TEAM NOTES
// ---------------------------------------------------------

/*
 * $ git log --oneline -- src/gameplay/Player.c
 *
 *   f00dbad  carra   real collisions (the else branch now does something)
 *   c0ffee1  carra   fix sprite angle (+pi/2)
 *   deadbee  carra   fix B entering and exiting car on the same frame
 *   1337abc  jastro  new character system
 *   1337abd  jastro  new character system (fix)
 *   1337abe  jastro  new character system (fix 2)
 *   1337abf  jastro  asdasdasd
 *   1337ac0  jastro  pls work
 *
 * TODO:
 *   - run with A              (assigned: Carra)
 *   - steal cars with people inside, like real GTA  (assigned: Carra)
 *   - pedestrians             (assigned: Carra)
 *   - take credit for all of the above  (assigned: Jastro) [DONE]
 *   - weapons                 (assigned: nobody, it was OceanStorm's) [DONE]
 *
 * KNOWN BUGS:
 *   - player aims without a weapon (FIXED: now he has three. It's still his personality)
 */
