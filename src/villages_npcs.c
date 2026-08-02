/*
 * BAREIRON VILLAGES, NPCS, WITCH BOSS, AND RETRO HEROES
 * Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "villages_npcs.h"
#include "globals.h"
#include "registries.h"

// ═══════════════════════════════════════════════════════════
// GLOBALS
// ═══════════════════════════════════════════════════════════

Village villages[MAX_VILLAGES];
Building village_buildings[MAX_VILLAGES][MAX_BUILDINGS_PER_VILLAGE];
NPC npcs[MAX_NPCS];
WitchBoss witch_boss;
RetroHero heroes[4];  // Max 4 heroes at once
TraderInventory trader_inventories[MAX_NPCS];

// Village name fragments
static const char* village_prefixes[] = {
    "New", "Old", "Lost", "Hidden", "Fallen", "Last", "First", "Broken",
    "Silent", "Quiet", "Dusty", "Iron", "Stone", "Oak", "Pine", "Red",
    "White", "Black", "Green", "Golden"
};
static const char* village_suffixes[] = {
    "Haven", "Refuge", "Outpost", "Settlement", "Camp", "Village",
    "Town", "Crossing", "Ford", "Bridge", "Mill", "Farm", "Hold",
    "Watch", "Keep", "Grove", "Field", "Hollow", "Bend", "Peak"
};

// NPC names by class
static const char* soldier_names[] = {"Marcus", "Vega", "Steele", "Carter", "Brooks", "Ramirez", "Chen", "Okafor"};
static const char* farmer_names[] = {"Eli", "Rose", "Wheat", "Barley", "Corn", "Grove", "Dale", "Meadow"};
static const char* smith_names[] = {"Forge", "Hammer", "Anvil", "Ironhand", "Steel", "Coal", "Flint", "Spark"};
static const char* trader_names[] = {"Silver", "Gold", "Merchant", "Trader", "Coin", "Pouch", "Scale", "Deal"};
static const char* healer_names[] = {"Mercy", "Grace", "Hope", "Remedy", "Salve", "Bloom", "Life", "Pure"};
static const char* librarian_names[] = {"Wise", "Sage", "Scroll", "Book", "Tome", "Quill", "Ink", "Page"};
static const char* elder_names[] = {"Elder", "Ancient", "Grand", "Old", "Wise", "Elder", "Gray", "Dawn"};
static const char* survivor_names[] = {"Ash", "Dust", "Rust", "Remnant", "Shadow", "Ghost", "Wisp", "Echo"};

// ═══════════════════════════════════════════════════════════
// VILLAGE GENERATION
// ═══════════════════════════════════════════════════════════

void init_village_system() {
    memset(villages, 0, sizeof(villages));
    memset(village_buildings, 0, sizeof(village_buildings));
    printf("[VILLAGES] Village system initialized\n");
}

const char* generate_village_name() {
    static char name[64];
    const char* prefix = village_prefixes[rand() % 20];
    const char* suffix = village_suffixes[rand() % 20];
    snprintf(name, sizeof(name), "%s %s", prefix, suffix);
    return name;
}

void generate_villages() {
    // Place villages at pseudo-random locations away from sanctuary
    int village_positions[8][2] = {
        {-200, -200}, {300, 100}, {-100, 250},
        {400, -300}, {-300, 50}, {150, -400},
        {-400, -100}, {500, 200}
    };
    
    for (int i = 0; i < MAX_VILLAGES; i++) {
        Village* v = &villages[i];
        v->active = 1;
        v->x = village_positions[i][0];
        v->y = 64;
        v->z = village_positions[i][1];
        v->size = 4 + (rand() % 6);  // 4-9 buildings
        v->population = v->size * 2;   // Roughly 2 NPCs per building
        v->danger_level = rand() % 3;
        v->discovered = 0;
        strncpy(v->name, generate_village_name(), sizeof(v->name) - 1);
        v->name[sizeof(v->name) - 1] = '\0';
        
        // Generate buildings
        for (int b = 0; b < v->size; b++) {
            int bx = v->x + (rand() % 30) - 15;
            int bz = v->z + (rand() % 30) - 15;
            int rot = rand() % 4;
            
            BuildingType types[] = {
                BLD_HOUSE_SMALL, BLD_HOUSE_SMALL, BLD_HOUSE_LARGE,
                BLD_WATCHTOWER, BLD_FARM, BLD_WELL, BLD_CHURCH,
                BLD_BARRACKS, BLD_SMITHY, BLD_TRADE_POST, BLD_GARDEN
            };
            BuildingType bt = types[rand() % 11];
            
            place_building(i, bt, bx, 64, bz, rot);
        }
        
        // Always add a well at center
        place_building(i, BLD_WELL, v->x, 64, v->z, 0);
        
        printf("[VILLAGE] Generated '%s' at (%d, %d) with %d buildings\n",
               v->name, v->x, v->z, v->size);
    }
    
    printf("[VILLAGES] %d villages placed in the world\n", MAX_VILLAGES);
}

void place_building(int village_id, BuildingType type, int x, int y, int z, int rotation) {
    if (village_id < 0 || village_id >= MAX_VILLAGES) return;
    
    for (int i = 0; i < MAX_BUILDINGS_PER_VILLAGE; i++) {
        Building* b = &village_buildings[village_id][i];
        if (!b->active) {
            b->active = 1;
            b->type = type;
            b->x = x;
            b->y = y;
            b->z = z;
            b->rotation = rotation;
            b->condition = 1 + (rand() % 2);  // worn or good
            b->looted = 0;
            return;
        }
    }
}

void check_village_discovery(int player_id, float x, float z) {
    (void)player_id;
    for (int i = 0; i < MAX_VILLAGES; i++) {
        Village* v = &villages[i];
        if (!v->active || v->discovered) continue;
        
        float dx = x - v->x;
        float dz = z - v->z;
        if (dx * dx + dz * dz < VILLAGE_RADIUS * VILLAGE_RADIUS) {
            v->discovered = 1;
            printf("\n");
            printf("╔════════════════════════════════════════════════════════════╗\n");
            printf("║  🏘️  DISCOVERED: %s                                  ║\n", v->name);
            printf("║                                                            ║\n");
            printf("║  Population: %d survivors                                 ║\n", v->population);
            printf("║  Buildings: %d                                            ║\n", v->size);
            switch(v->danger_level) {
                case 0: printf("║  Status: Safe ✓                                           ║\n"); break;
                case 1: printf("║  Status: Cautious ⚠                                      ║\n"); break;
                case 2: printf("║  Status: Hostile forces nearby! ⚠⚠                       ║\n"); break;
            }
            printf("║                                                            ║\n");
            printf("║  Talk to villagers for supplies and information.           ║\n");
            printf("╚════════════════════════════════════════════════════════════╝\n");
            printf("\n");
        }
    }
}

void list_villages(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  🏘️  KNOWN SETTLEMENTS                                   ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    int found = 0;
    for (int i = 0; i < MAX_VILLAGES; i++) {
        if (villages[i].active && villages[i].discovered) {
            printf("║  %s at (%d, %d)\n", villages[i].name, villages[i].x, villages[i].z);
            found++;
        }
    }
    
    if (!found) {
        printf("║  No villages discovered yet.                              ║\n");
    }
    
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// NPC SYSTEM
// ═══════════════════════════════════════════════════════════

void init_npc_system() {
    memset(npcs, 0, sizeof(npcs));
    printf("[NPCS] NPC system initialized\n");
}

const char* get_npc_class_name(NPCClass cls) {
    switch(cls) {
        case NPC_SURVIVOR: return "Survivor";
        case NPC_SOLDIER: return "Soldier";
        case NPC_FARMER: return "Farmer";
        case NPC_WEAPONSMITH: return "Weaponsmith";
        case NPC_TRADER: return "Trader";
        case NPC_HEALER: return "Healer";
        case NPC_LIBRARIAN: return "Librarian";
        case NPC_CHILD: return "Child";
        case NPC_ELDER: return "Elder";
        case NPC_WITCH: return "The Witch";
        case NPC_HERO_MEGAMAN: return "Proto";
        case NPC_HERO_LINK: return "The Hero";
        case NPC_HERO_SAMUS: return "Bounty Hunter";
        case NPC_HERO_MARIO: return "The Plumber";
        default: return "Unknown";
    }
}

const char* get_npc_greeting(NPCClass cls, int familiarity) {
    if (familiarity == 0) {
        switch(cls) {
            case NPC_SOLDIER: return "Stay alert. The dead don't sleep.";
            case NPC_FARMER: return "Got food if you got coin.";
            case NPC_WEAPONSMITH: return "Need iron? I got iron.";
            case NPC_TRADER: return "What're you buyin'?";
            case NPC_HEALER: return "You look hurt. I can help.";
            case NPC_LIBRARIAN: return "Knowledge is the only thing they can't kill.";
            case NPC_ELDER: return "Sit, child. Let me tell you of the before-times.";
            case NPC_CHILD: return "Are you gonna save us?";
            default: return "Hello... are you friendly?";
        }
    } else if (familiarity == 1) {
        switch(cls) {
            case NPC_SOLDIER: return "Still alive? Good. Keep it that way.";
            case NPC_FARMER: return "Back for more? Harvest was good.";
            case NPC_WEAPONSMITH: return "Your gear holding up?";
            case NPC_TRADER: return "Got new stock. Take a look.";
            case NPC_HEALER: return "You look better than last time.";
            case NPC_LIBRARIAN: return "Found anything interesting out there?";
            case NPC_ELDER: return "The world speaks to those who listen.";
            case NPC_CHILD: return "Did you kill any zombies?";
            default: return "Good to see you again.";
        }
    } else {
        switch(cls) {
            case NPC_SOLDIER: return "Brother. We've got your back.";
            case NPC_FARMER: return "Family. Eat free today.";
            case NPC_WEAPONSMITH: return "For you? No charge on repairs.";
            case NPC_TRADER: return "Best prices for my favorite customer.";
            case NPC_HEALER: return "Your health is my joy.";
            case NPC_LIBRARIAN: return "I've saved the best stories for you.";
            case NPC_ELDER: return "You give this old heart hope.";
            case NPC_CHILD: return "You're my hero!";
            default: return "You're one of us now.";
        }
    }
}

void spawn_npc(int village_id, NPCClass cls, float x, float z) {
    for (int i = 0; i < MAX_NPCS; i++) {
        if (!npcs[i].active) {
            NPC* npc = &npcs[i];
            npc->active = 1;
            npc->id = i;
            npc->class = cls;
            npc->x = x;
            npc->y = 64;
            npc->z = z;
            npc->village_id = village_id;
            npc->following_player = -1;
            npc->behavior = (cls == NPC_SOLDIER) ? 1 : 0;  // soldiers patrol
            npc->dialogue_state = 0;
            npc->has_quest = (rand() % 3 == 0) ? 1 : 0;  // 33% have quests
            npc->quest_completed = 0;
            npc->rare_spawn = 0;
            npc->defeated = 0;
            
            // Set health
            switch(cls) {
                case NPC_SOLDIER: npc->max_health = 150; break;
                case NPC_WITCH: npc->max_health = 5000; break;
                default: npc->max_health = 50; break;
            }
            npc->health = npc->max_health;
            
            // Set name
            const char** name_pool = NULL;
            int pool_size = 8;
            switch(cls) {
                case NPC_SOLDIER: name_pool = soldier_names; break;
                case NPC_FARMER: name_pool = farmer_names; break;
                case NPC_WEAPONSMITH: name_pool = smith_names; break;
                case NPC_TRADER: name_pool = trader_names; break;
                case NPC_HEALER: name_pool = healer_names; break;
                case NPC_LIBRARIAN: name_pool = librarian_names; break;
                case NPC_ELDER: name_pool = elder_names; break;
                default: name_pool = survivor_names; break;
            }
            if (name_pool) {
                strncpy(npc->name, name_pool[rand() % pool_size], NPC_NAME_LEN - 1);
                npc->name[NPC_NAME_LEN - 1] = '\0';
            }
            
            return;
        }
    }
}

void spawn_village_population(int village_id) {
    if (village_id < 0 || village_id >= MAX_VILLAGES) return;
    Village* v = &villages[village_id];
    
    // Spawn NPCs based on village buildings
    for (int i = 0; i < MAX_BUILDINGS_PER_VILLAGE; i++) {
        Building* b = &village_buildings[village_id][i];
        if (!b->active) continue;
        
        NPCClass cls = NPC_SURVIVOR;
        switch(b->type) {
            case BLD_BARRACKS: cls = NPC_SOLDIER; break;
            case BLD_FARM: cls = NPC_FARMER; break;
            case BLD_SMITHY: cls = NPC_WEAPONSMITH; break;
            case BLD_TRADE_POST: cls = NPC_TRADER; break;
            case BLD_CHURCH: cls = NPC_HEALER; break;
            case BLD_LIBRARY: cls = NPC_LIBRARIAN; break;
            default: cls = NPC_SURVIVOR; break;
        }
        
        spawn_npc(village_id, cls, b->x + 2, b->z + 2);
        
        // Add an elder to large houses
        if (b->type == BLD_HOUSE_LARGE) {
            spawn_npc(village_id, NPC_ELDER, b->x - 2, b->z - 2);
        }
        
        // Add children randomly
        if (rand() % 3 == 0) {
            spawn_npc(village_id, NPC_CHILD, b->x, b->z + 5);
        }
    }
}

void npc_interact(int player_id, int npc_id) {
    if (npc_id < 0 || npc_id >= MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active) return;
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  👤 %s (%s)                                          ║\n",
           npc->name, get_npc_class_name(npc->class));
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  \"%s\"\n", get_npc_greeting(npc->class, npc->dialogue_state));
    printf("║                                                            ║\n");
    
    if (npc->has_quest && !npc->quest_completed) {
        printf("║  💬 QUEST AVAILABLE                                        ║\n");
        printf("║  \"I need your help. There's something only an outsider      ║\n");
        printf("║   can do for us...\"                                        ║\n");
    }
    
    if (npc->class == NPC_TRADER || npc->class == NPC_FARMER ||
        npc->class == NPC_WEAPONSMITH || npc->class == NPC_HEALER) {
        printf("║                                                            ║\n");
        printf("║  [Trade] — Type /trade %d to see offers                  ║\n", npc_id);
    }
    
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Increase familiarity
    if (npc->dialogue_state < 2) npc->dialogue_state++;
    
    (void)player_id;
}

// ═══════════════════════════════════════════════════════════
// WITCH BOSS
// ═══════════════════════════════════════════════════════════

void init_witch_boss() {
    memset(&witch_boss, 0, sizeof(witch_boss));
    printf("[WITCH] Witch boss system initialized\n");
}

void spawn_witch_boss(float x, float z) {
    if (witch_boss.active) {
        printf("[WITCH] The Witch already prowls these lands!\n");
        return;
    }
    
    witch_boss.active = 1;
    witch_boss.x = x;
    witch_boss.y = 64;
    witch_boss.z = z;
    witch_boss.max_health = 3000;
    witch_boss.health = 3000;
    witch_boss.phase = WITCH_PHASE_1;
    witch_boss.last_spell = time(NULL);
    witch_boss.last_summon = time(NULL);
    witch_boss.summons_total = 0;
    witch_boss.defeated = 0;
    witch_boss.achievement_unlocked = 0;
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                                                            ║\n");
    printf("║     🧙 THE WITCH HAS APPEARED! 🧙                         ║\n");
    printf("║                                                            ║\n");
    printf("║  \"The darkness answers my call...\"                       ║\n");
    printf("║                                                            ║\n");
    printf("║  A twisted figure in tattered robes.                     ║\n");
    printf("║  Her eyes glow with sickly green light.                    ║\n");
    printf("║  The air grows cold. The shadows stir.                     ║\n");
    printf("║                                                            ║\n");
    printf("║  ACHIEVEMENT: Defeat the Witch to unlock 'Witch Hunter'    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void update_witch_boss() {
    if (!witch_boss.active || witch_boss.defeated) return;
    
    float hp_percent = witch_boss.health / witch_boss.max_health;
    time_t now = time(NULL);
    
    // Phase transitions
    if (witch_boss.phase == WITCH_PHASE_1 && hp_percent <= 0.60f) {
        witch_boss.phase = WITCH_PHASE_2;
        printf("[WITCH] 🦇 Phase 2: The Witch summons a swarm of shadows!\n");
    } else if (witch_boss.phase == WITCH_PHASE_2 && hp_percent <= 0.30f) {
        witch_boss.phase = WITCH_PHASE_3;
        printf("[WITCH] ☠️ Phase 3: The Witch enters desperation! Massive poison cloud!\n");
    }
    
    // Cast spells every 8 seconds
    if (now - witch_boss.last_spell >= 8) {
        witch_cast_spell();
        witch_boss.last_spell = now;
    }
    
    // Summon zombies every 12 seconds
    if (now - witch_boss.last_summon >= 12) {
        witch_summon_zombies();
        witch_boss.last_summon = now;
    }
    
    // Phase 3 healing
    if (witch_boss.phase == WITCH_PHASE_3 && now % 5 == 0) {
        witch_heal();
    }
    
    // Check defeat
    if (witch_boss.health <= 0) {
        witch_boss.defeated = 1;
        witch_boss.active = 0;
        printf("\n");
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║     🏆 THE WITCH IS DEFEATED! 🏆                         ║\n");
        printf("║                                                            ║\n");
        printf("║  The twisted figure crumbles to ash.                       ║\n");
        printf("║  The shadows retreat. The air warms.                       ║\n");
        printf("║                                                            ║\n");
        printf("║  🎖️ ACHIEVEMENT UNLOCKED: Witch Hunter                     ║\n");
        printf("║  You have proven yourself against the darkness.            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        printf("\n");
    }
}

void witch_cast_spell() {
    switch(witch_boss.phase) {
        case WITCH_PHASE_1:
            printf("[WITCH] 🧪 The Witch throws a poison vial!\n");
            break;
        case WITCH_PHASE_2:
            printf("[WITCH] 🦇 The Witch commands shadows to attack!\n");
            break;
        case WITCH_PHASE_3:
            witch_poison_cloud();
            break;
    }
}

void witch_summon_zombies() {
    printf("[WITCH] 💀 The Witch raises the dead!\n");
    witch_boss.summons_total += 3;
}

void witch_poison_cloud() {
    printf("[WITCH] ☠️ Poison cloud spreads! Stay away!\n");
}

void witch_heal() {
    witch_boss.health += 50;
    if (witch_boss.health > witch_boss.max_health) {
        witch_boss.health = witch_boss.max_health;
    }
    printf("[WITCH] 🌿 The Witch draws life from the corruption! (+50 HP)\n");
}

void grant_witch_achievement(int player_id) {
    (void)player_id;
    if (witch_boss.achievement_unlocked) return;
    witch_boss.achievement_unlocked = 1;
    printf("\n🎖️ ACHIEVEMENT: Witch Hunter — Defeated the Witch in combat!\n");
}

int is_witch_alive() {
    return witch_boss.active && !witch_boss.defeated;
}

void check_witch_defeat(int player_id) {
    if (witch_boss.defeated && !witch_boss.achievement_unlocked) {
        grant_witch_achievement(player_id);
    }
}

// ═══════════════════════════════════════════════════════════
// RETRO HERO EASTER EGGS
// ═══════════════════════════════════════════════════════════

void init_hero_system() {
    memset(heroes, 0, sizeof(heroes));
    printf("[HEROES] Retro hero system initialized\n");
}

const char* get_hero_catchphrase(NPCClass hero_type) {
    switch(hero_type) {
        case NPC_HERO_MEGAMAN: return "I fight for those who cannot.";
        case NPC_HERO_LINK: return "HYAH! The princess... I mean, let's go!";
        case NPC_HERO_SAMUS: return "Mission accepted. Target: undead.";
        case NPC_HERO_MARIO: return "It's-a-me! Let's-a-go!";
        default: return "...";
    }
}

const char* get_hero_description(NPCClass hero_type) {
    switch(hero_type) {
        case NPC_HERO_MEGAMAN: return "A blue android from a forgotten war. Fights with arm cannon.";
        case NPC_HERO_LINK: return "A hero from another world. Sword and shield at the ready.";
        case NPC_HERO_SAMUS: return "A bounty hunter in power armor. Missiles and ice beam.";
        case NPC_HERO_MARIO: return "A plumber who fell through a pipe into this world. Jumps high.";
        default: return "A mysterious wanderer.";
    }
}

void attempt_hero_spawn() {
    if ((float)rand() / RAND_MAX > HERO_SPAWN_CHANCE) return;
    
    NPCClass hero_types[] = {NPC_HERO_MEGAMAN, NPC_HERO_LINK, NPC_HERO_SAMUS, NPC_HERO_MARIO};
    NPCClass chosen = hero_types[rand() % 4];
    
    // Find empty hero slot
    for (int i = 0; i < 4; i++) {
        if (!heroes[i].active) {
            spawn_hero(chosen, 0, 0);
            return;
        }
    }
}

void spawn_hero(NPCClass hero_type, float x, float z) {
    for (int i = 0; i < 4; i++) {
        if (!heroes[i].active) {
            heroes[i].active = 1;
            heroes[i].hero_type = hero_type;
            heroes[i].x = x;
            heroes[i].y = 64;
            heroes[i].z = z;
            heroes[i].health = 200;
            heroes[i].following_player = -1;
            heroes[i].kills = 0;
            
            switch(hero_type) {
                case NPC_HERO_MEGAMAN:
                    strcpy(heroes[i].name, HERO_MEGAMAN_NAME);
                    strcpy(heroes[i].catchphrase, get_hero_catchphrase(hero_type));
                    break;
                case NPC_HERO_LINK:
                    strcpy(heroes[i].name, HERO_LINK_NAME);
                    strcpy(heroes[i].catchphrase, get_hero_catchphrase(hero_type));
                    break;
                case NPC_HERO_SAMUS:
                    strcpy(heroes[i].name, HERO_SAMUS_NAME);
                    strcpy(heroes[i].catchphrase, get_hero_catchphrase(hero_type));
                    break;
                case NPC_HERO_MARIO:
                    strcpy(heroes[i].name, HERO_MARIO_NAME);
                    strcpy(heroes[i].catchphrase, get_hero_catchphrase(hero_type));
                    break;
                default: break;
            }
            
            printf("\n");
            printf("╔════════════════════════════════════════════════════════════╗\n");
            printf("║     ⭐ RARE SPAWN! ⭐                                     ║\n");
            printf("║                                                            ║\n");
            printf("║  %s has appeared!                                         ║\n", heroes[i].name);
            printf("║                                                            ║\n");
            printf("║  \"%s\"\n", heroes[i].catchphrase);
            printf("║                                                            ║\n");
            printf("║  %s\n", get_hero_description(hero_type));
            printf("║                                                            ║\n");
            printf("║  Type /follow to have them join your party.               ║\n");
            printf("╚════════════════════════════════════════════════════════════╝\n");
            printf("\n");
            return;
        }
    }
}

void hero_interact(int player_id, int hero_id) {
    if (hero_id < 0 || hero_id >= 4) return;
    RetroHero* hero = &heroes[hero_id];
    if (!hero->active) return;
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  ⭐ %s                                              ║\n", hero->name);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  \"%s\"\n", hero->catchphrase);
    printf("║                                                            ║\n");
    printf("║  Kills: %d                                                ║\n", hero->kills);
    printf("║  Health: %.0f/200                                         ║\n", hero->health);
    printf("║                                                            ║\n");
    if (hero->following_player == -1) {
        printf("║  [Type /follow to recruit]                                ║\n");
    } else if (hero->following_player == player_id) {
        printf("║  Currently following you.                                ║\n");
    } else {
        printf("║  Following another player.                               ║\n");
    }
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

// ═══════════════════════════════════════════════════════════
// INIT ALL SYSTEMS
// ═══════════════════════════════════════════════════════════

void init_village_npc_systems() {
    init_village_system();
    init_npc_system();
    init_witch_boss();
    init_hero_system();
    
    generate_villages();
    
    // Spawn NPCs in each village
    for (int i = 0; i < MAX_VILLAGES; i++) {
        spawn_village_population(i);
    }
    
    printf("\n[VILLAGES/NPCS] All systems initialized!\n");
    printf("  Villages: %d\n", MAX_VILLAGES);
    printf("  NPCs: Up to %d\n", MAX_NPCS);
    printf("  Retro heroes: Enabled (0.1%% spawn chance)\n");
    printf("  Witch boss: Ready to summon\n\n");
}
