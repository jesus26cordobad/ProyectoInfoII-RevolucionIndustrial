// entities/Jugador.cpp
// Implementación completa del jugador con movimiento, animación y dibujo

#include "Jugador.h"
#include "../utils/ResourceManager.h"
#include <QDebug>
#include <cmath>

Jugador::Jugador(QObject *parent)
    : QObject(parent),
    puntaje(0),
    vidas(5),
    posX(0.0f),
    posY(0.0f),
    velocidad(200.0f),
    direccion(FRENTE),
    frameAnimacion(0),
    tiempoAnimacion(0.0f),
    velocidadAnimacion(10.0f),
    estaQuieto(true) {  // distinguir quieto/caminando

    qDebug() << "Jugador creado - Vidas:" << vidas << "Puntaje:" << puntaje;
}

Jugador::~Jugador() {
    qDebug() << "Jugador destruido";
}


// POSICIÓN Y MOVIMIENTO
void Jugador::setPosicion(float x, float y) {
    posX = x;
    posY = y;
    emit posicionCambiada(x, y);
}

void Jugador::mover(float deltaX, float deltaY) {
    posX += deltaX;
    posY += deltaY;
    emit posicionCambiada(posX, posY);
}

void Jugador::actualizarMovimiento(bool teclaW, bool teclaS, bool teclaA, bool teclaD, float deltaTime) {
    float dx = 0.0f;
    float dy = 0.0f;

    if (teclaW) dy -= velocidad * deltaTime;
    if (teclaS) dy += velocidad * deltaTime;
    if (teclaA) dx -= velocidad * deltaTime;
    if (teclaD) dx += velocidad * deltaTime;

    // Actualizar posición
    if (dx != 0.0f || dy != 0.0f) {
        mover(dx, dy);
        estaQuieto = false;

        // Determinar dirección visual
        if (teclaW) direccion = ESPALDA;
        else if (teclaS) direccion = FRENTE;
        else if (teclaA) direccion = IZQUIERDA;
        else if (teclaD) direccion = DERECHA;
    } else {
        estaQuieto = true;
        // Resetear al frame de "quieto" cuando se detiene
        frameAnimacion = 0;
    }
}

// ANIMACIÓN
void Jugador::actualizarAnimacion(float deltaTime) {
    // Solo animar si está caminando
    if (estaQuieto) {
        frameAnimacion = 0;  // Frame de quieto
        return;
    }

    tiempoAnimacion += deltaTime;

    // Cambiar frame cada 0.15 segundos (más lento = más natural)
    if (tiempoAnimacion >= 0.15f) {
        cambiarFrame();
        tiempoAnimacion = 0.0f;
    }
}

void Jugador::cambiarFrame() {
    if (direccion == FRENTE || direccion == ESPALDA) {
        // Intercalar entre frame 1 y 2
        if (frameAnimacion == 1) {
            frameAnimacion = 2;
        } else {
            frameAnimacion = 1;
        }
    } else {  // IZQUIERDA o DERECHA
        // Intercalar entre frame 0 y 1
        frameAnimacion = (frameAnimacion == 0) ? 1 : 0;
    }
}

// SPRITES Y DIBUJO
void Jugador::cargarSprites(const QString& spriteFrente, const QString& spriteEspalda,
                            const QString& spriteIzquierda, const QString& spriteDerecha) {
    this->spriteFrente = ResourceManager::getInstance().getSprite(spriteFrente);
    this->spriteEspalda = ResourceManager::getInstance().getSprite(spriteEspalda);
    this->spriteIzquierda = ResourceManager::getInstance().getSprite(spriteIzquierda);
    this->spriteDerecha = ResourceManager::getInstance().getSprite(spriteDerecha);

    qDebug() << "Sprites del jugador cargados";
}

QPixmap Jugador::getSpriteActual() const {
    switch (direccion) {
    case FRENTE:    return spriteFrente;
    case ESPALDA:   return spriteEspalda;
    case IZQUIERDA: return spriteIzquierda;
    case DERECHA:   return spriteDerecha;
    default:        return spriteFrente;
    }
}

void Jugador::dibujar(QPainter& painter) {
    QPixmap sprite = getSpriteActual();

    if (!sprite.isNull()) {
        int numFrames = 3;  // Por defecto 3 frames

        // Laterales solo tienen 2 frames
        if (direccion == IZQUIERDA || direccion == DERECHA) {
            numFrames = 2;
        }

        // Calcular frame dentro del sprite sheet
        int frameWidth = sprite.width() / numFrames;
        int frameHeight = sprite.height();

        // Recortar el frame correcto
        QRect sourceRect(frameAnimacion * frameWidth, 0, frameWidth, frameHeight);
        QRect targetRect(posX - 30, posY - 40, 60, 80);

        painter.drawPixmap(targetRect, sprite, sourceRect);
    } else {
        // Fallback: dibujar círculo
        painter.setBrush(QColor(255, 200, 100));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(posX, posY), 30, 40);
    }
}

// PUNTAJE Y VIDAS
void Jugador::agregarPuntaje(int puntos) {
    puntaje += puntos;
    emit puntajeModificado(puntaje);
    qDebug() << "Puntaje actualizado:" << puntaje;
}

void Jugador::perderVida() {
    if (vidas > 0) {
        vidas--;
        emit vidasModificadas(vidas);
        qDebug() << "Vida perdida. Vidas restantes:" << vidas;

        if (vidas <= 0) {
            emit jugadorMuerto();
            qDebug() << "¡Jugador sin vidas!";
        }
    }
}

void Jugador::ganarVida() {
    vidas++;
    emit vidasModificadas(vidas);
    qDebug() << "Vida ganada. Vidas totales:" << vidas;
}

void Jugador::reiniciar() {
    puntaje = 0;
    vidas = 5;
    posX = 0.0f;
    posY = 0.0f;
    direccion = FRENTE;
    frameAnimacion = 0;
    tiempoAnimacion = 0.0f;
    estaQuieto = true;

    emit puntajeModificado(puntaje);
    emit vidasModificadas(vidas);

    qDebug() << "Jugador reiniciado";
}
