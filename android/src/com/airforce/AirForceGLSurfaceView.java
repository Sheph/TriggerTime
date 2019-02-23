package com.airforce;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.util.Log;

class AirForceGLSurfaceView extends GLSurfaceView
{
    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
    {
        protected final int[] mConfigAttribs;
        private int[] mValue = new int[1];

        public ConfigChooser()
        {
            mConfigAttribs = new int[]
            {
                EGL10.EGL_RED_SIZE, 4,
                EGL10.EGL_GREEN_SIZE, 4,
                EGL10.EGL_BLUE_SIZE, 4,
                EGL10.EGL_RENDERABLE_TYPE, 4 /*EGL_OPENGL_ES2_BIT*/,
                EGL10.EGL_NONE
            };
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
        {
            // get (almost) all configs available by using r=g=b=4 so we
            // can chose with big confidence
            int[] num_config = new int[1];
            egl.eglChooseConfig(display, mConfigAttribs, null, 0, num_config);
            int numConfigs = num_config[0];

            if (numConfigs <= 0)
            {
                throw new IllegalArgumentException("No configs match configSpec");
            }

            // now actually read the configurations.
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, mConfigAttribs, configs, numConfigs, num_config);

            // chose the best one, taking into account multi sampling.
            EGLConfig config = chooseConfig(egl, display, configs);

            return config;
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs)
        {
            EGLConfig best = null;
            EGLConfig safe = null;
            int bestSampleBuffers = 0;
            int bestNumSamples = 0;

            for (EGLConfig config : configs)
            {
                int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

                // Match RGB565 as a fallback
                if (safe == null && r == 5 && g == 6 && b == 5)
                {
                    safe = config;
                }

                // now check for MSAA support
                int hasSampleBuffers = findConfigAttrib(egl, display, config, EGL10.EGL_SAMPLE_BUFFERS, 0);
                int numSamples = findConfigAttrib(egl, display, config, EGL10.EGL_SAMPLES, 0);

                //FIXME: MSAA 2 is too slow on some devices, make it optional.
                if (hasSampleBuffers >= bestSampleBuffers &&
                    numSamples > bestNumSamples && numSamples <= 1)
                {
                    best = config;
                    bestSampleBuffers = hasSampleBuffers;
                    bestNumSamples = numSamples;
                }
            }

            if (best != null)
                return best;
            else
                return safe;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue)
        {
            if (egl.eglGetConfigAttrib(display, config, attribute, mValue))
            {
                return mValue[0];
            }

            return defaultValue;
        }
    }

    public AirForceGLSurfaceView(Context context)
    {
        super(context);
        setEGLContextClientVersion(2);
        // we want to have custom config chooser here because
        // some devices (like Galaxy S3) seem to render things much
        // smoother with MSAA enabled. With MSAA disabled we observe some
        // rendering lags that are due to android not giving us enough
        // CPU time to render at 60fps...
        setEGLConfigChooser(new ConfigChooser());
        airForceRenderer = new AirForceRenderer(context);
        setRenderer(airForceRenderer);
    }

    public boolean onTouchEvent(final MotionEvent event)
    {
        final int action = event.getAction();
        final int actualEvent = action & MotionEvent.ACTION_MASK;

        if (actualEvent == MotionEvent.ACTION_POINTER_UP)
        {
            int i = event.getActionIndex();

            AirForceJNILib.input(
                    event.getPointerId(i),
                    event.getX(i),
                    event.getY(i),
                    true);
        }
        else if (actualEvent == MotionEvent.ACTION_UP)
        {
            AirForceJNILib.input(
                    event.getPointerId(0),
                    event.getX(0),
                    event.getY(0),
                    true);
        }
        else if (actualEvent != MotionEvent.ACTION_CANCEL)
        {
            for (int i = 0; i < event.getPointerCount(); ++i)
            {
                AirForceJNILib.input(
                    event.getPointerId(i),
                    event.getX(i),
                    event.getY(i),
                    false);
            }
        }

        return true;
    }

    public void stop()
    {
        airForceRenderer.stop();
    }

    AirForceRenderer airForceRenderer;
}
