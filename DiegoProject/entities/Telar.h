// entities/Telar.h
// Telar automático con temperatura, producción y dibujo

#ifndef TELAR_H
#define TELAR_H

#include <QObject>
#include <QPainter>
#include <QString>

class Telar : public QObject {
    Q_OBJECT

public:
    explicit Telar(float x, float y, QObject *parent = nullptr);
    ~Telar();

    // Getters
    float getTemperatura() const { return temperatura; }
    bool estaFuncionando() const { return funcionando; }
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    QString getColorAsignado() const { return colorAsignado; }
    float getProduccion() const { return produccionActual; }

    // Setters
    void setColorAsignado(const QString& color) { colorAsignado = color; }
    void setPosicion(float x, float y) { posX = x; posY = y; }

    // Métodos de actualización
    void actualizar(float deltaTime);
    void calentar(float incremento);
    void reparar();
    void apagar();

    // Producción
    void procesarCarrete(const QString& colorCarrete);
    bool puedeProducir() const;

    // Dibujo
    void dibujar(QPainter& painter);

signals:
    void sobrecalentado();
    void reparado();
    void produccionCompletada(float metros);

private:
    // Posición
    float posX;
    float posY;

    // Estado
    float temperatura;
    bool funcionando;
    float tiempoSinMantenimiento;

    // Producción
    QString colorAsignado;
    float produccionActual;  // Metros de tela producidos
    float velocidadProduccion; // Metros por segundo

    // Visualización
    int tamaño;
};

#endif // TELAR_H
