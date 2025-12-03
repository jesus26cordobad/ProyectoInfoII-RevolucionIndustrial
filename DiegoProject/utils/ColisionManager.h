// utils/ColisionManager.h
// Módulo para manejar colisiones entre entidades en el nivel
// Desacopla la lógica de colisión del NivelFabrica

#ifndef COLISIONMANAGER_H
#define COLISIONMANAGER_H

#include <QRect>
#include <QVector>

class Telar;
class Jugador;

class ColisionManager {
public:
    ColisionManager();
    ~ColisionManager();

    // Configurar áreas de colisión
    void configurarCaldera(float x, float y, float ancho, float alto);
    void configurarBanda(float x, float y, float ancho, float alto);
    void agregarTelar(float x, float y, float ancho, float alto);
    void limpiarTelares();

    // Verificar colisiones
    bool hayColisionConObjeto(float x, float y, float radio = 30.0f) const;
    bool hayColisionConTelar(float x, float y) const;
    bool hayColisionConCaldera(float x, float y) const;
    bool hayColisionConBanda(float x, float y) const;

    // Validar movimiento (devuelve posición ajustada si hay colisión)
    void validarMovimiento(float& x, float& y, float radio = 30.0f) const;

private:
    struct Colisionable {
        float x, y, ancho, alto;
    };

    Colisionable caldera;
    Colisionable banda;
    QVector<Colisionable> telares;

    bool colisionConRectangulo(float x, float y, float radio, const Colisionable& rect) const;
};

#endif // COLISIONMANAGER_H
