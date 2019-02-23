#include "query.rs"

void main()
{
    fixture[] fs = sf();

    if (fs.length != 1 || fs[0].getShape(0).type != 0) {
        print("Must select exactly one circle fixture.");
        return;
    }

    float radius = fs[0].getShape(0).getRadius();
    vec2 center = fs[0].getVertex(0).pos;
    int numSectors = myQueryNumericValue("Number of sectors:", 40);
    for (int i = 0; i < numSectors; i++) {
        float a = i * dr(360) / numSectors;
        vec2 v;
        v.set(radius * cos(a), radius * sin(a));
        v += center;
        fs[0].addVertex(i + 1, v);
    }
    fs[0].deleteVertex(0);
    fs[0].getShape(0).setType(3);
    fs[0].getShape(0).setRadius(0);
}
