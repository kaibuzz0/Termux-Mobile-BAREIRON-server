/*
 * BAREIRON MULTIPLAYER POLISH
 * Team systems, shared resources, and player interactions
 */

#ifndef H_MULTIPLAYER
#define H_MULTIPLAYER

#include <stdint.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════
// TEAM SYSTEM
// ═══════════════════════════════════════════════════════════

#define MAX_TEAMS 4
#define MAX_TEAM_MEMBERS 4
#define TEAM_NAME_LEN 32

typedef struct Team_s {
    int active;
    int id;
    char name[TEAM_NAME_LEN];
    int members[4];       // player IDs (-1 = empty)
    int num_members;
    int shared_materials; // Team material pool
    int shared_score;     // Team score total
    int base_x, base_z;   // Team base location
    int level;            // Team level (from completing objectives)
} Team;

// ═══════════════════════════════════════════════════════════
// PLAYER INTERACTIONS
// ═══════════════════════════════════════════════════════════

#define MAX_TRADE_REQUESTS 8

typedef struct TradeRequest_s {
    int active;
    int from_player;
    int to_player;
    int item_type;        // What being offered
    int quantity;
    int request_type;     // What wanted in return
    int request_qty;
    time_t expiry;
} TradeRequest;

// ═══════════════════════════════════════════════════════════
// SHARED FEATURES
// ═══════════════════════════════════════════════════════════

// Team functions
void init_team_system(void);
int create_team(int leader_id, const char* name);
int join_team(int player_id, int team_id);
void leave_team(int player_id);
void disband_team(int team_id);
void share_materials(int from_player, int to_player, int amount);
void deposit_team_materials(int player_id, int amount);
void withdraw_team_materials(int player_id, int amount);
void list_teams(void);
void show_team_info(int team_id);

// Trade between players
void init_player_trading(void);
void send_trade_request(int from, int to, int item, int qty, int want, int want_qty);
void accept_trade(int request_id);
void decline_trade(int request_id);
void list_trade_requests(int player_id);

// Chat/communication
void team_chat(int player_id, const char* message);
void global_chat(int player_id, const char* message);
void whisper(int from_player, int to_player, const char* message);

// Cooperative features
void revive_player(int reviver_id, int downed_id);
void share_ammo(int from, int to, int amount);
void share_health(int from, int to, int amount);
void call_for_help(int player_id);

// Global access
extern Team teams[MAX_TEAMS];
extern TradeRequest trade_requests[MAX_TRADE_REQUESTS];

#endif
