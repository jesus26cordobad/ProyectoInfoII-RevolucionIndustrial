// entities/Tren.cpp
// Implementación del Tren

#include "Tren.h"
#include <QDebug>

Tren::Tren(QObject *parent)
    : QObject(parent),
    velocidad(0.0f),
    posicion(0.0f),
    carbon(100.0f),
    cargamentos(0),
    boostActivo(false) {

    qDebug() << "Tren creado";
}

Tren::~Tren() {
    qDebug() << "Tren destruido";
}

void Tren::actualizar(float deltaTime) {
    // Mover tren
    posicion += velocidad * deltaTime;

    // Consumir carbón
    float consumo = (boostActivo ? 0.5f : 0.1f) * deltaTime;
    carbon -= consumo;

    if (carbon <= 0.0f) {
        carbon = 0.0f;
        velocidad = 0.0f;
        emit carbonAgotado();
    }

    boostActivo = false; // Boost dura un frame
}

void Tren::acelerar(float incremento) {
    velocidad += incremento;
    if (velocidad > 150.0f) {
        velocidad = 150.0f;
    }
}

void Tren::frenar(float decremento) {
    velocidad -= decremento;
    if (velocidad < 0.0f) {
        velocidad = 0.0f;
    }
}

void Tren::usarBoost() {
    if (carbon > 10.0f) {
        velocidad += 30.0f;
        boostActivo = true;
        qDebug() << "Boost activado";
    }
}

void Tren::cargarMercancia() {
    cargamentos++;
    emit cargamentoRecogido();
    qDebug() << "Cargamento recogido - Total:" << cargamentos;
}

void Tren::recogerCarbon(float cantidad) {
    carbon += cantidad;
    if (carbon > 100.0f) {
        carbon = 100.0f;
    }
    qDebug() << "Carbón recogido - Total:" << carbon;
}
