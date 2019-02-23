#include "query.rs"

void main()
{
    float angle;

    string em = getCurrentEditMode();

    if (em == "BODY") {
        body[] bs = sb();

        if (bs.length < 1) {
            print("Must select at least one body.");
            return;
        }
        angle = dr(myQueryNumericValue("Rotate by:", 180));
        rotate(bs, 1, angle);
    } else if (em == "FIXTURE") {
        fixture[] fs = sf();

        if (fs.length < 1) {
            print("Must select at least one fixture.");
            return;
        }
        angle = dr(myQueryNumericValue("Rotate by:", 180));
        rotate(fs, 1, angle);
    } else if (em == "VERTEX") {
        vertex[] vs = sv();

        if (vs.length < 1) {
            print("Must select at least one vertex.");
            return;
        }
        angle = dr(myQueryNumericValue("Rotate by:", 180));
        rotate(vs, 1, angle);
    } else if (em == "IMAGE") {
        image[] ijs = si();

        if (ijs.length < 1) {
            print("Must select at least one image.");
            return;
        }
        angle = dr(myQueryNumericValue("Rotate by:", 180));
        rotate(ijs, 1, angle);
    } else {
        print("Unsupported edit mode.");
    }
}
