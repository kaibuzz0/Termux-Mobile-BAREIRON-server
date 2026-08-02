/*
 * BAREIRON VILLAGES, NPCS, WITCH BOSS, AND RETRO HEROES
 * Implementation v2: Pathfinding, Trading, Hero Combat, Cleanup
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "villages_npcs.h"
#include "globals.h"

// ═══════════════════════════════════════════════════════════
// GLOBALS
// ═══════════════════════════════════════════════════════════

Village villages[MAX_VILLAGES];
Building village_buildings[MAX_VILLAGES][MAX_BUILDINGS_PER_VILLAGE];
NPC npcs[MAX_NPCS];
WitchBoss witch_boss;
RetroHero heroes[4];
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

// External references from zombie_game.c
extern struct Player_s { int id; char name[32]; float x, y, z; int weapon; int ammo; int max_ammo; int health; int max_health; int score; int kills; float speed_boost; float damage_boost; int invincible; time_t powerup_end; int materials; int barricades_built; } players[8];
extern int num_players;

// Forward declaration for zombie access
extern struct Zombie_s { int type; float x, y, z; float health; float max_health; float speed; float damage; int target_player; time_t spawn_time; int active; int can_explode; int can_spit; int is_invisible; float attack_range; } zombies[512];

// ═══════════════════════════════════════════════════════════
// VILLAGE GENERATION
// ═══════════════════════════════════════════════════════════

void init_village_system(void) {
    memset(villages, 0, sizeof(villages));
    memset(village_buildings, 0, sizeof(village_buildings));
    printf("[VILLAGES] Village system initialized\n");
}

const char* generate_village_name(void) {
    static char name[64];
    const char* prefix = village_prefixes[rand() % 20];
    const char* suffix = village_suffixes[rand() % 20];
    snprintf(name, sizeof(name), "%s %s", prefix, suffix);
    return name;
}

void generate_villages(void) {
    int positions[8][2] = {
        {-200, -200}, {300, 100}, {-100, 250},
        {400, -300}, {-300, 50}, {150, -400},
        {-400, -100}, {500, 200}
    };
    
    for (int i = 0; i < MAX_VILLAGES; i++) {
        Village* v = &villages[i];
        v->active = 1;
        v->x = positions[i][0];
        v->y = 64;
        v->z = positions[i][1];
        v->size = 4 + (rand() % 6);
        v->population = v->size * 2;
        v->danger_level = rand() % 3;
        v->discovered = 0;
        strncpy(v->name, generate_village_name(), sizeof(v->name) - 1);
        v->name[sizeof(v->name) - 1] = '\0';
        
        for (int b = 0; b < v->size; b++) {
            int bx = v->x + (rand() % 30) - 15;
            int bz = v->z + (rand() % 30) - 15;
            BuildingType types[] = {
                BLD_HOUSE_SMALL, BLD_HOUSE_SMALL, BLD_HOUSE_LARGE,
                BLD_WATCHTOWER, BLD_FARM, BLD_WELL, BLD_CHURCH,
                BLD_BARRACKS, BLD_SMITHY, BLD_TRADE_POST, BLD_GARDEN
            };
            place_building(i, types[rand() % 11], bx, 64, bz, rand() % 4);
        }
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
            b->condition = 1 + (rand() % 2);
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
            printf("\n╔════════════════════════════════════════════════════════════╗\n");
            printf("║  DISCOVERED: %-36s          ║\n", v->name);
            printf("║  Population: %-3d survivors                                ║\n", v->population);
            printf("║  Buildings: %-2d                                             ║\n", v->size);
            if (v->danger_level == 0) printf("║  Status: Safe                                              ║\n");
            else if (v->danger_level == 1) printf("║  Status: Cautious                                          ║\n");
            else printf("║  Status: Hostile forces nearby!                            ║\n");
            printf("║  Talk to villagers for supplies and information.           ║\n");
            printf("╚════════════════════════════════════════════════════════════╝\n\n");
        }
    }
}

void list_villages(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  KNOWN SETTLEMENTS                                         ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    int found = 0;
    for (int i = 0; i < MAX_VILLAGES; i++) {
        if (villages[i].active && villages[i].discovered) {
            printf("║  %s at (%d, %d)\n", villages[i].name, villages[i].x, villages[i].z);
            found++;
        }
    }
    if (!found) printf("║  No villages discovered yet.                                 ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// NPC SYSTEM
// ═══════════════════════════════════════════════════════════

void init_npc_system(void) {
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
            npc->npc_class = cls;
            npc->x = x;
            npc->y = 64;
            npc->z = z;
            npc->village_id = village_id;
            npc->following_player = -1;
            npc->behavior = (cls == NPC_SOLDIER) ? 1 : 0;
            npc->dialogue_state = 0;
            npc->has_quest = (rand() % 3 == 0) ? 1 : 0;
            npc->quest_completed = 0;
            npc->rare_spawn = 0;
            npc->defeated = 0;
            
            switch(cls) {
                case NPC_SOLDIER: npc->max_health = 150; break;
                case NPC_WITCH: npc->max_health = 5000; break;
                default: npc->max_health = 50; break;
            }
            npc->health = npc->max_health;
            
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
        if (b->type == BLD_HOUSE_LARGE) {
            spawn_npc(village_id, NPC_ELDER, b->x - 2, b->z - 2);
        }
        if (rand() % 3 == 0) {
            spawn_npc(village_id, NPC_CHILD, b->x, b->z + 5);
        }
    }
}

// ═══════════════════════════════════════════════════════════
// PATHFINDING (Simple lerp-based)
// ═══════════════════════════════════════════════════════════

void update_npc_pathfinding(void) {
    // NPCs that are following a player move toward them
    for (int i = 0; i < MAX_NPCS; i++) {
        NPC* npc = &npcs[i];
        if (!npc->active || npc->following_player < 0) continue;
        if (npc->following_player >= num_players) continue;
        
        float px = players[npc->following_player].x;
        float pz = players[npc->following_player].z;
        float dx = px - npc->x;
        float dz = pz - npc->z;
        float dist_sq = dx * dx + dz * dz;
        
        // If far, move closer (simple lerp)
        if (dist_sq > 16.0f) {  // More than 4 blocks away
            float speed = 2.0f;  // blocks per second
            float dist = sqrtf(dist_sq);
            float move = speed * 0.1f;  // per tick (10 ticks/sec)
            if (move > dist) move = dist;
            npc->x += (dx / dist) * move;
            npc->z += (dz / dist) * move;
        }
    }
    
    // Retro heroes that are following move too
    for (int i = 0; i < 4; i++) {
        RetroHero* hero = &heroes[i];
        if (!hero->active || hero->following_player < 0) continue;
        if (hero->following_player >= num_players) continue;
        
        float px = players[hero->following_player].x;
        float pz = players[hero->following_player].z;
        float dx = px - hero->x;
        float dz = pz - hero->z;
        float dist_sq = dx * dx + dz * dz;
        
        if (dist_sq > 16.0f) {
            float dist = sqrtf(dist_sq);
            float move = 3.0f * 0.1f;  // heroes are faster
            if (move > dist) move = dist;
            hero->x += (dx / dist) * move;
            hero->z += (dz / dist) * move;
        }
    }
}

void npc_follow_player(int npc_id, int player_id) {
    if (npc_id < 0 || npc_id >= MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active) return;
    npc->following_player = player_id;
    npc->behavior = 2;  // follow
    printf("[NPC] %s (%s) is now following you!\n", npc->name, get_npc_class_name(npc->npc_class));
}

void npc_interact(int player_id, int npc_id) {
    (void)player_id;
    if (npc_id < 0 || npc_id >= MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active) return;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  %-10s (%s)\n", npc->name, get_npc_class_name(npc->npc_class));
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  \"%s\"\n", get_npc_greeting(npc->npc_class, npc->dialogue_state));
    printf("║                                                            ║\n");
    
    if (npc->has_quest && !npc->quest_completed) {
        printf("║  QUEST: \"I need your help. There's something only an     ║\n");
        printf("║  outsider can do for us...\"                               ║\n");
    }
    
    if (npc->npc_class == NPC_TRADER || npc->npc_class == NPC_FARMER ||
        npc->npc_class == NPC_WEAPONSMITH || npc->npc_class == NPC_HEALER) {
        printf("║  [Trade] — Type /trade %d                                  ║\n", npc_id);
    }
    
    printf("║  [Follow] — Type /follow_npc %d to recruit                 ║\n", npc_id);
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    if (npc->dialogue_state < 2) npc->dialogue_state++;
}

// ═══════════════════════════════════════════════════════════
// TRADING SYSTEM (Actual Implementation)
// ═══════════════════════════════════════════════════════════

void init_trading_system(void) {
    memset(trader_inventories, 0, sizeof(trader_inventories));
    printf("[TRADE] Trading system initialized\n");
}

const char* get_item_name(int item_id) {
    switch(item_id) {
        case ITEM_AMMO: return "Ammo Clip";
        case ITEM_HEALTH_PACK: return "Health Pack";
        case ITEM_MATERIALS: return "Building Materials";
        case ITEM_WEAPON_UPGRADE: return "Weapon Upgrade";
        case ITEM_MAP_FRAGMENT: return "Map Fragment";
        default: return "Unknown Item";
    }
}

void generate_trader_inventory(int npc_id) {
    if (npc_id < 0 || npc_id >= MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active) return;
    
    TraderInventory* inv = &trader_inventories[npc_id];
    if (inv->num_offers > 0) return;  // Already generated
    
    inv->num_offers = 3 + (rand() % 2);  // 3-4 offers
    inv->last_restock = time(NULL);
    
    for (int i = 0; i < inv->num_offers; i++) {
        TradeOffer* offer = &inv->offers[i];
        offer->in_stock = 1;
        
        // Generate offers based on NPC class
        switch(npc->npc_class) {
            case NPC_FARMER:
                offer->item_id = ITEM_HEALTH_PACK;
                offer->quantity = 2 + (rand() % 3);
                offer->cost_materials = 50;
                offer->cost_score = 100;
                break;
            case NPC_WEAPONSMITH:
                offer->item_id = ITEM_AMMO;
                offer->quantity = 30 + (rand() % 20);
                offer->cost_materials = 25;
                offer->cost_score = 50;
                break;
            case NPC_TRADER:
                {
                    int items[] = {ITEM_AMMO, ITEM_HEALTH_PACK, ITEM_MATERIALS, ITEM_MAP_FRAGMENT};
                    offer->item_id = items[rand() % 4];
                    offer->quantity = (offer->item_id == ITEM_MATERIALS) ? 20 : 1;
                    offer->cost_materials = 30 + (rand() % 40);
                    offer->cost_score = 75 + (rand() % 100);
                }
                break;
            case NPC_HEALER:
                offer->item_id = ITEM_HEALTH_PACK;
                offer->quantity = 3 + (rand() % 3);
                offer->cost_materials = 40;
                offer->cost_score = 80;
                break;
            default:
                offer->item_id = ITEM_MATERIALS;
                offer->quantity = 10 + (rand() % 10);
                offer->cost_materials = 20;
                offer->cost_score = 40;
        }
    }
}

void restock_trader(int npc_id) {
    if (npc_id < 0 || npc_id >= MAX_NPCS) return;
    TraderInventory* inv = &trader_inventories[npc_id];
    if (time(NULL) - inv->last_restock < 300) return;  // Restock every 5 minutes
    
    for (int i = 0; i < inv->num_offers; i++) {
        inv->offers[i].in_stock = 1;
    }
    inv->last_restock = time(NULL);
    printf("[TRADE] Trader %d has restocked!\n", npc_id);
}

void show_trade_offers(int player_id, int npc_id) {
    (void)player_id;
    if (npc_id < 0 || npc_id >= MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active) return;
    
    generate_trader_inventory(npc_id);
    restock_trader(npc_id);
    TraderInventory* inv = &trader_inventories[npc_id];
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  💰 %-10s's Shop                                     ║\n", npc->name);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    for (int i = 0; i < inv->num_offers; i++) {
        TradeOffer* offer = &inv->offers[i];
        if (offer->in_stock) {
            printf("║  [%d] %-20s x%-2d  |  %d mats / %d pts     ║\n",
                   i, get_item_name(offer->item_id), offer->quantity,
                   offer->cost_materials, offer->cost_score);
        } else {
            printf("║  [%d] %-20s — SOLD OUT                                 ║\n",
                   i, get_item_name(offer->item_id));
        }
    }
    
    printf("║                                                            ║\n");
    printf("║  Type /buy %d [slot] to purchase                         ║\n", npc_id);
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

int execute_trade(int player_id, int npc_id, int offer_slot) {
    if (player_id < 0 || player_id >= 8) return 0;
    if (npc_id < 0 || npc_id >= MAX_NPCS) return 0;
    if (offer_slot < 0 || offer_slot >= TRADE_SLOTS) return 0;
    
    TraderInventory* inv = &trader_inventories[npc_id];
    if (offer_slot >= inv->num_offers) return 0;
    
    TradeOffer* offer = &inv->offers[offer_slot];
    if (!offer->in_stock) {
        printf("[TRADE] That item is sold out!\n");
        return 0;
    }
    
    // Check if player can afford (prefer materials, fallback to score)
    if (players[player_id].materials >= offer->cost_materials) {
        players[player_id].materials -= offer->cost_materials;
    } else if (players[player_id].score >= offer->cost_score) {
        players[player_id].score -= offer->cost_score;
    } else {
        printf("[TRADE] You can't afford that! Need %d materials or %d score.\n",
               offer->cost_materials, offer->cost_score);
        return 0;
    }
    
    // Give item to player
    offer->in_stock = 0;
    switch(offer->item_id) {
        case ITEM_AMMO:
            players[player_id].ammo += offer->quantity;
            printf("[TRADE] +%d ammo! (Total: %d)\n", offer->quantity, players[player_id].ammo);
            break;
        case ITEM_HEALTH_PACK:
            players[player_id].health += 50 * offer->quantity;
            if (players[player_id].health > players[player_id].max_health)
                players[player_id].health = players[player_id].max_health;
            printf("[TRADE] +%d health packs! Healed to %d HP.\n",
                   offer->quantity, players[player_id].health);
            break;
        case ITEM_MATERIALS:
            players[player_id].materials += offer->quantity;
            printf("[TRADE] +%d materials! (Total: %d)\n",
                   offer->quantity, players[player_id].materials);
            break;
        default:
            printf("[TRADE] You received %s x%d!\n", get_item_name(offer->item_id), offer->quantity);
    }
    
    return 1;
}

// ═══════════════════════════════════════════════════════════
// WITCH BOSS
// ═══════════════════════════════════════════════════════════

void init_witch_boss(void) {
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
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║     THE WITCH HAS APPEARED!                              ║\n");
    printf("║                                                            ║\n");
    printf("║  Phase 1: The Summoning                                    ║\n");
    printf("║  Health: 3,000                                             ║\n");
    printf("║                                                            ║\n");
    printf("║  \"The darkness answers my call...\"                       ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

void update_witch_boss(void) {
    if (!witch_boss.active || witch_boss.defeated) return;
    
    float hp_percent = witch_boss.health / witch_boss.max_health;
    time_t now = time(NULL);
    
    if (witch_boss.phase == WITCH_PHASE_1 && hp_percent <= 0.60f) {
        witch_boss.phase = WITCH_PHASE_2;
        printf("[WITCH] Phase 2: The Witch summons a swarm of shadows!\n");
    } else if (witch_boss.phase == WITCH_PHASE_2 && hp_percent <= 0.30f) {
        witch_boss.phase = WITCH_PHASE_3;
        printf("[WITCH] Phase 3: DESPERATION! Massive poison cloud!\n");
    }
    
    if (now - witch_boss.last_spell >= 8) {
        witch_cast_spell();
        witch_boss.last_spell = now;
    }
    if (now - witch_boss.last_summon >= 12) {
        witch_summon_zombies();
        witch_boss.last_summon = now;
    }
    if (witch_boss.phase == WITCH_PHASE_3 && now % 5 == 0) {
        witch_heal();
    }
    
    if (witch_boss.health <= 0) {
        witch_boss.defeated = 1;
        witch_boss.active = 0;
        printf("\n╔════════════════════════════════════════════════════════════╗\n");
        printf("║     THE WITCH IS DEFEATED!                               ║\n");
        printf("║                                                            ║\n");
        printf("║  The twisted figure crumbles to ash.                       ║\n");
        printf("║  A sacred artifact glows where the beast fell.             ║\n");
        printf("║                                                            ║\n");
        printf("║  ACHIEVEMENT: Witch Hunter                                ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n\n");
    }
}

void witch_cast_spell(void) {
    switch(witch_boss.phase) {
        case WITCH_PHASE_1: printf("[WITCH] The Witch throws a poison vial!\n"); break;
        case WITCH_PHASE_2: printf("[WITCH] The Witch commands shadows to attack!\n"); break;
        case WITCH_PHASE_3: witch_poison_cloud(); break;
    }
}

void witch_summon_zombies(void) {
    printf("[WITCH] The Witch raises the dead!\n");
    witch_boss.summons_total += 3;
}

void witch_poison_cloud(void) {
    printf("[WITCH] Poison cloud spreads! Stay away!\n");
}

void witch_heal(void) {
    witch_boss.health += 50;
    if (witch_boss.health > witch_boss.max_health)
        witch_boss.health = witch_boss.max_health;
    printf("[WITCH] The Witch draws life from corruption! (+50 HP)\n");
}

int is_witch_alive(void) {
    return witch_boss.active && !witch_boss.defeated;
}

void check_witch_defeat(int player_id) {
    if (witch_boss.defeated && !witch_boss.achievement_unlocked) {
        grant_witch_achievement(player_id);
    }
}

void grant_witch_achievement(int player_id) {
    (void)player_id;
    if (witch_boss.achievement_unlocked) return;
    witch_boss.achievement_unlocked = 1;
    printf("\n ACHIEVEMENT: Witch Hunter — Defeated the Witch!\n");
}

// ═══════════════════════════════════════════════════════════
// RETRO HERO EASTER EGGS (with combat)
// ═══════════════════════════════════════════════════════════

void init_hero_system(void) {
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

void attempt_hero_spawn(void) {
    if ((float)rand() / RAND_MAX > HERO_SPAWN_CHANCE) return;
    
    NPCClass hero_types[] = {NPC_HERO_MEGAMAN, NPC_HERO_LINK, NPC_HERO_SAMUS, NPC_HERO_MARIO};
    NPCClass chosen = hero_types[rand() % 4];
    
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
            RetroHero* hero = &heroes[i];
            hero->active = 1;
            hero->hero_type = hero_type;
            hero->x = x;
            hero->y = 64;
            hero->z = z;
            hero->health = 200;
            hero->following_player = -1;
            hero->kills = 0;
            hero->damage = 50;
            hero->attack_range = 8.0f;
            hero->last_attack = time(NULL);
            
            switch(hero_type) {
                case NPC_HERO_MEGAMAN:
                    strcpy(hero->name, HERO_MEGAMAN_NAME);
                    hero->damage = 75;
                    break;
                case NPC_HERO_LINK:
                    strcpy(hero->name, HERO_LINK_NAME);
                    hero->attack_range = 3.0f;  // melee
                    hero->damage = 100;
                    break;
                case NPC_HERO_SAMUS:
                    strcpy(hero->name, HERO_SAMUS_NAME);
                    hero->damage = 90;
                    break;
                case NPC_HERO_MARIO:
                    strcpy(hero->name, HERO_MARIO_NAME);
                    hero->damage = 60;
                    break;
                default: break;
            }
            strcpy(hero->catchphrase, get_hero_catchphrase(hero_type));
            
            printf("\n╔════════════════════════════════════════════════════════════╗\n");
            printf("║     RARE SPAWN!                                            ║\n");
            printf("║                                                            ║\n");
            printf("║  %s has appeared!                                          ║\n", hero->name);
            printf("║  \"%s\"\n", hero->catchphrase);
            printf("║                                                            ║\n");
            printf("║  %s\n", get_hero_description(hero_type));
            printf("║                                                            ║\n");
            printf("║  Type /follow to recruit them into your party.             ║\n");
            printf("╚════════════════════════════════════════════════════════════╝\n\n");
            return;
        }
    }
}

// Hero combat: attack nearby zombies
void hero_attack_zombies(void) {
    for (int h = 0; h < 4; h++) {
        RetroHero* hero = &heroes[h];
        if (!hero->active || hero->following_player < 0) continue;
        if (time(NULL) - hero->last_attack < 1) continue;  // Attack once per second
        
        // Find nearest zombie
        float nearest_dist = hero->attack_range * hero->attack_range;
        int target = -1;
        for (int z = 0; z < 512; z++) {
            if (!zombies[z].active) continue;
            float dx = zombies[z].x - hero->x;
            float dz = zombies[z].z - hero->z;
            float dist_sq = dx * dx + dz * dz;
            if (dist_sq < nearest_dist) {
                nearest_dist = dist_sq;
                target = z;
            }
        }
        
        if (target >= 0) {
            zombies[target].health -= hero->damage;
            hero->last_attack = time(NULL);
            
            const char* attack_verb = (hero->hero_type == NPC_HERO_LINK) ? "slashes" : "blasts";
            printf("[HERO] %s %s a zombie for %.0f damage!\n", hero->name, attack_verb, hero->damage);
            
            if (zombies[target].health <= 0) {
                zombies[target].active = 0;
                hero->kills++;
                printf("[HERO] %s destroyed a zombie! (Total kills: %d)\n", hero->name, hero->kills);
            }
        }
    }
}

void update_heroes(void) {
    update_npc_pathfinding();
    hero_attack_zombies();
}

void hero_interact(int player_id, int hero_id) {
    (void)player_id;
    if (hero_id < 0 || hero_id >= 4) return;
    RetroHero* hero = &heroes[hero_id];
    if (!hero->active) return;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  %s                                              ║\n", hero->name);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  \"%s\"\n", hero->catchphrase);
    printf("║  Damage: %.0f | Range: %.1f | Kills: %d                     ║\n",
           hero->damage, hero->attack_range, hero->kills);
    printf("║  Health: %.0f/200                                         ║\n", hero->health);
    if (hero->following_player == -1) {
        printf("║  [Type /follow to recruit]                                ║\n");
    } else if (hero->following_player == player_id) {
        printf("║  Currently following you.                                 ║\n");
    } else {
        printf("║  Following another player.                                 ║\n");
    }
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// INIT ALL SYSTEMS
// ═══════════════════════════════════════════════════════════

void init_village_npc_systems(void) {
    init_village_system();
    init_npc_system();
    init_trading_system();
    init_witch_boss();
    init_hero_system();
    
    generate_villages();
    
    for (int i = 0; i < MAX_VILLAGES; i++) {
        spawn_village_population(i);
    }
    
    printf("\n[VILLAGES/NPCS] All systems initialized!\n");
    printf("  Villages: %d\n", MAX_VILLAGES);
    printf("  NPCs: Up to %d\n", MAX_NPCS);
    printf("  Retro heroes: Enabled (0.1%% spawn chance)\n");
    printf("  Trading: Active with materials/score economy\n");
    printf("  Pathfinding: Simple lerp follow AI\n");
    printf("  Hero combat: Auto-attack nearby zombies\n\n");
}
