/*
 * BAREIRON VILLAGES, NPCS, WITCH BOSS, RETRO HEROES
 * Implementation v3: More content, NPC combat, quests, world events
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
Quest quests[MAX_QUESTS];
WorldEvent world_events[MAX_ACTIVE_EVENTS];

// Village name fragments
static const char* village_prefixes[] = {
    "New","Old","Lost","Hidden","Fallen","Last","First","Broken",
    "Silent","Quiet","Dusty","Iron","Stone","Oak","Pine","Red",
    "White","Black","Green","Golden"
};
static const char* village_suffixes[] = {
    "Haven","Refuge","Outpost","Settlement","Camp","Village",
    "Town","Crossing","Ford","Bridge","Mill","Farm","Hold",
    "Watch","Keep","Grove","Field","Hollow","Bend","Peak"
};

static const char* soldier_names[] = {"Marcus","Vega","Steele","Carter","Brooks","Ramirez","Chen","Okafor"};
static const char* farmer_names[] = {"Eli","Rose","Wheat","Barley","Corn","Grove","Dale","Meadow"};
static const char* smith_names[] = {"Forge","Hammer","Anvil","Ironhand","Steel","Coal","Flint","Spark"};
static const char* trader_names[] = {"Silver","Gold","Merchant","Trader","Coin","Pouch","Scale","Deal"};
static const char* healer_names[] = {"Mercy","Grace","Hope","Remedy","Salve","Bloom","Life","Pure"};
static const char* librarian_names[] = {"Wise","Sage","Scroll","Book","Tome","Quill","Ink","Page"};
static const char* elder_names[] = {"Elder","Ancient","Grand","Old","Wise","Elder","Gray","Dawn"};
static const char* survivor_names[] = {"Ash","Dust","Rust","Remnant","Shadow","Ghost","Wisp","Echo"};
static const char* fisher_names[] = {"River","Brook","Wave","Reed","Dock","Net","Boat","Shore"};
static const char* miner_names[] = {"Pick","Ore","Deep","Stone","Coal","Iron","Gem","Shaft"};
static const char* hunter_names[] = {"Wolf","Stalker","Track","Fletch","Hawk","Deer","Bear","Forest"};
static const char* guard_names[] = {"Shield","Spear","Watch","Vigil","Titan","Steel","Iron","Sentinel"};
static const char* scholar_names[] = {"Quill","Tome","Scroll","Ink","Page","Study","Learn","Mind"};
static const char* bard_names[] = {"Song","Lyric","Tune","Chord","Harp","Flute","Drum","Voice"};
static const char* alchemist_names[] = {"Brew","Flask","Elixir","Vial","Amber","Crystal","Mist","Smoke"};
static const char* innkeeper_names[] = {"Mug","Tap","Keg","Hearth","Fire","Warm","Cask","Barrel"};

// External references
extern struct Player_s { int id; char name[32]; float x,y,z; int weapon; int ammo; int max_ammo; int health; int max_health; int score; int kills; float speed_boost; float damage_boost; int invincible; time_t powerup_end; int materials; int barricades_built; } players[8];
extern int num_players;
extern struct Zombie_s { int type; float x,y,z; float health; float max_health; float speed; float damage; int target_player; time_t spawn_time; int active; int can_explode; int can_spit; int is_invisible; float attack_range; } zombies[512];

// ═══════════════════════════════════════════════════════════
// VILLAGE GENERATION (expanded types)
// ═══════════════════════════════════════════════════════════

void init_village_system(void) {
    memset(villages, 0, sizeof(villages));
    memset(village_buildings, 0, sizeof(village_buildings));
    printf("[VILLAGES] Village system initialized (v3)\n");
}

const char* generate_village_name(void) {
    static char name[64];
    snprintf(name, sizeof(name), "%s %s",
             village_prefixes[rand()%20], village_suffixes[rand()%20]);
    return name;
}

const char* get_village_type_name(VillageType vtype) {
    switch(vtype) {
        case VTYPE_FISHING: return "Fishing Village";
        case VTYPE_MINING: return "Mining Town";
        case VTYPE_FARMING: return "Farming Community";
        case VTYPE_FORTIFIED: return "Fortified Settlement";
        case VTYPE_RELIGIOUS: return "Holy Ground";
        case VTYPE_TRADING: return "Trade Hub";
        case VTYPE_ACADEMIC: return "Scholar's Retreat";
        default: return "Settlement";
    }
}

void generate_villages(void) {
    int positions[12][2] = {
        {-200,-200},{300,100},{-100,250},{400,-300},
        {-300,50},{150,-400},{-400,-100},{500,200},
        {0,350},{350,-150},{-150,-350},{250,300}
    };
    VillageType vtypes[] = {
        VTYPE_GENERIC,VTYPE_FISHING,VTYPE_MINING,VTYPE_FARMING,
        VTYPE_FORTIFIED,VTYPE_RELIGIOUS,VTYPE_TRADING,VTYPE_ACADEMIC,
        VTYPE_GENERIC,VTYPE_FISHING,VTYPE_FORTIFIED,VTYPE_TRADING
    };
    
    for (int i=0; i<MAX_VILLAGES; i++) {
        Village* v = &villages[i];
        v->active = 1;
        v->vtype = vtypes[i];
        v->x = positions[i][0];
        v->y = 64;
        v->z = positions[i][1];
        v->size = 4 + (rand()%6);
        v->population = v->size * 2;
        v->danger_level = rand()%3;
        v->discovered = 0;
        v->under_attack = 0;
        v->defense_rating = 0;
        strncpy(v->name, generate_village_name(), sizeof(v->name)-1);
        v->name[sizeof(v->name)-1] = '\0';
        
        // Type-specific buildings
        for (int b=0; b<v->size; b++) {
            int bx = v->x + (rand()%30)-15;
            int bz = v->z + (rand()%30)-15;
            BuildingType bt = BLD_HOUSE_SMALL;
            
            switch(v->vtype) {
                case VTYPE_FISHING:
                    {BuildingType opts[]={BLD_HOUSE_SMALL,BLD_FISHING_DOCK,BLD_WELL,BLD_TRADE_POST}; bt=opts[rand()%4];}
                    break;
                case VTYPE_MINING:
                    {BuildingType opts[]={BLD_HOUSE_SMALL,BLD_MINE_SHAFT,BLD_SMITHY,BLD_WELL}; bt=opts[rand()%4];}
                    break;
                case VTYPE_FARMING:
                    {BuildingType opts[]={BLD_HOUSE_SMALL,BLD_FARM,BLD_BARN,BLD_WINDMILL,BLD_WELL}; bt=opts[rand()%5];}
                    break;
                case VTYPE_FORTIFIED:
                    {BuildingType opts[]={BLD_HOUSE_SMALL,BLD_WATCHTOWER,BLD_BARRACKS,BLD_WELL}; bt=opts[rand()%4];}
                    break;
                case VTYPE_RELIGIOUS:
                    {BuildingType opts[]={BLD_HOUSE_SMALL,BLD_CHURCH,BLD_CEMETERY,BLD_LIBRARY}; bt=opts[rand()%4];}
                    break;
                case VTYPE_TRADING:
                    {BuildingType opts[]={BLD_HOUSE_SMALL,BLD_MARKET,BLD_TAVERN,BLD_TRADE_POST,BLD_STABLE}; bt=opts[rand()%5];}
                    break;
                case VTYPE_ACADEMIC:
                    {BuildingType opts[]={BLD_HOUSE_SMALL,BLD_LIBRARY,BLD_LIBRARY,BLD_WELL}; bt=opts[rand()%4];}
                    break;
                default:
                    {BuildingType opts[]={BLD_HOUSE_SMALL,BLD_HOUSE_SMALL,BLD_HOUSE_LARGE,BLD_WATCHTOWER,BLD_FARM,BLD_WELL,BLD_CHURCH,BLD_BARRACKS,BLD_SMITHY,BLD_TRADE_POST,BLD_GARDEN}; bt=opts[rand()%11];}
            }
            place_building(i, bt, bx, 64, bz, rand()%4);
        }
        place_building(i, BLD_WELL, v->x, 64, v->z, 0);
        
        printf("[VILLAGE] Generated '%s' (%s) at (%d, %d) with %d buildings\n",
               v->name, get_village_type_name(v->vtype), v->x, v->z, v->size);
    }
    printf("[VILLAGES] %d villages placed\n", MAX_VILLAGES);
}

void update_village_defense(int village_id) {
    if (village_id < 0 || village_id >= MAX_VILLAGES) return;
    Village* v = &villages[village_id];
    v->defense_rating = 0;
    
    // Count soldiers and watchtowers
    for (int i=0; i<MAX_NPCS; i++) {
        if (npcs[i].active && npcs[i].village_id==village_id && npcs[i].npc_class==NPC_SOLDIER)
            v->defense_rating += 10;
    }
    for (int b=0; b<MAX_BUILDINGS_PER_VILLAGE; b++) {
        if (village_buildings[village_id][b].active && village_buildings[village_id][b].type==BLD_WATCHTOWER)
            v->defense_rating += 5;
    }
}

void place_building(int village_id, BuildingType type, int x, int y, int z, int rotation) {
    if (village_id < 0 || village_id >= MAX_VILLAGES) return;
    for (int i=0; i<MAX_BUILDINGS_PER_VILLAGE; i++) {
        Building* b = &village_buildings[village_id][i];
        if (!b->active) {
            b->active = 1;
            b->type = type;
            b->x = x; b->y = y; b->z = z;
            b->rotation = rotation;
            b->condition = 1+(rand()%2);
            b->looted = 0;
            return;
        }
    }
}

void check_village_discovery(int player_id, float x, float z) {
    (void)player_id;
    for (int i=0; i<MAX_VILLAGES; i++) {
        Village* v = &villages[i];
        if (!v->active || v->discovered) continue;
        float dx = x-v->x, dz = z-v->z;
        if (dx*dx + dz*dz < VILLAGE_RADIUS*VILLAGE_RADIUS) {
            v->discovered = 1;
            printf("\n╔════════════════════════════════════════════════════════════╗\n");
            printf("║  DISCOVERED: %-36s          ║\n", v->name);
            printf("║  Type: %-20s                                ║\n", get_village_type_name(v->vtype));
            printf("║  Population: %-3d                                          ║\n", v->population);
            printf("║  Buildings: %-2d                                            ║\n", v->size);
            if (v->danger_level==0) printf("║  Status: Safe                                             ║\n");
            else if (v->danger_level==1) printf("║  Status: Cautious                                         ║\n");
            else printf("║  Status: Hostile forces nearby!                           ║\n");
            printf("║  Talk to villagers for supplies and quests.               ║\n");
            printf("╚════════════════════════════════════════════════════════════╝\n\n");
        }
    }
}

void list_villages(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  KNOWN SETTLEMENTS                                        ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    int found=0;
    for (int i=0; i<MAX_VILLAGES; i++) {
        if (villages[i].active && villages[i].discovered) {
            Village* v = &villages[i];
            printf("║  %-20s (%s) at (%d,%d)", v->name,
                   get_village_type_name(v->vtype), v->x, v->z);
            if (v->under_attack) printf(" [UNDER ATTACK!]");
            printf("\n");
            found++;
        }
    }
    if (!found) printf("║  No villages discovered yet.                              ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// NPC SYSTEM (expanded with combat stats)
// ═══════════════════════════════════════════════════════════

void init_npc_system(void) {
    memset(npcs, 0, sizeof(npcs));
    printf("[NPCS] NPC system initialized (v3 with combat)\n");
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
        case NPC_FISHER: return "Fisher";
        case NPC_MINER: return "Miner";
        case NPC_HUNTER: return "Hunter";
        case NPC_GUARD_CAPTAIN: return "Guard Captain";
        case NPC_SCHOLAR: return "Scholar";
        case NPC_BARD: return "Bard";
        case NPC_ALCHEMIST: return "Alchemist";
        case NPC_INNKEEPER: return "Innkeeper";
        default: return "Unknown";
    }
}

const char* get_npc_greeting(NPCClass cls, int familiarity) {
    if (familiarity==0) {
        switch(cls) {
            case NPC_SOLDIER: return "Stay alert. The dead don't sleep.";
            case NPC_FARMER: return "Got food if you got coin.";
            case NPC_WEAPONSMITH: return "Need iron? I got iron.";
            case NPC_TRADER: return "What're you buyin'?";
            case NPC_HEALER: return "You look hurt. I can help.";
            case NPC_LIBRARIAN: return "Knowledge is the only thing they can't kill.";
            case NPC_ELDER: return "Sit, child. Let me tell you of the before-times.";
            case NPC_CHILD: return "Are you gonna save us?";
            case NPC_FISHER: return "The river's calm today. Too calm.";
            case NPC_MINER: return "Deep earth keeps its secrets.";
            case NPC_HUNTER: return "I've seen things in the woods. Terrible things.";
            case NPC_GUARD_CAPTAIN: return "This village is under my protection.";
            case NPC_SCHOLAR: return "The ancients left warnings. We didn't listen.";
            case NPC_BARD: return "I've got songs of the old world, if you want to hear.";
            case NPC_ALCHEMIST: return "Brews for pain, brews for strength. What do you need?";
            case NPC_INNKEEPER: return "Room and board for a fair price. Watch your back.";
            default: return "Hello... are you friendly?";
        }
    } else if (familiarity==1) {
        switch(cls) {
            case NPC_SOLDIER: return "Still alive? Good. Keep it that way.";
            case NPC_FARMER: return "Back for more? Harvest was good.";
            case NPC_GUARD_CAPTAIN: return "The watch has reported fewer undead lately.";
            case NPC_BARD: return "I wrote a song about you. Want to hear it?";
            default: return "Good to see you again.";
        }
    } else {
        switch(cls) {
            case NPC_SOLDIER: return "Brother. We've got your back.";
            case NPC_GUARD_CAPTAIN: return "You have the full support of my guard.";
            case NPC_BARD: return "You are the hero of my finest ballad!";
            case NPC_ALCHEMIST: return "My best brews are yours, friend.";
            default: return "You're one of us now.";
        }
    }
}

void spawn_npc(int village_id, NPCClass cls, float x, float z) {
    for (int i=0; i<MAX_NPCS; i++) {
        if (!npcs[i].active) {
            NPC* npc = &npcs[i];
            npc->active = 1;
            npc->id = i;
            npc->npc_class = cls;
            npc->x = x; npc->y = 64; npc->z = z;
            npc->village_id = village_id;
            npc->following_player = -1;
            npc->behavior = (cls==NPC_SOLDIER || cls==NPC_GUARD_CAPTAIN) ? 1 : 0;
            npc->dialogue_state = 0;
            npc->has_quest = (rand()%3==0) ? 1 : 0;
            npc->quest_completed = 0;
            npc->rare_spawn = 0;
            npc->defeated = 0;
            npc->kills = 0;
            npc->last_attack = 0;
            
            // Set combat stats by class
            switch(cls) {
                case NPC_SOLDIER: npc->max_health=150; npc->damage=25; npc->attack_range=4; npc->attack_speed=1; break;
                case NPC_GUARD_CAPTAIN: npc->max_health=250; npc->damage=40; npc->attack_range=4; npc->attack_speed=1; break;
                case NPC_HUNTER: npc->max_health=100; npc->damage=35; npc->attack_range=12; npc->attack_speed=2; break;
                case NPC_WITCH: npc->max_health=5000; break;
                default: npc->max_health=50; npc->damage=0; npc->attack_range=0; break;
            }
            npc->health = npc->max_health;
            
            const char** pool = NULL; int pool_size = 8;
            switch(cls) {
                case NPC_SOLDIER: pool = soldier_names; break;
                case NPC_FARMER: pool = farmer_names; break;
                case NPC_WEAPONSMITH: pool = smith_names; break;
                case NPC_TRADER: pool = trader_names; break;
                case NPC_HEALER: pool = healer_names; break;
                case NPC_LIBRARIAN: pool = librarian_names; break;
                case NPC_ELDER: pool = elder_names; break;
                case NPC_FISHER: pool = fisher_names; break;
                case NPC_MINER: pool = miner_names; break;
                case NPC_HUNTER: pool = hunter_names; break;
                case NPC_GUARD_CAPTAIN: pool = guard_names; break;
                case NPC_SCHOLAR: pool = scholar_names; break;
                case NPC_BARD: pool = bard_names; break;
                case NPC_ALCHEMIST: pool = alchemist_names; break;
                case NPC_INNKEEPER: pool = innkeeper_names; break;
                default: pool = survivor_names; break;
            }
            if (pool) {
                strncpy(npc->name, pool[rand()%pool_size], NPC_NAME_LEN-1);
                npc->name[NPC_NAME_LEN-1] = '\0';
            }
            return;
        }
    }
}

void spawn_village_population(int village_id) {
    if (village_id < 0 || village_id >= MAX_VILLAGES) return;
    Village* v = &villages[village_id];
    
    for (int i=0; i<MAX_BUILDINGS_PER_VILLAGE; i++) {
        Building* b = &village_buildings[village_id][i];
        if (!b->active) continue;
        
        NPCClass cls = NPC_SURVIVOR;
        switch(b->type) {
            case BLD_BARRACKS: cls = NPC_SOLDIER; break;
            case BLD_FARM: cls = NPC_FARMER; break;
            case BLD_SMITHY: cls = NPC_WEAPONSMITH; break;
            case BLD_TRADE_POST: case BLD_MARKET: cls = NPC_TRADER; break;
            case BLD_CHURCH: cls = NPC_HEALER; break;
            case BLD_LIBRARY: cls = NPC_LIBRARIAN; break;
            case BLD_FISHING_DOCK: cls = NPC_FISHER; break;
            case BLD_MINE_SHAFT: cls = NPC_MINER; break;
            case BLD_BARN: case BLD_WINDMILL: cls = NPC_FARMER; break;
            case BLD_CEMETERY: cls = NPC_ELDER; break;
            case BLD_TAVERN: cls = NPC_INNKEEPER; break;
            case BLD_STABLE: cls = NPC_HUNTER; break;
            default: cls = NPC_SURVIVOR; break;
        }
        spawn_npc(village_id, cls, b->x+2, b->z+2);
        
        // Special buildings get extra NPCs
        if (b->type==BLD_BARRACKS && rand()%2==0)
            spawn_npc(village_id, NPC_GUARD_CAPTAIN, b->x-2, b->z-2);
        if (b->type==BLD_LIBRARY && rand()%2==0)
            spawn_npc(village_id, NPC_SCHOLAR, b->x, b->z+3);
        if (b->type==BLD_TAVERN && rand()%2==0)
            spawn_npc(village_id, NPC_BARD, b->x+1, b->z+1);
        if (b->type==BLD_MARKET && rand()%2==0)
            spawn_npc(village_id, NPC_ALCHEMIST, b->x-1, b->z-1);
        if (b->type==BLD_HOUSE_LARGE) {
            spawn_npc(village_id, NPC_ELDER, b->x-2, b->z-2);
        }
        if (rand()%3==0)
            spawn_npc(village_id, NPC_CHILD, b->x, b->z+5);
    }
}

// ═══════════════════════════════════════════════════════════
// NPC COMBAT (NEW: NPCs attack zombies)
// ═══════════════════════════════════════════════════════════

void npc_combat_tick(void) {
    time_t now = time(NULL);
    for (int n=0; n<MAX_NPCS; n++) {
        NPC* npc = &npcs[n];
        if (!npc->active || npc->damage <= 0) continue;
        if (now - npc->last_attack < npc->attack_speed) continue;
        
        // Find nearest zombie
        float nearest = npc->attack_range * npc->attack_range;
        int target = -1;
        for (int z=0; z<512; z++) {
            if (!zombies[z].active) continue;
            float dx = zombies[z].x - npc->x;
            float dz = zombies[z].z - npc->z;
            float d2 = dx*dx + dz*dz;
            if (d2 < nearest) {
                nearest = d2;
                target = z;
            }
        }
        
        if (target >= 0) {
            zombies[target].health -= npc->damage;
            npc->last_attack = now;
            
            if (npc->npc_class==NPC_SOLDIER || npc->npc_class==NPC_GUARD_CAPTAIN)
                npc->behavior = 4; // combat mode
            
            if (zombies[target].health <= 0) {
                zombies[target].active = 0;
                npc->kills++;
                if (npc->kills % 5 == 0) {
                    printf("[NPC] %s (%s) has slain %d zombies!\n",
                           npc->name, get_npc_class_name(npc->npc_class), npc->kills);
                }
            }
        } else if (npc->behavior==4) {
            npc->behavior = 1; // back to patrol
        }
    }
}

// ═══════════════════════════════════════════════════════════
// PATHFINDING
// ═══════════════════════════════════════════════════════════

void update_npc_pathfinding(void) {
    for (int i=0; i<MAX_NPCS; i++) {
        NPC* npc = &npcs[i];
        if (!npc->active || npc->following_player<0) continue;
        if (npc->following_player >= num_players) continue;
        if (npc->behavior==4) continue; // Don't follow in combat
        
        float px = players[npc->following_player].x;
        float pz = players[npc->following_player].z;
        float dx = px-npc->x, dz = pz-npc->z;
        float dist_sq = dx*dx + dz*dz;
        
        if (dist_sq > 16.0f) {
            float dist = sqrtf(dist_sq);
            float move = 2.0f * 0.1f;
            if (move > dist) move = dist;
            npc->x += (dx/dist)*move;
            npc->z += (dz/dist)*move;
        }
    }
    
    for (int i=0; i<4; i++) {
        RetroHero* hero = &heroes[i];
        if (!hero->active || hero->following_player<0) continue;
        if (hero->following_player >= num_players) continue;
        
        float px = players[hero->following_player].x;
        float pz = players[hero->following_player].z;
        float dx = px-hero->x, dz = pz-hero->z;
        float dist_sq = dx*dx + dz*dz;
        
        if (dist_sq > 16.0f) {
            float dist = sqrtf(dist_sq);
            float move = 3.0f * 0.1f;
            if (move > dist) move = dist;
            hero->x += (dx/dist)*move;
            hero->z += (dz/dist)*move;
        }
    }
}

void npc_follow_player(int npc_id, int player_id) {
    if (npc_id<0 || npc_id>=MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active) return;
    npc->following_player = player_id;
    npc->behavior = 2;
    printf("[NPC] %s (%s) is now following you!\n", npc->name, get_npc_class_name(npc->npc_class));
}

void npc_interact(int player_id, int npc_id) {
    (void)player_id;
    if (npc_id<0 || npc_id>=MAX_NPCS) return;
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
    
    if (npc->npc_class==NPC_TRADER || npc->npc_class==NPC_FARMER ||
        npc->npc_class==NPC_WEAPONSMITH || npc->npc_class==NPC_HEALER ||
        npc->npc_class==NPC_ALCHEMIST || npc->npc_class==NPC_INNKEEPER) {
        printf("║  [Trade] — Type /trade %d                                  ║\n", npc_id);
    }
    
    printf("║  [Follow] — Type /follow_npc %d to recruit                 ║\n", npc_id);
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    if (npc->dialogue_state < 2) npc->dialogue_state++;
}

// ═══════════════════════════════════════════════════════════
// TRADING SYSTEM
// ═══════════════════════════════════════════════════════════

void init_trading_system(void) {
    memset(trader_inventories, 0, sizeof(trader_inventories));
    printf("[TRADE] Trading system initialized\n");
}

const char* get_item_name(int item_id) {
    switch(item_id) {
        case ITEM_AMMO: return "Ammo Clip";
        case ITEM_HEALTH_PACK: return "Health Pack";
        case ITEM_MATERIALS: return "Materials";
        case ITEM_WEAPON_UPGRADE: return "Weapon Upgrade";
        case ITEM_MAP_FRAGMENT: return "Map Fragment";
        case ITEM_POTION_SPEED: return "Speed Potion";
        case ITEM_POTION_STRENGTH: return "Strength Potion";
        default: return "Unknown";
    }
}

void generate_trader_inventory(int npc_id) {
    if (npc_id<0 || npc_id>=MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active) return;
    
    TraderInventory* inv = &trader_inventories[npc_id];
    if (inv->num_offers > 0) return;
    
    inv->num_offers = 3 + (rand()%2);
    inv->last_restock = time(NULL);
    
    for (int i=0; i<inv->num_offers; i++) {
        TradeOffer* o = &inv->offers[i];
        o->in_stock = 1;
        switch(npc->npc_class) {
            case NPC_FARMER: case NPC_INNKEEPER:
                o->item_id = ITEM_HEALTH_PACK; o->quantity=2+(rand()%3);
                o->cost_materials=50; o->cost_score=100; break;
            case NPC_WEAPONSMITH:
                o->item_id = ITEM_AMMO; o->quantity=30+(rand()%20);
                o->cost_materials=25; o->cost_score=50; break;
            case NPC_ALCHEMIST:
                {int items[]={ITEM_POTION_SPEED,ITEM_POTION_STRENGTH,ITEM_HEALTH_PACK};
                o->item_id=items[rand()%3]; o->quantity=1+(rand()%2);
                o->cost_materials=60; o->cost_score=120;} break;
            case NPC_TRADER:
                {int items[]={ITEM_AMMO,ITEM_HEALTH_PACK,ITEM_MATERIALS,ITEM_MAP_FRAGMENT};
                o->item_id=items[rand()%4];
                o->quantity=(o->item_id==ITEM_MATERIALS)?20:1+(rand()%2);
                o->cost_materials=30+(rand()%40); o->cost_score=75+(rand()%100);} break;
            case NPC_HEALER:
                o->item_id = ITEM_HEALTH_PACK; o->quantity=3+(rand()%3);
                o->cost_materials=40; o->cost_score=80; break;
            default:
                o->item_id = ITEM_MATERIALS; o->quantity=10+(rand()%10);
                o->cost_materials=20; o->cost_score=40;
        }
    }
}

void restock_trader(int npc_id) {
    if (npc_id<0 || npc_id>=MAX_NPCS) return;
    TraderInventory* inv = &trader_inventories[npc_id];
    if (time(NULL)-inv->last_restock < 300) return;
    for (int i=0; i<inv->num_offers; i++) inv->offers[i].in_stock=1;
    inv->last_restock = time(NULL);
}

void show_trade_offers(int player_id, int npc_id) {
    (void)player_id;
    if (npc_id<0 || npc_id>=MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active) return;
    
    generate_trader_inventory(npc_id);
    restock_trader(npc_id);
    TraderInventory* inv = &trader_inventories[npc_id];
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  %-10s's Shop                                       ║\n", npc->name);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    for (int i=0; i<inv->num_offers; i++) {
        TradeOffer* o = &inv->offers[i];
        if (o->in_stock) {
            printf("║  [%d] %-20s x%-2d  |  %d mats / %d pts     ║\n",
                   i, get_item_name(o->item_id), o->quantity, o->cost_materials, o->cost_score);
        } else {
            printf("║  [%d] %-20s — SOLD OUT                                 ║\n", i, get_item_name(o->item_id));
        }
    }
    printf("║                                                            ║\n");
    printf("║  Type /buy %d [slot] to purchase                         ║\n", npc_id);
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

int execute_trade(int player_id, int npc_id, int offer_slot) {
    if (player_id<0 || player_id>=8) return 0;
    if (npc_id<0 || npc_id>=MAX_NPCS) return 0;
    if (offer_slot<0 || offer_slot>=TRADE_SLOTS) return 0;
    
    TraderInventory* inv = &trader_inventories[npc_id];
    if (offer_slot >= inv->num_offers) return 0;
    
    TradeOffer* o = &inv->offers[offer_slot];
    if (!o->in_stock) {
        printf("[TRADE] Sold out!\n");
        return 0;
    }
    
    if (players[player_id].materials >= o->cost_materials) {
        players[player_id].materials -= o->cost_materials;
    } else if (players[player_id].score >= o->cost_score) {
        players[player_id].score -= o->cost_score;
    } else {
        printf("[TRADE] Can't afford! Need %d mats or %d score.\n", o->cost_materials, o->cost_score);
        return 0;
    }
    
    o->in_stock = 0;
    switch(o->item_id) {
        case ITEM_AMMO:
            players[player_id].ammo += o->quantity;
            printf("[TRADE] +%d ammo! (Total: %d)\n", o->quantity, players[player_id].ammo); break;
        case ITEM_HEALTH_PACK:
            players[player_id].health += 50*o->quantity;
            if (players[player_id].health > players[player_id].max_health)
                players[player_id].health = players[player_id].max_health;
            printf("[TRADE] Healed! HP: %d\n", players[player_id].health); break;
        case ITEM_MATERIALS:
            players[player_id].materials += o->quantity;
            printf("[TRADE] +%d materials!\n", o->quantity); break;
        case ITEM_POTION_SPEED:
            players[player_id].speed_boost = 1.5f;
            players[player_id].powerup_end = time(NULL) + 30;
            printf("[TRADE] Speed boosted for 30 seconds!\n"); break;
        case ITEM_POTION_STRENGTH:
            players[player_id].damage_boost = 2.0f;
            players[player_id].powerup_end = time(NULL) + 30;
            printf("[TRADE] Damage doubled for 30 seconds!\n"); break;
        default:
            printf("[TRADE] Received %s x%d!\n", get_item_name(o->item_id), o->quantity);
    }
    return 1;
}

// ═══════════════════════════════════════════════════════════
// QUEST SYSTEM
// ═══════════════════════════════════════════════════════════

void init_quest_system(void) {
    memset(quests, 0, sizeof(quests));
    printf("[QUESTS] Quest system initialized\n");
}

const char* get_quest_type_name(QuestType qtype) {
    switch(qtype) {
        case QUEST_KILL_ZOMBIES: return "Extermination";
        case QUEST_FETCH_ITEM: return "Delivery";
        case QUEST_REACH_LOCATION: return "Pilgrimage";
        case QUEST_DEFEND_VILLAGE: return "Defense";
        case QUEST_ESCORT_NPC: return "Escort";
        case QUEST_RETRIEVE_RELIC: return "Retrieval";
        case QUEST_CRAFT_ITEM: return "Crafting";
        case QUEST_RESCUE_SURVIVOR: return "Rescue";
        default: return "Unknown";
    }
}

void generate_quest_for_npc(int npc_id) {
    if (npc_id<0 || npc_id>=MAX_NPCS) return;
    NPC* npc = &npcs[npc_id];
    if (!npc->active || !npc->has_quest || npc->quest_completed) return;
    
    // Find empty quest slot
    for (int i=0; i<MAX_QUESTS; i++) {
        if (!quests[i].active) {
            Quest* q = &quests[i];
            q->active = 1;
            q->id = i;
            q->giver_npc_id = npc_id;
            q->status = QUESTSTATUS_INACTIVE;
            q->current_count = 0;
            
            // Generate quest based on NPC class
            QuestType types[] = {
                QUEST_KILL_ZOMBIES, QUEST_FETCH_ITEM, QUEST_REACH_LOCATION,
                QUEST_DEFEND_VILLAGE, QUEST_RESCUE_SURVIVOR, QUEST_RETRIEVE_RELIC
            };
            q->qtype = types[rand()%6];
            
            switch(q->qtype) {
                case QUEST_KILL_ZOMBIES:
                    q->target_count = 10 + (rand()%20);
                    snprintf(q->title, QUEST_TITLE_LEN, "Clear the Dead");
                    snprintf(q->description, QUEST_DESC_LEN,
                        "The area around our village is crawling with zombies.\n"
                        "Kill %d of them and we'll reward you.", q->target_count);
                    q->reward_materials = 50 + (rand()%50);
                    q->reward_score = 200 + (rand()%200);
                    break;
                case QUEST_DEFEND_VILLAGE:
                    q->target_count = 1;
                    snprintf(q->title, QUEST_TITLE_LEN, "Defend %s", villages[npc->village_id].name);
                    snprintf(q->description, QUEST_DESC_LEN,
                        "Our scouts report a horde approaching!\n"
                        "Survive the attack and protect our people.");
                    q->reward_materials = 100;
                    q->reward_score = 500;
                    break;
                case QUEST_REACH_LOCATION:
                    q->target_x = (rand()%1000) - 500;
                    q->target_z = (rand()%1000) - 500;
                    snprintf(q->title, QUEST_TITLE_LEN, "Journey to the Unknown");
                    snprintf(q->description, QUEST_DESC_LEN,
                        "We need someone to scout coordinates (%d, %d).\n"
                        "Be careful out there.", q->target_x, q->target_z);
                    q->reward_materials = 30;
                    q->reward_score = 150;
                    break;
                case QUEST_RESCUE_SURVIVOR:
                    snprintf(q->title, QUEST_TITLE_LEN, "Rescue Operation");
                    snprintf(q->description, QUEST_DESC_LEN,
                        "One of our people is trapped in the ruins.\n"
                        "Find them and bring them home.");
                    q->reward_materials = 75;
                    q->reward_score = 300;
                    break;
                default:
                    snprintf(q->title, QUEST_TITLE_LEN, "A Favor");
                    snprintf(q->description, QUEST_DESC_LEN, "Help us with a small task.");
                    q->reward_materials = 25;
                    q->reward_score = 100;
            }
            return;
        }
    }
}

void activate_quest(int quest_id) {
    if (quest_id<0 || quest_id>=MAX_QUESTS) return;
    Quest* q = &quests[quest_id];
    if (!q->active || q->status!=QUESTSTATUS_INACTIVE) return;
    q->status = QUESTSTATUS_ACTIVE;
    q->start_time = time(NULL);
    printf("\n[QUEST STARTED] %s\n%s\n\n", q->title, q->description);
}

void update_quest_progress(int quest_id, int amount) {
    if (quest_id<0 || quest_id>=MAX_QUESTS) return;
    Quest* q = &quests[quest_id];
    if (!q->active || q->status!=QUESTSTATUS_ACTIVE) return;
    q->current_count += amount;
    if (q->current_count >= q->target_count) {
        complete_quest(quest_id, 0);
    }
}

void complete_quest(int quest_id, int player_id) {
    if (quest_id<0 || quest_id>=MAX_QUESTS) return;
    Quest* q = &quests[quest_id];
    if (!q->active) return;
    q->status = QUESTSTATUS_COMPLETED;
    q->current_count = q->target_count;
    
    if (player_id >= 0 && player_id < 8) {
        players[player_id].materials += q->reward_materials;
        players[player_id].score += q->reward_score;
    }
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  QUEST COMPLETED!                                         ║\n");
    printf("║  %s\n", q->title);
    printf("║  Reward: %d materials, %d score                            ║\n",
           q->reward_materials, q->reward_score);
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Mark NPC quest as done
    if (q->giver_npc_id >= 0 && q->giver_npc_id < MAX_NPCS) {
        npcs[q->giver_npc_id].quest_completed = 1;
    }
}

void list_active_quests(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  ACTIVE QUESTS                                            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    int found=0;
    for (int i=0; i<MAX_QUESTS; i++) {
        if (quests[i].active && quests[i].status==QUESTSTATUS_ACTIVE) {
            printf("║  [%d] %s (%s) %d/%d\n", i, quests[i].title,
                   get_quest_type_name(quests[i].qtype), quests[i].current_count, quests[i].target_count);
            found++;
        }
    }
    if (!found) printf("║  No active quests.                                        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// WORLD EVENTS
// ═══════════════════════════════════════════════════════════

void init_world_events(void) {
    memset(world_events, 0, sizeof(world_events));
    printf("[EVENTS] World event system initialized\n");
}

const char* get_event_name(EventType etype) {
    switch(etype) {
        case EVENT_HORDE_ATTACK: return "Horde Attack";
        case EVENT_WANDERING_MERCHANT: return "Wandering Merchant";
        case EVENT_ZOMBIE_SWARM: return "Zombie Swarm";
        case EVENT_SUPPLY_DROP: return "Supply Drop";
        case EVENT_DARK_RITUAL: return "Dark Ritual";
        case EVENT_SURVIVOR_RESCUE: return "Survivor Rescue";
        case EVENT_PLAGUE_OUTBREAK: return "Plague Outbreak";
        case EVENT_HEROIC_VISITOR: return "Heroic Visitor";
        default: return "Unknown Event";
    }
}

void trigger_event(EventType etype, int village_id, int player_id) {
    for (int i=0; i<MAX_ACTIVE_EVENTS; i++) {
        if (!world_events[i].active) {
            WorldEvent* e = &world_events[i];
            e->active = 1;
            e->etype = etype;
            e->target_village_id = village_id;
            e->target_player_id = player_id;
            e->start_time = time(NULL);
            e->completed = 0;
            e->failed = 0;
            e->intensity = 1 + (rand()%3);
            
            switch(etype) {
                case EVENT_HORDE_ATTACK:
                    e->duration = 120; // 2 minutes
                    snprintf(e->announcement, sizeof(e->announcement),
                        "HORDE ATTACK on %s! Defend the village!", villages[village_id].name);
                    villages[village_id].under_attack = 1;
                    villages[village_id].attack_start = time(NULL);
                    break;
                case EVENT_WANDERING_MERCHANT:
                    e->duration = 300; // 5 minutes
                    snprintf(e->announcement, sizeof(e->announcement),
                        "A wandering merchant has appeared near %s!", village_id>=0?villages[village_id].name:"the wilderness");
                    break;
                case EVENT_ZOMBIE_SWARM:
                    e->duration = 60;
                    snprintf(e->announcement, sizeof(e->announcement),
                        "MASSIVE ZOMBIE SWARM detected! Take cover!");
                    break;
                case EVENT_SUPPLY_DROP:
                    e->duration = 180;
                    snprintf(e->announcement, sizeof(e->announcement),
                        "Supply drop incoming! Look for the signal smoke.");
                    break;
                case EVENT_DARK_RITUAL:
                    e->duration = 90;
                    snprintf(e->announcement, sizeof(e->announcement),
                        "Dark energy gathers... The Witch grows stronger!");
                    break;
                case EVENT_SURVIVOR_RESCUE:
                    e->duration = 240;
                    snprintf(e->announcement, sizeof(e->announcement),
                        "A survivor is trapped in the ruins! Someone help them!");
                    break;
                default:
                    e->duration = 60;
                    snprintf(e->announcement, sizeof(e->announcement), "Something is happening...");
            }
            
            printf("\n╔════════════════════════════════════════════════════════════╗\n");
            printf("║  WORLD EVENT: %-36s           ║\n", get_event_name(etype));
            printf("╠════════════════════════════════════════════════════════════╣\n");
            printf("║  %s\n", e->announcement);
            printf("║  Intensity: %d  |  Duration: %ld seconds                     ║\n", e->intensity, e->duration);
            printf("╚════════════════════════════════════════════════════════════╝\n\n");
            return;
        }
    }
}

void attempt_random_event(void) {
    if ((rand()%100) > 5) return; // 5% chance per check
    
    EventType types[] = {
        EVENT_HORDE_ATTACK, EVENT_WANDERING_MERCHANT, EVENT_ZOMBIE_SWARM,
        EVENT_SUPPLY_DROP, EVENT_DARK_RITUAL, EVENT_SURVIVOR_RESCUE
    };
    EventType chosen = types[rand()%6];
    
    int village_id = -1;
    if (chosen==EVENT_HORDE_ATTACK || chosen==EVENT_WANDERING_MERCHANT) {
        // Pick a discovered village
        int discovered[MAX_VILLAGES];
        int num_disc = 0;
        for (int i=0; i<MAX_VILLAGES; i++)
            if (villages[i].discovered) discovered[num_disc++] = i;
        if (num_disc > 0) village_id = discovered[rand()%num_disc];
    }
    
    trigger_event(chosen, village_id, -1);
}

void update_world_events(void) {
    time_t now = time(NULL);
    for (int i=0; i<MAX_ACTIVE_EVENTS; i++) {
        WorldEvent* e = &world_events[i];
        if (!e->active) continue;
        
        // Check expiration
        if (now - e->start_time > e->duration) {
            if (!e->completed && !e->failed) {
                // Event expired unresolved
                if (e->etype == EVENT_HORDE_ATTACK && e->target_village_id >= 0) {
                    printf("[EVENT] %s was overrun! The village is in ruins.\n",
                           villages[e->target_village_id].name);
                    villages[e->target_village_id].under_attack = 0;
                    villages[e->target_village_id].danger_level = 3;
                }
            }
            e->active = 0;
            continue;
        }
        
        // Process active events
        switch(e->etype) {
            case EVENT_HORDE_ATTACK:
                if (e->target_village_id >= 0 && !villages[e->target_village_id].under_attack)
                    e->completed = 1;
                break;
            default:
                break;
        }
    }
}

void list_active_events(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  ACTIVE WORLD EVENTS                                      ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    int found=0;
    time_t now = time(NULL);
    for (int i=0; i<MAX_ACTIVE_EVENTS; i++) {
        if (!world_events[i].active) continue;
        long remaining = world_events[i].duration - (now - world_events[i].start_time);
        if (remaining < 0) remaining = 0;
        printf("║  %s — %lds remaining\n", get_event_name(world_events[i].etype), remaining);
        found++;
    }
    if (!found) printf("║  No active events. The world is quiet... for now.        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
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
        printf("[WITCH] The Witch already prowls!\n");
        return;
    }
    witch_boss.active=1; witch_boss.x=x; witch_boss.y=64; witch_boss.z=z;
    witch_boss.max_health=3000; witch_boss.health=3000;
    witch_boss.phase=WITCH_PHASE_1; witch_boss.last_spell=time(NULL);
    witch_boss.last_summon=time(NULL); witch_boss.summons_total=0;
    witch_boss.defeated=0; witch_boss.achievement_unlocked=0;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║     THE WITCH HAS APPEARED!                              ║\n");
    printf("║  Phase 1: The Summoning  |  Health: 3,000                ║\n");
    printf("║  \"The darkness answers my call...\"                       ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

void update_witch_boss(void) {
    if (!witch_boss.active || witch_boss.defeated) return;
    float hp = witch_boss.health / witch_boss.max_health;
    time_t now = time(NULL);
    if (witch_boss.phase==WITCH_PHASE_1 && hp<=0.60f) {
        witch_boss.phase=WITCH_PHASE_2;
        printf("[WITCH] Phase 2: Shadow swarm!\n");
    } else if (witch_boss.phase==WITCH_PHASE_2 && hp<=0.30f) {
        witch_boss.phase=WITCH_PHASE_3;
        printf("[WITCH] Phase 3: DESPERATION! Poison cloud!\n");
    }
    if (now-witch_boss.last_spell >= 8) { witch_cast_spell(); witch_boss.last_spell=now; }
    if (now-witch_boss.last_summon >= 12) { witch_summon_zombies(); witch_boss.last_summon=now; }
    if (witch_boss.phase==WITCH_PHASE_3 && now%5==0) witch_heal();
    if (witch_boss.health<=0) {
        witch_boss.defeated=1; witch_boss.active=0;
        printf("\n╔════════════════════════════════════════════════════════════╗\n");
        printf("║     THE WITCH IS DEFEATED!                               ║\n");
        printf("║  ACHIEVEMENT: Witch Hunter                                ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n\n");
    }
}

void witch_cast_spell(void) {
    switch(witch_boss.phase) {
        case WITCH_PHASE_1: printf("[WITCH] Throws poison vial!\n"); break;
        case WITCH_PHASE_2: printf("[WITCH] Commands shadows!\n"); break;
        case WITCH_PHASE_3: witch_poison_cloud(); break;
    }
}
void witch_summon_zombies(void) { printf("[WITCH] Raises the dead!\n"); witch_boss.summons_total+=3; }
void witch_poison_cloud(void) { printf("[WITCH] Poison cloud spreads!\n"); }
void witch_heal(void) {
    witch_boss.health+=50;
    if (witch_boss.health>witch_boss.max_health) witch_boss.health=witch_boss.max_health;
}
int is_witch_alive(void) { return witch_boss.active && !witch_boss.defeated; }
void grant_witch_achievement(int player_id) {
    (void)player_id;
    if (witch_boss.achievement_unlocked) return;
    witch_boss.achievement_unlocked=1;
    printf("\n ACHIEVEMENT: Witch Hunter!\n");
}

// ═══════════════════════════════════════════════════════════
// RETRO HEROES
// ═══════════════════════════════════════════════════════════

void init_hero_system(void) {
    memset(heroes, 0, sizeof(heroes));
    printf("[HEROES] Retro hero system initialized\n");
}

const char* get_hero_catchphrase(NPCClass hero_type) {
    switch(hero_type) {
        case NPC_HERO_MEGAMAN: return "I fight for those who cannot.";
        case NPC_HERO_LINK: return "HYAH! Let's go!";
        case NPC_HERO_SAMUS: return "Mission accepted. Target: undead.";
        case NPC_HERO_MARIO: return "It's-a-me! Let's-a-go!";
        default: return "...";
    }
}
const char* get_hero_description(NPCClass hero_type) {
    switch(hero_type) {
        case NPC_HERO_MEGAMAN: return "Blue android. Arm cannon.";
        case NPC_HERO_LINK: return "Sword and shield warrior.";
        case NPC_HERO_SAMUS: return "Power armor. Missiles.";
        case NPC_HERO_MARIO: return "Plumber. Fireballs. Jumps.";
        default: return "Mysterious wanderer.";
    }
}

void attempt_hero_spawn(void) {
    if ((float)rand()/RAND_MAX > HERO_SPAWN_CHANCE) return;
    NPCClass types[] = {NPC_HERO_MEGAMAN, NPC_HERO_LINK, NPC_HERO_SAMUS, NPC_HERO_MARIO};
    NPCClass chosen = types[rand()%4];
    for (int i=0; i<4; i++) if (!heroes[i].active) { spawn_hero(chosen, 0, 0); return; }
}

void spawn_hero(NPCClass hero_type, float x, float z) {
    for (int i=0; i<4; i++) {
        if (!heroes[i].active) {
            RetroHero* h = &heroes[i];
            h->active=1; h->hero_type=hero_type; h->x=x; h->y=64; h->z=z;
            h->health=200; h->following_player=-1; h->kills=0;
            h->damage=50; h->attack_range=8.0f; h->last_attack=time(NULL);
            switch(hero_type) {
                case NPC_HERO_MEGAMAN: strcpy(h->name,HERO_MEGAMAN_NAME); h->damage=75; break;
                case NPC_HERO_LINK: strcpy(h->name,HERO_LINK_NAME); h->attack_range=3.0f; h->damage=100; break;
                case NPC_HERO_SAMUS: strcpy(h->name,HERO_SAMUS_NAME); h->damage=90; break;
                case NPC_HERO_MARIO: strcpy(h->name,HERO_MARIO_NAME); h->damage=60; break;
                default: break;
            }
            strcpy(h->catchphrase, get_hero_catchphrase(hero_type));
            printf("\n╔════════════════════════════════════════════════════════════╗\n");
            printf("║  RARE SPAWN: %s                                  ║\n", h->name);
            printf("║  \"%s\"\n", h->catchphrase);
            printf("║  %s\n", get_hero_description(hero_type));
            printf("║  Type /follow to recruit.                               ║\n");
            printf("╚════════════════════════════════════════════════════════════╝\n\n");
            return;
        }
    }
}

void hero_attack_zombies(void) {
    time_t now = time(NULL);
    for (int h=0; h<4; h++) {
        RetroHero* hero = &heroes[h];
        if (!hero->active || hero->following_player<0) continue;
        if (now-hero->last_attack < 1) continue;
        
        float nearest = hero->attack_range*hero->attack_range;
        int target=-1;
        for (int z=0; z<512; z++) {
            if (!zombies[z].active) continue;
            float dx=zombies[z].x-hero->x, dz=zombies[z].z-hero->z;
            float d2=dx*dx+dz*dz;
            if (d2<nearest) { nearest=d2; target=z; }
        }
        
        if (target>=0) {
            zombies[target].health -= hero->damage;
            hero->last_attack = now;
            if (zombies[target].health<=0) {
                zombies[target].active=0;
                hero->kills++;
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
    if (hero_id<0 || hero_id>=4) return;
    RetroHero* h = &heroes[hero_id];
    if (!h->active) return;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  %s\n", h->name);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  \"%s\"\n", h->catchphrase);
    printf("║  DMG:%.0f | RNG:%.1f | Kills:%d\n", h->damage, h->attack_range, h->kills);
    if (h->following_player==-1) printf("║  [Type /follow to recruit]                                ║\n");
    else if (h->following_player==player_id) printf("║  Following you.                                           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// MASTER INIT + TICK
// ═══════════════════════════════════════════════════════════

void init_village_npc_systems(void) {
    init_village_system();
    init_npc_system();
    init_trading_system();
    init_quest_system();
    init_world_events();
    init_witch_boss();
    init_hero_system();
    
    generate_villages();
    for (int i=0; i<MAX_VILLAGES; i++) spawn_village_population(i);
    
    printf("\n[VILLAGES/NPCS] All systems initialized!\n");
    printf("  Villages: %d (8 types)\n", MAX_VILLAGES);
    printf("  NPCs: Up to %d (15 classes)\n", MAX_NPCS);
    printf("  Quests: %d max concurrent\n", MAX_QUESTS);
    printf("  Events: %d can be active\n", MAX_ACTIVE_EVENTS);
    printf("  Retro heroes: Enabled\n\n");
}

void tick_all_systems(void) {
    npc_combat_tick();
    update_heroes();
    update_witch_boss();
    update_world_events();
    // Random event check every ~30 seconds would happen in main loop
}
