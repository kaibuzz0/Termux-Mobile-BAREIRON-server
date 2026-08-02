// PackAdapter.java — RecyclerView adapter for content packs
package com.bareiron.game;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

public class PackAdapter extends RecyclerView.Adapter<PackAdapter.ViewHolder> {
    private List<ContentPack> packs;
    private OnPackClickListener listener;
    
    public interface OnPackClickListener {
        void onPackClick(ContentPack pack);
    }
    
    public PackAdapter(List<ContentPack> packs, OnPackClickListener listener) {
        this.packs = packs;
        this.listener = listener;
    }
    
    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
            .inflate(R.layout.item_content_pack, parent, false);
        return new ViewHolder(view);
    }
    
    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        ContentPack pack = packs.get(position);
        holder.tvName.setText(pack.name);
        holder.tvType.setText(pack.type.toUpperCase() + " | v" + pack.version);
        holder.tvDesc.setText(pack.description);
        
        String stats = pack.blockCount + " blocks | " + pack.mobCount + " mobs | " + pack.questCount + " quests";
        holder.tvStats.setText(stats);
        
        if (pack.active) {
            holder.btnAction.setText("ACTIVE");
            holder.btnAction.setEnabled(false);
        } else if (pack.installed) {
            holder.btnAction.setText("ACTIVATE");
            holder.btnAction.setEnabled(true);
        } else {
            holder.btnAction.setText("INSTALL");
            holder.btnAction.setEnabled(true);
        }
        
        holder.btnAction.setOnClickListener(v -> {
            if (listener != null) listener.onPackClick(pack);
        });
        
        holder.itemView.setOnClickListener(v -> {
            if (listener != null) listener.onPackClick(pack);
        });
    }
    
    @Override
    public int getItemCount() {
        return packs.size();
    }
    
    static class ViewHolder extends RecyclerView.ViewHolder {
        TextView tvName, tvType, tvDesc, tvStats;
        Button btnAction;
        
        ViewHolder(View itemView) {
            super(itemView);
            tvName = itemView.findViewById(R.id.tvName);
            tvType = itemView.findViewById(R.id.tvType);
            tvDesc = itemView.findViewById(R.id.tvDesc);
            tvStats = itemView.findViewById(R.id.tvStats);
            btnAction = itemView.findViewById(R.id.btnAction);
        }
    }
}
