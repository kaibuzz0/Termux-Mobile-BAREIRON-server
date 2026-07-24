/*
 * BAREIRON ZOMBIE SHOOTER - ENHANCED EDITION
 * 
 * NEW FEATURES:
 * - Power-ups (Health, Ammo, Speed, Damage)
 * - Barricades/Building system
 * - Custom maps support
 * - High score saving
 * - 8 zombie types (4 new!)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "zombie_game.h"

// ==================== CONSTANTS ====================

// Game modes
#define MODE_SURVIVAL 0
#define MODE_HORDE 1
#define MODE_BOSS 2
#define MODE_DEFENSE 3

// Entity types
#define ENTITY_PLAYER 0
#define ENTITY_ZOMBIE_NORMAL 100
#define ENTITY_ZOMBIE_FAST 101
#define ENTITY_ZOMBIE_TANK 102
#define ENTITY_ZOMBIE_BOSS 103
#define ENTITY_ZOMBIE_CRAWLER 104
#define ENTITY_ZOMBIE_EXPLODER 105
#define ENTITY_ZOMBIE_SPITTER 106
#define ENTITY_ZOMBIE_NINJA 107

// Power-up types
#define POWERUP_HEALTH 0
#define POWERUP_AMMO 1
#define POWERUP_SPEED 2
#define POWERUP_DAMAGE 3
#define POWERUP_INVINCIBLE 4
#define POWERUP_NUKE 5

// Barricade types
#define BARRICADE_WOOD 0
#define BARRICADE_STONE 1
#define BARRICADE_METAL 2
#define BARRICADE_ELECTRIC 3

// Weapons
#define WEAPON_PISTOL 0
#define WEAPON_SHOTGUN 1
#define WEAPON_RIFLE 2
#define WEAPON_MINIGUN 3
#define WEAPON_GRENADE 4
#define WEAPON_RPG 5
#define WEAPON_SNIPER 6

#define MAX_ZOMBIES 512
#define MAX_PLAYERS 8
#define MAX_POWERUPS 64
#define MAX_BARRICADES 128
#define MAX_HIGHSCORES 10

// ==================== STRUCTURES ====================

typedef struct {
    int type;
    float x, y, z;
    int duration;  // Seconds remaining
    int active;
} PowerUp;

typedef struct {
    int type;
    float x, y, z;
    float health;
    float max_health;
    int active;
    int owner;  // Player who built it
} Barricade;

typedef struct {
    char name[32];
    int score;
    int wave;
    int kills;
    time_t date;
} HighScore;

typedef struct {
    int type;
    float x, y, z;
    float health;
    float max_health;
    float speed;
    float damage;
    int target_player;
    time_t spawn_time;
    int active;
    // Special abilities
    int can_explode;
    int can_spit;
    int is_invisible;
    float attack_range;
} Zombie;

typedef struct {
    int id;
    char name[32];
    float x, y, z;
    int weapon;
    int ammo;
    int max_ammo;
    int health;
    int max_health;
    int score;
    int kills;
    // Power-up effects
    float speed_boost;
    float damage_boost;
    int invincible;
    time_t powerup_end;
    // Building
    int materials;
    int barricades_built;
} Player;

typedef struct {
    int mode;
    int wave;
    int zombies_alive;
    int max_zombies;
    float difficulty;
    time_t start_time;
    time_t wave_start;
    int powerup_spawn_timer;
    int horde_timer;
    int game_active;
    char map_name[64];
} GameState;

// ==================== GLOBALS ====================

GameState game;
Zombie zombies[MAX_ZOMBIES];
Player players[MAX_PLAYERS];
PowerUp powerups[MAX_POWERUPS];
Barricade barricades[MAX_BARRICADES];
HighScore highscores[MAX_HIGHSCORES];
int num_players = 0;
int num_highscores = 0;

// Extended weapon stats
const char* weapon_names[] = {
    "Pistol", "Shotgun", "Rifle", "Minigun", "Grenade", "RPG", "Sniper"
};
const int weapon_damage[] = {25, 60, 35, 15, 200, 500, 150};
const int weapon_ammo[] = {12, 8, 30, 200, 3, 1, 5};
const float weapon_range[] = {50, 30, 100, 80, 40, 60, 200};
const float weapon_fire_rate[] = {0.5, 1.0, 0.1, 0.05, 2.0, 3.0, 1.5};

// NEW: Zombie type definitions with special abilities
const char* zombie_names[] = {
    "Walker", "Runner", "Tank", "Boss", "Crawler", "Boomer", "Spitter", "Ninja"
};
const float zombie_health[] = {100, 50, 300, 2000, 75, 120, 90, 60};
const float zombie_speed[] = {3.0, 6.0, 1.5, 2.0, 4.5, 2.5, 3.5, 7.0};
const float zombie_damage[] = {10, 8, 20, 50, 12, 15, 10, 25};

// ==================== POWER-UP SYSTEM ====================

void spawn_powerup(float x, float z) {
    int type = rand() % 6;  // Random power-up type
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) {
            powerups[i].type = type;
            powerups[i].x = x;
            powerups[i].y = 64;
            powerups[i].z = z;
            powerups[i].duration = 30;  // 30 seconds on ground
            powerups[i].active = 1;
            
            const char* names[] = {"Health", "Ammo", "Speed", "Damage", "Invincible", "NUKE"};
            printf("[POWERUP] %s spawned at (%.1f, %.1f)!\n", names[type], x, z);
            return;
        }
    }
}

void check_powerup_pickup(int player_id) {
    Player* p = &players[player_id];
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            float dx = powerups[i].x - p->x;
            float dz = powerups[i].z - p->z;
            float dist = sqrt(dx*dx + dz*dz);
            
            if (dist < 2.0) {  // Pickup range
                const char* names[] = {"Health", "Ammo", "Speed", "Damage", "Invincible", "NUKE"};
                printf("[POWERUP] Player %s picked up %s!\n", p->name, names[powerups[i].type]);
                
                switch(powerups[i].type) {
                    case POWERUP_HEALTH:
                        p->health = p->max_health;
                        printf("  -> Full health restored!\n");
                        break;
                    case POWERUP_AMMO:
                        p->ammo = p->max_ammo * 2;
                        printf("  -> Double ammo!\n");
                        break;
                    case POWERUP_SPEED:
                        p->speed_boost = 2.0;
                        p->powerup_end = time(NULL) + 15;
                        printf("  -> Speed boost for 15 seconds!\n");
                        break;
                    case POWERUP_DAMAGE:
                        p->damage_boost = 2.0;
                        p->powerup_end = time(NULL) + 15;
                        printf("  -> Double damage for 15 seconds!\n");
                        break;
                    case POWERUP_INVINCIBLE:
                        p->invincible = 1;
                        p->powerup_end = time(NULL) + 10;
                        printf("  -> INVINCIBLE for 10 seconds!\n");
                        break;
                    case POWERUP_NUKE:
                        // Kill all zombies!
                        for (int z = 0; z < MAX_ZOMBIES; z++) {
                            if (zombies[z].active && zombies[z].health > 0) {
                                zombies[z].health = 0;
                                game.zombies_alive--;
                                p->kills++;
                                p->score += 500;
                            }
                        }
                        printf("  -> ☢️ NUKE! All zombies destroyed!\n");
                        break;
                }
                
                powerups[i].active = 0;
            }
        }
    }
}

// ==================== BARRICADE SYSTEM ====================

void build_barricade(int player_id, int type) {
    Player* p = &players[player_id];
    int cost = (type + 1) * 10;  // Wood=10, Stone=20, Metal=30, Electric=40
    
    if (p->materials < cost) {
        printf("[BARRICADE] Not enough materials! Need %d, have %d\n", cost, p->materials);
        return;
    }
    
    for (int i = 0; i < MAX_BARRICADES; i++) {
        if (!barricades[i].active) {
            barricades[i].type = type;
            barricades[i].x = p->x + 3;  // Build in front of player
            barricades[i].y = 64;
            barricades[i].z = p->z + 3;
            barricades[i].owner = player_id;
            barricades[i].active = 1;
            
            const char* names[] = {"Wooden", "Stone", "Metal", "Electric"};
            float healths[] = {100, 300, 800, 500};
            
            barricades[i].max_health = healths[type];
            barricades[i].health = healths[type];
            
            p->materials -= cost;
            p->barricades_built++;
            
            printf("[BARRICADE] %s barricade built! HP: %.0f/%.0f\n",
                   names[type], barricades[i].health, barricades[i].max_health);
            return;
        }
    }
    
    printf("[BARRICADE] Max barricades reached!\n");
}

void repair_barricade(int player_id, int barricade_id) {
    if (!barricades[barricade_id].active) return;
    
    Player* p = &players[player_id];
    int repair_cost = 5;
    
    if (p->materials < repair_cost) {
        printf("[BARRICADE] Need %d materials to repair\n", repair_cost);
        return;
    }
    
    Barricade* b = &barricades[barricade_id];
    float repair = b->max_health * 0.25;  // Repair 25%
    
    if (b->health + repair > b->max_health) {
        repair = b->max_health - b->health;
    }
    
    b->health += repair;
    p->materials -= repair_cost;
    
    printf("[BARRICADE] Repaired! HP: %.0f/%.0f\n", b->health, b->max_health);
}

// ==================== HIGH SCORE SYSTEM ====================

void load_highscores() {
    FILE* f = fopen("highscores.dat", "rb");
    if (f) {
        num_highscores = fread(highscores, sizeof(HighScore), MAX_HIGHSCORES, f);
        fclose(f);
        printf("[SCORE] Loaded %d high scores\n", num_highscores);
    }
}

void save_highscores() {
    FILE* f = fopen("highscores.dat", "wb");
    if (f) {
        fwrite(highscores, sizeof(HighScore), num_highscores, f);
        fclose(f);
    }
}

void add_highscore(const char* name, int score, int wave, int kills) {
    // Find position
    int pos = num_highscores;
    for (int i = 0; i < num_highscores; i++) {
        if (score > highscores[i].score) {
            pos = i;
            break;
        }
    }
    
    if (pos >= MAX_HIGHSCORES) return;  // Not in top 10
    
    // Shift others down
    if (num_highscores < MAX_HIGHSCORES) {
        num_highscores++;
    }
    for (int i = num_highscores - 1; i > pos; i--) {
        highscores[i] = highscores[i-1];
    }
    
    // Insert new score
    strncpy(highscores[pos].name, name, 31);
    highscores[pos].name[31] = '\0';
    highscores[pos].score = score;
    highscores[pos].wave = wave;
    highscores[pos].kills = kills;
    highscores[pos].date = time(NULL);
    
    save_highscores();
    
    printf("\n🏆 NEW HIGH SCORE!\n");
    printf("   %s - %d points (Wave %d, %d kills)\n\n", name, score, wave, kills);
}

void print_highscores() {
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║              🏆 HIGH SCORES 🏆                 ║\n");
    printf("╠════════════════════════════════════════════════╣\n");
    printf("║ Rank  Name                Score   Wave  Kills║\n");
    printf("╠════════════════════════════════════════════════╣\n");
    
    for (int i = 0; i < num_highscores; i++) {
        printf("║ #%2d   %-20s  %5d   %4d  %5d║\n",
               i+1, highscores[i].name, highscores[i].score,
               highscores[i].wave, highscores[i].kills);
    }
    
    if (num_highscores == 0) {
        printf("║         No high scores yet!                    ║\n");
    }
    
    printf("╚════════════════════════════════════════════════╝\n\n");
}

// ==================== ENHANCED ZOMBIE SPAWNING ====================

void spawn_zombie_enhanced(int type, float x, float z) {
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (!zombies[i].active || zombies[i].health <= 0) {
            zombies[i].type = type;
            zombies[i].x = x;
            zombies[i].y = 64;
            zombies[i].z = z;
            zombies[i].health = zombie_health[type - 100];
            zombies[i].max_health = zombie_health[type - 100];
            zombies[i].speed = zombie_speed[type - 100];
            zombies[i].damage = zombie_damage[type - 100];
            zombies[i].active = 1;
            zombies[i].spawn_time = time(NULL);
            
            // Special abilities
            zombies[i].can_explode = (type == ENTITY_ZOMBIE_EXPLODER);
            zombies[i].can_spit = (type == ENTITY_ZOMBIE_SPITTER);
            zombies[i].is_invisible = (type == ENTITY_ZOMBIE_NINJA);
            zombies[i].attack_range = (type == ENTITY_ZOMBIE_SPITTER) ? 20 : 2.5;
            
            game.zombies_alive++;
            
            printf("[ZOMBIE] Spawned %s at (%.1f, %.1f)\n", zombie_names[type - 100], x, z);
            return;
        }
    }
}

void spawn_wave_enhanced() {
    int base_zombies = 10 + game.wave * 5;
    printf("\n🌊 WAVE %d INCOMING! %d zombies approaching...\n\n", game.wave, base_zombies);
    
    for (int i = 0; i < base_zombies; i++) {
        float angle = ((float)rand() / RAND_MAX) * 2.0 * M_PI;
        float distance = 20 + ((float)rand() / RAND_MAX) * 30;
        float x = cos(angle) * distance;
        float z = sin(angle) * distance;
        
        // Determine zombie type based on wave
        int type = ENTITY_ZOMBIE_NORMAL;
        int roll = rand() % 100;
        
        if (game.wave >= 10) {
            // All types available
            if (roll < 5) type = ENTITY_ZOMBIE_NINJA;      // 5%
            else if (roll < 15) type = ENTITY_ZOMBIE_SPITTER;  // 10%
            else if (roll < 25) type = ENTITY_ZOMBIE_EXPLODER; // 10%
            else if (roll < 35) type = ENTITY_ZOMBIE_CRAWLER;  // 10%
            else if (roll < 50) type = ENTITY_ZOMBIE_FAST;     // 15%
            else if (roll < 65) type = ENTITY_ZOMBIE_TANK;     // 15%
            else type = ENTITY_ZOMBIE_NORMAL;                    // 35%
        } else if (game.wave >= 5) {
            if (roll < 20) type = ENTITY_ZOMBIE_FAST;
            else if (roll < 35) type = ENTITY_ZOMBIE_CRAWLER;
            else if (roll < 50) type = ENTITY_ZOMBIE_TANK;
        } else {
            if (roll < 30) type = ENTITY_ZOMBIE_FAST;
            else if (roll < 50) type = ENTITY_ZOMBIE_TANK;
        }
        
        spawn_zombie_enhanced(type, x, z);
    }
    
    // Boss every 5 waves
    if (game.wave % 5 == 0) {
        spawn_zombie_enhanced(ENTITY_ZOMBIE_BOSS, 0, 50);
        printf("☠️ BOSS SPAWNED! ☠️\n");
    }
    
    game.wave_start = time(NULL);
}

// ==================== CUSTOM MAP SYSTEM ====================

void load_map(const char* map_name) {
    char filename[128];
    snprintf(filename, sizeof(filename), "maps/%s.map", map_name);
    
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("[MAP] Map '%s' not found, using default\n", map_name);
        return;
    }
    
    printf("[MAP] Loading '%s'...\n", map_name);
    
    // Clear existing barricades
    memset(barricades, 0, sizeof(barricades));
    
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char type[32];
        float x, z;
        
        if (sscanf(line, "BARRICADE %s %f %f", type, &x, &z) == 3) {
            int btype = BARRICADE_WOOD;
            if (strcmp(type, "STONE") == 0) btype = BARRICADE_STONE;
            else if (strcmp(type, "METAL") == 0) btype = BARRICADE_METAL;
            else if (strcmp(type, "ELECTRIC") == 0) btype = BARRICADE_ELECTRIC;
            
            for (int i = 0; i < MAX_BARRICADES; i++) {
                if (!barricades[i].active) {
                    barricades[i].type = btype;
                    barricades[i].x = x;
                    barricades[i].z = z;
                    barricades[i].y = 64;
                    barricades[i].active = 1;
                    float healths[] = {100, 300, 800, 500};
                    barricades[i].health = healths[btype];
                    barricades[i].max_health = healths[btype];
                    break;
                }
            }
        }
    }
    
    fclose(f);
    strncpy(game.map_name, map_name, 63);
    game.map_name[63] = '\0';
    printf("[MAP] Loaded '%s' successfully!\n", map_name);
}

// ==================== UPDATED GAME TICK ====================

void zombie_game_tick_enhanced() {
    time_t now = time(NULL);
    
    // Power-up spawning (every 30 seconds)
    if (now - game.start_time > game.powerup_spawn_timer) {
        game.powerup_spawn_timer += 30;
        if (rand() % 100 < 30) {  // 30% chance
            float angle = ((float)rand() / RAND_MAX) * 2.0 * M_PI;
            float dist = 10 + ((float)rand() / RAND_MAX) * 20;
            spawn_powerup(cos(angle) * dist, sin(angle) * dist);
        }
    }
    
    // Check power-up pickups
    for (int p = 0; p < num_players; p++) {
        if (players[p].health > 0) {
            check_powerup_pickup(p);
            
            // Check power-up expiration
            if (players[p].powerup_end > 0 && now > players[p].powerup_end) {
                players[p].speed_boost = 1.0;
                players[p].damage_boost = 1.0;
                players[p].invincible = 0;
                players[p].powerup_end = 0;
                printf("[POWERUP] Player %s effects wore off\n", players[p].name);
            }
        }
    }
    
    // Update power-up durations
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active && powerups[i].duration > 0) {
            powerups[i].duration--;
            if (powerups[i].duration <= 0) {
                powerups[i].active = 0;
            }
        }
    }
    
    // Check wave completion
    if (game.zombies_alive <= 0 && game.game_active) {
        printf("\n✅ WAVE %d COMPLETE!\n", game.wave);
        
        // Bonus materials for surviving
        for (int p = 0; p < num_players; p++) {
            if (players[p].health > 0) {
                int bonus = game.wave * 5;
                players[p].materials += bonus;
                players[p].score += game.wave * 100;
                printf("   %s earned %d materials!\n", players[p].name, bonus);
            }
        }
        
        game.wave++;
        game.difficulty *= 1.2;
        spawn_wave_enhanced();
    }
    
    // Zombie movement and attacks
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (!zombies[i].active || zombies[i].health <= 0) continue;
        
        // Find nearest player
        float min_dist = 999999;
        int nearest = -1;
        
        for (int p = 0; p < num_players; p++) {
            if (players[p].health <= 0) continue;
            
            float dx = players[p].x - zombies[i].x;
            float dz = players[p].z - zombies[i].z;
            float dist = sqrt(dx*dx + dz*dz);
            
            if (dist < min_dist) {
                min_dist = dist;
                nearest = p;
            }
        }
        
        if (nearest < 0) continue;
        
        // Check for barricades blocking
        int blocked = 0;
        for (int b = 0; b < MAX_BARRICADES; b++) {
            if (!barricades[b].active) continue;
            float dx = barricades[b].x - zombies[i].x;
            float dz = barricades[b].z - zombies[i].z;
            float dist = sqrt(dx*dx + dz*dz);
            
            if (dist < 2) {
                // Attack barricade
                barricades[b].health -= zombies[i].damage * 0.1;
                blocked = 1;
                
                if (barricades[b].health <= 0) {
                    printf("[BARRICADE] Destroyed by %s!\n", zombie_names[zombies[i].type - 100]);
                    barricades[b].active = 0;
                }
                break;
            }
        }
        
        if (!blocked && min_dist > zombies[i].attack_range) {
            // Move toward player
            float dx = players[nearest].x - zombies[i].x;
            float dz = players[nearest].z - zombies[i].z;
            float dist = sqrt(dx*dx + dz*dz);
            
            float speed = zombies[i].speed * 0.05;
            if (zombies[i].is_invisible && dist > 10) {
                speed *= 1.5;  // Ninjas move faster when far
            }
            
            zombies[i].x += (dx / dist) * speed;
            zombies[i].z += (dz / dist) * speed;
        }
        
        // Attack if in range
        if (!blocked && min_dist <= zombies[i].attack_range) {
            if (!players[nearest].invincible) {
                players[nearest].health -= zombies[i].damage;
                printf("💥 %s hit by %s! HP: %d\n",
                       players[nearest].name, zombie_names[zombies[i].type - 100],
                       players[nearest].health);
                
                // Special: Exploder zombies damage nearby
                if (zombies[i].can_explode) {
                    printf("💣 BOOM! Exploder detonated!\n");
                    for (int p = 0; p < num_players; p++) {
                        float edx = players[p].x - zombies[i].x;
                        float edz = players[p].z - zombies[i].z;
                        float edist = sqrt(edx*edx + edz*edz);
                        if (edist < 10) {
                            players[p].health -= 50;
                            printf("   %s caught in explosion!\n", players[p].name);
                        }
                    }
                    zombies[i].health = 0;  // Die after exploding
                    game.zombies_alive--;
                }
                
                // Special: Spitter ranged attack
                if (zombies[i].can_spit && min_dist > 5) {
                    printf("🟢 %s spit acid!\n", zombie_names[zombies[i].type - 100]);
                }
            }
            
            if (players[nearest].health <= 0) {
                printf("☠️ %s was killed!\n", players[nearest].name);
                
                // Check if all players dead
                int survivors = 0;
                for (int p = 0; p < num_players; p++) {
                    if (players[p].health > 0) survivors++;
                }
                
                if (survivors == 0 && num_players > 0) {
                    printf("\n💀 GAME OVER - All players eliminated!\n");
                    game.game_active = 0;
                    
                    // Save high scores
                    for (int p = 0; p < num_players; p++) {
                        if (players[p].score > 0) {
                            add_highscore(players[p].name, players[p].score,
                                         game.wave, players[p].kills);
                        }
                    }
                    print_highscores();
                }
            }
        }
    }
}

// ==================== ENHANCED COMMANDS ====================

void handle_command_enhanced(const char* cmd, int player_id) {
    Player* p = &players[player_id];
    
    if (strcmp(cmd, "/start") == 0) {
        game.game_active = 1;
        game.wave = 1;
        game.start_time = time(NULL);
        game.powerup_spawn_timer = 30;
        game.zombies_alive = 0;
        game.difficulty = 1.0;
        
        // Reset players
        for (int i = 0; i < num_players; i++) {
            players[i].health = players[i].max_health;
            players[i].ammo = players[i].max_ammo;
            players[i].materials = 50;  // Starting materials
        }
        
        memset(zombies, 0, sizeof(zombies));
        memset(powerups, 0, sizeof(powerups));
        
        load_highscores();
        spawn_wave_enhanced();
        printf("\n🎮 GAME STARTED! Survive as long as you can!\n\n");
    }
    else if (strcmp(cmd, "/build") == 0) {
        printf("Usage: /build [wood|stone|metal|electric]\n");
    }
    else if (strncmp(cmd, "/build ", 7) == 0) {
        char* type = (char*)cmd + 7;
        if (strcmp(type, "wood") == 0) build_barricade(player_id, BARRICADE_WOOD);
        else if (strcmp(type, "stone") == 0) build_barricade(player_id, BARRICADE_STONE);
        else if (strcmp(type, "metal") == 0) build_barricade(player_id, BARRICADE_METAL);
        else if (strcmp(type, "electric") == 0) build_barricade(player_id, BARRICADE_ELECTRIC);
        else printf("Unknown barricade type. Use: wood, stone, metal, electric\n");
    }
    else if (strcmp(cmd, "/map") == 0) {
        printf("Usage: /map [mapname] - Available: default, fortress, maze\n");
    }
    else if (strncmp(cmd, "/map ", 5) == 0) {
        load_map(cmd + 5);
    }
    else if (strcmp(cmd, "/scores") == 0) {
        print_highscores();
    }
    else if (strcmp(cmd, "/materials") == 0) {
        printf("Materials: %d (used to build/repair barricades)\n", p->materials);
    }
    else if (strcmp(cmd, "/help") == 0) {
        printf("\n╔════════════════════════════════════════════════╗\n");
        printf("║     🧟 ZOMBIE SHOOTER - COMMAND LIST 🧟        ║\n");
        printf("╠════════════════════════════════════════════════╣\n");
        printf("║ COMBAT                                         ║\n");
        printf("║   /start          - Start zombie survival      ║\n");
        printf("║   /shoot          - Fire weapon                ║\n");
        printf("║   /reload         - Reload ammo                ║\n");
        printf("║   /weapon [0-6]   - Switch weapon            ║\n");
        printf("║                                                ║\n");
        printf("║ BUILDING                                       ║\n");
        printf("║   /build [type]   - Build barricade          ║\n");
        printf("║   /materials      - Check materials            ║\n");
        printf("║                                                ║\n");
        printf("║ INFO                                           ║\n");
        printf("║   /status         - Show game status         ║\n");
        printf("║   /wave           - Current wave             ║\n");
        printf("║   /scores         - High score table         ║\n");
        printf("║   /map [name]     - Load custom map          ║\n");
        printf("╚════════════════════════════════════════════════╝\n\n");
    }
    else {
        // Try original commands
        printf("Unknown command. Type /help for commands\n");
    }
}

// ==================== INITIALIZATION ====================

void zombie_game_init_enhanced() {
    memset(&game, 0, sizeof(game));
    memset(zombies, 0, sizeof(zombies));
    memset(powerups, 0, sizeof(powerups));
    memset(barricades, 0, sizeof(barricades));
    
    game.mode = MODE_SURVIVAL;
    game.wave = 1;
    game.max_zombies = MAX_ZOMBIES;
    game.difficulty = 1.0;
    game.start_time = time(NULL);
    game.powerup_spawn_timer = 30;
    
    srand(time(NULL));
    
    printf("\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║                                                ║\n");
    printf("║     🧟 BAREIRON ZOMBIE SHOOTER 🧟              ║\n");
    printf("║              ENHANCED EDITION                  ║\n");
    printf("║                                                ║\n");
    printf("║  Features:                                     ║\n");
    printf("║  • 8 Zombie Types                              ║\n");
    printf("║  • 6 Power-ups                                 ║\n");
    printf("║  • Barricade Building                          ║\n");
    printf("║  • Custom Maps                                 ║\n");
    printf("║  • High Scores                                 ║\n");
    printf("║                                                ║\n");
    printf("║  Type /start to begin!                         ║\n");
    printf("║                                                ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    printf("\n");
}
