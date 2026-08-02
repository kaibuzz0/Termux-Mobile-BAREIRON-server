/*
 * BAREIRON SAVE/LOAD SYSTEM
 * Binary persistence for player progress, world state, and discoveries
 */

#ifndef H_SAVEGAME
#define H_SAVEGAME

#include <stdint.h>
#include <time.h>
#include <stdio.h>

#define SAVE_VERSION 3
#define SAVE_FILENAME "bareiron_save.dat"

// Save file header
#pragma pack(push, 1)
typedef struct SaveHeader_s {
    uint32_t magic;        // 'BIRN' = 0x4249524E
    uint16_t version;
    uint16_t flags;
    time_t save_time;
    uint32_t checksum;
} SaveHeader;
#pragma pack(pop)

// ═══════════════════════════════════════════════════════════
// SAVE FUNCTIONS
// ═══════════════════════════════════════════════════════════

// Check if a save file exists
int save_exists(void);

// Write all game state to disk
void save_game_state(void);

// Read all game state from disk
int load_game_state(void);

// Delete save file (new game)
void delete_save(void);

// Save individual subsystems (v3 save format)
void sg_save_players(FILE* fp);
void sg_save_villages(FILE* fp);
void sg_save_quests(FILE* fp);
void sg_save_highscores(FILE* fp);
void sg_save_boss_states(FILE* fp);

// Load individual subsystems (v3 save format)
int sg_load_players(FILE* fp);
int sg_load_villages(FILE* fp);
int sg_load_quests(FILE* fp);
int sg_load_highscores(FILE* fp);
int sg_load_boss_states(FILE* fp);

// Display save info
void show_save_summary(void);

#endif
