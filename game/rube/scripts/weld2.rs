vertex[] v = sv();
fixture[] f = sf();
if ((v.length >= 2) && (f.length == 1)) {
    vec2 tmp;
    for (uint i = 0; i < v.length; i++) {
        if (v[i].getFixture() == f[0]) {
            tmp = v[i].wpos;
            break;
        }
    }
    for (uint i = 0; i < v.length; i++) {
        if (v[i].wpos != tmp) {
            v[i].setWorldPos(tmp);
        }
    }
}
else
    print("Please select two+ vertices and one fixture");
