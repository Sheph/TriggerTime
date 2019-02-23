set CUR_DIR=%CD%
cd ..\game\res
java -cp %CUR_DIR%\gdx.jar;%CUR_DIR%\gdx-natives.jar;%CUR_DIR%\gdx-backend-lwjgl.jar;%CUR_DIR%\gdx-backend-lwjgl-natives.jar;%CUR_DIR%\gdx-tools.jar com.badlogic.gdx.tools.particleeditor.ParticleEditor
