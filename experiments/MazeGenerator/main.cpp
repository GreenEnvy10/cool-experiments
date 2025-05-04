#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <stack>
#include <ctime>
#include <cstdlib>
#include <sstream>

const int CELL_SIZE = 10;  // Each cell will be 10x10 pixels

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Cell {
    bool visited = false;
    bool walls[4] = { true, true, true, true }; // Up, Down, Left, Right
};

int index(int x, int y, int cols, int rows) {
    if (x < 0 || y < 0 || x >= cols || y >= rows)
        return -1;
    return x + y * cols;
}

std::vector<Cell> generateMaze(int cols, int rows) {
    std::vector<Cell> grid(cols * rows);
    std::stack<sf::Vector2i> stack;
    sf::Vector2i current(0, 0);
    grid[index(current.x, current.y, cols, rows)].visited = true;
    stack.push(current);

    while (!stack.empty()) {
        current = stack.top();
        std::vector<Direction> neighbors;

        // Check each direction
        if (current.y > 0 && !grid[index(current.x, current.y - 1, cols, rows)].visited)
            neighbors.push_back(UP);
        if (current.y < rows - 1 && !grid[index(current.x, current.y + 1, cols, rows)].visited)
            neighbors.push_back(DOWN);
        if (current.x > 0 && !grid[index(current.x - 1, current.y, cols, rows)].visited)
            neighbors.push_back(LEFT);
        if (current.x < cols - 1 && !grid[index(current.x + 1, current.y, cols, rows)].visited)
            neighbors.push_back(RIGHT);

        if (!neighbors.empty()) {
            Direction dir = neighbors[rand() % neighbors.size()];
            sf::Vector2i next = current;

            switch (dir) {
                case UP: next.y--; break;
                case DOWN: next.y++; break;
                case LEFT: next.x--; break;
                case RIGHT: next.x++; break;
            }

            Cell& currentCell = grid[index(current.x, current.y, cols, rows)];
            Cell& nextCell = grid[index(next.x, next.y, cols, rows)];

            // Remove walls
            if (dir == UP) {
                currentCell.walls[0] = false;
                nextCell.walls[1] = false;
            } else if (dir == DOWN) {
                currentCell.walls[1] = false;
                nextCell.walls[0] = false;
            } else if (dir == LEFT) {
                currentCell.walls[2] = false;
                nextCell.walls[3] = false;
            } else if (dir == RIGHT) {
                currentCell.walls[3] = false;
                nextCell.walls[2] = false;
            }

            nextCell.visited = true;
            stack.push(next);
        } else {
            stack.pop();
        }
    }

    return grid;
}

void drawMaze(sf::RenderTexture& texture, const std::vector<Cell>& maze, int cols, int rows) {
    texture.clear(sf::Color::White);
    sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, 1));
    wall.setFillColor(sf::Color::Black);

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int i = index(x, y, cols, rows);
            float px = x * CELL_SIZE;
            float py = y * CELL_SIZE;

            if (maze[i].walls[0]) { // Up
                wall.setSize(sf::Vector2f(CELL_SIZE, 1));
                wall.setPosition(px, py);
                texture.draw(wall);
            }
            if (maze[i].walls[1]) { // Down
                wall.setSize(sf::Vector2f(CELL_SIZE, 1));
                wall.setPosition(px, py + CELL_SIZE);
                texture.draw(wall);
            }
            if (maze[i].walls[2]) { // Left
                wall.setSize(sf::Vector2f(1, CELL_SIZE));
                wall.setPosition(px, py);
                texture.draw(wall);
            }
            if (maze[i].walls[3]) { // Right
                wall.setSize(sf::Vector2f(1, CELL_SIZE));
                wall.setPosition(px + CELL_SIZE, py);
                texture.draw(wall);
            }
        }
    }

    texture.display();
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    int mazeWidth = 50;
    int mazeHeight = 50;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Backrooms Maze Generator");
    sf::Font font;
    font.loadFromFile("arial.ttf"); // You need this font file in the working directory

    sf::Text inputLabel("Width x Height:", font, 16);
    inputLabel.setPosition(10, 10);
    inputLabel.setFillColor(sf::Color::Black);

    sf::Text inputBox("", font, 16);
    inputBox.setPosition(150, 10);
    inputBox.setFillColor(sf::Color::Black);
    std::string inputText;

    sf::RectangleShape previewButton(sf::Vector2f(120, 30));
    previewButton.setPosition(10, 50);
    previewButton.setFillColor(sf::Color(200, 200, 200));
    sf::Text previewLabel("Preview", font, 16);
    previewLabel.setPosition(20, 55);
    previewLabel.setFillColor(sf::Color::Black);

    sf::RectangleShape saveButton(sf::Vector2f(120, 30));
    saveButton.setPosition(150, 50);
    saveButton.setFillColor(sf::Color(200, 200, 200));
    sf::Text saveLabel("Download", font, 16);
    saveLabel.setPosition(160, 55);
    saveLabel.setFillColor(sf::Color::Black);

    sf::RenderTexture mazeTexture;
    mazeTexture.create(2000, 2000); // Enough space

    std::vector<Cell> maze;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !inputText.empty()) {
                    inputText.pop_back();
                } else if (event.text.unicode < 128 && std::isdigit(event.text.unicode)) {
                    inputText += static_cast<char>(event.text.unicode);
                } else if (event.text.unicode == 'x' || event.text.unicode == 'X') {
                    inputText += 'x';
                }
                inputBox.setString(inputText);
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (previewButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    size_t pos = inputText.find('x');
                    if (pos != std::string::npos) {
                        mazeWidth = std::stoi(inputText.substr(0, pos));
                        mazeHeight = std::stoi(inputText.substr(pos + 1));
                        maze = generateMaze(mazeWidth, mazeHeight);
                        drawMaze(mazeTexture, maze, mazeWidth, mazeHeight);
                    }
                }

                if (saveButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    sf::Image mazeImage = mazeTexture.getTexture().copyToImage();
                    mazeImage.saveToFile("maze_output.png");
                    std::cout << "Saved maze_output.png\n";
                }
            }
        }

        window.clear(sf::Color(255, 255, 255));
        window.draw(inputLabel);
        window.draw(inputBox);
        window.draw(previewButton);
        window.draw(previewLabel);
        window.draw(saveButton);
        window.draw(saveLabel);

        if (!maze.empty()) {
            sf::Sprite sprite(mazeTexture.getTexture());
            sprite.setScale(0.3f, 0.3f); // Fit into window
            sprite.setPosition(10, 100);
            window.draw(sprite);
        }

        window.display();
    }

    return 0;
}
