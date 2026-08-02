// MainActivity.java — Launcher: server connect + content manager
package com.bareiron.game;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

public class MainActivity extends AppCompatActivity {
    private TextView tvStatus;
    private Button btnConnect;
    private Button btnContentManager;
    private RecyclerView rvActivePacks;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        tvStatus = findViewById(R.id.tvStatus);
        btnConnect = findViewById(R.id.btnConnect);
        btnContentManager = findViewById(R.id.btnContentManager);
        rvActivePacks = findViewById(R.id.rvActivePacks);
        
        btnConnect.setOnClickListener(v -> {
            startActivity(new Intent(this, ServerConnectActivity.class));
        });
        
        btnContentManager.setOnClickListener(v -> {
            startActivity(new Intent(this, ContentManagerActivity.class));
        });
        
        refreshStatus();
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        refreshStatus();
    }
    
    private void refreshStatus() {
        ContentPack active = ContentPackManager.get().getActivePack();
        if (active != null) {
            tvStatus.setText("Active: " + active.name + "\n" + 
                active.type + " | " + active.blockCount + " blocks | " + 
                active.mobCount + " mobs | " + active.questCount + " quests");
        } else {
            tvStatus.setText("No content pack active.\nTap Content Manager to add realms, dimensions, cities.");
        }
        
        // Show active packs grid
        List<ContentPack> activePacks = ContentPackManager.get().getPacksByType("realm");
        activePacks.addAll(ContentPackManager.get().getPacksByType("dimension"));
        activePacks.addAll(ContentPackManager.get().getPacksByType("city"));
        
        PackAdapter adapter = new PackAdapter(activePacks, pack -> {
            ContentPackManager.get().activatePack(pack.id);
            refreshStatus();
        });
        rvActivePacks.setLayoutManager(new GridLayoutManager(this, 2));
        rvActivePacks.setAdapter(adapter);
    }
}
