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
    bool isTriggered;       // Whether the domino has been triggered to fall

    // Constructor
    Domino(GLfloat xPos, GLfloat yPos, GLfloat zPos,
           GLfloat w, GLfloat h, GLfloat d,
           GLfloat red, GLfloat green, GLfloat blue)
        : x(xPos), y(yPos), z(zPos),
          width(w), height(h), depth(d),
          rotation(0.0f), r(red), g(green), b(blue),
          isFalling(false), isTriggered(false) {}

    // Update the Domino's animation
    void update(float deltaTime, float nextDominoX, float nextDominoZ, bool nextDominoFalling, bool triggerNext) {
    if (isFalling) {
        rotation += 100.0f * deltaTime; // Rotate 100 degrees/sec
        if (rotation > 90.0f) rotation = 90.0f; // Clamp at 90 degrees

        // If this domino has fallen enough to trigger the next one
        if (rotation > 45.0f && !nextDominoFalling) {
            isTriggered = true; // Mark this domino as triggered
        }
    }

    // Trigger the fall of the next domino if this one has fallen enough to touch it
    if (isTriggered && !nextDominoFalling && triggerNext) {
        float distanceToNext = sqrt(pow(nextDominoX - x, 2) + pow(nextDominoZ - z, 2));
        if (distanceToNext < (width + 0.2f)) {  // Domino has fallen enough to touch the next one
            // Trigger the next domino to fall
            isFalling = true;
            isTriggered = false; // Reset this domino's trigger
        }
    }
    }




    // Draw the Domino
    void draw() const {
        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(rotation, 1.0f, 0.0f, 0.0f); // Rotate around X-axis for front-facing fall
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
bool simulationActive = false; // Track if the simulation has started
int fallingDominoIndex = 0;  // Index of the domino that is currently falling

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

        // Add domino with position, color, etc.
        dominos.emplace_back(x, 1.0f, z, 0.7f, 2.0f, 0.2f, r, g, b);
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

    gluLookAt(10.0, 10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Update and draw each domino
    for (size_t i = 0; i < dominos.size(); ++i) {
        bool nextDominoFalling = (i + 1 < dominos.size()) ? dominos[i + 1].isFalling : false;
        bool triggerNext = (i + 1 < dominos.size()) ? true : false; // Can this domino trigger the next one?

        dominos[i].update(deltaTime, (i + 1 < dominos.size()) ? dominos[i + 1].x : 0.0f,
                          (i + 1 < dominos.size()) ? dominos[i + 1].z : 0.0f, nextDominoFalling, triggerNext);

        dominos[i].draw();
    }

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
            simulationActive = true;
            fallingDominoIndex = 0;
            dominos[fallingDominoIndex].isFalling = true;
        } else if (button == GLUT_RIGHT_BUTTON) {
            simulationActive = false;
            fallingDominoIndex = 0;
            for (auto& domino : dominos) {
                domino.rotation = 0.0f;
                domino.isFalling = false;
                domino.isTriggered = false;
            }
        }
    }
}
