#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int WIDTH = 200;
const int HEIGHT = 150;
const int PIXEL_SIZE = 4;

enum CellType { EMPTY, SAND, WALL, WATER, GAS };

enum UserAction { DRAW_SAND, DRAW_WALL, DRAW_WATER, DRAW_GAS, ERASE };

struct Cell {
    CellType type = EMPTY;
    float velocityY = 0.0f; // Vertical velocity for more realistic physics
    float velocityX = 0.0f; // Horizontal velocity for fluids
};

std::vector<std::vector<Cell>> grid(HEIGHT, std::vector<Cell>(WIDTH));

void update() {
    const float gravity = 0.1f;
    const float buoyancy = -0.05f; // For gas rising
    const float friction = 0.95f;
    const float maxVelocity = 3.0f;
    
    // Create a copy of the grid to avoid conflicts during updates
    std::vector<std::vector<Cell>> newGrid = grid;
    
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
                
                if (newY > y && grid[newY][x].type == EMPTY) {
                    // Move straight down
                    newGrid[newY][x] = cell;
                    newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
                } else if (newY > y) {
                    // Try diagonal movement
                    cell.velocityY = 0; // Reset velocity when blocked
                    int dx = (std::rand() % 2 == 0) ? -1 : 1;
                    if (x + dx >= 0 && x + dx < WIDTH && y + 1 < HEIGHT && grid[y + 1][x + dx].type == EMPTY) {
                        newGrid[y + 1][x + dx] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
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
                
                if (newY > y && grid[newY][x].type == EMPTY) {
                    // Move straight down
                    newGrid[newY][x] = cell;
                    newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
                } else {
                    // Water flows horizontally when blocked vertically
                    cell.velocityY = 0;
                    
                    // Check horizontal flow
                    bool canFlowLeft = (x > 0 && grid[y][x - 1].type == EMPTY);
                    bool canFlowRight = (x < WIDTH - 1 && grid[y][x + 1].type == EMPTY);
                    
                    if (canFlowLeft && canFlowRight) {
                        // Flow in random direction
                        int dx = (std::rand() % 2 == 0) ? -1 : 1;
                        newGrid[y][x + dx] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
                    } else if (canFlowLeft) {
                        newGrid[y][x - 1] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
                    } else if (canFlowRight) {
                        newGrid[y][x + 1] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
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
                    newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
                } else {
                    // Try diagonal movement
                    cell.velocityY = 0;
                    int dx = (cell.velocityX > 0) ? 1 : ((cell.velocityX < 0) ? -1 : (std::rand() % 3 - 1));
                    
                    if (dx != 0 && x + dx >= 0 && x + dx < WIDTH && y - 1 >= 0 && grid[y - 1][x + dx].type == EMPTY) {
                        newGrid[y - 1][x + dx] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
                    } else if (dx != 0 && x + dx >= 0 && x + dx < WIDTH && grid[y][x + dx].type == EMPTY) {
                        // Horizontal movement if can't go up
                        newGrid[y][x + dx] = cell;
                        newGrid[y][x] = {EMPTY, 0.0f, 0.0f};
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
    sf::RenderWindow window(sf::VideoMode(WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE), "Sand Simulator");

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
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            currentAction = ERASE;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_WALL) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x].type = WALL;
                grid[y][x].velocityX = 0.0f;
                grid[y][x].velocityY = 0.0f;
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_WATER) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x].type = WATER;
                grid[y][x].velocityX = 0.0f;
                grid[y][x].velocityY = 0.0f;
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_GAS) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x].type = GAS;
                grid[y][x].velocityX = 0.0f;
                grid[y][x].velocityY = -0.1f; // Gas starts with slight upward velocity
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == ERASE) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x].type = EMPTY;
                grid[y][x].velocityX = 0.0f;
                grid[y][x].velocityY = 0.0f;
            }
        }

        // Rysowanie piasku myszką
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_SAND) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x].type = SAND;
                grid[y][x].velocityX = 0.0f;
                grid[y][x].velocityY = 0.0f;
            }
        }

        update();

        window.clear();
        draw(window);
        window.display();
    }

    return 0;
}
