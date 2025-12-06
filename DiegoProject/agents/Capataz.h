// agents/Capataz.h
// Agente que repara telares con movimiento y animación

#ifndef CAPATAZ_H
#define CAPATAZ_H

#include "Agente.h"
#include <vector>
#include <map>
#include <QPixmap>
#include <QPainter>
#include <QVector>

class Telar;

class Capataz : public Agente {
    Q_OBJECT

public:
    enum Direccion {
        FRENTE,
        ESPALDA,
        IZQUIERDA,
        DERECHA
    };

    explicit Capataz(QObject *parent = nullptr);
    ~Capataz();

    // Configuración
    void setTelares(std::vector<Telar*>* t);
    void setPosicion(float x, float y) { posX = x; posY = y; }

    // Getters
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    Direccion getDireccion() const { return direccion; }
    int getFrame() const { return frameAnimacion; }
    bool estaReparando() const { return reparando; }

    // Ciclo de IA (heredado de Agente)
    void percibir() override;
    void razonar() override;
    void actuar() override;
    void aprender() override;

    // Actualización
    void actualizar(float deltaTime);

    // Dibujo
    void dibujar(QPainter& painter);
    void cargarSprites(const QString& spriteFrente, const QString& spriteEspalda,
                       const QString& spriteIzquierda, const QString& spriteDerecha);

    // INICIO NIVEL 2: Funcionalidad de palear carbon

    // Getters Nivel 2
    int getFramePalear() const { return framePalearN2; }
    bool estaPaleando() const { return paleandoN2; }

    // Metodos Nivel 2
    void cargarSpritesPalearN2(const QPixmap& spriteSheet);
    void iniciarPalearN2();
    void actualizarPalearN2(float deltaTime);
    void activarAyudaN2(bool activar) { ayudandoN2 = activar; }
    bool estaAyudandoN2() const { return ayudandoN2; }

    // FIN NIVEL 2

signals:
    void accionRealizada(int tipoAccion);

    // INICIO NIVEL 2: Señales
    void palearCompletadoN2();
    // FIN NIVEL 2

private:
    // Telares
    std::vector<Telar*>* telares;
    Telar* telarObjetivo;

    // Movimiento
    float posX, posY;
    float velocidad;
    Direccion direccion;

    // Animación
    int frameAnimacion;
    float tiempoAnimacion;
    bool estaQuieto;  // para distinguir quieto/caminando

    // Sprites
    QPixmap spriteFrente;
    QPixmap spriteEspalda;
    QPixmap spriteIzquierda;
    QPixmap spriteDerecha;

    // Estado de reparación
    float tiempoReparacion;
    float tiempoReparacionActual;
    bool reparando;

    // Mapa de frecuencias para aprendizaje
    std::map<int, int> frecuenciasFallos;

    // Métodos auxiliares
    int detectarTelarCritico();
    float calcularPrioridad(Telar* telar);
    void moverHacia(float x, float y, float deltaTime);
    void repararTelar(Telar* telar);
    void actualizarFrecuencias();
    void actualizarAnimacion(float deltaTime);
    void cambiarFrame();  // para controlar frames correctamente
    QPixmap getSpriteActual() const;

    // INICIO NIVEL 2: Atributos de palear

    bool ayudandoN2;
    bool paleandoN2;
    int framePalearN2;
    float tiempoPalearN2;
    float velocidadPalearN2;
    float tiempoSinPalearN2;
    QVector<QPixmap> spritesPalearN2;

    static constexpr float VELOCIDAD_PALEAR_N2 = 0.2f;
    static constexpr int TOTAL_FRAMES_PALEAR_N2 = 4;
    static constexpr float TIEMPO_DESCANSO_AUTO_N2 = 2.0f;

    // FIN NIVEL 2
};

#endif // CAPATAZ_H
