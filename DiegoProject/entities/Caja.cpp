// entities/Caja.cpp
// Implementacion de colision perfectamente inelastica
// Las cajas se pegan al chocar y se mueven juntas como un solo cuerpo
// Ecuaciones: M = m1 + m2, v' = (m1*v1 + m2*v2) / M

#include "Caja.h"
#include "../utils/ResourceManager.h"
#include <QDebug>
#include <cmath>

Caja::Caja(int numero, float x, float y, QObject *parent)
    : QObject(parent),
    numero(numero),
    posX(x),
    posY(y),
    velocidadX(0.0f),
    masa(1.0f),
    tamaño(40.0f),
    enMovimiento(false),
    pegada(false),
    cajaPegada(nullptr) {

    qDebug() << "Caja" << numero << "creada en posicion (" << posX << "," << posY << ")";
}

Caja::~Caja() {
    qDebug() << "Caja" << numero << "destruida";
}

void Caja::setPosicion(float x, float y) {
    posX = x;
    posY = y;
}

void Caja::setVelocidad(float vx) {
    velocidadX = vx;
    enMovimiento = (std::abs(velocidadX) > VELOCIDAD_MINIMA);
}

void Caja::setPegada(Caja* otra) {
    pegada = true;
    cajaPegada = otra;
}

void Caja::actualizar(float deltaTime) {
    if (!enMovimiento) return;

    // Calcular desplazamiento
    float deltaX = velocidadX * deltaTime;

    // Mover esta caja
    posX += deltaX;

    // Si esta pegada, mover la otra caja tambien
    if (pegada && cajaPegada) {
        moverCajasPegadas(deltaX);
    }

    // Aplicar friccion
    aplicarFriccion();

    // Verificar limites del area
    verificarLimites();

    // Detener si velocidad es muy baja
    if (std::abs(velocidadX) < VELOCIDAD_MINIMA) {
        detener();
    }
}

void Caja::dibujar(QPainter& painter) {
    // Obtener sprite segun numero de caja
    QString spriteKey = (numero == 1) ? "caja1" : "caja2";
    QPixmap sprite = ResourceManager::getInstance().getSprite(spriteKey);

    if (!sprite.isNull()) {
        painter.drawPixmap(posX - tamaño/2, posY - tamaño/2,
                           tamaño, tamaño, sprite);
    } else {
        // Fallback: dibujar rectangulo
        QColor colorCaja = (numero == 1) ? QColor(139, 69, 19) : QColor(160, 82, 45);
        painter.setBrush(colorCaja);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(posX - tamaño/2, posY - tamaño/2, tamaño, tamaño);

        // Numero de la caja
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.setPen(Qt::white);
        painter.drawText(posX - 8, posY + 8, QString::number(numero));
    }

    // Indicador de movimiento
    if (enMovimiento) {
        painter.setPen(QPen(Qt::yellow, 3));
        painter.drawRect(posX - tamaño/2 - 2, posY - tamaño/2 - 2,
                         tamaño + 4, tamaño + 4);

        // Flecha de direccion
        int flechaX = (velocidadX > 0) ? posX + tamaño/2 + 15 : posX - tamaño/2 - 15;
        painter.setPen(QPen(Qt::yellow, 2));
        painter.drawLine(posX, posY, flechaX, posY);

        if (velocidadX > 0) {
            painter.drawLine(flechaX, posY, flechaX - 5, posY - 5);
            painter.drawLine(flechaX, posY, flechaX - 5, posY + 5);
        } else {
            painter.drawLine(flechaX, posY, flechaX + 5, posY - 5);
            painter.drawLine(flechaX, posY, flechaX + 5, posY + 5);
        }
    }

    // Indicador si esta pegada
    if (pegada) {
        painter.setFont(QFont("Arial", 8));
        painter.setPen(Qt::red);
        painter.drawText(posX - 15, posY - tamaño/2 - 5, "PEGADA");
    }
}

void Caja::empujar(float direccionX) {
    if (enMovimiento) {
        qDebug() << "Caja" << numero << "ya esta en movimiento";
        return;
    }

    // Aplicar impulso
    velocidadX = direccionX * IMPULSO_EMPUJE;
    enMovimiento = true;

    qDebug() << "Caja" << numero << "empujada con velocidad:" << velocidadX;
}

void Caja::pegarseA(Caja* otra) {
    if (!otra || otra == this) return;

    qDebug() << "======= COLISION PERFECTAMENTE INELASTICA =======";
    qDebug() << "Caja" << numero << ": masa=" << masa << "vx=" << velocidadX;
    qDebug() << "Caja" << otra->getNumero() << ": masa=" << otra->getMasa()
             << "vx=" << otra->getVelocidadX();

    // Guardar valores antes de la colision
    float m1 = masa;
    float m2 = otra->getMasa();
    float v1 = velocidadX;
    float v2 = otra->getVelocidadX();

    // ECUACION: M = m1 + m2 (masa total del sistema)
    float masaTotal = m1 + m2;

    // ECUACION: v' = (m1*v1 + m2*v2) / M (conservacion del momento)
    float velocidadFinal = (m1 * v1 + m2 * v2) / masaTotal;

    // Actualizar masa de ambas cajas (ahora actuan como un sistema)
    masa = masaTotal;
    otra->masa = masaTotal;

    // Actualizar velocidad de ambas cajas (se mueven juntas)
    velocidadX = velocidadFinal;
    otra->setVelocidad(velocidadFinal);

    // Marcar como pegadas
    pegada = true;
    cajaPegada = otra;
    otra->setPegada(this);

    // Ambas cajas siguen en movimiento si la velocidad es suficiente
    enMovimiento = (std::abs(velocidadFinal) > VELOCIDAD_MINIMA);
    otra->enMovimiento = enMovimiento;

    qDebug() << "Resultado colision:";
    qDebug() << "  Masa total sistema: M =" << masaTotal << "kg";
    qDebug() << "  Velocidad final: v' =" << velocidadFinal << "px/s";
    qDebug() << "  Cajas ahora pegadas y moviendose juntas";
    qDebug() << "=================================================";
}

void Caja::detener() {
    velocidadX = 0.0f;
    enMovimiento = false;

    // Detener tambien la caja pegada
    if (pegada && cajaPegada) {
        cajaPegada->velocidadX = 0.0f;
        cajaPegada->enMovimiento = false;
    }

    qDebug() << "Caja" << numero << "detenida en posicion" << posX;
}

bool Caja::colisionaCon(const Caja* otra) const {
    if (!otra || otra == this) return false;
    if (pegada) return false;  // Ya estan pegadas

    // Calcular distancia horizontal
    float dx = std::abs(posX - otra->getPosX());
    float dy = std::abs(posY - otra->getPosY());

    // Radio de colision
    float radioColision = tamaño;

    // Colision si estan cerca
    bool colisionX = dx < radioColision;
    bool colisionY = dy < 10.0f;

    return colisionX && colisionY;
}

void Caja::aplicarFriccion() {
    velocidadX *= FRICCION;
}

void Caja::verificarLimites() {
    if (posX - tamaño/2 < AREA_MIN_X) {
        posX = AREA_MIN_X + tamaño/2;
        detener();
        qDebug() << "Caja" << numero << "choco con limite izquierdo";
    }

    if (posX + tamaño/2 > AREA_MAX_X) {
        posX = AREA_MAX_X - tamaño/2;
        detener();
        qDebug() << "Caja" << numero << "choco con limite derecho";
    }
}

void Caja::moverCajasPegadas(float deltaX) {
    if (cajaPegada) {
        cajaPegada->posX += deltaX;
    }
}
