// entities/Moneda.h
#ifndef MONEDA_H
#define MONEDA_H

#include <QWidget>
#include <QPixmap>

class Moneda {
public:
    Moneda(float x, float y, QWidget* parent);
    ~Moneda();

    void actualizar(float deltaTime);
    void dibujar(QPainter& painter);

    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    QRectF getHitbox() const;
    bool estaActiva() const { return activa; }
    void desactivar() { activa = false; }

private:
    float posX, posY;
    bool activa;
    QPixmap sprite;
    QWidget* parent;

    static constexpr float ANCHO = 30.0f;
    static constexpr float ALTO = 30.0f;
};

#endif
