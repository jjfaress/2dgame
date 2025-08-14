tiles = {
    grass = { id = 1 },
    water = { id = 2 },
    forest = { id = 3 },
    mountain = { id = 4 },
    desert = { id = 5 }
}

validNeighbors = {
    [tiles.grass.id] = {
        north = { tiles.grass.id, tiles.forest.id, tiles.water.id },
        east = { tiles.grass.id, tiles.forest.id, tiles.water.id },
        south = { tiles.grass.id, tiles.forest.id, tiles.water.id },
        west = { tiles.grass.id, tiles.forest.id, tiles.water.id }
    },
    [tiles.water.id] = {
        north = { tiles.grass.id, tiles.forest.id, tiles.water.id },
        east = { tiles.grass.id, tiles.forest.id, tiles.water.id },
        south = { tiles.grass.id, tiles.forest.id, tiles.water.id },
        west = { tiles.grass.id, tiles.forest.id, tiles.water.id }
    }
}