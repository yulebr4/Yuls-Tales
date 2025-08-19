	#include <iostream>
	#include <vector>
	#include <cmath>
	#include <cstdlib>
	#include <ctime>
	#include <sstream>
	#include <GL/gl.h>      
    #include <GL/glu.h>
	#include <GL/freeglut.h>
	#include <GL/glu.h>
	#include <locale.h>
	#include <SDL.h>
	#include <SDL_mixer.h>

	
	
	
	
	using namespace std;
	
	 
	
	void setSpanishLocale() {
	    // Configurar el locale para español
	    setlocale(LC_ALL, "es_ES.UTF-8");
	    // Alternativa para Windows:
	    // setlocale(LC_ALL, "spanish");
	}
	
	
	// Función para convertir int a string
	string intToString(int value) {
	    stringstream ss;
	    ss << value;
	    return ss.str();
	}
	
	// Función para convertir float a string con formato de tiempo
	string timeToString(float seconds) {
	    int minutes = (int)(seconds / 60);
	    int secs = (int)(seconds) % 60;
	    int centisecs = (int)((seconds - (int)seconds) * 100);
	    
	    stringstream ss;
	    if (minutes > 0) {
	        ss << minutes << ":";
	        if (secs < 10) ss << "0";
	    }
	    ss << secs << ".";
	    if (centisecs < 10) ss << "0";
	    ss << centisecs;
	    return ss.str();
	}
	
	// Constantes del juego
	const int WIDTH = 800, HEIGHT = 600;
	const float GRAVITY = 0.4f;
	const float JUMP_STRENGTH = 12.0f;
	const float FRICTION = 0.85f;
	const float MOVE_SPEED = 4.0f;
	const float BOUNCE_DAMPING = 0.8f;
	const float CLOUD_SPEED = 0.5f;
	const float FINAL_OBJECT_X = 3200.0f;
	const float BACKGROUND_CHANGE_TIME = 30.0f;
	const float BLINK_DURATION = 2.0f;
	const float BLINK_FREQUENCY = 0.3f;
	const float TEXT_DISPLAY_TIME = 3.0f;
	const float PLAYER_START_Y = 300.0f;
	const float PLAYER_MAX_HEALTH = 100.0f;
	const float ENEMY_MAX_HEALTH = 100.0f;
	const float BOSS_HEALTH_DESERT = 300.0f;
	const float BOSS_HEALTH_SPACE = 250.0f;
	const float BOSS_WIDTH = 150.0f;
	const float BOSS_HEIGHT = 100.0f;
	const float PROJECTILE_SPEED = 10.0f;
	const float PROJECTILE_RADIUS = 5.0f;
	const float FINISH_LINE_WIDTH = 120.0f;
	const float FINISH_LINE_HEIGHT = 150.0f;
	
	
	// Estados del juego
	enum GameState {
	    MENU,
	    LEVEL_SELECT,
	    INSTRUCTIONS,
	    STORY,
	    PLAYING,
	    PAUSED,
	    GAME_OVER,
	    LEVEL_COMPLETE,
	    SHOWING_TEXT,
	    GAME_COMPLETE,  
        ABOUT           
	    
	};
	
	
	
	// Tipos de objetos
	enum ObjectType {
	    COLLECTIBLE,
	    ENEMY,
	    POWER_UP,
	    FINAL_OBJECT
	};
	
	// Tipos de niveles
	enum LevelType {
	    MISTY_MORNING,
	    FOREST_ADVENTURE,
	    DESERT_JOURNEY,
	    NIGHT_FLIGHT,
	    SPACE_ODYSSEY
	};
	
	enum PlayerForm {
    RED_BALL,      // Forma normal (bola roja)
    ROCK_FORM,     // Forma de roca (más pesada)
    BEACH_BALL_FORM // Forma de bola de playa (más liviana)
};
	
	struct BossEnemy {
    float x, y;
    float baseY;           
    float width, height;
    float health;
    float speed;           // velocidad horizontal
    float hoverAmplitude;  // amplitud del bobbing vertical
    bool  active;
    float animationTime;

    BossEnemy(float px=0, float py=0, float w=0, float h=0, float hp=0, float spd=0, float hover=0) {
        x = px; y = py; baseY = py;
        width = w; height = h;
        health = hp; speed = spd;
        hoverAmplitude = hover;
        active = false;
        animationTime = 0.0f;
    }
};
	
	

	struct Projectile {
	    float x, y;
	    float velocityX, velocityY;
	    bool isActive;
	    
	    Projectile(float px, float py, float vx, float vy) {
	        x = px; y = py;
	        velocityX = vx; velocityY = vy;
	        isActive = true;
	    }
	};
	
	
	struct Enemy {
	    float x, y;
	    float width, height;
	    float speed;
	    float health;
	    bool isActive;
	    float animationTime;
	    bool isSquare;
	    
	    Enemy(float px, float py, float w, float h, float s, float hp, bool square = false) {
	        x = px; y = py; width = w; height = h;
	        speed = s; health = hp;
	        isActive = true;
	        animationTime = 0;
	        isSquare = square;
	    }
	};
	
	// Estructura para objetos del juego
	struct GameObject {
	    float x, y;
	    float width, height;
	    ObjectType type;
	    bool collected;
	    float animationTime;
	    
	    GameObject(float px, float py, float w, float h, ObjectType t) {
	        x = px; y = py; width = w; height = h; type = t;
	        collected = false; animationTime = 0;
	    }
	};
	
	// Estructura para plataformas
	struct Platform {
	    float x, y;
	    float width, height;
	    bool isCurved;
	    bool isLog;
	    float curveRadius;
	    bool hasSpikes;
	    bool isBreakable;
	    bool isBroken;
	    bool isMoving;
	    float moveDistance;
	    float moveSpeed;
	    float initialX;
	    bool isPurple;
	    float purpleTimer;
	    bool isRamp;
	    bool isSafe;
	    
	    Platform(float px, float py, float w, float h, bool curved = false, bool log = false, float radius = 0, 
	             bool spikes = false, bool breakable = false, bool moving = false, 
	             float moveDist = 0, float moveSpd = 0, bool ramp = false, bool safe = false) {
	        x = px; y = py; width = w; height = h; 
	        isCurved = curved; 
	        isLog = log;
	        curveRadius = radius;
	        hasSpikes = spikes;
	        isBreakable = breakable;
	        isBroken = false;
	        isMoving = moving;
	        moveDistance = moveDist;
	        moveSpeed = moveSpd;
	        initialX = px;
	        isPurple = false;
	        purpleTimer = 0;
	        isRamp = ramp;
	        isSafe = safe;
	    }
	};
	
	// Estructura del jugador (pelota roja)
	struct Player {
	    float x, y;
	    float radius;
	    float velocityX, velocityY;
	    bool onGround;
	    float bounceStrength;
	    float animationAngle;
	    float squashY;
	    bool isInvincible;
	    float invincibleTimer;
	    float health;
	    float shootCooldown;
	    PlayerForm currentForm;  // Forma actual del jugador
        
	    
	    Player() {
	        x = 100; y = PLAYER_START_Y; radius = 20;
	        velocityX = 0; velocityY = 0; onGround = false;
	        bounceStrength = 0.7f; animationAngle = 0;
	        squashY = 1.0f;
	        isInvincible = false;
	        invincibleTimer = 0;
	        health = PLAYER_MAX_HEALTH;
	        shootCooldown = 0;
	        currentForm = RED_BALL; // Empieza como bola roja
            
	    }
	};
	
	// Estructura de nivel
	struct Level {
	    string name;
	    string subtitle;
	    LevelType type;
	    int targetScore;
	    float colorR, colorG, colorB;
	    bool unlocked;
	    bool hasEnemies;
	    bool hasSpecialEffect;
	    
	    Level(string n, string sub, LevelType t, int target, float r, float g, float b, 
	          bool un = false, bool enemies = false, bool effect = false) {
	        name = n; subtitle = sub; type = t; targetScore = target;
	        colorR = r; colorG = g; colorB = b;
	        unlocked = un;
	        hasEnemies = enemies;
	        hasSpecialEffect = effect;
	    }
	};
	
	// Variables globales
	Player player;
	vector<Platform> platforms;
	vector<GameObject> gameObjects;
	vector<Enemy> enemies;
	vector<Projectile> projectiles;
	vector<Level> levels;
	GameState currentState = MENU;
	int score = 0;
	int lives = 3;
	float cameraX = 0;
	float animationTime = 0;
	float gameTime = 0;
	float backgroundTransition = 0;
	int currentLevel = 0;
	bool keys[256] = {false};
	int selectedMenuItem = 0;
	float menuAnimation = 0;
	bool backgroundChanged = false;
	float blinkTimer = 0;
	bool isBlinking = false;
	int instructionScroll = 0;
	float specialEffectTimer = 0;
	bool effectActive = false;
	bool showConfetti = false;
	float confettiTimer = 0.0f;
	vector<pair<float, float> > confettiParticles;
	BossEnemy boss;
    bool bossDefeated = false;
	int selectedOption = 0; // 0 = SÍ, 1 = NO
	
	// Variables globales para la música
	Mix_Music* menuMusic = NULL;
	Mix_Music* worldMusic[5] = {NULL};
	Mix_Chunk* jumpSound = NULL;
	Mix_Chunk* collectSound = NULL;
	Mix_Chunk* hurtSound = NULL;
	Mix_Chunk* winSound = NULL;      
    Mix_Chunk* starSound = NULL;    
    Mix_Music* endingMusic = NULL;   
	
	
	
	// Variables para controlar los textos
	vector<string> introTexts;
	int currentTextIndex = 0;
	float textDisplayTimer = 0.0f;
	
	// Textos de la historia
	const char* storyTextsArray[] = {
	    "En Yuls Tales, la historia gira en torno a Bounce,",
	    "una pelota roja que descubre que los colores",
	    "y los sonidos estan desapareciendo en su mundo.",
	    "",
	    "En su aventura, Bounce adquiere habilidades para",
	    "cambiar de forma, transformandose en una roca",
	    "y una pelota de playa para superar obstaculos.",
	    "",
	    "Finalmente, Bounce restaura los colores y libera",
	    "a las criaturas hipnotizadas, salvando su mundo."
	};
	const int storyTextsCount = sizeof(storyTextsArray)/sizeof(storyTextsArray[0]);
	
	// Textos de instrucciones
	const char* instructionTextsArray[] = {
	    "CONTROLES DEL JUEGO:",
	    "",
	    "Tecla W / Flecha Arriba: Saltar",
	    "Tecla A / Flecha Izquierda: Mover izquierda",
	    "Tecla D / Flecha Derecha: Mover derecha",
	    "Barra Espaciadora: Saltar",
	    "Tecla F: Disparar proyectiles",
	    "Tecla ESC: Volver al menu",
	    "Tecla P: Pausar el juego",
	    "Tecla T: Transformacion de las bolas",
	    "",
	    "OBJETIVO DEL JUEGO:",
	    "",
	    "Recoge al menos la mitad de las monedas doradas en cada nivel",
	    "para avanzar. Evita caer al vacio.",
	    "",
	    "Los objetos especiales te dan habilidades:",
	    "- Estrella: Teletransporte corto",
	    "",
	    "Al completar cada nivel, desbloquearas el siguiente.",
	    "",
	    "TRANSFORMACIONES (Tecla T):",
        "- Bola Roja: Equilibrada (forma normal)",
        "- Roca: Más pesada, resiste viento, salta menos",
        "- Bola Playa: Más liviana, salta más, menos tracción",
        "Las transformaciones duran 15 segundos",
       "Pulsa T nuevamente para cambiar/cancelar",
        "",
	    "DESBLOQUEO DE NIVELES:",
	    "Los niveles se iran desbloqueando a medida que",
	    "completes mas del 50% de los puntos requeridos",
	    "en cada nivel anterior.",
	    "",
	    "COMBATE:",
	    "- En los ultimos 3 niveles encontraras enemigos",
	    "- Salta sobre ellos o disparales para eliminarlos",
	    "- Evita sus ataques o te quitaran vida",
	    "",
	    "CONSEJOS:",
	    "- Usa el rebote para alcanzar lugares altos",
	    "- Planifica tu ruta para recolectar las monedas",
	    "- Las plataformas curvas te dan mas impulso",
	    "- Las estrellas te permiten teletransportarte",
	    "- Dispara a los enemigos desde lejos para evitar daño",
	    "",
	    "DIVIERTETE JUGANDO YULS TALES!"
	};
	const int instructionTextsCount = sizeof(instructionTextsArray)/sizeof(instructionTextsArray[0]);
	
	vector<string> storyTexts;
	vector<string> instructionTexts;
	
	void playMenuMusic() {
	    if (menuMusic) {
	        Mix_PlayMusic(menuMusic, -1); // -1 para loop infinito
	        Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // Ajustar volumen
	    }
	}
	
	void playWorldMusic(int world) {
	    if (world >= 0 && world < 5 && worldMusic[world]) {
	        Mix_PlayMusic(worldMusic[world], -1); // -1 para loop infinito
	        Mix_VolumeMusic(MIX_MAX_VOLUME / 3); // Ajustar volumen
	    }
	}
	
	void stopMusic() {
	    Mix_HaltMusic();
	}
	
	void pauseMusic() {
	    Mix_PauseMusic();
	}
	
	void resumeMusic() {
	    Mix_ResumeMusic();
	}
	
	
	// Funciones de inicialización de textos
	void initializeStoryTexts() {
	    storyTexts.clear();
	    for (int i = 0; i < storyTextsCount; i++) {
	        storyTexts.push_back(storyTextsArray[i]);
	    }
	}
	
	void initializeInstructionTexts() {
	    instructionTexts.clear();
	    for (int i = 0; i < instructionTextsCount; i++) {
	        instructionTexts.push_back(instructionTextsArray[i]);
	    }
	}
	
	// Funciones de dibujo mejoradas
	void drawCircle(float x, float y, float radius, float r, float g, float b) {
	    glColor3f(r, g, b);
	    glBegin(GL_TRIANGLE_FAN);
	    glVertex2f(x, y);
	    for (int i = 0; i <= 360; i += 10) {
	        float angle = i * 3.14159265f / 180.0f;
	        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
	    }
	    glEnd();
	}
	
	void drawEllipse(float x, float y, float radiusX, float radiusY, float r, float g, float b) {
	    glColor3f(r, g, b);
	    glBegin(GL_TRIANGLE_FAN);
	    glVertex2f(x, y);
	    for (int i = 0; i <= 360; i += 10) {
	        float angle = i * 3.14159265f / 180.0f;
	        glVertex2f(x + cos(angle) * radiusX, y + sin(angle) * radiusY);
	    }
	    glEnd();
	}
	
	void drawRect(float x, float y, float width, float height, float r, float g, float b) {
	    glColor3f(r, g, b);
	    glBegin(GL_QUADS);
	    glVertex2f(x, y);
	    glVertex2f(x + width, y);
	    glVertex2f(x + width, y + height);
	    glVertex2f(x, y + height);
	    glEnd();
	}
	
	void drawSpikes(float x, float y, float width, int count) {
	    glColor3f(0.3f, 0.3f, 0.3f);
	    float spikeWidth = width / count;
	    
	    for (int i = 0; i < count; i++) {
	        glBegin(GL_TRIANGLES);
	        glVertex2f(x + i * spikeWidth, y);
	        glVertex2f(x + (i + 0.5f) * spikeWidth, y + 15);
	        glVertex2f(x + (i + 1) * spikeWidth, y);
	        glEnd();
	    }
	}
	
	void drawCactus(float x, float y, float height) {
	    // Tallo principal
	    glColor3f(0.2f, 0.6f, 0.2f);
	    drawRect(x - 10, y, 20, height, 0.2f, 0.6f, 0.2f);
	    
	    // Brazos del cactus
	    drawRect(x - 10, y + height/2, 40, 20, 0.2f, 0.6f, 0.2f);
	    drawRect(x - 30, y + height/3, 40, 20, 0.2f, 0.6f, 0.2f);
	    
	    // Espinas
	    glColor3f(0.1f, 0.3f, 0.1f);
	    for (int i = 0; i < height; i += 15) {
	        glBegin(GL_LINES);
	        glVertex2f(x - 15, y + i);
	        glVertex2f(x - 25, y + i);
	        glVertex2f(x + 15, y + i);
	        glVertex2f(x + 25, y + i);
	        glEnd();
	    }
	}
	
	void drawLogPlatform(float x, float y, float width, float height) {
	    // Dibujar tronco
	    glColor3f(0.55f, 0.27f, 0.07f);
	    drawRect(x, y, width, height, 0.55f, 0.27f, 0.07f);
	    
	    // Dibujar anillos del tronco
	    glColor3f(0.45f, 0.17f, 0.07f);
	    for (int i = 0; i < height; i += 20) {
	        glBegin(GL_LINE_STRIP);
	        for (int j = 0; j <= width; j += 5) {
	            float wave = sin(j * 0.2f + animationTime) * 2.0f;
	            glVertex2f(x + j, y + i + wave);
	        }
	        glEnd();
	    }
	    
	    // Dibujar hueco en la parte inferior
	    glColor3f(0.2f, 0.1f, 0.05f);
	    float holeWidth = width * 0.6f;
	    float holeHeight = height * 0.3f;
	    drawRect(x + (width - holeWidth)/2, y, holeWidth, holeHeight, 0.2f, 0.1f, 0.05f);
	    
	    // Dibujar musgo
	    glColor3f(0.3f, 0.6f, 0.3f);
	    glBegin(GL_POINTS);
	    for (int i = 0; i < width; i += 5) {
	        if (rand() % 10 == 0) {
	            glVertex2f(x + i, y + height - (rand() % 10));
	        }
	    }
	    glEnd();
	}
	
	void drawGrassPlatform(float x, float y, float width, float height, bool hasSpikes = false, bool isPurple = false) {
	    // Base de tierra
	    if (isPurple && effectActive) {
	        drawRect(x, y, width, height, 0.5f, 0.2f, 0.7f);
	    } else {
	        drawRect(x, y, width, height, 0.55f, 0.27f, 0.07f);
	    }
	    
	    // Césped en la parte superior (siempre verde)
	    glColor3f(0.35f, 0.85f, 0.35f);
	    glBegin(GL_POLYGON);
	    for (int i = 0; i <= 180; i += 15) {
	        float wave = sin(i * 5 * 3.14159265f / 180.0f + animationTime * 2) * 5;
	        glVertex2f(x + (width * i / 180.0f), y + height + wave);
	    }
	    glEnd();
	    
	    // Espinas si las tiene
	    if (hasSpikes) {
	        drawSpikes(x, y + height, width, (int)(width / 20));
	    }
	}
	
	void drawStonePlatform(float x, float y, float width, float height, bool isPurple = false) {
	    // Base de roca
	    if (isPurple && effectActive) {
	        drawRect(x, y, width, height, 0.6f, 0.3f, 0.8f);
	    } else {
	        drawRect(x, y, width, height, 0.4f, 0.4f, 0.4f);
	    }
	    
	    // Detalles de roca
	    if (isPurple && effectActive) {
	        glColor3f(0.5f, 0.2f, 0.7f);
	    } else {
	        glColor3f(0.3f, 0.3f, 0.3f);
	    }
	    
	    glBegin(GL_POLYGON);
	    for (int i = 0; i <= height; i += 20) {
	        float wave = sin(i * 0.2f + animationTime) * (width * 0.2f);
	        glVertex2f(x + width/2 + wave, y + i);
	    }
	    for (int i = height; i >= 0; i -= 20) {
	        float wave = cos(i * 0.2f + animationTime) * (width * 0.2f);
	        glVertex2f(x + width/2 + wave, y + i);
	    }
	    glEnd();
	}
	
	
	
	void drawRamp(float x, float y, float width, float height) {
	    // Base de la rampa
	    glColor3f(0.55f, 0.27f, 0.07f);
	    glBegin(GL_TRIANGLES);
	    glVertex2f(x, y);
	    glVertex2f(x + width, y);
	    glVertex2f(x + width, y + height);
	    glEnd();
	    
	    // Detalles de la rampa
	    glColor3f(0.45f, 0.17f, 0.07f);
	    for (int i = 0; i < width; i += 20) {
	        glBegin(GL_LINES);
	        glVertex2f(x + i, y);
	        glVertex2f(x + i, y + (height * i / width));
	        glEnd();
	    }
	    
	    // Césped en la parte superior
	    glColor3f(0.35f, 0.85f, 0.35f);
	    glBegin(GL_LINE_STRIP);
	    for (int i = 0; i <= width; i += 10) {
	        float grassOffset = sin(i * 0.2f + animationTime * 2) * 5.0f;
	        glVertex2f(x + i, y + (height * i / width) + grassOffset);
	    }
	    glEnd();
	}
	
	void drawCloud(float x, float y, float scale) {
	    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	    
	    drawCircle(x, y, 20 * scale, 1.0f, 1.0f, 1.0f);
	    drawCircle(x + 25 * scale, y, 25 * scale, 1.0f, 1.0f, 1.0f);
	    drawCircle(x + 50 * scale, y, 20 * scale, 1.0f, 1.0f, 1.0f);
	    drawCircle(x + 15 * scale, y + 15 * scale, 18 * scale, 1.0f, 1.0f, 1.0f);
	    drawCircle(x + 35 * scale, y + 15 * scale, 22 * scale, 1.0f, 1.0f, 1.0f);
	}
	
	void drawStar(float x, float y, float size) {
    glColor3f(1.0f, 1.0f, 0.0f);
    
    // Dibujar estrella de 5 puntas
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    
    for (int i = 0; i <= 5; i++) {
        float angle = (i * 72.0f - 90.0f) * 3.14159265f / 180.0f;
        float outerX = x + cos(angle) * size;
        float outerY = y + sin(angle) * size;
        glVertex2f(outerX, outerY);
        
        angle = ((i * 72.0f + 36.0f) - 90.0f) * 3.14159265f / 180.0f;
        float innerX = x + cos(angle) * size * 0.5f;
        float innerY = y + sin(angle) * size * 0.5f;
        glVertex2f(innerX, innerY);
    }
    glEnd();
}
	    

	
	
	void drawCoin(float x, float y, float size) {
    // Parte dorada de la moneda
    glColor3f(1.0f, 0.9f, 0.2f);
    drawCircle(x, y, size, 1.0f, 0.9f, 0.2f);
    
    // Detalle central
    glColor3f(0.9f, 0.8f, 0.1f);
    drawCircle(x, y, size * 0.7f, 0.9f, 0.8f, 0.1f);
    
    // Borde de la moneda
    glColor3f(0.8f, 0.7f, 0.1f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159265f / 180.0f;
        glVertex2f(x + cos(angle) * size, y + sin(angle) * size);
    }
    glEnd();
}
	
	
	void drawEnemy(float x, float y, float width, float height, float animationTime, LevelType levelType, bool isSquare) {
	    // Obtener color actual
	    GLfloat currentColor[4];
	    glGetFloatv(GL_CURRENT_COLOR, currentColor);
	    
	    // Cuerpo del enemigo
	    if (isSquare) {
	        // Enemigo cuadrado
	        switch (levelType) {
	            case DESERT_JOURNEY:
	                glColor3f(0.8f, 0.7f, 0.4f);
	                break;
	            case NIGHT_FLIGHT:
	                glColor3f(0.4f, 0.2f, 0.6f);
	                break;
	            case SPACE_ODYSSEY:
	                glColor3f(0.2f, 0.8f, 0.2f);
	                break;
	            default:
	                glColor3f(0.8f, 0.2f, 0.2f);
	        }
	        
	        drawRect(x, y, width, height, currentColor[0], currentColor[1], currentColor[2]);
	        
	        // Ojos
	        float eyeOffset = sin(animationTime * 5) * 2;
	        glColor3f(1.0f, 1.0f, 1.0f);
	        drawRect(x + width/4, y + height/2 + eyeOffset, width/4, height/4, 1.0f, 1.0f, 1.0f);
	        drawRect(x + width/2, y + height/2 + eyeOffset, width/4, height/4, 1.0f, 1.0f, 1.0f);
	        
	        // Pupilas
	        glColor3f(0.0f, 0.0f, 0.0f);
	        drawRect(x + width/4 + width/8, y + height/2 + eyeOffset, width/8, height/8, 0.0f, 0.0f, 0.0f);
	        drawRect(x + width/2 + width/8, y + height/2 + eyeOffset, width/8, height/8, 0.0f, 0.0f, 0.0f);
	        
	        // Boca
	        glColor3f(0.0f, 0.0f, 0.0f);
	        glBegin(GL_LINE_STRIP);
	        glVertex2f(x + width/4, y + height/3);
	        glVertex2f(x + width/2, y + height/4);
	        glVertex2f(x + 3*width/4, y + height/3);
	        glEnd();
	    } else {
	        // Enemigo circular
	        switch (levelType) {
	            case DESERT_JOURNEY:
	                glColor3f(0.8f, 0.7f, 0.4f);
	                break;
	            case NIGHT_FLIGHT:
	                glColor3f(0.4f, 0.2f, 0.6f);
	                break;
	            case SPACE_ODYSSEY:
	                glColor3f(0.2f, 0.8f, 0.2f);
	                break;
	            default:
	                glColor3f(0.8f, 0.2f, 0.2f);
	        }
	        
	        drawEllipse(x + width/2, y + height/2, width/2, height/2, 
	                   currentColor[0], currentColor[1], currentColor[2]);
	        
	        // Ojos
	        float eyeOffset = sin(animationTime * 5) * 2;
	        glColor3f(1.0f, 1.0f, 1.0f);
	        drawCircle(x + width/3, y + height/2 + eyeOffset, width/6, 1.0f, 1.0f, 1.0f);
	        drawCircle(x + 2*width/3, y + height/2 + eyeOffset, width/6, 1.0f, 1.0f, 1.0f);
	        
	        // Pupilas
	        glColor3f(0.0f, 0.0f, 0.0f);
	        drawCircle(x + width/3, y + height/2 + eyeOffset, width/12, 0.0f, 0.0f, 0.0f);
	        drawCircle(x + 2*width/3, y + height/2 + eyeOffset, width/12, 0.0f, 0.0f, 0.0f);
	        
	        // Boca
	        glColor3f(0.0f, 0.0f, 0.0f);
	        glBegin(GL_LINE_STRIP);
	        for (int i = 0; i <= 10; i++) {
	            float t = (float)i / 10.0f;
	            float angle = 3.14159265f * 0.2f + t * 3.14159265f * 0.6f;
	            glVertex2f(x + width/2 + cos(angle) * width/3, y + height/2 + sin(angle) * width/4);
	        }
	        glEnd();
	    }
	    
	    // Barra de salud del enemigo
	    if (levelType >= DESERT_JOURNEY) {
	        float healthRatio = enemies[0].health / ENEMY_MAX_HEALTH;
	        glColor3f(1.0f - healthRatio, healthRatio, 0.0f);
	        drawRect(x, y - 10, width * healthRatio, 5, 1.0f - healthRatio, healthRatio, 0.0f);
	    }
	}
	
	void drawFinishLine(float x, float y) {
	    // Postes de la meta (más estéticos y grandes)
	    glColor3f(0.3f, 0.3f, 0.3f);
	    drawRect(x, y, 20, FINISH_LINE_HEIGHT, 0.3f, 0.3f, 0.3f);
	    drawRect(x + FINISH_LINE_WIDTH - 20, y, 20, FINISH_LINE_HEIGHT, 0.3f, 0.3f, 0.3f);
	    
	    // Bandera a cuadros (blanco y negro) más arriba y sin espacio
	    glBegin(GL_QUADS);
	    // Primer cuadro (blanco)
	    glColor3f(1.0f, 1.0f, 1.0f);
	    glVertex2f(x + 20, y + FINISH_LINE_HEIGHT - 10);
	    glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT - 10);
	    glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT - 40);
	    glVertex2f(x + 20, y + FINISH_LINE_HEIGHT - 40);
	    
	    // Segundo cuadro (negro)
	    glColor3f(0.0f, 0.0f, 0.0f);
	    glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT - 10);
	    glVertex2f(x + FINISH_LINE_WIDTH - 20, y + FINISH_LINE_HEIGHT - 10);
	    glVertex2f(x + FINISH_LINE_WIDTH - 20, y + FINISH_LINE_HEIGHT - 40);
	    glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT - 40);
	    
	    // Tercer cuadro (negro)
	    glVertex2f(x + 20, y + FINISH_LINE_HEIGHT - 40);
	    glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT - 40);
	    glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT - 70);
	    glVertex2f(x + 20, y + FINISH_LINE_HEIGHT - 70);
	    
	    // Cuarto cuadro (blanco)
	    glColor3f(1.0f, 1.0f, 1.0f);
	    glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT - 40);
	    glVertex2f(x + FINISH_LINE_WIDTH - 20, y + FINISH_LINE_HEIGHT - 40);
	    glVertex2f(x + FINISH_LINE_WIDTH - 20, y + FINISH_LINE_HEIGHT - 70);
	    glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT - 70);
	    glEnd();
	    
	    // Efecto de brillo en la meta
	    if (showConfetti) {
	        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	        glBegin(GL_TRIANGLES);
	        glVertex2f(x + FINISH_LINE_WIDTH/2, y + FINISH_LINE_HEIGHT/2);
	        glVertex2f(x + FINISH_LINE_WIDTH/2 + 40 * cos(animationTime * 2), 
	                  y + FINISH_LINE_HEIGHT/2 + 40 * sin(animationTime * 2));
	        glVertex2f(x + FINISH_LINE_WIDTH/2 + 40 * cos(animationTime * 2 + 3.14159265f/2), 
	                  y + FINISH_LINE_HEIGHT/2 + 40 * sin(animationTime * 2 + 3.14159265f/2));
	        glEnd();
	    }
	}
	
	void drawConfetti() {
	    if (!showConfetti) return;
	    
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    
	    for (size_t i = 0; i < confettiParticles.size(); i++) {
	        float x = confettiParticles[i].first;
	        float y = confettiParticles[i].second;
	        
	        // Colores aleatorios para el confeti
	        float r = (rand() % 100) / 100.0f;
	        float g = (rand() % 100) / 100.0f;
	        float b = (rand() % 100) / 100.0f;
	        
	        glColor4f(r, g, b, 0.7f);
	        drawRect(x, y, 5, 5, r, g, b);
	    }
	    
	    glDisable(GL_BLEND);
	}
	
	void drawProjectile(float x, float y) {
	    glColor3f(1.0f, 1.0f, 0.0f);
	    drawCircle(x, y, PROJECTILE_RADIUS, 1.0f, 1.0f, 0.0f);
	    
	    // Efecto de cola
	    glColor4f(1.0f, 0.5f, 0.0f, 0.7f);
	    glBegin(GL_TRIANGLE_STRIP);
	    glVertex2f(x - PROJECTILE_RADIUS * 2, y);
	    glVertex2f(x, y + PROJECTILE_RADIUS);
	    glVertex2f(x, y - PROJECTILE_RADIUS);
	    glEnd();
	}
	
	void drawPlatformBase(const Platform& platform) {
	    if (levels[currentLevel].type == NIGHT_FLIGHT) {
	        glColor3f(1.0f, 1.0f, 1.0f);  // blanco lunar
	        glEnable(GL_POLYGON_STIPPLE);  // efecto arenoso
	        GLubyte sandPattern[128] = {
	            0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	            0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
	            0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	            0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
	            0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	            0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
	            0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	            0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
	            0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	            0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
	            0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	            0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
	            0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	            0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
	            0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	            0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa
	        };
	        glPolygonStipple(sandPattern);
	    } else {
	        glDisable(GL_POLYGON_STIPPLE);
	    }
	
	    glBegin(GL_QUADS);
	    glVertex2f(platform.x, platform.y);
	    glVertex2f(platform.x + platform.width, platform.y);
	    glVertex2f(platform.x + platform.width, platform.y + platform.height);
	    glVertex2f(platform.x, platform.y + platform.height);
	    glEnd();
	    
	    glDisable(GL_POLYGON_STIPPLE); // Asegurar que se desactive después del uso
	}
	
	void renderTextCentered(float x, float y, const string& text, float size) {
	    float length = glutStrokeLength(GLUT_STROKE_ROMAN, reinterpret_cast<const unsigned char*>(text.c_str()));
	    float scale = size / 100.0f;
	
	    glPushMatrix();
	    glTranslatef(x - (length * scale / 2.0f), y, 0.0f);
	    glScalef(scale, scale, 1.0f);
	
	    // Sombra negra
	    glColor3f(0.0f, 0.0f, 0.0f);
	    glPushMatrix();
	    glTranslatef(2.0f, -2.0f, 0.0f); // desplazamiento sombra
	    for (size_t i = 0; i < text.length(); ++i) {
	        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
	    }
	    glPopMatrix();
	
	    // Texto principal blanco
	    glColor3f(1.0f, 1.0f, 1.0f);
	    for (size_t i = 0; i < text.length(); ++i) {
	        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
	    }
	
	    glPopMatrix();
	}
	
	
	void drawPlatforms() {
	    for (size_t i = 0; i < platforms.size(); i++) {
	        const Platform& platform = platforms[i];
	        if (platform.isBroken) continue;
	        
	        if (platform.isLog) {
	            drawLogPlatform(platform.x, platform.y, platform.width, platform.height);
	        } 
	        else if (levels[currentLevel].type == SPACE_ODYSSEY) {
	            // Plataformas espaciales con animación
	            glColor3f(0.1f, 0.1f, 0.4f); // Azul marino
	            float pulse = sin(animationTime * 3) * 0.1f + 1.0f;
	            
	            glBegin(GL_QUADS);
	            glVertex2f(platform.x, platform.y);
	            glVertex2f(platform.x + platform.width, platform.y);
	            glVertex2f(platform.x + platform.width * pulse, platform.y + platform.height);
	            glVertex2f(platform.x, platform.y + platform.height);
	            glEnd();
	            
	            // Efecto de energía
	            glColor4f(0.2f, 0.2f, 0.8f, 0.5f);
	            glBegin(GL_LINE_LOOP);
	            glVertex2f(platform.x, platform.y);
	            glVertex2f(platform.x + platform.width, platform.y);
	            glVertex2f(platform.x + platform.width, platform.y + platform.height);
	            glVertex2f(platform.x, platform.y + platform.height);
	            glEnd();
	        }
	        else if (levels[currentLevel].type == DESERT_JOURNEY) {
	            // Plataformas de arena con efecto de ondulación
	            glColor3f(0.7f, 0.6f, 0.3f);
	            glBegin(GL_QUADS);
	            for (int j = 0; j < platform.width; j += 20) {
	                float wave = sin(animationTime * 2 + j * 0.1f) * 5.0f;
	                glVertex2f(platform.x + j, platform.y + wave);
	                glVertex2f(platform.x + j + 20, platform.y + wave);
	                glVertex2f(platform.x + j + 20, platform.y + platform.height);
	                glVertex2f(platform.x + j, platform.y + platform.height);
	            }
	            glEnd();
	            
	            if (platform.hasSpikes) {
	                drawSpikes(platform.x, platform.y + platform.height, platform.width, 
	                         (int)(platform.width / 20));
	            }
	        }
	    
	        
	        else if (platform.isRamp) {
	            drawRamp(platform.x, platform.y, platform.width, platform.height);
	        }
	        else {
	            // Aquí aplicamos el efecto especial para NIGHT_FLIGHT
	            if (levels[currentLevel].type == NIGHT_FLIGHT) {
	                drawPlatformBase(platform);
	            } else {
	                // Para otros niveles, usar el código original
	                switch (levels[currentLevel].type) {
	                    case FOREST_ADVENTURE:
	                        drawGrassPlatform(platform.x, platform.y, platform.width, platform.height, 
	                                         platform.hasSpikes, platform.isPurple);
	                        break;
	                    default:
	                        drawGrassPlatform(platform.x, platform.y, platform.width, platform.height, 
	                                         platform.hasSpikes, platform.isPurple);
	                }
	            }
	        }
	    }
	}
	
	void drawStaticBackground() {
	    // Fondo del menú siempre azul con nubes
	    if (currentState == MENU || currentState == LEVEL_SELECT || currentState == INSTRUCTIONS || currentState == STORY || currentState == PAUSED) {
	        // Fondo degradado azul
	        glBegin(GL_QUADS);
	        glColor3f(0.2f, 0.4f, 0.8f);     glVertex2f(0, HEIGHT);
	        glColor3f(0.2f, 0.4f, 0.8f);     glVertex2f(WIDTH, HEIGHT);
	        glColor3f(0.5f, 0.7f, 1.0f);     glVertex2f(WIDTH, 0);
	        glColor3f(0.5f, 0.7f, 1.0f);     glVertex2f(0, 0);
	        glEnd();
	        
	        // Nubes
	        glEnable(GL_BLEND);
	        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	        for (int i = 0; i < 30; i++) {
	            float cloudX = i * 200 + (CLOUD_SPEED * gameTime);
	            while (cloudX > WIDTH + 1600) cloudX -= 2400;
	            float cloudY = 420 + (i % 5) * 8;
	            if (cloudX > -200 && cloudX < WIDTH + 1800) {
	                float scale = 0.8f + (i % 3) * 0.2f;
	                drawCloud(cloudX, cloudY, scale);
	            }
	        }
	        glDisable(GL_BLEND);
	        return;
	    }
	    
	    Level& level = levels[currentLevel];
	
	    float topR = level.colorR;
	    float topG = level.colorG;
	    float topB = level.colorB;
	
	    float bottomR = 0.7f;
	    float bottomG = 0.9f;
	    float bottomB = 1.0f;
	
	    // Efecto especial para el nivel 2 (parpadeo morado)
	    if (level.hasSpecialEffect && effectActive) {
	        topR = 0.6f;
	        topG = 0.2f;
	        topB = 0.8f;
	        bottomR = 0.4f;
	        bottomG = 0.1f;
	        bottomB = 0.6f;
	    }
	
	    // Fondo degradado vertical
	    glBegin(GL_QUADS);
	    glColor3f(topR, topG, topB);     glVertex2f(0, HEIGHT);
	    glColor3f(topR, topG, topB);     glVertex2f(WIDTH, HEIGHT);
	    glColor3f(bottomR, bottomG, bottomB); glVertex2f(WIDTH, 0);
	    glColor3f(bottomR, bottomG, bottomB); glVertex2f(0, 0);
	    glEnd();
	
	    // Elementos decorativos según el nivel
	    switch (level.type) {
	        case MISTY_MORNING:
	            // Nubes
	            glEnable(GL_BLEND);
	            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	            for (int i = 0; i < 30; i++) {
	                float cloudX = -cameraX + i * 200 + (CLOUD_SPEED * gameTime);
	                while (cloudX > WIDTH + 1600) cloudX -= 2400;
	                float cloudY = 420 + (i % 5) * 8;
	                if (cloudX > -200 && cloudX < WIDTH + 1800) {
	                    float scale = 0.8f + (i % 3) * 0.2f;
	                    drawCloud(cloudX, cloudY, scale);
	                }
	            }
	            glDisable(GL_BLEND);
	            break;
	            
	        case FOREST_ADVENTURE:
	            // Árboles en el fondo (verdes al principio, luego intermitentes morados)
	            for (int i = 0; i < 15; i++) {
	                float treeX = -cameraX * 0.5f + i * 300;
	                while (treeX > WIDTH + 1000) treeX -= 2400;
	                
	                if (treeX > -300 && treeX < WIDTH + 1800) {
	                    // Tronco
	                    glColor3f(0.4f, 0.3f, 0.2f);
	                    drawRect(treeX, 200, 30, 150, 0.4f, 0.3f, 0.2f);
	                    
	                    // Copa del árbol (verde al principio, luego intermitente morado)
	                    if (!effectActive) {
	                        glColor3f(0.3f, 0.7f, 0.3f);
	                    } else {
	                        if (fmod(animationTime, 1.0f) < 0.5f) {
	                            glColor3f(0.3f, 0.1f, 0.5f);
	                        } else {
	                            glColor3f(0.4f, 0.2f, 0.6f);
	                        }
	                    }
	                    
	                    GLfloat currentColor[4];
	                    glGetFloatv(GL_CURRENT_COLOR, currentColor);
	
	                    drawCircle(treeX + 15, 350, 70, currentColor[0], currentColor[1], currentColor[2]);
	                    drawCircle(treeX - 20, 330, 50, currentColor[0], currentColor[1], currentColor[2]);
	                    drawCircle(treeX + 50, 330, 50, currentColor[0], currentColor[1], currentColor[2]);
	                    drawCircle(treeX + 15, 380, 40, currentColor[0], currentColor[1], currentColor[2]);
	                }
	            }
	            break;
	            
	        case DESERT_JOURNEY:
	    // Fondo con degradado de desierto
	    glBegin(GL_QUADS);
	    glColor3f(0.9f, 0.8f, 0.5f);     glVertex2f(0, HEIGHT);
	    glColor3f(0.8f, 0.7f, 0.4f);     glVertex2f(WIDTH, HEIGHT);
	    glColor3f(1.0f, 0.9f, 0.6f);     glVertex2f(WIDTH, 0);
	    glColor3f(1.0f, 0.8f, 0.5f);     glVertex2f(0, 0);
	    glEnd();
	    
	    // Efecto de tormenta de arena
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    glColor4f(0.9f, 0.8f, 0.4f, 0.3f);
	    glBegin(GL_QUADS);
	    for (int i = 0; i < 10; i++) {
	        float offset = fmod(gameTime * 20 + i * 100, 200) - 100;
	        float height = 50 + i * 30;
	        glVertex2f(-100 + offset, height);
	        glVertex2f(WIDTH + 100 + offset, height);
	        glVertex2f(WIDTH + 100 + offset + 50, height + 50);
	        glVertex2f(-100 + offset + 50, height + 50);
	    }
	    glEnd();
	    glDisable(GL_BLEND);
	    
	    // Montañas en el fondo
	    for (int i = 0; i < 10; i++) {
	        float mountainX = -cameraX * 0.3f + i * 400;
	        while (mountainX > WIDTH + 1000) mountainX -= 2400;
	        
	        if (mountainX > -500 && mountainX < WIDTH + 1800) {
	            glColor3f(0.7f, 0.6f, 0.4f);
	            glBegin(GL_TRIANGLES);
	            glVertex2f(mountainX, 200);
	            glVertex2f(mountainX + 300, 200);
	            glVertex2f(mountainX + 150, 400);
	            glEnd();
	            
	            // Cactus en el fondo
	            drawCactus(mountainX + 50, 200, 100);
	            drawCactus(mountainX + 200, 200, 70);
	        }
	    }
	    break;
	            
	            
	        case NIGHT_FLIGHT:
	            // Estrellas en el cielo
	            glColor3f(1.0f, 1.0f, 1.0f);
	            glBegin(GL_POINTS);
	            for (int i = 0; i < 100; i++) {
	                float starX = fmod(i * 7919.0f, WIDTH);
	                float starY = 150 + fmod(i * 3299.0f, HEIGHT - 200);
	                float brightness = 0.5f + fmod(i * 123.456f, 0.5f);
	                
	                glColor3f(brightness, brightness, brightness);
	                glVertex2f(starX, starY);
	            }
	            glEnd();
	            
	            // Luna
	            glColor3f(0.9f, 0.9f, 0.8f);
	            drawCircle(WIDTH - 100, HEIGHT - 100, 50, 0.9f, 0.9f, 0.8f);
	            break;
	            
	        case SPACE_ODYSSEY:
	    // Estrellas que se mueven (efecto de viaje espacial)
	    glColor3f(1.0f, 1.0f, 1.0f);
	    glBegin(GL_POINTS);
	    for (int i = 0; i < 200; i++) {
	        float starX = fmod(i * 7919.0f + gameTime * 50, WIDTH);
	        float starY = fmod(i * 3299.0f, HEIGHT);
	        float brightness = 0.5f + fmod(i * 123.456f, 0.5f);
	        
	        glColor3f(brightness, brightness, brightness);
	        glVertex2f(starX, starY);
	    }
	    glEnd();
	    
	    // Planeta en el fondo con anillos
	    glColor3f(0.3f, 0.3f, 0.8f);
	    drawCircle(WIDTH - 150, HEIGHT - 150, 80, 0.3f, 0.3f, 0.8f);
	    
	    // Anillos del planeta
	    glColor4f(0.5f, 0.5f, 1.0f, 0.3f);
	    glBegin(GL_LINE_LOOP);
	    for (int i = 0; i < 360; i += 10) {
	        float angle = i * 3.14159265f / 180.0f;
	        glVertex2f(WIDTH - 150 + cos(angle) * 120, 
	                  HEIGHT - 150 + sin(angle) * 60);
	    }
	    glEnd();
	    break;
	            
	            // Planeta en el fondo
	            glColor3f(0.3f, 0.3f, 0.8f);
	            drawCircle(WIDTH - 150, HEIGHT - 150, 80, 0.3f, 0.3f, 0.8f);
	            break;
	    }
	}
	
	
	void drawPlayer() {
    // Efecto de parpadeo si es invencible
    if (player.isInvincible && fmod(player.invincibleTimer, 0.2f) < 0.1f) {
        return;
    }

    glPushMatrix();
    glTranslatef(player.x, player.y, 0);
    
    // Aplicar efecto de aplastamiento solo en la forma normal
    if (player.currentForm == RED_BALL) {
        float squashX = 1.0f + (1.0f - player.squashY) * 0.3f;
        glScalef(squashX, player.squashY, 1.0f);
    }

    switch(player.currentForm) {
        case ROCK_FORM:
            // Dibujar como roca - ahora circular con textura
            // Base gris con gradiente
            glBegin(GL_TRIANGLE_FAN);
            glColor3f(0.5f, 0.5f, 0.5f); // Gris medio en el centro
            glVertex2f(0, 0);
            glColor3f(0.3f, 0.3f, 0.3f); // Gris oscuro en los bordes
            for(int i = 0; i <= 360; i += 15) {
                float angle = i * 3.14159265f / 180.0f;
                glVertex2f(cos(angle) * player.radius, sin(angle) * player.radius);
            }
            glEnd();
            
            // Textura de roca (puntos aleatorios)
            glColor3f(0.2f, 0.2f, 0.2f);
            glPointSize(2.0f);
            glBegin(GL_POINTS);
            for(int i = 0; i < 30; i++) {
                float angle = rand() % 360 * 3.14159265f / 180.0f;
                float r = player.radius * (0.3f + 0.7f * (rand() % 100) / 100.0f);
                glVertex2f(cos(angle) * r, sin(angle) * r);
            }
            glEnd();
            break;
            
        case BEACH_BALL_FORM:
            // Dibujar como bola de playa con diseño de franjas
            // Base blanca
            glColor3f(1.0f, 1.0f, 1.0f);
            drawCircle(0, 0, player.radius, 1.0f, 1.0f, 1.0f);
            
            // Franjas de colores brillantes
            glBegin(GL_TRIANGLE_STRIP);
            for(int i = 0; i <= 12; i++) {
                float angle1 = i * 3.14159265f / 6.0f;
                float angle2 = (i+1) * 3.14159265f / 6.0f;
                
                // Alternar colores vivos
                if(i % 3 == 0) glColor3f(1.0f, 0.2f, 0.2f); // Rojo
                else if(i % 3 == 1) glColor3f(0.2f, 0.6f, 1.0f); // Azul
                else glColor3f(1.0f, 0.8f, 0.2f); // Amarillo
                
                glVertex2f(cos(angle1) * player.radius, sin(angle1) * player.radius);
                glVertex2f(cos(angle1) * player.radius*0.7f, sin(angle1) * player.radius*0.7f);
                
                glVertex2f(cos(angle2) * player.radius, sin(angle2) * player.radius);
                glVertex2f(cos(angle2) * player.radius*0.7f, sin(angle2) * player.radius*0.7f);
            }
            glEnd();
            
            // Líneas divisorias entre franjas
            glColor3f(0.1f, 0.1f, 0.1f);
            glLineWidth(1.5f);
            glBegin(GL_LINES);
            for(int i = 0; i < 6; i++) {
                float angle = i * 3.14159265f / 3.0f;
                glVertex2f(cos(angle) * player.radius*0.65f, sin(angle) * player.radius*0.65f);
                glVertex2f(cos(angle) * player.radius, sin(angle) * player.radius);
            }
            glEnd();
            break;
            
        case RED_BALL:
        default:
            // Dibujar como bola roja con reflejos más realistas
            // Base roja con gradiente
            glBegin(GL_TRIANGLE_FAN);
            glColor3f(1.0f, 0.2f, 0.2f); // Rojo brillante en el centro
            glVertex2f(0, 0);
            glColor3f(0.8f, 0.0f, 0.0f); // Rojo oscuro en los bordes
            for(int i = 0; i <= 360; i += 15) {
                float angle = i * 3.14159265f / 180.0f;
                glVertex2f(cos(angle) * player.radius, sin(angle) * player.radius);
            }
            glEnd();
            
            // Reflejo de luz más pronunciado
            glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
            drawCircle(-player.radius*0.4f, player.radius*0.4f, player.radius*0.3f, 1.0f, 1.0f, 1.0f);
            break;
    }
    
    // Ojos y boca (comunes a todas las formas pero con variaciones)
    switch(player.currentForm) {
        case ROCK_FORM:
            // Ojos más pequeños y serios para la roca
            glColor3f(1.0f, 1.0f, 1.0f);
            drawCircle(-player.radius * 0.25f, player.radius * 0.15f, player.radius * 0.1f, 1.0f, 1.0f, 1.0f);
            drawCircle(player.radius * 0.25f, player.radius * 0.15f, player.radius * 0.1f, 1.0f, 1.0f, 1.0f);
            
            glColor3f(0.0f, 0.0f, 0.0f);
            drawCircle(-player.radius * 0.25f, player.radius * 0.15f, player.radius * 0.05f, 0.0f, 0.0f, 0.0f);
            drawCircle(player.radius * 0.25f, player.radius * 0.15f, player.radius * 0.05f, 0.0f, 0.0f, 0.0f);
            
            // Boca recta (seria)
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_LINES);
            glVertex2f(-player.radius*0.2f, -player.radius*0.2f);
            glVertex2f(player.radius*0.2f, -player.radius*0.2f);
            glEnd();
            break;
            
        case BEACH_BALL_FORM:
            // Ojos grandes y alegres
            glColor3f(1.0f, 1.0f, 1.0f);
            drawCircle(-player.radius * 0.3f, player.radius * 0.2f, player.radius * 0.15f, 1.0f, 1.0f, 1.0f);
            drawCircle(player.radius * 0.3f, player.radius * 0.2f, player.radius * 0.15f, 1.0f, 1.0f, 1.0f);
            
            glColor3f(0.0f, 0.0f, 0.0f);
            drawCircle(-player.radius * 0.3f, player.radius * 0.2f, player.radius * 0.07f, 0.0f, 0.0f, 0.0f);
            drawCircle(player.radius * 0.3f, player.radius * 0.2f, player.radius * 0.07f, 0.0f, 0.0f, 0.0f);
            
            // Boca sonriente
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i <= 10; i++) {
                float t = (float)i / 10.0f;
                float angle = 3.14159265f * 0.7f + t * 3.14159265f * 0.6f;
                glVertex2f(cos(angle) * player.radius * 0.5f, 
                          sin(angle) * player.radius * 0.3f - player.radius * 0.1f);
            }
            glEnd();
            break;
            
        case RED_BALL:
        default:
            // Ojos normales
            glColor3f(1.0f, 1.0f, 1.0f);
            drawCircle(-player.radius * 0.3f, player.radius * 0.2f, player.radius * 0.15f, 1.0f, 1.0f, 1.0f);
            drawCircle(player.radius * 0.3f, player.radius * 0.2f, player.radius * 0.15f, 1.0f, 1.0f, 1.0f);
            
            glColor3f(0.0f, 0.0f, 0.0f);
            drawCircle(-player.radius * 0.3f, player.radius * 0.2f, player.radius * 0.07f, 0.0f, 0.0f, 0.0f);
            drawCircle(player.radius * 0.3f, player.radius * 0.2f, player.radius * 0.07f, 0.0f, 0.0f, 0.0f);
            
            // Boca sonriente normal
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i <= 10; i++) {
                float t = (float)i / 10.0f;
                float angle = 3.14159265f * 0.7f + t * 3.14159265f * 0.6f;
                glVertex2f(cos(angle) * player.radius * 0.6f, 
                          sin(angle) * player.radius * 0.6f - player.radius * 0.1f);
            }
            glEnd();
    }
    
    glPopMatrix();
    
    // Dibujar barra de salud del jugador
    float healthRatio = player.health / PLAYER_MAX_HEALTH;
    glColor3f(1.0f - healthRatio, healthRatio, 0.0f);
    drawRect(player.x - player.radius, player.y + player.radius + 10, 
             player.radius * 2 * healthRatio, 5, 
             1.0f - healthRatio, healthRatio, 0.0f);
    
    // Dibujar indicador de forma actual si no es la forma normal
    string formText;
    switch(player.currentForm) {
        case ROCK_FORM: 
            formText = "Bummpy Crocks";
            break;
        case BEACH_BALL_FORM:
            formText = "Wolly";
            break;
        case RED_BALL:
        default:
            formText = "Bounse"; // Nombre para la forma básica
            break;
    }
        
        glColor3f(1.0f, 1.0f, 0.0f);
        glRasterPos2f(player.x - 40, player.y + player.radius + 30);
        for(size_t i = 0; i < formText.size(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, formText[i]);
        }
        
        
    }

	
	void drawGameObjects() {
	    for (size_t i = 0; i < gameObjects.size(); i++) {
	        const GameObject& obj = gameObjects[i];
	        
	        if (!obj.collected) {
	            switch (obj.type) {
	                case COLLECTIBLE:
	                    drawCoin(obj.x + obj.width/2, obj.y + obj.height/2, obj.width/2);
	                    break;
	                case POWER_UP:
	                    drawStar(obj.x + obj.width/2, obj.y + obj.height/2, obj.width/2);
	                    break;
	                case ENEMY:
	                    drawEnemy(obj.x, obj.y, obj.width, obj.height, obj.animationTime, levels[currentLevel].type, false);
	                    break;
	                case FINAL_OBJECT:
	                    // Dibujar la línea de meta en lugar del objeto final
	                    drawFinishLine(obj.x, obj.y);
	                    break;
	            }
	        }
	    }
	}
	
	void drawEnemies() {
	    for (size_t i = 0; i < enemies.size(); i++) {
	        const Enemy& enemy = enemies[i];
	        if (enemy.isActive) {
	            drawEnemy(enemy.x, enemy.y, enemy.width, enemy.height, enemy.animationTime, levels[currentLevel].type, enemy.isSquare);
	        }
	    }
	}
	
	void drawProjectiles() {
	    for (size_t i = 0; i < projectiles.size(); i++) {
	        const Projectile& proj = projectiles[i];
	        if (proj.isActive) {
	            drawProjectile(proj.x, proj.y);
	        }
	    }
	}
	
	void drawUI() {
	    // Dibujar información de juego (puntuación, vidas, tiempo)
	    glColor3f(1.0f, 1.0f, 1.0f);
	    glRasterPos2f(20, HEIGHT - 30);
	    
	    string scoreText = "Puntaje: " + intToString(score);
	    for (size_t i = 0; i < scoreText.size(); i++) {
	        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, scoreText[i]);
	    }
	    
	    glRasterPos2f(20, HEIGHT - 60);
	    string livesText = "Vidas: " + intToString(lives);
	    for (size_t i = 0; i < livesText.size(); i++) {
	        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, livesText[i]);
	    }
	    
	    glRasterPos2f(WIDTH - 200, HEIGHT - 30);
	    string timeText = "Tiempo: " + timeToString(gameTime);
	    for (size_t i = 0; i < timeText.size(); i++) {
	        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, timeText[i]);
	    }
	    
	    // Mostrar objetivo del nivel
	    glRasterPos2f(WIDTH/2 - 100, HEIGHT - 30);
	    string targetText = "Objetivo: " + intToString(levels[currentLevel].targetScore);
	    for (size_t i = 0; i < targetText.size(); i++) {
	        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, targetText[i]);
	    }
	    
	    // Mostrar salud del jugador si hay enemigos
	    if (levels[currentLevel].hasEnemies) {
	        glRasterPos2f(20, HEIGHT - 90);
	        string healthText = "Salud: " + intToString((int)player.health);
	        for (size_t i = 0; i < healthText.size(); i++) {
	            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, healthText[i]);
	        }
	    }
	}
	
	void drawTextScreen() {
	    glClear(GL_COLOR_BUFFER_BIT);
	    
	    // Dibujar fondo del nivel actual
	    drawStaticBackground();
	    
	    // Dibujar texto centrado
	    glColor3f(1.0f, 1.0f, 1.0f);
	    glRasterPos2f(WIDTH/2 - 150, HEIGHT/2);
	    
	    string currentText = introTexts[currentTextIndex];
	    for (size_t i = 0; i < currentText.size(); i++) {
	        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, currentText[i]);
	    }
	    
	    // Dibujar indicador para continuar (parpadeante)
	    if (fmod(textDisplayTimer, 1.0f) < 0.5f) {
	        glColor3f(1.0f, 1.0f, 0.0f);
	        glRasterPos2f(WIDTH/2 - 50, HEIGHT/2 - 50);
	        string continueText = "Presiona ESPACIO";
	        for (size_t i = 0; i < continueText.size(); i++) {
	            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, continueText[i]);
	        }
	    }
	}

	
	void drawMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Dibujar fondo azul con nubes
    drawStaticBackground();
    
   
    // Título del juego (opcional, puedes quitarlo si el logo ya lo incluye)
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(WIDTH/2 - 60, HEIGHT - 100);
    string title = "YULS TALES";
    for (size_t i = 0; i < title.size(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, title[i]);
    }
    
    // Opciones del menú como en la imagen
    string menuItems[] = {"Nuevo Juego", "Seleccionar Nivel", "Instrucciones", "Historia", "Acerca de", "Salir"};
    for (int i = 0; i < 6; i++) {
        if (i == selectedMenuItem) {
            glColor3f(1.0f, 1.0f, 0.0f);
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        glRasterPos2f(WIDTH / 2 - 70, HEIGHT / 2 - i * 40);
        for (size_t j = 0; j < menuItems[i].size(); j++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, menuItems[i][j]);
        }
    }
}
	
	
	
	void drawPauseMenu() {
		
	    // Configurar vista
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluOrtho2D(0, WIDTH, 0, HEIGHT);
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	    
	    // 1. Dibujar fondo del nivel primero
	    drawStaticBackground();
	    
	    // 2. Dibujar mundo del juego (congelado)
	    glPushMatrix();
	    glTranslatef(-cameraX, 0, 0);
	    drawPlatforms();
	    drawGameObjects();
	    if (levels[currentLevel].hasEnemies) {
	        drawEnemies();
	    }
	    drawProjectiles();
	    drawPlayer();
	    drawConfetti();
	    glPopMatrix();
	    
	    // 3. Dibujar UI del juego
	    drawUI();
	    
	    // 4. Dibujar overlay semitransparente oscuro (directo, sin drawRect)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.4f); // 40% de opacidad, ajusta si quieres
	
	glBegin(GL_QUADS);
	    glVertex2f(0, 0);
	    glVertex2f(WIDTH, 0);
	    glVertex2f(WIDTH, HEIGHT);
	    glVertex2f(0, HEIGHT);
	glEnd();
	
	glDisable(GL_BLEND);
	    
	    float menuWidth = 400;
	    float menuHeight = 300;
	    float menuX = (WIDTH - menuWidth) / 2;
	    float menuY = (HEIGHT - menuHeight) / 2;
	    
	    // Marco del menú
	    glColor3f(0.1f, 0.1f, 0.3f);
	    drawRect(menuX, menuY, menuWidth, menuHeight, 0.1f, 0.1f, 0.3f);
	    
	    // Borde del menú
	    glColor3f(0.3f, 0.3f, 0.6f);
	    glLineWidth(2.0f);
	    glBegin(GL_LINE_LOOP);
	    glVertex2f(menuX, menuY);
	    glVertex2f(menuX + menuWidth, menuY);
	    glVertex2f(menuX + menuWidth, menuY + menuHeight);
	    glVertex2f(menuX, menuY + menuHeight);
	    glEnd();
	    glLineWidth(1.0f);
	    
	    // Título "PAUSA"
	    glColor3f(1.0f, 1.0f, 1.0f);
	    glRasterPos2f(menuX + menuWidth/2 - 40, menuY + menuHeight - 40);
	    string pauseText = "PAUSA";
	    for (size_t i = 0; i < pauseText.size(); i++) {
	        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, pauseText[i]);
	    }
	    
	    // Opciones del menú
	    string menuItems[] = {"CONTINUAR", "REINICIAR", "SALIR"};
	    for (int i = 0; i < 3; i++) {
	        if (i == selectedMenuItem) {
	            glColor3f(1.0f, 1.0f, 0.0f); // Amarillo para la opción seleccionada
	        } else {
	            glColor3f(1.0f, 1.0f, 1.0f); // Blanco para las otras opciones
	        }
	        
	        glRasterPos2f(menuX + menuWidth/2 - 50, menuY + menuHeight - 100 - i * 40);
	        for (size_t j = 0; j < menuItems[i].size(); j++) {
	            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, menuItems[i][j]);
	        }
	    }
	    
	}
	
	
	// ==== FUNCIONES DE TEXTO (fuera de cualquier otra) ====
	void renderTextStroke(float x, float y, string text, float scale = 0.3f) {
	    glPushMatrix();
	    glTranslatef(x, y, 0);
	    glScalef(scale, scale, scale);
	    for (size_t i = 0; i < text.size(); ++i) {
	        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
	    }
	    glPopMatrix();
	}
	
	void renderTextWithShadow(float x, float y, string text, float scale = 0.3f) {
	    glColor3f(0.0f, 0.0f, 0.0f); // Sombra
	    renderTextStroke(x + 3, y - 3, text, scale);
	    
	    glColor3f(1.0f, 1.0f, 0.0f); // Texto principal
	    renderTextStroke(x, y, text, scale);
	}
	
	// ===== ui_theme.h (o arriba de tus draws) =====
struct Color { float r,g,b,a; };
static inline Color RGBA(unsigned r, unsigned g, unsigned b, float a=1.0f){
    return Color{ r/255.0f, g/255.0f, b/255.0f, a };
}
static inline void glColor(const Color& c){ glColor4f(c.r,c.g,c.b,c.a); }

struct UITheme {
    Color bgTop, bgBottom;         // gradiente fondo
    Color panelBg, panelBorder;    // panel
    Color text, textMuted;         // textos
    Color titleTop, titleBottom;   // gradiente de título/píldora
    Color accent;                  // color de selección/llamado
};



static UITheme THEME = {
    // Por defecto (nocturno violeta)
    /*bgTop*/      RGBA(17, 12, 44),
    /*bgBottom*/   RGBA(46, 28, 93),
    /*panelBg*/    RGBA(25, 33, 71, 0.92f),
    /*panelBorder*/RGBA(120,130,200, 0.35f),
    /*text*/       RGBA(234,242,255),
    /*textMuted*/  RGBA(160,175,210),
    /*titleTop*/   RGBA(255,166,77),
    /*titleBottom*/RGBA(210, 72,52),
    /*accent*/     RGBA(255,201,77)  // ámbar suave
};

// Temas por mundo (opcional)
static UITheme SKY_DAY()   { UITheme t=THEME; t.bgTop=RGBA(8,11,37); t.bgBottom=RGBA(78,40,146); return t; }
static UITheme FOREST()    { UITheme t=THEME; t.bgTop=RGBA(11,34,28); t.bgBottom=RGBA(20,73,59); t.panelBg=RGBA(18,46,44,0.92f); return t; }
static UITheme DESERT()    { UITheme t=THEME; t.bgTop=RGBA(56,35,10); t.bgBottom=RGBA(116,78,22); t.panelBg=RGBA(52,40,22,0.92f); t.titleTop=RGBA(255,204,102); t.titleBottom=RGBA(224,142,62); return t; }
static UITheme NIGHT()     { return THEME; }
static UITheme SPACE()     { UITheme t=THEME; t.bgTop=RGBA(10,14,44); t.bgBottom=RGBA(74,56,160); t.titleTop=RGBA(144,128,255); t.titleBottom=RGBA(84,72,200); t.accent=RGBA(144,200,255); return t; }

static inline void drawGradientRect(float x,float y,float w,float h,const Color& top,const Color& bottom){
    glBegin(GL_QUADS);
    glColor(top);    glVertex2f(x, y+h); glVertex2f(x+w, y+h);
    glColor(bottom); glVertex2f(x+w, y); glVertex2f(x, y);
    glEnd();
}

static inline void drawPanel(float x,float y,float w,float h,const UITheme& T){
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor(T.panelBg);
    glBegin(GL_QUADS);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
    glLineWidth(2.0f); glColor(T.panelBorder);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
    glDisable(GL_BLEND);
}

static inline void drawTitlePill(float x,float y,float w,float h,const UITheme& T){
    drawGradientRect(x,y,w,h, T.titleTop, T.titleBottom);
}

static inline void drawSelectionOutline(float x,float y,float w,float h,const UITheme& T,float timeSec){
    float pulse = 0.55f + 0.45f * sinf(timeSec*5.0f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // halo
    glColor( Color{T.accent.r, T.accent.g, T.accent.b, 0.18f + 0.12f*pulse} );
    glBegin(GL_QUADS);
    glVertex2f(x-3,y-3); glVertex2f(x+w+3,y-3); glVertex2f(x+w+3,y+h+3); glVertex2f(x-3,y+h+3);
    glEnd();
    // borde
    glLineWidth(2.0f + 2.0f*pulse); glColor( Color{T.accent.r, T.accent.g, T.accent.b, 0.95f} );
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
    glDisable(GL_BLEND);
}


	
	void drawLevelSelect() {
    const UITheme& T = NIGHT();          // cambia a SKY_DAY()/FOREST()/DESERT()/SPACE() si quieres

    // Fondo + gradiente
    glClearColor(0,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
    drawGradientRect(0,0, WIDTH, HEIGHT, T.bgTop, T.bgBottom);

    // Estrellas sutiles (opcional)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 120; i++) glVertex2f(rand()%WIDTH, rand()%HEIGHT);
    glEnd();

    // Título tipo píldora
    float titleW=360, titleH=44, titleX=(WIDTH-titleW)/2, titleY=HEIGHT-90;
    drawTitlePill(titleX, titleY, titleW, titleH, T);
    glColor(T.text);
    // Si no tienes centrado, deja un offset aproximado:
    glRasterPos2f(titleX + 22, titleY + 28);
    const std::string title = "SELECCION DE NIVEL";
    for(char c: title) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);

    // Panel contenedor
    float px=140, py=100, pw=520, ph=360;
    drawPanel(px,py,pw,ph,T);

    // Lista
    float rowH=62, rx=px+26, ry=py+ph-24-rowH;

    for (size_t i=0; i<levels.size(); ++i, ry-=rowH) {
        // Resaltado de selección
        if ((int)i == selectedMenuItem) {
            drawSelectionOutline(rx-10, ry-8, pw-32, rowH-8, T, animationTime);
        }

        // Icono (estrella si unlocked, candado si no)
        if (levels[i].unlocked) {
            glColor(T.accent);
            drawStar(rx-2, ry+18, 14);
        } else {
            glColor(Color{0.6f,0.6f,0.6f,1.0f});
            drawRect(rx-10, ry+6, 18, 18, 0.6f,0.6f,0.6f);
            glColor(Color{0.35f,0.35f,0.35f,1.0f});
            drawRect(rx-5, ry+22, 8, 5, 0.35f,0.35f,0.35f);
        }

        // Texto principal / secundario
        glColor((int)i==selectedMenuItem ? T.text : (levels[i].unlocked ? T.text : T.textMuted));
        glRasterPos2f(rx+28, ry+30);
        for(char c: levels[i].name) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

        glColor(levels[i].unlocked ? Color{T.text.r,T.text.g,T.text.b,0.75f} : T.textMuted);
        glRasterPos2f(rx+36, ry+12);
        for(char c: levels[i].subtitle) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);

        // Etiqueta bloqueado (opcional)
        if (!levels[i].unlocked) {
            glColor(Color{0.85f,0.35f,0.35f,1.0f});
            const std::string lockedText = "(BLOQUEADO)";
            glRasterPos2f(px+pw-160, ry+18);
            for(char c: lockedText) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }
    }

    // Hint inferior
    glColor(T.textMuted);
    const std::string hint = "Usa ARRIBA/ABAJO para NAVEGAR, ENTER para JUGAR, ESC para VOLVER";
    glRasterPos2f((WIDTH - 700)/2.0f, 42);
    for(char c: hint) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}
	
	void drawInstructions() {
    const UITheme& T = NIGHT();

    glClearColor(0,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
    drawGradientRect(0,0, WIDTH, HEIGHT, T.bgTop, T.bgBottom);

    // Título
    float titleW=300, titleH=42, titleX=(WIDTH-titleW)/2, titleY=HEIGHT-90;
    drawTitlePill(titleX, titleY, titleW, titleH, T);
    glColor(T.text);
    glRasterPos2f(titleX + 36, titleY + 28);
    const std::string t = "CONTROLES DEL JUEGO";
    for(char c: t) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);

    // Panel
    float px=60, py=60, pw=WIDTH-120, ph=HEIGHT-160;
    drawPanel(px,py,pw,ph,T);

    // Texto con scroll
    int maxLines = (int)((ph-48) / 28);                 // altura de línea ~28px
    int startLine = instructionScroll;
    int endLine   = std::min(startLine + maxLines, (int)instructionTexts.size());

    float x = px + 24;
    float y = py + ph - 36;

    for (int i=startLine; i<endLine; ++i) {
        glColor(T.text);
        glRasterPos2f(x, y - (i-startLine)*28);
        for(char c: instructionTexts[i]) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

    // Scrollbar ámbar
    if ((int)instructionTexts.size() > maxLines) {
        float trackH = ph-24;
        float barH = (float)maxLines / (float)instructionTexts.size() * trackH;
        float barY = py+12 + (float)instructionScroll / (float)(instructionTexts.size()-maxLines) * (trackH - barH);
        glColor(T.accent);
        glBegin(GL_QUADS);
        glVertex2f(px+pw-16, barY);
        glVertex2f(px+pw- 4, barY);
        glVertex2f(px+pw- 4, barY+barH);
        glVertex2f(px+pw-16, barY+barH);
        glEnd();
    }

    // Hint
    glColor(T.textMuted);
    const std::string hint = "Usa ARRIBA/ABAJO para MOVER, ESC para volver";
    glRasterPos2f((WIDTH - 420)/2.0f, 36);
    for(char c: hint) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}
	
	void drawStory() {
    const UITheme& T = NIGHT();

    glClearColor(0,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
    drawGradientRect(0,0, WIDTH, HEIGHT, T.bgTop, T.bgBottom);

    // Estrellas sutiles
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i=0; i<150; ++i) glVertex2f(rand()%WIDTH, rand()%HEIGHT);
    glEnd();

    // Título
    float titleW=360, titleH=44, titleX=(WIDTH-titleW)/2, titleY=HEIGHT-90;
    drawTitlePill(titleX, titleY, titleW, titleH, T);
    glColor(T.text);
    glRasterPos2f(titleX + 26, titleY + 28);
    const std::string title = "Historia de Yuls Tales";
    for(char c: title) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);

    // Panel del cuerpo
    float px=48, py=56, pw=WIDTH-96, ph=HEIGHT-160;
    drawPanel(px,py,pw,ph,T);

    // Texto de la historia
    float x = px + 24;
    float y = py + ph - 36;
    for (size_t i=0; i<storyTexts.size(); ++i) {
        glColor(T.text);
        glRasterPos2f(x, y - (float)i*28.0f);
        for(char c: storyTexts[i]) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

    // Hint
    glColor(T.textMuted);
    const std::string backText = "Presiona ESC para volver al menu";
    glRasterPos2f((WIDTH - 350)/2.0f, 36);
    for(char c: backText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}
	
	
	void drawGameOverScreen() {
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	    // Fondo oscuro translúcido
	    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);  // 60% oscuro
	    glBegin(GL_QUADS);
	        glVertex2f(0, 0);
	        glVertex2f(WIDTH, 0);
	        glVertex2f(WIDTH, HEIGHT);
	        glVertex2f(0, HEIGHT);
	    glEnd();
	
	    // Cuadro central tipo panel
	    float panelWidth = 500;
	    float panelHeight = 300;
	    float panelX = (WIDTH - panelWidth) / 2;
	    float panelY = (HEIGHT - panelHeight) / 2;
	
	    // Fondo del panel (azul oscuro translúcido)
	    glColor4f(0.1f, 0.1f, 0.2f, 0.8f);
	    glBegin(GL_QUADS);
	        glVertex2f(panelX, panelY);
	        glVertex2f(panelX + panelWidth, panelY);
	        glVertex2f(panelX + panelWidth, panelY + panelHeight);
	        glVertex2f(panelX, panelY + panelHeight);
	    glEnd();
	
	    // Borde del panel
	    glColor3f(0.3f, 0.6f, 1.0f);
	    glLineWidth(3.0f);
	    glBegin(GL_LINE_LOOP);
	        glVertex2f(panelX, panelY);
	        glVertex2f(panelX + panelWidth, panelY);
	        glVertex2f(panelX + panelWidth, panelY + panelHeight);
	        glVertex2f(panelX, panelY + panelHeight);
	    glEnd();
	    glLineWidth(1.0f);
	
	    // Título: ¡HAS PERDIDO!
	    glColor3f(1.0f, 0.2f, 0.2f);
	    renderTextCentered(WIDTH / 2, panelY + panelHeight - 60, "¡HAS PERDIDO!", 35.0f);
	
	    // Subtítulo
	    glColor3f(1.0f, 1.0f, 1.0f);
	    renderTextCentered(WIDTH / 2, panelY + panelHeight - 110, "¿Deseas jugar de nuevo?", 15.0f);
	
	    // Opciones
	    string opcion1 = (selectedOption == 0) ? "> SI <" : "  SI";
	    string opcion2 = (selectedOption == 1) ? "> NO <" : "  NO";
	
	    renderTextCentered(WIDTH / 2, panelY + panelHeight - 160, opcion1, 7.4f);
	    renderTextCentered(WIDTH / 2, panelY + panelHeight - 210, opcion2, 7.4f);
	
	    glDisable(GL_BLEND);
	}
	
	
	
	// Helpers compatibles con C++98
    static int bitmapTextWidth(void* font, const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(s[i]);
        w += glutBitmapWidth(font, (int)ch);
    }
    return w;
}
    static void drawBitmapString(void* font, const std::string& s) {
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(s[i]);
        glutBitmapCharacter(font, (int)ch);
    }
}

void drawLevelComplete() {
    // Fondo semitransparente oscuro
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();
    glDisable(GL_BLEND);

    // Marco del cuadro
    float boxWidth = 500;
    float boxHeight = 400;
    float boxX = (WIDTH - boxWidth) / 2.0f;
    float boxY = (HEIGHT - boxHeight) / 2.0f;

    // Marco exterior
    glColor3f(0.8f, 0.8f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(boxX - 5, boxY - 5);
    glVertex2f(boxX + boxWidth + 5, boxY - 5);
    glVertex2f(boxX + boxWidth + 5, boxY + boxHeight + 5);
    glVertex2f(boxX - 5, boxY + boxHeight + 5);
    glEnd();

    // Marco principal
    glColor3f(0.2f, 0.2f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(boxX, boxY);
    glVertex2f(boxX + boxWidth, boxY);
    glVertex2f(boxX + boxWidth, boxY + boxHeight);
    glVertex2f(boxX, boxY + boxHeight);
    glEnd();

    // Borde interior
    glColor3f(0.4f, 0.4f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(boxX, boxY);
    glVertex2f(boxX + boxWidth, boxY);
    glVertex2f(boxX + boxWidth, boxY + boxHeight);
    glVertex2f(boxX, boxY + boxHeight);
    glEnd();
    glLineWidth(1.0f);

    // Título
    glColor3f(1.0f, 1.0f, 0.0f);
    std::string titulo = "NIVEL COMPLETADO!";
    int tituloWidth = bitmapTextWidth(GLUT_BITMAP_TIMES_ROMAN_24, titulo);
    glRasterPos2f(WIDTH/2.0f - tituloWidth/2.0f, boxY + boxHeight - 50);
    drawBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, titulo);

    // Estrellas
    float starSize = 40.0f;
    float starY = boxY + boxHeight - 120.0f;
    drawStar(WIDTH / 2.0f - starSize * 2.0f, starY, starSize);
    if (score >= levels[currentLevel].targetScore * 0.5f) {
        drawStar(WIDTH / 2.0f, starY, starSize);
    }
    if (score >= levels[currentLevel].targetScore) {
        drawStar(WIDTH / 2.0f + starSize * 2.0f, starY, starSize);
    }

    // Nombre del nivel
    glColor3f(1.0f, 1.0f, 1.0f);
    std::string nivelText = levels[currentLevel].name + ": " + levels[currentLevel].subtitle;
    int nivelWidth = bitmapTextWidth(GLUT_BITMAP_HELVETICA_18, nivelText);
    glRasterPos2f(WIDTH/2.0f - nivelWidth/2.0f, boxY + boxHeight - 180.0f);
    drawBitmapString(GLUT_BITMAP_HELVETICA_18, nivelText);

    // Puntuación
    std::string scoreText = std::string("Puntuacion: ") + intToString(score);
    int scoreWidth = bitmapTextWidth(GLUT_BITMAP_HELVETICA_12, scoreText);
    glRasterPos2f(WIDTH/2.0f - scoreWidth/2.0f, boxY + boxHeight - 230.0f);
    drawBitmapString(GLUT_BITMAP_HELVETICA_12, scoreText);

    // Tiempo
    std::string timeText = std::string("Tiempo: ") + timeToString(gameTime);
    int timeWidth = bitmapTextWidth(GLUT_BITMAP_HELVETICA_12, timeText);
    glRasterPos2f(WIDTH/2.0f - timeWidth/2.0f, boxY + boxHeight - 270.0f);
    drawBitmapString(GLUT_BITMAP_HELVETICA_12, timeText);

    // Nivel desbloqueado
    if (score >= levels[currentLevel].targetScore * 0.5f && currentLevel + 1 < (int)levels.size()) {
        glColor3f(0.5f, 1.0f, 0.5f);
        std::string unlockText = std::string("Nivel ") + levels[currentLevel + 1].name + " desbloqueado!";
        int unlockWidth = bitmapTextWidth(GLUT_BITMAP_HELVETICA_18, unlockText);
        glRasterPos2f(WIDTH/2.0f - unlockWidth/2.0f, boxY + boxHeight - 320.0f);
        drawBitmapString(GLUT_BITMAP_HELVETICA_18, unlockText);
    }

    // Mensajes parpadeantes
    if (fmod(animationTime, 1.0f) < 0.5f) {
        glColor3f(1.0f, 1.0f, 0.0f);
        std::string mensajePrincipal =
            (currentLevel + 1 < (int)levels.size() && score >= levels[currentLevel].targetScore * 0.5f)
            ? "Presiona ENTER para el siguiente nivel"
            : "Presiona ENTER para continuar";
        int msgWidth = bitmapTextWidth(GLUT_BITMAP_9_BY_15, mensajePrincipal);
        glRasterPos2f(WIDTH/2.0f - msgWidth/2.0f, boxY + 50.0f);
        drawBitmapString(GLUT_BITMAP_9_BY_15, mensajePrincipal);

        glColor3f(1.0f, 0.5f, 0.5f);
        std::string mensajeSecundario = "Presiona ESC para volver al menu";
        int secMsgWidth = bitmapTextWidth(GLUT_BITMAP_9_BY_15, mensajeSecundario);
        glRasterPos2f(WIDTH/2.0f - secMsgWidth/2.0f, boxY + 20.0f);
        drawBitmapString(GLUT_BITMAP_9_BY_15, mensajeSecundario);
    }
}
	
	
	
	
	// CAMBIAR A FALSE DEL MUNDO 2 EN ADELANTE 
	void initializeLevels() {
	    levels.clear();
	    levels.push_back(Level("Capitulo 1", "Manana Nublada", MISTY_MORNING, 100, 0.5f, 0.8f, 1.0f, true, false, false));
	    levels.push_back(Level("Capitulo 2", "Aventura en el Bosque", FOREST_ADVENTURE, 200, 0.2f, 0.8f, 0.3f, false, false, true));
	    levels.push_back(Level("Capitulo 3", "Viaje al Desierto", DESERT_JOURNEY, 300, 1.0f, 0.8f, 0.3f, false, true, false));
	    levels.push_back(Level("Capitulo 4", "Vuelo Nocturno", NIGHT_FLIGHT, 400, 0.2f, 0.2f, 0.6f, false, true, false));
	    levels.push_back(Level("Capitulo 5", "Odisea Espacial", SPACE_ODYSSEY, 500, 0.4f, 0.2f, 0.8f, false, true, false));
	}
	
	void initializeIntroTexts() {
	    introTexts.clear();
	    switch (currentLevel) {
	        case 0:
	            introTexts.push_back("Una manana nublada en Sky Bean");
	            introTexts.push_back("Land algo no marchaba bien.");
	            introTexts.push_back("Parecia que los colores se estaban desvaneciendo.");
	            introTexts.push_back("Bounce empezo a andar,");
	            introTexts.push_back("preguntandose donde estaban los pajaros.");
	            break;
	        case 1:
	            introTexts.push_back("Bounce llega al Bosque Encantado,");
	            introTexts.push_back("donde los arboles han perdido sus colores.");
	            introTexts.push_back("Debe recolectar las monedas doradas");
	            introTexts.push_back("para restaurar la vida del bosque.");
	            break;
	        case 2:
	            introTexts.push_back("El Desierto Ardiente es el siguiente reto.");
	            introTexts.push_back("Cuidado con las criaturas de arena");
	            introTexts.push_back("que protegen las monedas doradas.");
	            introTexts.push_back("Salta sobre ellas o disparales para derrotarlas!");
	            break;
	        case 3:
	            introTexts.push_back("La Noche Eterna se cierne sobre Bounce.");
	            introTexts.push_back("Enemigos oscuros merodean por las sombras.");
	            introTexts.push_back("Usa tu agilidad y tus disparos para esquivarlos");
	            introTexts.push_back("y recupera las monedas de luz.");
	            break;
	        case 4:
	            introTexts.push_back("El espacio exterior es el ultimo desafio.");
	            introTexts.push_back("Enfrentate a los aliens que han robado");
	            introTexts.push_back("los colores del universo.");
	            introTexts.push_back("Disparales y recupera todo!");
	            break;
	    }
	    
	    currentTextIndex = 0;
	    textDisplayTimer = 0.0f;
	    currentState = SHOWING_TEXT;
	}
	
	void initializePlatforms() {
	    platforms.clear();
	    
	    // Altura base para todos los niveles (más centrado)
	    const float baseHeight = HEIGHT / 2 - 50;
	    
	    switch (levels[currentLevel].type) {
	        case MISTY_MORNING:
	            // Plataforma inicial (recta)
	            platforms.push_back(Platform(0, baseHeight, 200, 30));
	            
	            // Plataforma recta normal (reemplazo del tronco)
	            platforms.push_back(Platform(250, baseHeight, 150, 30));
	            
	            // Plataforma recta
	            platforms.push_back(Platform(450, baseHeight, 150, 30));
	            
	            // Plataforma nueva
	            platforms.push_back(Platform(2400, baseHeight - 50, 150, 30));
	            
	            // Plataforma con espinas
	            platforms.push_back(Platform(650, baseHeight, 120, 25, false, false, 0, true));
	            
	            // Plataforma recta flotante segura (para teletransporte)
	            platforms.push_back(Platform(800, baseHeight - 50, 120, 25, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Plataforma recta flotante con espinas
	            platforms.push_back(Platform(950, baseHeight - 50, 120, 25, false, false, 0, true));
	            
	            // Gran plataforma final recta segura (para teletransporte)
	            platforms.push_back(Platform(1100, baseHeight - 30, 300, 40, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Plataforma vertical final (como una torre)
	            platforms.push_back(Platform(1400, baseHeight - 30, 50, 200));
	            
	            // Plataformas adicionales seguras
	            platforms.push_back(Platform(1500, baseHeight + 170, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(1750, baseHeight + 120, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(1950, baseHeight + 70, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2200, baseHeight + 20, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            
	            platforms.push_back(Platform(2600, baseHeight - 30, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2750, baseHeight - 80, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2950, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	
	            break;
	            
	        case FOREST_ADVENTURE:
	            // Plataforma inicial (recta)
	            platforms.push_back(Platform(0, baseHeight, 200, 30));
	            
	            // Plataforma recta
	            platforms.push_back(Platform(250, baseHeight, 150, 30));
	            
	            // Plataforma móvil (tronco flotante)
	            platforms.push_back(Platform(450, baseHeight - 30, 100, 30, false, true, 0, false, false, true, 100, 1.5f));
	            
	            // Plataforma recta segura
	            platforms.push_back(Platform(600, baseHeight, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Plataforma móvil (tronco flotante)
	            platforms.push_back(Platform(800, baseHeight - 50, 120, 30, false, true, 0, false, false, true, 150, 2.0f));
	            
	            // Plataforma final segura (sin rampa)
	            platforms.push_back(Platform(1000, baseHeight - 30, 350, 40, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Plataformas adicionales seguras
	            platforms.push_back(Platform(1400, baseHeight + 40, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(1650, baseHeight + 20, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(1850, baseHeight, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2100, baseHeight - 20, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Nueva plataforma movible cerca de la meta
	            platforms.push_back(Platform(2300, baseHeight - 40, 120, 30, false, false, 0, false, false, true, 80, 1.2f, false, true));
	            
	            platforms.push_back(Platform(2600, baseHeight - 30, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2750, baseHeight - 80, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2950, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	
	            break;
	            
	        case DESERT_JOURNEY:
	            // Plataforma inicial (recta)
	            platforms.push_back(Platform(0, baseHeight, 200, 30));
	            
	            // Plataformas con espinas (cactus)
	            platforms.push_back(Platform(250, baseHeight, 150, 30, false, false, 0, true));
	            platforms.push_back(Platform(450, baseHeight - 40, 150, 30));
	            platforms.push_back(Platform(650, baseHeight, 150, 30, false, false, 0, true));
	            
	            // Plataforma móvil (roca flotante) segura
	            platforms.push_back(Platform(850, baseHeight - 30, 200, 30, false, false, 0, false, false, true, 150, 1.0f, false, true));
	            
	            // Plataforma recta segura
	            platforms.push_back(Platform(1100, baseHeight, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Plataforma móvil (roca flotante) segura
	            platforms.push_back(Platform(1300, baseHeight - 50, 200, 30, false, false, 0, false, false, true, 100, 1.5f, false, true));
	            
	            // Plataforma final segura (sin rampa)
	            platforms.push_back(Platform(1550, baseHeight - 30, 300, 40, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            platforms.push_back(Platform(2400, baseHeight, 200, 30));

	            
	            // Plataformas adicionales seguras
	            platforms.push_back(Platform(1900, baseHeight - 30, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2200, baseHeight - 50, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            platforms.push_back(Platform(2600, baseHeight - 30, 150, 30, false, false, 0, false, false, false, 0, 0, false, true)); 
                platforms.push_back(Platform(2750, baseHeight - 80, 150, 30, false, false, 0, false, false, false, 0, 0, false, true)); 
                platforms.push_back(Platform(2950, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
                break;
	
	            
	            
	        case NIGHT_FLIGHT:
	            // Plataforma inicial (recta)
	            platforms.push_back(Platform(0, baseHeight, 200, 30));
	            
	            // Plataformas flotantes (rectas) seguras
	            platforms.push_back(Platform(250, baseHeight - 30, 150, 30, false, false, 0, false, false, true, 100, 1.0f, false, true));
	            platforms.push_back(Platform(450, baseHeight + 30, 150, 30, false, false, 0, false, false, true, 80, 0.8f, false, true));
	            platforms.push_back(Platform(650, baseHeight - 30, 150, 30, false, false, 0, false, false, true, 120, 1.2f, false, true));
	            
	            // Plataforma recta segura
	            platforms.push_back(Platform(850, baseHeight, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Plataformas adicionales seguras para la noche
	            platforms.push_back(Platform(1100, baseHeight, 200, 30, false, false, 0, false, false, true, 150, 1.5f, false, true));
	            platforms.push_back(Platform(1350, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(1550, baseHeight, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(1800, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2000, baseHeight, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2250, baseHeight - 50, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            platforms.push_back(Platform(2600, baseHeight - 30, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2750, baseHeight - 80, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2950, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	
	            break;
	            
	        case SPACE_ODYSSEY:
	            // Plataforma inicial (recta)
	            platforms.push_back(Platform(0, baseHeight, 200, 30));
	            
	            // Plataformas flotantes (rectas) seguras
	            platforms.push_back(Platform(250, baseHeight - 30, 150, 30, false, false, 0, false, false, true, 120, 1.0f, false, true));
	            platforms.push_back(Platform(450, baseHeight + 30, 150, 30, false, false, 0, false, false, true, 100, 0.9f, false, true));
	            platforms.push_back(Platform(650, baseHeight - 30, 150, 30, false, false, 0, false, false, true, 150, 1.2f, false, true));
	            
	            // Plataforma recta segura
	            platforms.push_back(Platform(850, baseHeight, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Plataformas adicionales seguras para el espacio
	            platforms.push_back(Platform(1100, baseHeight, 200, 30, false, false, 0, false, false, true, 200, 1.5f, false, true));
	            platforms.push_back(Platform(1350, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(1550, baseHeight, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(1800, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2000, baseHeight, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            platforms.push_back(Platform(2250, baseHeight - 50, 200, 30, false, false, 0, false, false, false, 0, 0, false, true));
	            
	            // Cambiar estas líneas:
                platforms.push_back(Platform(2600, baseHeight - 30, 150, 30, false, false, 0, false, false, false, 0, 0, false, true)); 
                platforms.push_back(Platform(2750, baseHeight - 80, 150, 30, false, false, 0, false, false, false, 0, 0, false, true)); 
                platforms.push_back(Platform(2950, baseHeight - 50, 150, 30, false, false, 0, false, false, false, 0, 0, false, true));
                break;
	    }
	    
	    // Eliminar plataformas después del objeto final para que no se vea más mundo
	    for (size_t i = 0; i < platforms.size(); ) {
	        if (platforms[i].x > FINAL_OBJECT_X + 200) {
	            platforms.erase(platforms.begin() + i);
	        } else {
	            i++;
	        }
	    }
	}
	
	void initializeGameObjects() {
	    gameObjects.clear();
	    enemies.clear();
	    showConfetti = false;
	    confettiParticles.clear();
	    
	    // Altura base para todos los niveles (más centrado)
	    const float baseHeight = HEIGHT / 2 - 50;
	    
	    switch (levels[currentLevel].type) {
	        case MISTY_MORNING:
	            // Monedas más accesibles y mejor distribuidas
	            gameObjects.push_back(GameObject(100, baseHeight + 60, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(350, baseHeight + 40, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(700, baseHeight + 40, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(900, baseHeight + 40, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1200, baseHeight + 60, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1500, baseHeight + 200, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1850, baseHeight + 150, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2200, baseHeight + 70, 20, 20, COLLECTIBLE));
	            
	            // Estrella de teletransporte
	            gameObjects.push_back(GameObject(500, baseHeight + 80, 40, 40, POWER_UP));
	            break;
	            
	        case FOREST_ADVENTURE:
	            gameObjects.push_back(GameObject(150, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(400, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(700, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(900, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1200, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1600, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2000, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2300, baseHeight + 30, 20, 20, COLLECTIBLE));
	            
	            // Estrella de teletransporte
	            gameObjects.push_back(GameObject(600, baseHeight + 70, 40, 40, POWER_UP));
	            break;
	            
	        case DESERT_JOURNEY:
	            gameObjects.push_back(GameObject(200, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(500, baseHeight + 10, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(800, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1100, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1400, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1700, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2000, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2300, baseHeight + 50, 20, 20, COLLECTIBLE));
	            
	            // Estrella de teletransporte
	            gameObjects.push_back(GameObject(700, baseHeight + 70, 40, 40, POWER_UP));
	            
	            // Lo colocamos en el último tramo (ver cambio #3 para que patrulle solo el final)
                     boss = BossEnemy(FINAL_OBJECT_X - 600.0f, baseHeight + 140.0f,
                     220.0f, 140.0f, 400.0f, 2.2f, 32.0f);
                     boss.active = true;
                     bossDefeated = false;

                     break; // <- ESTE break evita que caiga a NIGHT_FLIGHT
	            
	        case NIGHT_FLIGHT:
	            gameObjects.push_back(GameObject(200, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(500, baseHeight + 70, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(800, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1100, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1400, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1700, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2000, baseHeight + 70, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2300, baseHeight + 50, 20, 20, COLLECTIBLE));
	            
	            // Estrella de teletransporte
	            gameObjects.push_back(GameObject(800, baseHeight + 90, 40, 40, POWER_UP));
	            
	            // Enemigos nocturnos
	            enemies.push_back(Enemy(600, baseHeight - 40, 50, 50, 3.0f, ENEMY_MAX_HEALTH, true));
	            enemies.push_back(Enemy(1300, baseHeight - 60, 50, 50, 3.5f, ENEMY_MAX_HEALTH, false));
	            enemies.push_back(Enemy(1900, baseHeight - 80, 50, 50, 4.0f, ENEMY_MAX_HEALTH, true));
	            enemies.push_back(Enemy(2500, baseHeight - 60, 50, 50, 3.0f, ENEMY_MAX_HEALTH, false));
	            break;
	            
	        case SPACE_ODYSSEY:
	            gameObjects.push_back(GameObject(200, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(500, baseHeight + 70, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(800, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1100, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1400, baseHeight + 30, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(1700, baseHeight + 50, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2000, baseHeight + 70, 20, 20, COLLECTIBLE));
	            gameObjects.push_back(GameObject(2300, baseHeight + 50, 20, 20, COLLECTIBLE));
	            
	            // Estrella de teletransporte
	            gameObjects.push_back(GameObject(900, baseHeight + 90, 40, 40, POWER_UP));
	            
	            // Configurar jefes espaciales
	            boss = BossEnemy(2600.0f, baseHeight + 180.0f, 240.0f, 150.0f, 350.0f, 2.6f, 42.0f);
                boss.active = true;
                bossDefeated = false;
	    }
	    
	    // Limpiar enemigos para niveles que no los tienen
	    if (levels[currentLevel].type == DESERT_JOURNEY ||
	        levels[currentLevel].type == NIGHT_FLIGHT ||
	        levels[currentLevel].type == SPACE_ODYSSEY) {
	        // No hacer nada - ya están configurados arriba
	    } else {
	        enemies.clear();
	    }
	    
	    // Plataforma y objeto final
	    platforms.push_back(Platform(FINAL_OBJECT_X - 50, baseHeight - 20, FINISH_LINE_WIDTH + 100, 20));
	    gameObjects.push_back(GameObject(FINAL_OBJECT_X, baseHeight, FINISH_LINE_WIDTH, FINISH_LINE_HEIGHT, FINAL_OBJECT));
	}
	
	bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	    return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
	}
	
	void updateMovingPlatforms() {
	    for (size_t i = 0; i < platforms.size(); i++) {
	        Platform& platform = platforms[i];
	        if (platform.isMoving && !platform.isBroken) {
	            platform.x = platform.initialX + sin(animationTime * platform.moveSpeed) * platform.moveDistance;
	        }
	        
	        // Actualizar efecto morado intermitente en las plataformas
	        if (levels[currentLevel].hasSpecialEffect) {
	            platform.purpleTimer += 0.016f;
	            if (platform.purpleTimer >= 1.0f) {
	                platform.purpleTimer = 0.0f;
	                platform.isPurple = !platform.isPurple;
	            }
	        }
	    }
	}
	
void handlePlatformCollisions() {
    player.onGround = false;
    
    for (size_t i = 0; i < platforms.size(); i++) {
        Platform& platform = platforms[i];
        if (platform.isBroken) continue;
        
        if (platform.isLog) {
            // Colisión con tronco vertical
            if (player.x + player.radius > platform.x && 
                player.x - player.radius < platform.x + platform.width &&
                player.y - player.radius <= platform.y + platform.height && 
                player.y - player.radius >= platform.y - 5 &&
                player.velocityY <= 0) {
                
                // Verificar si está en el hueco del tronco
                float holeX = platform.x + (platform.width - platform.width * 0.6f)/2;
                float holeWidth = platform.width * 0.6f;
                float holeHeight = platform.height * 0.3f;
                
                if (player.x > holeX && player.x < holeX + holeWidth && 
                    player.y - player.radius < platform.y + holeHeight) {
                    // Teletransportar arriba del tronco
                    player.y = platform.y + platform.height + player.radius;
                    player.velocityY = JUMP_STRENGTH * 0.5f;
                } else {
                    // Colisión normal con el tronco
                    player.y = platform.y + platform.height + player.radius;
                    
                    if (fabs(player.velocityY) > 2.0f) {
                        player.velocityY = -player.velocityY * BOUNCE_DAMPING;
                        player.squashY = 0.6f;
                    } else {
                        player.velocityY = 0;
                    }
                    player.onGround = true;
                }
            }
        }
        else if (platform.isRamp) {
            // Colisión con rampa
            float rampHeight = (player.x - platform.x) * platform.height / platform.width;
            float rampTop = platform.y + rampHeight;
            
            if (player.x > platform.x && player.x < platform.x + platform.width &&
                player.y - player.radius <= rampTop && 
                player.y - player.radius >= rampTop - 5 &&
                player.velocityY <= 0) {
                
                player.y = rampTop + player.radius;
                
                if (fabs(player.velocityY) > 2.0f) {
                    player.velocityY = -player.velocityY * BOUNCE_DAMPING;
                    player.squashY = 0.6f;
                } else {
                    player.velocityY = 0;
                }
                player.onGround = true;
            }
        }
        else if (platform.isCurved) {
            float platformCenterX = platform.x + platform.width / 2;
            float distanceFromCenter = fabs(player.x - platformCenterX);
            
            if (distanceFromCenter < platform.width / 2) {
                float t = (float)distanceFromCenter / (platform.width / 2);
                float curveHeight = sin((1 - t) * 3.14159265f) * platform.curveRadius;
                float expectedY = platform.y + curveHeight;
                
                if (player.y - player.radius <= expectedY + platform.height && 
                    player.y - player.radius >= expectedY - 5 && 
                    player.velocityY <= 0) {
                    
                    // Verificar si hay espinas
                    if (platform.hasSpikes && !platform.isSafe && !player.isInvincible) {
                        player.health -= 10;
                        player.isInvincible = true;
                        player.invincibleTimer = 2.0f;
                        if (hurtSound) Mix_PlayChannel(-1, hurtSound, 0);
                        player.velocityY = JUMP_STRENGTH * 0.3f;
                    } else {
                        // Ajustar la posición del jugador para que siga la curva
                        player.y = expectedY + platform.height + player.radius;
                        player.velocityY = -player.velocityY * BOUNCE_DAMPING;
                        player.squashY = 0.6f;
                        player.onGround = true;
                    }
                }
            }
        }
        else {
            // Colisión con plataforma normal
            if (player.x + player.radius > platform.x && 
                player.x - player.radius < platform.x + platform.width &&
                player.y - player.radius <= platform.y + platform.height && 
                player.y - player.radius >= platform.y - 5 &&
                player.velocityY <= 0) {
                
                // SIEMPRE permitir el rebote físico, incluso cuando es invencible
                player.y = platform.y + platform.height + player.radius;
                
                if (fabs(player.velocityY) > 2.0f) {
                    player.velocityY = -player.velocityY * BOUNCE_DAMPING;
                    player.squashY = 0.6f;
                } else {
                    player.velocityY = 0;
                }
                player.onGround = true;
                
                // Solo aplicar daño si NO es invencible y la plataforma tiene espinas
                if (platform.hasSpikes && !platform.isSafe && !player.isInvincible) {
                    player.health -= 10;
                    player.isInvincible = true;
                    player.invincibleTimer = 2.0f;
                    if (hurtSound) Mix_PlayChannel(-1, hurtSound, 0);
                    
                    // Pequeño rebote hacia arriba al recibir daño
                    player.velocityY = JUMP_STRENGTH * 0.3f;
                }
            }
        }
    }
}


	void handleProjectileCollisions() {
    for (size_t i = 0; i < projectiles.size(); ) {
        Projectile& proj = projectiles[i];
        bool shouldErase = false;
        
        // Verificar colisión con enemigos
        for (size_t j = 0; j < enemies.size(); j++) {
            Enemy& enemy = enemies[j];
            if (enemy.isActive && 
                checkCollision(proj.x - PROJECTILE_RADIUS, proj.y - PROJECTILE_RADIUS,
                              PROJECTILE_RADIUS * 2, PROJECTILE_RADIUS * 2,
                              enemy.x, enemy.y, enemy.width, enemy.height)) {
                enemy.health -= 20;
                if (enemy.health <= 0) {
                    enemy.isActive = false;
                    score += 30;
                }
                shouldErase = true;
                break;
            }
        }
        
        // Verificar colisión con el jefe (si existe)
        if (boss.active && 
            checkCollision(proj.x - PROJECTILE_RADIUS, proj.y - PROJECTILE_RADIUS,
                          PROJECTILE_RADIUS * 2, PROJECTILE_RADIUS * 2,
                          boss.x, boss.y, boss.width, boss.height)) {
            boss.health -= 20;
            if (boss.health <= 0) {
                boss.active = false;
                bossDefeated = true;
                score += 150;
            }
            shouldErase = true;
        }
        
        // Eliminar proyectiles que hayan colisionado o salido de pantalla
        if (shouldErase || 
            proj.x < cameraX || proj.x > cameraX + WIDTH ||
            proj.y < 0 || proj.y > HEIGHT) {
            projectiles.erase(projectiles.begin() + i);
        } else {
            i++;
        }
    }
}
	
	
void handleObjectCollisions() {
    int totalCoins = 0;
    int collectedCoins = 0;
    bool canFinishLevel = true;
    float playerX = player.x;
    float playerY = player.y;

    // Verificar si hay jefe y no ha sido derrotado (solo para mundos 3 y 5)
    if ((levels[currentLevel].type == DESERT_JOURNEY || levels[currentLevel].type == SPACE_ODYSSEY) && !bossDefeated) {
        canFinishLevel = false;
    }

    for (size_t i = 0; i < gameObjects.size(); ++i) {
        GameObject& obj = gameObjects[i];

        if (!obj.collected &&
            checkCollision(
                player.x - player.radius, player.y - player.radius,
                player.radius * 2, player.radius * 2,
                obj.x, obj.y, obj.width, obj.height)) {

            if (obj.type == COLLECTIBLE) {
                obj.collected = true;
                score += 10;
                if (collectSound) Mix_PlayChannel(-1, collectSound, 0);
            }
            else if (obj.type == POWER_UP) {
                obj.collected = true;
                score += 50;
                if (starSound) Mix_PlayChannel(-1, starSound, 0);

                // Teletransporte a plataforma segura
                bool teleported = false;
                for (size_t j = 0; j < platforms.size(); ++j) {
                    const Platform& platform = platforms[j];
                    if (platform.isSafe && platform.x > player.x) {
                        player.x = platform.x + platform.width / 2;
                        player.y = platform.y + platform.height + player.radius;
                        player.velocityY = 0;
                        player.velocityX = 0;
                        teleported = true;
                        break;
                    }
                }
                if (!teleported) {
                    for (size_t j = 0; j < platforms.size(); ++j) {
                        const Platform& platform = platforms[j];
                        if (platform.isSafe) {
                            player.x = platform.x + platform.width / 2;
                            player.y = platform.y + platform.height + player.radius;
                            player.velocityY = 0;
                            player.velocityX = 0;
                            break;
                        }
                    }
                }
            }
            else if (obj.type == FINAL_OBJECT) {
                // Contar monedas recolectadas
                totalCoins = 0;
                collectedCoins = 0;
                for (size_t j = 0; j < gameObjects.size(); ++j) {
                    if (gameObjects[j].type == COLLECTIBLE) {
                        ++totalCoins;
                        if (gameObjects[j].collected) ++collectedCoins;
                    }
                }

                // ¿cumple el 50%?
                if (collectedCoins >= totalCoins / 2) {
                    // Detener música actual (esto ahora está en el código de transición de nivel)
                    // if (Mix_PlayingMusic()) Mix_HaltMusic();
                    
                    // Reproducir sonido de victoria en TODOS los mundos
                    if (winSound) Mix_PlayChannel(-1, winSound, 0);

                    showConfetti = true;
                    confettiTimer = 3.0f;

                    // Confeti
                    confettiParticles.clear();
                    for (int k = 0; k < 100; ++k) {
                        confettiParticles.push_back(std::make_pair(
                            obj.x + obj.width / 2 + (rand() % 200 - 100),
                            obj.y + obj.height / 2 + (rand() % 200 - 100)
                        ));
                    }

                    // Efecto salto
                    player.velocityY = JUMP_STRENGTH * 0.8f;
                    
                    // Marcar objeto final como recolectado
                    obj.collected = true;
                } else {
                    // No completa: no marcar como recogido
                    obj.collected = false;
                }
            }
        }
    }

    // Verificación adicional para asegurar el sonido de meta
    if (canFinishLevel && playerX >= FINAL_OBJECT_X - 100 && !showConfetti) {
        for (size_t i = 0; i < gameObjects.size(); i++) {
            if (gameObjects[i].type == FINAL_OBJECT && !gameObjects[i].collected) {
                // Solo reproducir sonido si no se ha hecho ya
                if (winSound) Mix_PlayChannel(-1, winSound, 0);
                gameObjects[i].collected = true;
                showConfetti = true;
                confettiTimer = 3.0f;
                
                // Confeti adicional por si acaso
                confettiParticles.clear();
                for (int k = 0; k < 100; ++k) {
                    confettiParticles.push_back(std::make_pair(
                        gameObjects[i].x + gameObjects[i].width / 2 + (rand() % 200 - 100),
                        gameObjects[i].y + gameObjects[i].height / 2 + (rand() % 200 - 100)
                    ));
                }
                
                player.velocityY = JUMP_STRENGTH * 0.8f;
            }
        }
    }
}
	
	void shootProjectile() {
	    if (player.shootCooldown <= 0) {
	        // Crear un nuevo proyectil en la dirección que mira el jugador
	        float direction = 1.0f;
	        if (player.velocityX < 0) direction = -1.0f;
	        
	        projectiles.push_back(Projectile(
	            player.x, player.y, 
	            direction * PROJECTILE_SPEED, 
	            0.0f
	        ));
	        
	        player.shootCooldown = 0.5f;
	    }
	}
	
	void updateConfetti() {
	    if (!showConfetti) return;
	    
	    confettiTimer -= 0.016f;
	    if (confettiTimer <= 0) {
	        showConfetti = false;
	        currentState = LEVEL_COMPLETE;
	    }
	    
	    // Mover las partículas de confeti
	    for (size_t i = 0; i < confettiParticles.size(); i++) {
	        confettiParticles[i].second -= 2.0f;
	        confettiParticles[i].first += (rand() % 5 - 2);
	    }
	}
	
	// Reemplaza la función handleSingleBoss existente con esta versión corregida:
	void handleBossCollisions() {
    if (!levels[currentLevel].hasEnemies || !boss.active) return;

    // Movimiento horizontal con rebote en “arena de combate”
    boss.animationTime += 0.016f;
    boss.x += boss.speed;

    float arenaWidth = 500.0f; // patrulla en los últimos 500px
    float rightBound = FINAL_OBJECT_X - 100.0f;
    float leftBound  = rightBound - arenaWidth;
    if (leftBound < 0) leftBound = 0;

    // Bobbing vertical (SIEMPRE en el aire)
    boss.y = boss.baseY + sinf(boss.animationTime * 2.0f) * boss.hoverAmplitude;

    // Daño por contacto al jugador
    if (player.x + player.radius > boss.x && player.x - player.radius < boss.x + boss.width &&
        player.y + player.radius > boss.y && player.y - player.radius < boss.y + boss.height) {

        if (!player.isInvincible) {
            player.health -= 15;
            player.isInvincible = true;
            player.invincibleTimer = 2.0f;
            if (hurtSound) Mix_PlayChannel(-1, hurtSound, 0);
        }
    }

    // Colisiones de proyectiles con el boss
    for (size_t i=0; i<projectiles.size();) {
        Projectile& p = projectiles[i];
        if (p.isActive &&
            p.x > boss.x && p.x < boss.x + boss.width &&
            p.y > boss.y && p.y < boss.y + boss.height) {

            boss.health -= 20;
            projectiles.erase(projectiles.begin()+i);
            if (boss.health <= 0) {
                boss.active = false;
                bossDefeated = true;
                score += 150;
            }
            continue;
        }
        ++i;
    }
}
	    
	    
	      
	
	// Necesitarás modificar tu función renderTextCentered para aceptar la fuente:
void renderTextCentered(int x, int y, const string& text, void* font = GLUT_BITMAP_9_BY_15, float scale = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);
    
    int width = 0;
    for (char c : text) {
        width += glutBitmapWidth(font, c);
    }
    
    glRasterPos2i(-width/2, 0);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
}
	
	void drawGameComplete() {
    // Silencio garantizado en la pantalla de agradecimiento
    if (Mix_PlayingMusic()) Mix_HaltMusic();

    const UITheme& T = NIGHT();

    glClearColor(0,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
    drawGradientRect(0,0, WIDTH, HEIGHT, T.bgTop, T.bgBottom);

    // Panel
    float px=100, py=100, pw=WIDTH-200, ph=HEIGHT-200;
    drawPanel(px,py,pw,ph,T);

    // Título - Fuente grande y llamativa
    glColor3f(1.0f, 1.0f, 0.0f);
    renderTextCentered(WIDTH/2, py + ph - 100, "¡GANASTE!", GLUT_BITMAP_TIMES_ROMAN_24, 2.0f);

    // Mensaje - Fuente estándar más grande
    glColor3f(1.0f, 1.0f, 1.0f);
    renderTextCentered(WIDTH/2, py + ph - 180, "Terminaste la aventura de Yuls Tales", GLUT_BITMAP_HELVETICA_18);
    renderTextCentered(WIDTH/2, py + ph - 230, "Esperamos que hayas disfrutado", GLUT_BITMAP_HELVETICA_18);

    // Créditos - Fuente estándar
    glColor3f(0.8f, 0.8f, 0.8f);
    renderTextCentered(WIDTH/2, py + ph - 300, "Tu heroe salvo el reino de la oscuridad", GLUT_BITMAP_9_BY_15);
    renderTextCentered(WIDTH/2, py + ph - 330, "y trajo paz a estas tierras", GLUT_BITMAP_9_BY_15);

    // Instrucción para volver (parpadeante)
    if (fmod(animationTime, 1.0f) < 0.5f) {
        glColor3f(1.0f, 1.0f, 0.0f);
        renderTextCentered(WIDTH/2, py + 50, "Presiona ESC para salir", GLUT_BITMAP_HELVETICA_12);
    }
}

void drawAbout() {
    const UITheme& T = NIGHT();

    glClearColor(0,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
    drawGradientRect(0,0, WIDTH, HEIGHT, T.bgTop, T.bgBottom);

    // Panel
    float px=100, py=100, pw=WIDTH-200, ph=HEIGHT-200;
    drawPanel(px,py,pw,ph,T);

    // Título - Fuente destacada
    glColor3f(1.0f, 1.0f, 0.0f);
    renderTextCentered(WIDTH/2, py + ph - 50, "CREDITOS", GLUT_BITMAP_TIMES_ROMAN_24, 1.5f);

    // Créditos principales
    glColor3f(1.0f, 1.0f, 1.0f);
    renderTextCentered(WIDTH/2, py + ph - 120, "Creado por: Yulenny Bonilla", GLUT_BITMAP_HELVETICA_18);
    renderTextCentered(WIDTH/2, py + ph - 170, "Matricula 2023-0769", GLUT_BITMAP_HELVETICA_18);
    
    // Información adicional - Fuente más pequeña
    glColor3f(0.8f, 0.8f, 0.8f);
    renderTextCentered(WIDTH/2, py + ph - 230, "Juego desarrollado con C++ y OpenGL", GLUT_BITMAP_9_BY_15);
    renderTextCentered(WIDTH/2, py + ph - 260, "Motor gráfico personalizado", GLUT_BITMAP_9_BY_15);
    renderTextCentered(WIDTH/2, py + ph - 290, "Bandas sonoras compuestas por Artista X", GLUT_BITMAP_9_BY_15);

    // Año/versión - Fuente pequeña
    glColor3f(0.6f, 0.6f, 0.6f);
    renderTextCentered(WIDTH/2, py + 60, "© 2025 Todos los derechos reservados", GLUT_BITMAP_8_BY_13);

    // Instrucción para volver
    if (fmod(animationTime, 1.0f) < 0.5f) {
        glColor3f(1.0f, 1.0f, 0.0f);
        renderTextCentered(WIDTH/2, py + 30, "Presiona ESC para regresar", GLUT_BITMAP_HELVETICA_12);
    }
}


	// Función para dibujar jefes
	void drawBoss() {
    if (!boss.active) return;

    // Barra de vida sobre el boss
    float healthRatio = std::max(0.0f, boss.health) / ((levels[currentLevel].type == DESERT_JOURNEY) ? BOSS_HEALTH_DESERT : BOSS_HEALTH_SPACE);
    glColor3f(1.0f - healthRatio, healthRatio, 0.0f);
    drawRect(boss.x, boss.y + boss.height + 12, boss.width * healthRatio, 8, 1.0f - healthRatio, healthRatio, 0.0f);

    // Animación base (aletear / tentáculos)
    float t = boss.animationTime;
    float flap = sinf(t * 8.0f);    // -1..1
    float wob  = sinf(t * 2.0f);    // -1..1

    if (levels[currentLevel].type == DESERT_JOURNEY) {
        // ---- DRAGÓN DE FUEGO (estilo de la imagen, volador) ----
        // cuerpo
        glColor3f(0.95f, 0.35f, 0.15f);
        drawEllipse(boss.x + boss.width*0.45f, boss.y + boss.height*0.45f, boss.width*0.45f, boss.height*0.35f, 0.95f, 0.35f, 0.15f);

        // cabeza
        glColor3f(0.98f, 0.50f, 0.20f);
        drawCircle(boss.x + boss.width*0.85f, boss.y + boss.height*0.65f, boss.height*0.22f, 0.98f,0.50f,0.20f);

        // ojo
        glColor3f(1.0f, 0.95f, 0.2f);
        drawCircle(boss.x + boss.width*0.90f, boss.y + boss.height*0.72f, boss.height*0.06f, 1,1,0.2f);

        // ala superior (aleteo)
        glPushMatrix();
        glTranslatef(boss.x + boss.width*0.35f, boss.y + boss.height*0.70f, 0);
        glRotatef(flap * 18.0f, 0,0,1);
        glColor3f(0.85f, 0.25f, 0.18f);
        glBegin(GL_TRIANGLES);
            glVertex2f(0, 0);
            glVertex2f(-boss.width*0.55f,  boss.height*0.10f);
            glVertex2f(-boss.width*0.25f, -boss.height*0.35f);
        glEnd();
        glPopMatrix();

        // ala inferior
        glPushMatrix();
        glTranslatef(boss.x + boss.width*0.45f, boss.y + boss.height*0.30f, 0);
        glRotatef(-flap * 16.0f, 0,0,1);
        glColor3f(0.88f, 0.30f, 0.20f);
        glBegin(GL_TRIANGLES);
            glVertex2f(0, 0);
            glVertex2f(-boss.width*0.50f, -boss.height*0.05f);
            glVertex2f(-boss.width*0.20f,  boss.height*0.32f);
        glEnd();
        glPopMatrix();

        // cola
        glColor3f(0.8f, 0.2f, 0.1f);
        glBegin(GL_TRIANGLES);
            glVertex2f(boss.x + boss.width*0.05f, boss.y + boss.height*0.45f);
            glVertex2f(boss.x - boss.width*0.15f, boss.y + boss.height*(0.45f + 0.10f*wob));
            glVertex2f(boss.x + boss.width*0.05f, boss.y + boss.height*0.35f);
        glEnd();

        // llamita (suave)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.8f, 0.2f, 0.6f);
        drawCircle(boss.x + boss.width*0.98f, boss.y + boss.height*(0.62f + 0.01f*wob), boss.height*(0.08f + 0.02f*fabsf(flap)), 1,0.8f,0.2f);
        glDisable(GL_BLEND);

    } else { // SPACE_ODYSSEY
        // ---- PULPO ALIEN (tentáculos ondulantes) ----
        // cabeza/ cuerpo
        glColor3f(0.55f, 0.25f, 0.85f);
        drawEllipse(boss.x + boss.width*0.5f, boss.y + boss.height*0.55f, boss.width*0.45f, boss.height*0.40f, 0.55f,0.25f,0.85f);

        // ojos
        glColor3f(1,1,1);
        drawCircle(boss.x + boss.width*0.40f, boss.y + boss.height*0.65f, boss.height*0.10f, 1,1,1);
        drawCircle(boss.x + boss.width*0.60f, boss.y + boss.height*0.65f, boss.height*0.10f, 1,1,1);
        glColor3f(0,0,0);
        drawCircle(boss.x + boss.width*0.40f, boss.y + boss.height*0.65f, boss.height*0.05f, 0,0,0);
        drawCircle(boss.x + boss.width*0.60f, boss.y + boss.height*0.65f, boss.height*0.05f, 0,0,0);

        // tentáculos (6)
        glColor3f(0.65f, 0.35f, 0.95f);
        for (int i=0;i<6;i++){
            float phase = t*2.0f + i*0.6f;
            float len = boss.height*0.55f + 6.0f*sinf(phase);
            float tx = boss.x + boss.width*(0.20f + i*0.12f);
            float ty = boss.y + boss.height*0.12f;
            glBegin(GL_QUADS);
                glVertex2f(tx, ty);
                glVertex2f(tx+8, ty);
                glVertex2f(tx+8 + 18*sinf(phase), ty - len);
                glVertex2f(tx     + 18*sinf(phase), ty - len);
            glEnd();
        }
    }
}
	
	void updatePlayer() {
    if (currentState != PLAYING) return;
    
    player.animationAngle += 0.1f;
    animationTime += 0.016f;
    gameTime += 0.016f;
    
    // Actualizar temporizador de invencibilidad
    if (player.isInvincible) {
        player.invincibleTimer -= 0.016f;
        if (player.invincibleTimer <= 0) {
            player.isInvincible = false;
        }
    }
    
    // Actualizar cooldown de disparo
    if (player.shootCooldown > 0) {
        player.shootCooldown -= 0.016f;
    }
    
    // Suavizar el efecto de aplastamiento (solo para forma normal)
    if (player.currentForm == RED_BALL && player.squashY < 1.0f) {
        player.squashY += 0.05f;
        if (player.squashY > 1.0f) player.squashY = 1.0f;
    }
    

    // Propiedades físicas según la forma actual
    float currentMoveSpeed = MOVE_SPEED;
    float currentGravity = GRAVITY;
    float currentJumpStrength = JUMP_STRENGTH;
    float currentBounceDamping = BOUNCE_DAMPING;
    float currentFriction = FRICTION;
    
    switch(player.currentForm) {
        case ROCK_FORM:
            // Más pesada, menos ágil pero con movimiento más controlado
            currentMoveSpeed *= 0.8f;  // 20% más lenta (antes era 0.6f)
            currentGravity *= 1.5f;   // 50% más gravedad (antes era 1.8f)
            currentJumpStrength *= 0.7f; // 30% menos salto (antes era 0.6f)
            currentBounceDamping *= 0.8f; // Rebota menos (antes era 0.7f)
            currentFriction *= 1.1f;  // Poco más de fricción (antes era 1.3f)
            break;
            
        case BEACH_BALL_FORM:
            // Más liviana, más ágil
            currentMoveSpeed *= 1.2f;  // 20% más rápida
            currentGravity *= 0.6f;   // 40% menos gravedad
            currentJumpStrength *= 1.3f; // 30% más salto
            currentBounceDamping *= 1.1f; // Rebota más
            currentFriction *= 0.8f;  // Menos fricción (se desliza más)
            break;
    }
    
    // Movimiento horizontal
    if (keys['a'] || keys['A']) {
        player.velocityX = -currentMoveSpeed;
    } else if (keys['d'] || keys['D']) {
        player.velocityX = currentMoveSpeed;
    } else {
        player.velocityX *= currentFriction;
        if (fabs(player.velocityX) < 0.1f) player.velocityX = 0;
    }
    
    // Salto
    if ((keys['w'] || keys['W'] || keys[' ']) && player.onGround) {
    player.velocityY = currentJumpStrength;
    player.onGround = false;
    Mix_PlayChannel(-1, jumpSound, 0); // Reproducir sonido de salto
}
    
    // Aplicar gravedad
    player.velocityY -= currentGravity;
    
    // Limitar velocidad de caída
    if (player.velocityY < -currentJumpStrength * 1.5f) {
        player.velocityY = -currentJumpStrength * 1.5f;
    }
    
    // Actualizar posición
    player.x += player.velocityX;
    player.y += player.velocityY;
    
    // Limitar el movimiento después de la meta
    if (player.x > FINAL_OBJECT_X + 100) {
        player.x = FINAL_OBJECT_X + 100;
        player.velocityX = 0;
    }
    
    // Actualizar plataformas móviles
    updateMovingPlatforms();
    
    // Manejar colisiones
    handlePlatformCollisions();
    handleObjectCollisions();
    handleProjectileCollisions();
    handleBossCollisions();
    
    // Actualizar confeti si es necesario
    if (showConfetti) {
        updateConfetti();
    }
    
    bool canFinishLevel = true;
    bool levelComplete = false;
    float playerX = player.x;
    float playerY = player.y;

    if ((levels[currentLevel].type == DESERT_JOURNEY || levels[currentLevel].type == SPACE_ODYSSEY) && !bossDefeated) {
    canFinishLevel = false;
}

    // Verificar finalización del nivel
    if (canFinishLevel && playerX >= FINAL_OBJECT_X - 100 && !showConfetti) {
        // Buscar el objeto final para activar los confetis
        for (size_t i = 0; i < gameObjects.size(); i++) {
            if (gameObjects[i].type == FINAL_OBJECT && !gameObjects[i].collected) {
                gameObjects[i].collected = true;
                showConfetti = true;
                confettiTimer = 3.0f;
                
                // Crear partículas de confeti
                confettiParticles.clear();
                for (int j = 0; j < 100; j++) {
                    confettiParticles.push_back(make_pair(
                        gameObjects[i].x + gameObjects[i].width/2 + (rand() % 200 - 100),
                        gameObjects[i].y + gameObjects[i].height/2 + (rand() % 200 - 100)
                    ));
                }
                
                // Rebote del jugador
                player.velocityY = JUMP_STRENGTH * 0.8f;
                break;
            }
        }
    } 
    
    // Verificar si el jugador cayó
    if (player.y < -50) {
        lives--;
        if (lives <= 0) {
            currentState = GAME_OVER;
        } else {
            player.x = 100;
            player.y = PLAYER_START_Y;
            player.velocityX = 0;
            player.velocityY = 0;
            player.health = PLAYER_MAX_HEALTH;
            player.isInvincible = true;
            player.invincibleTimer = 2.0f;
            player.currentForm = RED_BALL; // Volver a forma normal al morir
            
        }
    }
    
    // Verificar si el jugador perdió toda su salud
    if (player.health <= 0) {
        lives--;
        if (lives <= 0) {
            currentState = GAME_OVER;
        } else {
            player.x = 100;
            player.y = PLAYER_START_Y;
            player.velocityX = 0;
            player.velocityY = 0;
            player.health = PLAYER_MAX_HEALTH;
            player.isInvincible = true;
            player.invincibleTimer = 2.0f;
            player.currentForm = RED_BALL; // Volver a forma normal al morir
        }
    }
    
    // Actualizar la cámara para seguir al jugador
    cameraX = player.x - WIDTH / 2;
    if (cameraX < 0) cameraX = 0;
    
    // Efecto especial para el nivel 2 (parpadeo morado)
    if (levels[currentLevel].hasSpecialEffect) {
        specialEffectTimer += 0.016f;
        if (specialEffectTimer >= 1.0f) {
            specialEffectTimer = 0.0f;
            effectActive = !effectActive;
        }
    }
}
	
	void drawGameWorld() {
	    glPushMatrix();
	    glTranslatef(-cameraX, 0, 0);
	    
	    drawPlatforms();
	    drawGameObjects();
	    if (levels[currentLevel].hasEnemies) {
	        drawEnemies();
	        if ((levels[currentLevel].type == DESERT_JOURNEY || levels[currentLevel].type == SPACE_ODYSSEY) && boss.active) {
           drawBoss();
         }  
	    }
	    drawProjectiles();
	    drawPlayer();
	    drawConfetti();
	    
	    glPopMatrix();
	}
	
	void display() {
	    glClear(GL_COLOR_BUFFER_BIT);
	    
	    switch (currentState) {
	        case MENU:
	            drawMenu();
	            break;
	        case LEVEL_SELECT:
	            drawLevelSelect();
	            break;
	        case INSTRUCTIONS:
	            drawInstructions();
	            break;
	        case STORY:
	            drawStory();
	            break;
	        case SHOWING_TEXT:
	            drawTextScreen();
	            break;
	        case PLAYING:
	            // Dibujar fondo primero
	            glMatrixMode(GL_PROJECTION);
	            glPushMatrix();
	            glLoadIdentity();
	            gluOrtho2D(0, WIDTH, 0, HEIGHT);
	            
	            glMatrixMode(GL_MODELVIEW);
	            glPushMatrix();
	            glLoadIdentity();
	            
	            drawStaticBackground();
	            
	            
	            glPopMatrix();
	            glMatrixMode(GL_PROJECTION);
	            glPopMatrix();
	            glMatrixMode(GL_MODELVIEW);
	            
	            // Dibujar mundo del juego
	            glPushMatrix();
	            glTranslatef(-cameraX, 0, 0);
	            
	            drawPlatforms();
	            drawGameObjects();
	            if (levels[currentLevel].hasEnemies) {
	                drawEnemies();
	            }
	            drawProjectiles();
	            drawPlayer();
	            drawConfetti();
	            
	            glPopMatrix();
	            
	            // Dibujar UI
	            glMatrixMode(GL_PROJECTION);
	            glPushMatrix();
	            glLoadIdentity();
	            gluOrtho2D(0, WIDTH, 0, HEIGHT);
	            
	            glMatrixMode(GL_MODELVIEW);
	            glPushMatrix();
	            glLoadIdentity();
	            
	            drawUI();
	            
	            glPopMatrix();
	            glMatrixMode(GL_PROJECTION);
	            glPopMatrix();
	            glMatrixMode(GL_MODELVIEW);
	            break;
	            
	            case PAUSED:
	    // Configurar vista
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluOrtho2D(0, WIDTH, 0, HEIGHT);
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	    
	    // Dibujar fondo del nivel
	    drawStaticBackground();
	    
	    // Dibujar mundo del juego (congelado)
	    glPushMatrix();
	    glTranslatef(-cameraX, 0, 0);
	    drawGameWorld();
	    glPopMatrix();
	    
	    // Dibujar UI del juego
	    drawUI();
	    
	    // Dibujar menú de pausa encima
	    drawPauseMenu();
	    break;
	    
	    
	        case GAME_OVER:
	            drawGameOverScreen();
	            break;
	        case LEVEL_COMPLETE:
	            drawLevelComplete();
	            break;
	        case GAME_COMPLETE:
                drawGameComplete();
            break;
            case ABOUT:
              drawAbout();
            break;
	    }
	    
	
	    
	    glutSwapBuffers();
	}
	
	
void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
    
    switch (currentState) {
        case MENU:
            if (key == 13) { // Enter
                stopMusic();
                playMenuMusic();
                switch (selectedMenuItem) {
                    case 0: // New Game
                        currentLevel = 0;
                        score = 0;
                        lives = 3;
                        gameTime = 0;
                        player = Player();
                        initializePlatforms();
                        initializeGameObjects();
                        initializeIntroTexts();
                        break;
                    case 1: // Level Select
                        currentState = LEVEL_SELECT;
                        selectedMenuItem = 0;
                        break;
                    case 2: // Instructions
                        currentState = INSTRUCTIONS;
                        instructionScroll = 0;
                        break;
                    case 3: // Story
                        currentState = STORY;
                        break;
                    case 4: // Acerca de
                        currentState = ABOUT;
                        break;
                    case 5: // Salir
                        exit(0);
                        break;
                }
            }
            break;
        case LEVEL_SELECT:
            if (key == 13) { // Enter
                if (levels[selectedMenuItem].unlocked) {
                    stopMusic();
                    playWorldMusic(selectedMenuItem);
                    currentLevel = selectedMenuItem;
                    score = 0;
                    lives = 3;
                    gameTime = 0;
                    player = Player();
                    initializePlatforms();
                    initializeGameObjects();
                    initializeIntroTexts();
                }
            } else if (key == 27) { // ESC
                currentState = MENU;
                selectedMenuItem = 0;
            }
            break;
        case INSTRUCTIONS:
            if (key == 27) { // ESC
                currentState = MENU;
                selectedMenuItem = 0;
            }
            break;
        case STORY:
            if (key == 27) { // ESC
                currentState = MENU;
                selectedMenuItem = 0;
            }
            break;
        case SHOWING_TEXT:
            if (key == ' ') { // Espacio para avanzar texto
                currentTextIndex++;
                textDisplayTimer = 0.0f;
                
                if (currentTextIndex >= (int)introTexts.size()) {
                    currentState = PLAYING;
                    playWorldMusic(currentLevel);
                }
            }
            break;
        case PLAYING:
            if (key == 't' || key == 'T') {
                if (player.currentForm == RED_BALL) {
                    player.currentForm = ROCK_FORM;
                } else if (player.currentForm == ROCK_FORM) {
                    player.currentForm = BEACH_BALL_FORM;
                } else {
                    player.currentForm = RED_BALL;
                }
                player.isInvincible = true;
                player.invincibleTimer = 0.5f;
            }
            
            if (key == 'p' || key == 'P') {
                pauseMusic();
                currentState = PAUSED;
                selectedMenuItem = 0;
            }
            break;
        case PAUSED:
            if (key == 13) { // Enter
                switch (selectedMenuItem) {
                    case 0: // Continuar
                        resumeMusic();
                        currentState = PLAYING;
                        break;
                    case 1: // Reiniciar
                        score = 0;
                        lives = 3;
                        gameTime = 0;
                        player = Player();
                        initializePlatforms();
                        initializeGameObjects();
                        currentState = PLAYING;
                        playWorldMusic(currentLevel);
                        break;
                    case 2: // Salir
                        stopMusic();
                        playMenuMusic();
                        currentState = MENU;
                        selectedMenuItem = 0;
                        break;
                }
            } 
            else if (key == 27) { // ESC
                resumeMusic();
                currentState = PLAYING;
            }
            break;
        case GAME_OVER:
            if (key == 13) { // ENTER
                if (selectedOption == 0) {
                    score = 0;
                    lives = 3;
                    gameTime = 0;
                    player = Player();
                    initializePlatforms();
                    initializeGameObjects();
                    currentState = PLAYING;
                    playWorldMusic(currentLevel);
                } else {
                    stopMusic();
                    playMenuMusic();
                    currentState = MENU;
                    selectedMenuItem = 0;
                }
            }
            break;
        case LEVEL_COMPLETE:
            if (key == 13) { // Enter
                if (currentLevel + 1 < (int)levels.size() && 
                    score >= levels[currentLevel].targetScore * 0.5f) {
                    if (Mix_PlayingMusic()) {
                        Mix_HaltMusic();
                    }
                    currentLevel++;
                    gameTime = 0;
                    player = Player();
                    initializePlatforms();
                    initializeGameObjects();
                    initializeIntroTexts();
                    playWorldMusic(currentLevel);
                } else {
                    if (currentLevel == (int)levels.size() - 1) {
                        currentState = GAME_COMPLETE;
                        stopMusic();
                        Mix_PlayMusic(endingMusic, 0);
                        playMenuMusic();
                    } else {
                        stopMusic();
                        playMenuMusic();
                        currentState = MENU;
                        selectedMenuItem = 0;
                    }
                }
            }
            else if (key == 27) { // ESC
                stopMusic();
                playMenuMusic();
                currentState = MENU;
                selectedMenuItem = 0;
            }
            break;
        case GAME_COMPLETE:
            if (key == 27) { // ESC
                stopMusic();
                playMenuMusic();
                currentState = MENU;
            }
            break;
        case ABOUT:  // <-- NUEVO CASO AÑADIDO
            if (key == 27) { // ESC
                currentState = MENU;
                playMenuMusic();
            }
            break;
    }
}
	
	void keyboardUp(unsigned char key, int x, int y) {
	    keys[key] = false;
	}
	
	void specialKeys(int key, int x, int y) {
	    switch (currentState) {
	        case MENU:
	            if (key == GLUT_KEY_UP) {
	                selectedMenuItem = (selectedMenuItem - 1 + 6) % 6;
	            } else if (key == GLUT_KEY_DOWN) {
	                selectedMenuItem = (selectedMenuItem + 1) % 6;
	            }
	            break;
	        case LEVEL_SELECT:
	            if (key == GLUT_KEY_UP) {
	                selectedMenuItem = (selectedMenuItem - 1 + (int)levels.size()) % (int)levels.size();
	            } else if (key == GLUT_KEY_DOWN) {
	                selectedMenuItem = (selectedMenuItem + 1) % (int)levels.size();
	            }
	            break;
	        case INSTRUCTIONS:
	            if (key == GLUT_KEY_UP) {
	                if (instructionScroll > 0) instructionScroll--;
	            } else if (key == GLUT_KEY_DOWN) {
	                int maxLines = (HEIGHT - 150) / 30;
	                if (instructionScroll < (int)instructionTexts.size() - maxLines) {
	                    instructionScroll++;
	                }
	            }
	            break;
	        case PLAYING:
	            if (key == GLUT_KEY_LEFT) {
	                keys['a'] = true;
	            } else if (key == GLUT_KEY_RIGHT) {
	                keys['d'] = true;
	            } else if (key == GLUT_KEY_UP) {
	                keys['w'] = true;
	            }
	            break;
	        case PAUSED:
	            if (key == GLUT_KEY_UP) {
	                selectedMenuItem = (selectedMenuItem - 1 + 3) % 3;
	            } else if (key == GLUT_KEY_DOWN) {
	                selectedMenuItem = (selectedMenuItem + 1) % 3;
	            }
	            break;
	            
	            case GAME_OVER:
	    if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN) {
	        selectedOption = 1 - selectedOption; // alternar entre 0 (SÍ) y 1 (NO)
	    }
	    break;
	    case GAME_COMPLETE:
        case ABOUT:
            if (key == 27) { // ESC
                currentState = MENU;
                playMenuMusic();
            }
            break;
	    }
	}
	
	void specialUpKeys(int key, int x, int y) {
	    switch (currentState) {
	        case PLAYING:
	            if (key == GLUT_KEY_LEFT) {
	                keys['a'] = false;
	            } else if (key == GLUT_KEY_RIGHT) {
	                keys['d'] = false;
	            } else if (key == GLUT_KEY_UP) {
	                keys['w'] = false;
	            }
	            break;
	    }
	}
	
	void update(int value) {
	    if (currentState == SHOWING_TEXT) {
	        textDisplayTimer += 0.016f;
	        
	        // Avanzar automáticamente después de tiempo
	        if (textDisplayTimer >= TEXT_DISPLAY_TIME) {
	            currentTextIndex++;
	            textDisplayTimer = 0.0f;
	            
	            if (currentTextIndex >= (int)introTexts.size()) {
	                currentState = PLAYING;
	            }
	        }
	    }
	    else if (currentState == PLAYING) {
	        updatePlayer();
	    }
	    
	    glutPostRedisplay();
	    glutTimerFunc(16, update, 0);
	}
	
	
	
	int main(int argc, char** argv) {
	    // Configurar locale para soportar tildes y ñ
	    setSpanishLocale();
	    
	    // Inicializar textos
	    initializeStoryTexts();
	    initializeInstructionTexts();
	    
	    glutInit(&argc, argv);
	    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	    glutInitWindowSize(WIDTH, HEIGHT);
	    
	    // Centrar la ventana
	    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	    glutInitWindowPosition((screenWidth - WIDTH) / 2, (screenHeight - HEIGHT) / 2);
	    
	    glutCreateWindow("Yuls Tales - Red Ball Adventure");
	    
	    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT);
	    glutSwapBuffers();
	    
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluOrtho2D(0, WIDTH, 0, HEIGHT);
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	    
	    srand(time(NULL));
	    
	    // 2. INICIALIZAR SISTEMA DE AUDIO (AÑADIDO AQUÍ)
	    // Inicializar SDL_mixer
	    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
	        cerr << "Error al inicializar SDL audio: " << SDL_GetError() << endl;
	        return -1;
	    }
	
	    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
	        cerr << "Error al inicializar SDL_mixer: " << Mix_GetError() << endl;
	        return -1;
	    }
	
	    // Cargar las músicas
	    menuMusic = Mix_LoadMUS("menuprincipal.mp3");
	    if (!menuMusic) {
	        cerr << "Error al cargar música del menú: " << Mix_GetError() << endl;
	    }
	
	    worldMusic[0] = Mix_LoadMUS("mundo1.mp3");
	    worldMusic[1] = Mix_LoadMUS("mundo2.mp3");
	    worldMusic[2] = Mix_LoadMUS("mundo3.mp3");
	    worldMusic[3] = Mix_LoadMUS("mundo4.mp3");
	    worldMusic[4] = Mix_LoadMUS("mundo5.mp3");
	
	    // Cargar efectos de sonido
	    jumpSound = Mix_LoadWAV("saltar.wav");
	    collectSound = Mix_LoadWAV("moneda.wav");
	    hurtSound = Mix_LoadWAV("dano.wav");
	    winSound = Mix_LoadWAV("win.wav");
        starSound = Mix_LoadWAV("star.wav");
        endingMusic = Mix_LoadMUS("ending.mp3");
	    
	    // REPRODUCIR MÚSICA DEL MENÚ AL INICIAR (AÑADIDO AQUÍ)
        playMenuMusic();
        
   

	    
	    initializeLevels();
	    
	    glutDisplayFunc(display);
	    glutKeyboardFunc(keyboard);
	    glutKeyboardUpFunc(keyboardUp);
	    glutSpecialFunc(specialKeys);
	    glutSpecialUpFunc(specialUpKeys);
	    glutTimerFunc(16, update, 0);
	    
	    glutMainLoop();
	    
	    
	    
	    // Limpieza de audio antes de salir (opcional pero recomendado)
	    Mix_FreeMusic(menuMusic);
	    for (int i = 0; i < 5; i++) {
	        if (worldMusic[i]) Mix_FreeMusic(worldMusic[i]);
	    }
	    Mix_FreeChunk(jumpSound);
	    Mix_FreeChunk(collectSound);
	    Mix_FreeChunk(hurtSound);
	    Mix_FreeChunk(winSound);
        Mix_FreeChunk(starSound);
        Mix_FreeMusic(endingMusic);  
	    SDL_Quit();
	    
	    return 0;
	}

