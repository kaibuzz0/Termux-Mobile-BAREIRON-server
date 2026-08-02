/*
 * BAREIRON MULTIPLAYER POLISH
 * Team systems, trading, and cooperative features
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "multiplayer.h"
#include "globals.h"

// External player data
extern struct Player_s { int id; char name[32]; float x,y,z; int weapon; int ammo; int max_ammo; int health; int max_health; int score; int kills; float speed_boost; float damage_boost; int invincible; time_t powerup_end; int materials; int barricades_built; } players[8];
extern int num_players;

Team teams[MAX_TEAMS];
TradeRequest trade_requests[MAX_TRADE_REQUESTS];

// Team names
static const char* team_name_prefixes[] = {"Iron","Steel","Shadow","Blood","Dawn","Dusk","Storm","Flame"};
static const char* team_name_suffixes[] = {"Legion","Company","Crew","Gang","Squad","Brothers","Pack","Wolves"};

// ═══════════════════════════════════════════════════════════
// TEAM SYSTEM
// ═══════════════════════════════════════════════════════════

void init_team_system(void) {
    memset(teams, 0, sizeof(teams));
    for (int i=0; i<MAX_TEAMS; i++) {
        teams[i].id = i;
        for (int j=0; j<4; j++) teams[i].members[j] = -1;
    }
    printf("[TEAMS] Team system initialized (up to %d teams, %d members each)\n", MAX_TEAMS, MAX_TEAM_MEMBERS);
}

int create_team(int leader_id, const char* name) {
    if (leader_id < 0 || leader_id >= 8) return -1;
    
    // Check if player is already in a team
    for (int i=0; i<MAX_TEAMS; i++) {
        for (int j=0; j<4; j++) {
            if (teams[i].members[j] == leader_id) {
                printf("[TEAM] You're already in a team! Leave first.\n");
                return -1;
            }
        }
    }
    
    // Find empty team slot
    for (int i=0; i<MAX_TEAMS; i++) {
        if (!teams[i].active) {
            teams[i].active = 1;
            teams[i].members[0] = leader_id;
            teams[i].num_members = 1;
            teams[i].shared_materials = 0;
            teams[i].shared_score = 0;
            teams[i].base_x = (int)players[leader_id].x;
            teams[i].base_z = (int)players[leader_id].z;
            teams[i].level = 1;
            
            if (name && name[0]) {
                strncpy(teams[i].name, name, TEAM_NAME_LEN-1);
            } else {
                snprintf(teams[i].name, TEAM_NAME_LEN, "%s %s",
                         team_name_prefixes[rand()%8], team_name_suffixes[rand()%8]);
            }
            teams[i].name[TEAM_NAME_LEN-1] = '\0';
            
            printf("\n╔════════════════════════════════════════════════════════════╗\n");
            printf("║  TEAM CREATED!                                            ║\n");
            printf("║  %s (ID: %d)                                               ║\n", teams[i].name, i);
            printf("║  Leader: %s                                               ║\n", players[leader_id].name);
            printf("║  Base: (%d, %d)                                           ║\n", teams[i].base_x, teams[i].base_z);
            printf("╚════════════════════════════════════════════════════════════╝\n\n");
            return i;
        }
    }
    
    printf("[TEAM] All team slots are full!\n");
    return -1;
}

int join_team(int player_id, int team_id) {
    if (player_id < 0 || player_id >= 8) return 0;
    if (team_id < 0 || team_id >= MAX_TEAMS) return 0;
    if (!teams[team_id].active) {
        printf("[TEAM] That team doesn't exist!\n");
        return 0;
    }
    if (teams[team_id].num_members >= MAX_TEAM_MEMBERS) {
        printf("[TEAM] That team is full!\n");
        return 0;
    }
    
    // Leave current team first
    leave_team(player_id);
    
    // Join new team
    for (int i=0; i<MAX_TEAM_MEMBERS; i++) {
        if (teams[team_id].members[i] == -1) {
            teams[team_id].members[i] = player_id;
            teams[team_id].num_members++;
            printf("[TEAM] %s joined %s!\n", players[player_id].name, teams[team_id].name);
            return 1;
        }
    }
    return 0;
}

void leave_team(int player_id) {
    if (player_id < 0 || player_id >= 8) return;
    
    for (int i=0; i<MAX_TEAMS; i++) {
        for (int j=0; j<MAX_TEAM_MEMBERS; j++) {
            if (teams[i].members[j] == player_id) {
                teams[i].members[j] = -1;
                teams[i].num_members--;
                printf("[TEAM] %s left %s.\n", players[player_id].name, teams[i].name);
                
                // If team is empty, disband
                if (teams[i].num_members <= 0) {
                    printf("[TEAM] %s has been disbanded.\n", teams[i].name);
                    teams[i].active = 0;
                }
                return;
            }
        }
    }
}

void deposit_team_materials(int player_id, int amount) {
    if (player_id < 0 || player_id >= 8) return;
    if (amount <= 0) return;
    if (players[player_id].materials < amount) amount = players[player_id].materials;
    
    for (int i=0; i<MAX_TEAMS; i++) {
        for (int j=0; j<MAX_TEAM_MEMBERS; j++) {
            if (teams[i].members[j] == player_id) {
                players[player_id].materials -= amount;
                teams[i].shared_materials += amount;
                printf("[TEAM] %s deposited %d materials into %s's pool. (Total: %d)\n",
                       players[player_id].name, amount, teams[i].name, teams[i].shared_materials);
                return;
            }
        }
    }
    printf("[TEAM] You're not in a team!\n");
}

void withdraw_team_materials(int player_id, int amount) {
    if (player_id < 0 || player_id >= 8) return;
    if (amount <= 0) return;
    
    for (int i=0; i<MAX_TEAMS; i++) {
        for (int j=0; j<MAX_TEAM_MEMBERS; j++) {
            if (teams[i].members[j] == player_id) {
                if (teams[i].shared_materials < amount) amount = teams[i].shared_materials;
                teams[i].shared_materials -= amount;
                players[player_id].materials += amount;
                printf("[TEAM] %s withdrew %d materials from %s's pool. (Remaining: %d)\n",
                       players[player_id].name, amount, teams[i].name, teams[i].shared_materials);
                return;
            }
        }
    }
    printf("[TEAM] You're not in a team!\n");
}

void share_materials(int from_player, int to_player, int amount) {
    if (from_player < 0 || from_player >= 8 || to_player < 0 || to_player >= 8) return;
    if (amount <= 0) return;
    if (players[from_player].materials < amount) {
        printf("[TRADE] You don't have enough materials!\n");
        return;
    }
    
    players[from_player].materials -= amount;
    players[to_player].materials += amount;
    printf("[TRADE] %s gave %d materials to %s.\n",
           players[from_player].name, amount, players[to_player].name);
}

void share_ammo(int from, int to, int amount) {
    if (from < 0 || from >= 8 || to < 0 || to >= 8) return;
    if (amount <= 0) return;
    if (players[from].ammo < amount) {
        printf("[TRADE] You don't have enough ammo!\n");
        return;
    }
    players[from].ammo -= amount;
    players[to].ammo += amount;
    printf("[TRADE] %s gave %d ammo to %s.\n",
           players[from].name, amount, players[to].name);
}

void share_health(int from, int to, int amount) {
    if (from < 0 || from >= 8 || to < 0 || to >= 8) return;
    if (amount <= 0) return;
    if (players[from].health <= 50) {
        printf("[TRADE] You're too wounded to donate health!\n");
        return;
    }
    int give = amount;
    if (give > players[from].health - 50) give = players[from].health - 50;
    players[from].health -= give;
    players[to].health += give;
    if (players[to].health > players[to].max_health) players[to].health = players[to].max_health;
    printf("[TRADE] %s gave %d HP to %s.\n", players[from].name, give, players[to].name);
}

void revive_player(int reviver_id, int downed_id) {
    if (reviver_id < 0 || reviver_id >= 8 || downed_id < 0 || downed_id >= 8) return;
    if (players[downed_id].health > 0) {
        printf("[REVIVE] %s isn't downed!\n", players[downed_id].name);
        return;
    }
    
    // Check proximity (within 5 blocks)
    float dx = players[reviver_id].x - players[downed_id].x;
    float dz = players[reviver_id].z - players[downed_id].z;
    if (dx*dx + dz*dz > 25.0f) {
        printf("[REVIVE] You're too far from %s!\n", players[downed_id].name);
        return;
    }
    
    players[downed_id].health = players[downed_id].max_health / 4; // 25% HP on revive
    printf("[REVIVE] %s revived %s! (%d HP)\n",
           players[reviver_id].name, players[downed_id].name, players[downed_id].health);
}

void list_teams(void) {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  ACTIVE TEAMS                                             ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    int found = 0;
    for (int i=0; i<MAX_TEAMS; i++) {
        if (teams[i].active) {
            printf("║  [%d] %s (Lvl %d)\n", i, teams[i].name, teams[i].level);
            printf("║      Members: ");
            for (int j=0; j<MAX_TEAM_MEMBERS; j++) {
                if (teams[i].members[j] >= 0)
                    printf("%s ", players[teams[i].members[j]].name);
            }
            printf("\n║      Pool: %d mats | %d score\n",
                   teams[i].shared_materials, teams[i].shared_score);
            found++;
        }
    }
    if (!found) printf("║  No active teams.                                         ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

void show_team_info(int team_id) {
    if (team_id < 0 || team_id >= MAX_TEAMS || !teams[team_id].active) {
        printf("[TEAM] That team doesn't exist!\n");
        return;
    }
    Team* t = &teams[team_id];
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  %s (Level %d)                                           ║\n", t->name, t->level);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Members:\n");
    for (int i=0; i<MAX_TEAM_MEMBERS; i++) {
        if (t->members[i] >= 0) {
            printf("║    %s — Score: %d | Kills: %d\n",
                   players[t->members[i]].name,
                   players[t->members[i]].score,
                   players[t->members[i]].kills);
        }
    }
    printf("║                                                            ║\n");
    printf("║  Shared Pool: %d materials\n", t->shared_materials);
    printf("║  Team Score: %d\n", t->shared_score);
    printf("║  Base: (%d, %d)\n", t->base_x, t->base_z);
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

void call_for_help(int player_id) {
    if (player_id < 0 || player_id >= 8) return;
    printf("\n🆘 %s IS CALLING FOR HELP at (%.0f, %.0f)!\n",
           players[player_id].name, players[player_id].x, players[player_id].z);
    printf("     All nearby players rush to assist!\n\n");
}

// ═══════════════════════════════════════════════════════════
// PLAYER TRADING (REQUEST SYSTEM)
// ═══════════════════════════════════════════════════════════

void init_player_trading(void) {
    memset(trade_requests, 0, sizeof(trade_requests));
    printf("[TRADE] Player trading initialized\n");
}

void send_trade_request(int from, int to, int item, int qty, int want, int want_qty) {
    if (from < 0 || from >= 8 || to < 0 || to >= 8) return;
    if (from == to) {
        printf("[TRADE] Can't trade with yourself!\n");
        return;
    }
    
    for (int i=0; i<MAX_TRADE_REQUESTS; i++) {
        if (!trade_requests[i].active) {
            trade_requests[i].active = 1;
            trade_requests[i].from_player = from;
            trade_requests[i].to_player = to;
            trade_requests[i].item_type = item;
            trade_requests[i].quantity = qty;
            trade_requests[i].request_type = want;
            trade_requests[i].request_qty = want_qty;
            trade_requests[i].expiry = time(NULL) + 60; // 60 second expiry
            
            const char* item_names[] = {"","Ammo","Health","Materials"};
            printf("\n[TRADE] %s offers %d %s for %d %s.\n",
                   players[from].name, qty, item_names[item],
                   want_qty, item_names[want]);
            printf("        %s, type /accept %d to accept.\n\n", players[to].name, i);
            return;
        }
    }
    printf("[TRADE] Too many pending requests!\n");
}

void accept_trade(int request_id) {
    if (request_id < 0 || request_id >= MAX_TRADE_REQUESTS) return;
    TradeRequest* req = &trade_requests[request_id];
    if (!req->active) {
        printf("[TRADE] That request has expired!\n");
        return;
    }
    if (time(NULL) > req->expiry) {
        req->active = 0;
        printf("[TRADE] That request has expired!\n");
        return;
    }
    
    // Verify both players can afford
    int from = req->from_player;
    int to = req->to_player;
    
    // Check what 'from' is offering
    int offer_ok = 0;
    switch(req->item_type) {
        case 1: offer_ok = (players[from].ammo >= req->quantity); break;
        case 3: offer_ok = (players[from].materials >= req->quantity); break;
    }
    
    // Check what 'to' is paying
    int pay_ok = 0;
    switch(req->request_type) {
        case 1: pay_ok = (players[to].ammo >= req->request_qty); break;
        case 3: pay_ok = (players[to].materials >= req->request_qty); break;
    }
    
    if (!offer_ok || !pay_ok) {
        printf("[TRADE] One of you can't afford the trade!\n");
        return;
    }
    
    // Execute trade
    switch(req->item_type) {
        case 1: players[from].ammo -= req->quantity; players[to].ammo += req->quantity; break;
        case 3: players[from].materials -= req->quantity; players[to].materials += req->quantity; break;
    }
    switch(req->request_type) {
        case 1: players[to].ammo -= req->request_qty; players[from].ammo += req->request_qty; break;
        case 3: players[to].materials -= req->request_qty; players[from].materials += req->request_qty; break;
    }
    
    req->active = 0;
    printf("[TRADE] Trade completed! %s and %s exchanged goods.\n",
           players[from].name, players[to].name);
}

void list_trade_requests(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  PENDING TRADE REQUESTS                                   ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    int found = 0;
    time_t now = time(NULL);
    for (int i=0; i<MAX_TRADE_REQUESTS; i++) {
        if (trade_requests[i].active && trade_requests[i].to_player == player_id) {
            long remaining = trade_requests[i].expiry - now;
            if (remaining > 0) {
                printf("║  [%d] From %s — %ds remaining\n",
                       i, players[trade_requests[i].from_player].name, remaining);
                found++;
            } else {
                trade_requests[i].active = 0;
            }
        }
    }
    if (!found) printf("║  No pending requests.                                       ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

// ═══════════════════════════════════════════════════════════
// CHAT
// ═══════════════════════════════════════════════════════════

void team_chat(int player_id, const char* message) {
    if (player_id < 0 || player_id >= 8) return;
    
    // Find player's team
    for (int i=0; i<MAX_TEAMS; i++) {
        for (int j=0; j<MAX_TEAM_MEMBERS; j++) {
            if (teams[i].members[j] == player_id) {
                printf("[TEAM %s] %s: %s\n", teams[i].name, players[player_id].name, message);
                return;
            }
        }
    }
    printf("[TEAM] You're not in a team!\n");
}

void global_chat(int player_id, const char* message) {
    if (player_id < 0 || player_id >= 8) return;
    printf("[GLOBAL] %s: %s\n", players[player_id].name, message);
}

void whisper(int from_player, int to_player, const char* message) {
    if (from_player < 0 || from_player >= 8 || to_player < 0 || to_player >= 8) return;
    printf("[WHISPER %s → %s] %s\n",
           players[from_player].name, players[to_player].name, message);
}
