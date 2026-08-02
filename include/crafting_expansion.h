/*
 * BAREIRON CRAFTING EXPANSION
 * Extended crafting recipes and buildable items
 */

#ifndef H_CRAFTING_EXPANSION
#define H_CRAFTING_EXPANSION

#include <stdint.h>

// ═══════════════════════════════════════════════════════════
// CRAFTABLE ITEMS
// ═══════════════════════════════════════════════════════════

#define MAX_RECIPES 32

// Crafted item types
typedef enum {
    CRAFT_NONE = 0,
    CRAFT_TRAP_SPIKE,         // Damages zombies that walk over it
    CRAFT_TRAP_FIRE,          // Burns zombies
    CRAFT_TRAP_ICE,           // Slows zombies
    CRAFT_TURRET_BASIC,       // Auto-targets zombies
    CRAFT_TURRET_HEAVY,       // More damage, slower
    CRAFT_TURRET_SNIPER,      // Long range, high damage
    CRAFT_MEDKIT_ADVANCED,    // Full heal + temp regen
    CRAFT_AMMO_EXPLOSIVE,     // Splash damage rounds
    CRAFT_AMMO_FREEZING,      // Slows targets
    CRAFT_GRENADE_MOLOTOV,    // Fire area
    CRAFT_GRENADE_FREEZE,     // Ice area
    CRAFT_GRENADE_HOLY,       // Massive damage to undead
    CRAFT_ARMOR_LIGHT,        // +25 max HP
    CRAFT_ARMOR_HEAVY,        // +50 max HP, -10% speed
    CRAFT_ARMOR_NANO,         // +75 max HP, regen
    CRAFT_TOOL_REPAIR,        // Instantly repair barricades
    CRAFT_TOOL_DECONSTRUCT,   // Break down for materials
    CRAFT_BEACON_SMOKE,       // Marks location for drops
    CRAFT_BEACON_HOLO,        // Holographic decoy
    CRAFT_FOOD_RATION,        // +10 HP
    CRAFT_FOOD_STEW,          // +25 HP, +speed
    CRAFT_FOOD_FEAST,         // +50 HP, team heal
    CRAFT_MAP_RADAR,          // Shows zombies nearby
    CRAFT_MAP_SURVEY,         // Reveals terrain
    CRAFT_CHARM_LUCK,         // Better drops
    CRAFT_CHARM_WARD,         // Reduced zombie aggro
    CRAFT_CHARM_VENGEANCE     // Damage boost when hurt
} CraftableItem;

// Recipe structure
typedef struct Recipe_s {
    CraftableItem result;
    char name[32];
    char description[128];
    int mats_cost;        // Materials needed
    int score_cost;       // Alternative: score
    int requires_smithy; // Needs blacksmith?
    int requires_alchemist; // Needs alchemist?
} Recipe;

// ═══════════════════════════════════════════════════════════
// CRAFTING TABLE
// ═══════════════════════════════════════════════════════════

void init_crafting_system(void);
void show_crafting_menu(int player_id);
int craft_item(int player_id, CraftableItem item);
void show_recipe_details(CraftableItem item);
const char* get_craftable_name(CraftableItem item);
const char* get_craftable_desc(CraftableItem item);

// External
extern Recipe recipes[MAX_RECIPES];

#endif
