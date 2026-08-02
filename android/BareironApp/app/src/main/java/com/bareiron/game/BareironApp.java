// BareironApp.java — Application class
package com.bareiron.game;

import android.app.Application;
import android.content.Context;

public class BareironApp extends Application {
    private static Context appContext;
    
    @Override
    public void onCreate() {
        super.onCreate();
        appContext = getApplicationContext();
        ContentPackManager.init(appContext);
    }
    
    public static Context getAppContext() {
        return appContext;
    }
}
