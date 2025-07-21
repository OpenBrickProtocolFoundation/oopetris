package com.github.oopetris;

import org.libsdl.app.SDLActivity;
import com.discord.socialsdk.DiscordSocialSdkInit;
import android.os.Bundle;

public class MainActivity extends SDLActivity { 

    // Setup
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        DiscordSocialSdkInit.setEngineActivity(this);
    }

}

