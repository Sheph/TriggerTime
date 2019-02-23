body[] bs = sb();

if (bs.length != 1) {
    print("Must select exactly one body.");
    return;
}

body b = bs[0];
vec2 d = b.pos - b.getWorldCenter();
translate(b.getFixtures(), d);
translate(b.getImages(), d);
