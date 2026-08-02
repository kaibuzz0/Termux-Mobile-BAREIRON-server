/*
 * BAREIRON CRAFTING EXPANSION
 * Extended recipes: traps, turrets, advanced ammo, armor, beacons
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crafting_expansion.h"

Recipe recipes[MAX_RECIPES];

static const char* craftable_names[] = {
    "","Spike Trap","Fire Trap","Ice Trap","Basic Turret","Heavy Turret",
    "Sniper Turret","Advanced Medkit","Explosive Ammo","Freezing Ammo",
    "Molotov Grenade","Freeze Grenade","Holy Grenade","Light Armor",
    "Heavy Armor","Nano Armor","Repair Tool","Deconstructor","Smoke Beacon",
    "Holo Decoy","Field Ration","Hearty Stew","Hero's Feast","Zombie Radar",
    "Survey Map","Luck Charm","Ward Charm","Vengeance Charm"
};

static const char* craftable_descs[] = {
    "",
    "Damages zombies walking over it.",
    "Sets zombies on fire.",
    "Slows zombies to a crawl.",
    "Auto-targets nearest zombie.",
    "High damage turret, slow fire rate.",
    "Long range, instant kills weak zombies.",
    "Full heal + health regeneration.",
    "Splash damage bullets.",
    "Bullets that freeze targets.",
    "Fire everywhere!",
    "Ice everywhere!",
    "Massive damage to undead.",
    "+25 max HP, light protection.",
    "+50 max HP, heavy plating.",
    "+75 max HP, regenerates health.",
    "Instantly repair any barricade.",
    "Break items down for materials.",
    "Marks location for supply drops.",
    "Distracts zombies with hologram.",
    "Restores 10 HP.",
    "Restores 25 HP + speed boost.",
    "Restores 50 HP, heals nearby allies.",
    "Reveals nearby zombies on map.",
    "Reveals terrain ahead.",
    "Better drops from enemies.",
    "Zombies are less likely to target you.",
    "Damage increases as health decreases."
};

void init_crafting_system(void) {
    memset(recipes, 0, sizeof(recipes));
    
    // Tier 1: Basic (cheap)
    recipes[1] = (Recipe){CRAFT_TRAP_SPIKE, "Spike Trap", "Damages zombies", 15, 50, 0, 0};
    recipes[2] = (Recipe){CRAFT_FOOD_RATION, "Field Ration", "Quick snack", 5, 10, 0, 0};
    recipes[3] = (Recipe){CRAFT_AMMO_EXPLOSIVE, "Explosive Rounds", "x30 splash ammo", 20, 75, 0, 0};
    
    // Tier 2: Intermediate
    recipes[4] = (Recipe){CRAFT_TRAP_FIRE, "Fire Trap", "Burns zombies", 30, 100, 0, 0};
    recipes[5] = (Recipe){CRAFT_TURRET_BASIC, "Basic Turret", "Auto-defense", 50, 200, 1, 0};
    recipes[6] = (Recipe){CRAFT_ARMOR_LIGHT, "Light Armor", "+25 max HP", 40, 150, 1, 0};
    recipes[7] = (Recipe){CRAFT_MEDKIT_ADVANCED, "Advanced Medkit", "Full heal + regen", 35, 125, 0, 1};
    
    // Tier 3: Advanced
    recipes[8] = (Recipe){CRAFT_TURRET_HEAVY, "Heavy Turret", "Big damage", 100, 400, 1, 0};
    recipes[9] = (Recipe){CRAFT_ARMOR_HEAVY, "Heavy Armor", "+50 max HP", 80, 300, 1, 0};
    recipes[10] = (Recipe){CRAFT_GRENADE_MOLOTOV, "Molotov", "Fire area", 25, 100, 0, 0};
    recipes[11] = (Recipe){CRAFT_BEACON_SMOKE, "Smoke Beacon", "Call supplies", 60, 250, 0, 0};
    
    // Tier 4: Expert
    recipes[12] = (Recipe){CRAFT_TURRET_SNIPER, "Sniper Turret", "Long range kills", 150, 600, 1, 0};
    recipes[13] = (Recipe){CRAFT_ARMOR_NANO, "Nano Armor", "+75 HP + regen", 120, 500, 1, 1};
    recipes[14] = (Recipe){CRAFT_GRENADE_HOLY, "Holy Grenade", "Anti-undead", 75, 300, 0, 1};
    recipes[15] = (Recipe){CRAFT_CHARM_VENGEANCE, "Vengeance Charm", "Rage = power", 100, 400, 0, 1};
    
    printf("[CRAFT] Crafting system initialized with %d recipes\n", MAX_RECIPES);
}

const char* get_craftable_name(CraftableItem item) {
    if (item < 0 || item >= sizeof(craftable_names)/sizeof(craftable_names[0])) return "Unknown";
    return craftable_names[item];
}

const char* get_craftable_desc(CraftableItem item) {
    if (item < 0 || item >= sizeof(craftable_descs)/sizeof(craftable_descs[0])) return "???";
    return craftable_descs[item];
}

void show_crafting_menu(int player_id) {
    (void)player_id;
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  CRAFTING MENU                                            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  %-20s %-30s %s\n", "Item", "Description", "Cost");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    for (int i=1; i<MAX_RECIPES; i++) {
        if (recipes[i].result == CRAFT_NONE) continue;
        Recipe* r = &recipes[i];
        printf("║  [%2d] %-18s %-28s %dm/%ds\n",
               r->result, r->name, r->description, r->mats_cost, r->score_cost);
    }
    
    printf("║                                                            ║\n");
    printf("║  Type /craft [id] to create                                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

int craft_item(int player_id, CraftableItem item) {
    if (player_id < 0 || player_id >= 8) return 0;
    if (item <= CRAFT_NONE || item >= sizeof(craftable_names)/sizeof(craftable_names[0])) {
        printf("[CRAFT] Invalid item!\n");
        return 0;
    }
    
    // Find recipe
    Recipe* r = NULL;
    for (int i=0; i<MAX_RECIPES; i++) {
        if (recipes[i].result == item) {
            r = &recipes[i];
            break;
        }
    }
    if (!r || r->result == CRAFT_NONE) {
        printf("[CRAFT] Recipe not found!\n");
        return 0;
    }
    
    // Check if player can afford
    extern struct Player_s { int id; char name[32]; float x,y,z; int weapon; int ammo; int max_ammo; int health; int max_health; int score; int kills; float speed_boost; float damage_boost; int invincible; time_t powerup_end; int materials; int barricades_built; } players[8];
    
    if (players[player_id].materials < r->mats_cost) {
        printf("[CRAFT] Not enough materials! Need %d, have %d.\n",
               r->mats_cost, players[player_id].materials);
        return 0;
    }
    
    // Deduct cost
    players[player_id].materials -= r->mats_cost;
    
    // Apply crafted item effect
    switch(item) {
        case CRAFT_ARMOR_LIGHT:
            players[player_id].max_health += 25;
            players[player_id].health += 25;
            printf("[CRAFT] Created Light Armor! Max HP +25\n");
            break;
        case CRAFT_ARMOR_HEAVY:
            players[player_id].max_health += 50;
            players[player_id].health += 50;
            printf("[CRAFT] Created Heavy Armor! Max HP +50 (speed -10%%)\n");
            break;
        case CRAFT_ARMOR_NANO:
            players[player_id].max_health += 75;
            players[player_id].health += 75;
            printf("[CRAFT] Created Nano Armor! Max HP +75, health regeneration enabled.\n");
            break;
        case CRAFT_MEDKIT_ADVANCED:
            players[player_id].health = players[player_id].max_health;
            printf("[CRAFT] Used Advanced Medkit! Fully healed.\n");
            break;
        case CRAFT_AMMO_EXPLOSIVE:
            players[player_id].ammo += 30;
            printf("[CRAFT] Created 30 explosive rounds!\n");
            break;
        case CRAFT_AMMO_FREEZING:
            players[player_id].ammo += 30;
            printf("[CRAFT] Created 30 freezing rounds!\n");
            break;
        case CRAFT_FOOD_RATION:
            players[player_id].health += 10;
            if (players[player_id].health > players[player_id].max_health)
                players[player_id].health = players[player_id].max_health;
            printf("[CRAFT] Ate Field Ration! +10 HP\n");
            break;
        case CRAFT_FOOD_STEW:
            players[player_id].health += 25;
            if (players[player_id].health > players[player_id].max_health)
                players[player_id].health = players[player_id].max_health;
            players[player_id].speed_boost = 1.3f;
            players[player_id].powerup_end = time(NULL) + 20;
            printf("[CRAFT] Ate Hearty Stew! +25 HP, speed boost!\n");
            break;
        case CRAFT_TRAP_SPIKE:
        case CRAFT_TRAP_FIRE:
        case CRAFT_TRAP_ICE:
            printf("[CRAFT] Placed %s!\n", get_craftable_name(item));
            break;
        case CRAFT_TURRET_BASIC:
        case CRAFT_TURRET_HEAVY:
        case CRAFT_TURRET_SNIPER:
            printf("[CRAFT] Deployed %s! It will auto-target zombies.\n", get_craftable_name(item));
            break;
        default:
            printf("[CRAFT] Created %s!\n", get_craftable_name(item));
    }
    
    return 1;
}

void show_recipe_details(CraftableItem item) {
    if (item <= CRAFT_NONE) return;
    Recipe* r = NULL;
    for (int i=0; i<MAX_RECIPES; i++) {
        if (recipes[i].result == item) { r = &recipes[i]; break; }
    }
    if (!r) return;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  RECIPE: %-30s                           ║\n", r->name);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  %s\n", r->description);
    printf("║  Cost: %d materials OR %d score\n", r->mats_cost, r->score_cost);
    if (r->requires_smithy) printf("║  Requires: Blacksmith\n");
    if (r->requires_alchemist) printf("║  Requires: Alchemist\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}
