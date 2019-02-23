// to work around RUBE crash bug.

float myQueryNumericValue(string description, float defaultValue)
{
    body[] bs = sb();
    fixture[] fs = sf();
    vertex[] vs = sv();
    image[] ijs = si();
    deselect(bs);
    deselect(fs);
    deselect(vs);
    deselect(ijs);
    float val = queryNumericValue(description, defaultValue);
    select(bs);
    select(fs);
    select(vs);
    select(ijs);
    return val;
}

vec2 myQueryVectorValue(string description, float defaultX, float defaultY)
{
    body[] bs = sb();
    fixture[] fs = sf();
    vertex[] vs = sv();
    image[] ijs = si();
    deselect(bs);
    deselect(fs);
    deselect(vs);
    deselect(ijs);
    vec2 val = queryVectorValue(description, defaultX, defaultY);
    select(bs);
    select(fs);
    select(vs);
    select(ijs);
    return val;
}
