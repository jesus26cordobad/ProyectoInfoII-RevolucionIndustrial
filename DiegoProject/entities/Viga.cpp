// entities/Viga.cpp
#include "Viga.h"
#include "../utils/ResourceManager.h"
#include <QPainter>

Viga::Viga(float x, float y, TipoViga tipo, QWidget* parent)
    : posX(x), posY(y), tipo(tipo), parent(parent) {

    // Cargar sprite según tipo
    switch(tipo) {
    case IZQUIERDA:
        sprite = ResourceManager::getInstance().getSprite("viga4");
        ancho = 400.0f;
        alto = 60.0f;
        break;
    case CENTRO:
        sprite = ResourceManager::getInstance().getSprite("viga1");
        ancho = 400.0f;
        alto = 60.0f;
        break;
    case DERECHA:
        sprite = ResourceManager::getInstance().getSprite("viga3");
        ancho = 400.0f;
        alto = 60.0f;
        break;
    case SUELO:  // ✅ NUEVO
        // No cargar sprite (será invisible o color sólido)
        ancho = 2000.0f;  // Muy ancho para cubrir toda la pantalla
        alto = 100.0f;
        break;
    }
}

Viga::~Viga() {
}

void Viga::dibujar(QPainter& painter) {
    if (tipo == SUELO) {
        // ✅ Dibujar suelo con color sólido (marrón oscuro)
        painter.fillRect(posX, posY, ancho, alto, QColor(80, 60, 40));
        return;
    }

    // Vigas normales con sprite
    QRectF destRect(posX, posY, ancho, alto);
    painter.drawPixmap(destRect.toRect(), sprite);
}

QRectF Viga::getHitbox() const {
    // La hitbox es solo la parte superior de la viga (para pararse)
    return QRectF(posX, posY, ancho, 10.0f);
}
