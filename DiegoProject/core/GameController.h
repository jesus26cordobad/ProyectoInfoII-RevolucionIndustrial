// core/GameController.h
// Controlador principal que gestiona el flujo del juego
// Maneja la creación y transición entre niveles

#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>

// Forward declarations
class Nivel;
class Jugador;

class GameController : public QObject {
    Q_OBJECT

public:
    explicit GameController(QObject *parent = nullptr);
    ~GameController();

    // Métodos públicos
    void iniciarNivel(int numeroNivel);
    void finalizarNivel(bool victoria);
    void pausarJuego();
    void reanudarJuego();

    // Getters
    int getNivelActual() const { return nivelActual; }
    Jugador* getJugador() const { return jugador; }

signals:
    void nivelTerminado();
    void juegoFinalizado();
    void puntajeActualizado(int puntaje);

private:
    // Métodos privados
    void crearNivel(int numero);
    void limpiarNivel();
    void guardarProgreso();

    // Atributos
    int nivelActual;
    Jugador* jugador;
    Nivel* nivelEnCurso;

    // Estado del juego
    bool jugandoActualmente;
    bool pausado;
};

#endif // GAMECONTROLLER_H
