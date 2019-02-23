vertex[] vs = sv();
if (vs.length >= 1) {
    for (uint i = 0; i < vs.length; i++) {
        vertex v = vs[i];
        v.setPos(-v.pos.x, v.pos.y);
    }
}
else
    print("Please select at least one vertex");
