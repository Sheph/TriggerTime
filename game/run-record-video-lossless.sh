apitrace trace -o ./airforce.trace ./tt
glretrace -s - airforce.trace | \
avconv -y -r 60 -f image2pipe -vcodec ppm -i pipe: -c:v libx264 -preset ultrafast -qp 0 -f mp4 airforce.mp4
