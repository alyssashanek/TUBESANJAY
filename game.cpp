#include "game.h"
#include "globals.h"
#include "environment.h"
#include "track.h"
#include "car.h"
#include "ui.h"
#include "helpers.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <algorithm>

using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Inisialisasi OpenGl dan game
void setup(void) {

    // Aktifkan depth dan lighting
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Lampu utama
    glEnable(GL_LIGHT1); // Headlight kiri mobil
    glEnable(GL_LIGHT2); // Headlight kanan mobil

    // Cahaya ambient global (Cahaya dasar supaya scene tidak gelap)
    GLfloat globalAmbient[] = {0.55f, 0.55f, 0.55f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Cahaya diffuse matahari (Warna cahaya utama yang menyinari objek)
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 0.95f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    // Cahaya specular matahari (Efek pantulan cahaya pada objek)
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Pantulan cahaya untuk headlight kiri
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);

    // Pantulan cahaya untuk headlight kanan
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);

    // Sudut sorot lampu mobil
    GLfloat spotCutoff = 60.0f;
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotCutoff);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spotCutoff);

    // Mengaktifkan warna material untuk objek
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Menghaluskan shading objek
    glShadeModel(GL_SMOOTH);

    // Menormalkan normal vector lighting untuk efek cahaya
    glEnable(GL_NORMALIZE);

    // Lighting mengikuti arah kamera
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Inisialisasi lampu start lintasan
    currentLightRow = -1;
    updateLightSequence(0);

    // Inisialisasi confetti kiri dan kanan
    initConfetti(confettiCannon1, 200.0, 0.0, 100);
    initConfetti(confettiCannon2, 280.0, 10.0, 100);

    // Load tekstur
    loadGrassTexture();
    loadBillboardTexture();
}

// Mengatur ukuran window dan kamera
void resize(int w, int h) {

    // Mencegah height bernilai 0
    if (h == 0) {
        h = 1;
    }

    // Mengatur area render sesuai ukuran window
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    // Mengatur matrix proyeksi untuk perspektif 3D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Menghitung rasio window 
    float aspect = (float)w / (float)h;

    // Mengatur perspektif kamera
    gluPerspective(70.0, aspect, 1.0, 2000.0);

    // Kembali ke model view
    glMatrixMode(GL_MODELVIEW);
}

// Mengatur checkpoint dan waktu lap
void updateCheckpoint(float x, float z) {
    switch (currentCheckpoint) {

        // Start lap
        case 0:
            if (z > 0 && !timerRunning) {
                currentCheckpoint++;

                // Mulai timer lap
                lapStartTime = glutGet(GLUT_ELAPSED_TIME);
                timerRunning = true;
                std::cout << "Lap started!\n";
            }

            break;
        
        // Checkpoint 1
        case 1:
            if (z > 280) {
                currentCheckpoint++;
            }

            break;

        // Checkpoint 2
        case 2:
            if (x < -200) {
                currentCheckpoint++;
            }

            break;

        // Checkpoint 3
        case 3:
            if (z < -320) {
                currentCheckpoint++;
            }

            break;

        // Checkpoint 4
        case 4:
            if (x > 0 && z > 60) {
                currentCheckpoint++;
            }

            break;
        
        // Checkpoint 5
        case 5:
            if (x > 240) {
                currentCheckpoint++;
            }

            break;
        
        // Finish line
        case 6:
            if (z > 0) {
                currentCheckpoint++;

                // Stop timer
                timerRunning = false;
                lapEndTime = glutGet(GLUT_ELAPSED_TIME);

                // Hitung waktu lap
                float lapTime = (lapEndTime - lapStartTime) / 1000.0f;

                // Update best lap time jika lebih cepat dari sebelumnya
                if (!hasBestLapTime || lapTime < bestLapTime) {
                    bestLapTime = lapTime;
                    hasBestLapTime = true;
                    std::cout << "New best lap: " << bestLapTime << " seconds.\n";
                } else {
                    std::cout << "Lap completed in " << lapTime << " seconds.\n";
                }

                // Reset lampu start
                currentLightRow = -1;
            }

            break;
    }
}

// Mengecek apakah posisi mobil berada di dalam area collision
int isInsideAnyBox(float x, float z, float boxes[][6], int numBoxes) {

    // Radius area collision untuk mobil
    float radius = 5.0f;

    for (int i = 0; i < numBoxes; i++) {
        // Batas minimum dan maksimum dari box
        float x1 = fmin(boxes[i][0], boxes[i][3]) - radius;
        float x2 = fmax(boxes[i][0], boxes[i][3]) + radius;
        float z1 = fmin(boxes[i][2], boxes[i][5]) - radius;
        float z2 = fmax(boxes[i][2], boxes[i][5]) + radius;

        // Mengecek apakah pusat mobil berada di dalam box yang diperluas dengan radius
        if (x >= x1 && x <= x2 && z >= z1 && z <= z2) {
            return 1;
        }
    }
    
    // Posisi mobil tidak berada di dalam area collision
    return 0;
}

// Mengecek apakah posisi mobil berada di dalam sudut tertentu
bool isWithinAngles(float x, float z, float cx, float cz, float startAngle, float endAngle) {
    
    // Menghitung sudut titik terhadap pusat lingkaran
    float angleToPoint = atan2(z - cz, x - cx) * 180.0 / PI;

    // Mengubah sudut negatif menjadi positif
    if (angleToPoint < 0) {
        angleToPoint += 360;
    }

    // Normalisasi sudut awal dan akhir agar berada dalam rentang 0-360
    float normalizedStartAngle = fmod(startAngle, 360);
    float normalizedEndAngle = fmod(endAngle, 360);

    // Jika sudut tidak melewati 0 derajat 
    if (normalizedStartAngle <= normalizedEndAngle) {
        return angleToPoint >= normalizedStartAngle && angleToPoint <= normalizedEndAngle;
    } else { // Jika sudut melewati 0 derajat
        return angleToPoint >= normalizedStartAngle || angleToPoint <= normalizedEndAngle;
    }
}

// Mengecek apakah posisi mobil menyentuh area lingkaran collision
int isInsideAnyCircle(float x, float z, float circles[][7], int numCircles) {
    
    // Radius area collision untuk mobil
    float carRadius = 6;

    for (int i = 0; i < numCircles; i++) {

        // Titik pusat lingkaran
        float cx = circles[i][0];
        float cz = circles[i][2];

        // Radius luar dan dalam
        float outerRadius = circles[i][3] + carRadius;
        float innerRadius = circles[i][4] - carRadius;

        // Sudut area lingkaran
        float startAngle = circles[i][5] * 180.0 / PI;
        float endAngle = circles[i][6] * 180.0 / PI;

        // Radius kuadrat untuk perhitungan jarak tanpa akar
        float outerRadiusSquared = outerRadius * outerRadius;
        float innerRadiusSquared = innerRadius * innerRadius;

        // Jarak posisi mobil ke pusat lingkaran
        float dx = x - cx;
        float dz = z - cz;
        float distSquared = dx * dx + dz * dz;

        // Mengecek collision pada area lingkaran dengan mempertimbangkan sudutnya
        if (distSquared <= outerRadiusSquared && distSquared >= innerRadiusSquared && isWithinAngles(x, z, cx, cz, startAngle, endAngle)) {
            return 1; // Terjadi collision
        }
    }

    // Tidak terjadi collision
    return 0;
}

// Update game setiap frame
void update(int value) {

    // Pengaturan siang dan malam
    if (day) { // Siang hari

        // Aktifkan cahaya matahari
        glEnable(GL_LIGHT0);

        // Matikan headlight mobil saat siang (dinonaktifkan)
        if (headlightMode == 3) {
            glDisable(GL_LIGHT1);
            glDisable(GL_LIGHT2);
        }

    } else { // Malam hari

        // Matikan cahaya matahari
        glDisable(GL_LIGHT0);

        // Nyalakakan headlight mobil saat malam (diaktifkan)
        if (headlightMode == 3) {
            glEnable(GL_LIGHT1);
            glEnable(GL_LIGHT2);
        }
    }
    
    // Mobil maju
    if (keyStates['w']) {
        velocity += acceleration;

        // Batasi kecepatan maksimum
        if (velocity > maxVelocity) {
            velocity = maxVelocity;
        }
    } 
    
    // Mobil mundur atau rem
    else if (keyStates['s'] && lapStartTime) {
        velocity -= deceleration;

        // Batasi kecepatan minimum
        if (velocity < -maxVelocity) {
            velocity = -maxVelocity;
        }
    } 
    
    // Otomatis melambat saat tidak menekan tombol
    else { 
        if (velocity > 0) {
            velocity -= deceleration;
        } else if (velocity < 0) {
            velocity += deceleration;
        }

        // Hentikan mobil sepenuhnya jika sangat lambat
        if (std::abs(velocity) < deceleration) {
            velocity = 0;
        }
    }

    // Maksimum sudut roda saat berbelok
    const float maxWheelAngle = 35.0f;

    // Kecepatan perubahan sudut roda saat berbelok
    const float wheelAngleStep = 6.5f;

    // Belok saat mobil bergerak
    if (velocity != 0 && lapStartTime) {

        // Pengaruh kecepatan terhadap steering
        float speedFactor = fabs(velocity) / maxVelocity;
        float steeringPower = 2.2f - (0.7f * speedFactor);

        // Minimum steering power
        if (steeringPower < 1.25f) {
            steeringPower = 1.25f;
        }

        // Besar perubahan arah mobil berdasarkan kecepatan dan belokan
        float turnAdjustment = turnSpeed * steeringPower * (velocity > 0 ? 1 : -1);

        // Belok kiri
        if (keyStates['a']) {
            angleX += turnAdjustment;
            wheelAngle += wheelAngle < maxWheelAngle ? wheelAngleStep : 0;

            // Batasi sudut roda
            wheelAngle = std::min(wheelAngle, maxWheelAngle);
        } 
        
        // Belok kanan
        else if (keyStates['d']) {
            angleX -= turnAdjustment;
            wheelAngle -= wheelAngle > -maxWheelAngle ? wheelAngleStep : 0;

            // Batasi sudut roda
            wheelAngle = std::max(wheelAngle, -maxWheelAngle);
        }
    } 
    
    // Mengembalikan posisi roda ke tengah saat tidak berbelok atau mobil tidak bergerak
    else {
        if (wheelAngle < 0) {
            wheelAngle += wheelAngleStep;
            wheelAngle = std::min(wheelAngle, 0.0f);
        } else if (wheelAngle > 0) {
            wheelAngle -= wheelAngleStep;
            wheelAngle = std::max(wheelAngle, 0.0f);
        }
    }

    // Posisi baru mobil
    float proposedMeZ = meZ + velocity * cos(angleX * PI / 180);
    float proposedMeX = meX + velocity * sin(angleX * PI / 180);

    // Collision lintasan
    if (!isInsideAnyBox(proposedMeX, proposedMeZ, axisBarriers, axisBarriersCount) &&
        !isInsideAnyCircle(proposedMeX, proposedMeZ, curveBarriers, curveBarriersCount)) {
        
        // Update posisi mobil jika tidak terjadi collision 
        meZ = proposedMeZ;
        meX = proposedMeX;
    } 
    
    // Efek pantulan saat tabrakan dengan dinding
    else {
        float elasticity = 0.25;
        velocity = -velocity * elasticity;
        meZ += velocity * cos(angleX * PI / 180);
        meX += velocity * sin(angleX * PI / 180);
    }
    
    // Update objek animasi
    if(!useIdleFunc){
        for (int i = 0; i < 6; i++) {
            // Update gerakan awan
            cloudPositions[i].z += cloudSpeed;

            // Reset posisi awan saat melwari batas
            if (cloudPositions[i].z > 25.0) {
                cloudPositions[i].z = -25.0;
            }
        }

        // Rotasi piala
        trophyRotationAngle += 2.0f;

        // Reset rotasi piala
        if (trophyRotationAngle > 360.0f) {
            trophyRotationAngle -= 360.0f;
        }
    }

    // Render ulang scene
    glutPostRedisplay();

    // Timer update berikutnya
    glutTimerFunc(16, update, 0);
    
    // Update checkpoint
    updateCheckpoint(meX, meZ);
}

// Render semua scene game
void drawScene(void){

    // Menampilkan start screen sebelum game dimulai
    if (!gameStarted) {
        drawStartScreen();
        return;
    }

    // Ukuran window
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    // Mencegah height bernilai 0
    if (h == 0) {
        h = 1;
    }

    // Area render sesuai ukuran window
    glViewport(0, 0, w, h);

    // Setup perspektif kamera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100.0, (float)w / (float)h, 1.0, 2000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Warna langit siang dan malam
    if(day) {
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    } else {
        glClearColor(0.05, 0.05, 0.25, 1.0);
    }
    
    // Membersihkan frame sebelumnya
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Posisi kamera
    float baseCameraX, baseCameraY, baseCameraZ;

    // Titik yang dilihat kamera
    float targetX, targetY, targetZ;

    // Pengaturan kamera FPV dan TPV
    float cameraDistance = fpv ? 0.1f : 90.0f;
    float cameraHeight = fpv ? 10.0f : 35.0f;

    // Jarak kamera samping
    float sideOffset = 70.0f;

    // Posisi default kamera di belakang mobil
    baseCameraX = meX - cameraDistance * sin(angleX * PI / 180);
    baseCameraZ = meZ - cameraDistance * cos(angleX * PI / 180);
    baseCameraY = meY + cameraHeight;

    // Titik fokus kamera
    targetX = meX;
    targetY = meY + 10;
    targetZ = meZ;

    // Kamera melihat ke belakang
    if (lookBehind) {
        baseCameraX = meX + cameraDistance * sin(angleX * PI / 180);
        baseCameraZ = meZ + cameraDistance * cos(angleX * PI / 180);
    } 
    
    // Kamera melihat ke kiri
    else if (lookLeft) {
        baseCameraX = meX + sideOffset * cos(angleX * PI / 180);
        baseCameraZ = meZ - sideOffset * sin(angleX * PI / 180);
    } 
    
    // Kamera melihat ke kanan
    else if (lookRight) {
        baseCameraX = meX - sideOffset * cos(angleX * PI / 180);
        baseCameraZ = meZ + sideOffset * sin(angleX * PI / 180);
    }

    // Setup kamera
    gluLookAt(
        baseCameraX, baseCameraY, baseCameraZ,
        targetX, targetY, targetZ,
        0.0f, 1.0f, 0.0f
    );

    // Posisi cahaya matahari
    GLfloat sunlightPos[] = {250.0f, 350.0f, 250.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sunlightPos);

    // Render environment, lintasan, dan objek
    drawGrass();
    drawBillboard();
    drawHill(-200, 0, 450, 90);
    drawClouds();
    drawTrees();
    drawTrack();
    drawStartFinishLine();
    drawStartLight();
    drawTrophy();
    drawRacecar();
    drawHUDGauge();

    if (day) {
        drawSun();
    } else {
        drawMoon();
    }
    
    // Timer lap berjalan
    if (timerRunning) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        currentLapTime = (currentTime - lapStartTime) / 1000.0f;
        char currentLapTimeText[100];
        sprintf(currentLapTimeText, "Current Lap Time: %.2f seconds", currentLapTime);
        setOrthographicProjection();
        drawText(currentLapTimeText, 10, 50);
        resetPerspectiveProjection();
    }

    // Setelah lap selesai, tampilkan confetti dan waktu lap
    if (currentCheckpoint > 6) {

        // Confetti untuk perayaan lap selesai
        updateAndDrawConfetti(confettiCannon1);
        updateAndDrawConfetti(confettiCannon2);

        // Teks waktu lap
        char lapTimeText[100];
        float lapTime = (lapEndTime - lapStartTime) / 1000.0f;
        sprintf(lapTimeText, "Lap completed in %.2f seconds.", lapTime);
        setOrthographicProjection();
        int startX = 10;
        int startY = 50;
        int lineGap = 30;
        drawText(lapTimeText, startX, startY);

        // Teks best lap time
        char bestLapText[100];
        if (hasBestLapTime) {
            sprintf(bestLapText, "Best Lap: %.2f seconds", bestLapTime);
        } else {
            sprintf(bestLapText, "Best Lap: --");
        }
        drawText(bestLapText, startX, startY + lineGap);
        drawText("Press 'r' to restart.", startX, startY + lineGap * 2);
        resetPerspectiveProjection();
    }

    glutSwapBuffers();
}

// Update urutan lampu start
void updateLightSequence(int value) {

    // Selama lampu belum menyala semua
    if (currentLightRow < 3) {
        currentLightRow++;

        // Render ulang scene
        glutPostRedisplay(); 

        // Timer untuk lampu berikutnya
        glutTimerFunc(lightUpdateTime, updateLightSequence, 0);
    }
}

// Mengatur ukuran start screen saat window diresize
void startScreenResize(int w, int h) {
    // Mencegah pembagian dengan nol
    if (h == 0) {
        h = 1;
    }

    // Area render sesuai ukuran window
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    // Setup proyeksi 2D untuk start screen
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Menyesuaikan ukuran window
    gluOrtho2D(0.0, w, 0.0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Timer global game
void globalTimer(int value) {

    // Pindah ke window utama
    glutSetWindow(mainWindow);

    // Render ulang scene
    glutPostRedisplay();

    // Jalankan timer lagi setelah 16ms
    glutTimerFunc(16, globalTimer, 0);
}

// Update animasi saat idle (gerakan awan dan rotasi piala)
void idle() {
    for (int i = 0; i < 6; i++) {
        // Update posisi dan gerakan awan
        cloudPositions[i].z += cloudSpeed;

        // Reset posisi awan saat melewati batas
        if (cloudPositions[i].z > 25.0) {
            cloudPositions[i].z = -25.0;
        }
    }

    // Rotasi piala
    trophyRotationAngle += 2.0f;

    // Reset rotasi piala
    if (trophyRotationAngle > 360.0f) {
        trophyRotationAngle -= 360.0f;
    }

    // Render ulang scene
    glutPostRedisplay();
}

// Menampilkan petunjuk kontrol game
void printInteraction(void) {
    cout << "Petunjuk Kontrol Permainan:" << endl;

    cout << "Kontrol Umum:" << endl;
    cout << "\tESC - Keluar dari permainan." << endl;
    cout << "\tR - Reset permainan, mengembalikan posisi mobil serta mengatur ulang timer lap dan kecepatan." << endl;

    cout << "Kontrol Pergerakan:" << endl;
    cout << "\tW - Menambah kecepatan mobil." << endl;
    cout << "\tS - Mengurangi kecepatan atau mundur." << endl;
    cout << "\tA - Belok ke kiri." << endl;
    cout << "\tD - Belok ke kanan." << endl;

    cout << "Kontrol Kamera dan Pandangan:" << endl;
    cout << "\tC - Melihat ke belakang." << endl;
    cout << "\tQ - Melihat ke kiri tanpa mengubah arah mobil." << endl;
    cout << "\tE - Melihat ke kanan tanpa mengubah arah mobil." << endl;
    cout << "\tX - Mengganti tampilan First Person View (FPV) dan Third Person View." << endl;

    cout << "Kontrol Lampu:" << endl;
    cout << "\tH - Mengganti mode lampu mobil: Auto, Mati, Low Beam, dan High Beam." << endl;

    cout << "Kontrol Tambahan (Menembus Tembok):" << endl;
    cout << "\tPanah Atas - Bergerak maju mengikuti arah mobil." << endl;
    cout << "\tPanah Bawah - Bergerak mundur mengikuti arah mobil." << endl;

    cout << "Kontrol Arah Pandang:" << endl;
    cout << "\tPanah Kiri - Memutar arah pandang ke kiri tanpa bergerak." << endl;
    cout << "\tPanah Kanan - Memutar arah pandang ke kanan tanpa bergerak." << endl;

    cout << "Kontrol Mouse:" << endl;
    cout << "\tKlik kiri pada start window untuk memilih mode dan memulai permainan." << endl;
    cout << "\tKlik kanan saat bermain untuk membuka menu popup dan mengganti mode siang/malam." << endl;
}

// Memulai game utama
void switchToMainGame() {

    // Pindah ke window utama
    glutSetWindow(mainWindow);

    // Reset posisi mobil
    meX = 240;
    meY = 0;
    meZ = -40;

    // Reset rotasi mobil
    angleX = 0;
    angleY = (headlightMode == 2 ? -1 : -1.25);

    // Reset checkpoint dan timer
    currentCheckpoint = 0;
    timerRunning = false;
    lapStartTime = 0;

    // Reset kecepatan
    velocity = 0;

    // Reset lampu start
    currentLightRow = -1;
    updateLightSequence(0);

    // Mulai game
    gameStarted = true;

    // Render ulang scene
    glutPostRedisplay();
}