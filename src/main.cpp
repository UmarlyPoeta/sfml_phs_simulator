#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int WIDTH = 200;
const int HEIGHT = 150;
const int PIXEL_SIZE = 4;

enum CellType { EMPTY, SAND };

struct Cell {
    CellType type = EMPTY;
};

std::vector<std::vector<Cell>> grid(WIDTH, std::vector<Cell>(HEIGHT));

void update() {
    for (int y = HEIGHT - 2; y >= 0; --y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (grid[x][y].type == SAND) {
                if (grid[x][y + 1].type == EMPTY) {
                    std::swap(grid[x][y], grid[x][y + 1]);
                } else {
                    int dx = (std::rand() % 2 == 0) ? -1 : 1;
                    if (x + dx >= 0 && x + dx < WIDTH && grid[x + dx][y + 1].type == EMPTY) {
                        std::swap(grid[x][y], grid[x + dx][y + 1]);
                    }
                }
            }
        }
    }
}

void draw(sf::RenderWindow& window) {
    sf::RectangleShape pixel(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            if (grid[x][y].type == SAND) {
                pixel.setFillColor(sf::Color(194, 178, 128));
                pixel.setPosition(x * PIXEL_SIZE, y * PIXEL_SIZE);
                window.draw(pixel);
            }
        }
    }
}

int main() {
    std::srand(std::time(nullptr));
    sf::RenderWindow window(sf::VideoMode(WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE), "Sand Simulator");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Rysowanie piasku myszką
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            int x = mouse.x / PIXEL_SIZE;
            int y = mouse.y / PIXEL_SIZE;
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                grid[x][y].type = SAND;
            }
        }

        update();

        window.clear();
        draw(window);
        window.display();
    }

    return 0;
}
