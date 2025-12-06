// entities/Tren.h
// Tren del jugador (Nivel 2)

#ifndef TREN_H
#define TREN_H

#include <QObject>

class Tren : public QObject {
    Q_OBJECT

public:
    explicit Tren(QObject *parent = nullptr);
    ~Tren();

    // Getters
    float getVelocidad() const { return velocidad; }
    float getPosicion() const { return posicion; }
    float getCarbon() const { return carbon; }
    int getCargamentos() const { return cargamentos; }

    // Métodos
    void actualizar(float deltaTime);
    void acelerar(float incremento);
    void frenar(float decremento);
    void usarBoost();
    void cargarMercancia();
    void recogerCarbon(float cantidad);

signals:
    void carbonAgotado();
    void cargamentoRecogido();

private:
    float velocidad;
    float posicion;
    float carbon;
    int cargamentos;
    bool boostActivo;
};

#endif // TREN_H
