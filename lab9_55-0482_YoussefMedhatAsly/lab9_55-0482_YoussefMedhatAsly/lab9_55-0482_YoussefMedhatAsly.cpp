#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <cmath>
#include <stdio.h> // Added for printf
#include <stdlib.h> // For rand()
#include <string>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


// Global variables for player position and camera view
float playerX = 0.0, playerY = -1.6, playerZ = 0.0;
float cameraAngle = 0.0;
int currentView = 0;  // 0 = Top, 1 = Side, 2 = Front

// Add these variables at the top
float cameraPosX = 1.0, cameraPosY = 1.0, cameraPosZ = 2.0;  // Camera position

// Define the bounding box for the diving platform (upper and lower parts)
float upperPlatformX = 1.3, upperPlatformY = 1.5, upperPlatformZ = -2.4;
float upperPlatformWidth = 0.35, upperPlatformHeight = 0.05, upperPlatformDepth = 2.75;
float lowerPlatformX = 1.3, lowerPlatformY = 1.0, lowerPlatformZ = -2.4;
float lowerPlatformWidth = 0.35, lowerPlatformHeight = 0.05, lowerPlatformDepth = 1.25;

bool isJumping = false;
float verticalVelocity = 0.0f;  // Initial jump velocity
const float GRAVITY = -0.01f;   // Gravity effect; adjust for desired fall speed
const float JUMP_STRENGTH = 0.2f; // Jump initial velocity
const float GROUND_LEVEL = -1.6f; // Ground Y-position
float horizontalVelocity = 0.0f;     // Horizontal velocity for forward movement
const float JUMP_FORWARD_STRENGTH = 0.05f; // Forward velocity during jump (negative for forward in Z)

// Define wall boundaries (adjust values as needed)
const float LEFT_WALL_X = -2.0f;   // Left wall boundary in X-axis
const float RIGHT_WALL_X = 2.0f;   // Right wall boundary in X-axis
const float BACK_WALL_Z = -2.4f;   // Back wall boundary in Z-axis

// Initial wall color
float wallColorR = 0.8f;
float wallColorG = 0.8f;
float wallColorB = 0.8f;

//life jacket animation pre-defined variables
float lifeJacketBrightness = 0.5f; // Start with medium brightness
bool increasingBrightness = true;  // To alternate between increasing and decreasing

// Define variables for the scoreboard color and animation flag
bool isScoreboardAnimating = false;
float scoreboardColorR = 0.8f, scoreboardColorG = 0.8f, scoreboardColorB = 0.8f;

// Define variables for the pool color and animation flag
bool poolColorChanging = false;
float poolColorR = 0.95f, poolColorG = 0.95f, poolColorB = 0.9f; // Initial pool color

// Global variable to track life ring y-position
float lifeRingYPosition = 0.4f;
bool moveUp = true;              // Flag to control the movement direction
bool isMoving = false;           // Flag to control if it is moving

// Define variables for the diving platform animation
float platformScaleZ = 3.5f;  // Scaling factor for the platform along the X-axis
bool isScalingActive = false;     // Flag to track if continuous scaling is active
bool scalingUp = true;            // Direction flag: true for growing, false for shrinking

//Define variables for the olympic ring animation
float rotationAngle = 0.0f;
bool isRotationActive = false;

//player rotation variables
float playerRotationAngle = 0.0f;

//variables for collision with life jacket
bool lifeJacketVisible = true;

//variables for end game win or lost
int gameTime = 15; // Game lasts 60 seconds
int timeRemaining = gameTime * 60; // Convert seconds to frames
bool gameRunning = true;
std::string gameOverText = "";


// Function declarations
void drawWall(double posX, double posY, double posZ,double scaleX, double scaleY, double scaleZ,double colorR, double colorG, double colorB,double normalX, double normalY, double normalZ) ;
void drawPlayer();
void setupLights();
void setupCamera();
void handleSpecialKeys(int key, int x, int y);
void handleKeys(unsigned char key, int x, int y);
bool checkCollisionWithPlatform(float platformX, float platformY, float platformZ, float width, float height, float depth);
bool checkCollisionWithGoal();
void drawGround();
void drawDivingPlatform();
void Display();
void updatePlayerPosition();
void drawPool(double posX, double posY, double posZ,double poolWidth, double poolDepth, double poolHeight,double tileThickness, double tileColorR, double tileColorG, double tileColorB) ;
void drawOlympicRing(float x, float y, float z, float radius, float thickness, float r, float g, float b);
void drawOlympicRings();
void drawDoorOnBackWall();
void collisionupdatePlayerPosition();
void drawLifeRingOnBackWall();
void drawLadder();
void drawScoreboard();
void drawLifeJacket();
void updateWallColors(int value);
void updateLifeJacketBrightness(int value);
void animateScoreboardColor(int value);
void animatePoolColor();
void translatelifeRing(int value);
void scaleDivingPlatform(int value);
bool checkCollisionLifeJacket(float playerX, float playerY, float playerZ,float lifeJacketX, float lifeJacketY, float lifeJacketZ, float collisionRadius);
void updateScene();
void updateTimerEnd(int value);
void endGame();

void updateScene() {
    // Example player and life jacket positions (replace with actual variables)
    float lifeJacketX = -0.5f;
    float lifeJacketY = -1.3f;
    float lifeJacketZ = -2.3f;
    float collisionRadius = 0.4f; // Adjust based on the size of the player and life jacket

    // Check for collision if the life jacket is still visible
    if (lifeJacketVisible &&
        checkCollisionLifeJacket(playerX, playerY, playerZ, lifeJacketX, lifeJacketY, lifeJacketZ, collisionRadius)) {
        lifeJacketVisible = false; // Remove the life jacket from the scene
        printf("Collision with life jacket detected!\n");
    }
}

bool checkCollisionLifeJacket(float playerX, float playerY, float playerZ,
                    float lifeJacketX, float lifeJacketY, float lifeJacketZ, float collisionRadius) {
    float distance = sqrt(pow(playerX - lifeJacketX, 2) +
                          pow(playerY - lifeJacketY, 2) +
                          pow(playerZ - lifeJacketZ, 2));
    return distance <= collisionRadius;
}

void updateLifeJacketBrightness(int value) {
    // Adjust brightness direction
    if (increasingBrightness) {
        lifeJacketBrightness += 0.1f;
        if (lifeJacketBrightness >= 1.0f) { // Reached maximum brightness
            lifeJacketBrightness = 1.0f;
            increasingBrightness = false;
        }
    } else {
        lifeJacketBrightness -= 0.1f;
        if (lifeJacketBrightness <= 0.3f) { // Reached minimum brightness
            lifeJacketBrightness = 0.3f;
            increasingBrightness = true;
        }
    }

    glutPostRedisplay(); // Redraw the scene with the updated brightness

    // Set the timer to call this function again after 500 ms
    glutTimerFunc(500, updateLifeJacketBrightness, 0);
}


void drawLifeJacket() {
    // Render only if the life jacket is visible
       if (!lifeJacketVisible) {
           return;
       }
    
    glPushMatrix();
    glTranslatef(-0.5f, -1.3f, -2.3f); // Position the life jacket near the back wall

    // Left panel of the life jacket (Orange)
    glColor3f(lifeJacketBrightness, lifeJacketBrightness * 0.8f, 0.0f); // Yellowish color
    glPushMatrix();
    glTranslatef(-0.15f, 0.0f, 0.0f);
    glScalef(0.1f, 0.3f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Right panel of the life jacket (Orange)
    glColor3f(lifeJacketBrightness, lifeJacketBrightness * 0.8f, 0.0f); // Yellowish color
    glPushMatrix();
    glTranslatef(0.15f, 0.0f, 0.0f);
    glScalef(0.1f, 0.3f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Top connector between the panels (Orange)
    glColor3f(lifeJacketBrightness, lifeJacketBrightness * 0.8f, 0.0f); // Yellowish color
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.0f);
    glScalef(0.3f, 0.1f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Horizontal black belt (optional detail, not a primitive)
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for belt
    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.0f);
    glScalef(0.3f, 0.05f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}



void collisionupdatePlayerPosition() {
    // Limit player position on X-axis based on wall boundaries
    if (playerX < LEFT_WALL_X) {
        playerX = LEFT_WALL_X;
    }
    if (playerX > RIGHT_WALL_X) {
        playerX = RIGHT_WALL_X;
    }

    // Limit player position on Z-axis based on back wall boundary
    if (playerZ < BACK_WALL_Z) {
        playerZ = BACK_WALL_Z;
    }

    // Implement gravity or other movement calculations here
    if (isJumping) {
        verticalVelocity += GRAVITY;          // Apply gravity
        playerY += verticalVelocity;          // Update Y position

        if (playerY <= GROUND_LEVEL) {        // Check for landing
            playerY = GROUND_LEVEL;
            verticalVelocity = 0.0f;
            isJumping = false;
        }
    }
}

// Function to draw a door on the back wall using three primitives
void drawDoorOnBackWall() {
    // Door dimensions and position
    float doorWidth = 0.6;
    float doorHeight = 1.5;
    float doorThickness = 0.05;
    float doorPosX = -2.0;          // Centered horizontally on the wall
    float doorPosY = -1.25;         // Position from the bottom of the wall
    float doorPosZ = -2.35;         // Depth position on the back wall

    // Draw the door frame (rectangle for the door base)
    glPushMatrix();
    glColor3f(0.4, 0.2, 0.1);       // Brown color for the door
    glTranslatef(doorPosX, doorPosY, doorPosZ);
    glScalef(doorWidth, doorHeight, doorThickness);
    glutSolidCube(1.0);             // Main door rectangle
    glPopMatrix();

    // Draw the left doorpost (cuboid on the left side)
    glPushMatrix();
    glColor3f(0.3, 0.15, 0.05);     // Darker color for the posts
    glTranslatef(doorPosX - doorWidth / 2 + 0.05, doorPosY, doorPosZ + doorThickness / 2);
    glScalef(0.05, doorHeight, 0.05); // Adjust width, height, and depth for a thin vertical cuboid
    glutSolidCube(1.0);             // Left vertical doorpost
    glPopMatrix();

    // Draw the right doorpost (cuboid on the right side)
    glPushMatrix();
    glColor3f(0.3, 0.15, 0.05);
    glTranslatef(doorPosX + doorWidth / 2 - 0.05, doorPosY, doorPosZ + doorThickness / 2);
    glScalef(0.05, doorHeight, 0.05); // Adjust width, height, and depth for a thin vertical cuboid
    glutSolidCube(1.0);             // Right vertical doorpost
    glPopMatrix();

    // Draw the top doorpost (horizontal cuboid across the top)
    glPushMatrix();
    glColor3f(0.3, 0.15, 0.05);
    glTranslatef(doorPosX, doorPosY + doorHeight / 2 - 0.05, doorPosZ + doorThickness / 2);
    glScalef(doorWidth, 0.05, 0.05); // Adjust width, height, and depth for a horizontal cuboid
    glutSolidCube(1.0);             // Top horizontal doorpost
    glPopMatrix();

    // Draw a small rectangle as the door handle or panel
    glPushMatrix();
    glColor3f(0.8, 0.7, 0.6);       // Light color for the handle
    glTranslatef(doorPosX + doorWidth / 4, doorPosY - 0.3, doorPosZ + doorThickness + 0.02);
    glScalef(0.1, 0.2, 0.02);       // Adjust size for the handle
    glutSolidCube(1.0);             // Small rectangle for handle
    glPopMatrix();
}

void updateWallColors(int value) {
    // Generate random RGB values between 0 and 1
    wallColorR = static_cast<float>(rand()) / RAND_MAX;
    wallColorG = static_cast<float>(rand()) / RAND_MAX;
    wallColorB = static_cast<float>(rand()) / RAND_MAX;

    glutPostRedisplay(); // Redraw the scene with the new colors

    // Set up the timer to call this function again after an interval (e.g., 1000 ms)
    glutTimerFunc(2000, updateWallColors, 0);
}

// Function to draw a wall with customizable position, scale, color, and normal direction
void drawWall(double posX, double posY, double posZ,
              double scaleX, double scaleY, double scaleZ,
              double colorR, double colorG, double colorB,
              double normalX, double normalY, double normalZ) {
    // First primitive: Main wall block
    glPushMatrix();
    glColor3f(wallColorR, wallColorG, wallColorB);          // Set wall color
    glNormal3f(normalX, normalY, normalZ);      // Set wall normal direction
    glTranslated(posX, posY, posZ);             // Position the main wall block
    glScaled(scaleX, scaleY, scaleZ);           // Scale the main wall block
    glutSolidCube(1.0);                         // Draw wall as a cube
    glPopMatrix();

    // Second primitive: Thicker, black cap or border on top of the wall
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);                   // Set cap color to black
    glNormal3f(normalX, normalY, normalZ);      // Set wall normal direction
    glTranslated(posX, posY + (scaleY * 0.5) + 0.15, posZ); // Position above main wall
    glScaled(scaleX * 1.1, 0.3, scaleZ * 1.1); // Thicker cap with increased height
    glutSolidCube(1.0);                         // Draw cap as another cube
    glPopMatrix();
}

// Function to draw a ring (as a torus) at a given position with specified color
void drawOlympicRing(float x, float y, float z, float radius, float thickness, float r, float g, float b) {
    glPushMatrix();
    glColor3f(r, g, b);            // Set ring color
    glTranslatef(x, y, z+3);          // Position the ring
    glRotatef(90, 1.0, 0.0, 0.0);   // Rotate to face forward
    glutSolidTorus(thickness, radius, 30, 30); // Draw ring as a torus
    glPopMatrix();
}

// Function to draw the Olympic rings on the left wall
void drawOlympicRings() {
    float ringRadius = 0.2;         // Outer radius of each ring
    float ringThickness = 0.02;     // Thickness of the ring

    // Base position for the left wall
    float baseX = -1.4;
    float baseY = -2.0;
    float baseZ = -2.0;

    glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);        // Move to center point of rotation
        glRotatef(rotationAngle, 0.0, 1.0, 0.0); // Rotate around y-axis for wheel-like spin
        glTranslatef(0.0, 0.0, 0.0);       // Translate back to original position

      

    // Draw each Olympic ring with appropriate offsets and colors
    drawOlympicRing(baseX, baseY, baseZ, ringRadius, ringThickness, 0.0, 0.5, 1.0);     // Blue ring
    drawOlympicRing(baseX + 0.25, baseY, baseZ + 0.15, ringRadius, ringThickness, 1.0, 1.0, 0.0); // Yellow ring
    drawOlympicRing(baseX + 0.5, baseY, baseZ, ringRadius, ringThickness, 0.0, 0.0, 0.0);     // Black ring
    drawOlympicRing(baseX + 0.75, baseY, baseZ + 0.15, ringRadius, ringThickness, 0.0, 1.0, 0.0); // Green ring
    drawOlympicRing(baseX + 1.0, baseY, baseZ, ringRadius, ringThickness, 1.0, 0.0, 0.0);     // Red ring

    glPopMatrix();
}



void animatePoolColor() {
    if (poolColorChanging) {
        poolColorR = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  // Random red
        poolColorG = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  // Random green
        poolColorB = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  // Random blue
        glutPostRedisplay();  // Request a redraw to apply new color
        glutTimerFunc(1000, [](int){ animatePoolColor(); }, 0);  // Continue animation every second
    }
}

void drawPool(double posX, double posY, double posZ,
              double poolWidth, double poolDepth, double poolHeight,
              double tileThickness, double tileColorR, double tileColorG, double tileColorB) {

    // Adjust the vertical position to make sure the pool base is a little bit down
    double poolBaseHeight = posY - poolHeight * 0.5 - 0.5;  // Slightly more negative to lower the pool

    // Draw pool water (taller blue rectangle)
    glPushMatrix();
    glColor3f(0.0, 0.5, 1.0);                   // Blue color for water
    glTranslated(posX - poolWidth * 0.5, poolBaseHeight, posZ); // Position pool at ground level and shift left
    glScaled(poolWidth, poolHeight, poolDepth);  // Scale to desired pool dimensions
    glutSolidCube(1.0);                          // Draw pool as a cube
    glPopMatrix();

    // Draw surrounding tiles (off-white border around the pool)
    double tileBorderOffset = tileThickness * 0.5; // Make sure tiles align with the pool edges

    // Left tile border (stuck to the left side of the pool)
    glPushMatrix();
    glColor3f(poolColorR, poolColorG, poolColorB);
    glTranslated(posX - poolWidth * 0.5 - tileBorderOffset-1, poolBaseHeight, posZ);
    glScaled(tileThickness, poolHeight, poolDepth + tileThickness);
    glutSolidCube(1.0);
    glPopMatrix();

    // Right tile border (stuck to the right side of the pool)
    glPushMatrix();
    glColor3f(poolColorR, poolColorG, poolColorB);
    glTranslated(posX + poolWidth * 0.44 + tileBorderOffset-1, poolBaseHeight+0.01, posZ);
    glScaled(tileThickness, poolHeight, poolDepth + tileThickness);
    glutSolidCube(1.0);
    glPopMatrix();

    // Front tile border (stuck to the front side of the pool)
    glPushMatrix();
    glColor3f(poolColorR, poolColorG, poolColorB);
    glTranslated(posX-1, poolBaseHeight, posZ + poolDepth * 0.5 + tileBorderOffset);
    glScaled(poolWidth + tileThickness, poolHeight, tileThickness);
    glutSolidCube(1.0);
    glPopMatrix();

    // Back tile border (stuck to the back side of the pool)
    glPushMatrix();
    glColor3f(poolColorR, poolColorG, poolColorB);
    glTranslated(posX-1, poolBaseHeight, posZ - poolDepth * 0.5 - tileBorderOffset);
    glScaled(poolWidth + tileThickness, poolHeight, tileThickness);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawPlayer() {
    glPushMatrix();
        glTranslatef(playerX, playerY, playerZ); // Move player to position
        glRotatef(playerRotationAngle, 0.0f, 1.0f, 0.0f); // Rotate around the y-axis
        
    // Set color for skin tone (beige)
    glColor3f(1.0, 0.8, 0.6);

    // Draw head (cube shape)
    glPushMatrix();
    glTranslated(0.0, 0.35, 0.0); // Position the head above the body
    glScaled(0.15, 0.15, 0.15); // Scale to make the head cube-shaped
    glutSolidCube(1.0);  // Head as a cube
    glPopMatrix();

    // Draw body (tall rectangular shape)
    glPushMatrix();
    glTranslated(0.0, 0.1, 0.0); // Position the body below the head
    glScaled(0.2, 0.3, 0.1);  // Scale to make the body taller and rectangular
    glutSolidCube(1.0);  // Body as a rectangular block
    glPopMatrix();

    // Draw arms (slender rectangular shapes)
    glPushMatrix();
    glTranslated(-0.175, 0.1, 0.0); // Position left arm next to body
    glScaled(0.1, 0.25, 0.1);  // Scale to make arm slender
    glutSolidCube(1.0);  // Left arm
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.175, 0.1, 0.0); // Position right arm next to body
    glScaled(0.1, 0.25, 0.1);  // Scale to make arm slender
    glutSolidCube(1.0);  // Right arm
    glPopMatrix();

    // Draw legs with two-tone effect for swim shorts
    // Left leg
    glPushMatrix();
    // Upper half of the left leg (blue for swim shorts)
    glPushMatrix();
    glColor3f(0.2, 0.3, 1.0);  // Blue color for shorts
    glTranslated(-0.075, -0.15, 0.0); // Position upper part of left leg
    glScaled(0.1, 0.15, 0.1);  // Scale to half height for shorts
    glutSolidCube(1.0);
    glPopMatrix();

    // Lower half of the left leg (beige for skin tone)
    glPushMatrix();
    glColor3f(1.0, 0.8, 0.6);  // Reset color to skin tone
    glTranslated(-0.075, -0.35, 0.0); // Position lower part of left leg
    glScaled(0.1, 0.15, 0.1);  // Scale to half height for lower leg
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();

    // Right leg
    glPushMatrix();
    // Upper half of the right leg (blue for swim shorts)
    glPushMatrix();
    glColor3f(0.2, 0.3, 1.0);  // Blue color for shorts
    glTranslated(0.075, -0.15, 0.0); // Position upper part of right leg
    glScaled(0.1, 0.15, 0.1);  // Scale to half height for shorts
    glutSolidCube(1.0);
    glPopMatrix();

    // Lower half of the right leg (beige for skin tone)
    glPushMatrix();
    glColor3f(1.0, 0.8, 0.6);  // Reset color to skin tone
    glTranslated(0.075, -0.35, 0.0); // Position lower part of right leg
    glScaled(0.1, 0.15, 0.1);  // Scale to half height for lower leg
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}

void translatelifeRing(int value) {
    if (isMoving) {
        // Update position based on the moveUp flag
        if (moveUp) {
            lifeRingYPosition += 0.1f;  // Move up
            if (lifeRingYPosition > 0.7f) {  // Maximum height
                moveUp = false;  // Change direction to move down
            }
        } else {
            lifeRingYPosition -= 0.1f;  // Move down
            if (lifeRingYPosition < -0.6f) {  // Minimum height
                moveUp = true;  // Change direction to move up
            }
        }
    }

    glutPostRedisplay();  // Request redraw to update the scene
    glutTimerFunc(16, translatelifeRing, 0);  // Set up the next update (about 60 FPS)
}

void drawLifeRingOnBackWall() {
    glPushMatrix();

        // Position the life ring on the back wall
        glTranslatef(1.5f, lifeRingYPosition, -2.4f); // Adjust height and depth for placement on the back wall

        // Draw the outer ring (orange)
        glColor3f(1.0f, 0.5f, 0.0f); // Orange color
        glutSolidTorus(0.05, 0.25, 30, 30); // Outer torus with inner radius 0.05, outer radius 0.25

        // Draw the inner ring (off-white)
        glColor3f(0.9f, 0.9f, 0.9f); // Off-white color
        glutSolidTorus(0.05, 0.15, 30, 30); // Inner torus with inner radius 0.05, outer radius 0.15

        // Draw the black cube in the center
        glColor3f(0.0f, 0.0f, 0.0f); // Black color
        glPushMatrix();
        glScalef(0.1f, 0.1f, 0.1f); // Scale down the cube for a central position
        glutSolidCube(1.0f); // Draw cube with size 1.0 (scaled)
        glPopMatrix();

        glPopMatrix();
}

// Timer function to change scoreboard color
void animateScoreboardColor(int value) {
    if (isScoreboardAnimating) {
        // Generate random colors
        scoreboardColorR = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        scoreboardColorG = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        scoreboardColorB = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        // Redraw and set timer to call this function again
        glutPostRedisplay();
        glutTimerFunc(1000, animateScoreboardColor, 0);  // Adjust timing as needed
    }
}

void drawScoreboard() {
    glPushMatrix();
    glTranslatef(-1.0f, 1.5f, -2.2f); // Position the entire scoreboard; adjust as needed

    // Outer scoreboard rectangle (light grey background)
    glColor3f(scoreboardColorR, scoreboardColorG, scoreboardColorB);  // Use current scoreboard color
    glBegin(GL_QUADS);
        glVertex3f(-0.9f, 0.5f, 0.0f);  // Top left
        glVertex3f(0.9f, 0.5f, 0.0f);   // Top right
        glVertex3f(0.9f, -0.5f, 0.0f);  // Bottom right
        glVertex3f(-0.9f, -0.5f, 0.0f); // Bottom left
    glEnd();

    // Red rectangle beneath the clock and score section
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    glBegin(GL_QUADS);
        glVertex3f(-0.9f, -0.1f, 0.01f); // Top left
        glVertex3f(0.9f, -0.1f, 0.01f);  // Top right
        glVertex3f(0.9f, -0.5f, 0.01f);  // Bottom right
        glVertex3f(-0.9f, -0.5f, 0.01f); // Bottom left
    glEnd();

    // Draw clock face as a brown disk on the left side
    glPushMatrix();
    glTranslatef(-0.45f, 0.2f, 0.2f); // Position the clock above the red rectangle

    // Clock face (brown circle)
    glColor3f(0.65f, 0.16f, 0.16f); // Brown color for clock face
    float radius = 0.15f;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f); // Center of the clock
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(cos(angle) * radius, sin(angle) * radius);
        }
    glEnd();

    // Clock arrows
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for arrows

    // Minute hand (longer)
    glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, radius * 0.8f); // Vertical line for minute hand
    glEnd();

    // Hour hand (shorter)
    glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(radius * 0.5f, 0.0f); // Horizontal line for hour hand
    glEnd();

    glPopMatrix(); // End clock drawing




    // Score rectangle on the right side (off-white)
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f); // black color for score section
    glTranslatef(0.4f, 0.1f, 0.20f);  // Position to the right of the clock
    glBegin(GL_QUADS);
    glVertex3f(-0.3f, 0.15f, 0.0f); // Top left (increased size)
       glVertex3f(0.3f, 0.15f, 0.0f);  // Top right (increased size)
       glVertex3f(0.3f, -0.15f, 0.0f); // Bottom right (increased size)
       glVertex3f(-0.3f, -0.15f, 0.0f);// Bottom left (increased size)
   glEnd();
    glPopMatrix();

    // 3 squares below the red rectangle
    glTranslatef(0.0f, -0.105f, 0.20f); // Move down under the red rectangle for square alignment

    // Left square (green)
    glColor3f(0.0f, 1.0f, 0.0f); // Bright green color
    glBegin(GL_QUADS);
        glVertex3f(-0.2f, -0.1f, 0.01f); // Top left
        glVertex3f(0.0f, -0.1f, 0.01f);  // Top right
        glVertex3f(0.0f, -0.3f, 0.01f);  // Bottom right
        glVertex3f(-0.2f, -0.3f, 0.01f); // Bottom left
    glEnd();

    // Middle square (green)
    glColor3f(0.0f, 1.0f, 0.0f); // Bright green color
    glTranslatef(0.3f, 0.0f, 0.0f); // Shift to the right
    glBegin(GL_QUADS);
        glVertex3f(-0.2f, -0.1f, 0.01f);
        glVertex3f(0.0f, -0.1f, 0.01f);
        glVertex3f(0.0f, -0.3f, 0.01f);
        glVertex3f(-0.2f, -0.3f, 0.01f);
    glEnd();

    // Right square (black)
    glColor3f(0.0f, 0.0f, 0.0f); // Black color
    glTranslatef(0.3f, 0.0f, 0.0f); // Shift to the right again
    glBegin(GL_QUADS);
        glVertex3f(-0.2f, -0.1f, 0.01f);
        glVertex3f(0.0f, -0.1f, 0.01f);
        glVertex3f(0.0f, -0.3f, 0.01f);
        glVertex3f(-0.2f, -0.3f, 0.01f);
    glEnd();

    glPopMatrix(); // Restore transformations
}


void drawLadder() {
    glPushMatrix();

    // Position ladder on the back wall
    glTranslatef(0.5f, -1.8f, -2.4f);  // Adjust to position on the back wall

    // Set ladder color to black
    glColor3f(0.0f, 0.0f, 0.0f);  // Black color for the ladder

    // Draw the left side bar (vertical line)
    glPushMatrix();
    glTranslatef(-0.2f, 0.0f, 0.0f);  // Move to the left side
    glBegin(GL_QUADS);  // Draw a rectangle (cube)
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.05f, 0.0f, 0.0f);
    glVertex3f(0.05f, 4.5f, 0.0f);  // Increased height to 4.5f
    glVertex3f(0.0f, 4.5f, 0.0f);
    glEnd();
    glPopMatrix();

    // Draw the right side bar (vertical line)
    glPushMatrix();
    glTranslatef(0.2f, 0.0f, 0.0f);  // Move to the right side
    glBegin(GL_QUADS);  // Draw a rectangle (cube)
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.05f, 0.0f, 0.0f);
    glVertex3f(0.05f, 4.5f, 0.0f);  // Increased height to 4.5f
    glVertex3f(0.0f, 4.5f, 0.0f);
    glEnd();
    glPopMatrix();

    // Draw rungs (horizontal)
    float rungHeight = 0.4f;  // Increased the distance between rungs
    for (int i = 0; i < 12; ++i) {  // 12 steps for more rungs
        glPushMatrix();
        glTranslatef(0.0f, rungHeight * i, 0.0f);  // Position each rung
        glBegin(GL_QUADS);  // Create a rectangle (cube) for each rung
        glVertex3f(-0.205f, 0.0f, 0.0f);  // Left side of the rung
        glVertex3f(0.205f, 0.0f, 0.0f);   // Right side of the rung
        glVertex3f(0.205f, 0.05f, 0.0f);  // Right side of the rung (height adjusted here)
        glVertex3f(-0.205f, 0.05f, 0.0f); // Left side of the rung (height adjusted here)
        glEnd();

        glEnd();
        glPopMatrix();
    }

    glPopMatrix();
}


// Function to setup lights in the scene
void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat shininess[] = { 50 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
    GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

// Function to setup camera view
void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 10.0);  // Set perspective for depth
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Camera position
}


// Function to handle special key presses (like arrow keys)
void handleSpecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:  // Left Arrow key
            cameraPosX -= 0.1;
            break;
        case GLUT_KEY_UP:    // Up Arrow key
            cameraPosY += 0.1;
            break;
        case GLUT_KEY_RIGHT: // Right Arrow key
            cameraPosX += 0.1;
            break;
        case GLUT_KEY_DOWN:  // Down Arrow key
            cameraPosY -= 0.1;
            break;
    }
    glutPostRedisplay();  // Redraw the scene
}


// Function to handle key presses for controlling the player
void handleKeys(unsigned char key, int x, int y) {
    // Store original position
    float originalX = playerX, originalY = playerY, originalZ = playerZ;

    switch (key) {
        case 'w': // Move player forward
            playerZ += 0.1;
            playerRotationAngle = 180.0f;
            break;
        case 's': // Move player backward
            playerZ -= 0.1;
            playerRotationAngle = 0.0f;
            break;
        case 'a': // Move player left
            playerX -= 0.1;
            playerRotationAngle = 270.0f;
            break;
        case 'd': // Move player right
            playerX += 0.1;
            playerRotationAngle = 90.0f;
            break;
        case 'q': // Move player up
            playerY += 0.1;
            break;
        case 'r': // Reset player position
            playerX = 0.0;
            playerY = -1.6;
            playerZ = 0.0;
            break;
        case '1': // Switch to Top View
            cameraPosX=0.0;
            cameraPosY=4.5;
            cameraPosZ=2;
            break;
        case '2': // Switch to Side View
            cameraPosX=-1.2;
            cameraPosY=2.7;
            cameraPosZ=2;
            break;
        case '3': // Switch to Front View
            cameraPosX=0.0;
            cameraPosY=0.0;
            cameraPosZ=2;
            break;
            
        case 'c':  // 'c' key starts animation
            //score board
            isScoreboardAnimating = true;
            animateScoreboardColor(0);  // Start the color change
            
            //pool
            poolColorChanging = true;
            animatePoolColor();
            
            // Start moving the life ring
            isMoving = true;
            
            // Scale the platform longer
            isScalingActive = true;
            
            // Start rotation
            isRotationActive = true;
             break;
        case 'x':  // 'x' key for stop animation
            //score board
            isScoreboardAnimating = false;
            
            //pool
            poolColorChanging = false;
            
            // Stop moving the life ring
            isMoving = false;
            
            // Scale the platform shorter
            isScalingActive = false;
            platformScaleZ = 3.5f;
            
            // Stop rotation
            isRotationActive = false;
             break;
        case ' ': // Space bar to jump
            if (!isJumping) {  // Only allow jumping if not already in the air
                isJumping = true;
                verticalVelocity = JUMP_STRENGTH;
                horizontalVelocity = JUMP_FORWARD_STRENGTH; // Set forward movement in Z
            }
            break;
//        case 'z': // 'z' key to print camera position
//            printf("Camera Position: X = %.2f, Y = %.2f, Z = %.2f\n", cameraPosX, cameraPosY, cameraPosZ);
//            break;
//        case 'f': // Space bar to jump
//            playerRotationAngle += 10.0f;
//                    if (playerRotationAngle >= 360.0f) {
//                        playerRotationAngle -= 360.0f; // Wrap-around to keep angle in 0-359 range
//                    }
//            break;
           
    }

    collisionupdatePlayerPosition(); // Apply boundary checks after movement

    // Check for collisions with both parts of the platform
    if (checkCollisionWithPlatform(upperPlatformX, upperPlatformY, upperPlatformZ, upperPlatformWidth, upperPlatformHeight, upperPlatformDepth) ||
        checkCollisionWithPlatform(lowerPlatformX, lowerPlatformY, lowerPlatformZ, lowerPlatformWidth, lowerPlatformHeight, lowerPlatformDepth)) {
        // Reset to original position if a collision is detected
        playerX = originalX;
        playerY = originalY;
        playerZ = originalZ;
    }

    glutPostRedisplay();  // Redraw the scene
}


void updatePlayerPosition() {
    if (isJumping) {
        // Apply vertical movement
        playerY += verticalVelocity;
        verticalVelocity += GRAVITY;

        // Apply forward movement
        playerZ += horizontalVelocity;  // Update Z position for forward movement

        // Check for landing on the ground
        if (playerY <= GROUND_LEVEL) {
            playerY = GROUND_LEVEL;
            isJumping = false;
            verticalVelocity = 0.0f;
            horizontalVelocity = 0.0f; // Stop forward movement when landing
        }
    }
}






bool checkCollisionWithPlatform(float platformX, float platformY, float platformZ, float width, float height, float depth) {
    return (playerX > platformX - width && playerX < platformX + width &&
            playerY > platformY - height && playerY < platformY + height &&
            playerZ > platformZ - depth && playerZ < platformZ + depth);
}

// Function to check collision with goal
bool checkCollisionWithGoal() {
    float goalX = 0.3, goalY = 0.0, goalZ = 0.0; // Goal position
    float goalSize = 0.15;

    // Simple collision detection with goal (bounding box check)
    if (playerX > goalX - goalSize && playerX < goalX + goalSize &&
        playerY > goalY - goalSize && playerY < goalY + goalSize &&
        playerZ > goalZ - goalSize && playerZ < goalZ + goalSize) {
        return true; // Collision detected
    }
    return false;
}

void drawGround() {
    glPushMatrix();
    glColor3f(0.5, 0.35, 0.05);  // Brown color for the ground
    glTranslated(0.0, -2.0, 0.0); // Position ground at the base
    glScaled(5.0, 0.02, 5.0);     // Scale to cover area within walls
    glutSolidCube(1.0);
    glPopMatrix();
}

void scaleDivingPlatform(int value) {
    if (isScalingActive) {
            // Toggle between growing and shrinking based on scalingUp flag
            if (scalingUp) {
                platformScaleZ += 0.05f;  // Grow the platform
                if (platformScaleZ >= 6.0f) {
                    scalingUp = false;    // Switch to shrinking
                }
            } else {
                platformScaleZ -= 0.05f;  // Shrink the platform
                if (platformScaleZ <= 1.0f) {
                    scalingUp = true;     // Switch to growing
                }
            }
        }
    

    glutPostRedisplay();  // Request redraw to update the scene
    glutTimerFunc(16, scaleDivingPlatform, 0);  // Set up the next update (about 60 FPS)
}

void drawDivingPlatform() {
    // Set color to baby blue
    glColor3f(0.68f, 0.85f, 0.9f);

    // Draw the larger upper platform
    glPushMatrix();
    glTranslated(1.3, 1.5, -2.4);  // Position near the back wall and slightly to the right
    glScaled(0.7, 0.1, platformScaleZ);       // Make the platform larger and thin
    glutSolidCube(1.0);             // Draw the cube
    glPopMatrix();

    // Draw the smaller lower platform
    glPushMatrix();
    glTranslated(1.3, 1.4, -2.4);  // Position below the upper platform
    glScaled(0.40, 0.1, 1.5);       // Smaller size for the lower platform
    glutSolidCube(1.0);            // Draw the cube
    glPopMatrix();

    // Create a GLU quadric object for drawing cylinders
    GLUquadricObj* quadric = gluNewQuadric();  // Create a new quadric object

    // Fence (horizontal rod) supported by two vertical rods on each side
    glPushMatrix();
    glTranslated(1.3, 1.5, -2.4); // Position it near the upper platform

    // Horizontal fence (rod) sticking to the back wall
    glPushMatrix();
    glTranslated(0.0, 0.1, 0.0); // Position the horizontal rod slightly above the platform
    glScaled(0.7, 0.05, 0.05);   // Scale it to make it look like a rod
    glutSolidCube(1.0);          // Draw the horizontal rod
    glPopMatrix();

    // Vertical support rods on the left side of the fence
    glPushMatrix();
    glTranslated(-0.35, 0.0, 0.0); // Position the left vertical rod
    glScaled(0.05, 0.2, 0.05);     // Scale it to look like a vertical rod
    gluCylinder(quadric, 0.05, 0.05, 0.2, 10, 10); // Draw the vertical rod
    glPopMatrix();

    // Right vertical support rods
    glPushMatrix();
    glTranslated(0.35, 0.0, 0.0); // Position the right vertical rod
    glScaled(0.05, 0.2, 0.05);     // Scale it to look like a vertical rod
    gluCylinder(quadric, 0.05, 0.05, 0.2, 10, 10); // Draw the vertical rod
    glPopMatrix();

    glPopMatrix();  // End of fence group

    // Delete the quadric object to free memory
    gluDeleteQuadric(quadric);
}



void Display() {
    setupLights();
    setupCamera();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (gameRunning) {
        // Draw left wall
        drawWall(-2.5, 0.0, 0.0, 0.1, 5.0, 5.0, 0.8, 0.8, 0.8, 1, 0, 0);
        
        // Draw right wall
        drawWall(2.5, 0.0, 0.0, 0.1, 5.0, 5.0, 0.8, 0.8, 0.8, -1, 0, 0);
        
        // Draw back wall
        drawWall(0.0, 0.0, -2.5, 5.0, 5.0, 0.1, 0.8, 0.8, 0.8, 0, 0, 1);
        
        updatePlayerPosition(); // Apply gravity and jumping
        
        // Draw ground
        drawGround();
        
        // Draw player
        drawPlayer();
        
        // Draw diving platform
        drawDivingPlatform();
        
        // Rotate rings if active
        if (isRotationActive) {
            rotationAngle += 1.0f; // Adjust speed if needed
            if (rotationAngle >= 360.0f) {
                rotationAngle -= 360.0f;
            }
        }
        // Draw the Olympic rings on the left wall
        drawOlympicRings();
        
        // Draw the door on the back wall
        drawDoorOnBackWall();
        
        drawLifeRingOnBackWall();
        
        drawLadder();
        
        drawScoreboard();
        
        // Update scene logic
        updateScene();
        
        drawLifeJacket();
        
        // Draw pool to the right of the scene at ground level
        drawPool(2.5, -0.5, 0.0, 2.0, 4.0, 1.0, 0.1, 0.95, 0.95, 0.9);
        
        updateTimerEnd(NULL);
        
        // Draw goal object
        if (checkCollisionWithGoal()) {
            glColor3f(1.0, 1.0, 0.0);  // Goal turns yellow when collected
        } else {
            glColor3f(1.0, 0.0, 0.0);  // Goal is red by default
        }
        
        glFlush();
    }
    else {
        // Ensure orthographic projection
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Determine the message
            std::string gameOverText = lifeJacketVisible ? "YOU LOST!" : "YOU WON!";
            int textWidth = 0;
            for (char c : gameOverText) {
                textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);
            }

            // Position text at window center
            glColor3f(1.0f, 0.0f, 0.0f);
            glRasterPos2f((WINDOW_WIDTH - textWidth) / 2, WINDOW_HEIGHT / 2);

            // Render the message
            for (char c : gameOverText) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
            }

        
    }
    glutSwapBuffers();
}

// End game and display the game over screen
void endGame() {
    gameRunning = false;
}

void updateTimerEnd(int value){
    // Decrease time remaining
    timeRemaining--;
    if (timeRemaining <= 0) {
        endGame();
    }
    
    glutPostRedisplay();
   

}



int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("Olympic Diving Game");
    glutDisplayFunc(Display);
    glutKeyboardFunc(handleKeys);        // Remove this line (if you used for normal keys)
    glutSpecialFunc(handleSpecialKeys);  // Add this line to handle special keys

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glutTimerFunc(2000, updateWallColors, 0);
    glutTimerFunc(500, updateLifeJacketBrightness, 0); // Start the brightness animation timer
    glutTimerFunc(16, translatelifeRing, 0);  // Start the update loop
    glutTimerFunc(16, scaleDivingPlatform, 0);  // Set up the next update (about 60 FPS)

    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glutMainLoop();
}
