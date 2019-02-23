vertex[] v = sv();
fixture[] f = sf();
if ((v.length == 1) && (f.length == 1)) {
    f[0].setVertices(v[0].getFixture().getVertices());
}
else
    print("Please select one vertex and one fixture");
