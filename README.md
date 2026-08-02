# bareiron
Minimalist Minecraft server for memory-restrictive embedded systems.

The goal of this project is to enable hosting Minecraft servers on very weak devices, such as the ESP32. The project's priorities are, in order: **memory usage**, **performance**, and **features**. Because of this, compliance with vanilla Minecraft is not guaranteed, nor is it a goal of the project.

- Minecraft version: `1.21.8`
- Protocol version: `772`

> [!WARNING]
> Currently, only the vanilla client is officially supported. Issues have been reported when using Fabric or similar.

## Quick Start (One Command)

The easiest way to get running:

```bash
# 1. Clone the repo
git clone https://github.com/kaibuzz0/Termux-Mobile-BAREIRON-server.git
cd Termux-Mobile-BAREIRON-server

# 2. Run the installer (downloads Java 21 if needed, builds everything)
./install.sh

# 3. Start the server
./bareiron
```

That's it. The server listens on port 25565. Connect with Minecraft Java 1.21.8.

### Test Your Server

```bash
python3 test_server.py          # Test local connection
python3 test_server.py IP PORT  # Test remote server
```

### Quick Start Script

If you've already run `./install.sh`, you can just run:

```bash
./quickstart.sh   # Builds if needed, then starts server
```

## Game Features (Enhanced Edition)

This fork adds rich gameplay on top of the barebones server:

- **8 Zombie Types** — Walker, Runner, Tank, Boss, Crawler, Boomer, Spitter, Ninja
- **6 Power-ups** — Insta-kill, Double Points, Nuke, Max Ammo, Carpenter, Speed Cola
- **Barricade Building** — Build and repair defenses with materials
- **7 Weapons** — Pistol, Shotgun, Assault Rifle, Sniper, SMG, LMG, RPG
- **Wave Survival** — Escalating difficulty with boss every 5 waves

### Discoveries & Lore
- **The Father's House** — Hidden sanctuary at (352, 33, -318). Safe zone with 7 biblical references. Type `/sanctuary` for clues.
- **Advanced Gameplay** — Weapon upgrades, 8 perks (Juggernog, Speed Cola, Double Tap, Quick Revive, Stamin-Up, PhD Flopper, Deadshot, Mule Kick), discovery items
- **The Ancient Boss** — 3-phase boss fight. Type `/boss` to summon

### Villages & NPCs
- **12 Villages** — Procedurally generated with unique types (Fishing, Mining, Farming, Fortified, Holy, Trade, Academic)
- **15 NPC Classes** — Soldier, Farmer, Weaponsmith, Trader, Healer, Librarian, Fisher, Miner, Hunter, Guard Captain, Scholar, Bard, Alchemist, Innkeeper, Elder
- **NPC Combat** — Soldiers and hunters fight zombies automatically
- **Trading** — Buy ammo, health packs, materials from NPCs
- **Witch Boss** — 3-phase boss with achievement. Type `/summon_witch`
- **Retro Heroes** — Rare spawns: Mega Man, Link, Samus, Mario. Follow and fight for you.

### Multiplayer & Teams
- **Teams** — Create/join teams, shared material pool
- **Player Trading** — Share materials, ammo, health
- **Revive System** — Restore downed teammates
- **Call for Help** — SOS broadcast with coordinates

### Quests & Events
- **Quests** — 8 quest types. Type `/quests` to see active, `/quest [id]` to accept
- **World Events** — Horde attacks, wandering merchants, supply drops, dark rituals. Type `/events`

### Crafting
- **27 Craftable Items** — Traps, turrets, armor, grenades, potions, charms. Type `/craft`

### Save System
- **Persistent Save** — Binary save format. `/save`, `/load`, `/delete_save`

### Commands
Type `/help` in-game for the full command list.

## Compilation (Advanced)

Before compiling, you'll need to dump registry data from a vanilla Minecraft server. On Linux, this can be done automatically using the `extract_registries.sh` script. Otherwise, the manual process is as follows: create a folder called `notchian` here, and put a Minecraft server JAR in it. Then, follow [this guide](https://minecraft.wiki/w/Minecraft_Wiki:Projects/wiki.vg_merge/Data_Generators) to dump all of the registries (use the _second_ command with the `--all` flag). Finally, run `build_registries.js` with either [bun](https://bun.sh/), [node](https://nodejs.org/en/download), or [deno](https://docs.deno.com/runtime/getting_started/installation/).

- To compile on Linux, install `gcc` and run `./build.sh`.
- For compiling on Windows, there are a few options:
  - To compile a native Windows binary: install [MSYS2](https://www.msys2.org/) and open the "MSYS2 MINGW64" shell. From there, run `pacman -Sy mingw-w64-x86_64-gcc`, navigate to this project's directory, and run `./build.sh`.
  - To compile a native 32-bit binary (compatible with Windows 95/98, but why would you ever want that), use the same steps above, except with `pacman -Sy mingw-w64-cross-gcc` and `./build.sh --9x`.
  - To compile a MSYS2-linked binary: install [MSYS2](https://www.msys2.org/), and open the "MSYS2 MSYS" shell. From there, install `gcc` (run `pacman -Sy gcc`), navigate to this project's directory and run `./build.sh`. 
  - To compile and run a Linux binary from Windows: install WSL, and from there install `gcc` and run `./build.sh` in this project's directory.
- To target an ESP variant, set up a PlatformIO project (select the ESP-IDF framework, **not Arduino**) and clone this repository on top of it. See **Configuration** below for further steps. For better performance, consider changing the clock speed and enabling compiler optimizations. If you don't know how to do this, there are plenty of resources online.

## Configuration
Configuring the server requires compiling it from its source code as described in the section above.

Most user-friendly configuration options are available in `include/globals.h`, including WiFi credentials for embedded setups. Some other details, like the MOTD or starting time of day, can be found in `src/globals.c`. For everything else, you'll have to dig through the code.

Here's a summary of some of the more important yet less trivial options for those who plan to use this on a real microcontroller with real players:

- Depending on the player count, the performance of the MCU, and the bandwidth of your network, player position broadcasting could potentially throttle your connection. If you find this to be the case, try commenting out `BROADCAST_ALL_MOVEMENT` and `SCALE_MOVEMENT_UPDATES_TO_PLAYER_COUNT`. This will tie movement to the tickrate. If this change makes movement too choppy, you can decrease `TIME_BETWEEN_TICKS` at the cost of more compute.
- If you experience crashes or instability related to chests or water, those features can be disabled with `ALLOW_CHESTS` and `DO_FLUID_FLOW`, respectively.
- If you find frequent repeated chunk generation to choke the server, increasing `VISITED_HISTORY` might help. There isn't _that_ much of a memory footprint for this - increasing it to `64` for example would only take up 240 extra bytes per allocated player.

## Non-volatile storage (optional)
This section applies to those who target ESP variants and wish to persist world data after a shutdown. *This is not necessary on PC platforms*, as world and player data is written to `world.bin` by default.

The simplest way to accomplish this is to set up LittleFS in PlatformIO and comment out the `#ifndef` surrounding `SYNC_WORLD_TO_DISK` in `globals.h`. Since flash writes are typically slow and blocking, you'll likely want to uncomment `DISK_SYNC_BLOCKS_ON_INTERVAL`. Depending on the flash size of your board, you may also have to decrease `MAX_BLOCK_CHANGES`, so that the world data fits in your LittleFS partition.

If using an SD card module or other virtual file system, you'll have to implement the filesystem setup routine on your own. The built-in serializer should still work though, as it uses POSIX filesystem calls.

Alternatively, if you can't set up a file system, you can dump and upload world data over TCP. This can be enabled by uncommenting `DEV_ENABLE_BEEF_DUMPS` in `globals.h`. *Note: this system implements no security or authentication.* With this option enabled, anyone with access to the server can upload arbitrary world data.

## Contribution
- Create issues and discuss with the maintainer(s) before making pull requests. Even for small changes.
- Follow the existing code style. Ensure that your changes fit in with the surrounding code, even if you disagree with the style. Pull requests with inconsistent style will be nitpicked.
- Test your code before creating a pull request or requesting a review, regardless of how "simple" your change is. It's a basic form of respect towards the maintainer and reviewer.
- Development tooling and compilation improvements _are not welcome,_ unless you've worked with the codebase long enough to have noticed practical shortcomings in that area. Adding a single compiler flag is not a meaningful first contribution.
- For information on the Minecraft server protocol, [refer to the wiki](https://minecraft.wiki/w/Java_Edition_protocol/Packets). For everything else, use a [search engine](https://google.com).