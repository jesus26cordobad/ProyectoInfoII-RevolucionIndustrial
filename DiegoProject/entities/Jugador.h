// entities/Jugador.h
// Representa al jugador con movimiento, animación y estados

#ifndef JUGADOR_H
#define JUGADOR_H

#include <QObject>
#include <QPixmap>
#include <QPainter>
#include <QString>

class Jugador : public QObject {
    Q_OBJECT

public:
    enum Direccion {
        FRENTE,    // Sur (↓)
        ESPALDA,   // Norte (↑)
        IZQUIERDA, // Oeste (←)
        DERECHA    // Este (→)
    };

    explicit Jugador(QObject *parent = nullptr);
    ~Jugador();

    // Getters básicos
    int getPuntaje() const { return puntaje; }
    int getVidas() const { return vidas; }
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    Direccion getDireccion() const { return direccion; }
    int getFrame() const { return frameAnimacion; }

    // Setters
    void setPosicion(float x, float y);
    void setVelocidad(float vel) { velocidad = vel; }
    void agregarPuntaje(int puntos);

    // Métodos de juego
    void perderVida();
    void ganarVida();
    void reiniciar();
    bool estaVivo() const { return vidas > 0; }

    // Movimiento (refactorizado desde NivelFabrica)
    void mover(float deltaX, float deltaY);
    void actualizarMovimiento(bool teclaW, bool teclaS, bool teclaA, bool teclaD, float deltaTime);

    // Animación
    void actualizarAnimacion(float deltaTime);
    void setDireccion(Direccion dir) { direccion = dir; }

    // Dibujo
    void dibujar(QPainter& painter);

    // Sprites
    void cargarSprites(const QString& spriteFrente, const QString& spriteEspalda,
                       const QString& spriteIzquierda, const QString& spriteDerecha);

signals:
    void puntajeModificado(int nuevoPuntaje);
    void vidasModificadas(int nuevasVidas);
    void jugadorMuerto();
    void posicionCambiada(float x, float y);

private:
    // Estado básico
    int puntaje;
    int vidas;
    float posX;
    float posY;

    // Movimiento
    float velocidad;
    Direccion direccion;

    // Animación
    int frameAnimacion;      // 0 = quieto, 1-2 = caminando
    float tiempoAnimacion;
    float velocidadAnimacion; // Frames por segundo
    bool estaQuieto;         // distinguir quieto/caminando

    // Sprites
    QPixmap spriteFrente;
    QPixmap spriteEspalda;
    QPixmap spriteIzquierda;
    QPixmap spriteDerecha;

    // Métodos privados
    void cambiarFrame();
    QPixmap getSpriteActual() const;
};

#endif // JUGADOR_H
