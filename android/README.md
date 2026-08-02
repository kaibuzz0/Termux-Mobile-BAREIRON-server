# 📱 BAREIRON Android Content Manager

A lightweight Android companion app for the BAREIRON Minecraft server. The server stays micro (~227KB). All heavy content lives here.

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                Android App (You are here)            │
│  ├─ Realms, Dimensions, Cities, Add-ons             │
│  ├─ Content Pack Manager (install/activate)         │
│  ├─ Server Connection (IP + Port)                   │
│  └─ World Browser (JSON-driven, no server load)      │
├─────────────────────────────────────────────────────┤
│              Minecraft Protocol 772                    │
├─────────────────────────────────────────────────────┤
│              BAREIRON Server (~227KB)                │
│  ├─ Core zombie shooter                              │
│  ├─ Father's House sanctuary                        │
│  ├─ Villages, NPCs, quests, events                  │
│  ├─ Crafting, multiplayer, save/load                │
│  └─ Minimal binary, no heavy content                │
└─────────────────────────────────────────────────────┘
```

## What This App Does

| Feature | Purpose |
|---------|---------|
| **Server Connect** | Enter IP/port, ping server, save connection |
| **Content Manager** | Browse realms, dimensions, cities, add-ons |
| **Pack Activation** | Select which content loads in your world |
| **JSON-Driven** | All content is JSON files — no code changes needed |

## Content Pack Format

Each pack is a JSON file in `assets/content/<type>/`:

```json
{
    "id": "realm_aetherfall",
    "name": "Aetherfall",
    "type": "realm",
    "version": "1.0",
    "author": "Your Name",
    "description": "Floating sky kingdom with crystal islands.",
    "mcVersion": "1.21.8",
    "blockCount": 256,
    "mobCount": 18,
    "itemCount": 32,
    "questCount": 12,
    "structureCount": 8,
    "dependencies": [],
    "biomes": ["floating_islands", "thunder_peaks"],
    "features": ["gravity_pads", "storm_events", "wing_serpent_boss"]
}
```

## Types

| Type | Description | Example |
|------|-------------|---------|
| `realm` | Large overworld expansion | Aetherfall (floating islands) |
| `dimension` | Separate dimension (like Nether/End) | Nether Moria (underground) |
| `city` | Built-up urban area | Steelhaven (crater city) |
| `addon` | Content injection (mobs, items, recipes) | Creature Compendium (+52 mobs) |

## Adding Your Own Content

1. Create JSON in `assets/content/<type>/your_pack.json`
2. Add thumbnail in `res/drawable/` (optional)
3. Rebuild APK
4. Or: install JSON via Content Manager "Import" (future)

## Build

```bash
cd android/BareironApp

# With Android Studio: File → Open → build.gradle
# With command line:
./gradlew assembleDebug

# APK output: app/build/outputs/apk/debug/app-debug.apk
```

## File Structure

```
android/BareironApp/
├── app/src/main/
│   ├── java/com/bareiron/game/
│   │   ├── BareironApp.java              # Application class
│   │   ├── ContentPack.java              # Data model
│   │   ├── ContentPackManager.java       # Load/activate packs
│   │   ├── MainActivity.java             # Launcher
│   │   ├── ServerConnectActivity.java    # Server ping + connect
│   │   ├── ContentManagerActivity.java   # Tabbed browser
│   │   ├── ContentPagerAdapter.java    # ViewPager
│   │   ├── ContentListFragment.java      # List per type
│   │   └── PackAdapter.java              # RecyclerView adapter
│   │
│   ├── res/layout/
│   │   ├── activity_main.xml
│   │   ├── activity_server_connect.xml
│   │   ├── activity_content_manager.xml
│   │   ├── fragment_content_list.xml
│   │   └── item_content_pack.xml
│   │
│   ├── res/values/
│   │   ├── strings.xml
│   │   └── themes.xml
│   │
│   └── assets/content/
│       ├── realms/aetherfall.json
│       ├── dimensions/nether_moria.json
│       ├── cities/steelhaven.json
│       └── addons/creature_compendium.json
│
├── build.gradle                          # Project build
└── settings.gradle
```

## Future Features

- [ ] CDN download for content packs (no rebuild needed)
- [ ] Pack editor UI (create JSON in-app)
- [ ] Server-side content sync (push active pack to server)
- [ ] Map viewer (show world layout from JSON)
- [ ] Quest tracker (read quest JSON, show progress)
- [ ] Skin/texture pack manager
- [ ] Chat overlay (in-game chat from phone)
- [ ] Player stats dashboard

## Why Separate App?

The BAREIRON server is designed to stay under 250KB — small enough for ESP32 and Termux. Heavy content (thousands of blocks, dozens of realms, massive cities) would blow that up.

By keeping content in the Android app:
- Server stays lean and fast
- Content can be huge (unlimited JSON files)
- Users pick what they want (modular)
- Updates without recompiling server
- Artists can create packs without touching C

---

**The server is micro. The app is macro. Best of both worlds.**
