// physics/FisicaMotor.h
// Motor de físicas con las ecuaciones de movimiento

#ifndef FISICAMOTOR_H
#define FISICAMOTOR_H

#include <QObject>
#include <cmath>

class FisicaMotor : public QObject {
    Q_OBJECT

public:
    explicit FisicaMotor(QObject *parent = nullptr);
    ~FisicaMotor();

    // Configuración
    void setGravedad(float g) { gravedad = g; }
    void setDeltaTime(float dt) { deltaTime = dt; }

    float getGravedad() const { return gravedad; }
    float getDeltaTime() const { return deltaTime; }

    // === FÍSICAS DEL NIVEL 1: FÁBRICA ===

    // Movimiento lineal uniforme: x = x0 + v*t
    float movimientoLinealUniforme(float posicionInicial, float velocidad, float tiempo);

    // Oscilación amortiguada: x = A * e^(-bt) * cos(ωt)
    float oscilacionAmortiguada(float amplitud, float amortiguamiento,
                                float frecuencia, float tiempo);

    // Crecimiento exponencial: T = T0 * e^(kt)
    float crecimientoExponencial(float valorInicial, float constanteK, float tiempo);

    // === FÍSICAS DEL NIVEL 2: FERROCARRIL ===

    // MRUA - Velocidad: v = v0 + a*t
    float velocidadMRUA(float velocidadInicial, float aceleracion, float tiempo);

    // MRUA - Posición: x = x0 + v0*t + 0.5*a*t²
    float posicionMRUA(float posicionInicial, float velocidadInicial,
                       float aceleracion, float tiempo);

    // Trayectoria parabólica - Eje Y: y = y0 + vy*t - 0.5*g*t²
    float trayectoriaParabolicaY(float alturaInicial, float velocidadVertical, float tiempo);

    // Trayectoria parabólica - Eje X: x = x0 + vx*t
    float trayectoriaParabolicaX(float posicionInicial, float velocidadHorizontal, float tiempo);

    // Decrecimiento exponencial: C = C0 * e^(-kt)
    float decrecimientoExponencial(float valorInicial, float constanteK, float tiempo);

    // === FÍSICAS DEL NIVEL 3: PUENTE ===

    // Oscilación de péndulo: θ = θ0 * cos(sqrt(g/L) * t)
    float anguloPendulo(float anguloInicial, float longitudCable, float tiempo);

    // Caída libre: y = y0 - 0.5*g*t²
    float caidaLibre(float alturaInicial, float tiempo);

    // Velocidad en caída libre: v = -g*t
    float velocidadCaidaLibre(float tiempo);

    // Fricción - Aceleración: a = -μ*g
    float aceleracionFriccion(float coeficienteRozamiento);

    // Fricción - Velocidad: v = v0 + a*t
    float velocidadConFriccion(float velocidadInicial, float coeficienteRozamiento, float tiempo);

private:
    float gravedad;    // m/s² (por defecto 9.81)
    float deltaTime;   // Tiempo entre frames
};

#endif // FISICAMOTOR_H
