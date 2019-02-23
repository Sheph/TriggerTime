bool shouldDisplayProperty(body b, const string& in n) {
    if (getCustomProperties("body").find(n) < 0) return true;
    if (n == "class") return true;
    if (b.getCustomString("class") == "weapon_station") {
        if (n == "weaponType" || n == "minAmmo") return true;
    } else if (b.getCustomString("class") == "turret1") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "pyrobot") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "laser_blocker") {
        if (n == "width" || n == "length" || n == "zorder" || n == "color" ||
            n == "blockPlayer" || n == "blockAlly" || n == "blockEnemy" ||
            n == "blockRock") return true;
    } else if (b.getCustomString("class") == "powerup_inventory") {
        if (n == "inventoryItem") return true;
    } else if (b.getCustomString("class") == "placeholder" || b.getCustomString("class") == "ally_placeholder") {
        if (n == "inventoryItem" || n == "height" ||
            n == "radius" || n == "num_sectors" || n == "image_flip" ||
            n == "image_zorder" || n == "circle_zorder" || n == "image" || n == "alpha") return true;
    } else if (b.getCustomString("class") == "rock_placeholder") {
        if (n == "height" ||
            n == "radius" || n == "num_sectors" || n == "image_flip" ||
            n == "image_zorder" || n == "circle_zorder" || n == "image" || n == "alpha") return true;
    } else if (b.getCustomString("class") == "vehicle_placeholder") {
        if (n == "radius" || n == "num_sectors" ||
            n == "zorder" || n == "alpha") return true;
    } else if (b.getCustomString("class") == "instance") {
        if (n == "assetPath") return true;
    } else if (b.getCustomString("class") == "spider_nest") {
        if (n == "probability1" || n == "autoTarget" || n == "dead") return true;
    } else if (b.getCustomString("class") == "weapon_trap") {
        if (n == "weaponType" || n == "missileType") return true;
    } else if (b.getCustomString("class") == "sentry1" ||
               b.getCustomString("class") == "sentry2") {
        if (n == "folded") return true;
    } else if (b.getCustomString("class") == "powerup_ammo") {
        if (n == "weaponType") return true;
    } else if (b.getCustomString("class") == "crate1" ||
               b.getCustomString("class") == "crate2" ||
               b.getCustomString("class") == "crate3" ||
               b.getCustomString("class") == "crate4") {
        if (n == "prob_health" || n == "prob_gem" || n == "prob_ammo" ||
            n == "prob_ammo_proxmine" || n == "prob_ammo_rlauncher" ||
            n == "prob_ammo_machineGun" || n == "prob_ammo_plasmagun" ||
            n == "prob_ammo_shotgun" || n == "prob_ammo_eshield") return true;
    } else if (b.getCustomString("class") == "powerup_weapon") {
        if (n == "weaponType" || n == "minAmmo") return true;
    } else if (b.getCustomString("class") == "keeper") {
        if (n == "folded" || n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "health_station") {
        if (n == "amount") return true;
    } else if (b.getCustomString("class") == "garbage") {
        if (n == "cancelCollisions") return true;
    } else if (b.getCustomString("class") == "acidworm1") {
        if (n == "folded" || n == "foldable" || n == "autoTarget" || n == "unfoldRadius") return true;
    } else if (b.getCustomString("class") == "beetle1") {
        if (n == "shoot" || n == "split" || n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "beetle2") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "centipede1") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "enforcer1") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "enforcer2") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "powergen") {
        if (n == "rusted") return true;
    } else if (b.getCustomString("class") == "powergen2") {
        if (n == "rusted") return true;
    } else if (b.getCustomString("class") == "baby_spider1") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "baby_spider2") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "gorger1") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "homer") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "guardian") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "guardian2") {
        if (n == "autoTarget") return true;
    } else if (b.getCustomString("class") == "creature") {
        if (n == "i") return true;
    } else if (b.getCustomString("class") == "sawer") {
        if (n == "length" || n == "fixed" || n == "autoTarget") return true;
    }
    if (n == "gravityGunAware" || n == "visible" ||
        n == "freezable" || n == "freezeRadius" || n == "glassy" || n == "freezePhysics" || n == "segId" ||
        n == "collisionImpulseMultiplier" || n == "detourable") return true;
    return false;
}

bool shouldDisplayProperty(fixture f, const string& in n) {
    if (getCustomProperties("fixture").find(n) < 0) return true;
    if (f.getBody().getCustomString("class") == "rock" ||
        f.getBody().getCustomString("class") == "terrain" ||
        f.getBody().getCustomString("class") == "blocker2" ||
        f.getBody().getCustomString("class") == "garbage" ||
        f.getBody().getCustomString("class") == "enemy_building") {
        if (n == "class") return true;
        if (f.getCustomString("class") == "terrain_points") {
            if (n == "image") return true;
            if (f.getCustomString("image") == "") return false;
            if ((n == "stencilFail" || n == "stencilPass") &&
                f.getCustomBool("stencilSet")) return true;
            if ((n == "stencilFunc" || n == "stencilRef") &&
                f.getCustomBool("stencilUse")) return true;
            if (n != "image" && n != "image_width" &&
                n != "image_height" && n != "zorder" &&
                n != "stencilSet" && n != "stencilUse" && n != "color") return false;
        } else if (f.getCustomString("class") == "terrain_hole") {
            return false;
        } else if (f.getCustomString("class") == "chain") {
            if (n != "first_point" && n != "tension" &&
                n != "step" && n != "height1" &&
                n != "height2" && n != "angle1" &&
                n != "angle2" && n != "image" &&
                n != "zorder") return false;
        } else if (f.getCustomString("class") == "stripe") {
            if (n != "first_polygon" && n != "base" &&
                n != "image" && n != "image_width" &&
                n != "zorder" && n != "huge") return false;
        } else if (f.getCustomString("class") == "hermite_stripe") {
            if (n != "first_point" && n != "tension" &&
                n != "step" && n != "base" &&
                n != "image" &&
                n != "zorder" && n != "color" && n != "huge") return false;
        } else if (f.getCustomString("class") == "polygon") {
            if (n == "image") return true;
            if (f.getCustomString("image") == "") return false;
            if (f.getShape(0).type == 0 &&
                n == "num_sectors") return true;
            if (n != "image" && n != "image_width" &&
                n != "image_height" &&
                n != "zorder" && n != "color") return false;
        } else if (f.getCustomString("class") == "line_stripe") {
            if (n != "first_point" &&
                n != "base" &&
                n != "image" &&
                n != "zorder" && n != "color" && n != "huge") return false;
        } else if (f.getCustomString("class") == "proj") {
            if (n != "image" &&
                n != "zorder" &&
                n != "color" &&
                n != "image_fixedPos" && n != "image_flip" &&
                n != "image_visible" && n != "first_point") return false;
        } else if (f.getCustomString("class") == "tentacle") {
            if (n != "image" &&
                n != "zorder" &&
                n != "color" && n != "width1" && n != "width2" &&
                n != "image_flip" && n != "num_iterations" &&
                n != "tension" && n != "step") return false;
        } else {
            return false;
        }
    } else if (f.getBody().getCustomString("class") == "hermite_path") {
        if (n != "first_point" && n != "tension" && n != "num_iterations") return false;
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
        if (n == "peffect" || n == "light" || n == "mask" || n == "visible" || n == "fixedPos" || n == "dummy") return true;
        return false;
    }
    return true;
}

void main() {}
