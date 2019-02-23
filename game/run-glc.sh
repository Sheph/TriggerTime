# 1. git clone git@github.com:lukeforeman/glc.git
# 2. cd ./glc
# 3. git checkout pulseaudio
# 4. git clone git@github.com:nullkey/glc-support.git support
# 5. cmake .
# 6. make
GLC_DIR="../../../glc/src"

rm ./airforce.glc
rm ./airforce.mkv
ALSOFT_DRIVERS=alsa LD_LIBRARY_PATH="$GLC_DIR/glc:$GLC_DIR/hook" $GLC_DIR/glc-capture --fps=60 --lock-fps -o ./airforce.glc ./tt

$GLC_DIR/glc-play ./airforce.glc -a 1 -o airforce.wav
$GLC_DIR/glc-play ./airforce.glc -o - -y 1 -g "0.02;0;1.25;1.25;1.25" | avconv -i airforce.wav -i - -preset ultrafast -vcodec libx264 -acodec flac airforce.mkv
rm ./airforce.wav
