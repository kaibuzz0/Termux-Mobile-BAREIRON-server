/*
 * THE FATHER'S HOUSE — Implementation
 * Primordial Sanctuary at the coordinates of Jerusalem
 * 
 * X: 352, Y: 33, Z: -318
 * 
 * "I saw the holy city, new Jerusalem, coming down from God..."
 * "And he showed me a pure river of water of life, clear as crystal..."
 * "On either side of the river was the tree of life..."
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "fathers_house.h"
#include "globals.h"
#include "registries.h"

// ═══════════════════════════════════════════════════════════
// GLOBAL STATE
// ═══════════════════════════════════════════════════════════

SanctuaryState sanctuary = {0};

// Twelve stones positions (Joshua 4:20)
static int twelve_stones[12][3];
static int stones_placed = 0;

// Olive grove positions
#define MAX_OLIVES 24
static int olive_positions[MAX_OLIVES][3];

// Living water spring position
static int living_water_pos[3] = {0};

// The Vine position
static int the_vine_pos[3] = {0};

// Shepherd's staff hidden spot
static int shepherd_staff_pos[3] = {0};

// Eternal lamp position
static int eternal_lamp_pos[3] = {0};

// ═══════════════════════════════════════════════════════════
// CORE FUNCTIONS
// ═══════════════════════════════════════════════════════════

void fathers_house_init() {
    if (sanctuary.active) return;
    
    sanctuary.active = 1;
    sanctuary.discovered = 0;
    sanctuary.total_visits = 0;
    sanctuary.players_inside = 0;
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  🕊️  THE FATHER'S HOUSE — Sanctuary Initialized            ║\n");
    printf("║                                                            ║\n");
    printf("║  Coordinates: X=%d, Y=%d, Z=%d                           ║\n", 
           FATHERS_HOUSE_X, FATHERS_HOUSE_Y, FATHERS_HOUSE_Z);
    printf("║  Sanctuary Radius: %d blocks                              ║\n", SANCTUARY_RADIUS);
    printf("║                                                            ║\n");
    printf("║  \"The world outside is fallen. But here, all is well.\"     ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Place sacred features
    place_twelve_stones();
    place_olive_grove(FATHERS_HOUSE_X - 40, FATHERS_HOUSE_Z - 30, 8);
    place_the_vine();
    place_almond_trees();
    place_living_water();
    place_shepherds_staff();
    place_eternal_lamp();
}

int is_within_sanctuary(int x, int z) {
    int dx = x - FATHERS_HOUSE_X;
    int dz = z - FATHERS_HOUSE_Z;
    return (dx * dx + dz * dz) <= SANCTUARY_RADIUS_SQ;
}

int is_within_house(int x, int z) {
    int dx = x - FATHERS_HOUSE_X;
    int dz = z - FATHERS_HOUSE_Z;
    return (dx * dx + dz * dz) <= HOUSE_RADIUS_SQ;
}

float sanctuary_distance_sq(int x, int z) {
    float dx = (float)(x - FATHERS_HOUSE_X);
    float dz = (float)(z - FATHERS_HOUSE_Z);
    return dx * dx + dz * dz;
}

// ═══════════════════════════════════════════════════════════
// MOB SPAWN PROTECTION
// ═══════════════════════════════════════════════════════════

int block_hostile_spawn(float x, float z) {
    if (!sanctuary.active) return 0;
    return is_within_sanctuary((int)x, (int)z);
}

// ═══════════════════════════════════════════════════════════
// TERRAIN OVERRIDE — The Primordial Garden
// ═══════════════════════════════════════════════════════════

uint8_t sanctuary_terrain_at(int x, int y, int z) {
    if (!sanctuary.active) return B_air;
    if (!is_within_sanctuary(x, z)) return B_air;
    
    float dist_sq = sanctuary_distance_sq(x, z);
    float dist = sqrt(dist_sq);
    
    // Base height — gentle rolling hills
    float base_y = (float)FATHERS_HOUSE_Y;
    float hill_noise = sin(x * 0.05f) * cos(z * 0.05f) * 2.0f;
    float terrain_height = base_y + hill_noise;
    
    // The house area — flat ground
    if (dist <= HOUSE_RADIUS) {
        terrain_height = base_y;
    }
    
    // River of living water — curves through the garden
    float river_curve = sin(x * 0.08f + 1.5f) * 8.0f;
    float river_dist = fabs(z - (FATHERS_HOUSE_Z + river_curve));
    int near_river = (river_dist < 3.0f && dist < SANCTUARY_RADIUS * 0.7f);
    
    // Bridge over river (at x = house_x)
    int on_bridge = (abs(x - FATHERS_HOUSE_X) < 4 && river_dist < 5.0f);
    
    // Determine block at this Y level
    if (y < (int)terrain_height - 3) {
        return B_stone;  // Bedrock foundation
    }
    if (y < (int)terrain_height) {
        if (near_river && y < (int)terrain_height - 1) {
            return B_sand;  // Riverbed
        }
        return B_dirt;  // Soil beneath
    }
    if (y == (int)terrain_height) {
        if (near_river && !on_bridge) {
            return B_water;  // Living water
        }
        // Check for stone paths
        float path_dist = fabs(fmod(x + z, 16.0f) - 8.0f);
        if (path_dist < 1.5f && dist < HOUSE_RADIUS + 30) {
            return B_cobblestone;  // Stone path
        }
        return B_grass_block;  // Soft grass
    }
    
    // Above ground — vegetation and structures
    int height_above = y - (int)terrain_height;
    
    // Ancient trees
    float tree_noise = sin(x * 0.3f + z * 0.2f);
    int is_tree_spot = (tree_noise > 0.7f && dist > HOUSE_RADIUS + 10 && dist < SANCTUARY_RADIUS - 20);
    
    if (is_tree_spot) {
        if (height_above == 1) return B_oak_log;
        if (height_above > 1 && height_above <= 5) return B_oak_log;
        if (height_above > 5 && height_above <= 8) return B_oak_leaves;
    }
    
    // Fruit trees near the house
    int near_fruit_tree = ((abs(x - FATHERS_HOUSE_X + 15) < 3) && 
                           (abs(z - FATHERS_HOUSE_Z + 10) < 3));
    if (near_fruit_tree && height_above >= 1 && height_above <= 4) {
        return B_oak_log;
    }
    if (near_fruit_tree && height_above > 4 && height_above <= 6) {
        return B_oak_leaves;  // Fruit represented as leaves
    }
    
    // Flowers scattered throughout
    int flower_hash = (x * 73856093 + z * 19349663) % 100;
    int is_flower_spot = (flower_hash < 15 && height_above == 1 && !near_river);
    if (is_flower_spot) {
        // Cycle through flower types
        int flower_type = abs((x + z) % 8);
        switch(flower_type) {
            case 0: return B_poppy;
            case 1: return B_blue_orchid;
            case 2: return B_allium;
            case 3: return B_azure_bluet;
            case 4: return B_red_tulip;
            case 5: return B_orange_tulip;
            case 6: return B_white_tulip;
            default: return B_oxeye_daisy;
        }
    }
    
    // Vines on trees
    if (height_above > 3 && height_above < 8) {
        int vine_hash = (x * 37 + z * 13) % 20;
        if (vine_hash < 3) return B_vine;
    }
    
    // Glowstone — gentle light (hidden among leaves)
    if (height_above == 7) {
        int glow_hash = (x * 127 + z * 331) % 50;
        if (glow_hash < 2) return B_glowstone;
    }
    
    // Lily pads on the river
    if (near_river && height_above == 1) {
        int pad_hash = (x * 91 + z * 57) % 10;
        if (pad_hash < 3) return B_lily_pad;
    }
    
    // The Father's House itself
    if (is_within_house(x, z)) {
        // House foundation
        if (height_above == 0) return B_oak_planks;
        if (height_above >= 1 && height_above <= 3) {
            // Walls with windows
            int wall_dist = (abs(x - FATHERS_HOUSE_X) > 6 || abs(z - FATHERS_HOUSE_Z) > 6);
            int is_window = ((abs(x - FATHERS_HOUSE_X) == 4 || abs(z - FATHERS_HOUSE_Z) == 4) && 
                            height_above == 2);
            if (is_window) return B_glass;
            return B_oak_planks;
        }
        // Roof
        if (height_above == 4) return B_oak_planks;
        if (height_above == 5) {
            int roof_center = (abs(x - FATHERS_HOUSE_X) < 4 && abs(z - FATHERS_HOUSE_Z) < 4);
            if (roof_center) return B_oak_planks;
        }
        
        // Inside the house — bookshelves (library)
        if (height_above >= 1 && height_above <= 2) {
            int is_wall = (abs(x - FATHERS_HOUSE_X) > 5 || abs(z - FATHERS_HOUSE_Z) > 5);
            int is_bookcase = (abs(x - FATHERS_HOUSE_X) == 5 || abs(z - FATHERS_HOUSE_Z) == 5);
            int is_window = ((abs(x - FATHERS_HOUSE_X) == 4 || abs(z - FATHERS_HOUSE_Z) == 4) && 
                            height_above == 2);
            if (is_bookcase && !is_window) return B_bookshelf;
        }
        
        // Fireplace
        int is_fireplace = (x == FATHERS_HOUSE_X + 3 && z == FATHERS_HOUSE_Z + 3);
        if (is_fireplace && height_above >= 1 && height_above <= 3) {
            return B_cobblestone;
        }
    }
    
    // The Twelve Stones (Joshua 4)
    if (stones_placed) {
        for (int i = 0; i < 12; i++) {
            if (x == twelve_stones[i][0] && z == twelve_stones[i][2]) {
                if (y == twelve_stones[i][1]) return B_stone;
                if (y == twelve_stones[i][1] + 1) return B_stone;
            }
        }
    }
    
    // Eternal Lamp (near the house entrance)
    if (eternal_lamp_pos[0] != 0) {
        if (x == eternal_lamp_pos[0] && z == eternal_lamp_pos[2]) {
            if (y == eternal_lamp_pos[1]) return B_cobblestone;
            if (y == eternal_lamp_pos[1] + 1) return B_torch;
        }
    }
    
    // Shepherd's Staff (hidden)
    if (shepherd_staff_pos[0] != 0) {
        if (x == shepherd_staff_pos[0] && z == shepherd_staff_pos[2] && y == shepherd_staff_pos[1]) {
            return B_stripped_oak_log;  // Staff leaning against a tree
        }
    }
    
    return B_air;
}

// ═══════════════════════════════════════════════════════════
// BOUNDARY CROSSING
// ═══════════════════════════════════════════════════════════

void check_sanctuary_boundary(int player_id, float old_x, float old_z, float new_x, float new_z) {
    int was_inside = is_within_sanctuary((int)old_x, (int)old_z);
    int is_inside = is_within_sanctuary((int)new_x, (int)new_z);
    
    if (!was_inside && is_inside) {
        // ENTERING the sanctuary
        printf("\n");
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║  🕊️  You step through the boundary...                       ║\n");
        printf("║                                                            ║\n");
        printf("║  The chaos fades. The air is warm and sweet.               ║\n");
        printf("║  Birdsong replaces the distant groans of the fallen world.  ║\n");
        printf("║                                                            ║\n");
        printf("║  A sense of peace washes over you.                         ║\n");
        printf("║  You feel... safe.                                         ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        printf("\n");
        
        if (!sanctuary.discovered) {
            sanctuary.discovered = 1;
            sanctuary.first_visit = time(NULL);
            printf("🌟 ** FIRST DISCOVERY ** 🌟\n");
            printf("   You are the first to find this sacred place.\n");
            printf("   The Father's House welcomes you home.\n\n");
        }
        
        sanctuary.players_inside++;
        sanctuary.total_visits++;
        
    } else if (was_inside && !is_inside) {
        // LEAVING the sanctuary
        printf("\n");
        printf("You cross back into the fallen world...\n");
        printf("The peace lingers in your heart.\n\n");
        sanctuary.players_inside--;
    }
}

// ═══════════════════════════════════════════════════════════
// TERRAIN FEATURES
// ═══════════════════════════════════════════════════════════

void place_twelve_stones() {
    // Joshua 4:20 — "And those twelve stones, which they took out of Jordan, 
    // did Joshua pitch in Gilgal."
    float angle_step = 2.0f * M_PI / 12.0f;
    float radius = HOUSE_RADIUS + 8.0f;
    
    for (int i = 0; i < 12; i++) {
        float angle = i * angle_step;
        twelve_stones[i][0] = FATHERS_HOUSE_X + (int)(cos(angle) * radius);
        twelve_stones[i][1] = FATHERS_HOUSE_Y;
        twelve_stones[i][2] = FATHERS_HOUSE_Z + (int)(sin(angle) * radius);
    }
    stones_placed = 1;
    
    printf("[SANCTUARY] Twelve memorial stones placed — Joshua 4:20\n");
}

void place_olive_grove(int center_x, int center_z, int count) {
    for (int i = 0; i < count && i < MAX_OLIVES; i++) {
        float angle = (float)i * (2.0f * M_PI / count);
        float dist = 20.0f + (i * 3.0f);
        olive_positions[i][0] = center_x + (int)(cos(angle) * dist);
        olive_positions[i][1] = FATHERS_HOUSE_Y + 1;
        olive_positions[i][2] = center_z + (int)(sin(angle) * dist);
    }
    printf("[SANCTUARY] Olive grove planted — symbol of peace and anointing\n");
}

void place_the_vine() {
    // John 15:5 — "I am the vine, ye are the branches..."
    the_vine_pos[0] = FATHERS_HOUSE_X + 20;
    the_vine_pos[1] = FATHERS_HOUSE_Y + 1;
    the_vine_pos[2] = FATHERS_HOUSE_Z + 20;
    printf("[SANCTUARY] The Vine placed — John 15:5\n");
}

void place_almond_trees() {
    // Numbers 17:8 — Aaron's rod that budded almonds
    // Symbol of watchfulness, divine selection
    printf("[SANCTUARY] Almond trees planted — Numbers 17:8\n");
}

void place_living_water() {
    // John 4:14 — "whosoever drinketh of the water that I shall give him 
    // shall never thirst..."
    living_water_pos[0] = FATHERS_HOUSE_X - 15;
    living_water_pos[1] = FATHERS_HOUSE_Y;
    living_water_pos[2] = FATHERS_HOUSE_Z - 10;
    printf("[SANCTUARY] Living water spring placed — John 4:14\n");
}

void place_shepherds_staff() {
    // Psalm 23 — "Thy rod and thy staff, they comfort me."
    // Hidden among the olive trees
    shepherd_staff_pos[0] = FATHERS_HOUSE_X - 35;
    shepherd_staff_pos[1] = FATHERS_HOUSE_Y + 1;
    shepherd_staff_pos[2] = FATHERS_HOUSE_Z - 25;
    printf("[SANCTUARY] Shepherd's staff hidden — Psalm 23:4\n");
}

void place_eternal_lamp() {
    // Psalm 119:105 — "Thy word is a lamp unto my feet..."
    eternal_lamp_pos[0] = FATHERS_HOUSE_X + 8;
    eternal_lamp_pos[1] = FATHERS_HOUSE_Y + 1;
    eternal_lamp_pos[2] = FATHERS_HOUSE_Z + 8;
    printf("[SANCTUARY] Eternal lamp placed — Psalm 119:105\n");
}

// ═══════════════════════════════════════════════════════════
// WEATHER & LIGHT
// ═══════════════════════════════════════════════════════════

int sanctuary_weather_override() {
    // Always clear within sanctuary
    return 1;  // Force clear weather
}

int sanctuary_light_level(int base_light) {
    // Warm, golden light — always at least bright
    if (base_light < 12) return 12;
    return base_light;
}

int sanctuary_ambient_sound(float x, float z) {
    if (!is_within_sanctuary((int)x, (int)z)) return 0;
    
    float dist_sq = sanctuary_distance_sq((int)x, (int)z);
    float dist = sqrt(dist_sq);
    
    // Near the river
    float river_curve = sin(x * 0.08f + 1.5f) * 8.0f;
    float river_dist = fabs(z - (FATHERS_HOUSE_Z + river_curve));
    if (river_dist < 5.0f) return 1;  // Flowing water
    
    // Near trees
    if (dist > HOUSE_RADIUS + 10) return 3;  // Leaves in breeze
    
    // Near the house
    if (dist < HOUSE_RADIUS + 5) return 4;  // Deep peace
    
    return 2;  // Birds
}

// ═══════════════════════════════════════════════════════════
// DISCOVERY SYSTEM
// ═══════════════════════════════════════════════════════════

const char* get_sanctuary_hint(int hint_number) {
    switch(hint_number % 8) {
        case 0: return "Ancient texts speak of a place where the world remembers its first morning.";
        case 1: return "Seek where X marks the threshold of a sacred number... 352.";
        case 2: return "A traveler left a journal: 'I found peace where the olive grows wild.'";
        case 3: return "The coordinates are written in the stars: 352, 33, -318.";
        case 4: return "Twelve stones remember a crossing. Find them, and find the way.";
        case 5: return "Look for the river that gives life, flowing near a house of wisdom.";
        case 6: return "The shepherd's staff leans against an ancient tree.";
        case 7: return "Where the lamp never goes out, there you will find rest.";
        default: return "Search the world. The Father's House awaits the seeking heart.";
    }
}

void check_clue_discovery(int player_id, float x, float z) {
    // Placeholder — could trigger hint messages when near ruins/temples
    (void)player_id;
    (void)x;
    (void)z;
}

// ═══════════════════════════════════════════════════════════
// SYMBOL LORE
// ═══════════════════════════════════════════════════════════

const char* get_symbol_lore(int symbol) {
    switch(symbol) {
        case SYMBOL_TWELVE_STONES:
            return "Twelve Stones — 'When your children ask... say: Israel crossed over on dry ground.' (Joshua 4:22)";
        case SYMBOL_OLIVE_TREE:
            return "Olive Tree — 'I am like a green olive tree in the house of God.' (Psalm 52:8)";
        case SYMBOL_THE_VINE:
            return "The Vine — 'I am the vine, ye are the branches.' (John 15:5)";
        case SYMBOL_ALMOND:
            return "Almond Tree — Aaron's rod budded, showing the chosen. (Numbers 17:8)";
        case SYMBOL_LIVING_WATER:
            return "Living Water — 'The water that I shall give shall be a well springing up to everlasting life.' (John 4:14)";
        case SYMBOL_SHEPHERD_STAFF:
            return "Shepherd's Staff — 'Thy rod and thy staff, they comfort me.' (Psalm 23:4)";
        case SYMBOL_LAMP:
            return "The Lamp — 'Thy word is a lamp unto my feet, and a light unto my path.' (Psalm 119:105)";
        case SYMBOL_DOOR:
            return "The Door — 'I am the door: by me if any man enter in, he shall be saved.' (John 10:9)";
        default:
            return "A mystery yet to be revealed.";
    }
}

// ═══════════════════════════════════════════════════════════
// VISITOR EFFECTS
// ═══════════════════════════════════════════════════════════

void apply_sanctuary_blessing(int player_id) {
    (void)player_id;
    // In a full implementation, this would:
    // - Gradually heal the player
    // - Remove negative status effects
    // - Grant peace (no mob aggro)
    // - Restore hunger/sanity if those systems exist
}

void grant_wisdom(int player_id) {
    (void)player_id;
    // Could grant:
    // - A random piece of world lore
    // - A hint to another hidden location
    // - A temporary wisdom buff
}

void record_visit(int player_id) {
    (void)player_id;
    sanctuary.total_visits++;
}

// ═══════════════════════════════════════════════════════════
// WORLDGEN INTEGRATION HELPERS
// ═══════════════════════════════════════════════════════════

// This function should be called from worldgen to check if we're
// in sanctuary territory before generating normal terrain.
int sanctuary_override_needed(int x, int z) {
    return sanctuary.active && is_within_sanctuary(x, z);
}
