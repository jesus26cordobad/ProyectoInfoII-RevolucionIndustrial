// agents/Ladron.h
#ifndef LADRON_H
#define LADRON_H

#include <QWidget>
#include <QPixmap>

class Ladron {
public:
    Ladron(QWidget* parent);
    ~Ladron();

    void actualizar(float deltaTime, float jugadorX, float jugadorY);
    void dibujar(QPainter& painter);

    void setPosicion(float x, float y);
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    QRectF getHitbox() const;

    float getVelocidad() const { return velocidadX; }
    void setVelocidad(float vel) { velocidadX = vel; }

    // Física
    bool estaEnAire() const { return enAire; }
    void setEnAire(bool valor) { enAire = valor; }
    float getVelocidadY() const { return velocidadY; }
    void setVelocidadY(float vel) { velocidadY = vel; }
    void aplicarGravedad(float deltaTime);
    void saltar();

    void aumentarVelocidad(float incremento);
    void cargarSprites(const QPixmap& corrDer, const QPixmap& corrIzq,
                       const QPixmap& saltoDer, const QPixmap& saltoIzq);
    int getFrameActual() const { return frameActual; }

private:
    float posX, posY;
    float velocidadX, velocidadY;
    float velocidadBase;
    bool mirandoDerecha;
    bool enAire;

    QPixmap spriteCorrDer, spriteCorrIzq;
    QPixmap spriteSaltoDer, spriteSaltoIzq;

    int frameActual;
    float tiempoFrame;

    QWidget* parent;

    static constexpr float ANCHO = 90.0f;
    static constexpr float ALTO = 150.0f;
    static constexpr float VELOCIDAD_INICIAL = 100.0f;
    static constexpr float VELOCIDAD_ANIMACION = 0.15f;
    static constexpr int TOTAL_FRAMES_CORRER = 6;
    static constexpr int TOTAL_FRAMES_SALTO = 6;
    static constexpr float GRAVEDAD = 800.0f;
    static constexpr float FUERZA_SALTO = -500.0f;
    static constexpr float PROBABILIDAD_SALTO = 0.01f;
};

#endif
