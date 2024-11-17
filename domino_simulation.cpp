#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

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
class Domino;

bool isColliding(const Domino& d1, const Domino& d2);
// Domino class
class Domino {
public:
    GLfloat x, y, z;        // Position
    GLfloat width, height, depth; // Dimensions
    GLfloat rotation;       // Rotation angle
    float r, g, b;          // RGB color
    bool isFalling;         // Falling state
    bool isEnteringCircle;  // Is the domino still entering the circle?
    float speed;            // Speed of movement into the circle
    float entryDelay;       // Time delay before the domino starts moving
    float timeElapsed;      // Tracks elapsed time since simulation start

    // Constructor
    Domino(GLfloat xPos, GLfloat yPos, GLfloat zPos,
           GLfloat w, GLfloat h, GLfloat d,
           GLfloat red, GLfloat green, GLfloat blue,
           float delay)
        : x(xPos), y(yPos), z(zPos),
          width(w), height(h), depth(d),
          rotation(0.0f), r(red), g(green), b(blue),
          isFalling(false), isEnteringCircle(true),
          speed(3.0f), entryDelay(delay), timeElapsed(0.0f) {}

// Update the Domino's animation
void update(float deltaTime, float targetX, float targetZ, std::vector<Domino>& dominos, size_t index) {
    timeElapsed += deltaTime;

    if (timeElapsed < entryDelay) {
        // Wait until the entry delay has passed
        return;
    }

    if (isEnteringCircle) {
        // Animate moving from the left into the circle
        float distanceToTarget = sqrt(pow(targetX - x, 2) + pow(targetZ - z, 2));
        if (distanceToTarget < 0.1f) {
            // Stop moving when close to the target
            isEnteringCircle = false;
            y = 1.0f; // Reset to ground level
        } else {
            // Move towards the target position
            x += speed * deltaTime * (targetX - x) / distanceToTarget;
            z += speed * deltaTime * (targetZ - z) / distanceToTarget;

            // Apply subtle bounce effect
            y = fabs(sin(distanceToTarget * 2.0f)) * 1.0f; // Reduced bounce speed and height
        }
    }

    if (isFalling) {
        std::cout << "Domino " << index << " is falling!\n\t W: " << dominos[index].width << " H: " << dominos[index].height<< " D: "<< dominos[index].depth << "\t X:" << dominos[index].x << " Y: " << dominos[index].y << " Z: " << dominos[index].z << std::endl;
        rotation += 100.0f * deltaTime;

        if (rotation > 90.0f) {
            rotation = 90.0f; 
        }

        // Check for collision with the next domino in the array (neighboring domino)
        size_t nextIndex = (index + 1) % dominos.size(); // Wrap around if it reaches the end
        size_t prevIndex = (index -1) % dominos.size();
        // If the domino is rotating and its rotation reaches -90 degrees, check for collision
        std::cout << "Checking collision of Domino " << index << " with Domino " << nextIndex << "\n\t W: " << dominos[nextIndex].width << " H: " << dominos[nextIndex].height<< " D: "<< dominos[nextIndex].depth << "\t X:" << dominos[nextIndex].x << " Y: " << dominos[nextIndex].y << " Z: " << dominos[nextIndex].z << std::endl;
        std::cout << "Checking collision of Domino " << index << " with Domino " << prevIndex << "\n\t W: " << dominos[prevIndex].width << " H: " << dominos[prevIndex].height<< " D: "<< dominos[prevIndex].depth << "\t X:" << dominos[prevIndex].x << " Y: " << dominos[prevIndex].y << " Z: " << dominos[prevIndex].z << std::endl;
        if (isColliding(*this, dominos[nextIndex]) && !dominos[nextIndex].isFalling) {
            std::cout << "COLLISION DETECTED" << std::endl;

            // Only start falling the next domino if it is not already falling
            dominos[nextIndex].isFalling = true;

            // Stop further rotation once collision occurs
            rotation = 90.0f; 
        }
        /*
        else if(isColliding(*this, dominos[prevIndex]) && !dominos[prevIndex].isFalling)
        {
                        std::cout << "COLLISION DETECTED" << std::endl;
            std::cout << "Domino " << nextIndex << " is falling!" << std::endl;

            // Only start falling the next domino if it is not already falling
            dominos[prevIndex].isFalling = true;

            // Stop further rotation once collision occurs
            rotation = 90.0f; 
        }*/
    }
}
    // Draw the Domino
void draw() const {
    // Set material properties for the domino
    GLfloat matDiffuse[] = {r, g, b, 1.0f}; // Diffuse color
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);

    glPushMatrix();

    // Move to bottom of the domino (translate down by half its height)
    glTranslatef(x, y - height / 2.0f, z);

    // Rotate around the X-axis for front-facing fall (rotation around bottom edge)
    glRotatef(rotation, 1.0f, 0.0f, 0.0f);

    // Translate back to original position (restore the top of the domino to its initial height)
    glTranslatef(0.0f, height / 2.0f, 0.0f);

    // Draw the domino
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

// Function declarations
void init();
void display();
void reshape(int win_w, int win_h);
void onMouseClick(int button, int state, int x, int y);

int main(int argc, char** argv) {
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
        float x = -20.0f;         // Start off-screen to the left
        float z = 0.0f;           // Align in a straight horizontal line
        float hue = i / 36.0f;    // Hue for color
        float r, g, b;
        HSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
        float delay = i * 0.2f;   // Stagger entry with increasing delay
        dominos.emplace_back(x, 3.0f, z, 0.7f, 2.0f, 0.2f, r, g, b, delay);
    }

    glutMainLoop();
    return 0;
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set light properties
    GLfloat lightPos[] = {10.0f, 10.0f, 10.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

    GLfloat lightSpecular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glClearColor(0.784f, 0.635f, 0.784f, 1.0f);
}

void display() {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(15.0, 10.0, 15.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Set target positions for the circle
    std::vector<std::pair<float, float>> circlePositions;
    for (int i = 0; i < 36; ++i) {
        float angle = i * (360.0f / 36);
        float rad = angle * (M_PI / 180.0f);
        circlePositions.emplace_back(5.0f * cos(rad), 5.0f * sin(rad));
    }

    // Update and draw each domino
    for (size_t i = 0; i < dominos.size(); ++i) {
        auto [targetX, targetZ] = circlePositions[i];

        // Update the domino's state, no need for nextDominoFalling
        // Update the domino's state, now passing the index to update function
        dominos[i].update(deltaTime, targetX, targetZ, dominos, i);

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
            for (auto& domino : dominos) {
                domino.isEnteringCircle = false; // Stop the entry animation
                domino.y = 1.0f; // Reset to ground level
            }
            dominos[0].isFalling = true; // Start the falling animation
        } else if (button == GLUT_RIGHT_BUTTON) {
            for (auto& domino : dominos) {
                domino.rotation = 0.0f;
                domino.isFalling = false;
                domino.isEnteringCircle = true; // Restart the entry animation
                domino.x = -20.0f; // Reset to the start position (off-screen)
                domino.y = 3.0f;  // Reset bounce height
                domino.timeElapsed = 0.0f; // Reset the timer
            }
        }
    }
}

bool isColliding(const Domino& d1, const Domino& d2) {
    // Convert width, depth, and height to half-extents for convenience
    glm::vec3 halfSize1 = glm::vec3(d1.width / 2.0f, d1.height / 2.0f, d1.depth / 2.0f);
    glm::vec3 halfSize2 = glm::vec3(d2.width / 2.0f, d2.height / 2.0f, d2.depth / 2.0f);

    // Define rotation matrices based on current rotation angles
    glm::mat4 rotationMatrix1 = glm::rotate(glm::mat4(1.0f), glm::radians(d1.rotation), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(d2.rotation), glm::vec3(1.0f, 0.0f, 0.0f));

    // World corners computation
    glm::vec3 d1Corners[8], d2Corners[8];

    // Generate the corners for d1
    glm::vec3 localCorners1[] = {
        {-halfSize1.x, -halfSize1.y, -halfSize1.z}, {halfSize1.x, -halfSize1.y, -halfSize1.z},
        {halfSize1.x, -halfSize1.y, halfSize1.z},  {-halfSize1.x, -halfSize1.y, halfSize1.z},
        {-halfSize1.x, halfSize1.y, -halfSize1.z}, {halfSize1.x, halfSize1.y, -halfSize1.z},
        {halfSize1.x, halfSize1.y, halfSize1.z},   {-halfSize1.x, halfSize1.y, halfSize1.z}
    };

    for (int i = 0; i < 8; i++) {
        d1Corners[i] = glm::vec3(rotationMatrix1 * glm::vec4(localCorners1[i], 1.0f)) + glm::vec3(d1.x, d1.y, d1.z);
    }

    // Generate the corners for d2
    glm::vec3 localCorners2[] = {
        {-halfSize2.x, -halfSize2.y, -halfSize2.z}, {halfSize2.x, -halfSize2.y, -halfSize2.z},
        {halfSize2.x, -halfSize2.y, halfSize2.z},  {-halfSize2.x, -halfSize2.y, halfSize2.z},
        {-halfSize2.x, halfSize2.y, -halfSize2.z}, {halfSize2.x, halfSize2.y, -halfSize2.z},
        {halfSize2.x, halfSize2.y, halfSize2.z},   {-halfSize2.x, halfSize2.y, halfSize2.z}
    };

    for (int i = 0; i < 8; i++) {
        d2Corners[i] = glm::vec3(rotationMatrix2 * glm::vec4(localCorners2[i], 1.0f)) + glm::vec3(d2.x, d2.y, d2.z);
    }

    // Compute AABB bounds for d1 and d2
    glm::vec3 d1Min = d1Corners[0], d1Max = d1Corners[0];
    glm::vec3 d2Min = d2Corners[0], d2Max = d2Corners[0];

    for (int i = 1; i < 8; i++) {
        d1Min = glm::min(d1Min, d1Corners[i]);
        d1Max = glm::max(d1Max, d1Corners[i]);
        d2Min = glm::min(d2Min, d2Corners[i]);
        d2Max = glm::max(d2Max, d2Corners[i]);
    }

    // Debugging prints for AABB bounds
    std::cout << "Domino A AABB: Min(" << d1Min.x << ", " << d1Min.y << ", " << d1Min.z
              << ") Max(" << d1Max.x << ", " << d1Max.y << ", " << d1Max.z << ")\n";
    std::cout << "Domino B AABB: Min(" << d2Min.x << ", " << d2Min.y << ", " << d2Min.z
              << ") Max(" << d2Max.x << ", " << d2Max.y << ", " << d2Max.z << ")\n";

    // Check for overlap in all three dimensions (AABB test)
    bool collision = (d1Min.x <= d2Max.x && d1Max.x >= d2Min.x) &&
                     (d1Min.y <= d2Max.y && d1Max.y >= d2Min.y) &&
                     (d1Min.z <= d2Max.z && d1Max.z >= d2Min.z);

    if (collision) {
        std::cout << "Collision detected between dominos!\n";
    } else {
        std::cout << "No collision detected.\n";
    }

    return collision;
}


