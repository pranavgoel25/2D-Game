#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include<map>
#include<string>
#include<stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;
    GLuint MatrixID;
} Matrices;

GLuint programID;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
//    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
//    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
//    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    cout<<endl;
    cout<<"Why you close game? :( "<<endl;
    cout<<endl;
    cout<<endl;
    glfwTerminate();
    //    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
            0,                  // attribute 0. Vertices
            3,                  // size (x,y,z)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
            1,                  // attribute 1. Color
            3,                  // size (r,g,b)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

double diff;
float l_speed = 0.30;
double current_time;
double old_time;
int score=0;
int penalty=5;
double brickspeed=0.01;
int right_press=0;
int zoomlevel=0;
int level=1;
int movered=0;
int movegreen=0;
float panx=0;
float pany=0;

typedef struct Color
{
    float r,g,b;
}color;

typedef struct Sprite
{
    string name;
    color c;
    float x,y;
    VAO* object;
    float height;
    float width;
    int islaser;
    int isbox;
    int status;
    float angle;
    float xspeed;
    float yspeed;
    string component;
}Sprite;


map <string,Sprite> boxes;  //store bottom boxes and laser boxes
map <string,Sprite> laser;  //store laser
map <string,Sprite> brick;  //store brick
map <string,Sprite> scoreboard;  //store scoreboard
map <string,Sprite> mirror;  //store mirrors
map <string,Sprite> background; //store background
map <string,Sprite> moving; //store moving objects
map <string, Sprite> speed; //store speed rectangles

void printn();
void printm();

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Function is called first on GLFW_PRESS.

    if (action == GLFW_RELEASE) 
    {
        switch (key) 
        {
            case GLFW_KEY_RIGHT_ALT:
                movegreen=0;
                break;
            case GLFW_KEY_RIGHT_CONTROL:
                movered=0;
                break;
            case GLFW_KEY_LEFT:                 
                if(movered==1)                  //move red box left
                {
                    boxes["redbox"].x-=0.1;
                    break;
                }
                else if(movegreen==1)           //move green box left
                    boxes["greenbox"].x-=0.1;
                break;
            case GLFW_KEY_RIGHT:
                if(movered==1)                  //move red box right
                {
                    boxes["redbox"].x+=0.1;
                    break;
                }
                else if(movegreen==1)           //move green box right
                    boxes["greenbox"].x+=0.1;
                break;
            case GLFW_KEY_J:
                panx+=0.1;
                break;
            case GLFW_KEY_L:
                panx-=0.1;
                break;
            case GLFW_KEY_I:
                pany-=0.1;
                break;
            case GLFW_KEY_K:
                pany+=0.1;
                break;
            case GLFW_KEY_S:                    //move cannon up
                boxes["laserbox"].y+=0.1;
                boxes["laserbox2"].y+=0.1;
                laser["laser"].y+=0.1;
                break;
            case GLFW_KEY_F:                    //move cannon down
                boxes["laserbox"].y-=0.1;
                boxes["laserbox2"].y-=0.1;
                laser["laser"].y-=0.1;
                break;
            case GLFW_KEY_A:                    //increase angle of cannon
                boxes["laserbox2"].angle+=10;
                if(laser["laser"].status==0)
                    laser["laser"].angle+=10;
                break;
            case GLFW_KEY_D:                    //decrease angle of cannon
                boxes["laserbox2"].angle-=10;
                if(laser["laser"].status==0)
                    laser["laser"].angle-=10;
                break;
            case GLFW_KEY_N:                    //increase speed of bricks
                printn();
                break;
            case GLFW_KEY_M:                    //decrease speed of bricks
                printm();
                break;
            case GLFW_KEY_UP:                   //zoom in
                zoomlevel+=1;
                break;
            case GLFW_KEY_DOWN:                 //zoom out
                zoomlevel-=1;
                if(zoomlevel<=0)
                    zoomlevel=0;
                break;
            default:
                break;
        }
    }
    else if (action == GLFW_PRESS) 
    {
        switch (key) 
        {
            case GLFW_KEY_RIGHT_CONTROL:
                movered=1;
                break;
            case GLFW_KEY_RIGHT_ALT:
                movegreen=1;
                break;
            case GLFW_KEY_ESCAPE:
                quit(window);
                break;
            case GLFW_KEY_SPACE:                //shoot laser
                laser["laser"].status=1;
                break;
            default:
                break;
        }
    }
    else if (action == GLFW_REPEAT)
    {
        switch(key)
        {
            case GLFW_KEY_J:
                panx+=0.1;
                break;
            case GLFW_KEY_L:
                panx-=0.1;
                break;
            case GLFW_KEY_I:
                pany-=0.1;
                break;
            case GLFW_KEY_K:
                pany+=0.1;
                break;
            case GLFW_KEY_A:                    //increase angleof cannon
                boxes["laserbox2"].angle+=10;
                if(laser["laser"].status==0)
                    laser["laser"].angle+=10;
                break;
            case GLFW_KEY_D:                    //decrease angle of cannon
                boxes["laserbox2"].angle-=10;
                if(laser["laser"].status==0)
                    laser["laser"].angle-=10;
                break;
            case GLFW_KEY_S:                    //move cannon up
                boxes["laserbox"].y+=0.1;
                boxes["laserbox2"].y+=0.1;
                laser["laser"].y+=0.1;
                break;
            case GLFW_KEY_F:                    //move cannon down
                boxes["laserbox"].y-=0.1;
                boxes["laserbox2"].y-=0.1;
                laser["laser"].y-=0.1;
                break;
            case GLFW_KEY_LEFT:                 
                if(movered==1)                  //move red box left
                {
                    boxes["redbox"].x-=0.1;
                    break;
                }
                else if(movegreen==1)           //move green box left
                    boxes["greenbox"].x-=0.1;
                break;
            case GLFW_KEY_RIGHT:
                if(movered==1)                  //move red box right
                {
                    boxes["redbox"].x+=0.1;
                    break;
                }
                else if(movegreen==1)           //move green box right
                    boxes["greenbox"].x+=0.1;
                break;
            default:
                break;
        }
    }
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
    switch (key) {
        case 'Q':
        case 'q':
            quit(window);
            break;
        default:
            break;
    }
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:                //left mouse button
            if (action == GLFW_RELEASE)
                laser["laser"].status=1;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:               //right mouse button
            if (action == GLFW_PRESS)               //drag objects around
            {
                right_press = 1;
            }
            else if(action == GLFW_RELEASE)
                right_press=0;
            break;
        default:
            break;
    }
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
       is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    GLfloat fov = 90.0f;

    // sets the viewport of openGL renderer
    glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);
    // set the projection matrix as perspective
    /* glMatrixMode (GL_PROJECTION);
       glLoadIdentity ();
       gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
    // Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

VAO *line, *rectangle,*rectangleb;

// Creates the triangle object used in this sample code
void createLine ()
{
    /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

    /* Define vertex array as used in glBegin (GL_TRIANGLES) */
    static const GLfloat vertex_buffer_data [] = {
        -4, 0,0, // vertex 0
        0,0,0, // vertex 1
        4,0,0, // vertex 2
    };

    static const GLfloat color_buffer_data [] = {
        1,1,0, // color 0
        0,1,1, // color 1
        1,0,1, // color 2
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    line = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
}

// Creates the rectangle object used in this sample code
void createRectangle (string name, float x,float y, color A,color B,color C,color D, float height, float width,int isbox,int islaser,string component) 
{
    // GL3 accepts only Triangles. Quads are not supported
    float w = width/2.0;
    float h = height/2.0;

    GLfloat vertex_buffer_data [] = 
    {
        -w,-h,0, // vertex 1
        w,-h,0, // vertex 2
        w, h,0, // vertex 3

        w,h,0, // vertex 3
        -w,h,0, // vertex 4
        -w,-h,0  // vertex 1
    };

    GLfloat color_buffer_data [] = 
    {
        A.r,A.g,A.b, // color 1
        B.r,B.g,B.b, // color 2
        C.r,C.g,C.b, // color 3

        C.r,C.g,C.b, // color 3
        D.r,D.g,D.b, // color 4
        A.r,A.g,A.b  // color 1
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);

    Sprite prsprite = {};

    prsprite.name = name;
    prsprite.c = A;
    prsprite.x = x;
    prsprite.y = y;
    prsprite.height = height;
    prsprite.width = width;
    prsprite.object = rectangle;
    prsprite.isbox=isbox;
    prsprite.islaser=islaser;
    prsprite.status=0;
    if(component=="redbrick" || component == "blackbrick" || component == "greenbrick")
    {
        prsprite.yspeed=brickspeed;
        prsprite.status=0;
        prsprite.component = component;
        brick[name]=prsprite;
    }
    else if(component=="score")
    {
        prsprite.status=0;
        scoreboard[name]=prsprite;
    }
    else if(name=="laser")
    {
        prsprite.status=0;
        prsprite.angle=0;
        prsprite.xspeed = 0.3;
        prsprite.yspeed = 0.2;
        laser[name]=prsprite;
    }
    else if(component=="mirror")
    {
        if(name=="mirror1")
            prsprite.angle = 135;
        else if(name=="mirror2")
            prsprite.angle = 45;
        mirror[name]=prsprite;
    }
    else if(component == "background")
    {
        prsprite.angle=45;
        background[name]=prsprite;
    }
    else if(component == "moving")
    {
        prsprite.status=0;
        moving[name]=prsprite;
    }
    else if(component == "speed")
    {
        speed[name]=prsprite;
    }
    else
    {
        prsprite.status=0;
        boxes[name]=prsprite;
    }

}


void createCircle (string name, color C, float x, float y, float r, int NoOfParts, string component)
{
    int parts = NoOfParts;
    float radius = r;
    GLfloat vertex_buffer_data[parts*9];
    GLfloat color_buffer_data[parts*9];
    int i,j;
    float angle=(2*M_PI/parts);
    float current_angle = 0;
    for(i=0;i<parts;i++)
    {
        for(j=0;j<3;j++)
        {
            color_buffer_data[i*9+j*3]=C.r;
            color_buffer_data[i*9+j*3+1]=C.g;
            color_buffer_data[i*9+j*3+2]=C.b;
        }
        vertex_buffer_data[i*9]=0;
        vertex_buffer_data[i*9+1]=0;
        vertex_buffer_data[i*9+2]=0;
        vertex_buffer_data[i*9+3]=radius*cos(current_angle);
        vertex_buffer_data[i*9+4]=radius*sin(current_angle);
        vertex_buffer_data[i*9+5]=0;
        vertex_buffer_data[i*9+6]=radius*cos(current_angle+angle);
        vertex_buffer_data[i*9+7]=radius*sin(current_angle+angle);
        vertex_buffer_data[i*9+8]=0;
        current_angle+=angle;
    }
    VAO* circle;
    circle = create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_FILL);
    Sprite prsprite = {};
    prsprite.c = C;
    prsprite.name = name;
    prsprite.object = circle;
    prsprite.x=x;
    prsprite.y=y;
    prsprite.angle = 0;
    prsprite.height=2*r;
    prsprite.width=2*r;

    background[name]=prsprite;
}

float camera_rotation_angle = 90;

void printn()
{
    brickspeed+=0.01;
    level+=1;
    if(brickspeed>0.03)
    {
        brickspeed=0.03;
        level=3;
    }
    for(map<string,Sprite>::iterator it=brick.begin();it!=brick.end();it++)
    {
        string current=it->first;
        brick[current].yspeed=brickspeed;
    }
}

void printm()
{
    level-=1;
    brickspeed-=0.01;
    if(brickspeed<0.01)
    {
        brickspeed=0.01;
        level=1;
    }
    for(map<string,Sprite>::iterator it=brick.begin();it!=brick.end();it++)
    {
        string current=it->first;
        brick[current].yspeed=brickspeed;
    }
}

void checkcollision(string name, Sprite laser1)
{
    float t1;
    t1 = sqrt(((brick[name].x-laser1.x)*(brick[name].x - laser1.x)) + ((brick[name].y - laser1.y)*(brick[name].y - laser1.y)));
    float t2;
    t2 = (brick[name].width/2.0) + (laser1.width/2.0);
    float t3;
    t3 = (brick[name].height/2.0) + (laser1.height/2.0);
    if((t1-t2<0.0) && (t1-t3<0.0))
    {
        laser["laser"].status=0;
        laser["laser"].x = boxes["laserbox2"].x;
        laser["laser"].y = boxes["laserbox2"].y;
        brick[name].status=1;
        if(brick[name].component == "blackbrick")
        {
            score+=1;
        }
        return;
    }

}

void checkbasketcollect(string name, float x,float y)
{
    if(brick[name].x >= x-0.5 && brick[name].x <= x + 0.5)
    {
        brick[name].status=1;
        score+=1;
    }
    else
        brick[name].status=0;
}

void lightitup(int sc,int bit)
{
    if(bit==0)
    {
        scoreboard["top1"].status=0;
        scoreboard["center1"].status=0;
        scoreboard["bottom1"].status=0;
        scoreboard["ul1"].status=0;
        scoreboard["ur1"].status=0;
        scoreboard["br1"].status=0;
        scoreboard["bl1"].status=0;
        if(sc==2 || sc==3 || sc ==5 ||sc ==6 || sc ==7||sc ==8||sc==9||sc==0)
            scoreboard["top1"].status=1;
        if(sc==2||sc==3||sc==4||sc==5||sc==6||sc==8||sc==9)
            scoreboard["center1"].status=1;
        if(sc==2||sc==3||sc==5||sc==6||sc==8||sc==9||sc==0)
            scoreboard["bottom1"].status=1;
        if(sc==4||sc==5||sc==6||sc==8||sc==9||sc==0)
            scoreboard["ul1"].status=1;
        if(sc==1||sc==2||sc==3||sc==4||sc==7||sc==8||sc==9||sc==0)
            scoreboard["ur1"].status=1;
        if(sc==2||sc==6||sc==8||sc==0)
            scoreboard["bl1"].status=1;
        if(sc==1||sc==3||sc==4||sc==5|sc==6||sc==7||sc==8||sc==9||sc==0)
            scoreboard["br1"].status=1;
    }
    else if(bit==1)
    {
        scoreboard["top2"].status=0;
        scoreboard["center2"].status=0;
        scoreboard["bottom2"].status=0;
        scoreboard["ul2"].status=0;
        scoreboard["ur2"].status=0;
        scoreboard["br2"].status=0;
        scoreboard["bl2"].status=0;
        if(sc==2 || sc==3 || sc ==5 ||sc ==6 || sc ==7||sc ==8||sc==9||sc==0)
            scoreboard["top2"].status=1;
        if(sc==2||sc==3||sc==4||sc==5||sc==6||sc==8||sc==9)
            scoreboard["center2"].status=1;
        if(sc==2||sc==3||sc==5||sc==6||sc==8||sc==9||sc==0)
            scoreboard["bottom2"].status=1;
        if(sc==4||sc==5||sc==6||sc==8||sc==9||sc==0)
            scoreboard["ul2"].status=1;
        if(sc==1||sc==2||sc==3||sc==4||sc==7||sc==8||sc==9||sc==0)
            scoreboard["ur2"].status=1;
        if(sc==2||sc==6||sc==8||sc==0)
            scoreboard["bl2"].status=1;
        if(sc==1||sc==3||sc==4||sc==5|sc==6||sc==7||sc==8||sc==9||sc==0)
            scoreboard["br2"].status=1;
    }
    else if (bit==2)
    {
        scoreboard["top3"].status=0;
        scoreboard["center3"].status=0;
        scoreboard["bottom3"].status=0;
        scoreboard["ul3"].status=0;
        scoreboard["ur3"].status=0;
        scoreboard["br3"].status=0;
        scoreboard["bl3"].status=0;
        if(sc==2 || sc==3 || sc ==5 ||sc ==6 || sc ==7||sc ==8||sc==9||sc==0)
            scoreboard["top3"].status=1;
        if(sc==2||sc==3||sc==4||sc==5||sc==6||sc==8||sc==9)
            scoreboard["center3"].status=1;
        if(sc==2||sc==3||sc==5||sc==6||sc==8||sc==9||sc==0)
            scoreboard["bottom3"].status=1;
        if(sc==4||sc==5||sc==6||sc==8||sc==9||sc==0)
            scoreboard["ul3"].status=1;
        if(sc==1||sc==2||sc==3||sc==4||sc==7||sc==8||sc==9||sc==0)
            scoreboard["ur3"].status=1;
        if(sc==2||sc==6||sc==8||sc==0)
            scoreboard["bl3"].status=1;
        if(sc==1||sc==3||sc==4||sc==5|sc==6||sc==7||sc==8||sc==9||sc==0)
            scoreboard["br3"].status=1;
    }

}

void checkbaskets()
{
    float diff;
    diff = abs(boxes["redbox"].x - boxes["greenbox"].x);
    if(diff<1.0)
    {
        boxes["redbox"].status=1;
        boxes["greenbox"].status=1;
    }
    else
    {
        boxes["redbox"].status=0;
        boxes["greenbox"].status=0;
    }

}

int checklasermirror(string lname, string mname)
{
    float t1;
    t1 = sqrt((laser["laser"].x - mirror[mname].x)*(laser["laser"].x - mirror[mname].x) + (laser["laser"].y - mirror[mname].y)*(laser["laser"].y - mirror[mname].y));
    float t2;
    t2 = (laser["laser"].height/2.0) + (mirror[mname].height/2.0);
    float t3;
    t3 = (laser["laser"].width/2.0) + (mirror[mname].width/2.0);
    if((t1-t2<0.0) && (t1-t3<0.0))
        return 1;
    else
        return 0;
}

int chacklasermove()
{
    float t1;
    t1 = sqrt((laser["laser"].x - moving["move1"].x)*(laser["laser"].x - moving["move1"].x) + (laser["laser"].y - moving["move1"].y)*(laser["laser"].y - moving["move1"].y));
    float t2;
    t2 = (moving["move1"].height/2.0) + (laser["laser"].height/2.0);
    float t3;
    t3 = (moving["move1"].width/2.0) + (laser["laser"].width/2.0);
    if(t1<t2 && t1<t3)
        return 1;
    else
        return 0;
}


/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw (GLFWwindow* window)
{
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    //  Don't change unless you are sure!!
    Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    //  Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    //  Don't change unless you are sure!!
    glm::mat4 MVP;	// MVP = Projection * View * Model

    // Load identity to model matrix
    Matrices.model = glm::mat4(1.0f);

    /* Render your scene */

    glm::mat4 translateLine = glm::translate (glm::vec3(0.0f+panx, -2.8f+pany,0.0f)); // glTranslatef
    glm::mat4 scaleLine;
    if(zoomlevel==0)
        scaleLine = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
    else if(zoomlevel!=0)
        scaleLine = glm::scale (glm::vec3((1.05f*zoomlevel),1.05f*zoomlevel,1.05f*zoomlevel));
    glm::mat4 lineTransform = scaleLine * translateLine;
    Matrices.model *= lineTransform; 
    MVP = VP * Matrices.model; // MVP = p * V * M

    // COMMENT- Don't change unless you are sure!!
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // COMMENT- draw3DObject draws the VAO given to it using current MVP matrix
    draw3DObject(line);

    double newx,newy;



    // COMMENT- Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
    // COMMENT- glPopMatrix ();
    for(map<string,Sprite>::iterator it=boxes.begin();it!=boxes.end();it++)
    {
        glfwGetCursorPos(window, &newx, &newy);
        newx = newx/75 - 4;
        newy = (-1*newy)/75 + 4;
        
        string current = it->first;
        glm::mat4 translateRectangle;

        Matrices.model = glm::mat4(1.0f);
        if(boxes[current].isbox==1)
        {
            if(right_press==1)
            {
                if(boxes[current].x-0.5 <= newx && boxes[current].x + 0.5 >= newx && boxes[current].y - 0.5 <= newy && boxes[current].y + 0.5 >= newy)
                {
                    boxes[current].x = newx;
                }
            }
            if(boxes[current].x>=3.5-panx)   //keep the box in the frame
            {
                boxes[current].x=3.5-panx;
            }
            if(boxes[current].x<= -3.5)
                boxes[current].x= -3.5;
            translateRectangle = glm::translate (glm::vec3(boxes[current].x+panx,boxes[current].y+pany,0.0));// glTranslatef
        }
        else if(boxes[current].islaser==1)
        {
            if(right_press==1)
            {
                if(boxes[current].x-0.5 <= newx && boxes[current].x + 0.5 >= newx && boxes[current].y - 0.5 <= newy && boxes[current].y + 0.5 >= newy)
                {
                    boxes["laserbox"].y = newy;
                    boxes["laserbox2"].y = newy;
                }
            
                double angtemp;
                angtemp = atan(newy/newx)*180/M_PI;
                boxes["laserbox2"].angle = angtemp;
            }
            if(boxes[current].y>=2.25)
                boxes[current].y=2.25;
            if(boxes[current].y<= -2.3)
                boxes[current].y= -2.3;
            translateRectangle = glm::translate (glm::vec3(boxes[current].x+panx,boxes[current].y+pany,0.0));// glTranslatef
            if(boxes[current].name=="laserbox2")
            {
                glm::mat4 rotateRectangle = glm::rotate((float)(boxes["laserbox2"].angle*M_PI/180.0f),glm::vec3(0,0,1));
                translateRectangle*=rotateRectangle;
            }
        }
        glm::mat4 scaleRectangle;
        if(zoomlevel==0)
            scaleRectangle = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
        else if(zoomlevel!=0)
            scaleRectangle = glm::scale (glm::vec3(1.05f*zoomlevel,1.05f*zoomlevel,1.05f*zoomlevel));
        scaleRectangle *= translateRectangle;
        Matrices.model *= (scaleRectangle);
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // draw3DObject draws the VAO given to it using current MVP matrix
        draw3DObject(boxes[current].object);
    }
    
    for(map<string,Sprite>::iterator it=mirror.begin();it!=mirror.end();it++)
    {
        string current = it->first;
        glm::mat4 translateRectangle;
        Matrices.model = glm::mat4(1.0f);
        glm::mat4 scaleMirror;
        if(zoomlevel==0)
            scaleMirror = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
        else if(zoomlevel!=0)
            scaleMirror = glm::scale (glm::vec3(1.05f*zoomlevel,1.05f*zoomlevel,1.05f*zoomlevel));
        translateRectangle = glm::translate (glm::vec3(mirror[current].x+panx,mirror[current].y+pany,0.0));
        glm::mat4 rotateRectangle = glm::rotate ((float)(mirror[current].angle*M_PI/180.0),glm::vec3(0,0,1));
        scaleMirror *= translateRectangle;
        scaleMirror*=rotateRectangle;
        Matrices.model *= scaleMirror;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(mirror[current].object);
    }

    if(laser["laser"].status==1)
    {
        diff = (current_time - old_time)*60;
        for(map<string,Sprite>::iterator it=mirror.begin();it!=mirror.end();it++)
        {
            string current = it->first;
            if(checklasermirror(laser["laser"].name,mirror[current].name))
            {
                laser["laser"].yspeed = sin(laser["laser"].angle*(M_PI/180))*l_speed;
                laser["laser"].xspeed = cos(laser["laser"].angle*(M_PI/180))*l_speed;
                laser["laser"].x+=diff*laser["laser"].xspeed;
                laser["laser"].y+=diff*laser["laser"].yspeed;
                laser["laser"].angle = laser["laser"].angle + 2*mirror[current].angle;

            }
        }
        laser["laser"].yspeed = sin(laser["laser"].angle*(M_PI/180))*l_speed;
        laser["laser"].xspeed = cos(laser["laser"].angle*(M_PI/180))*l_speed;
        laser["laser"].x+=diff*laser["laser"].xspeed;
        laser["laser"].y+=diff*laser["laser"].yspeed;
        Matrices.model = glm::mat4(1.0f);
        glm::mat4 scaleRectangle;
        if(zoomlevel==0)
            scaleRectangle = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
        else if(zoomlevel!=0)
            scaleRectangle = glm::scale (glm::vec3(1.3f*zoomlevel,1.3f*zoomlevel,1.3f*zoomlevel));
        glm::mat4 translateRectangle = glm::translate (glm::vec3(laser["laser"].x,laser["laser"].y,0.0));
        glm::mat4 rotateRectangle = glm::rotate((float)(laser["laser"].angle*M_PI/180.0f),glm::vec3(0,0,1));
        scaleRectangle*= translateRectangle * rotateRectangle;
        Matrices.model *= (scaleRectangle);
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(laser["laser"].object);
        if(laser["laser"].x>4.0 || laser["laser"].x<-4.0)
        {
            laser["laser"].status=0;
            laser["laser"].x = boxes["laserbox2"].x + panx;
            laser["laser"].y = boxes["laserbox2"].y;
            laser["laser"].angle = boxes["laserbox2"].angle;
        }
        if(laser["laser"].y>4.0 || laser["laser"].y<-4.0)
        {
            laser["laser"].status=0;
            laser["laser"].x = boxes["laserbox2"].x + panx;
            laser["laser"].y = boxes["laserbox2"].y;
            laser["laser"].angle = boxes["laserbox2"].angle;
        }
        int stat=0;
        stat = chacklasermove();
        if(stat)
        {
            laser["laser"].status=0;
            laser["laser"].x = boxes["laserbox2"].x + panx;
            laser["laser"].y = boxes["laserbox2"].y;
            laser["laser"].angle = boxes["laserbox2"].angle;
        }

    }
    else if(laser["laser"].status==0)
    {
        laser["laser"].x = boxes["laserbox2"].x + panx;
        laser["laser"].y = boxes["laserbox2"].y;
        laser["laser"].angle = boxes["laserbox2"].angle;
    }

    checkbaskets();
    
    for(map<string,Sprite>::iterator it=brick.begin();it!=brick.end();it++)
    {
        string current = it->first;
        glm::mat4 translateRectangle;
        Matrices.model = glm::mat4(1.0f);
        if(brick[current].status==0) //check if laser is colliding with the laser
            checkcollision(brick[current].name,laser["laser"]);
        if(brick[current].status==0)
            if(brick[current].y<=-2.8)
                if(brick[current].component=="redbrick" && boxes["redbox"].status==0)   //check if correct basket is collecting the brick
                    checkbasketcollect(brick[current].name,boxes["redbox"].x,boxes["redbox"].y);
                else if(brick[current].component == "greenbrick" && boxes["greenbox"].status==0) 
                    checkbasketcollect(brick[current].name, boxes["greenbox"].x, boxes["greenbox"].y);
                else if(brick[current].component == "blackbrick")
                {
                    penalty-=1;
                    brick[current].status=1;
                }
        if(brick[current].y<=-3.1)  //check if brick is below the baskets. Remove it.
            brick[current].status=1;

        if(brick[current].status!=1)
        {
            brick[current].y-=brick[current].yspeed;
            glm::mat4 scaleRectangle;
            if(zoomlevel==0)
                scaleRectangle = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
            else if(zoomlevel!=0)
                scaleRectangle = glm::scale (glm::vec3(1.3f*zoomlevel,1.3f*zoomlevel,1.3f*zoomlevel));
            translateRectangle = glm::translate (glm::vec3(brick[current].x+panx,brick[current].y+pany,0.0));
            Matrices.model *= scaleRectangle * translateRectangle;
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DObject(brick[current].object);
        }
        else
        {
            int example =1;
        }
    }
    lightitup(score%10,0);
    int temps;
    temps=score/10;
    lightitup(temps,1);
    lightitup(penalty,2);
    for(map<string,Sprite>::iterator it=scoreboard.begin();it!=scoreboard.end();it++)
    {
        string current = it->first;
        glm::mat4 translateRectangle;
        Matrices.model = glm::mat4(1.0f);

        if(scoreboard[current].status==1)
        {
            glm::mat4 scaleRectangle;
            if(zoomlevel==0)
                scaleRectangle = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
            else if(zoomlevel!=0)
                scaleRectangle = glm::scale (glm::vec3(1.1f*zoomlevel,1.1f*zoomlevel,1.1f*zoomlevel));
            translateRectangle = glm::translate (glm::vec3(scoreboard[current].x+panx,scoreboard[current].y+pany,0.0));
            Matrices.model *= scaleRectangle * translateRectangle;
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DObject(scoreboard[current].object);
        }
    }
    for(map<string,Sprite>::iterator it=background.begin();it!=background.end();it++)
    {
        string current = it->first;
        glm::mat4 translateCircle;
        Matrices.model = glm::mat4(1.0f);

        glm::mat4 scaleRectangle;
        if(zoomlevel==0)
            scaleRectangle = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
        else if(zoomlevel!=0)
            scaleRectangle = glm::scale (glm::vec3(1.01f*zoomlevel,1.01f*zoomlevel,1.01f*zoomlevel));
        translateCircle = glm::translate (glm::vec3(background[current].x + panx,background[current].y+pany, 0.0));
        glm::mat4 rotateCircle = glm::rotate((float)(background[current].angle*M_PI/180.0f),glm::vec3(0,0,1));
        scaleRectangle *= translateCircle * rotateCircle;
        Matrices.model *= scaleRectangle;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(background[current].object);
    }

    for(map<string,Sprite>::iterator it=moving.begin();it!=moving.end();it++)
    {
        string current = it->first;
        glm::mat4 translateRectangle;
        Matrices.model = glm::mat4(1.0f);
        if(moving[current].status==0)
            moving[current].y+=0.05;
        else if(moving[current].status==1)
            moving[current].y-=0.05;

        if(moving[current].y>=3.1)
            moving[current].status=1;
        else if(moving[current].y<=-3.1)
            moving[current].status=0;

        glm::mat4 scaleRectangle;
        if(zoomlevel==0)
            scaleRectangle = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
        else if(zoomlevel!=0)
            scaleRectangle = glm::scale (glm::vec3(1.1f*zoomlevel,1.1f*zoomlevel,1.1f*zoomlevel));

        translateRectangle = glm::translate (glm::vec3(moving[current].x+panx,moving[current].y+pany,0.0));
        scaleRectangle *= translateRectangle;
        Matrices.model *= scaleRectangle;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(moving[current].object);
    }
    int s1=1,s2=0,s3=0;
    for(map<string,Sprite>::iterator it=speed.begin();it!=speed.end();it++)
    {
        string current = it->first;
        glm::mat4 translateRectangle;
        Matrices.model = glm::mat4(1.0f);
        if(level>=2)
            s2=1;
        else
            s2=0;
        if(level>2)
            s3=1;
        else
            s3=0;

        glm::mat4 scaleRectangle;
        if(zoomlevel==0)
            scaleRectangle = glm::scale (glm::vec3(1.0f,1.0f,1.0f));
        else if(zoomlevel!=0)
            scaleRectangle = glm::scale (glm::vec3(1.1f*zoomlevel,1.1f*zoomlevel,1.1f*zoomlevel));
        if(speed[current].name=="speed1")
        {
            translateRectangle = glm::translate (glm::vec3(speed[current].x+panx,speed[current].y+pany,0.0));
            scaleRectangle *= translateRectangle;
            Matrices.model *= scaleRectangle;
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DObject(speed[current].object);
        }
        if(speed[current].name=="speed2")
            if(s2==1)
            {
                translateRectangle = glm::translate (glm::vec3(speed[current].x+panx,speed[current].y+pany,0.0));
                scaleRectangle *= translateRectangle;
                Matrices.model *= scaleRectangle;
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(speed[current].object);
            }
        if(speed[current].name == "speed3")
            if(s3==1)
            {
                translateRectangle = glm::translate (glm::vec3(speed[current].x+panx,speed[current].y+pany,0.0));
                scaleRectangle *= translateRectangle;
                Matrices.model *= scaleRectangle;
                MVP = VP * Matrices.model;
                glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
                draw3DObject(speed[current].object);
            }
    }
    
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        //        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Brick Breaker - Pranav Goel", NULL, NULL);

    if (!window) {
        glfwTerminate();
        //        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
       is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
    /* Objects should be created before any other gl function and shaders */
    color SkyBlue = {0,1,1};
    color WhiteShade = {(float)224/255,(float)224/255,(float)224/255};
    color Yellow = {1,1,0};
    color D = {1,1,1};
    color Blue = {0,0,1};
    color Red = {1,0,0};
    color Green = {0,1,0};
    color Black = {0,0,0};
    // Create the models
    createLine (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
    createCircle ("circle1",WhiteShade,2.3,3.0,0.4,15,"m1");
    createRectangle ("star1",1.0,3.5,Yellow,Yellow,Yellow,Yellow, 0.30,0.30,0,0,"background");
    createRectangle ("star2",-1.0,3.5,Yellow,Yellow,Yellow,Yellow, 0.30,0.30,0,0,"background");
    createRectangle ("life",-3.8,3.40,Black,Black,Black,Black,0.20,0.20,0,0,"background");
    createRectangle ("move1",-2.1,0.0,Yellow,Red,Red,Yellow,1.5,0.2,0,0,"moving");
    createRectangle ("redbox",0.6,-3.5,Red,Red,Red,Red,1,1,1,0,"bottom");
    createRectangle ("greenbox",-0.6,-3.5,Green,Green,Green,Green,1,1,1,0,"bottom");
    createRectangle ("laserbox", -3.6,0,Blue,Red,Blue,Red,0.80,0.80,0,1,"laser");
    createRectangle ("laserbox2",-3,0,Red,Blue,Red,Blue,0.40,0.40,0,1,"laser");
    createRectangle ("mirror1",2.5,2.0,SkyBlue,SkyBlue,SkyBlue,SkyBlue,0.3,1.0,0,0,"mirror");
    createRectangle ("mirror2",2.5,-1.0,SkyBlue,SkyBlue,SkyBlue,SkyBlue,0.3,1.0,0,0,"mirror");
    float t1,t2;
    t1 = boxes["laserbox2"].x;
    t2 = boxes["laserbox2"].y;
    
    createRectangle ("top1",3.6,3.8,Blue,Blue,Blue,Blue,0.02,0.4,0,0,"score");
    createRectangle ("center1",3.6,3.3,Blue,Blue,Blue,Blue,0.02,0.4,0,0,"score");
    createRectangle ("bottom1",3.6,2.8,Blue,Blue,Blue,Blue,0.02,0.4,0,0,"score");
    createRectangle ("ul1",3.4,3.55,Blue,Blue,Blue,Blue,0.5,0.02,0,0,"score");
    createRectangle ("ur1",3.8,3.55,Blue,Blue,Blue,Blue,0.5,0.02,0,0,"score");
    createRectangle ("bl1",3.4,3.05,Blue,Blue,Blue,Blue,0.5,0.02,0,0,"score");
    createRectangle ("br1",3.8,3.05,Blue,Blue,Blue,Blue,0.5,0.02,0,0,"score");

    createRectangle ("top2",3.0,3.8,Blue,Blue,Blue,Blue,0.02,0.4,0,0,"score");
    createRectangle ("center2",3.0,3.3,Blue,Blue,Blue,Blue,0.02,0.4,0,0,"score");
    createRectangle ("bottom2",3.0,2.8,Blue,Blue,Blue,Blue,0.02,0.4,0,0,"score");
    createRectangle ("ul2",2.8,3.55,Blue,Blue,Blue,Blue,0.5,0.02,0,0,"score");
    createRectangle ("ur2",3.2,3.55,Blue,Blue,Blue,Blue,0.5,0.02,0,0,"score");
    createRectangle ("bl2",2.8,3.05,Blue,Blue,Blue,Blue,0.5,0.02,0,0,"score");
    createRectangle ("br2",3.2,3.05,Blue,Blue,Blue,Blue,0.5,0.02,0,0,"score");
    
    createRectangle ("top3",-3.45,3.8,Blue,Blue,Blue,Blue,0.02,0.3,0,0,"score");
    createRectangle ("center3",-3.45,3.40,Blue,Blue,Blue,Blue,0.02,0.3,0,0,"score");
    createRectangle ("bottom3",-3.45,3.00,Blue,Blue,Blue,Blue,0.02,0.3,0,0,"score");
    createRectangle ("ul3",-3.6,3.60,Blue,Blue,Blue,Blue,0.4,0.03,0,0,"score");
    createRectangle ("ur3",-3.3,3.60,Blue,Blue,Blue,Blue,0.4,0.02,0,0,"score");
    createRectangle ("bl3",-3.6,3.20,Blue,Blue,Blue,Blue,0.4,0.02,0,0,"score");
    createRectangle ("br3",-3.3,3.20,Blue,Blue,Blue,Blue,0.4,0.02,0,0,"score");

    createRectangle ("speed1",-3.60,2.8,Yellow,Yellow,Yellow,Yellow,0.10,0.10,0,0,"speed");
    createRectangle ("speed2",-3.45,2.8,Yellow,Yellow,Yellow,Yellow,0.10,0.10,0,0,"speed");
    createRectangle ("speed3",-3.30,2.8,Yellow,Yellow,Yellow,Yellow,0.10,0.10,0,0,"speed");
    
    createRectangle ("laser",t1,t2,Blue,Blue,Blue,Blue,0.10,1.0,0,0,"laser");

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor ((float)128/255, (float)128/255, (float)128/255, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

/*    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;*/
}

string convert(int num)
{
    int len;
    len=0;
    int temp=num;
    while(temp)
    {
        len++;
        temp=temp/10;
    }
    char arr[10];
    int  i=0,rem;
    while(len)
    {
        rem=num%10;
        arr[len-1]=(char)('0'+rem);
        num = num/10;
        i++;
        len=len-1;
    }
    arr[i]='\0';
    return arr;
}
void create_brick(int count)
{
    int i,j;
    color Black = {0,0,0};
    color Red = {1,0,0};
    color Green = {0,1,0};
    for(i=count;i>count-1;i=i-1)
    {
        string temp = convert(i);
        float x,y;
        x = -2.0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.0+2.0)));
        y = 3.1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(4.0-3.3)));
        int temp1;
        temp1 = rand() % 3;
        if(temp1==0)
            createRectangle(temp,x,y,Black,Black,Black,Black,0.20,0.20,0,0,"blackbrick");
        else if(temp1==1)
            createRectangle(temp,x,y,Red,Red,Red,Red,0.20,0.20,0,0,"redbrick");
        else if(temp1==2)
            createRectangle(temp,x,y,Green,Green,Green,Green,0.20,0.20,0,0,"greenbrick");

    }


}

int main (int argc, char** argv)
{
    int width = 600;
    int height = 600;

    GLFWwindow* window = initGLFW(width, height);

    initGL (window, width, height);

    double last_update_time = glfwGetTime();
    double brick_time=last_update_time;
    int count=3;
    int flag=0;
    score=0;

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) 
    {

        // OpenGL Draw commands
        if(penalty>0)
        {
            current_time = glfwGetTime();

            draw(window);
            old_time = current_time;
            current_time = glfwGetTime();
            if(current_time-brick_time>1.5)
            {
                count+=3;
                create_brick(count);
                brick_time=glfwGetTime();
            }

            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            // Poll for Keyboard and mouse events
            glfwPollEvents();

            // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
            current_time = glfwGetTime(); // Time in seconds
            if ((current_time - last_update_time) >= 0.5) 
            { // atleast 0.2s elapsed since last frame
                // do something every 0.2 seconds ..
                last_update_time = current_time;
            }
        }
        else
        {
            cout<<" "<<endl;
            cout<<" "<<endl;
            cout<<"GAME OVER. YOUR SCORE IS: "<<score<<endl;
            cout<<endl;
            break;
        }

    }

    glfwTerminate();
    //    return 0;
    exit(EXIT_SUCCESS);
}
