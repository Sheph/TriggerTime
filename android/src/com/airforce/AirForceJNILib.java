package com.airforce;

import android.content.res.AssetManager;

public class AirForceJNILib
{
     static
     {
         System.loadLibrary("openal-soft");
         System.loadLibrary("afjni");
     }

     public static native boolean init(AssetManager assetManager);
     public static native void suspend();
     public static native boolean resize(int w, int h);
     public static native boolean step();
     public static native void cancelStep();
     public static native void render();
     public static native void input(int finger, float x, float y, boolean up);
     public static native void menu();
}
