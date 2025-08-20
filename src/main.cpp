#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int WIDTH = 200;
const int HEIGHT = 150;
const int PIXEL_SIZE = 4;

enum CellType { EMPTY, SAND, WALL, WATER, GAS, FIRE };

enum UserAction { DRAW_SAND, DRAW_WALL, DRAW_WATER, DRAW_GAS, DRAW_FIRE, ERASE };

struct Cell {
    CellType type = EMPTY;
    float velocityY = 0.0f; // Vertical velocity for more realistic physics
    float velocityX = 0.0f; // Horizontal velocity for fluids
    int life = 0; // For fire lifetime and other temporary effects
};

std::vector<std::vector<Cell>> grid(HEIGHT, std::vector<Cell>(WIDTH));

void update() {
    const float gravity = 0.1f;
    const float buoyancy = -0.05f; // For gas rising
    const float friction = 0.95f;
    const float maxVelocity = 3.0f;
    
    // Create a copy of the grid to avoid conflicts during updates
    std::vector<std::vector<Cell>> newGrid = grid;
    
    // Helper function to check if a material can displace another
    auto canDisplace = [](CellType moving, CellType stationary) {
        if (stationary == EMPTY) return true;
        if (stationary == WALL) return false;
        // Density-based displacement: heavier materials displace lighter ones
        int densityMoving = (moving == SAND) ? 3 : (moving == WATER) ? 2 : (moving == FIRE) ? 1 : (moving == GAS) ? 0 : 0;
        int densityStationary = (stationary == SAND) ? 3 : (stationary == WATER) ? 2 : (stationary == FIRE) ? 1 : (stationary == GAS) ? 0 : 0;
        return densityMoving > densityStationary;
    };
    
    // Update physics for each cell
    for (int y = HEIGHT - 2; y >= 0; --y) {
        for (int x = 0; x < WIDTH; ++x) {
            Cell& cell = newGrid[y][x];
            
            if (cell.type == EMPTY || cell.type == WALL) {
                continue; // Skip empty cells and walls
            }
            
            // Apply physics based on material type
            if (cell.type == SAND) {
                // Apply gravity
                cell.velocityY += gravity;
                cell.velocityY = std::min(cell.velocityY, maxVelocity);
                
                // Try to move down
                int newY = y + (int)cell.velocityY;
                newY = std::min(newY, HEIGHT - 1);
                
                if (newY > y && canDisplace(SAND, grid[newY][x].type)) {
                    // Move straight down, possibly displacing lighter materials
                    if (grid[newY][x].type != EMPTY) {
                        newGrid[y][x] = grid[newY][x]; // Displaced material goes up
                    } else {
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                    }
                    newGrid[newY][x] = cell;
                } else if (newY > y) {
                    // Try diagonal movement
                    cell.velocityY = 0; // Reset velocity when blocked
                    int dx = (std::rand() % 2 == 0) ? -1 : 1;
                    if (x + dx >= 0 && x + dx < WIDTH && y + 1 < HEIGHT && canDisplace(SAND, grid[y + 1][x + dx].type)) {
                        if (grid[y + 1][x + dx].type != EMPTY) {
                            newGrid[y][x] = grid[y + 1][x + dx];
                        } else {
                            newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                        }
                        newGrid[y + 1][x + dx] = cell;
                    }
                }
            }
            else if (cell.type == WATER) {
                // Apply gravity
                cell.velocityY += gravity;
                cell.velocityY = std::min(cell.velocityY, maxVelocity);
                
                // Try to move down first
                int newY = y + (int)cell.velocityY;
                newY = std::min(newY, HEIGHT - 1);
                
                if (newY > y && canDisplace(WATER, grid[newY][x].type)) {
                    // Move straight down, possibly displacing lighter materials
                    if (grid[newY][x].type != EMPTY) {
                        newGrid[y][x] = grid[newY][x]; // Displaced material goes up
                    } else {
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                    }
                    newGrid[newY][x] = cell;
                } else {
                    // Water flows horizontally when blocked vertically
                    cell.velocityY = 0;
                    
                    // Check horizontal flow
                    bool canFlowLeft = (x > 0 && canDisplace(WATER, grid[y][x - 1].type));
                    bool canFlowRight = (x < WIDTH - 1 && canDisplace(WATER, grid[y][x + 1].type));
                    
                    if (canFlowLeft && canFlowRight) {
                        // Flow in random direction
                        int dx = (std::rand() % 2 == 0) ? -1 : 1;
                        if (grid[y][x + dx].type != EMPTY) {
                            newGrid[y][x] = grid[y][x + dx];
                        } else {
                            newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                        }
                        newGrid[y][x + dx] = cell;
                    } else if (canFlowLeft) {
                        if (grid[y][x - 1].type != EMPTY) {
                            newGrid[y][x] = grid[y][x - 1];
                        } else {
                            newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                        }
                        newGrid[y][x - 1] = cell;
                    } else if (canFlowRight) {
                        if (grid[y][x + 1].type != EMPTY) {
                            newGrid[y][x] = grid[y][x + 1];
                        } else {
                            newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                        }
                        newGrid[y][x + 1] = cell;
                    }
                }
            }
            else if (cell.type == GAS) {
                // Apply buoyancy (negative gravity)
                cell.velocityY += buoyancy;
                cell.velocityY = std::max(cell.velocityY, -maxVelocity);
                
                // Apply horizontal drift
                cell.velocityX += (std::rand() % 3 - 1) * 0.01f; // Random drift
                cell.velocityX *= friction; // Apply friction
                cell.velocityX = std::max(-1.0f, std::min(1.0f, cell.velocityX));
                
                // Try to move up
                int newY = y + (int)cell.velocityY;
                newY = std::max(newY, 0);
                
                if (newY < y && grid[newY][x].type == EMPTY) {
                    // Move straight up
                    newGrid[newY][x] = cell;
                    newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                } else {
                    // Try diagonal movement
                    cell.velocityY = 0;
                    int dx = (cell.velocityX > 0) ? 1 : ((cell.velocityX < 0) ? -1 : (std::rand() % 3 - 1));
                    
                    if (dx != 0 && x + dx >= 0 && x + dx < WIDTH && y - 1 >= 0 && grid[y - 1][x + dx].type == EMPTY) {
                        newGrid[y - 1][x + dx] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                    } else if (dx != 0 && x + dx >= 0 && x + dx < WIDTH && grid[y][x + dx].type == EMPTY) {
                        // Horizontal movement if can't go up
                        newGrid[y][x + dx] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                    }
                }
            }
            else if (cell.type == FIRE) {
                // Fire physics: rises like gas but has lifetime and spreads
                cell.life--;
                
                if (cell.life <= 0) {
                    // Fire dies out
                    newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                    continue;
                }
                
                // Fire spreads to adjacent flammable materials
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = x + dx, ny = y + dy;
                        if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                            if (grid[ny][nx].type == SAND && std::rand() % 20 == 0) {
                                // Sand has a chance to catch fire
                                newGrid[ny][nx] = {FIRE, 0.0f, -0.1f, 100 + std::rand() % 100};
                            }
                        }
                    }
                }
                
                // Apply buoyancy like gas
                cell.velocityY += buoyancy * 2; // Fire rises faster than gas
                cell.velocityY = std::max(cell.velocityY, -maxVelocity);
                
                // Apply horizontal drift
                cell.velocityX += (std::rand() % 3 - 1) * 0.02f; // More chaotic than gas
                cell.velocityX *= friction;
                cell.velocityX = std::max(-2.0f, std::min(2.0f, cell.velocityX));
                
                // Try to move up
                int newY = y + (int)cell.velocityY;
                newY = std::max(newY, 0);
                
                if (newY < y && grid[newY][x].type == EMPTY) {
                    // Move straight up
                    newGrid[newY][x] = cell;
                    newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                } else {
                    // Try diagonal movement
                    cell.velocityY = 0;
                    int dx = (cell.velocityX > 0) ? 1 : ((cell.velocityX < 0) ? -1 : (std::rand() % 3 - 1));
                    
                    if (dx != 0 && x + dx >= 0 && x + dx < WIDTH && y - 1 >= 0 && grid[y - 1][x + dx].type == EMPTY) {
                        newGrid[y - 1][x + dx] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                    } else if (dx != 0 && x + dx >= 0 && x + dx < WIDTH && grid[y][x + dx].type == EMPTY) {
                        // Horizontal movement if can't go up
                        newGrid[y][x + dx] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
                    }
                }
            }
        }
    }
    
    // Apply the updated grid
    grid = newGrid;
}

void draw(sf::RenderWindow& window) {
    sf::RectangleShape pixel(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (grid[y][x].type == SAND) {
                pixel.setFillColor(sf::Color(194, 178, 128)); // Sand color
            } else if (grid[y][x].type == WALL) {
                pixel.setFillColor(sf::Color(100, 100, 100)); // Wall color
            } else if (grid[y][x].type == WATER) {
                pixel.setFillColor(sf::Color(64, 164, 223)); // Water color (blue)
            } else if (grid[y][x].type == GAS) {
                pixel.setFillColor(sf::Color(200, 200, 200, 100)); // Gas color (light gray, semi-transparent)
            } else if (grid[y][x].type == FIRE) {
                // Fire color with slight variation for flickering effect
                int flicker = std::rand() % 50;
                pixel.setFillColor(sf::Color(255, 165 - flicker, 0)); // Orange-red fire
            } else {
                pixel.setFillColor(sf::Color::Black); // Empty cell color
            }
            pixel.setPosition(x * PIXEL_SIZE, y * PIXEL_SIZE);
            window.draw(pixel);
        }
    }
}

int main() {
    std::srand(std::time(nullptr));
    sf::RenderWindow window(sf::VideoMode(WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE), "Advanced Physics Simulator - S:Sand W:Wall L:Water G:Gas F:Fire E:Erase");

    UserAction currentAction = DRAW_SAND;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            currentAction = DRAW_SAND;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            currentAction = DRAW_WALL;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
            currentAction = DRAW_WATER;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
            currentAction = DRAW_GAS;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
            currentAction = DRAW_FIRE;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            currentAction = ERASE;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_WALL) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x] = {WALL, 0.0f, 0.0f, 0};
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_WATER) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x] = {WATER, 0.0f, 0.0f, 0};
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_GAS) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x] = {GAS, 0.0f, -0.1f, 0}; // Gas starts with slight upward velocity
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_FIRE) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x] = {FIRE, 0.0f, -0.2f, 300 + std::rand() % 200}; // Fire with random lifetime
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == ERASE) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x] = {EMPTY, 0.0f, 0.0f, 0};
            }
        }

        // Rysowanie piasku myszką
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_SAND) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x] = {SAND, 0.0f, 0.0f, 0};
            }
        }

        update();

        window.clear();
        draw(window);
        window.display();
    }

    return 0;
}
