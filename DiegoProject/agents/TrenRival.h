// agents/TrenRival.h
// Agente inteligente del Nivel 2: Red Ferroviaria
// Trenes que evitan colisiones y compiten por recursos

#ifndef TRENRIVAL_H
#define TRENRIVAL_H

#include "Agente.h"
#include <list>

// Forward declaration
class Tren;

// Estructura para almacenar colisiones pasadas
struct ColisionRegistrada {
    float posicionX;
    int viaID;

    ColisionRegistrada(float x, int v) : posicionX(x), viaID(v) {}
};

class TrenRival : public Agente {
    Q_OBJECT

public:
    explicit TrenRival(QObject *parent = nullptr);
    ~TrenRival() override;

    // Implementación del ciclo de IA
    void percibir() override;
    void razonar() override;
    void actuar() override;
    void aprender() override;

    // Métodos específicos
    void setTrenJugador(Tren* tren);
    float getVelocidad() const { return velocidad; }
    int getViaActual() const { return viaActual; }

private:
    // Detección
    float calcularDistanciaAlJugador();
    bool hayPeligroColision();
    bool hayRecursosCerca();

    // Decisión
    void decidirCambioVia();
    void ajustarVelocidad();

    // Aprendizaje
    void registrarColision(float posX, int via);
    bool esZonaPeligrosa(float posX, int via);

    // Atributos específicos
    Tren* trenJugador;

    float velocidad;
    float velocidadMaxima;
    int viaActual;
    int viaObjetivo;

    // Historial de colisiones para aprendizaje
    std::list<ColisionRegistrada> colisionesPasadas;

    bool evitandoColision;
    float distanciaSeguridad;
};

#endif // TRENRIVAL_H
