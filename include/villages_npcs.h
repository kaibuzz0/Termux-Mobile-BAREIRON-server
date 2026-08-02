/*
 * BAREIRON VILLAGES, NPCS, WITCH BOSS, AND RETRO HEROES
 * Header v3: More content, NPC combat, quests, world events
 */

#ifndef H_VILLAGES_NPCS
#define H_VILLAGES_NPCS

#include <stdint.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════
// VILLAGE GENERATION
// ═══════════════════════════════════════════════════════════

#define MAX_VILLAGES 12
#define MAX_BUILDINGS_PER_VILLAGE 16
#define VILLAGE_RADIUS 40

// Village types (affects buildings and NPCs)
typedef enum {
    VTYPE_GENERIC = 0,
    VTYPE_FISHING,        // Docks, fishers, boats
    VTYPE_MINING,         // Mineshafts, miners, blacksmiths
    VTYPE_FARMING,        // Barns, windmills, farmers
    VTYPE_FORTIFIED,      // Walls, barracks, guards
    VTYPE_RELIGIOUS,      // Church, cemetery, healers
    VTYPE_TRADING,        // Market, tavern, innkeepers
    VTYPE_ACADEMIC        // Library, scholars, alchemists
} VillageType;

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
    BLD_RUINS,            // Destroyed building
    // NEW BUILDING TYPES
    BLD_FISHING_DOCK,     // Boats and nets
    BLD_MINE_SHAFT,       // Underground entrance
    BLD_BARN,             // Animals and storage
    BLD_WINDMILL,         // Grain processing
    BLD_CEMETERY,         // Graves and memorials
    BLD_MARKET,           // Open-air stalls
    BLD_TAVERN,           // Food, drink, rumors
    BLD_STABLE            // Horses and mounts
} BuildingType;

typedef struct Building_s {
    int active;
    BuildingType type;
    int x, y, z;
    int rotation;
    int condition;        // 0=ruined, 1=worn, 2=good
    int looted;
} Building;

typedef struct Village_s {
    int active;
    VillageType vtype;
    int x, y, z;
    int size;
    int population;
    int danger_level;     // 0=safe, 1=cautious, 2=hostile nearby, 3=under_attack
    int discovered;
    int under_attack;     // Active horde event?
    time_t attack_start;
    int defense_rating;   // Based on soldiers + towers
    char name[32];
} Village;

// ═══════════════════════════════════════════════════════════
// NPC SYSTEM (expanded classes)
// ═══════════════════════════════════════════════════════════

#define MAX_NPCS 96
#define NPC_NAME_LEN 32

// NPC classes (original + new)
typedef enum {
    NPC_NONE = 0,
    NPC_SURVIVOR,
    NPC_SOLDIER,          // Basic fighter
    NPC_FARMER,
    NPC_WEAPONSMITH,
    NPC_TRADER,
    NPC_HEALER,
    NPC_LIBRARIAN,
    NPC_CHILD,
    NPC_ELDER,
    NPC_HERO_MEGAMAN,
    NPC_HERO_LINK,
    NPC_HERO_SAMUS,
    NPC_HERO_MARIO,
    NPC_WITCH,
    // NEW CLASSES
    NPC_FISHER,           // Fishing village
    NPC_MINER,            // Mining village
    NPC_HUNTER,           // Hunts zombies, sells pelts
    NPC_GUARD_CAPTAIN,    // Leads soldiers, gives orders
    NPC_SCHOLAR,          // Research, lore, books
    NPC_BARD,             // Stories, morale boost
    NPC_ALCHEMIST,        // Potions, poisons, buffs
    NPC_INNKEEPER         // Taverns, rooms, rumors
} NPCClass;

typedef struct NPC_s {
    int active;
    int id;
    NPCClass npc_class;
    char name[NPC_NAME_LEN];
    float x, y, z;
    float health;
    float max_health;
    float damage;
    float attack_range;
    float attack_speed;
    int village_id;
    int following_player;
    int behavior;         // 0=stand, 1=patrol, 2=follow, 3=flee, 4=combat
    time_t last_action;
    time_t last_attack;
    int dialogue_state;
    int has_quest;
    int quest_completed;
    int rare_spawn;
    int defeated;
    int kills;            // Track zombie kills
} NPC;

// ═══════════════════════════════════════════════════════════
// QUEST SYSTEM
// ═══════════════════════════════════════════════════════════

#define MAX_QUESTS 32
#define QUEST_TITLE_LEN 64
#define QUEST_DESC_LEN 512

typedef enum {
    QUEST_NONE = 0,
    QUEST_KILL_ZOMBIES,   // Kill N zombies
    QUEST_FETCH_ITEM,     // Bring item to NPC
    QUEST_REACH_LOCATION, // Visit coordinates
    QUEST_DEFEND_VILLAGE, // Survive horde attack
    QUEST_ESCORT_NPC,     // Protect NPC to destination
    QUEST_RETRIEVE_RELIC, // Find sacred item
    QUEST_CRAFT_ITEM,     // Build/craft something
    QUEST_RESCUE_SURVIVOR // Save trapped person
} QuestType;

typedef enum {
    QUESTSTATUS_INACTIVE = 0,
    QUESTSTATUS_ACTIVE,
    QUESTSTATUS_COMPLETED,
    QUESTSTATUS_FAILED
} QuestStatus;

typedef struct Quest_s {
    int active;
    int id;
    QuestType qtype;
    QuestStatus status;
    char title[QUEST_TITLE_LEN];
    char description[QUEST_DESC_LEN];
    int giver_npc_id;     // Who gave the quest
    int target_count;     // How many needed
    int current_count;    // How many done
    int target_x, target_z; // For reach/escort quests
    int reward_materials;
    int reward_score;
    int reward_item_id;
    int reward_item_qty;
    time_t time_limit;    // 0 = no limit
    time_t start_time;
} Quest;

// ═══════════════════════════════════════════════════════════
// WORLD EVENTS
// ═══════════════════════════════════════════════════════════

#define MAX_ACTIVE_EVENTS 4

typedef enum {
    EVENT_NONE = 0,
    EVENT_HORDE_ATTACK,      // Zombies attack a village
    EVENT_WANDERING_MERCHANT,// Travelling trader appears
    EVENT_ZOMBIE_SWARM,      // Massive zombie spawn nearby
    EVENT_SUPPLY_DROP,       // Airdrop of materials
    EVENT_DARK_RITUAL,       // Witch summons extra zombies
    EVENT_SURVIVOR_RESCUE,   // Trapped survivor needs help
    EVENT_PLAGUE_OUTBREAK,   // Village gets infected
    EVENT_HEROIC_VISITOR     // Rare NPC visits village
} EventType;

typedef struct WorldEvent_s {
    int active;
    EventType etype;
    int target_village_id; // -1 = wilderness
    int target_player_id;  // -1 = all players
    float x, z;            // Event location
    int intensity;         // 1-5 scale
    time_t start_time;
    time_t duration;       // How long event lasts
    int completed;
    int failed;
    char announcement[256];
    char outcome[256];
} WorldEvent;

// ═══════════════════════════════════════════════════════════
// TRADING SYSTEM
// ═══════════════════════════════════════════════════════════

#define TRADE_SLOTS 4
#define ITEM_AMMO 1
#define ITEM_HEALTH_PACK 2
#define ITEM_MATERIALS 3
#define ITEM_WEAPON_UPGRADE 4
#define ITEM_MAP_FRAGMENT 5
#define ITEM_POTION_SPEED 6
#define ITEM_POTION_STRENGTH 7

typedef struct TradeOffer_s {
    int item_id;
    int quantity;
    int cost_materials;
    int cost_score;
    int in_stock;
} TradeOffer;

typedef struct TraderInventory_s {
    TradeOffer offers[TRADE_SLOTS];
    int num_offers;
    time_t last_restock;
} TraderInventory;

// ═══════════════════════════════════════════════════════════
// WITCH BOSS
// ═══════════════════════════════════════════════════════════

#define WITCH_PHASE_1 1
#define WITCH_PHASE_2 2
#define WITCH_PHASE_3 3

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

#define HERO_SPAWN_CHANCE 0.001f

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
const char* get_village_type_name(VillageType vtype);
void update_village_defense(int village_id);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — NPCS (expanded)
// ═══════════════════════════════════════════════════════════

void init_npc_system(void);
void spawn_npc(int village_id, NPCClass cls, float x, float z);
void spawn_village_population(int village_id);
void update_npcs(void);
void update_npc_pathfinding(void);
void npc_combat_tick(void);        // NEW: NPCs attack zombies
void npc_interact(int player_id, int npc_id);
void npc_follow_player(int npc_id, int player_id);
void npc_defend_player(int npc_id, int player_id);
void npc_dialogue(int npc_id, int player_id);
const char* get_npc_class_name(NPCClass cls);
const char* get_npc_greeting(NPCClass cls, int familiarity);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — QUESTS
// ═══════════════════════════════════════════════════════════

void init_quest_system(void);
int generate_quest(int giver_npc_id);
void activate_quest(int quest_id);
void update_quest_progress(int quest_id, int amount);
void complete_quest(int quest_id, int player_id);
void fail_quest(int quest_id);
void check_quest_completion(int quest_id);
void list_active_quests(int player_id);
void show_quest_details(int quest_id);
const char* get_quest_type_name(QuestType qtype);
void generate_quest_for_npc(int npc_id);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — WORLD EVENTS
// ═══════════════════════════════════════════════════════════

void init_world_events(void);
void update_world_events(void);
void attempt_random_event(void);
void trigger_event(EventType etype, int village_id, int player_id);
void announce_event(const WorldEvent* event);
void resolve_event(WorldEvent* event);
void event_horde_attack(WorldEvent* event);
void event_wandering_merchant(WorldEvent* event);
void event_zombie_swarm(WorldEvent* event);
void event_supply_drop(WorldEvent* event);
void event_dark_ritual(WorldEvent* event);
void event_survivor_rescue(WorldEvent* event);
void event_plague_outbreak(WorldEvent* event);
void event_heroic_visitor(WorldEvent* event);
void list_active_events(int player_id);
const char* get_event_name(EventType etype);

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
void tick_all_systems(void);  // Called every game tick

// Global access
extern Village villages[MAX_VILLAGES];
extern Building village_buildings[MAX_VILLAGES][MAX_BUILDINGS_PER_VILLAGE];
extern NPC npcs[MAX_NPCS];
extern WitchBoss witch_boss;
extern RetroHero heroes[4];
extern TraderInventory trader_inventories[MAX_NPCS];
extern Quest quests[MAX_QUESTS];
extern WorldEvent world_events[MAX_ACTIVE_EVENTS];

#endif
