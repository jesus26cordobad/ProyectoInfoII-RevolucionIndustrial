// levels/NivelFerrocarril.h
// Nivel 2: La Red Ferroviaria

#ifndef NIVELFERROCARRIL_H
#define NIVELFERROCARRIL_H

#include "Nivel.h"
#include <vector>
#include <list>
#include <QPixmap>
#include <QVector>
#include <QRect>

// Forward declarations
class MontanaCarbon;
class VagonTren;
class Carbon;
class Capataz;
class HUDRenderer;

class NivelFerrocarril : public Nivel {
    Q_OBJECT

public:
    explicit NivelFerrocarril(Jugador* jugador, QWidget *parent = nullptr);
    ~NivelFerrocarril() override;

    void inicializar() override;
    void actualizar() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    // Limpieza
    void limpiarNivel();

    // Creacion de entidades
    void crearMontanasCarbon();
    void crearVagonTren();

    // Actualizacion de componentes
    void actualizarJugador(float deltaTime);
    void actualizarCapataz(float deltaTime);
    void actualizarVagones(float deltaTime);
    void actualizarCarbones(float deltaTime);
    void actualizarFondoAnimado(float deltaTime);
    void actualizarDificultad(float deltaTime);

    // Verificaciones
    void verificarCondicionesFinales();
    void verificarColisionCarbonVagon();

    // Dibujo
    void dibujarFondo(QPainter& painter);
    void dibujarMontanasCarbon(QPainter& painter);
    void dibujarVagones(QPainter& painter);
    void dibujarCarbones(QPainter& painter);
    void dibujarJugador(QPainter& painter);
    void dibujarCapataz(QPainter& painter);
    void dibujarHUD(QPainter& painter);
    void dibujarGameOver(QPainter& painter);
    void mostrarPantallaGameOver(const QString& razon);

    // Manejo de eventos
    void onClickMontanaJugador();
    void onClickMontanaCapataz();
    void lanzarCarbon(float origenX, float origenY, bool esJugador);

    // Entidades del nivel
    MontanaCarbon* montanaJugador;
    MontanaCarbon* montanaCapataz;
    std::vector<VagonTren*> vagones;
    std::list<Carbon*> carbones;
    Capataz* capataz;
    HUDRenderer* hudRenderer;

    // Sprites
    QVector<QPixmap> fondosAnimados;
    QPixmap spriteTren;
    QPixmap spriteJugadorPalear;
    QPixmap spriteCapatazPalear;

    // Variables de gameplay
    int trenesCompletados;
    int trenesObjetivo;
    int trenesFallidos;
    float velocidadTrenBase;
    float velocidadTrenActual;
    float tiempoEntreVagones;
    float tiempoDesdeUltimoVagon;

    // Animacion de fondo
    int frameFondoActual;
    float tiempoAnimacionFondo;

    // Control de capataz
    bool capatazActivo;
    float tiempoAutoPalearCapataz;
    int vagonesAyudadosCapataz;

    // Variables Delay
    bool esperandoLanzarCarbonesJugador;
    bool esperandoLanzarCarbonesCapataz;
    float tiempoEsperaLanzamiento;
    static constexpr float DELAY_LANZAMIENTO = 0.6f;  // Frame 4
    float origenXGuardado;
    float origenYGuardado;
    bool esLanzamientoJugador;

    // Constantes
    static constexpr int TRENES_OBJETIVO = 10;
    static constexpr float VELOCIDAD_TREN_INICIAL = 80.0f;
    static constexpr float TIEMPO_ENTRE_VAGONES_INICIAL = 5.0f;
    static constexpr float INCREMENTO_VELOCIDAD = 10.0f;
    static constexpr float TIEMPO_AUTO_PALEAR_CAPATAZ = 1.5f;
    static constexpr int MAX_VAGONES_AYUDA_CAPATAZ = 2;
    static constexpr float POSICION_ZONA_PALEO = 200.0f;
    static constexpr float GRAVEDAD = 400.0f;
    QRect botonPausaRect;
    bool mostrarGameOver;
    QString razonGameOver;
};

#endif // NIVELFERROCARRIL_H
