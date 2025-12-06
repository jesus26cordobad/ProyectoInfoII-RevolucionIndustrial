// entities/VagonTren.h

#ifndef VAGONTREN_H
#define VAGONTREN_H

#include <QObject>
#include <QPixmap>
#include <QPainter>
#include <QRectF>

class VagonTren : public QObject {
    Q_OBJECT

public:
    explicit VagonTren(float y, float velocidadInicial, QObject *parent = nullptr);
    ~VagonTren();

    // Getters
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    float getVelocidad() const { return velocidad; }
    int getCarbonRecibido() const { return carbonRecibido; }
    int getCarbonRequerido() const { return carbonRequerido; }
    bool haTerminado() const { return terminado; }
    bool estaCompleto() const { return carbonRecibido >= carbonRequerido; }

    // Metodos
    void cargarSprite(const QPixmap& sprite);
    void actualizar(float deltaTime);
    void recibirCarbon();
    void dibujar(QPainter& painter);

    // Area de recepcion
    QRectF getAreaRecepcion() const;
    bool puedeRecibirCarbon(float x, float y) const;

signals:
    void trenSalio(bool completado);
    void carbonRecibidoCambiado(int cantidad);
    void vagonLleno();

private:
    float posX, posY;  // Posición donde se dibuja (esquina superior izquierda)
    float velocidad;
    int carbonRecibido;
    int carbonRequerido;
    bool terminado;

    QPixmap sprite;

    static constexpr float POS_INICIAL_X = -1000.0f;  // Empieza fuera de pantalla (izquierda)
    static constexpr float POS_SALIDA_X = 1400.0f;    // Sale de pantalla (derecha)
    static constexpr int CARBON_REQUERIDO_BASE = 20;  // Carbones necesarios

    // TAMAÑO DEL SPRITE (cambiar aquí para ajustar tamaño visual)
    static constexpr int ANCHO_SPRITE = 900;  // Ancho del sprite del tren
    static constexpr int ALTO_SPRITE = 900;   // Alto del sprite del tren

    // ÁREA DE RECEPCIÓN (zona verde donde el carbón entra)
    static constexpr float ANCHO_AREA_RECEPCION = 700.0f;
    static constexpr float ALTO_AREA_RECEPCION = 300.0f;
};

#endif // VAGONTREN_H
