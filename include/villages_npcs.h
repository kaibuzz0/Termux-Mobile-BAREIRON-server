/*
 * BAREIRON VILLAGES, NPCS, AND WITCH BOSS
 * 
 * Small procedurally-generated settlements in the fallen world.
 * NPC survivors with classes, behaviors, trading, and combat.
 * The Witch — a roaming boss with dark magic.
 * Rare retro hero easter eggs that fight alongside players.
 * 
 * Memory-conscious design: Simple pathfinding via lerp.
 * NPCs stand guard, follow players, trade goods, and fight zombies.
 */

#ifndef H_VILLAGES_NPCS
#define H_VILLAGES_NPCS

#include <stdint.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════
// VILLAGE GENERATION
// ═══════════════════════════════════════════════════════════

#define MAX_VILLAGES 8
#define MAX_BUILDINGS_PER_VILLAGE 12
#define VILLAGE_RADIUS 40

// Building types
typedef enum {
    BLD_NONE = 0,
    BLD_HOUSE_SMALL,      // 1-room dwelling
    BLD_HOUSE_LARGE,      // 2-room dwelling
    BLD_WATCHTOWER,       // Defensive tower
    BLD_FARM,             // Wheat/potato field
    BLD_WELL,             // Water source
    BLD_CHURCH,           // Small chapel
    BLD_BARRACKS,         // Soldier quarters
    BLD_SMITHY,           // Weapon repair
    BLD_TRADE_POST,       // Trading hub
    BLD_LIBRARY,          // Lore books
    BLD_GARDEN,           // Flowers and crops
    BLD_RUINS             // Destroyed building
} BuildingType;

typedef struct Building_s {
    int active;
    BuildingType type;
    int x, y, z;          // Position
    int rotation;         // 0-3 (N/E/S/W)
    int condition;        // 0=ruined, 1=worn, 2=good
    int looted;           // Has player searched it?
} Building;

typedef struct Village_s {
    int active;
    int x, y, z;          // Center of village
    int size;             // Number of buildings
    int population;       // Number of NPCs
    int danger_level;     // 0=safe, 1=cautious, 2=hostile nearby
    int discovered;       // Has a player found it?
    char name[32];        // Procedural name
} Village;

// ═══════════════════════════════════════════════════════════
// NPC SYSTEM
// ═══════════════════════════════════════════════════════════

#define MAX_NPCS 64
#define NPC_NAME_LEN 32

// NPC classes
typedef enum {
    NPC_NONE = 0,
    NPC_SURVIVOR,         // Generic survivor, no special function
    NPC_SOLDIER,          // Fights zombies near village
    NPC_FARMER,           // Sells food/crops
    NPC_WEAPONSMITH,      // Sells ammo, repairs weapons
    NPC_TRADER,           // General goods exchange
    NPC_HEALER,           // Sells health, removes poison
    NPC_LIBRARIAN,        // Gives hints, sells maps
    NPC_CHILD,            // Gives quests, emotional value
    NPC_ELDER,            // Lore keeper, gives wisdom
    NPC_HERO_MEGAMAN,     // Easter egg: follows player, fights
    NPC_HERO_LINK,        // Easter egg: follows player, fights
    NPC_HERO_SAMUS,       // Easter egg: follows player, fights
    NPC_HERO_MARIO,       // Easter egg: follows player, fights
    NPC_WITCH             // Boss enemy
} NPCClass;

typedef struct NPC_s {
    int active;
    int id;
    NPCClass npc_class;
    char name[NPC_NAME_LEN];
    float x, y, z;
    float health;
    float max_health;
    int village_id;       // Which village they belong to (-1 = wanderer)
    int following_player; // -1 = none, 0+ = player_id
    int behavior;         // 0=stand, 1=patrol, 2=follow, 3=flee
    time_t last_action;
    int dialogue_state;   // 0=first meeting, 1=familiar, 2=friend
    int has_quest;        // 0=no, 1=yes
    int quest_completed;  // 0=no, 1=yes
    int rare_spawn;       // 0=common, 1=rare (heroes)
    int defeated;         // For bosses
} NPC;

// ═══════════════════════════════════════════════════════════
// TRADING SYSTEM
// ═══════════════════════════════════════════════════════════

#define TRADE_SLOTS 4
#define ITEM_AMMO 1
#define ITEM_HEALTH_PACK 2
#define ITEM_MATERIALS 3
#define ITEM_WEAPON_UPGRADE 4
#define ITEM_MAP_FRAGMENT 5

typedef struct TradeOffer_s {
    int item_id;          // What they sell
    int quantity;         // How many
    int cost_materials;   // Price in materials
    int cost_score;       // Alternative: price in score points
    int in_stock;         // Replenishes over time
} TradeOffer;

typedef struct TraderInventory_s {
    TradeOffer offers[TRADE_SLOTS];
    int num_offers;
    time_t last_restock;
} TraderInventory;

// ═══════════════════════════════════════════════════════════
// WITCH BOSS
// ═══════════════════════════════════════════════════════════

#define WITCH_PHASE_1 1   // Summons zombies, throws poison
#define WITCH_PHASE_2 2   // At 60% HP: summons bats, darkness
#define WITCH_PHASE_3 3   // At 30% HP: massive poison cloud, healing

typedef struct WitchBoss_s {
    int active;
    float x, y, z;
    float health;
    float max_health;
    int phase;
    time_t last_spell;
    time_t last_summon;
    int summons_total;
    int defeated;
    int achievement_unlocked;
} WitchBoss;

// ═══════════════════════════════════════════════════════════
// RETRO HERO EASTER EGGS
// ═══════════════════════════════════════════════════════════

#define HERO_MEGAMAN_NAME "Proto"
#define HERO_LINK_NAME "The Hero of Time"
#define HERO_SAMUS_NAME "Bounty Hunter"
#define HERO_MARIO_NAME "The Plumber"

#define HERO_SPAWN_CHANCE 0.001f  // 0.1% chance per spawn attempt

typedef struct RetroHero_s {
    int active;
    NPCClass hero_type;
    char name[32];
    float x, y, z;
    float health;
    int following_player;
    int kills;
    char catchphrase[128];
    float damage;
    float attack_range;
    time_t last_attack;
} RetroHero;

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — VILLAGES
// ═══════════════════════════════════════════════════════════

void init_village_system(void);
void generate_villages(void);
void generate_village_at(int index, int x, int z);
void place_building(int village_id, BuildingType type, int x, int y, int z, int rotation);
const char* generate_village_name(void);
void check_village_discovery(int player_id, float x, float z);
void list_villages(int player_id);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — NPCS
// ═══════════════════════════════════════════════════════════

void init_npc_system(void);
void spawn_npc(int village_id, NPCClass cls, float x, float z);
void spawn_village_population(int village_id);
void update_npcs(void);
void update_npc_pathfinding(void);
void npc_interact(int player_id, int npc_id);
void npc_follow_player(int npc_id, int player_id);
void npc_defend_player(int npc_id, int player_id);
void npc_dialogue(int npc_id, int player_id);
const char* get_npc_class_name(NPCClass cls);
const char* get_npc_greeting(NPCClass cls, int familiarity);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — TRADING
// ═══════════════════════════════════════════════════════════

void init_trading_system(void);
void generate_trader_inventory(int npc_id);
void restock_trader(int npc_id);
void show_trade_offers(int player_id, int npc_id);
int execute_trade(int player_id, int npc_id, int offer_slot);
const char* get_item_name(int item_id);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — WITCH BOSS
// ═══════════════════════════════════════════════════════════

void init_witch_boss(void);
void spawn_witch_boss(float x, float z);
void update_witch_boss(void);
void witch_cast_spell(void);
void witch_summon_zombies(void);
void witch_poison_cloud(void);
void witch_heal(void);
void check_witch_defeat(int player_id);
int is_witch_alive(void);
void grant_witch_achievement(int player_id);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — RETRO HEROES
// ═══════════════════════════════════════════════════════════

void init_hero_system(void);
void attempt_hero_spawn(void);
void spawn_hero(NPCClass hero_type, float x, float z);
void update_heroes(void);
void hero_attack_zombies(void);
void hero_interact(int player_id, int hero_id);
const char* get_hero_catchphrase(NPCClass hero_type);
const char* get_hero_description(NPCClass hero_type);

// ═══════════════════════════════════════════════════════════
// INIT ALL SYSTEMS
// ═══════════════════════════════════════════════════════════

void init_village_npc_systems(void);

// Global access for main game
extern Village villages[MAX_VILLAGES];
extern Building village_buildings[MAX_VILLAGES][MAX_BUILDINGS_PER_VILLAGE];
extern NPC npcs[MAX_NPCS];
extern WitchBoss witch_boss;
extern RetroHero heroes[4];
extern TraderInventory trader_inventories[MAX_NPCS];

#endif
