package com.airforce;

import android.app.Activity;
import android.os.Bundle;

public class AirForceActivity extends Activity
{
    private AirForceGLSurfaceView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        glView = new AirForceGLSurfaceView(this);
        setContentView(glView);
    }

    @Override
    protected void onDestroy() {
        glView.stop();
        super.onDestroy();
    }

    @Override
    public void onBackPressed()
    {
        AirForceJNILib.menu();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        AirForceJNILib.suspend();
        glView.onPause();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        glView.onResume();
    }
}
