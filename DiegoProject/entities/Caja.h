// entities/Caja.h
// Clase Caja para demostrar colision perfectamente inelastica
// Las cajas se pegan al chocar y se mueven juntas

#ifndef CAJA_H
#define CAJA_H

#include <QObject>
#include <QPainter>
#include <QString>

class Caja : public QObject {
    Q_OBJECT

public:
    explicit Caja(int numero, float x, float y, QObject *parent = nullptr);
    ~Caja();

    // Getters
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    float getVelocidadX() const { return velocidadX; }
    float getMasa() const { return masa; }
    int getNumero() const { return numero; }
    bool estaEnMovimiento() const { return enMovimiento; }
    bool estaPegada() const { return pegada; }
    Caja* getCajaPegada() const { return cajaPegada; }

    // Setters
    void setPosicion(float x, float y);
    void setVelocidad(float vx);
    void setPegada(Caja* otra);

    // Metodos principales
    void actualizar(float deltaTime);
    void dibujar(QPainter& painter);
    void empujar(float direccionX);
    void pegarseA(Caja* otra);
    void detener();

    // Colisiones
    bool colisionaCon(const Caja* otra) const;

private:
    // Identificacion
    int numero;  // 1 o 2

    // Posicion
    float posX;
    float posY;

    // Fisica
    float velocidadX;
    float masa;
    float tamaño;

    // Estado
    bool enMovimiento;
    bool pegada;
    Caja* cajaPegada;  // Referencia a la otra caja si estan pegadas

    // Constantes fisicas
    static constexpr float FRICCION = 0.95f;
    static constexpr float VELOCIDAD_MINIMA = 5.0f;
    static constexpr float IMPULSO_EMPUJE = 150.0f;

    // Limites del area
    static constexpr float AREA_MIN_X = 250.0f;
    static constexpr float AREA_MAX_X = 550.0f;

    // Metodos privados
    void aplicarFriccion();
    void verificarLimites();
    void moverCajasPegadas(float deltaX);
};

#endif // CAJA_H
