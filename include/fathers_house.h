/*
 * THE FATHER'S HOUSE
 * Primordial Sanctuary — Hidden at the coordinates of Jerusalem
 * 
 * "In my Father's house are many mansions... I go to prepare a place for you."
 * 
 * This is a sacred space within the fallen world. A memory of Eden.
 * Players find it not by quest, but by pilgrimage — following whispers,
 * ancient texts, and the longing of their own hearts.
 */

#ifndef H_FATHERS_HOUSE
#define H_FATHERS_HOUSE

#include <stdint.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════
// COORDINATES — Jerusalem symbolic location
// ═══════════════════════════════════════════════════════════

#define FATHERS_HOUSE_X     352
#define FATHERS_HOUSE_Y     33
#define FATHERS_HOUSE_Z    -318

// Sanctuary radius: the circle of peace
#define SANCTUARY_RADIUS    150
#define SANCTUARY_RADIUS_SQ (SANCTUARY_RADIUS * SANCTUARY_RADIUS)

// The house itself sits at the heart
#define HOUSE_RADIUS        25
#define HOUSE_RADIUS_SQ     (HOUSE_RADIUS * HOUSE_RADIUS)

// ═══════════════════════════════════════════════════════════
// SANCTUARY STATE
// ═══════════════════════════════════════════════════════════

typedef struct {
    int active;           // Has the sanctuary been initialized?
    int discovered;       // Has any player found it?
    time_t first_visit;   // When was it first discovered?
    char first_discoverer[32];  // Who found it first?
    int total_visits;     // How many times has it been visited?
    int players_inside;   // Currently inside
} SanctuaryState;

extern SanctuaryState sanctuary;

// ═══════════════════════════════════════════════════════════
// CORE FUNCTIONS
// ═══════════════════════════════════════════════════════════

// Initialize the sanctuary system
void fathers_house_init();

// Check if coordinates are within sanctuary bounds
int is_within_sanctuary(int x, int z);
int is_within_house(int x, int z);
float sanctuary_distance_sq(int x, int z);

// Check if a player has entered/exited and trigger effects
void check_sanctuary_boundary(int player_id, float old_x, float old_z, float new_x, float new_z);

// Block hostile mob spawning within sanctuary
int block_hostile_spawn(float x, float z);

// Generate sanctuary terrain (override normal worldgen)
uint8_t sanctuary_terrain_at(int x, int y, int z);

// Generate the Father's House structure
void generate_fathers_house();

// ═══════════════════════════════════════════════════════════
// AMBIENT EFFECTS
// ═══════════════════════════════════════════════════════════

// Clear weather within sanctuary (override global weather)
int sanctuary_weather_override();

// Get light level boost (warm, golden light)
int sanctuary_light_level(int base_light);

// Get ambient sound hint (0=none, 1=water, 2=birds, 3=leaves, 4=peace)
int sanctuary_ambient_sound(float x, float z);

// ═══════════════════════════════════════════════════════════
// DISCOVERY SYSTEM
// ═══════════════════════════════════════════════════════════

// Generate a clue/hint for a specific location
void generate_world_clue(int clue_x, int clue_y, int clue_z, int clue_type);

// Check if player is near a hidden clue
void check_clue_discovery(int player_id, float x, float z);

// Get a cryptic hint pointing toward the sanctuary
const char* get_sanctuary_hint(int hint_number);

// ═══════════════════════════════════════════════════════════
// TERRAIN FEATURES — The Primordial Garden
// ═══════════════════════════════════════════════════════════

// Twelve stones (Joshua 4) — memorial stones
void place_twelve_stones();

// Olive trees — symbol of peace and anointing
void place_olive_grove(int center_x, int center_z, int count);

// The Vine — symbol of connection and fruitfulness
void place_the_vine();

// Almond trees — symbol of watchfulness (Aaron's rod)
void place_almond_trees();

// Living water — spring that never runs dry
void place_living_water();

// Shepherd's staff — hidden near the house
void place_shepherds_staff();

// The Lamp — eternal light
void place_eternal_lamp();

// ═══════════════════════════════════════════════════════════
// BIBLICAL SYMBOLISM
// ═══════════════════════════════════════════════════════════

#define SYMBOL_TWELVE_STONES   0  // Joshua 4 — remembrance
#define SYMBOL_OLIVE_TREE      1  // Romans 11 — grafting, peace
#define SYMBOL_THE_VINE        2  // John 15 — abiding
#define SYMBOL_ALMOND          3  // Numbers 17 — watchfulness, chosen
#define SYMBOL_LIVING_WATER    4  // John 4 — never thirst
#define SYMBOL_SHEPHERD_STAFF  5  // Psalm 23 — guidance, protection
#define SYMBOL_LAMP            6  // Psalm 119 — word is lamp
#define SYMBOL_DOOR            7  // John 10 — gate, way in

// Get the hidden meaning of a symbol
const char* get_symbol_lore(int symbol);

// ═══════════════════════════════════════════════════════════
// VISITOR EFFECTS
// ═══════════════════════════════════════════════════════════

// Apply healing/peace effects to players inside
void apply_sanctuary_blessing(int player_id);

// Grant wisdom (lore/hint) to visitor
void grant_wisdom(int player_id);

// Record a player's visit
void record_visit(int player_id);

// ═══════════════════════════════════════════════════════════
// WORLDGEN INTEGRATION
// ═══════════════════════════════════════════════════════════

// Check if sanctuary terrain should override normal worldgen
int sanctuary_override_needed(int x, int z);

#endif
