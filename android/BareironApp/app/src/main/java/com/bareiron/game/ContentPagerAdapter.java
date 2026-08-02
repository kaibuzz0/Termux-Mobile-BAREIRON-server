// ContentPagerAdapter.java — ViewPager adapter for content tabs
package com.bareiron.game;

import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.viewpager2.adapter.FragmentStateAdapter;

public class ContentPagerAdapter extends FragmentStateAdapter {
    public ContentPagerAdapter(@NonNull FragmentActivity activity) {
        super(activity);
    }
    
    @NonNull
    @Override
    public Fragment createFragment(int position) {
        ContentListFragment fragment = new ContentListFragment();
        Bundle args = new Bundle();
        switch (position) {
            case 0: args.putString("type", "realm"); break;
            case 1: args.putString("type", "dimension"); break;
            case 2: args.putString("type", "city"); break;
            case 3: args.putString("type", "addon"); break;
            default: args.putString("type", "realm");
        }
        fragment.setArguments(args);
        return fragment;
    }
    
    @Override
    public int getItemCount() {
        return 4;
    }
}
