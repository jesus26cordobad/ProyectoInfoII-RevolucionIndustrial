// entities/Carrete.cpp
// parábola con sprites: normal → girado → destruido

#include "Carrete.h"
#include "../utils/ResourceManager.h"
#include <QDebug>
#include <cmath>

Carrete::Carrete(QString color, float velocidad, QObject *parent)
    : QObject(parent),
    color(color),
    posX(0.0f),
    posY(0.0f),
    velocidad(velocidad),
    cayendo(false),
    tiempoCayendo(0.0f),
    destruido(false),
    girado(false),
    enParabola(false),
    tocoPisoParabola(false),
    velocidadVertical(0.0f),
    velocidadHorizontal(0.0f),
    maxAlturaParabola(1000.0f),
    tiempoDesvanecimiento(0.0f),
    opacidad(255) {

    qDebug() << "Carrete creado - Color:" << color;
}

Carrete::~Carrete() {
    qDebug() << "Carrete destruido - Color:" << color;
}

void Carrete::setPosicion(float x, float y) {
    posX = x;
    posY = y;
}

void Carrete::actualizar(float deltaTime) {
    if (destruido) {
        // Desvanecer gradualmente
        tiempoDesvanecimiento += deltaTime;
        opacidad = std::max(0, 255 - static_cast<int>(tiempoDesvanecimiento * 200));
        return;
    }

    if (cayendo) {
        aplicarGravedad(deltaTime);
        tiempoCayendo += deltaTime;
    } else {
        moverEnBanda(deltaTime);
    }
}

void Carrete::moverEnBanda(float deltaTime) {
    posX -= velocidad * deltaTime;
}

void Carrete::aplicarGravedad(float deltaTime) {
    if (enParabola) {
        // MOVIMIENTO PARABÓLICO
        velocidadVertical += GRAVEDAD * deltaTime;
        posY += velocidadVertical * deltaTime;
        posX += velocidadHorizontal * deltaTime;

        // Detectar punto máximo de la parábola para cambiar a sprite girado
        if (velocidadVertical > 0 && !girado) {
            // Pasó el punto máximo (empezó a caer), cambiar a girado
            girado = true;
            qDebug() << "Carrete girado en punto máximo Y=" << posY;
        }
    } else {
        // Caída vertical normal
        posY += 200.0f * deltaTime;
    }
}

void Carrete::iniciarParabola() {
    enParabola = true;
    velocidadVertical = VELOCIDAD_REBOTE;
    velocidadHorizontal = VELOCIDAD_HORIZONTAL;
    girado = false;
    maxAlturaParabola = posY;
    qDebug() << "Iniciando parábola desde Y=" << posY;
}

void Carrete::destruir() {
    destruido = true;
    enParabola = false;
    velocidadVertical = 0.0f;
    velocidadHorizontal = 0.0f;
    tiempoDesvanecimiento = 0.0f;
    qDebug() << "Carrete destruido - iniciando desvanecimiento";
}

void Carrete::empezarDesvanecimiento() {
    tiempoDesvanecimiento = 0.0f;
}

void Carrete::dibujar(QPainter& painter) {
    QString spriteKey;

    // SELECCIÓN DE SPRITE SEGÚN ESTADO
    if (destruido) {
        spriteKey = "carrete_" + color + "_destruido";
    } else if (girado && enParabola) {
        spriteKey = "carrete_" + color + "_girado";
    } else {
        spriteKey = "carrete_" + color;
    }

    QPixmap sprite = ResourceManager::getInstance().getSprite(spriteKey);

    // Aplicar opacidad si está desvaneciendo
    if (destruido && opacidad < 255) {
        painter.setOpacity(opacidad / 255.0f);
    }

    if (!sprite.isNull()) {
        painter.drawPixmap(posX - 15, posY - 15,
                           sprite.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // Fallback
        QColor colorCarrete;
        if (color == "rojo") colorCarrete = destruido ? QColor(100, 0, 0) : Qt::red;
        else if (color == "azul") colorCarrete = destruido ? QColor(0, 0, 100) : Qt::blue;
        else if (color == "verde") colorCarrete = destruido ? QColor(0, 100, 0) : Qt::green;
        else colorCarrete = Qt::gray;

        painter.setBrush(colorCarrete);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(posX, posY), 15, 15);

        if (destruido) {
            painter.setPen(QPen(Qt::white, 2));
            painter.drawLine(posX - 10, posY - 10, posX + 10, posY + 10);
            painter.drawLine(posX + 10, posY - 10, posX - 10, posY + 10);
        }
    }

    // Restaurar opacidad
    if (destruido && opacidad < 255) {
        painter.setOpacity(1.0f);
    }

    // SOMBRA SIEMPRE VISIBLE (hasta que se desvanece)
    if (cayendo && opacidad > 50) {
        painter.setOpacity(0.4f);
        painter.setBrush(QColor(0, 0, 0, 100));
        painter.setPen(Qt::NoPen);

        // Sombra en el suelo (Y=625)
        float distanciaAlSuelo = 625.0f - posY;
        if (distanciaAlSuelo > 0 && distanciaAlSuelo < 200) {
            // Sombra más grande cuando está más cerca
            float tamañoSombra = 15.0f - (distanciaAlSuelo / 200.0f) * 10.0f;
            painter.drawEllipse(QPointF(posX, 625.0f), tamañoSombra, 5);
        }
        painter.setOpacity(1.0f);
    }
}
