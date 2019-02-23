#include "query.rs"

void main()
{
    joint[] rj = getJoints("nodup_joint");
    body[] bs = sb();
    if (bs.length < 1) {
        print("Please select at least 1 body.");
        return;
    }
    deselect(bs);
    string[] names;
    body cbody;
    for (uint i = 0; i < bs.length; i++) {
        names.insertLast(bs[i].getName());
        if (bs[i].getName().findFirst("head") == 0) {
            cbody = bs[i];
        }
    }
    float[] d;
    float scl = 0.3;
    d.insertLast(-4.0 * scl);
    d.insertLast(-4.0 * scl);
    d.insertLast(-4.0 * scl);
    d.insertLast(-4.0 * scl);
    d.insertLast(1.0 * scl);
    d.insertLast(3.0 * scl);
    d.insertLast(3.0 * scl);
    float s = 1.0;
    body[] b1;
    body[] b2;
    for (uint i = 0; i < 7; i++) {
        body[] tmp;
        body[] tmp2 = duplicate(bs, true);
        for (uint j = 0; j < bs.length; j++) {
            for (uint k = 0; k < tmp2.length; k++) {
                if (bs[j].getName() == tmp2[k].getName()) {
                    tmp.insertLast(tmp2[k]);
                }
            }
        }
        joint[] jts;
        for (uint j = 0; j < tmp.length; j++) {
            tmp[j].setName(names[j] + "_" + (i + 1));
            joint[] tjts = tmp[j].getJoints();
            for (uint k = 0; k < tjts.length; k++) {
                if ((tjts[k].getName() != "nodup_joint") && (jts.find(tjts[k]) == -1)) {
                    jts.insertLast(tjts[k]);
                }
            }
        }
        for (uint j = 0; j < jts.length; j++) {
            string n = jts[j].getName();
            int idx = n.findFirst("_");
            if (idx != -1) {
                n = n.substr(0, idx);
            }
            jts[j].setName(n + "_" + (i + 1));
        }
        scale(tmp, 1, 1.1, 1.1);
        vec2 ttt;
        ttt.set(d[i] * s, 0.0);
        translate(tmp, ttt.rotatedBy(cbody.angle));
        s *= 1.2;
        for (uint j = 0; j < tmp.length; j++) {
            image[] imgs = tmp[j].getImages();
            for (uint k = 0; k < imgs.length; k++) {
                if (i == 4) {
                    imgs[k].setRenderOrder(imgs[k].getRenderOrder() + 243);
                } else {
                    imgs[k].setRenderOrder(imgs[k].getRenderOrder() + 4);
                }
            }
            fixture[] frs = tmp[j].getFixtures();
            for (uint k = 0; k < frs.length; k++) {
                if (tmp[j].getName().findFirst("head") == 0) {
                    if (i == 3) {
                        frs[k].setSensor(0);
                        frs[k].setFilterCategoryBits(16);
                        frs[k].setFilterMaskBits(65519);
                        frs[k].setFilterGroupIndex(0);
                    } else {
                        frs[k].setSensor(1);
                        frs[k].setFilterCategoryBits(0);
                        frs[k].setFilterMaskBits(0);
                        frs[k].setFilterGroupIndex(0);
                    }
                }
                frs[k].setDensity(frs[k].getDensity() / 1.2);
            }
            if ((i >= 4) && (tmp[j].getName().findFirst("head") != 0)) {
                body kb = duplicate(tmp[j], false);
                kb.setType(1);
                kb.setName(kb.getName() + "s");
                kb.setLinearDamping(0);
                kb.setAngularDamping(0);
                delete(kb.getImages());
                fixture[] frs = kb.getFixtures();
                for (uint k = 0; k < frs.length; k++) {
                    frs[k].setFilterCategoryBits(16);
                    frs[k].setFilterMaskBits(2);
                }
            }
        }
        for (uint j = 0; j < bs.length; j++) {
            if (bs[j].getName().findFirst("head") == 0) {
                b1.insertLast(bs[j]);
                break;
            }
        }
        for (uint j = 0; j < tmp.length; j++) {
            if (tmp[j].getName().findFirst("head") == 0) {
                b2.insertLast(tmp[j]);

                image[] imgs = tmp[j].getImages();
                for (uint k = 0; k < imgs.length; k++) {
                    if (imgs[k].getName() == "body") {
                        imgs[k].setAngle(rnd() * dr(360));
                    }
                }

                break;
            }
        }
        bs = tmp;
    }
    float ln = 1.5;
    for (uint i = 0; i < 7; i++) {
        vec2 anchorA = b1[i].getLocalPoint(b2[i].pos);
        addJoint(-1, '{"type":"motor","bodyA":'+b1[i].id+',"bodyB":'+b2[i].id+',"anchorA":{"x":'+anchorA.x+',"y":'+anchorA.y+'},"maxTorque":'+800+',"maxForce":'+10+',"correctionFactor":'+0.1+'}}');
        anchorA.set(0, 0);
        ln *= 1.05;
        addJoint(-1, '{"type":"rope","bodyA":'+b1[i].id+',"bodyB":'+b2[i].id+',"anchorA":{"x":'+anchorA.x+',"y":'+anchorA.y+'},"anchorB":{"x":'+anchorA.x+',"y":'+anchorA.y+'},"maxLength":'+ln+'}');
    }
    joint[] rj2 = getJoints("nodup_joint");
    for (uint i = 0; i < rj2.length; i++) {
        int fnd = 0;
        for (uint j = 0; j < rj.length; j++) {
            if (rj2[i] == rj[j]) {
                fnd = 1;
                break;
            }
        }
        if (fnd == 0) {
            rj2[i].delete();
        }
    }
}
