#define MAIN
#include <math.h>
#include "GL_utilities.h"
#include "MicroGlut.h"
#include "LoadTGA.h"
#include "LittleOBJLoader.h"
#include "VectorUtils4.h"

#define near 1.0
#define far 500.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

const double PI = 3.14159;

vec2 tex_coords[] =
{
  vec2(0.0f,0.0f),
  vec2(0.0f,20.0f),
  vec2(20.0f,0.0f), 
  vec2(20.0f,20.0f)
};
GLuint indices[] =
{
  0, 1, 2, 1, 3, 2
};

#define kRoadWidth 300.0f  // width of the road
#define kRoadLength 30.0f  // length of the road

vec3 road_vertices[] =
{
    vec3(-kRoadWidth / 2, 0.0f, -kRoadLength / 2),
    vec3(-kRoadWidth / 2, 0.0f, kRoadLength / 2),
    vec3(kRoadWidth / 2, 0.0f, -kRoadLength / 2),
    vec3(kRoadWidth / 2, 0.0f, kRoadLength / 2)
};

vec3 road_vertex_normals[] =
{
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f)
};

vec2 road_tex_coords[] =
{
    vec2(0.0f, 0.0f),
    vec2(0.0f, 1.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f)
};

GLfloat projectionMatrix[] = 
{
    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
    0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
    0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
    0.0f, 0.0f, -1.0f, 0.0f 
    };

// Globals
mat4 rot, trans, scale, total;
vec3 cameraTrans = vec3(0.0, 16.0, -50.0);
vec3 LERP(vec3 start, vec3 end, float progress);


GLfloat colorvertices[] =
{
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
};
GLuint program;
GLuint skybox_shader;
    
GLuint myTex1;

Model *trafficlight;
Model *car1;
Model *m;
Model *skybox;
Model *road;
Model *car2;
// vertex array object
unsigned int car1VertexArrayObjID;
unsigned int car1VertexBufferObjID;
unsigned int car1IndexBufferObjID;
unsigned int car1NormalBufferObjID;
unsigned int car1TexCoordBufferObjID;

unsigned int trafficlightVertexArrayObjID;
unsigned int trafficlightVertexBufferObjID;
unsigned int trafficlightIndexBufferObjID;
unsigned int trafficlightNormalBufferObjID;
unsigned int trafficlightTexCoordBufferObjID;

unsigned int skyboxVertexArrayObjID;
unsigned int skyboxVertexBufferObjID;
unsigned int skyboxIndexBufferObjID;
unsigned int skyboxNormalBufferObjID;
unsigned int skyboxTexCoordBufferObjID;

vec3 lightSourcesColorsArr[] = { vec3(1.0f, 0.0f, 0.0f), // Red light

                                 vec3(0.0f, 1.0f, 0.0f), // Green light

                                 vec3(0.0f, 0.0f, 1.0f), // Blue light

                                 vec3(1.0f, 1.0f, 1.0f) }; // White light

GLint isDirectional[] = {0,0,1,1};

vec3 lightSourcesDirectionsPositions[] = { vec3(0.0f, 0.0f, 0.0f), // Red light, positional

                                       vec3(0.0f, 0.0f, 0.0f), // Green light, positional

                                       vec3(-1.0f, 0.0f, 0.0f), // Blue light along X

                                       vec3(0.0f, 0.0f, -1.0f) }; // White light along Z


GLfloat specularExponent[] = {200.0, 100.0, 60.0};


void uploadGeometry(void){
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);

    //glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
	printError("GL inits");

    car1 = LoadModel("car3.obj");
    trafficlight = LoadModel("trafficlight.obj");
    skybox = LoadModel("./skybox-alt/skybox-full-tweaked.obj");
    vec3 colorVect = vec3(1.0,1.0,1.0);
    road = LoadDataToModel(road_vertices, road_vertex_normals, road_tex_coords, &colorVect, indices, 4, 6);
    
	// Load and compile shader
    program = loadShaders("lab3-1.vert", "lab3-1.frag");
    skybox_shader = loadShaders("lab3-1skybox.vert", "lab3-1skybox.frag");
	printError("init shader");
	
    // Texture stuff
    LoadTGATextureSimple("./skybox-alt/atmosphere-cloud.tga", &myTex1);
}

void vertexData(){
    // Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &car1VertexArrayObjID);
	glBindVertexArray(car1VertexArrayObjID);

    glGenVertexArrays(1, &trafficlightVertexArrayObjID);
	glBindVertexArray(trafficlightVertexArrayObjID);
    	
    glGenVertexArrays(1, &skyboxVertexArrayObjID);
	glBindVertexArray(skyboxVertexArrayObjID);
    
    // Allocate Vertex Buffer Objects
    glGenBuffers(1, &skyboxVertexBufferObjID);
	glGenBuffers(1, &skyboxIndexBufferObjID);
	glGenBuffers(1, &skyboxNormalBufferObjID);
	glGenBuffers(1, &skyboxTexCoordBufferObjID);

    glGenBuffers(1, &trafficlightVertexBufferObjID);
	glGenBuffers(1, &trafficlightIndexBufferObjID);
	glGenBuffers(1, &trafficlightNormalBufferObjID);
	glGenBuffers(1, &trafficlightTexCoordBufferObjID);

    glGenBuffers(1, &car1VertexBufferObjID);
	glGenBuffers(1, &car1IndexBufferObjID);
	glGenBuffers(1, &car1NormalBufferObjID);
	glGenBuffers(1, &car1TexCoordBufferObjID);

    // VBO for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, car1VertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, car1->numVertices*3*sizeof(GLfloat), car1->vertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, skybox->numVertices*3*sizeof(GLfloat), skybox->vertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    glBindBuffer(GL_ARRAY_BUFFER, trafficlightVertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, trafficlight->numVertices*3*sizeof(GLfloat), trafficlight->vertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    // VBO for normal data
    glBindBuffer(GL_ARRAY_BUFFER, car1NormalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, car1->numVertices*3*sizeof(GLfloat), car1->normalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, car1IndexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, car1->numIndices*sizeof(GLuint), car1->indexArray, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxNormalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, skybox->numVertices*3*sizeof(GLfloat), skybox->normalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxIndexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, skybox->numIndices*sizeof(GLuint), skybox->indexArray, GL_STATIC_DRAW);

    if (skybox->texCoordArray != NULL)
    {
        glBindBuffer(GL_ARRAY_BUFFER, skyboxTexCoordBufferObjID);
        glBufferData(GL_ARRAY_BUFFER, skybox->numVertices*2*sizeof(GLfloat), skybox->texCoordArray, GL_STATIC_DRAW);
        glVertexAttribPointer(glGetAttribLocation(skybox_shader, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(glGetAttribLocation(skybox_shader, "inTexCoord"));
    }

        if (car1->texCoordArray != NULL)
    {
        glBindBuffer(GL_ARRAY_BUFFER, car1TexCoordBufferObjID);
        glBufferData(GL_ARRAY_BUFFER, car1->numVertices*2*sizeof(GLfloat), car1->texCoordArray, GL_STATIC_DRAW);
        glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
    }

    glBindBuffer(GL_ARRAY_BUFFER, trafficlightNormalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, trafficlight->numVertices*3*sizeof(GLfloat), trafficlight->normalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trafficlightIndexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, trafficlight->numIndices*sizeof(GLuint), trafficlight->indexArray, GL_STATIC_DRAW);

    if (trafficlight->texCoordArray != NULL)
    {
        glBindBuffer(GL_ARRAY_BUFFER, trafficlightTexCoordBufferObjID);
        glBufferData(GL_ARRAY_BUFFER, trafficlight->numVertices*2*sizeof(GLfloat), trafficlight->texCoordArray, GL_STATIC_DRAW);
        glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
    }
}

GLfloat z = -3.14 / 2.0;
vec3 upVector = vec3(0.0, 1.0, 0.0);
vec3 bunnyTrans = vec3(0.0, 4.0, 0.0);
mat4 lv = lookAtv(cameraTrans, bunnyTrans, upVector);
mat4 newlv;
GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
GLfloat x = 0.002*t;

void moveCamera(void){
    GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
    GLfloat x = 0.002*t;
    GLfloat y = -3.14 / 2.0;

    GLfloat rotMatrix[] =
    {
        cos(z), 0.0f, sin(z), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sin(z), 0.0f, cos(z), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glUniformMatrix4fv(glGetUniformLocation(program, "rotMatrix"), 1, GL_TRUE, rotMatrix);
    if (glutKeyIsDown('a')) { 
        lv = T(0.1, 0.0, 0.0) * lv;
     }
     else if (glutKeyIsDown('A')) { 
        lv = Ry(-0.01) * lv;
     }
     
    if (glutKeyIsDown('d')) { 
        lv = T(-0.1, 0.0, 0.0) * lv;
     }
    else if (glutKeyIsDown('D')) { 
        lv = Ry(0.01) * lv;
     }
    if (glutKeyIsDown('w')) { 
        lv = T(0.0, 0.0, 0.1) * lv;
     }
    else if (glutKeyIsDown('W')) { 
        lv = Rx(-0.01) * lv;
     }
     if (glutKeyIsDown('s')) { 
        lv = T(0.0, 0.0, -0.1) * lv;
    }
     
     else if (glutKeyIsDown('S')) { 
        lv = Rx(0.01) * lv;
     }  
    
    rot = Rz(x);
    total = T(0.0,0.0,0.0);

    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

    glUniformMatrix4fv(glGetUniformLocation(program, "rotMatrix"), 1, GL_TRUE, rotMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "realV"), 1, GL_TRUE, lv.m);

    glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "rotMatrix"), 1, GL_TRUE, rotMatrix);
    glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "projMatrix"), 1, GL_TRUE, projectionMatrix);

    glBindVertexArray(skyboxVertexArrayObjID);    // Select VAO

    glDisable(GL_DEPTH_TEST);
}

const int numberOfCars = 10;
Point3D carPositions[numberOfCars];
Point3D carSpeeds[numberOfCars];
GLfloat carRotation[numberOfCars];

vec3 targetLanePositions[numberOfCars];
float laneChangeProgress[numberOfCars];
bool isChangingLane[numberOfCars];

void init(void)
{
    glutRepeatingTimer(16.5);
    dumpInfo();
    uploadGeometry();
    vertexData();

    // Initialize car positions
    // carPositions[0] = SetVector(30.0, 3.7, 7.0);
    carPositions[numberOfCars-1] = SetVector(-50.0, 3.7, 7.0);

    // carSpeeds[0] = SetVector(-0.1, 0, 0);
    carSpeeds[numberOfCars-1] = SetVector(0.1, 0, 0);

    // carRotation[0] = -PI/2;
    carRotation[numberOfCars-1] = PI/2;

    for (int i = 0; i < numberOfCars / 2; i++)
    {
        carPositions[i] = SetVector(60.0 + i * 80.0, 3.7, 15.0);
        carSpeeds[i] = SetVector(-0.1, 0, 0);
        carRotation[i] = -PI / 2;
    }

    for (int i = 0; i < numberOfCars / 2 - 1; i++)
    {
        carPositions[i + numberOfCars / 2] = SetVector(10.0 + i * 80.0, 3.7, 7.0);
        carSpeeds[i + numberOfCars / 2] = SetVector(-0.1, 0, 0);
        carRotation[i + numberOfCars / 2] = -PI / 2;
    }

    // Initialize lane change state
    for (int i = 0; i < numberOfCars; i++) {
        targetLanePositions[i] = carPositions[i];
        laneChangeProgress[i] = 0.0f;
        isChangingLane[i] = false;
    }
}

// void carCollision(void){

//     float xdist_collid = sqrt(pow(carPositions[5].x-carPositions[9].x,2));

//     if (xdist_collid < 20.0){
//         carSpeeds[5] = 0;
// 		carSpeeds[9] = 0;
//     }
	
// 	for (int i =0; i < numberOfCars; i++)
// 	{
// 		for (int j = 1; j < numberOfCars; j++)
// 		{
//             if (i == 5 && j == 9){
//                 continue;
//             }
//             if (i == 9 && j == 5){
//                 continue;
//             }
// 			float xdist = sqrt(pow(carPositions[i].x-carPositions[j].x,2));
//             float zdist = sqrt(pow(carPositions[i].z-carPositions[j].z,2));
// 			float radie = 1;
// 			if (xdist < 23.0 && i !=j && zdist < 0.1)
// 			{
// 				printf("collision\n");
// 				vec3 coll_dirr = normalize(carPositions[i]-carPositions[j]);
// 				carPositions[i].z + 10;
//                 carPositions[j].z + 10;
// 				carSpeeds[i] = 0;
// 				carSpeeds[j] = 0;
// 			}
// 		}	
// 	}
// }

void carCollision(void){

    float xdist_collid = sqrt(pow(carPositions[5].x - carPositions[9].x, 2));

    if (xdist_collid < 20.0) {
        carSpeeds[5] = SetVector(0, 0, 0);
        carSpeeds[9] = SetVector(0, 0, 0);
    }

    for (int i = 0; i < numberOfCars; i++) {
        for (int j = 1; j < numberOfCars; j++) {
            if ((i == 5 && j == 9) || (i == 9 && j == 5)) {
                continue;
            }

            float xdist = sqrt(pow(carPositions[i].x - carPositions[j].x, 2));
            float zdist = sqrt(pow(carPositions[i].z - carPositions[j].z, 2));
            float radie = 1;

            if (xdist < 23.0 && i != j && zdist < 0.1) {
                printf("collision\n");

                // Start lane change if not already changing lanes
                if (!isChangingLane[i] && carSpeeds[i].x != 0) {
                    if (carPositions[i].z == 7.0) {
                        targetLanePositions[i] = SetVector(carPositions[i].x, carPositions[i].y, 15.0); // Move to the right lane
                    } else if (carPositions[i].z == 15.0) {
                        targetLanePositions[i] = SetVector(carPositions[i].x, carPositions[i].y, 7.0); // Move to the left lane
                    }
                    isChangingLane[i] = true;
                    laneChangeProgress[i] = 0.0f;
                }

                // Ensure the car continues moving
                carSpeeds[i] = SetVector(-0.1, 0, 0);
            }
        }
    }

    // Update car positions for lane changes
    for (int i = 0; i < numberOfCars; i++) {
        if (isChangingLane[i]) {
            laneChangeProgress[i] += 0.01f; // Adjust the speed of lane change here
            if (laneChangeProgress[i] >= 1.0f) {
                laneChangeProgress[i] = 1.0f;
                isChangingLane[i] = false;
                carPositions[i] = targetLanePositions[i];
            } else {
                carPositions[i] = LERP(carPositions[i], targetLanePositions[i], laneChangeProgress[i]);
            }
        } else {
            // Continue forward movement
            carPositions[i].x += carSpeeds[i].x;
        }
    }
}

vec3 LERP(vec3 start, vec3 end, float progress) {
    return SetVector(
        start.x + (end.x - start.x) * progress,
        start.y + (end.y - start.y) * progress,
        start.z + (end.z - start.z) * progress
    );
}

void display(void)
{
	printError("pre display");
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    moveCamera();

    mat4 s = mat4(1);
    mat4 s_result = s * S(5, 5, 5);
    glUseProgram(skybox_shader); 
    
    newlv = lv;
    newlv.m[3] = 0;
    newlv.m[7] = 0;
    newlv.m[11] = 0;
    glBindTexture(GL_TEXTURE_2D, myTex1);
    glUniform1i(glGetUniformLocation(skybox_shader, "texunit"), 0);
    glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "realV"), 1, GL_TRUE, newlv.m);
    glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "mdlMatrix"), 1, GL_TRUE, s.m);

    DrawModel(skybox, skybox_shader, "in_Position", NULL, "inTexCoord");

    glEnable(GL_DEPTH_TEST);

    glUseProgram(program);

    trans = T(0, 0, 22.0);
    scale = S(0.05f);
    rot = Rx(-PI/2) * Rz(PI/2);
	total = trans * rot * scale;
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(trafficlight, program, "in_Position", "in_Normal", NULL);

    for (int i =0; i < numberOfCars; i++)
	{
	
		carPositions[i] = carPositions[i] + carSpeeds[i];
        trans = T(carPositions[i].x, carPositions[i].y, carPositions[i].z);
        scale = S(0.5f);
        rot = Ry(carRotation[i]);
	    total = trans * rot * scale;
        glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
        DrawModel(car1, program, "in_Position", "in_Normal", NULL);
	}
    carCollision();

    GLfloat m = (-50.0+x);
    GLfloat n = 30.0-x;


    trans = T(0, 3.7, 10.0);
    scale = S(0.5f);
    rot = Ry(-PI/2);
	total = trans * rot * scale;
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    // DrawModel(car1, program, "in_Position", "in_Normal", NULL);

    n = 70.0-x;
    trans = T(0, 3.7, 10.0);
    scale = S(0.5f);
    rot = Ry(-PI/2);
	total = trans * rot * scale;
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    // DrawModel(car1, program, "in_Position", "in_Normal", NULL);   

    trans = T(0.0, 0.0, 15.0);
    rot = Ry(-PI);
    total = trans * rot;
    glBindTexture(GL_TEXTURE_2D, myTex1);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    glUniform1f(glGetUniformLocation(program, "specularExponent"), specularExponent[0]);
    DrawModel(road, program, "in_Position", "in_Normal", NULL);

	printError("display");

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("TSBK11 Project");
	glutDisplayFunc(display); 
    glutRepeatingTimer(20);
	init ();
	glutMainLoop();
	return 0;
}
