// ui/MainMenu.h

#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QPainterPath>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>

// Forward declaration
class GameController;

// Estructura para partículas de humo realistas
struct HumoParticula {
    QPainterPath forma;
    QPointF posicion;
    QPointF velocidad;
    float opacidad;
    float escala;
    float rotacion;
    float velocidadRotacion;

    HumoParticula() : opacidad(255), escala(1.0f), rotacion(0), velocidadRotacion(0) {}
};

class MainMenu : public QWidget {
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onNivel1Clicked();
    void onNivel2Clicked();
    void onNivel3Clicked();
    void onNivelesClicked();
    void onCreditosClicked();
    void onSalirClicked();
    void actualizarAnimaciones();
    void toggleMute();          // ← NUEVO (botón mute)

private:
    void setupUI();
    void inicializarHumo();
    void crearFormaHumo(HumoParticula& p);
    void dibujarFondo(QPainter& painter);
    void dibujarHumo(QPainter& painter);
    void dibujarMenu(QPainter& painter);
    void dibujarEngranajeHover(QPainter& painter, const QPointF& pos);
    void actualizarHumo();
    int obtenerOpcionEnPosicion(const QPoint& pos);
    void inicializarMusica();  // ← NUEVO

    // Recursos
    QPixmap fondoImagen;

    // Audio
    QMediaPlayer* reproductorMusica;
    QAudioOutput* salidaAudio;
    QPushButton* botonMute;

    // Animaciones
    QTimer* timerAnimacion;
    QVector<HumoParticula> humo;

    // Opciones del menú
    QStringList opcionesMenu;
    QStringList opcionesNiveles;
    int opcionSeleccionada;
    bool nivelesExpandido;
    bool musicaMuteada;

    // Controlador del juego
    GameController* controladorJuego;
};

#endif // MAINMENU_H
