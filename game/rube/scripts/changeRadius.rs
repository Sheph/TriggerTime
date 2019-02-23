#include "query.rs"

void main()
{
    fixture[] fs = sf();

    if (fs.length < 1) {
        print("Must select at least one fixture.");
        return;
    }

    float r = myQueryNumericValue("new radius:", 1);

    for (uint i = 0; i < fs.length; i++) {
        fs[i].getShapes()[0].setRadius(r);
    }
}
