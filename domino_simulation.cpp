#include <GL/glut.h> // Windowing & Scene Rendering 
#include <iostream> // Console logging
#include <cmath> 

class Domino 
{
    public:
        GLfloat x, y, z; // Position
        GLfloat width, height, depth; // Dimensions
        GLfloat rotation; // Rotation angle
        float r, g, b; // RGB color

        // Constructor
        Domino
        (
            GLfloat xPos, GLfloat yPos, GLfloat zPos,
            GLfloat w, GLfloat h, GLfloat d,
            GLfloat red, GLfloat green, GLfloat blue
        ): x(xPos), y(yPos), z(zPos),
            width(w), height(h), depth(d),
            rotation(0.0f),
            r(red), g(green), b(blue) {}

        // Draw the Domino
        void draw() const
        {
            glPushMatrix();
            glTranslatef(x,y,z);
            glRotatef(rotation, 0.0f,0.0f,1.0f);
            glColor3f(r,g,b);
            drawRectangularPrism(width, height, depth);
            glPopMatrix();
        }
    private:
        // Utility function to draw a rectangular prism
        static void drawRectangularPrism(GLfloat w, GLfloat h, GLfloat d)
        {
            // Halve params
            GLfloat hw = w / 2.0f;
            GLfloat hh = h / 2.0f;
            GLfloat hd = d / 2.0f;

            glBegin(GL_QUADS);

            // Front face
            glVertex3f(-hw, -hh,  hd); glVertex3f( hw, -hh,  hd); 
            glVertex3f( hw,  hh,  hd); glVertex3f(-hw,  hh,  hd);

            // Back face
            glVertex3f(-hw, -hh, -hd); glVertex3f(-hw,  hh, -hd); 
            glVertex3f( hw,  hh, -hd); glVertex3f( hw, -hh, -hd);

            // Left face
            glVertex3f(-hw, -hh, -hd); glVertex3f(-hw, -hh,  hd); 
            glVertex3f(-hw,  hh,  hd); glVertex3f(-hw,  hh, -hd);

            // Right face
            glVertex3f( hw, -hh, -hd); glVertex3f( hw,  hh, -hd); 
            glVertex3f( hw,  hh,  hd); glVertex3f( hw, -hh,  hd);

            // Top face
            glVertex3f(-hw,  hh,  hd); glVertex3f( hw,  hh,  hd); 
            glVertex3f( hw,  hh, -hd); glVertex3f(-hw,  hh, -hd);

            // Bottom face
            glVertex3f(-hw, -hh, -hd); glVertex3f( hw, -hh, -hd); 
            glVertex3f( hw, -hh,  hd); glVertex3f(-hw, -hh,  hd);

            glEnd();
        }
};

void DrawDominoSequence();

GLint WIN_WIDTH, WIN_HEIGHT = 800; // Window width & height

void display(); // Render the scene
void reshape(int win_w, int win_h); // Handle dynamic window resizing 
void init(); // Set up OpenGl state


// Invoke custom compile command default task : Ctrl + Shift + B
int main(int argc, char** argv)
{
    std::cout << "Domino Simulation" << std::endl;

    // Init. GLUT
    glutInit(&argc, argv);
                    // Double Buffer RGB Color  Depth Buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Domino Simulation");

    init();

    // Register callback functions
    glutDisplayFunc(display); // Display
    glutReshapeFunc(reshape); // Reshape
    glutIdleFunc(display); // Continious redraw of scene

    // Enter GLUT event processing loop
    glutMainLoop();

    return 0;
}

void init()
{
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glClearColor(0.784f, 0.635f, 0.784f,1.0f); // Set clear color
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glLoadIdentity(); // Reset model-view matrix

    gluLookAt
    (
        5.0, 3.0, 5.0, // Eye position
        1.0, 0.0, 0.0,  // Look-At point
        0.0, 1.0, 0.0 // Up vector
    );

    DrawDominoSequence();

    glutSwapBuffers(); // Swap buffers for double buffering

}

void reshape (int win_w, int win_h)
{
    glViewport(0,0, win_w, win_h); // Set viewport to cover new window
    glMatrixMode(GL_PROJECTION); // Switch to projection matrix
    glLoadIdentity();
    gluPerspective(45.0, (float)win_w / win_h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW); // Switch to model-view

}

void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
    int i = int(h * 6.0f);
    float f = (h * 6.0f) - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
}


void DrawDominoSequence() {
    const int numDominos = 36; // Number of dominos in the circle
    const float radius = 5.0f; // Radius of the circle
    const float height = 2.0f; // Height of each domino
    const float width = 0.7f;  // Width of each domino
    const float depth = 0.2f;  // Depth of each domino

    for (int i = 0; i < numDominos; ++i) {
        float angle = i * (360.0f / numDominos); // Angle for each domino
        float rad = angle * (M_PI / 180.0f);     // Convert to radians

        // Calculate position based on circle
        float x = radius * cos(rad);
        float z = radius * sin(rad);

        // Calculate rainbow color (HSV to RGB conversion)
        float hue = angle / 360.0f; // Hue value (normalized)
        float r, g, b;
        HSVtoRGB(hue, 1.0f, 1.0f, r, g, b); // Full saturation and value

        // Create and draw the domino
        Domino domino(x, height / 2.0f, z, width, height, depth, r, g, b);
        domino.draw();
    }
}
