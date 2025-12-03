// physics/FisicaMotor.cpp
// Implementación de todas las ecuaciones físicas del juego

#include "FisicaMotor.h"
#include <QDebug>

FisicaMotor::FisicaMotor(QObject *parent)
    : QObject(parent),
    gravedad(9.81f),
    deltaTime(0.016f) {  // ~60 FPS

    qDebug() << "FisicaMotor inicializado - g:" << gravedad;
}

FisicaMotor::~FisicaMotor() {
    qDebug() << "FisicaMotor destruido";
}

// ============================================
// FÍSICAS DEL NIVEL 1: FÁBRICA TEXTIL
// ============================================

float FisicaMotor::movimientoLinealUniforme(float posicionInicial, float velocidad, float tiempo) {
    // x = x0 + v*t
    return posicionInicial + velocidad * tiempo;
}

float FisicaMotor::oscilacionAmortiguada(float amplitud, float amortiguamiento,
                                         float frecuencia, float tiempo) {
    // x = A * e^(-bt) * cos(ωt)
    float exponencial = std::exp(-amortiguamiento * tiempo);
    float coseno = std::cos(frecuencia * tiempo);
    return amplitud * exponencial * coseno;
}

float FisicaMotor::crecimientoExponencial(float valorInicial, float constanteK, float tiempo) {
    // T = T0 * e^(kt)
    return valorInicial * std::exp(constanteK * tiempo);
}

// ============================================
// FÍSICAS DEL NIVEL 2: RED FERROVIARIA
// ============================================

float FisicaMotor::velocidadMRUA(float velocidadInicial, float aceleracion, float tiempo) {
    // v = v0 + a*t
    return velocidadInicial + aceleracion * tiempo;
}

float FisicaMotor::posicionMRUA(float posicionInicial, float velocidadInicial,
                                float aceleracion, float tiempo) {
    // x = x0 + v0*t + 0.5*a*t²
    return posicionInicial + velocidadInicial * tiempo + 0.5f * aceleracion * tiempo * tiempo;
}

float FisicaMotor::trayectoriaParabolicaY(float alturaInicial, float velocidadVertical, float tiempo) {
    // y = y0 + vy*t - 0.5*g*t²
    return alturaInicial + velocidadVertical * tiempo - 0.5f * gravedad * tiempo * tiempo;
}

float FisicaMotor::trayectoriaParabolicaX(float posicionInicial, float velocidadHorizontal, float tiempo) {
    // x = x0 + vx*t
    return posicionInicial + velocidadHorizontal * tiempo;
}

float FisicaMotor::decrecimientoExponencial(float valorInicial, float constanteK, float tiempo) {
    // C = C0 * e^(-kt)
    return valorInicial * std::exp(-constanteK * tiempo);
}

// ============================================
// FÍSICAS DEL NIVEL 3: CONSTRUCCIÓN DEL PUENTE
// ============================================

float FisicaMotor::anguloPendulo(float anguloInicial, float longitudCable, float tiempo) {
    // θ = θ0 * cos(sqrt(g/L) * t)
    float omega = std::sqrt(gravedad / longitudCable);
    return anguloInicial * std::cos(omega * tiempo);
}

float FisicaMotor::caidaLibre(float alturaInicial, float tiempo) {
    // y = y0 - 0.5*g*t²
    return alturaInicial - 0.5f * gravedad * tiempo * tiempo;
}

float FisicaMotor::velocidadCaidaLibre(float tiempo) {
    // v = -g*t (negativo porque va hacia abajo)
    return -gravedad * tiempo;
}

float FisicaMotor::aceleracionFriccion(float coeficienteRozamiento) {
    // a = -μ*g (negativo porque frena el movimiento)
    return -coeficienteRozamiento * gravedad;
}

float FisicaMotor::velocidadConFriccion(float velocidadInicial, float coeficienteRozamiento, float tiempo) {
    // v = v0 + a*t donde a = -μ*g
    float aceleracion = aceleracionFriccion(coeficienteRozamiento);
    return velocidadInicial + aceleracion * tiempo;
}
