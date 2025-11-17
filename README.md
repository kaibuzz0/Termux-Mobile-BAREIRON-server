# Termux-Mobile-BAREIRON-server
Run a mobile server for mincraft 
BareIron on Termux — Full Install Guide (Android)

A step-by-step guide to compiling and running the BareIron Minecraft server on an Android device using Termux.

This README documents the exact working installation path used to successfully build and launch a BareIron server on a mobile device.


---

📌 Requirements

Android 8+
Termux (from F-Droid or GitHub, not Play Store)
Minecraft server JAR (1.21.x)
Internet connection for initial setup

---

📁 1. Install Required Packages

>Open Termux and run:

pkg update && pkg upgrade -y

pkg install -y git build-essential nodejs
pkg install -y openjdk-17
pkg install -y openjdk-21 openjdk-21-x
pkg install -y termux-api

>This installs:
C compiler & build tools
Node.js
Java 21 (required by Mojang’s data generator)
Termux API tools



---

>📁 2. Clone BareIron Repository

cd ~
git clone https://github.com/p2r3/bareiron.git
cd ~/bareiron

>BareIron now exists at:

~/bareiron


---

>📁 3. Enable Storage Access & Copy server.jar

>Enable access:

termux-setup-storage

>Find your downloaded Minecraft JAR here:

~/storage/downloads

>Create the directory:

mkdir -p notchian

>Copy your server JAR:

cp ~/storage/downloads/server.jar notchian/server.jar


---

📁 4. Confirm Java 21 is Active

java -version

>Expected output:

openjdk version "21.x"

>If needed:

update-alternatives --config java

>Select Java 21.


---

>📁 5. Extract Mojang Registries

cd ~/bareiron
chmod +x extract_registries.sh
./extract_registries.sh

>This:
-Unpacks the vanilla server
-Runs Mojang’s data generator
-Dumps vanilla registries
-Then process registries into C:

node build_registries.js

>Outputs:
-Wrote registries.c and registries.h


---

📁 6. Fix Compile Error (B_redstone_block)

>If you encounter:

error: use of undeclared identifier 'B_redstone_block'

>Edit the file:

nano src/procedures.c

>Find the line containing:

case B_redstone_block:

>Comment it out:

// case B_redstone_block:
//     break;

>Save & exit (Ctrl+O, Enter, Ctrl+X).


---

📁 7. Build BareIron

chmod +x build.sh
./build.sh

>If successful, you will see:

Server listening on port 25565...


---

📁 8. Running the Server (Any Time)

>From a new Termux session:

cd ~/bareiron
./bareiron


---

>📡 9. Connecting to the Server

>On the same phone (via PojavLauncher)
>Use:

127.0.0.1:25565

>On local Wi-Fi (if hotspot/Wi-Fi active)
>Check IP:

ifconfig

>Or:

termux-wifi-connectioninfo

>Connect via LAN:

YOUR_PHONE_IP:25565


---

📌 Status

✔️ Fully builds on Android
✔️ Runs successfully on Termux
✔️ Can accept players (local or LAN)
✔️ Ready for future mods (zombies, grenades, custom gameplay)


---

📜 License

This guide is provided freely to help others build BareIron on Android.


---
