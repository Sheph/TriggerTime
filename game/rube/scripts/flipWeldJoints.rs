bool sortFunc(joint a, joint b) {
    return a.getBodyA().getCustomInt("segId", -1000) < b.getBodyA().getCustomInt("segId", -1000);
}

void main()
{
    joint[] js = sortJoints(sj(), sortFunc);

    if (js.length < 1) {
        print("Must select at least one joint.");
        return;
    }

    float[] refAngles(js.length);

    for (uint i = 0; i < js.length; i++) {
        joint j = js[i];
        if (js[i].getType() == 8) {
            refAngles[i] = j.getReferenceAngle();
            j.setReferenceAngle(j.getBodyB().angle - j.getBodyA().angle);
        }
    }

    for (uint i = 0; i < js.length; i++) {
        joint j = js[i];
        if (js[i].getType() == 8) {
            // welds
            vec2 tmp = (j.getBodyB().pos - j.getWorldAnchorB()).rotatedBy(j.getBodyA().angle + refAngles[i] - j.getBodyB().angle);
            tmp += j.getWorldAnchorA();
            j.getBodyB().setAngle(j.getBodyA().angle + refAngles[i]);
            j.getBodyB().setPos(tmp);
        }
    }
}
