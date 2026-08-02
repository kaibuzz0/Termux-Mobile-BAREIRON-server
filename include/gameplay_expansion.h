/*
 * BAREIRON EXPANSION — Advanced Gameplay + Discovery
 * 
 * NEW ZOMBIES:
 * - Mutant (toxic aura, poisons nearby players)
 * - Ghost (phases through barricades, ethereal)
 * - Juggernaut (immune to knockback, extreme armor)
 * - Swarm (spawns 3 mini-zombies on death)
 * 
 * BOSS: The Ancient
 * - Phase 1: Ground assault (summons minions)
 * - Phase 2: Rage mode (speed + damage doubled)
 * - Phase 3: Desperation (explodes on death, drops sacred artifact)
 * 
 * WEAPON UPGRADES:
 * - Damage tiers: +25%, +50%, +100%
 * - Fire rate upgrades
 * - Special ammo (explosive rounds, incendiary, freezing)
 * 
 * PERKS (permanent unlocks):
 * - Juggernog (extra health)
 * - Speed Cola (faster reload)
 * - Double Tap (double fire rate)
 * - Quick Revive (self-revive once per game)
 */

#ifndef H_GAMEPLAY_EXPANSION
#define H_GAMEPLAY_EXPANSION

#include <stdint.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════
// NEW ZOMBIE TYPES
// ═══════════════════════════════════════════════════════════

#define ENTITY_ZOMBIE_MUTANT      108  // Toxic aura, poison damage
#define ENTITY_ZOMBIE_GHOST       109  // Phases through barricades
#define ENTITY_ZOMBIE_JUGGERNAUT  110  // Immune to knockback, extreme HP
#define ENTITY_ZOMBIE_SWARM       111  // Spawns mini-zombies on death
#define ENTITY_ZOMBIE_MINI        112  // Tiny, fast, spawned by Swarm

// ═══════════════════════════════════════════════════════════
// WEAPON UPGRADE SYSTEM
// ═══════════════════════════════════════════════════════════

#define UPGRADE_NONE       0
#define UPGRADE_DAMAGE_1   1   // +25% damage
#define UPGRADE_DAMAGE_2   2   // +50% damage
#define UPGRADE_DAMAGE_3   3   // +100% damage
#define UPGRADE_FIRE_RATE  4   // +30% fire rate
#define UPGRADE_MAGAZINE   5   // +50% ammo capacity
#define UPGRADE_SPECIAL    6   // Explosive/incendiary/freezing rounds

#define MAX_WEAPON_UPGRADES 3  // Max upgrades per weapon

typedef struct {
    int level;              // Current upgrade level (0-3)
    int upgrades[3];        // Which upgrades are applied
    int special_ammo_type;  // 0=normal, 1=explosive, 2=incendiary, 3=freezing
} WeaponUpgrade;

// ═══════════════════════════════════════════════════════════
// PERK SYSTEM (Permanent Unlocks)
// ═══════════════════════════════════════════════════════════

#define PERK_JUGGERNOG      0  // +50 max health
#define PERK_SPEED_COLA     1  // 2x reload speed
#define PERK_DOUBLE_TAP     2  // 2x fire rate
#define PERK_QUICK_REVIVE   3  // Self-revive once per game
#define PERK_STAMIN_UP      4  // +30% movement speed
#define PERK_PHD_FLOPPER    5  // Immune to explosive damage
#define PERK_DEADSHOT       6  // Headshots deal 3x damage
#define PERK_MULE_KICK      7  // Carry 3 weapons instead of 2

#define MAX_PERKS 8

typedef struct {
    int unlocked[MAX_PERKS];     // 0=locked, 1=unlocked
    int active[MAX_PERKS];       // Currently active in-game
    int perk_cost[MAX_PERKS];    // Cost to unlock (points/materials)
} PerkSystem;

// ═══════════════════════════════════════════════════════════
// BOSS: THE ANCIENT
// ═══════════════════════════════════════════════════════════

#define BOSS_PHASE_1    1  // Normal, summons minions
#define BOSS_PHASE_2    2  // At 50% HP: rage mode
#define BOSS_PHASE_3    3  // At 25% HP: desperation

typedef struct {
    int active;
    int phase;
    float x, y, z;
    float health;
    float max_health;
    float base_damage;
    float base_speed;
    int minions_summoned;
    time_t last_summon;
    time_t phase_change_time;
    int sacred_artifact_dropped;
} BossAncient;

// ═══════════════════════════════════════════════════════════
// DISCOVERY ITEMS — World Scattered Lore
// ═══════════════════════════════════════════════════════════

#define MAX_DISCOVERY_ITEMS 64
#define MAX_JOURNAL_ENTRIES 32

// Types of discovery items
typedef enum {
    DISC_NONE = 0,
    DISC_ANCIENT_BOOK,      // Readable lore
    DISC_TRAVELER_JOURNAL,  // Personal account, cryptic directions
    DISC_WORN_MAP,          // Partial coordinates
    DISC_RUINED_TEMPLE,     // Structure with inscription
    DISC_HIDDEN_INSCRIPTION, // Wall carving
    DISC_BROKEN_STATUE,     // Fragmented symbol
    DISC_SACRED_RELIC,      // Rare find
    DISC_STAR_CHART         // Celestial alignment hint
} DiscoveryType;

typedef struct {
    int active;
    DiscoveryType type;
    int x, y, z;            // World position
    int discovered;           // Has a player found this?
    char content[512];        // Readable text
    int clue_points_to_sanctuary;  // Does this hint toward Father's House?
    int points_to_boss;     // Does this hint toward The Ancient?
} DiscoveryItem;

typedef struct {
    DiscoveryItem items[MAX_DISCOVERY_ITEMS];
    int num_items;
    int total_discovered;
    int sanctuary_clues_found;
    int boss_clues_found;
} DiscoverySystem;

// Journal entry structure
typedef struct {
    int unlocked;
    char title[64];
    char text[1024];
    int relates_to;  // 0=sanctuary, 1=boss, 2=world_lore
} JournalEntry;

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — ZOMBIES
// ═══════════════════════════════════════════════════════════

// Spawn new zombie types
void spawn_mutant_zombie(float x, float z);
void spawn_ghost_zombie(float x, float z);
void spawn_juggernaut_zombie(float x, float z);
void spawn_swarm_zombie(float x, float z);
void spawn_mini_zombie(float x, float z, int parent_id);

// Special behaviors
void mutant_toxic_aura(int zombie_id);      // Damage nearby players
void ghost_phase_through(int zombie_id);      // Ignore barricades
void juggernaut_charge(int zombie_id);        // Ram attack
void swarm_death_spawn(int zombie_id);          // Spawn mini-zombies on death

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — WEAPON UPGRADES
// ═══════════════════════════════════════════════════════════

void init_weapon_upgrades();
int upgrade_weapon(int player_id, int weapon_id, int upgrade_type);
int get_weapon_damage_with_upgrades(int player_id, int weapon_id);
float get_weapon_fire_rate_with_upgrades(int player_id, int weapon_id);
const char* get_upgrade_name(int upgrade_type);
const char* get_special_ammo_name(int ammo_type);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — PERKS
// ═══════════════════════════════════════════════════════════

void init_perk_system();
int unlock_perk(int player_id, int perk_id);
int activate_perk(int player_id, int perk_id);
const char* get_perk_name(int perk_id);
const char* get_perk_description(int perk_id);
void apply_perk_effects(int player_id);

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — BOSS: THE ANCIENT
// ═══════════════════════════════════════════════════════════

void init_boss_ancient();
void spawn_boss_ancient();
void update_boss_ancient();
void boss_phase_transition(int new_phase);
void boss_summon_minions();
void boss_desperation_attack();
void drop_sacred_artifact();
int is_boss_active();

// ═══════════════════════════════════════════════════════════
// FUNCTIONS — DISCOVERY
// ═══════════════════════════════════════════════════════════

void init_discovery_system();
void generate_world_discoveries();
void place_ruined_temple(int x, int y, int z);
void place_ancient_book(int x, int y, int z, const char* text);
void place_traveler_journal(int x, int y, int z, const char* text);
void place_worn_map(int x, int y, int z, int partial_x, int partial_z);
void place_hidden_inscription(int x, int y, int z, const char* text);
void place_sacred_relic(int x, int y, int z);

// Discovery interaction
void check_discovery_proximity(int player_id, float x, float y, float z);
void read_discovery_item(int player_id, int item_id);
void unlock_journal_entry(int entry_id);

// Content generation
const char* generate_ancient_book_text(int book_id);
const char* generate_journal_text(int journal_id);
const char* generate_inscription_text(int inscription_id);
const char* generate_relic_description(int relic_id);

// ═══════════════════════════════════════════════════════════
// JOURNAL ENTRIES (Unlockable Lore)
// ═══════════════════════════════════════════════════════════

extern JournalEntry journal_entries[MAX_JOURNAL_ENTRIES];
void init_journal_entries();
void display_journal_entry(int entry_id);
void list_unlocked_journals(int player_id);

// ═══════════════════════════════════════════════════════════
// WORLD GENERATION INTEGRATION
// ═══════════════════════════════════════════════════════════

// Called during worldgen to place discovery items
void integrate_discoveries_into_worldgen();

// Check if a position is suitable for a discovery item
int is_valid_discovery_location(int x, int z);

// ═══════════════════════════════════════════════════════════
// GAME COMMANDS
// ═══════════════════════════════════════════════════════════

void handle_upgrade_command(int player_id, const char* cmd);
void handle_perk_command(int player_id, const char* cmd);
void handle_boss_command(int player_id, const char* cmd);
void handle_journal_command(int player_id, const char* cmd);
void handle_discoveries_command(int player_id);

#endif
