// ContentListFragment.java — Shows list of content packs per type
package com.bareiron.game;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

public class ContentListFragment extends Fragment {
    private String type = "realm";
    private RecyclerView recyclerView;
    private TextView tvEmpty;
    
    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            type = getArguments().getString("type", "realm");
        }
    }
    
    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_content_list, container, false);
        recyclerView = view.findViewById(R.id.recyclerView);
        tvEmpty = view.findViewById(R.id.tvEmpty);
        
        refreshList();
        return view;
    }
    
    private void refreshList() {
        List<ContentPack> packs = ContentPackManager.get().getPacksByType(type);
        
        if (packs.isEmpty()) {
            recyclerView.setVisibility(View.GONE);
            tvEmpty.setVisibility(View.VISIBLE);
            tvEmpty.setText("No " + type + "s installed yet.\nDownload from the Add-ons tab or create your own.");
        } else {
            recyclerView.setVisibility(View.VISIBLE);
            tvEmpty.setVisibility(View.GONE);
            
            PackAdapter adapter = new PackAdapter(packs, pack -> {
                ContentPackManager.get().activatePack(pack.id);
                if (getActivity() != null) {
                    getActivity().finish();
                }
            });
            recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
            recyclerView.setAdapter(adapter);
        }
    }
    
    @Override
    public void onResume() {
        super.onResume();
        refreshList();
    }
}
