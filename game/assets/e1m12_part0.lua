local myUp = 1;
local myRight = 2;
local myDown = 3;
local myLeft = 4;

local myLongUp1 = 1;
local myLongUp2 = 2;
local myLongRight = 3;
local myLongDown1 = 4;
local myLongDown2 = 5;
local myLongLeft = 6;

local myBigUp1 = 1;
local myBigUp2 = 2;
local myBigRight1 = 3;
local myBigRight2 = 4;
local myBigDown1 = 5;
local myBigDown2 = 6;
local myBigLeft1 = 7;
local myBigLeft2 = 8;

local maze = {};
local mazeStack = {};
local mazeUsed = {};
local mazeOpenEnds = {};
local mazeKeys = {};
local mazeInstances = {};

local blocks = {};
local longHorzBlocks = {};
local bigBlocks = {};

local function sidesToIndex(sides)
    local idx = 0;
    local p = 1;
    for i = 1, #sides, 1 do
        idx = idx + sides[i] * p;
        p = p * 2;
    end
    return idx;
end

local function is3way(y, x)
    return (maze[y - 1][x][myDown] + maze[y + 1][x][myUp] + maze[y][x - 1][myRight] + maze[y][x + 1][myLeft]) >= 3;
end

local function sidesShift(sides)
    local nSides = sides;
    local tmp = nSides[#sides];
    for i = #sides - 1, 1, -1 do
        nSides[i + 1] = nSides[i];
    end
    nSides[1] = tmp;
    return nSides;
end

local function mazeFillRatio()
    local n = 0;
    for i = 2, 8, 1 do
        for j = 2, 8, 1 do
            if sidesToIndex(maze[i][j]) ~= 0 then
                n = n + 1;
            end
        end
    end
    return n / 49;
end

local function indexToPos(y, x)
    return vec2((x - 1) * 50, -(y - 5) * 50);
end

local function numKeys(...)
    local indices = {...};
    local num = 0;
    for _, v in pairs(indices) do
        if mazeKeys[v[1]][v[2]] ~= -1 then
            num = num + 1;
        end
    end
    return num;
end

local function getKey(...)
    local indices = {...};
    local num = 0;
    for _, v in pairs(indices) do
        if mazeKeys[v[1]][v[2]] ~= -1 then
            return mazeKeys[v[1]][v[2]];
        end
    end
    return -1;
end

local function addBlock(name, sides)
    local aSides = {0, 0, 0, 0};
    for _, v in pairs(sides) do
       aSides[v] = 1;
    end
    table.insert(blocks[sidesToIndex(aSides)], {name, 0});
    aSides = sidesShift(aSides);
    table.insert(blocks[sidesToIndex(aSides)], {name, -math.pi / 2});
    aSides = sidesShift(aSides);
    table.insert(blocks[sidesToIndex(aSides)], {name, -math.pi});
    aSides = sidesShift(aSides);
    table.insert(blocks[sidesToIndex(aSides)], {name, -3 * math.pi / 2});
end

local function addLongBlock(name, sides)
    local aSides = {0, 0, 0, 0, 0, 0};
    for _, v in pairs(sides) do
       aSides[v] = 1;
    end
    table.insert(longHorzBlocks[sidesToIndex(aSides)], {name, 0});
    aSides = sidesShift(aSides);
    aSides = sidesShift(aSides);
    aSides = sidesShift(aSides);
    table.insert(longHorzBlocks[sidesToIndex(aSides)], {name, -math.pi});
end

local function addBigBlock(name, sides)
    local aSides = {0, 0, 0, 0, 0, 0, 0, 0};
    for _, v in pairs(sides) do
       aSides[v] = 1;
    end
    table.insert(bigBlocks[sidesToIndex(aSides)], {name, 0});
    aSides = sidesShift(aSides);
    aSides = sidesShift(aSides);
    table.insert(bigBlocks[sidesToIndex(aSides)], {name, -math.pi / 2});
    aSides = sidesShift(aSides);
    aSides = sidesShift(aSides);
    table.insert(bigBlocks[sidesToIndex(aSides)], {name, -math.pi});
    aSides = sidesShift(aSides);
    aSides = sidesShift(aSides);
    table.insert(bigBlocks[sidesToIndex(aSides)], {name, -3 * math.pi / 2});
end

local function generateMaze(y, x)
    local function setSides(my, mx, sides)
        local numWays = 0;
        for _, v in pairs(sides) do
            if v == 1 then
                numWays = numWays + 1;
            end
        end
        if (numWays < 3) then
            local fr = mazeFillRatio();
            if fr > 0.5 then
                numWays = 0;
                table.insert(mazeOpenEnds, {my, mx});
            elseif numWays == 1 then
                if (fr <= 0.25) and (math.random(1, 3) == 1) then
                    numWays = 1;
                elseif (fr > 0.25) and (math.random(1, 2) == 1) then
                    numWays = 1;
                else
                    numWays = 2;
                end
            else
                numWays = 1;
            end
            local dirs = {1, 2, 3, 4};
            table.shuffle(dirs);
            for _, dir in pairs(dirs) do
                if numWays == 0 then
                    break;
                end
                if sides[dir] == -1 then
                    sides[dir] = 1;
                    numWays = numWays - 1;
                end
            end
        end
        local dirs = {1, 2, 3, 4};
        for _, dir in pairs(dirs) do
            if sides[dir] == -1 then
                sides[dir] = 0;
            end
        end
        maze[my][mx] = sides;
        table.insert(mazeStack, {my, mx});
    end
    local dirs = {1, 2, 3, 4};
    table.shuffle(dirs);
    for _, dir in pairs(dirs) do
        if (maze[y][x][dir] == 1) then
            local sides = {-1, -1, -1, -1};
            if (dir == myUp) then
                if (sidesToIndex(maze[y - 1][x]) == 0) then
                    sides[myDown] = 1;
                    if (x == 2) or is3way(y - 1, x - 1) then
                        sides[myLeft] = 0;
                    elseif (sidesToIndex(maze[y - 1][x - 1]) ~= 0) then
                        sides[myLeft] = maze[y - 1][x - 1][myRight];
                    end
                    if (x == 8) or is3way(y - 1, x + 1) then
                        sides[myRight] = 0;
                    elseif (sidesToIndex(maze[y - 1][x + 1]) ~= 0) then
                        sides[myRight] = maze[y - 1][x + 1][myLeft];
                    end
                    if ((y - 1) == 2) or is3way(y - 2, x) then
                        sides[myUp] = 0;
                    elseif (sidesToIndex(maze[y - 2][x]) ~= 0) then
                        sides[myUp] = maze[y - 2][x][myDown];
                    end
                    setSides(y - 1, x, sides);
                end
            elseif (dir == myRight) then
                if (sidesToIndex(maze[y][x + 1]) == 0) then
                    sides[myLeft] = 1;
                    if (y == 2) or is3way(y - 1, x + 1) then
                        sides[myUp] = 0;
                    elseif (sidesToIndex(maze[y - 1][x + 1]) ~= 0) then
                        sides[myUp] = maze[y - 1][x + 1][myDown];
                    end
                    if ((x + 1) == 8) or is3way(y, x + 2) then
                        sides[myRight] = 0;
                    elseif (sidesToIndex(maze[y][x + 2]) ~= 0) then
                        sides[myRight] = maze[y][x + 2][myLeft];
                    end
                    if (y == 8) or is3way(y + 1, x + 1) then
                        sides[myDown] = 0;
                    elseif (sidesToIndex(maze[y + 1][x + 1]) ~= 0) then
                        sides[myDown] = maze[y + 1][x + 1][myUp];
                    end
                    setSides(y, x + 1, sides);
                end
            elseif (dir == myDown) then
                if (sidesToIndex(maze[y + 1][x]) == 0) then
                    sides[myUp] = 1;
                    if (x == 2) or is3way(y + 1, x - 1) then
                        sides[myLeft] = 0;
                    elseif (sidesToIndex(maze[y + 1][x - 1]) ~= 0) then
                        sides[myLeft] = maze[y + 1][x - 1][myRight];
                    end
                    if ((y + 1) == 8) or is3way(y + 2, x) then
                        sides[myDown] = 0;
                    elseif (sidesToIndex(maze[y + 2][x]) ~= 0) then
                        sides[myDown] = maze[y + 2][x][myUp];
                    end
                    if (x == 8) or is3way(y + 1, x + 1) then
                        sides[myRight] = 0;
                    elseif (sidesToIndex(maze[y + 1][x + 1]) ~= 0) then
                        sides[myRight] = maze[y + 1][x + 1][myLeft];
                    end
                    setSides(y + 1, x, sides);
                end
            else
                if (sidesToIndex(maze[y][x - 1]) == 0) then
                    sides[myRight] = 1;
                    if (y == 2) or is3way(y - 1, x - 1) then
                        sides[myUp] = 0;
                    elseif (sidesToIndex(maze[y - 1][x - 1]) ~= 0) then
                        sides[myUp] = maze[y - 1][x - 1][myDown];
                    end
                    if ((x - 1) == 2) or is3way(y, x - 2) then
                        sides[myLeft] = 0;
                    elseif (sidesToIndex(maze[y][x - 2]) ~= 0) then
                        sides[myLeft] = maze[y][x - 2][myRight];
                    end
                    if (y == 8) or is3way(y + 1, x - 1) then
                        sides[myDown] = 0;
                    elseif (sidesToIndex(maze[y + 1][x - 1]) ~= 0) then
                        sides[myDown] = maze[y + 1][x - 1][myUp];
                    end
                    setSides(y, x - 1, sides);
                end
            end
        end
    end
end

local function generateMazeFromStack()
    while #mazeStack > 0 do
        local tmp = mazeStack;
        mazeStack = {};
        table.shuffle(tmp);
        for _, v in pairs(tmp) do
            generateMaze(v[1], v[2]);
        end
    end
end

local function makeMazeBossBlock()
    local function isfr(my, mx)
        if (mx < 1) or (mx > 9) or (my < 1) or (my > 9) then
            return true;
        end
        return sidesToIndex(maze[my][mx]) == 0;
    end
    local function mark(my, mx)
        if (mx < 1) or (mx > 9) or (my < 1) or (my > 9) then
            return;
        end
        mazeUsed[my][mx] = true;
    end

    local done = false;
    local brXY = {0, 0};
    local brPos = nil;
    local brAngle = nil;

    table.shuffle(mazeOpenEnds);
    for k, v in pairs(mazeOpenEnds) do
        local y = v[1];
        local x = v[2];
        local dirs = {1, 2, 3, 4};
        table.shuffle(dirs);
        for _, dir in pairs(dirs) do
            if maze[y][x][dir] == 0 then
                if (dir == myUp) then
                    if isfr(y - 1, x) and isfr(y - 1, x - 1) and isfr(y - 1, x + 1) and
                       isfr(y - 2, x) and isfr(y - 2, x - 1) and isfr(y - 2, x + 1) and
                       isfr(y - 3, x) and isfr(y - 3, x - 1) and isfr(y - 3, x + 1) then
                       mark(y - 1, x); mark(y - 1, x - 1); mark(y - 1, x + 1);
                       mark(y - 2, x); mark(y - 2, x - 1); mark(y - 2, x + 1);
                       mark(y - 3, x); mark(y - 3, x - 1); mark(y - 3, x + 1);
                       done = true;
                       maze[y][x][dir] = 1;
                       maze[y - 1][x][myDown] = 1;
                       mazeOpenEnds[k] = nil;
                       brXY = {y - 1, x};
                       brPos = indexToPos(y - 1, x);
                       brAngle = math.pi / 2;
                       break;
                    end
                elseif (dir == myRight) then
                    if isfr(y, x + 1) and isfr(y - 1, x + 1) and isfr(y + 1, x + 1) and
                       isfr(y, x + 2) and isfr(y - 1, x + 2) and isfr(y + 1, x + 2) and
                       isfr(y, x + 3) and isfr(y - 1, x + 3) and isfr(y + 1, x + 3) then
                       mark(y, x + 1); mark(y - 1, x + 1); mark(y + 1, x + 1);
                       mark(y, x + 2); mark(y - 1, x + 2); mark(y + 1, x + 2);
                       mark(y, x + 3); mark(y - 1, x + 3); mark(y + 1, x + 3);
                       done = true;
                       maze[y][x][dir] = 1;
                       maze[y][x + 1][myLeft] = 1;
                       mazeOpenEnds[k] = nil;
                       brXY = {y, x + 1};
                       brPos = indexToPos(y, x + 1);
                       brAngle = 0;
                       break;
                    end
                elseif (dir == myDown) then
                    if isfr(y + 1, x) and isfr(y + 1, x + 1) and isfr(y + 1, x - 1) and
                       isfr(y + 2, x) and isfr(y + 2, x + 1) and isfr(y + 2, x - 1) and
                       isfr(y + 3, x) and isfr(y + 3, x + 1) and isfr(y + 3, x - 1) then
                       mark(y + 1, x); mark(y + 1, x + 1); mark(y + 1, x - 1);
                       mark(y + 2, x); mark(y + 2, x + 1); mark(y + 2, x - 1);
                       mark(y + 3, x); mark(y + 3, x + 1); mark(y + 3, x - 1);
                       done = true;
                       maze[y][x][dir] = 1;
                       maze[y + 1][x][myUp] = 1;
                       mazeOpenEnds[k] = nil;
                       brXY = {y + 1, x};
                       brPos = indexToPos(y + 1, x);
                       brAngle = -math.pi / 2;
                       break;
                    end
                else
                    if isfr(y, x - 1) and isfr(y + 1, x - 1) and isfr(y - 1, x - 1) and
                       isfr(y, x - 2) and isfr(y + 1, x - 2) and isfr(y - 1, x - 2) and
                       isfr(y, x - 3) and isfr(y + 1, x - 3) and isfr(y - 1, x - 3) then
                       mark(y, x - 1); mark(y + 1, x - 1); mark(y - 1, x - 1);
                       mark(y, x - 2); mark(y + 1, x - 2); mark(y - 1, x - 2);
                       mark(y, x - 3); mark(y + 1, x - 3); mark(y - 1, x - 3);
                       done = true;
                       maze[y][x][dir] = 1;
                       maze[y][x - 1][myRight] = 1;
                       mazeOpenEnds[k] = nil;
                       brXY = {y, x - 1};
                       brPos = indexToPos(y, x - 1);
                       brAngle = math.pi;
                       break;
                    end
                end
            end
        end
        if done then
            break;
        end
    end
    if not done then
        return false;
    end

    local tmp = {};
    for _, v in pairs(mazeOpenEnds) do
        table.insert(tmp, v);
    end
    mazeOpenEnds = tmp;
    table.shuffle(mazeOpenEnds);

    if #mazeOpenEnds < 3 then
        return false;
    end

    done = false;

    for k, v in pairs(mazeOpenEnds) do
        local y = v[1];
        local x = v[2];
        local sides = maze[y][x];
        local numWays = 0;
        for i = 1, #sides, 1 do
            if sides[i] == 1 then
                numWays = numWays + 1;
            end
        end
        if numWays == 1 then
            local tmpb = blocks[sidesToIndex(maze[y][x])];
            tmpb = tmpb[math.random(1, #tmpb)];
            local inst = mazeCreateSpecialBlock(tmpb[1], indexToPos(y, x), tmpb[2]);

            mazeInstances[y][x] = inst;
            mazeUsed[y][x] = true;
            mazeOpenEnds[k] = nil;
            done = true;
            break;
        end
    end

    if not done then
        return false;
    end

    tmp = {};
    for _, v in pairs(mazeOpenEnds) do
        table.insert(tmp, v);
    end
    mazeOpenEnds = tmp;
    table.shuffle(mazeOpenEnds);

    mazeKeys[mazeOpenEnds[1][1]][mazeOpenEnds[1][2]] = const.InventoryItemRedKey;
    mazeKeys[mazeOpenEnds[2][1]][mazeOpenEnds[2][2]] = const.InventoryItemBlueKey;

    bossInst = scene:instanciate("e1m12_bossblock.json", brPos, brAngle);
    mazeInstances[brXY[1]][brXY[2]] = bossInst;
    bossInst.mySpecial = true;
    bossInst.myActivateFirst = 1;
    bossInst.myBoss = true;

    preBossPos = brPos + vec2(35, 0):rotated(brAngle + math.pi);

    return true;
end

local function makeMazeBigBlocks()
    while true do
        local poses = {};
        for i = 2, 8, 1 do
            for j = 2, 8, 1 do
                if (not mazeUsed[i][j]) and (not mazeUsed[i][j + 1]) and (not mazeUsed[i + 1][j]) and (not mazeUsed[i + 1][j + 1]) then
                    if (sidesToIndex(maze[i][j]) ~= 0) and (sidesToIndex(maze[i][j + 1]) ~= 0) and (sidesToIndex(maze[i + 1][j]) ~= 0) and (sidesToIndex(maze[i + 1][j + 1]) ~= 0) then
                        local sides = {maze[i][j][myUp],
                                       maze[i][j + 1][myUp],
                                       maze[i][j + 1][myRight],
                                       maze[i + 1][j + 1][myRight],
                                       maze[i + 1][j + 1][myDown],
                                       maze[i + 1][j][myDown],
                                       maze[i + 1][j][myLeft],
                                       maze[i][j][myLeft]};
                        if (#bigBlocks[sidesToIndex(sides)] > 0) and (numKeys({i, j}, {i, j + 1}, {i + 1, j}, {i + 1, j + 1}) <= 1) then
                            table.insert(poses, {i, j});
                        end
                    end
                end
            end
        end

        table.shuffle(poses);

        if (#poses <= 0) then
            break;
        end

        local i = poses[1][1];
        local j = poses[1][2];
        local sides = {maze[i][j][myUp],
                       maze[i][j + 1][myUp],
                       maze[i][j + 1][myRight],
                       maze[i + 1][j + 1][myRight],
                       maze[i + 1][j + 1][myDown],
                       maze[i + 1][j][myDown],
                       maze[i + 1][j][myLeft],
                       maze[i][j][myLeft]};
        local tmpb = bigBlocks[sidesToIndex(sides)];
        tmpb = tmpb[math.random(1, #tmpb)];

        local key = getKey({i, j}, {i, j + 1}, {i + 1, j}, {i + 1, j + 1});

        local inst = nil;

        if tmpb[2] == 0 then
            inst = mazeCreateBlock(tmpb[1], indexToPos(i, j), tmpb[2], key);
        elseif tmpb[2] == -math.pi / 2 then
            inst = mazeCreateBlock(tmpb[1], indexToPos(i, j + 1), tmpb[2], key);
        elseif tmpb[2] == -math.pi then
            inst = mazeCreateBlock(tmpb[1], indexToPos(i + 1, j + 1), tmpb[2], key);
        else
            inst = mazeCreateBlock(tmpb[1], indexToPos(i + 1, j), tmpb[2], key);
        end

        mazeInstances[i][j] = inst;
        mazeInstances[i][j + 1] = inst;
        mazeInstances[i + 1][j] = inst;
        mazeInstances[i + 1][j + 1] = inst;

        mazeUsed[i][j] = true;
        mazeUsed[i][j + 1] = true;
        mazeUsed[i + 1][j] = true;
        mazeUsed[i + 1][j + 1] = true;

        maze[i][j][myRight] = 0;
        maze[i][j][myDown] = 0;
        maze[i][j + 1][myLeft] = 0;
        maze[i][j + 1][myDown] = 0;
        maze[i + 1][j][myRight] = 0;
        maze[i + 1][j][myUp] = 0;
        maze[i + 1][j + 1][myUp] = 0;
        maze[i + 1][j + 1][myLeft] = 0;
    end
end

local function makeMazeLongHorzBlocks()
    while true do
        local poses = {};
        for i = 2, 8, 1 do
            for j = 2, 8, 1 do
                if (not mazeUsed[i][j]) and (not mazeUsed[i][j + 1]) then
                    if (sidesToIndex(maze[i][j]) ~= 0) and (sidesToIndex(maze[i][j + 1]) ~= 0) then
                        local sides = {maze[i][j][myUp],
                                       maze[i][j + 1][myUp],
                                       maze[i][j + 1][myRight],
                                       maze[i][j + 1][myDown],
                                       maze[i][j][myDown],
                                       maze[i][j][myLeft]};
                        if (#longHorzBlocks[sidesToIndex(sides)] > 0) and (numKeys({i, j}, {i, j + 1}) <= 1) then
                            table.insert(poses, {i, j});
                        end
                    end
                end
            end
        end

        table.shuffle(poses);

        if (#poses <= 0) then
            break;
        end

        local i = poses[1][1];
        local j = poses[1][2];
        local sides = {maze[i][j][myUp],
                       maze[i][j + 1][myUp],
                       maze[i][j + 1][myRight],
                       maze[i][j + 1][myDown],
                       maze[i][j][myDown],
                       maze[i][j][myLeft]};
        local tmpb = longHorzBlocks[sidesToIndex(sides)];
        tmpb = tmpb[math.random(1, #tmpb)];

        local key = getKey({i, j}, {i, j + 1});

        local inst = nil;

        if tmpb[2] ~= 0 then
            inst = mazeCreateBlock(tmpb[1], indexToPos(i, j + 1), tmpb[2], key);
        else
            inst = mazeCreateBlock(tmpb[1], indexToPos(i, j), tmpb[2], key);
        end

        mazeInstances[i][j] = inst;
        mazeInstances[i][j + 1] = inst;

        mazeUsed[i][j] = true;
        mazeUsed[i][j + 1] = true;

        maze[i][j][myRight] = 0;
        maze[i][j + 1][myLeft] = 0;
    end
end

local function makeMazeLongVertBlocks()
    while true do
        local poses = {};
        for i = 2, 8, 1 do
            for j = 2, 8, 1 do
                if (not mazeUsed[i][j]) and (not mazeUsed[i + 1][j]) then
                    if (sidesToIndex(maze[i][j]) ~= 0) and (sidesToIndex(maze[i + 1][j]) ~= 0) then
                        local sides = {maze[i][j][myRight],
                                       maze[i + 1][j][myRight],
                                       maze[i + 1][j][myDown],
                                       maze[i + 1][j][myLeft],
                                       maze[i][j][myLeft],
                                       maze[i][j][myUp]};
                        if (#longHorzBlocks[sidesToIndex(sides)] > 0) and (numKeys({i, j}, {i + 1, j}) <= 1) then
                            table.insert(poses, {i, j});
                        end
                    end
                end
            end
        end

        table.shuffle(poses);

        if (#poses <= 0) then
            break;
        end

        local i = poses[1][1];
        local j = poses[1][2];
        local sides = {maze[i][j][myRight],
                       maze[i + 1][j][myRight],
                       maze[i + 1][j][myDown],
                       maze[i + 1][j][myLeft],
                       maze[i][j][myLeft],
                       maze[i][j][myUp]};
        local tmpb = longHorzBlocks[sidesToIndex(sides)];
        tmpb = tmpb[math.random(1, #tmpb)];

        local key = getKey({i, j}, {i + 1, j});

        local inst = nil;

        if tmpb[2] ~= 0 then
            inst = mazeCreateBlock(tmpb[1], indexToPos(i + 1, j), tmpb[2] - (math.pi / 2), key);
        else
            inst = mazeCreateBlock(tmpb[1], indexToPos(i, j), tmpb[2] - (math.pi / 2), key);
        end

        mazeInstances[i][j] = inst;
        mazeInstances[i + 1][j] = inst;

        mazeUsed[i][j] = true;
        mazeUsed[i + 1][j] = true;

        maze[i][j][myDown] = 0;
        maze[i + 1][j][myUp] = 0;
    end
end

local function makeMazeBlocks()
    local indices = {};
    for i = 1, 9, 1 do
        for j = 1, 9, 1 do
            table.insert(indices, {i, j});
        end
    end
    table.shuffle(indices);
    for _, v in pairs(indices) do
        local i = v[1];
        local j = v[2];
        if not mazeUsed[i][j] then
            if sidesToIndex(maze[i][j]) == 0 then
                scene:instanciate("e1m12_block0.json", indexToPos(i, j), 0);
            else
                local tmpb = blocks[sidesToIndex(maze[i][j])];
                tmpb = tmpb[math.random(1, #tmpb)];
                local inst = mazeCreateBlock(tmpb[1], indexToPos(i, j), tmpb[2], getKey({i, j}));
                mazeInstances[i][j] = inst;
            end
        end
    end
end

local function makeMazeDoors()
    local colors = { "red", "blue" };
    table.shuffle(colors);
    local colorIndex = 1;

    for x = 1, 9, 1 do
        for y = 1, 9, 1 do
            if maze[y][x][myDown] == 1 then
                local inst = nil;
                if (mazeInstances[y][x]) ~= nil and mazeInstances[y][x].mySpecial then
                    inst = makeMazeKeyAirlock(indexToPos(y, x) - vec2(0, 25), 0, colors[colorIndex]);
                    colorIndex = colorIndex + 1;
                elseif (mazeInstances[y + 1][x] ~= nil) and mazeInstances[y + 1][x].mySpecial then
                    inst = makeMazeKeyAirlock(indexToPos(y, x) - vec2(0, 25), math.pi, colors[colorIndex]);
                    colorIndex = colorIndex + 1;
                else
                    inst = makeMazeAirlock(indexToPos(y, x) - vec2(0, 25), 0);
                end
                inst.myC = {mazeInstances[y][x], mazeInstances[y + 1][x]};
                if ((inst.myC[1] ~= nil) and (inst.myC[1].myBoss ~= nil)) or
                   ((inst.myC[2] ~= nil) and (inst.myC[2].myBoss ~= nil)) then
                   bossDoor = inst;
                end
            end
        end
    end
    for y = 1, 9, 1 do
        for x = 1, 9, 1 do
            if maze[y][x][myRight] == 1 then
                local inst = nil;
                if (mazeInstances[y][x]) ~= nil and mazeInstances[y][x].mySpecial then
                    inst = makeMazeKeyAirlock(indexToPos(y, x) + vec2(25, 0), math.pi / 2, colors[colorIndex]);
                    colorIndex = colorIndex + 1;
                elseif (mazeInstances[y][x + 1] ~= nil) and mazeInstances[y][x + 1].mySpecial then
                    inst = makeMazeKeyAirlock(indexToPos(y, x) + vec2(25, 0), 3 * math.pi / 2, colors[colorIndex]);
                    colorIndex = colorIndex + 1;
                else
                    inst = makeMazeAirlock(indexToPos(y, x) + vec2(25, 0), math.pi / 2);
                end
                inst.myC = {mazeInstances[y][x], mazeInstances[y][x + 1]};
                if ((inst.myC[1] ~= nil) and (inst.myC[1].myBoss ~= nil)) or
                   ((inst.myC[2] ~= nil) and (inst.myC[2].myBoss ~= nil)) then
                   bossDoor = inst;
                end
            end
        end
    end
end

-- main

for i = 1, 15, 1 do
    blocks[i] = {};
end

for i = 1, 63, 1 do
    longHorzBlocks[i] = {};
end

for i = 1, 255, 1 do
    bigBlocks[i] = {};
end

addBlock("e1m12_block1.json", { myLeft });
addBlock("e1m12_block2.json", { myLeft, myUp });
addBlock("e1m12_block3.json", { myLeft, myUp, myDown });
addBlock("e1m12_block4.json", { myLeft, myRight });
addBlock("e1m12_block5.json", { myLeft, myUp });
addBlock("e1m12_block6.json", { myLeft, myUp, myDown });
addBlock("e1m12_block7.json", { myLeft, myRight });
addBlock("e1m12_block8.json", { myLeft });
addBlock("e1m12_block9.json", { myLeft, myUp });
addBlock("e1m12_block10.json", { myLeft, myUp, myDown });
addLongBlock("e1m12_longblock1.json", { myLongUp1, myLongUp2, myLongDown1, myLongDown2 });
addLongBlock("e1m12_longblock2.json", { myLongUp2, myLongDown2 });
addLongBlock("e1m12_longblock3.json", { myLongLeft, myLongRight });
addLongBlock("e1m12_longblock4.json", { myLongUp1, myLongRight, myLongDown2 });
addBigBlock("e1m12_bigblock1.json", { myBigUp2, myBigDown1, myBigDown2 });
addBigBlock("e1m12_bigblock2.json", { myBigRight1, myBigDown1, myBigDown2 });
addBigBlock("e1m12_bigblock3.json", { myBigRight1, myBigDown2 });
addBigBlock("e1m12_bigblock4.json", { myBigRight1, myBigDown1, myBigLeft1 });

while true do
    maze = {};
    mazeStack = {};
    mazeUsed = {};
    mazeOpenEnds = {};

    for i = 1, 9, 1 do
        maze[i] = {};
        mazeUsed[i] = {};
        mazeKeys[i] = {};
        mazeInstances[i] = {};
        for j = 1, 9, 1 do
            maze[i][j] = {0, 0, 0, 0};
            mazeUsed[i][j] = false;
            mazeKeys[i][j] = -1;
            mazeInstances[i][j] = nil;
        end
    end

    maze[5][1][myRight] = 1;
    mazeUsed[5][1] = true;
    table.insert(mazeStack, {5, 1});

    generateMazeFromStack();
    if makeMazeBossBlock() then
        break;
    end

    print("bad maze, regenerating...");
end

makeMazeBigBlocks();
makeMazeLongHorzBlocks();
makeMazeLongVertBlocks();
makeMazeBlocks();
makeMazeDoors();
