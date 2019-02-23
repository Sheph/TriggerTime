bool shouldDisplayProperty(body b, const string& in n) {
    if (getCustomProperties("body").find(n) < 0) return true;
    if (b.getName() == "root") {
        if (n == "freezable" || n == "freezeRadius") return true;
    }
    if (n == "gravityGunAware" || n == "freezePhysics" || n == "collisionImpulseMultiplier" ||
        n == "detourable" || n == "segId") return true;
    return false;
}

bool shouldDisplayProperty(fixture f, const string& in n) {
    if (getCustomProperties("fixture").find(n) < 0) return true;
    if (n == "class") return true;
    if (f.getCustomString("class") == "tentacle") {
        if (f.getBody().getCustomInt("segId") != 0) return false;
        if (n != "image" &&
            n != "zorder" &&
            n != "color" && n != "width1" && n != "width2" &&
            n != "image_flip" && n != "num_iterations" &&
            n != "tension" && n != "step") return false;
    } else {
        return false;
    }
    return true;
}

bool shouldDisplayProperty(image i, const string& in n) {
    if (getCustomProperties("image").find(n) < 0) return true;
    if (i.getCustomString("peffect") != "") {
        if (n == "peffect") return true;
        return false;
    } else if (i.getCustomString("light") != "") {
        if (n == "light") return true;
        if (i.getCustomString("light") == "point") {
            if (n == "distance") return true;
        } else if (i.getCustomString("light") == "cone") {
            if (n == "distance" ||
                n == "nearDistance" ||
                n == "coneAngle") return true;
        } else if (i.getCustomString("light") == "line") {
            if (n == "length" ||
                n == "distance" ||
                n == "bothWays") return true;
        } else if (i.getCustomString("light") == "ring") {
            if (n == "distance" ||
                n == "nearDistance" ||
                n == "farDistance") return true;
        } else {
            return false;
        }
        if (n != "diffuse" && n != "numRays" &&
            n != "visible" && n != "softLength" &&
            n != "xray" && n != "dynamic" && n != "reverse" &&
            n != "intensity" && n != "nearOffset" && n != "hitBlockers") return false;
    } else {
        if (n == "peffect" || n == "light" || n == "dummy" || n == "visible" || n == "fixedPos") return true;
        return false;
    }
    return true;
}

void main() {}
