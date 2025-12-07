// agents/Ladron.cpp
#include "Ladron.h"
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <random>

Ladron::Ladron(QWidget* parent)
    : posX(100), posY(300),
    velocidadX(0), velocidadY(0),
    velocidadBase(VELOCIDAD_INICIAL),
    mirandoDerecha(true),
    enAire(false),
    frameActual(0),
    tiempoFrame(0.0f),
    parent(parent) {
}

Ladron::~Ladron() {
}

void Ladron::cargarSprites(const QPixmap& corrDer, const QPixmap& corrIzq,
                           const QPixmap& saltoDer, const QPixmap& saltoIzq) {
    spriteCorrDer = corrDer;
    spriteCorrIzq = corrIzq;
    spriteSaltoDer = saltoDer;
    spriteSaltoIzq = saltoIzq;
}

void Ladron::setPosicion(float x, float y) {
    posX = x;
    posY = y;
}

void Ladron::actualizar(float deltaTime, float jugadorX, float jugadorY) {
    float dx = jugadorX - posX;
    float dy = jugadorY - posY;
    float distancia = std::sqrt(dx * dx + dy * dy);
    float distanciaHorizontal = std::abs(dx);
    float distanciaVertical = dy;

    if (distanciaHorizontal > 30.0f && !enAire) {
        if (dx > 0) {
            velocidadX = velocidadBase;
            mirandoDerecha = true;
        } else {
            velocidadX = -velocidadBase;
            mirandoDerecha = false;
        }
    } else if (!enAire) {
        velocidadX = 0;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);

    if (!enAire && distancia < 400.0f) {
        bool deberSaltar = false;

        if (distanciaVertical < -80.0f && distanciaHorizontal < 250.0f) {
            // Jugador está arriba y relativamente cerca horizontalmente
            deberSaltar = true;
            qDebug() << "Ladrón salta hacia jugador arriba";
        }

        else if (distanciaVertical > -50.0f && distanciaVertical < 50.0f &&
                 distanciaHorizontal < 150.0f) {
            // Salto aleatorio para agregar dinamismo
            if (dis(gen) < 0.03f) {
                deberSaltar = true;
                qDebug() << "Ladrón salta aleatoriamente";
            }
        }

        else if (distanciaVertical > 100.0f && distanciaHorizontal < 100.0f) {
            // Saltar hacia adelante para caer de la viga
            if (dis(gen) < 0.05f) {
                deberSaltar = true;
                qDebug() << "Ladrón salta para bajar de viga";
            }
        }

        if (deberSaltar) {
            saltar();
        }
    }

    posX += velocidadX * deltaTime;

    // Límites de pantalla
    if (posX < 0) posX = 0;
    if (posX > parent->width() - ANCHO) posX = parent->width() - ANCHO;

    if (std::abs(velocidadX) > 10.0f && !enAire) {
        tiempoFrame += deltaTime;
        if (tiempoFrame >= VELOCIDAD_ANIMACION) {
            tiempoFrame = 0.0f;
            frameActual = (frameActual + 1) % 6;
        }
    } else if (!enAire) {
        frameActual = 0;
        tiempoFrame = 0.0f;
    }
}

void Ladron::aplicarGravedad(float deltaTime) {
    if (enAire) {
        velocidadY += GRAVEDAD * deltaTime;
        posY += velocidadY * deltaTime;

        if (velocidadY > 800.0f) {
            velocidadY = 800.0f;
        }
    } else {
        velocidadY = 0.0f;
    }
}

void Ladron::saltar() {
    if (!enAire) {
        velocidadY = FUERZA_SALTO;
        enAire = true;
        qDebug() << "Ladrón salta";
    }
}

void Ladron::aumentarVelocidad(float incremento) {
    velocidadBase += incremento;
    qDebug() << "Ladrón más rápido:" << velocidadBase;
}

void Ladron::dibujar(QPainter& painter) {
    QPixmap spriteActual;
    int frameAUsar = 0;
    int totalFrames = 6;

    if (enAire && std::abs(velocidadY) > 50.0f) {
        spriteActual = mirandoDerecha ? spriteSaltoDer : spriteSaltoIzq;
        totalFrames = 6;

        if (velocidadY < -200) {
            frameAUsar = 1;
        } else if (velocidadY < 0) {
            frameAUsar = 2;
        } else if (velocidadY < 200) {
            frameAUsar = 3;
        } else {
            frameAUsar = 4;
        }
    } else if (std::abs(velocidadX) > 10.0f && !enAire) {
        // Corriendo en el suelo
        spriteActual = mirandoDerecha ? spriteCorrDer : spriteCorrIzq;
        totalFrames = 6;
        frameAUsar = frameActual;
    } else {
        // Quieto
        spriteActual = mirandoDerecha ? spriteCorrDer : spriteCorrIzq;
        totalFrames = 6;
        frameAUsar = 0;
    }

    if (spriteActual.isNull()) {
        qDebug() << "ERROR: Sprite del ladrón es NULL";
        return;
    }

    int anchoFrame = spriteActual.width() / totalFrames;
    QRect sourceRect(frameAUsar * anchoFrame, 0, anchoFrame, spriteActual.height());
    QRectF destRect(posX, posY, ANCHO, ALTO);

    painter.drawPixmap(destRect.toRect(), spriteActual, sourceRect);
}

QRectF Ladron::getHitbox() const {
    return QRectF(posX + 10, posY + 10, ANCHO - 20, ALTO - 20);
}
