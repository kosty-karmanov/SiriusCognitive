#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include <windows.h>

using namespace std;
using namespace sf;

struct button {
    string text;
    int x, y, width, height, action, font_size;

    button(string s1, int x1, int y1, int width1, int height1, int action1,
           int size) {
        text = s1;
        x = x1;
        y = y1;
        width = width1;
        height = height1;
        action = action1;
        font_size = size;
    }
};

int window_width = 2560, window_height = 1440, current_illumination = -1, state = 0, target = -1;
bool is_locked_buttons = false;
RenderWindow window(VideoMode(window_width, window_height), "Simulation");
mt19937 mt(chrono::high_resolution_clock::now().time_since_epoch().count());
vector<button> buttons;
vector<Color> colors = {Color(219, 68, 64), Color(248, 222, 84),
                        Color(194, 60, 169), Color(16, 216, 227),
                        Color(79, 212, 71), Color(135, 204, 46),
                        Color(232, 4, 127), Color(60, 100, 240)};


class MyShape {
private:
    bool used = false, fake = false;
    string type;
    CircleShape circleShape;
    RectangleShape rectangleShape;
public:
    MyShape() = default;

    void become_rectangle(RectangleShape shape, bool is_fake = false) {
        type = "rectangle";
        rectangleShape = shape;
        fake = is_fake;
    }

    void become_circle(CircleShape circle, bool is_fake = false) {
        type = "circle";
        circleShape = circle;
        fake = is_fake;
    }

    void draw() {
        if (type == "rectangle") {
            window.draw(rectangleShape);
        }
        if (type == "circle") {
            window.draw(circleShape);
        }
    }

    bool is_used() {
        return used;
    }

    bool is_fake() {
        return fake;
    }

    void set_used() {
        used = true;
    }

    pair<int, int> get_position() {
        pair<int, int> pos;
        if (type == "circle") {
            pos.first = circleShape.getPosition().x;
            pos.second = circleShape.getPosition().y;
        } else {
            pos.first = rectangleShape.getPosition().x;
            pos.second = rectangleShape.getPosition().y;
        }
        return pos;
    }

    int get_size() {
        if (type == "circle") {
            return circleShape.getRadius();
        }
        return rectangleShape.getSize().x;
    }

    void draw_at_center() {
        if (type == "rectangle") {
            rectangleShape.setPosition(1200, 500);
            window.draw(rectangleShape);
        }
        if (type == "circle") {
            circleShape.setPosition(1200, 500);
            window.draw(circleShape);
        }
    }
};

vector<MyShape> shapes, correct_ans, incorrect_ans;


void Type(RenderWindow &window2, float x, float y, String s,
          Color col = Color::White, int size = 24) {
    Text text;
    Font font;
    font.loadFromFile("../arial.ttf");
    text.setFont(font);
    text.setString(s);
    text.setPosition(x - text.getLocalBounds().width / 2,
                     y - text.getLocalBounds().height / 2);
    text.setFillColor(col);
    text.setCharacterSize(size);
    window2.draw(text);
}


void create_buttons_state1() {
    buttons = {
            button("random", 100, 50, 80, 50, 1, 30),
    };
}

void create_buttons_state2() {
    buttons = {
            button("Yes", 675, 1200, 260, 160, 2, 100),
            button("No", 1476, 1200, 260, 160, 3, 100),
    };
}


void draw_buttons() {
    int cnt = 0;
    for (button but: buttons) {
        cnt++;
        Color cul = Color(43, 45, 48);
        if (cnt == current_illumination)
            cul = Color(65, 119, 244);
        CircleShape circle(but.height / 2);
        circle.setPosition(but.x, but.y);
        circle.setFillColor(cul);
        window.draw(circle);

        circle.setPosition(but.x + but.width, but.y);
        window.draw(circle);

        RectangleShape box_but1(Vector2f(but.width, but.height));
        box_but1.setPosition(but.x + but.height / 2, but.y);
        box_but1.setFillColor(cul);
        window.draw(box_but1);
        Type(window, but.x + (but.height),
             but.y + but.height / 10, but.text, Color::White, but.font_size);
    }
}

void buttons_illumination() {
    int wind_x = window.getPosition().x;

    int wind_y = window.getPosition().y;

    int mouse_x = Mouse::getPosition().x - 14;

    int mouse_y = Mouse::getPosition().y - 70;

    if (mouse_x >= wind_x and mouse_x <= wind_x + window.getSize().x and
        mouse_y >= wind_y and mouse_y <= wind_y + window.getSize().y) {
        int field_x = mouse_x - wind_x;
        int field_y = mouse_y - wind_y;
        int cnt = 0;
        for (button but: buttons) {
            cnt++;
            int x1 = but.x;
            int y1 = but.y;
            int h = but.height;
            int w = but.width;
            if (field_x > x1 and field_x < x1 + w + h and field_y > y1 and
                field_y < y1 + h) {
                current_illumination = cnt;
                return;
            }
        }
        current_illumination = -1;
    }
}


bool is_near(int x1, int y1, int size1, int x2, int y2, int size2) {
    return sqrt(abs(x1 - x2) * abs(x1 - x2) + abs(y1 - y2) * abs(y1 - y2)) < size1 + size2;
}


bool is_near_with_shapes(int x, int y, int size) {
    for (MyShape shape: shapes) {
        if (is_near(x, y, size, shape.get_position().first, shape.get_position().second, shape.get_size() * 1.1)) {
            return true;
        }
    }
    return false;
}


pair<int, int> generate_position() {
    int x = mt() % (window_width - 600) + 100;
    int y = mt() % (window_height - 600) + 100;
    return {x, y};
}

bool generate_bool() {
    return mt() % 4;
}

Color generate_color() {
    return colors[mt() % colors.size()];
}

int generate_size() {
    return mt() % 150 + 50;
}

void draw_circle(int radius, int x, int y, Color color, bool fill, bool is_fake) {
    CircleShape shape(radius);
    shape.setPosition(x, y);
    if (fill) {
        shape.setFillColor(color);
    } else {
        shape.setFillColor(Color().White);
        shape.setOutlineColor(color);
        shape.setOutlineThickness(20);
    }
    shapes.emplace_back();
    shapes[shapes.size() - 1].become_circle(shape, is_fake);
}

void draw_rectangle(int height, int width, int x, int y, Color color, bool fill, bool is_fake) {
    RectangleShape rectangleShape(Vector2f(height, width));
    rectangleShape.setPosition(x, y);
    if (fill) {
        rectangleShape.setFillColor(color);
    } else {
        rectangleShape.setFillColor(Color().White);
        rectangleShape.setOutlineColor(color);
        rectangleShape.setOutlineThickness(20);
    }
    shapes.emplace_back();
    shapes[shapes.size() - 1].become_rectangle(rectangleShape, is_fake);
}

void draw_triangle(int size, int x, int y, Color color, bool fill, bool is_fake = false) {
    CircleShape triangle(size, 3);
    triangle.setPosition(x, y);
    if (fill) {
        triangle.setFillColor(color);
    } else {
        triangle.setFillColor(Color().White);
        triangle.setOutlineColor(color);
        triangle.setOutlineThickness(20);
    }
    shapes.emplace_back();
    shapes[shapes.size() - 1].become_circle(triangle, is_fake);
}

void draw_random_circle(bool is_fake = false) {
    pair<int, int> pos = generate_position();
    Color color = generate_color();
    int radius = generate_size();
    bool fill = generate_bool();
    while (is_near_with_shapes(pos.first, pos.second, radius)) {
        pos = generate_position();
    }
    draw_circle(radius, pos.first, pos.second, color, fill, is_fake);
}

void draw_random_rectangle(bool is_fake = false) {
    pair<int, int> pos = generate_position();
    Color color = generate_color();
    int height = generate_size(), width = generate_size();
    bool fill = generate_bool();
    while (is_near_with_shapes(pos.first, pos.second, height)) {
        pos = generate_position();
    }
    draw_rectangle(height, width, pos.first, pos.second, color, fill, is_fake);
}


void draw_random_triangle(bool is_fake = false) {
    pair<int, int> pos = generate_position();
    Color color = generate_color();
    int radius = generate_size();
    bool fill = generate_bool();
    while (is_near_with_shapes(pos.first, pos.second, radius)) {
        pos = generate_position();
    }
    draw_triangle(radius, pos.first, pos.second, color, fill, is_fake);
}


void generate_shapes() {
    shapes.clear();
    correct_ans.clear();
    incorrect_ans.clear();
    for (int i = 0; i < generate_size() % 3 + 1; ++i) {
        draw_random_circle();
    }
    for (int i = 0; i < generate_size() % 3 + 1; ++i) {
        draw_random_triangle();
    }
    for (int i = 0; i < generate_size() % 3 + 1; ++i) {
        draw_random_rectangle();
    }
}


void generate_fake_shapes() {
    for (int i = 0; i < generate_size() % 3 + 1; ++i) {
        draw_random_circle(true);
    }
    for (int i = 0; i < generate_size() % 3 + 1; ++i) {
        draw_random_triangle(true);
    }
    for (int i = 0; i < generate_size() % 3 + 1; ++i) {
        draw_random_rectangle(true);
    }
}


void update_target() {
    target = mt() % shapes.size();
    int cnt = 0;
    while (shapes[target].is_used()) {
        target = mt() % shapes.size();
        cnt++;
        if (cnt == 10000) {
            break;
        }
    }
    if (cnt == 10000) {
        state = 1;
        create_buttons_state1();
        Type(window, 1250, 600, to_string(correct_ans.size()) + "/" + to_string(shapes.size()), Color().Black, 100);
        window.display();
        Sleep(2000);
        return;
    }
    shapes[target].set_used();
}


void draw_question_shape() {
    MyShape myShape = shapes[target];
    myShape.draw_at_center();
}


void draw_shapes() {
    for (MyShape &shape: shapes) {
        shape.draw();
    }
}


void mouse_reaction(Event &event) {
    if (is_locked_buttons) {
        return;
    }
    is_locked_buttons = true;
    int x = event.mouseButton.x;
    int y = event.mouseButton.y;
    for (button but: buttons) {
        int x1 = but.x;
        int y1 = but.y;
        int h = but.height;
        int w = but.width;
        if (x > x1 and x < x1 + w + h and y > y1 and y < y1 + h and
            event.mouseButton.button == Mouse::Left) {
            if (but.action == 1) {
                generate_shapes();
                draw_shapes();
                window.display();
                Sleep(2000);
                window.clear(Color().White);
                window.display();
                state = 2;
                create_buttons_state2();
                generate_fake_shapes();
                update_target();
            } else if (but.action == 2) {
                if (!shapes[target].is_fake()) {
                    correct_ans.emplace_back(shapes[target]);
                } else {
                    incorrect_ans.emplace_back(shapes[target]);
                }
                update_target();
            } else if (but.action == 3) {
                if (shapes[target].is_fake()) {
                    correct_ans.emplace_back(shapes[target]);
                } else {
                    incorrect_ans.emplace_back(shapes[target]);
                }
                update_target();
            }
        }
    }
}


int main() {
    create_buttons_state1();
    while (window.isOpen()) {
        window.clear(Color().White);
        Event event;
        while (window.pollEvent(event) and window.hasFocus()) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }
        if (event.type == Event::MouseButtonPressed) {
            mouse_reaction(event);
        }
        if (event.type == Event::MouseButtonReleased) {
            is_locked_buttons = false;
        }
        if (state == 2) {
            Type(window, 850, 100, "Was this object on the screen?", Color().Black, 100);
            Type(window, 2350, 20, to_string(correct_ans.size() + incorrect_ans.size()) + "/" + to_string(shapes.size()), Color().Black, 100);
            draw_question_shape();
        }

        buttons_illumination();
        draw_buttons();
        window.display();
    }
}
