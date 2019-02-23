joint[] js = sj();

if (js.length < 1) {
    print("Must select at least one joint.");
    return;
}

for (uint i = 0; i < js.length; i++) {
    joint j = js[i];
    if (js[i].getType() == 1) {
        // revolutes
        j.setReferenceAngle(j.getBodyB().angle - j.getBodyA().angle);
        vec2 ab = j.getWorldAnchorB();
        ab = j.getBodyA().getLocalPoint(ab);
        j.setLocalAnchorA(ab);
    } else if (js[i].getType() == 2) {
        // prismatics
        j.setReferenceAngle(j.getBodyB().angle - j.getBodyA().angle);
        vec2 pos = j.getBodyB().pos;
        vec2 lpos = j.getBodyA().getLocalPoint(pos);
        j.setLocalAnchorA(lpos);
        lpos = j.getBodyB().getLocalPoint(pos);
        j.setLocalAnchorB(lpos);
    } else if (js[i].getType() == 8) {
        // welds
        body b = j.getBodyB();
        j.setReferenceAngle(j.getBodyB().angle - j.getBodyA().angle);
        vec2 ab = j.getWorldAnchorB();
        ab = j.getBodyA().getLocalPoint(ab);
        j.setLocalAnchorA(ab);
    } else if (js[i].getType() == 11) {
        // motors
        body b = j.getBodyB();
        j.setReferenceAngle(j.getBodyB().angle - j.getBodyA().angle);
        vec2 ab = j.getWorldAnchorB();
        ab = j.getBodyA().getLocalPoint(ab);
        j.setLocalAnchorA(ab);
    }
}
