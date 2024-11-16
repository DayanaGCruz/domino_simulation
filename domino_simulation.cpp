#include <GL/glut.h> 
#include <iostream> // Console logging

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

void DrawDominoSequence()
{
    // Create a single domino
    Domino domino1(0.0f, 1.0f, 0.0f, 1.0f, 2.0f, 0.5f, 0.784f, 0.635f, 0.784f);

    // Create another domino with different properties
    Domino domino2(2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 0.5f, 0.1f, 0.5f, 0.5f);

    domino1.draw();
    domino2.draw();
}