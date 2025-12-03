// entities/Carrete.h
// Carrete con parábola, sprites girado y destruido

#ifndef CARRETE_H
#define CARRETE_H

#include <QObject>
#include <QString>
#include <QPainter>

class Carrete : public QObject {
    Q_OBJECT

public:
    explicit Carrete(QString color, float velocidad, QObject *parent = nullptr);
    ~Carrete();

    // Getters
    QString getColor() const { return color; }
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    float getVelocidad() const { return velocidad; }
    bool estaCayendo() const { return cayendo; }
    float getTiempoCayendo() const { return tiempoCayendo; }
    bool estaDestruido() const { return destruido; }
    bool estaGirado() const { return girado; }

    // Setters
    void setPosicion(float x, float y);
    void setVelocidad(float v) { velocidad = v; }
    void setCayendo(bool c) { cayendo = c; }
    void setTiempoCayendo(float t) { tiempoCayendo = t; }

    // Actualización
    void actualizar(float deltaTime);
    void moverEnBanda(float deltaTime);
    void aplicarGravedad(float deltaTime);
    void iniciarParabola();  // Iniciar el rebote parabólico
    void destruir();          // Marcar como destruido
    void empezarDesvanecimiento(); // Empezar a desvanecerse

    // Dibujo
    void dibujar(QPainter& painter);

signals:
    void llegaAlDestino();
    void carretePerdido();

private:
    QString color;
    float posX;
    float posY;
    float velocidad;
    bool cayendo;
    float tiempoCayendo;

    // SISTEMA DE UNA PARÁBOLA
    bool destruido;
    bool girado;              // Si está en sprite girado
    bool enParabola;          // Si está haciendo la parábola
    bool tocoPisoParabola;    // Si ya tocó el piso después de la parábola
    float velocidadVertical;  // Para movimiento vertical
    float velocidadHorizontal; // Para movimiento horizontal
    float maxAlturaParabola;  // Y máxima alcanzada (para detectar cambio a girado)
    float tiempoDesvanecimiento; // Tiempo para desvanecer sprite destruido
    int opacidad;             // Opacidad del sprite destruido

    // Constantes de física
    static constexpr float GRAVEDAD = 300.0f;  // px/s²
    static constexpr float VELOCIDAD_REBOTE = -200.0f;  // Velocidad vertical inicial
    static constexpr float VELOCIDAD_HORIZONTAL = -50.0f; // Velocidad hacia izquierda
};

#endif // CARRETE_H
