void main()
{
    fixture[] fs = sf();

    if (fs.length < 1) {
        print("Must select at least one fixture.");
        return;
    }

    for (uint i = 0; i < fs.length; i++) {
        fixture f = fs[i];
        select(f.getVertices());
    }
}
