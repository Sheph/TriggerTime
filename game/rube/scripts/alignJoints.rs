joint[] js = sj();

if (js.length < 1) {
    print("Must select at least one joint.");
    return;
}

for (uint i = 0; i < js.length; i++) {
    joint j = js[i];
    if (js[i].getType() == 2) {
        // prismatics
        vec2 v;
        v.set(cos(j.getBodyB().angle), sin(j.getBodyB().angle));
        j.setLocalAxis(v);
    }
}
