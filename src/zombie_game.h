/*
 * BAREIRON ZOMBIE SHOOTER
 * Header file
 */

#ifndef ZOMBIE_SHOOTER_H
#define ZOMBIE_SHOOTER_H

// Game initialization
void zombie_game_init_enhanced();
void zombie_player_join(int player_id);
void zombie_shooter_update();

// Game actions
void player_shoot(int player_id);
void spawn_wave_enhanced();
void zombie_game_tick_enhanced();

// Commands
void handle_command_enhanced(const char* cmd);
void print_status();

// Zombie types
#define ZOMBIE_NORMAL 100
#define ZOMBIE_FAST 101
#define ZOMBIE_TANK 102
#define ZOMBIE_BOSS 103

// Weapons
#define WEAPON_PISTOL 0
#define WEAPON_SHOTGUN 1
#define WEAPON_RIFLE 2
#define WEAPON_MINIGUN 3
#define WEAPON_GRENADE 4

#endif
