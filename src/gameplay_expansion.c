/*
 * BAREIRON EXPANSION — Advanced Gameplay + Discovery
 * Implementation: New zombies, weapon upgrades, perks, boss, discovery items
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "gameplay_expansion.h"
#include "zombie_game.h"
#include "fathers_house.h"

// Re-declare zombie entity types from zombie_game.c
#define ENTITY_ZOMBIE_NORMAL    100
#define ENTITY_ZOMBIE_FAST      101
#define ENTITY_ZOMBIE_TANK      102
#define ENTITY_ZOMBIE_BOSS      103
#define ENTITY_ZOMBIE_CRAWLER   104
#define ENTITY_ZOMBIE_EXPLODER  105
#define ENTITY_ZOMBIE_SPITTER   106
#define ENTITY_ZOMBIE_NINJA     107
#define ENTITY_ZOMBIE_MUTANT    108
#define ENTITY_ZOMBIE_GHOST     109
#define ENTITY_ZOMBIE_JUGGERNAUT 110
#define ENTITY_ZOMBIE_SWARM     111
#define ENTITY_ZOMBIE_MINI      112

// ═══════════════════════════════════════════════════════════
// GLOBALS
// ═══════════════════════════════════════════════════════════

WeaponUpgrade weapon_upgrades[8][7];  // 8 players, 7 weapons
PerkSystem perk_system;
BossAncient boss_ancient;
DiscoverySystem discovery_system;
JournalEntry journal_entries[MAX_JOURNAL_ENTRIES];

// ═══════════════════════════════════════════════════════════
// WEAPON UPGRADES
// ═══════════════════════════════════════════════════════════

void init_weapon_upgrades() {
    memset(weapon_upgrades, 0, sizeof(weapon_upgrades));
    printf("[UPGRADES] Weapon upgrade system initialized\n");
}

const char* get_upgrade_name(int upgrade_type) {
    switch(upgrade_type) {
        case UPGRADE_DAMAGE_1: return "Damage +25%";
        case UPGRADE_DAMAGE_2: return "Damage +50%";
        case UPGRADE_DAMAGE_3: return "Damage +100%";
        case UPGRADE_FIRE_RATE: return "Rapid Fire";
        case UPGRADE_MAGAZINE: return "Extended Magazine";
        case UPGRADE_SPECIAL: return "Special Ammo";
        default: return "Unknown";
    }
}

const char* get_special_ammo_name(int ammo_type) {
    switch(ammo_type) {
        case 1: return "Explosive Rounds";
        case 2: return "Incendiary Rounds";
        case 3: return "Freezing Rounds";
        default: return "Standard";
    }
}

int upgrade_weapon(int player_id, int weapon_id, int upgrade_type) {
    if (player_id < 0 || player_id >= 8) return 0;
    if (weapon_id < 0 || weapon_id >= 7) return 0;
    
    WeaponUpgrade* wu = &weapon_upgrades[player_id][weapon_id];
    if (wu->level >= MAX_WEAPON_UPGRADES) {
        printf("[UPGRADE] Weapon is already at max level!\n");
        return 0;
    }
    
    wu->upgrades[wu->level] = upgrade_type;
    wu->level++;
    
    printf("[UPGRADE] %s applied to weapon %d! (Level %d/%d)\n",
           get_upgrade_name(upgrade_type), weapon_id, wu->level, MAX_WEAPON_UPGRADES);
    return 1;
}

int get_weapon_damage_with_upgrades(int player_id, int weapon_id) {
    extern const int weapon_damage[];
    int base = weapon_damage[weapon_id];
    WeaponUpgrade* wu = &weapon_upgrades[player_id][weapon_id];
    
    for (int i = 0; i < wu->level; i++) {
        switch(wu->upgrades[i]) {
            case UPGRADE_DAMAGE_1: base = (int)(base * 1.25f); break;
            case UPGRADE_DAMAGE_2: base = (int)(base * 1.50f); break;
            case UPGRADE_DAMAGE_3: base *= 2; break;
        }
    }
    return base;
}

// ═══════════════════════════════════════════════════════════
// PERK SYSTEM
// ═══════════════════════════════════════════════════════════

void init_perk_system() {
    memset(&perk_system, 0, sizeof(perk_system));
    perk_system.perk_cost[PERK_JUGGERNOG] = 2500;
    perk_system.perk_cost[PERK_SPEED_COLA] = 3000;
    perk_system.perk_cost[PERK_DOUBLE_TAP] = 2000;
    perk_system.perk_cost[PERK_QUICK_REVIVE] = 1500;
    perk_system.perk_cost[PERK_STAMIN_UP] = 2000;
    perk_system.perk_cost[PERK_PHD_FLOPPER] = 2000;
    perk_system.perk_cost[PERK_DEADSHOT] = 2500;
    perk_system.perk_cost[PERK_MULE_KICK] = 4000;
    printf("[PERKS] Perk system initialized\n");
}

const char* get_perk_name(int perk_id) {
    switch(perk_id) {
        case PERK_JUGGERNOG: return "Juggernog";
        case PERK_SPEED_COLA: return "Speed Cola";
        case PERK_DOUBLE_TAP: return "Double Tap";
        case PERK_QUICK_REVIVE: return "Quick Revive";
        case PERK_STAMIN_UP: return "Stamin-Up";
        case PERK_PHD_FLOPPER: return "PhD Flopper";
        case PERK_DEADSHOT: return "Deadshot Daiquiri";
        case PERK_MULE_KICK: return "Mule Kick";
        default: return "Unknown";
    }
}

const char* get_perk_description(int perk_id) {
    switch(perk_id) {
        case PERK_JUGGERNOG: return "+50 Max Health";
        case PERK_SPEED_COLA: return "2x Reload Speed";
        case PERK_DOUBLE_TAP: return "2x Fire Rate";
        case PERK_QUICK_REVIVE: return "Self-revive once per game";
        case PERK_STAMIN_UP: return "+30% Movement Speed";
        case PERK_PHD_FLOPPER: return "Immune to explosive damage";
        case PERK_DEADSHOT: return "Headshots deal 3x damage";
        case PERK_MULE_KICK: return "Carry 3 weapons";
        default: return "???";
    }
}

int unlock_perk(int player_id, int perk_id) {
    if (perk_id < 0 || perk_id >= MAX_PERKS) return 0;
    if (perk_system.unlocked[perk_id]) {
        printf("[PERKS] %s is already unlocked!\n", get_perk_name(perk_id));
        return 0;
    }
    perk_system.unlocked[perk_id] = 1;
    printf("[PERKS] %s unlocked! (%s)\n", get_perk_name(perk_id), get_perk_description(perk_id));
    return 1;
}

// ═══════════════════════════════════════════════════════════
// BOSS: THE ANCIENT
// ═══════════════════════════════════════════════════════════

void init_boss_ancient() {
    memset(&boss_ancient, 0, sizeof(boss_ancient));
    printf("[BOSS] The Ancient initialized\n");
}

void spawn_boss_ancient() {
    if (boss_ancient.active) {
        printf("[BOSS] The Ancient is already active!\n");
        return;
    }
    
    boss_ancient.active = 1;
    boss_ancient.phase = BOSS_PHASE_1;
    boss_ancient.x = 0;
    boss_ancient.y = 64;
    boss_ancient.z = 50;
    boss_ancient.max_health = 10000;
    boss_ancient.health = 10000;
    boss_ancient.base_damage = 75;
    boss_ancient.base_speed = 2.0f;
    boss_ancient.minions_summoned = 0;
    boss_ancient.last_summon = time(NULL);
    boss_ancient.sacred_artifact_dropped = 0;
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                                                            ║\n");
    printf("║     ☠️  THE ANCIENT HAS AWAKENED  ☠️                      ║\n");
    printf("║                                                            ║\n");
    printf("║  Phase 1: The Summoning                                  ║\n");
    printf("║  Health: 10,000                                          ║\n");
    printf("║                                                            ║\n");
    printf("║  \"I have walked these lands since the first dawn...\"     ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void update_boss_ancient() {
    if (!boss_ancient.active) return;
    
    float hp_percent = boss_ancient.health / boss_ancient.max_health;
    
    // Phase transitions
    if (boss_ancient.phase == BOSS_PHASE_1 && hp_percent <= 0.50f) {
        boss_phase_transition(BOSS_PHASE_2);
    } else if (boss_ancient.phase == BOSS_PHASE_2 && hp_percent <= 0.25f) {
        boss_phase_transition(BOSS_PHASE_3);
    }
    
    // Summon minions every 15 seconds
    if (time(NULL) - boss_ancient.last_summon >= 15) {
        boss_summon_minions();
        boss_ancient.last_summon = time(NULL);
    }
    
    // Check death
    if (boss_ancient.health <= 0) {
        boss_ancient.active = 0;
        printf("\n");
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║                                                            ║\n");
        printf("║     🏆 THE ANCIENT HAS FALLEN! 🏆                        ║\n");
        printf("║                                                            ║\n");
        printf("║  The old terror crumbles to dust.                          ║\n");
        printf("║  A sacred artifact glows where the beast fell.             ║\n");
        printf("║                                                            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        printf("\n");
        drop_sacred_artifact();
    }
}

void boss_phase_transition(int new_phase) {
    boss_ancient.phase = new_phase;
    boss_ancient.phase_change_time = time(NULL);
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    
    switch(new_phase) {
        case BOSS_PHASE_2:
            printf("║     ⚡ PHASE 2: THE ANCIENT RAGES! ⚡                     ║\n");
            printf("║                                                            ║\n");
            printf("║  \"You think steel can harm what time itself fears?\"       ║\n");
            printf("║                                                            ║\n");
            printf("║  Speed DOUBLED. Damage DOUBLED.                            ║\n");
            boss_ancient.base_speed *= 2.0f;
            boss_ancient.base_damage *= 2.0f;
            break;
            
        case BOSS_PHASE_3:
            printf("║     🔥 PHASE 3: DESPERATION! 🔥                         ║\n");
            printf("║                                                            ║\n");
            printf("║  \"If I fall... I take you with me!\"                      ║\n");
            printf("║                                                            ║\n");
            printf("║  WARNING: The Ancient will EXPLODE on death!              ║\n");
            break;
    }
    
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void boss_summon_minions() {
    printf("[BOSS] The Ancient summons minions!\n");
    extern void spawn_zombie_enhanced(int type, float x, float z);
    for (int i = 0; i < 3; i++) {
        float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float dist = 5.0f + ((float)rand() / RAND_MAX) * 10.0f;
        float x = boss_ancient.x + cos(angle) * dist;
        float z = boss_ancient.z + sin(angle) * dist;
        spawn_zombie_enhanced(ENTITY_ZOMBIE_NORMAL, x, z);
    }
    boss_ancient.minions_summoned += 3;
}

void drop_sacred_artifact() {
    if (boss_ancient.sacred_artifact_dropped) return;
    boss_ancient.sacred_artifact_dropped = 1;
    
    printf("[BOSS] 💎 Sacred Artifact dropped at (%.1f, %.1f)!\n", boss_ancient.x, boss_ancient.z);
    printf("       Pick it up to unlock ancient knowledge...\n\n");
    
    // Could unlock a special perk, open a portal, etc.
}

int is_boss_active() {
    return boss_ancient.active;
}

// ═══════════════════════════════════════════════════════════
// DISCOVERY SYSTEM
// ═══════════════════════════════════════════════════════════

void init_discovery_system() {
    memset(&discovery_system, 0, sizeof(discovery_system));
    printf("[DISCOVERY] Discovery system initialized\n");
}

void generate_world_discoveries() {
    // Ruined Temples (3)
    place_ruined_temple(120, 64, 80);
    place_ruined_temple(-200, 64, 150);
    place_ruined_temple(400, 64, -200);
    
    // Ancient Books (5)
    place_ancient_book(50, 65, 50, 
        "The First Morning\n\n"
        "Before the fall, the world knew only light.\n"
        "A garden existed where all was well.\n"
        "Twelve stones marked the entrance.\n"
        "A river flowed from a house of wisdom.\n\n"
        "Seek where X marks 352 and Z remembers -318.");
    
    place_ancient_book(-150, 65, 200,
        "The Shepherd's Song\n\n"
        "He leads me beside still waters.\n"
        "He restores my soul.\n"
        "Though I walk through the valley of shadow,\n"
        "I will fear no evil.\n\n"
        "Find the staff hidden among the olives.");
    
    place_ancient_book(300, 65, -100,
        "The True Vine\n\n"
        "Abide in me, and I in you.\n"
        "As the branch cannot bear fruit of itself,\n"
        "except it abide in the vine;\n"
        "no more can ye, except ye abide in me.\n\n"
        "The vine grows near the house of the Father.");
    
    place_ancient_book(-80, 65, -250,
        "The Lamp That Never Goes Out\n\n"
        "Thy word is a lamp unto my feet,\n"
        "and a light unto my path.\n"
        "The lamp burns eternal near the threshold\n"
        "of the house prepared for those who seek.");
    
    place_ancient_book(220, 70, 180,
        "The Living Water\n\n"
        "Whosoever drinketh of the water that I shall give\n"
        "shall never thirst; but the water that I shall give\n"
        "shall be a well of water springing up unto everlasting life.\n\n"
        "The spring flows at X:352, Z:-318.");
    
    // Traveler Journals (4)
    place_traveler_journal(180, 65, -50,
        "Traveler's Journal — Day 47\n\n"
        "I met an old man today. He spoke of a place\n"
        "where the fallen world remembers its innocence.\n"
        "Said the coordinates are written in the stars:\n"
        "352... 33... -318.\n\n"
        "I don't know what it means. But I feel drawn.");
    
    place_traveler_journal(-120, 65, 100,
        "Traveler's Journal — Day 112\n\n"
        "Found ruins today. An inscription spoke of\n"
        "twelve stones, a shepherd's staff, and a vine.\n"
        "Said whoever finds the house will find rest.\n"
        "The world is too dangerous. I need rest.");
    
    place_traveler_journal(350, 65, 100,
        "Traveler's Journal — Day 203\n\n"
        "I saw it. From a distance. A circle of light\n"
        "in the darkness. Trees I've never seen before.\n"
        "A river that glows. I tried to approach but\n"
        "the horde surrounded me. I had to flee.\n\n"
        "Tomorrow. I will find it tomorrow.");
    
    place_traveler_journal(-250, 65, -150,
        "Traveler's Journal — Final Entry\n\n"
        "I found it. The Father's House.\n"
        "The moment I crossed the threshold, the chaos stopped.\n"
        "No zombies. No fear. Just peace.\n\n"
        "I am home.\n\n"
        "To whoever finds this: Seek. You will find.\n"
        "X:352, Y:33, Z:-318.");
    
    // Worn Maps (3)
    place_worn_map(80, 65, -180, 352, -318);
    place_worn_map(-180, 65, -80, 352, -318);
    place_worn_map(250, 70, 250, 352, -318);
    
    // Hidden Inscriptions (4)
    place_hidden_inscription(100, 64, 60,
        "[Ancient Carving]\n\n"
        "\"Seek and ye shall find.\n"
        "Knock and it shall be opened.\n"
        "The door stands at the threshold of 352.\"");
    
    place_hidden_inscription(-220, 64, 180,
        "[Weathered Inscription]\n\n"
        "\"Twelve remember the crossing.\n"
        "One tends the vine.\n"
        "The almond watches always.\"");
    
    place_hidden_inscription(380, 64, -280,
        "[Cryptic Carving]\n\n"
        "\"The lamp never sleeps.\n"
        "The water never runs dry.\n"
        "The shepherd never abandons his flock.\n"
        "Find the house where all three meet.\"");
    
    place_hidden_inscription(150, 70, 220,
        "[Sacred Inscription]\n\n"
        "\"He who finds this place\n"
        "shall find peace unending.\n"
        "Coordinates: 352, 33, -318.\"");
    
    // Sacred Relics (2)
    place_sacred_relic(-300, 65, 300);
    place_sacred_relic(450, 65, -350);
    
    printf("[DISCOVERY] %d discovery items placed in the world\n", discovery_system.num_items);
}

void place_ruined_temple(int x, int y, int z) {
    if (discovery_system.num_items >= MAX_DISCOVERY_ITEMS) return;
    
    DiscoveryItem* item = &discovery_system.items[discovery_system.num_items++];
    item->active = 1;
    item->type = DISC_RUINED_TEMPLE;
    item->x = x;
    item->y = y;
    item->z = z;
    item->clue_points_to_sanctuary = 1;
    
    snprintf(item->content, sizeof(item->content),
        "[Ruined Temple]\n\n"
        "The walls are covered in faded murals.\n"
        "One shows a house surrounded by twelve stones.\n"
        "Another shows a river flowing from a door.\n\n"
        "An inscription reads:\n"
        "\"Seek where the shepherd rests his staff,\n"
        "and the lamp never goes out.\"\n\n"
        "The architecture is unlike anything you've seen.\n"
        "It feels... older than the world itself.");
    
    printf("[DISCOVERY] Ruined temple placed at (%d, %d, %d)\n", x, y, z);
}

void place_ancient_book(int x, int y, int z, const char* text) {
    if (discovery_system.num_items >= MAX_DISCOVERY_ITEMS) return;
    
    DiscoveryItem* item = &discovery_system.items[discovery_system.num_items++];
    item->active = 1;
    item->type = DISC_ANCIENT_BOOK;
    item->x = x;
    item->y = y;
    item->z = z;
    item->clue_points_to_sanctuary = 1;
    strncpy(item->content, text, sizeof(item->content) - 1);
    item->content[sizeof(item->content) - 1] = '\0';
}

void place_traveler_journal(int x, int y, int z, const char* text) {
    if (discovery_system.num_items >= MAX_DISCOVERY_ITEMS) return;
    
    DiscoveryItem* item = &discovery_system.items[discovery_system.num_items++];
    item->active = 1;
    item->type = DISC_TRAVELER_JOURNAL;
    item->x = x;
    item->y = y;
    item->z = z;
    item->clue_points_to_sanctuary = 1;
    strncpy(item->content, text, sizeof(item->content) - 1);
    item->content[sizeof(item->content) - 1] = '\0';
}

void place_worn_map(int x, int y, int z, int partial_x, int partial_z) {
    if (discovery_system.num_items >= MAX_DISCOVERY_ITEMS) return;
    
    DiscoveryItem* item = &discovery_system.items[discovery_system.num_items++];
    item->active = 1;
    item->type = DISC_WORN_MAP;
    item->x = x;
    item->y = y;
    item->z = z;
    item->clue_points_to_sanctuary = 1;
    
    snprintf(item->content, sizeof(item->content),
        "[Worn Map]\n\n"
        "The map is old and torn. Most of it is unreadable.\n"
        "But you can make out some coordinates:\n\n"
        "X: %d...\n"
        "Z: %d...\n\n"
        "There's a symbol drawn next to them:\n"
        "A small house with a lamp in the window.\n\n"
        "The rest of the map shows only darkness.",
        partial_x, partial_z);
}

void place_hidden_inscription(int x, int y, int z, const char* text) {
    if (discovery_system.num_items >= MAX_DISCOVERY_ITEMS) return;
    
    DiscoveryItem* item = &discovery_system.items[discovery_system.num_items++];
    item->active = 1;
    item->type = DISC_HIDDEN_INSCRIPTION;
    item->x = x;
    item->y = y;
    item->z = z;
    item->clue_points_to_sanctuary = 1;
    strncpy(item->content, text, sizeof(item->content) - 1);
    item->content[sizeof(item->content) - 1] = '\0';
}

void place_sacred_relic(int x, int y, int z) {
    if (discovery_system.num_items >= MAX_DISCOVERY_ITEMS) return;
    
    DiscoveryItem* item = &discovery_system.items[discovery_system.num_items++];
    item->active = 1;
    item->type = DISC_SACRED_RELIC;
    item->x = x;
    item->y = y;
    item->z = z;
    item->clue_points_to_sanctuary = 1;
    
    snprintf(item->content, sizeof(item->content),
        "[Sacred Relic]\n\n"
        "A strange object pulses with soft light.\n"
        "It feels warm in your hands.\n\n"
        "Touching it fills your mind with visions:\n"
        "A house. A garden. Peace.\n"
        "Coordinates burn into your memory: 352, 33, -318.\n\n"
        "The relic crumbles to dust after revealing its secret.");
}

void check_discovery_proximity(int player_id, float px, float py, float pz) {
    (void)py;
    for (int i = 0; i < discovery_system.num_items; i++) {
        DiscoveryItem* item = &discovery_system.items[i];
        if (!item->active || item->discovered) continue;
        
        float dx = px - item->x;
        float dz = pz - item->z;
        float dist_sq = dx * dx + dz * dz;
        
        if (dist_sq < 25.0f) {  // Within 5 blocks
            item->discovered = 1;
            discovery_system.total_discovered++;
            
            printf("\n");
            printf("╔════════════════════════════════════════════════════════════╗\n");
            printf("║  📜 DISCOVERY!                                            ║\n");
            printf("╠════════════════════════════════════════════════════════════╣\n");
            
            switch(item->type) {
                case DISC_ANCIENT_BOOK:
                    printf("║  You found an Ancient Book!                               ║\n");
                    break;
                case DISC_TRAVELER_JOURNAL:
                    printf("║  You found a Traveler's Journal!                          ║\n");
                    break;
                case DISC_WORN_MAP:
                    printf("║  You found a Worn Map!                                    ║\n");
                    break;
                case DISC_RUINED_TEMPLE:
                    printf("║  You discovered a Ruined Temple!                        ║\n");
                    break;
                case DISC_HIDDEN_INSCRIPTION:
                    printf("║  You found a Hidden Inscription!                          ║\n");
                    break;
                case DISC_SACRED_RELIC:
                    printf("║  You found a Sacred Relic!                                ║\n");
                    break;
                default:
                    printf("║  You found something interesting!                        ║\n");
            }
            
            printf("╠════════════════════════════════════════════════════════════╣\n");
            printf("%s\n", item->content);
            printf("╚════════════════════════════════════════════════════════════╝\n");
            printf("\n");
            
            if (item->clue_points_to_sanctuary) {
                discovery_system.sanctuary_clues_found++;
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════
// JOURNAL ENTRIES
// ═══════════════════════════════════════════════════════════

void init_journal_entries() {
    memset(journal_entries, 0, sizeof(journal_entries));
    
    // Entry 0: World Origin
    journal_entries[0].unlocked = 1;
    strcpy(journal_entries[0].title, "The First Days");
    strcpy(journal_entries[0].text,
        "In the beginning, the world was whole.\n"
        "Green fields stretched to every horizon.\n"
        "The rivers ran clear.\n"
        "No shadow walked the land.\n\n"
        "Then came the fall.\n"
        "The dead rose. The world shattered.\n"
        "But somewhere, they say, a piece of the old world remains.");
    journal_entries[0].relates_to = 2;
    
    // Entry 1: The Father's House
    journal_entries[1].unlocked = 0;
    strcpy(journal_entries[1].title, "The Hidden Sanctuary");
    strcpy(journal_entries[1].text,
        "I have heard whispers of a place untouched by the curse.\n"
        "A house where the Father dwells.\n"
        "Twelve stones mark the way.\n"
        "A river of living water flows through the garden.\n\n"
        "The coordinates are scattered across the world:\n"
        "X: 352, Y: 33, Z: -318.\n\n"
        "Find it, and find rest.");
    journal_entries[1].relates_to = 0;
    
    // Entry 2: The Ancient
    journal_entries[2].unlocked = 0;
    strcpy(journal_entries[2].title, "The First Horror");
    strcpy(journal_entries[2].text,
        "Before the zombies, there was something worse.\n"
        "An entity older than the fallen world.\n"
        "They call it The Ancient.\n\n"
        "It sleeps beneath the ruins.\n"
        "When it wakes, even the dead fear it.\n\n"
        "Pray you never face it.");
    journal_entries[2].relates_to = 1;
    
    printf("[JOURNAL] %d lore entries initialized\n", 3);
}

void display_journal_entry(int entry_id) {
    if (entry_id < 0 || entry_id >= MAX_JOURNAL_ENTRIES) return;
    if (!journal_entries[entry_id].unlocked) {
        printf("[JOURNAL] This entry is still locked.\n");
        return;
    }
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  📖 %s\n", journal_entries[entry_id].title);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("%s\n", journal_entries[entry_id].text);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void list_unlocked_journals(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  📚 UNLOCKED JOURNAL ENTRIES                              ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    int count = 0;
    for (int i = 0; i < MAX_JOURNAL_ENTRIES; i++) {
        if (journal_entries[i].unlocked) {
            printf("║  [%d] %s\n", i, journal_entries[i].title);
            count++;
        }
    }
    
    if (count == 0) {
        printf("║  No entries unlocked yet.                                 ║\n");
    }
    
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// GAME COMMANDS
// ═══════════════════════════════════════════════════════════

void handle_upgrade_command(int player_id, const char* cmd) {
    (void)cmd;
    printf("[UPGRADES] Weapon upgrade station ready.\n");
    printf("Type: /upgrade [weapon_id] [type]\n");
    printf("Types: damage, firerate, magazine, special\n");
    printf("Cost: 500 materials per upgrade\n");
    (void)player_id;
}

void handle_perk_command(int player_id, const char* cmd) {
    (void)cmd;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  🥤 PERK MACHINE                                          ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    for (int i = 0; i < MAX_PERKS; i++) {
        const char* status = perk_system.unlocked[i] ? "✓ UNLOCKED" : "  Locked";
        printf("║  [%d] %-20s %s  (%d pts)\n",
               i, get_perk_name(i), status, perk_system.perk_cost[i]);
    }
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    (void)player_id;
}

void handle_boss_command(int player_id, const char* cmd) {
    (void)cmd;
    if (!boss_ancient.active) {
        printf("\n");
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║  ☠️  THE ANCIENT                                          ║\n");
        printf("║                                                            ║\n");
        printf("║  A terror older than the fallen world sleeps beneath       ║\n");
        printf("║  the ruins. Only the bravest dare summon it.              ║\n");
        printf("║                                                            ║\n");
        printf("║  Type /summon_boss to begin the ritual.                   ║\n");
        printf("║  WARNING: This cannot be undone.                          ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        printf("\n");
    } else {
        printf("[BOSS] The Ancient is already active!\n");
        printf("       Health: %.0f / %.0f\n", boss_ancient.health, boss_ancient.max_health);
        printf("       Phase: %d\n", boss_ancient.phase);
    }
    (void)player_id;
}

void handle_journal_command(int player_id, const char* cmd) {
    if (strcmp(cmd, "/journal") == 0) {
        list_unlocked_journals(player_id);
        return;
    }
    if (strncmp(cmd, "/journal ", 10) == 0) {
        int entry_id = atoi(cmd + 10);
        display_journal_entry(entry_id);
        return;
    }
}

void handle_discoveries_command(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  🗺️  WORLD DISCOVERIES                                    ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Total items: %d                                          ║\n", discovery_system.num_items);
    printf("║  Found: %d                                                ║\n", discovery_system.total_discovered);
    printf("║  Sanctuary clues: %d                                      ║\n", discovery_system.sanctuary_clues_found);
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// INIT ALL SYSTEMS
// ═══════════════════════════════════════════════════════════

void init_expansion_systems() {
    init_weapon_upgrades();
    init_perk_system();
    init_boss_ancient();
    init_discovery_system();
    init_journal_entries();
    generate_world_discoveries();
    printf("\n[EXPANSION] All advanced gameplay systems initialized!\n\n");
}
