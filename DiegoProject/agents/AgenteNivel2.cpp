// agents/AgenteNivel2.cpp
// Implementacion del capataz autonomo para nivel 2
// Ayuda paleando carbon cuando el jugador tiene pocas vidas

#include "AgenteNivel2.h"
#include "../utils/ResourceManager.h"
#include <QDebug>
#include <cstdlib>

AgenteNivel2::AgenteNivel2(QObject *parent)
    : QObject(parent),
    posX(0.0f),
    posY(0.0f),
    activo(false),
    paleando(false),
    framePaleo(0),
    tiempoPaleo(0.0f),
    duracionFramePaleo(0.2f),
    tiempoDesdeUltimoPaleo(0.0f),
    intervaloEntrepaleos(3.0f) {

    qDebug() << "Agente (capataz nivel 2) creado";
}

AgenteNivel2::~AgenteNivel2() {
    qDebug() << "Agente destruido";
}

void AgenteNivel2::setPosicion(float x, float y) {
    posX = x;
    posY = y;
}

void AgenteNivel2::setActivo(bool estado) {
    activo = estado;

    if (activo) {
        qDebug() << "Capataz activado - Comenzara a ayudar";
    } else {
        qDebug() << "Capataz desactivado";
        paleando = false;
        framePaleo = 0;
    }
}

void AgenteNivel2::actualizar(float deltaTime) {
    if (!activo) return;

    tiempoDesdeUltimoPaleo += deltaTime;

    // Actualizar animacion de paleo
    if (paleando) {
        actualizarPaleo(deltaTime);
    } else {
        // Decidir si debe palear
        if (debePalear()) {
            iniciarPaleo();
        }
    }
}

void AgenteNivel2::iniciarPaleo() {
    if (!paleando) {
        paleando = true;
        framePaleo = 0;
        tiempoPaleo = 0.0f;
        tiempoDesdeUltimoPaleo = 0.0f;

        qDebug() << "Capataz inicia paleo";
    }
}

void AgenteNivel2::actualizarPaleo(float deltaTime) {
    if (!paleando) return;

    tiempoPaleo += deltaTime;

    if (tiempoPaleo >= duracionFramePaleo) {
        framePaleo++;
        tiempoPaleo = 0.0f;

        qDebug() << "Capataz frame paleo:" << framePaleo;

        if (framePaleo >= 5) {
            framePaleo = 0;
            paleando = false;

            qDebug() << "Capataz completa paleo";
            emit paleoFinalizado();
        }
    }
}

void AgenteNivel2::dibujar(QPainter& painter) {
    if (!activo) return;

    QPixmap sprite;

    if (paleando && framePaleo >= 0 && framePaleo < 5) {
        sprite = spritesPaleo[framePaleo];
    } else {
        sprite = spritesPaleo[0];
    }

    if (!sprite.isNull()) {
        painter.drawPixmap(posX - 25, posY - 25, 50, 50, sprite);
    } else {
        // Fallback: circulo marron oscuro
        painter.setBrush(QColor(101, 67, 33));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(QPointF(posX, posY), 20, 20);

        // Texto
        painter.setFont(QFont("Arial", 8));
        painter.setPen(Qt::white);
        painter.drawText(posX - 20, posY + 35, "CAPATAZ");
    }
}

void AgenteNivel2::cargarSpritesPaleo() {
    for (int i = 0; i < 5; i++) {
        QString spriteKey = QString("capataz_paleo_%1").arg(i);
        spritesPaleo[i] = ResourceManager::getInstance().getSprite(spriteKey);
    }

    qDebug() << "Sprites de capataz cargados";
}

bool AgenteNivel2::debePalear() {
    // Palear cada cierto intervalo
    if (tiempoDesdeUltimoPaleo < intervaloEntrepaleos) {
        return false;
    }

    // 50% de probabilidad de palear
    int aleatorio = rand() % 100;
    return aleatorio < 50;
}
