#include "query.rs"

void main()
{
    body[] bs = sb();

    if (bs.length != 1) {
        print("Must select exactly one body.");
        return;
    }

    deselect(bs);
    vec2 np = myQueryVectorValue("Enter new body position", 0, 0);
    select(bs);
    if ((np.x == 0) && (np.y == 0)) {
        return;
    }
    body b = bs[0];
    vec2 d = b.pos - np;
    translate(b.getFixtures(), d);
    translate(b.getImages(), d);
    b.setPos(np);
}
