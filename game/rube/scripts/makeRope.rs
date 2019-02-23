#include "query.rs"

bool sortFunc(body a, body b) {
    return a.getCustomInt("segId") < b.getCustomInt("segId");
}

void main()
{
    body[] bs = sortBodies(sb(), sortFunc);
    if ( bs.length < 3 ) {
        print("Please select >= 3 bodies.");
        return;
    }
    float sw = myQueryNumericValue("Segment width:", 1.0);
    for (uint i = 1; i < bs.length; i++) {
        vec2 anchorA;
        anchorA.set(-sw/2.0, 0.0);
        vec2 anchorB;
        anchorB.set(sw/2.0, 0.0);
        addJoint(-1, '{"type":"revolute","bodyA":'+bs[i-1].id+',"bodyB":'+bs[i].id+',"anchorA":{"x":'+anchorA.x+',"y":'+anchorA.y+'},"anchorB":{"x":'+anchorB.x+',"y":'+anchorB.y+'}}');
    }
    for (uint i = 1; i < bs.length; i++) {
        vec2 anchorA;
        vec2 anchorB;
        anchorA.set(0, 0);
        anchorB.set(0, 0);
        float length = sw*i;
        addJoint(-1, '{"type":"rope","bodyA":'+bs[0].id+',"bodyB":'+bs[i].id+',"anchorA":{"x":'+anchorA.x+',"y":'+anchorA.y+'},"anchorB":{"x":'+anchorB.x+',"y":'+anchorB.y+'},"maxLength":'+length+'}');
        addJoint(-1, '{"type":"rope","bodyA":'+bs[bs.length - 1].id+',"bodyB":'+bs[bs.length - i - 1].id+',"anchorA":{"x":'+anchorA.x+',"y":'+anchorA.y+'},"anchorB":{"x":'+anchorB.x+',"y":'+anchorB.y+'},"maxLength":'+length+'}');
        addJoint(-1, '{"type":"rope","bodyA":'+bs[i - 1].id+',"bodyB":'+bs[i].id+',"anchorA":{"x":'+anchorA.x+',"y":'+anchorA.y+'},"anchorB":{"x":'+anchorB.x+',"y":'+anchorB.y+'},"maxLength":'+sw+'}');
    }
}
