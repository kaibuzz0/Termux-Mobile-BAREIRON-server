/*
 * BAREIRON SAVE/LOAD SYSTEM
 * Persistent player progress, world state, and discoveries
 * 
 * Save format v3:
 * - Header (magic, version, timestamp, checksum)
 * - Game state (wave, difficulty, mode)
 * - Players (stats, materials, position)
 * - Villages (discovered status, danger level)
 * - Quests (active, completed)
 * - High scores
 * - Boss states (Ancient, Witch)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "savegame.h"
#include "villages_npcs.h"
#include "globals.h"

#define SAVE_MAGIC 0x4249524E  // 'BIRN'

// External game state from zombie_game.c
extern struct Player_s { int id; char name[32]; float x,y,z; int weapon; int ammo; int max_ammo; int health; int max_health; int score; int kills; float speed_boost; float damage_boost; int invincible; time_t powerup_end; int materials; int barricades_built; } players[8];
extern int num_players;
extern struct HighScore_s { char name[32]; int score; int wave; int kills; time_t date; } highscores[10];
extern int num_highscores;
extern struct Barricade_s { int type; float x,y,z; float health; float max_health; int active; int owner; } barricades[128];
extern struct GameState_s { int mode; int wave; int zombies_alive; int max_zombies; float difficulty; time_t start_time; time_t wave_start; int powerup_spawn_timer; int horde_timer; int game_active; char map_name[64]; } game;
extern struct BossAncient_s { int active; int phase; float x,y,z; float health; float max_health; float base_damage; float base_speed; int minions_summoned; time_t last_summon; time_t phase_change_time; int sacred_artifact_dropped; } boss_ancient;

// ═══════════════════════════════════════════════════════════
// UTILITIES
// ═══════════════════════════════════════════════════════════

static uint32_t crc32(const uint8_t* data, size_t len) {
    // Simple CRC32 implementation
    static uint32_t table[256];
    static int init = 0;
    if (!init) {
        for (int i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int j = 0; j < 8; j++)
                c = (c >> 1) ^ (c & 1 ? 0xEDB88320 : 0);
            table[i] = c;
        }
        init = 1;
    }
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++)
        crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFF;
}

int save_exists(void) {
    FILE* fp = fopen(SAVE_FILENAME, "rb");
    if (fp) { fclose(fp); return 1; }
    return 0;
}

void delete_save(void) {
    remove(SAVE_FILENAME);
    printf("[SAVE] Save file deleted. Starting fresh.\n");
}

void show_save_summary(void) {
    if (!save_exists()) {
        printf("[SAVE] No save file found.\n");
        return;
    }
    FILE* fp = fopen(SAVE_FILENAME, "rb");
    if (!fp) return;
    
    SaveHeader hdr;
    if (fread(&hdr, sizeof(hdr), 1, fp) == 1) {
        printf("\n╔════════════════════════════════════════════════════════════╗\n");
        printf("║  SAVE FILE INFO                                           ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║  Version: %d                                              ║\n", hdr.version);
        printf("║  Saved: %s", ctime(&hdr.save_time));
        printf("║  Checksum: %08X                                        ║\n", hdr.checksum);
        printf("╚════════════════════════════════════════════════════════════╝\n\n");
    }
    fclose(fp);
}

// ═══════════════════════════════════════════════════════════
// SAVE FUNCTIONS
// ═══════════════════════════════════════════════════════════

void sg_save_players(FILE* fp) {
    // Save player count
    fwrite(&num_players, sizeof(int), 1, fp);
    // Save each player
    for (int i = 0; i < num_players; i++) {
        fwrite(&players[i], sizeof(players[i]), 1, fp);
    }
}

void sg_save_villages(FILE* fp) {
    for (int i = 0; i < MAX_VILLAGES; i++) {
        // Save village core data
        fwrite(&villages[i].discovered, sizeof(int), 1, fp);
        fwrite(&villages[i].danger_level, sizeof(int), 1, fp);
        fwrite(&villages[i].under_attack, sizeof(int), 1, fp);
    }
    // Save NPC quest states
    for (int i = 0; i < MAX_NPCS; i++) {
        fwrite(&npcs[i].dialogue_state, sizeof(int), 1, fp);
        fwrite(&npcs[i].quest_completed, sizeof(int), 1, fp);
        fwrite(&npcs[i].kills, sizeof(int), 1, fp);
    }
}

void sg_save_quests(FILE* fp) {
    int active_quests = 0;
    for (int i = 0; i < MAX_QUESTS; i++)
        if (quests[i].active && quests[i].status == QUESTSTATUS_ACTIVE)
            active_quests++;
    fwrite(&active_quests, sizeof(int), 1, fp);
    for (int i = 0; i < MAX_QUESTS; i++) {
        if (quests[i].active && quests[i].status == QUESTSTATUS_ACTIVE) {
            fwrite(&quests[i].id, sizeof(int), 1, fp);
            fwrite(&quests[i].qtype, sizeof(QuestType), 1, fp);
            fwrite(&quests[i].current_count, sizeof(int), 1, fp);
            fwrite(&quests[i].target_count, sizeof(int), 1, fp);
        }
    }
}

void sg_save_highscores(FILE* fp) {
    fwrite(&num_highscores, sizeof(int), 1, fp);
    for (int i = 0; i < num_highscores; i++) {
        fwrite(&highscores[i], sizeof(highscores[i]), 1, fp);
    }
}

void sg_save_boss_states(FILE* fp) {
    // Ancient boss
    extern int is_boss_active(void);  // from gameplay_expansion
    int ancient_active = is_boss_active();
    fwrite(&ancient_active, sizeof(int), 1, fp);
    if (ancient_active) {
        fwrite(&boss_ancient.health, sizeof(float), 1, fp);
        fwrite(&boss_ancient.phase, sizeof(int), 1, fp);
    }
    // Witch boss
    int witch_alive = is_witch_alive();
    fwrite(&witch_alive, sizeof(int), 1, fp);
    if (witch_alive) {
        fwrite(&witch_boss.health, sizeof(float), 1, fp);
        fwrite(&witch_boss.phase, sizeof(int), 1, fp);
    }
}

void save_game_state(void) {
    FILE* fp = fopen(SAVE_FILENAME, "wb");
    if (!fp) {
        printf("[SAVE] ERROR: Could not open save file!\n");
        return;
    }
    
    // Write header
    SaveHeader hdr;
    hdr.magic = SAVE_MAGIC;
    hdr.version = SAVE_VERSION;
    hdr.flags = 0;
    hdr.save_time = time(NULL);
    hdr.checksum = 0;  // Will compute after writing
    fwrite(&hdr, sizeof(hdr), 1, fp);
    
    // Write game state
    fwrite(&game, sizeof(game), 1, fp);
    
    // Write all subsystems
    sg_save_players(fp);
    sg_save_villages(fp);
    sg_save_quests(fp);
    sg_save_highscores(fp);
    sg_save_boss_states(fp);
    
    // Compute checksum
    long file_size = ftell(fp);
    uint8_t* data = malloc(file_size);
    if (data) {
        rewind(fp);
        fread(data, 1, file_size, fp);
        hdr.checksum = crc32(data, file_size);
        free(data);
        // Rewrite header with checksum
        rewind(fp);
        fwrite(&hdr, sizeof(hdr), 1, fp);
    }
    
    fclose(fp);
    printf("[SAVE] Game saved successfully! (%ld bytes)\n", file_size);
}

// ═══════════════════════════════════════════════════════════
// LOAD FUNCTIONS
// ═══════════════════════════════════════════════════════════

int sg_load_players(FILE* fp) {
    if (fread(&num_players, sizeof(int), 1, fp) != 1) return 0;
    if (num_players < 0 || num_players > 8) return 0;
    for (int i = 0; i < num_players; i++) {
        if (fread(&players[i], sizeof(players[i]), 1, fp) != 1) return 0;
    }
    return 1;
}

int sg_load_villages(FILE* fp) {
    for (int i = 0; i < MAX_VILLAGES; i++) {
        if (fread(&villages[i].discovered, sizeof(int), 1, fp) != 1) return 0;
        if (fread(&villages[i].danger_level, sizeof(int), 1, fp) != 1) return 0;
        if (fread(&villages[i].under_attack, sizeof(int), 1, fp) != 1) return 0;
    }
    for (int i = 0; i < MAX_NPCS; i++) {
        if (fread(&npcs[i].dialogue_state, sizeof(int), 1, fp) != 1) return 0;
        if (fread(&npcs[i].quest_completed, sizeof(int), 1, fp) != 1) return 0;
        if (fread(&npcs[i].kills, sizeof(int), 1, fp) != 1) return 0;
    }
    return 1;
}

int sg_load_quests(FILE* fp) {
    int active_quests;
    if (fread(&active_quests, sizeof(int), 1, fp) != 1) return 0;
    for (int i = 0; i < active_quests; i++) {
        int qid;
        QuestType qtype;
        int cur, target;
        if (fread(&qid, sizeof(int), 1, fp) != 1) return 0;
        if (fread(&qtype, sizeof(QuestType), 1, fp) != 1) return 0;
        if (fread(&cur, sizeof(int), 1, fp) != 1) return 0;
        if (fread(&target, sizeof(int), 1, fp) != 1) return 0;
        // Recreate quest
        if (qid >= 0 && qid < MAX_QUESTS) {
            quests[qid].active = 1;
            quests[qid].id = qid;
            quests[qid].qtype = qtype;
            quests[qid].status = QUESTSTATUS_ACTIVE;
            quests[qid].current_count = cur;
            quests[qid].target_count = target;
        }
    }
    return 1;
}

int sg_load_highscores(FILE* fp) {
    if (fread(&num_highscores, sizeof(int), 1, fp) != 1) return 0;
    if (num_highscores < 0 || num_highscores > 10) return 0;
    for (int i = 0; i < num_highscores; i++) {
        if (fread(&highscores[i], sizeof(highscores[i]), 1, fp) != 1) return 0;
    }
    return 1;
}

int sg_load_boss_states(FILE* fp) {
    int ancient_active;
    if (fread(&ancient_active, sizeof(int), 1, fp) != 1) return 0;
    if (ancient_active) {
        float health; int phase;
        if (fread(&health, sizeof(float), 1, fp) != 1) return 0;
        if (fread(&phase, sizeof(int), 1, fp) != 1) return 0;
        // Restore ancient boss
        extern void spawn_boss_ancient(void);
        spawn_boss_ancient();
        boss_ancient.health = health;
        boss_ancient.phase = phase;
    }
    int witch_alive;
    if (fread(&witch_alive, sizeof(int), 1, fp) != 1) return 0;
    if (witch_alive) {
        float health; int phase;
        if (fread(&health, sizeof(float), 1, fp) != 1) return 0;
        if (fread(&phase, sizeof(int), 1, fp) != 1) return 0;
        spawn_witch_boss(0, 80);
        witch_boss.health = health;
        witch_boss.phase = phase;
    }
    return 1;
}

int load_game_state(void) {
    FILE* fp = fopen(SAVE_FILENAME, "rb");
    if (!fp) {
        printf("[SAVE] No save file found. Starting new game.\n");
        return 0;
    }
    
    SaveHeader hdr;
    if (fread(&hdr, sizeof(hdr), 1, fp) != 1) {
        printf("[SAVE] ERROR: Could not read header!\n");
        fclose(fp);
        return 0;
    }
    
    if (hdr.magic != SAVE_MAGIC) {
        printf("[SAVE] ERROR: Invalid save file!\n");
        fclose(fp);
        return 0;
    }
    
    if (hdr.version != SAVE_VERSION) {
        printf("[SAVE] WARNING: Save version mismatch (%d vs %d). Some data may be lost.\n",
               hdr.version, SAVE_VERSION);
    }
    
    // Read game state
    if (fread(&game, sizeof(game), 1, fp) != 1) {
        printf("[SAVE] ERROR: Could not read game state!\n");
        fclose(fp);
        return 0;
    }
    
    // Load subsystems
    if (!sg_load_players(fp)) goto load_fail;
    if (!sg_load_villages(fp)) goto load_fail;
    if (!sg_load_quests(fp)) goto load_fail;
    if (!sg_load_highscores(fp)) goto load_fail;
    if (!sg_load_boss_states(fp)) goto load_fail;
    
    fclose(fp);
    printf("[SAVE] Game loaded successfully!\n");
    printf("  Players: %d\n", num_players);
    printf("  Wave: %d\n", game.wave);
    printf("  Discovered villages: ", game.wave);
    int disc = 0;
    for (int i = 0; i < MAX_VILLAGES; i++)
        if (villages[i].discovered) disc++;
    printf("%d\n", disc);
    return 1;
    
load_fail:
    printf("[SAVE] ERROR: Corrupted save data!\n");
    fclose(fp);
    return 0;
}
