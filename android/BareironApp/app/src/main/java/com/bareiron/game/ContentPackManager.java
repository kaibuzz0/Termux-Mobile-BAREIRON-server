// ContentPackManager.java — Load, install, activate content packs
package com.bareiron.game;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class ContentPackManager {
    private static final String TAG = "BareironContent";
    private static ContentPackManager instance;
    private Context context;
    private Map<String, ContentPack> packs = new HashMap<>();
    private ContentPack activePack = null;
    
    private ContentPackManager(Context ctx) {
        this.context = ctx;
        loadBuiltInPacks();
        loadUserPacks();
    }
    
    public static void init(Context ctx) {
        if (instance == null) {
            instance = new ContentPackManager(ctx);
        }
    }
    
    public static ContentPackManager get() {
        return instance;
    }
    
    // Load packs bundled in assets/content/
    private void loadBuiltInPacks() {
        AssetManager am = context.getAssets();
        try {
            String[] dirs = am.list("content");
            if (dirs == null) return;
            for (String dir : dirs) {
                String[] files = am.list("content/" + dir);
                if (files == null) continue;
                for (String file : files) {
                    if (file.endsWith(".json")) {
                        loadPackFromAsset("content/" + dir + "/" + file);
                    }
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "Failed to list assets: " + e.getMessage());
        }
    }
    
    private void loadPackFromAsset(String path) {
        try (InputStream is = context.getAssets().open(path)) {
            String json = readStream(is);
            ContentPack pack = new Gson().fromJson(json, ContentPack.class);
            pack.installed = true;
            packs.put(pack.id, pack);
            Log.i(TAG, "Loaded pack: " + pack.name + " (" + pack.type + ")");
        } catch (Exception e) {
            Log.e(TAG, "Failed to load " + path + ": " + e.getMessage());
        }
    }
    
    private void loadUserPacks() {
        File dir = new File(context.getFilesDir(), "content_packs");
        if (!dir.exists()) return;
        File[] files = dir.listFiles((d, name) -> name.endsWith(".json"));
        if (files == null) return;
        for (File f : files) {
            try (FileInputStream fis = new FileInputStream(f)) {
                String json = readStream(fis);
                ContentPack pack = new Gson().fromJson(json, ContentPack.class);
                pack.installed = true;
                packs.put(pack.id, pack);
            } catch (Exception e) {
                Log.e(TAG, "Failed to load user pack: " + e.getMessage());
            }
        }
    }
    
    private String readStream(InputStream is) throws IOException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        byte[] buf = new byte[1024];
        int n;
        while ((n = is.read(buf)) != -1) {
            baos.write(buf, 0, n);
        }
        return baos.toString(StandardCharsets.UTF_8.name());
    }
    
    public List<ContentPack> getPacksByType(String type) {
        List<ContentPack> result = new ArrayList<>();
        for (ContentPack p : packs.values()) {
            if (p.type.equals(type)) result.add(p);
        }
        return result;
    }
    
    public List<ContentPack> getAllPacks() {
        return new ArrayList<>(packs.values());
    }
    
    public ContentPack getPack(String id) {
        return packs.get(id);
    }
    
    public void activatePack(String id) {
        ContentPack p = packs.get(id);
        if (p != null) {
            activePack = p;
            p.active = true;
            Log.i(TAG, "Activated pack: " + p.name);
        }
    }
    
    public ContentPack getActivePack() {
        return activePack;
    }
    
    // Install a pack from downloaded JSON
    public boolean installPack(String json) {
        try {
            ContentPack pack = new Gson().fromJson(json, ContentPack.class);
            File dir = new File(context.getFilesDir(), "content_packs");
            dir.mkdirs();
            File out = new File(dir, pack.id + ".json");
            try (FileWriter fw = new FileWriter(out)) {
                fw.write(json);
            }
            pack.installed = true;
            packs.put(pack.id, pack);
            return true;
        } catch (Exception e) {
            Log.e(TAG, "Install failed: " + e.getMessage());
            return false;
        }
    }
    
    // Get world data for the active pack
    public String getWorldData() {
        if (activePack == null) return null;
        try {
            InputStream is = context.getAssets().open("content/" + activePack.type + "s/" + activePack.id + "/world.json");
            return readStream(is);
        } catch (IOException e) {
            // Fallback: generate basic world data
            return generateBasicWorld(activePack);
        }
    }
    
    private String generateBasicWorld(ContentPack pack) {
        return "{\"name\":\"" + pack.name + "\",\"type\":\"" + pack.type + "\",\"description\":\"" + pack.description + "\"}";
    }
}
