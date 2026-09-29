#include "tile.hpp"

Tile::Tile() : piece(nullptr), colour(0) {}
Tile::~Tile() {delete piece;}