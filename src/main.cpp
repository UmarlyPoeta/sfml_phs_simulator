#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int WIDTH = 200;
const int HEIGHT = 150;
const int PIXEL_SIZE = 4;

enum CellType { EMPTY, SAND , WALL};

enum UserAction { DRAW_SAND, DRAW_WALL, ERASE };

struct Cell {
    CellType type = EMPTY;
};

std::vector<std::vector<Cell>> grid(HEIGHT, std::vector<Cell>(WIDTH));

void update() {
    for (int y = HEIGHT - 2; y >= 0; --y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (grid[y][x].type == SAND) {
                if (grid[y + 1][x].type == EMPTY) {
                    std::swap(grid[y][x], grid[y + 1][x]);
                } else {
                    int dx = (std::rand() % 2 == 0) ? -1 : 1;
                    if (x + dx >= 0 && x + dx < WIDTH && grid[y + 1][x + dx].type == EMPTY) {
                        std::swap(grid[y][x], grid[y + 1][x + dx]);
                    }
                }
            }

            if (grid[y][x].type == EMPTY && y < HEIGHT - 1 && grid[y + 1][x].type == WALL) {
                grid[y][x].type = WALL;
            }

            if (grid[y][x].type == WALL) {
                // Ensure walls do not move
                continue;
            }
        }
    }
}

void draw(sf::RenderWindow& window) {
    sf::RectangleShape pixel(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (grid[y][x].type == SAND) {
                pixel.setFillColor(sf::Color(194, 178, 128)); // Sand color
            } else if (grid[y][x].type == WALL) {
                pixel.setFillColor(sf::Color(100, 100, 100)); // Wall color
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
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            currentAction = ERASE;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_WALL) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x].type = WALL;
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == ERASE) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x].type = EMPTY;
            }
        }

        // Rysowanie piasku myszką
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && currentAction == DRAW_SAND) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[y][x].type = SAND;
            }
        }

        update();

        window.clear();
        draw(window);
        window.display();
    }

    return 0;
}
