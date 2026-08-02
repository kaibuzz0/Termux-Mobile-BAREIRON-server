// ContentPack.java — Data model for downloadable content
package com.bareiron.game;

public class ContentPack {
    public String id;           // e.g. "realm_firelands_v2"
    public String name;         // "Firelands"
    public String type;         // "realm", "dimension", "city", "addon"
    public String version;      // "2.1.0"
    public String author;       // Community or official
    public String description;
    public String thumbnail;
    public String downloadUrl;  // CDN or local path
    public long sizeBytes;
    public boolean installed;
    public boolean active;
    public String[] dependencies; // Other pack IDs required
    public String mcVersion;    // "1.21.8"
    
    // Content counts
    public int blockCount;
    public int mobCount;
    public int itemCount;
    public int questCount;
    public int structureCount;
    
    public ContentPack() {}
    
    public ContentPack(String id, String name, String type, String description) {
        this.id = id;
        this.name = name;
        this.type = type;
        this.description = description;
        this.version = "1.0";
        this.installed = false;
        this.active = false;
    }
}
