CD="$PWD"
cd ../game/res
java -cp $CD/gdx.jar:$CD/gdx-natives.jar:$CD/gdx-backend-lwjgl.jar:$CD/gdx-backend-lwjgl-natives.jar:$CD/gdx-tools.jar com.badlogic.gdx.tools.particleeditor.ParticleEditor
