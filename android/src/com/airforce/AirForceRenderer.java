package com.airforce;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;

class AirForceRenderer implements GLSurfaceView.Renderer
{
    private AssetManager assetManager;
    private Activity activity;
    private Thread gameThread = null;
    private boolean stopped = false;

    public AirForceRenderer(Context context)
    {
        this.assetManager = context.getAssets();
        this.activity = (Activity)context;
    }

    public void stop()
    {
        stopped = true;

        if (gameThread != null)
        {
            AirForceJNILib.cancelStep();
            try
            {
                gameThread.join();
            }
            catch (InterruptedException e)
            {
                gameThread.interrupt();
            }
        }
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        if (!AirForceJNILib.init(assetManager))
        {
            throw new RuntimeException("Unable to initialize JNI");
        }

        if (gameThread != null)
        {
            if (!AirForceJNILib.resize(0, 0))
            {
                throw new RuntimeException("Unable to initialize game");
            }
        }
    }

    public void onSurfaceChanged(GL10 gl, int w, int h)
    {
        if (gameThread == null)
        {
            if (!AirForceJNILib.resize(w, h))
            {
                throw new RuntimeException("Unable to initialize game");
            }

            gameThread = new Thread(new Runnable()
            {
                @Override
                public void run()
                {
                    while (!stopped)
                    {
                        if (!AirForceJNILib.step())
                        {
                            activity.finish();
                        }
                    }
                }
            });
            gameThread.setName("game");
            gameThread.start();
        }
    }

    public void onDrawFrame(GL10 gl)
    {
        AirForceJNILib.render();
    }
}
