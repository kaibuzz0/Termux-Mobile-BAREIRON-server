# 🧟 bareiron — Enhanced Zombie Shooter Edition

A **memory-first** Minecraft server fork turned into a fully-featured zombie survival shooter.

```
  Memory: ~227KB binary  |  Protocol: 772 (MC 1.21.8)  |  Players: 8 max
```

> This fork keeps bareiron's extreme minimalism while adding deep survival gameplay —
> zombies, bosses, villages, NPCs, quests, crafting, teams, and persistent saves.

---

## 📦 Quick Start (3 Commands)

```bash
# 1. Clone
git clone https://github.com/kaibuzz0/Termux-Mobile-BAREIRON-server.git
cd Termux-Mobile-BAREIRON-server

# 2. Install (auto-detects Java 21, downloads Minecraft JAR, builds)
./install.sh

# 3. Run
./quickstart.sh        # or: ./bareiron
```

That's it. Server listens on **port 25565**. Connect with **Minecraft Java 1.21.8**.

### Verify It's Working

```bash
python3 test_server.py          # Local test
python3 test_server.py IP PORT  # Remote test
```

Expected output:
```
Version: 1.21.8  |  Protocol: 772  |  MOTD: A bareiron server
✅ ALL TESTS PASSED
```

---

## 🎮 What's Different From Upstream?

| | **Upstream bareiron** | **This Fork** |
|---|---|---|
| Purpose | Vanilla-compatible Minecraft server | Zombie survival shooter |
| Binary | ~150KB | ~227KB (+77KB for all features) |
| Gameplay | Sandbox | Wave-based survival, bosses, RPG systems |
| NPCs | None | 15 classes, combat AI, trading |
| World | Empty | 12 villages, 8 village types |
| Progression | None | Perks, weapon upgrades, quests, crafting |
| Multiplayer | Basic | Teams, shared pools, trading, revive |
| Persistence | world.bin only | Full save/load system |

---

## 🎯 Feature Breakdown

### Core Combat
- **8 Zombie Types** — Walker, Runner, Tank, Boss, Crawler, Boomer, Spitter, Ninja
- **6 Power-ups** — Insta-kill, Double Points, Nuke, Max Ammo, Carpenter, Speed Cola
- **7 Weapons** — Pistol, Shotgun, Assault Rifle, Sniper, SMG, LMG, RPG
- **Wave Survival** — Escalating difficulty with boss every 5 waves
- **Barricade Building** — Place/repair defenses with collected materials

### The Father's House (Sanctuary)
Hidden at coordinates **(352, 33, -318)**. A primordial safe zone with:
- 150-block radius where hostile mobs cannot spawn
- 7 biblical references: twelve stones, olive grove, vine, almond trees, living water, shepherd's staff, eternal lamp
- `/sanctuary` command for discovery clues

### Bosses
| Boss | Command | Phases | Reward |
|------|---------|--------|--------|
| **The Ancient** | `/boss` | 3 (awakening → rage → cataclysm) | Sacred artifact drop |
| **The Witch** | `/summon_witch` | 3 (summoning → shadows → poison cloud) | "Witch Hunter" achievement |

### Villages & NPCs (12 Settlements)

**Village Types:**
`Fishing` `Mining` `Farming` `Fortified` `Religious` `Trading` `Academic` `Generic`

**15 NPC Classes:**

| Class | Role | Combat? |
|-------|------|---------|
| Survivor | Generic villager | No |
| Soldier | Fights zombies | **Yes — 25 dmg** |
| Guard Captain | Village defender | **Yes — 40 dmg** |
| Hunter | Ranged fighter | **Yes — 35 dmg, 12-block range** |
| Farmer | Sells food | No |
| Weaponsmith | Sells ammo, upgrades | No |
| Trader | General goods | No |
| Healer | Removes poison, heals | No |
| Librarian | Hints, lore | No |
| Fisher / Miner / Bard / Alchemist / Innkeeper / Scholar / Elder | Village flavor | No |

- NPCs follow players (`/follow_npc [id]`)
- Familiarity system: repeated visits unlock better dialogue
- 33% chance any NPC has a quest

### Quests (8 Types)

| Type | Description |
|------|-------------|
| Extermination | Kill N zombies |
| Defense | Survive horde attack |
| Pilgrimage | Reach coordinates |
| Rescue | Save trapped survivor |
| Retrieval | Find sacred relic |

- 32 max concurrent quests
- Rewards: materials + score
- Commands: `/quests`, `/quest [id]`

### World Events (8 Types)

Random events with 5% trigger chance:
- **Horde Attack** — Village under siege (2 min)
- **Wandering Merchant** — Special trader spawns (5 min)
- **Zombie Swarm** — Massive spawn nearby (1 min)
- **Supply Drop** — Free materials (3 min)
- **Dark Ritual** — Witch powers up (1.5 min)
- **Survivor Rescue** — Trapped NPC needs help (4 min)

Command: `/events`

### Crafting (27 Items, 4 Tiers)

| Tier | Examples | Cost |
|------|----------|------|
| Basic | Spike Trap, Field Ration, Explosive Ammo | 5–20 mats |
| Intermediate | Fire Trap, Basic Turret, Light Armor | 30–50 mats |
| Advanced | Heavy Turret, Heavy Armor, Molotov | 60–100 mats |
| Expert | Sniper Turret, Nano Armor, Holy Grenade | 100–150 mats |

Some recipes require a **Blacksmith** or **Alchemist** NPC nearby.

Command: `/craft` for menu, `/craft [id]` to build.

### Retro Hero Easter Eggs (0.1% Spawn)

| Hero | Reference | Damage | Style |
|------|-----------|--------|-------|
| **Proto** | Mega Man | 75 | Ranged (arm cannon) |
| **The Hero of Time** | Link | 100 | Melee (sword) |
| **Bounty Hunter** | Samus | 90 | Ranged (missiles) |
| **The Plumber** | Mario | 60 | Ranged (fireballs) |

Command: `/heroes` to check status, `/follow` to recruit nearest.

### Multiplayer & Teams

- **Teams** — Up to 4 teams, 4 members each. Shared material pool.
- **Player Trading** — Direct material/ammo/health transfer
- **Emergency Revive** — Restore downed teammates (proximity check)
- **SOS** — `/call_help` broadcasts coordinates

Commands:
```
/team create [name]   /team join [id]   /team leave
/deposit [n]          /withdraw [n]     /share [player] [amount]
/call_help            /trade_req
```

### Save System

Binary save format with CRC32 checksum. Persists:
- Player stats, inventory, position
- Village discoveries
- Active quests
- High scores
- Boss states

Commands: `/save`, `/load`, `/delete_save`

---

## ⌨️ Command Reference

### Gameplay
| Command | Function |
|---------|----------|
| `/start` | Begin game |
| `/stop` | End game |
| `/status` | Current game state |
| `/wave` | Current wave info |
| `/nextwave` | Force next wave |
| `/difficulty [1-5]` | Set difficulty |

### Weapons & Power-ups
| Command | Function |
|---------|----------|
| `/weapon [1-7]` | Switch weapon |
| `/ammo` | Current ammo |
| `/weapons` | List weapons |
| `/materials` | Material count |
| `/perks` | Active perks |
| `/upgrades` | Weapon upgrades |
| `/nuke` | Clear all zombies |

### Discovery & Lore
| Command | Function |
|---------|----------|
| `/sanctuary` | The Father's House info |
| `/hint` | Cryptic guidance |
| `/lore` | Sacred symbols meaning |
| `/discoveries` | Found items |

### Villages & NPCs
| Command | Function |
|---------|----------|
| `/villages` | List settlements |
| `/npcs` | Nearby NPCs |
| `/trade [id]` | Trade with NPC |
| `/buy [id] [slot]` | Purchase from NPC |
| `/follow_npc [id]` | Recruit NPC |
| `/follow` | Recruit hero |
| `/summon_witch` | Spawn Witch boss |
| `/witch` | Witch status |
| `/boss` | Summon Ancient |
| `/heroes` | Hero status |

### Quests & Events
| Command | Function |
|---------|----------|
| `/quests` | Active quests |
| `/quest [id]` | Accept quest |
| `/events` | Active world events |

### Crafting
| Command | Function |
|---------|----------|
| `/craft` | Show crafting menu |
| `/craft [id]` | Craft item |

### Save
| Command | Function |
|---------|----------|
| `/save` | Save game |
| `/load` | Load saved game |
| `/delete_save` | Delete save |

### Multiplayer
| Command | Function |
|---------|----------|
| `/team create [name]` | Create team |
| `/team join [id]` | Join team |
| `/team leave` | Leave team |
| `/team list` | Show teams |
| `/team info [id]` | Team details |
| `/deposit [n]` | Pool materials |
| `/withdraw [n]` | Take from pool |
| `/share [p] [n]` | Give materials |
| `/call_help` | SOS broadcast |
| `/trade_req` | Pending requests |

### Utility
| Command | Function |
|---------|----------|
| `/help` | Full command list |
| `/scores` | High scores |
| `/map [file]` | Load custom map |
| `/players` | Connected players |
| `/me [msg]` | Emote |
| `/time` | Server time |
| `/seed` | World seed |
| `/coords` | Your position |
| `/difficulty` | Current difficulty |
| `/modes` | Game modes |
| `/survival` | Survival mode |
| `/endless` | Endless mode |
| `/horde` | Horde mode |
| `/build` | Build mode |

---

## 🏗️ Architecture

This project deliberately sacrifices vanilla compliance for extreme minimalism:

- **No dimensions** — No Nether/End (would double binary size)
- **No entity AI pathfinding** — NPCs use simple lerp follow
- **No inventory system** — Weapons, ammo, materials are integers
- **No block physics** — Static world, no redstone, no fluid flow
- **No auth/encryption** — Open server, LAN-focused

The result: a **227KB binary** that runs on ESP32, Termux, and any device with `gcc`.

---

## 🛠️ Compilation (Advanced)

If you prefer manual compilation over `./install.sh`:

### Prerequisites
- `gcc` (any version)
- `nodejs` (for registry generation)
- Java 21+ (for Minecraft server JAR extraction)

### Steps
```bash
# 1. Download Minecraft server JAR
mkdir -p notchian
curl -L -o notchian/server.jar \
    https://piston-data.mojang.com/v1/objects/6bce4ef400e4efaa63a13d5e6f6b500be969ef81/server.jar

# 2. Extract registry data
cd notchian
java -DbundlerMainClass="net.minecraft.data.Main" -jar server.jar --all
cd ..

# 3. Build C headers
node build_registries.js

# 4. Compile
gcc src/*.c -O2 -Iinclude -o bareiron -lm

# 5. Run
./bareiron
```

### Platform Notes

**Termux (Android):**
```bash
pkg install gcc nodejs openjdk-21
./install.sh
```

**Windows (MSYS2):**
```bash
pacman -Sy mingw-w64-x86_64-gcc nodejs
# Then follow manual steps above
```

**ESP32:**
Set up PlatformIO with ESP-IDF framework. See upstream bareiron docs for details.

---

## ⚙️ Configuration

Edit `include/globals.h` for:
- WiFi credentials (ESP32)
- Player slots, tick rate
- Broadcast settings
- Chest/fluid flow toggles

Edit `config/game.json` for:
- Zombie spawn rates
- Wave parameters
- Difficulty scaling

---

## 📁 File Map

```
.
├── bareiron                  # Server binary (generated)
├── install.sh               # One-command installer
├── quickstart.sh            # Fast build + run
├── test_server.py           # Protocol tester
│
├── include/
│   ├── registries.h         # Minecraft data (generated)
│   ├── zombie_game.h        # Core game
│   ├── fathers_house.h      # Sanctuary system
│   ├── gameplay_expansion.h # Bosses, perks, upgrades
│   ├── villages_npcs.h      # Villages, NPCs, Witch, Heroes
│   ├── savegame.h           # Binary persistence
│   ├── multiplayer.h        # Teams, trading, chat
│   └── crafting_expansion.h # Crafting recipes
│
├── src/
│   ├── *.c                  # All source files
│   └── ...                  # Same filenames as headers
│
├── config/
│   └── game.json            # Gameplay parameters
│
└── notchian/
    └── server.jar           # Minecraft JAR (downloaded)
```

---

## 🙏 Credits

- **Upstream:** [p2r3/bareiron](https://github.com/p2r3/bareiron) — The original memory-first Minecraft server
- **This fork:** Enhanced by kaibuzz0 — Zombie shooter gameplay, NPCs, quests, crafting, saves, multiplayer

## 📜 License

Same as upstream bareiron. See original repository for details.

## 🤝 Contributing

This is a personal fork. For upstream contributions, see [p2r3/bareiron](https://github.com/p2r3/bareiron).

For this fork: open an issue if something's broken. Pull requests welcome but this is primarily a solo project.
