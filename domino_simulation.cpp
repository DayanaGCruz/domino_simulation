#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <iostream>

// Utility function to convert HSV to RGB
void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
    int i = int(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
}

// Domino class
class Domino {
public:
    GLfloat x, y, z;        // Position
    GLfloat width, height, depth; // Dimensions
    GLfloat rotation;       // Rotation angle
    float r, g, b;          // RGB color
    bool isFalling;         // Falling state
    float fallDelay;        // Time delay before falling

    // Constructor
    Domino(GLfloat xPos, GLfloat yPos, GLfloat zPos,
           GLfloat w, GLfloat h, GLfloat d,
           GLfloat red, GLfloat green, GLfloat blue, float delay)
        : x(xPos), y(yPos), z(zPos),
          width(w), height(h), depth(d),
          rotation(0.0f), r(red), g(green), b(blue),
          isFalling(false), fallDelay(delay) {}

    // Update the Domino's animation
    void update(float deltaTime) {
        if (isFalling && fallDelay <= 0.0f) {
            rotation += 100.0f * deltaTime; // Rotate 100 degrees/sec
            if (rotation > 90.0f) rotation = 90.0f; // Clamp at 90 degrees
        } else if (isFalling) {
            fallDelay -= deltaTime; // Countdown to start falling
        }
    }

    // Draw the Domino
    void draw() const {
        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(rotation, 0.0f, 0.0f, 1.0f); // Rotate around Z-axis
        glColor3f(r, g, b);
        drawRectangularPrism(width, height, depth);
        glPopMatrix();
    }

private:
    // Utility function to draw a rectangular prism
    static void drawRectangularPrism(GLfloat w, GLfloat h, GLfloat d) {
        GLfloat hw = w / 2.0f, hh = h / 2.0f, hd = d / 2.0f;

        glBegin(GL_QUADS);
        // Front face
        glVertex3f(-hw, -hh, hd); glVertex3f(hw, -hh, hd);
        glVertex3f(hw, hh, hd); glVertex3f(-hw, hh, hd);
        // Back face
        glVertex3f(-hw, -hh, -hd); glVertex3f(-hw, hh, -hd);
        glVertex3f(hw, hh, -hd); glVertex3f(hw, -hh, -hd);
        // Left face
        glVertex3f(-hw, -hh, -hd); glVertex3f(-hw, -hh, hd);
        glVertex3f(-hw, hh, hd); glVertex3f(-hw, hh, -hd);
        // Right face
        glVertex3f(hw, -hh, -hd); glVertex3f(hw, hh, -hd);
        glVertex3f(hw, hh, hd); glVertex3f(hw, -hh, hd);
        // Top face
        glVertex3f(-hw, hh, hd); glVertex3f(hw, hh, hd);
        glVertex3f(hw, hh, -hd); glVertex3f(-hw, hh, -hd);
        // Bottom face
        glVertex3f(-hw, -hh, -hd); glVertex3f(hw, -hh, -hd);
        glVertex3f(hw, -hh, hd); glVertex3f(-hw, -hh, hd);
        glEnd();
    }
};

// Global variables
std::vector<Domino> dominos;
GLint WIN_WIDTH = 800, WIN_HEIGHT = 800;
bool simulationActive = false;

// Function declarations
void init();
void display();
void reshape(int win_w, int win_h);
void onMouseClick(int button, int state, int x, int y);
void DrawDominoSequence();

int main(int argc, char** argv) {
    std::cout << "Domino Simulation" << std::endl;

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("Domino Simulation");

    init();

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(onMouseClick);
    glutIdleFunc(display);

    // Initialize dominos
    for (int i = 0; i < 36; ++i) {
        float angle = i * (360.0f / 36); // Angle for each domino
        float rad = angle * (M_PI / 180.0f);
        float x = 5.0f * cos(rad);
        float z = 5.0f * sin(rad);

        float hue = angle / 360.0f;
        float r, g, b;
        HSVtoRGB(hue, 1.0f, 1.0f, r, g, b);

        dominos.emplace_back(x, 1.0f, z, 0.7f, 2.0f, 0.2f, r, g, b, i * 0.1f);
    }

    glutMainLoop();
    return 0;
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.784f, 0.635f, 0.784f, 1.0f);
}

void display() {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(10.0, 5.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    if (simulationActive) {
        for (auto& domino : dominos) {
            domino.update(deltaTime);
        }
    }
    DrawDominoSequence();

    glutSwapBuffers();
}

void reshape(int win_w, int win_h) {
    glViewport(0, 0, win_w, win_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)win_w / win_h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void onMouseClick(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            simulationActive = true; // Start the simulation
            for (auto& domino : dominos) {
                domino.isFalling = true;
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            simulationActive = false; // Reset the simulation
            for (auto& domino : dominos) {
                domino.isFalling = false;
                domino.rotation = 0.0f;
                domino.fallDelay = 0.0f;
            }
        }
    }
}

void DrawDominoSequence() {
    for (const auto& domino : dominos) {
        domino.draw();
    }
}
