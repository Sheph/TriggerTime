apitrace trace -o ./airforce.trace ./tt
glretrace -s - airforce.trace | \
avconv -y -r 60 -f image2pipe -vcodec ppm -i pipe: -c:v libx264 -preset ultrafast -b:v 16000k -pass 1 -f mp4 /dev/null && \
glretrace -s - airforce.trace | \
avconv -r 60 -f image2pipe -vcodec ppm -i pipe: -c:v libx264 -preset ultrafast -b:v 16000k -pass 2 -f mp4 airforce.mp4
