// entities/Moneda.cpp
#include "Moneda.h"
#include "../utils/ResourceManager.h"
#include <QPainter>

Moneda::Moneda(float x, float y, QWidget* parent)
    : posX(x), posY(y), activa(true), parent(parent) {

    sprite = ResourceManager::getInstance().getSprite("moneda");
}

Moneda::~Moneda() {
}

void Moneda::actualizar(float deltaTime) {
    Q_UNUSED(deltaTime);
    // Las monedas son estáticas, no necesitan actualización física
}

void Moneda::dibujar(QPainter& painter) {
    if (!activa) return;

    QRectF destRect(posX, posY, ANCHO, ALTO);
    painter.drawPixmap(destRect.toRect(), sprite);
}

QRectF Moneda::getHitbox() const {
    return QRectF(posX, posY, ANCHO, ALTO);
}
