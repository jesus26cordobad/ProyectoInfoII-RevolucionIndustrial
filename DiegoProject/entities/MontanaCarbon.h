// entities/MontanaCarbon.h
#ifndef MONTANACARBON_H
#define MONTANACARBON_H

#include <QObject>
#include <QPixmap>
#include <QPainter>
#include <QVector>

class MontanaCarbon : public QObject {
    Q_OBJECT

public:
    explicit MontanaCarbon(float x, float y, QObject *parent = nullptr);
    ~MontanaCarbon();

    // Getters
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    int getFrameActual() const { return frameActual; }
    int getCantidadRestante() const { return cantidadRestante; }
    bool estaVacia() const { return cantidadRestante <= 0; }

    // Metodos
    void cargarSprites(const QPixmap& spriteSheet);
    void disminuir();
    void reiniciar();
    void dibujar(QPainter& painter);

    // Area clickeable
    bool contienePunto(float x, float y) const;

signals:
    void carbonAgotado();
    void cantidadCambiada(int cantidad);

private:
    void extraerFrames(const QPixmap& spriteSheet);

    float posX, posY;
    int frameActual;
    int cantidadRestante;
    QVector<QPixmap> frames;
    static constexpr int TOTAL_FRAMES = 10;
    static constexpr int CANTIDAD_INICIAL = 100;
    static constexpr int ANCHO_CLICK = 150;
    static constexpr int ALTO_CLICK = 150;
};

#endif // MONTANACARBON_H
