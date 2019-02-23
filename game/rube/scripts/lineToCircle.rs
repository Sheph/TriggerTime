fixture[] fs = sf();

if (fs.length != 1 || fs[0].getShape(0).type != 3) {
    print("Must select exactly one loop fixture.");
    return;
}

vec2 lower = fs[0].getVertex(0).pos;
vec2 upper = lower;
for (int i = 1; i < fs[0].getNumVertices(); i++) {
    vec2 v = fs[0].getVertex(i).pos;
    if (v.x < lower.x) { lower.x = v.x; }
    if (v.y < lower.y) { lower.y = v.y; }
    if (v.x > upper.x) { upper.x = v.x; }
    if (v.y > upper.y) { upper.y = v.y; }
}

vec2 center = (lower + upper) / 2;
vec2 ext = (upper - lower) / 2;
float radius = ext.x;
if (ext.y > ext.x) radius = ext.y;

fs[0].addVertex(fs[0].getNumVertices(), center);
fs[0].getShape(0).setRadius(radius);
fs[0].getShape(0).setType(0);

for (int i = fs[0].getNumVertices() - 2; i >= 0; i--) {
    fs[0].deleteVertex(0);
}
