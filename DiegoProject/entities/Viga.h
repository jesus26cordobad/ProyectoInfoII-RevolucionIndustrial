// entities/Viga.h
#ifndef VIGA_H
#define VIGA_H

#include <QWidget>
#include <QPixmap>
#include <QRectF>

class Viga {
public:
    enum TipoViga {
        IZQUIERDA,  // viga4
        CENTRO,     // viga1
        DERECHA,    // viga3
        SUELO       // ✅ NUEVO - plataforma suelo
    };

    Viga(float x, float y, TipoViga tipo, QWidget* parent);
    ~Viga();

    void dibujar(QPainter& painter);
    QRectF getHitbox() const;

    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    float getAncho() const { return ancho; }
    float getAlto() const { return alto; }

private:
    float posX, posY;
    float ancho, alto;
    TipoViga tipo;
    QPixmap sprite;
    QWidget* parent;
};

#endif
