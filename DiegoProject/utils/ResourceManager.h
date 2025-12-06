// utils/ResourceManager.h
// Gestor centralizado de recursos (sprites, sonidos, etc.)

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QPixmap>
#include <QMap>
#include <QString>
#include <QDebug>

class ResourceManager {
public:
    // Singleton pattern
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Cargar todos los recursos del juego
    void cargarRecursos() {
        qDebug() << "Cargando recursos del juego...";

        // NIVEL 1: FÁBRICA TEXTIL
        sprites["banda_transportadora1"] = QPixmap(":/recursos/images/nivel1/banda_transportadora.png");
        sprites["banda_transportadora2"] = QPixmap(":/recursos/images/nivel1/banda_transportadora2.png");
        sprites["caldera_principal"] = QPixmap(":/recursos/images/nivel1/caldera_principal.png");
        sprites["capataz"] = QPixmap(":/recursos/images/nivel1/capataz.png");
        sprites["capataz_frente"] = QPixmap(":/recursos/images/nivel1/capataz_frente.png");
        sprites["capataz_espalda"] = QPixmap(":/recursos/images/nivel1/capataz_espalda.png");
        sprites["capataz_izquierda"] = QPixmap(":/recursos/images/nivel1/capataz_izquierda.png");
        sprites["capataz_derecha"] = QPixmap(":/recursos/images/nivel1/capataz_derecha.png");
        sprites["carrete_azul"] = QPixmap(":/recursos/images/nivel1/carrete_azul.png");
        sprites["carrete_rojo"] = QPixmap(":/recursos/images/nivel1/carrete_rojo.png");
        sprites["carrete_verde"] = QPixmap(":/recursos/images/nivel1/carrete_verde.png");
        sprites["carrete_azul_girado"] = QPixmap(":/recursos/images/nivel1/carrete_azul_girado.png");
        sprites["carrete_rojo_girado"] = QPixmap(":/recursos/images/nivel1/carrete_rojo_girado.png");
        sprites["carrete_verde_girado"] = QPixmap(":/recursos/images/nivel1/carrete_verde_girado.png");
        sprites["carrete_azul_destruido"] = QPixmap(":/recursos/images/nivel1/carrete_azul_destruido.png");
        sprites["carrete_rojo_destruido"] = QPixmap(":/recursos/images/nivel1/carrete_rojo_destruido.png");
        sprites["carrete_verde_destruido"] = QPixmap(":/recursos/images/nivel1/carrete_verde_destruido.png");
        sprites["fondo_fabrica"] = QPixmap(":/recursos/images/nivel1/fondo_fabrica.png");
        sprites["ingeniero"] = QPixmap(":/recursos/images/nivel1/ingeniero.png");
        sprites["jugador"] = QPixmap(":/recursos/images/nivel1/jugador.png");
        sprites["jugador_frente"] = QPixmap(":/recursos/images/nivel1/jugador_frente.png");
        sprites["jugador_espalda"] = QPixmap(":/recursos/images/nivel1/jugador_espalda.png");
        sprites["jugador_izquierda"] = QPixmap(":/recursos/images/nivel1/jugador_izquierda.png");
        sprites["jugador_derecha"] = QPixmap(":/recursos/images/nivel1/jugador_derecha.png");
        sprites["manometro_vapor"] = QPixmap(":/recursos/images/nivel1/manometro_vapor.png");
        sprites["telar_normal"] = QPixmap(":/recursos/images/nivel1/telar_normal.png");
        sprites["telar_caliente"] = QPixmap(":/recursos/images/nivel1/telar_caliente.png");
        sprites["telar_averiado"] = QPixmap(":/recursos/images/nivel1/telar_averiado.png");
        sprites["caja1"] = QPixmap(":recursos/images/nivel1/caja1.png");
        sprites["caja2"] = QPixmap(":recursos/images/nivel1/caja2.png");

        // INICIO NIVEL 2: FERROCARRIL
        sprites["fondo_frame1"] = QPixmap(":/recursos/images/nivel2/fondo_frame1.png");
        sprites["fondo_frame2"] = QPixmap(":/recursos/images/nivel2/fondo_frame2.png");
        sprites["fondo_frame3"] = QPixmap(":/recursos/images/nivel2/fondo_frame3.png");
        sprites["fondo_frame4"] = QPixmap(":/recursos/images/nivel2/fondo_frame4.png");
        sprites["fondo_frame5"] = QPixmap(":/recursos/images/nivel2/fondo_frame5.png");
        sprites["jugadornivel2"] = QPixmap(":/recursos/images/nivel2/jugadornivel2.png");
        sprites["capataznivel2"] = QPixmap(":/recursos/images/nivel2/capataznivel2.png");
        sprites["carbon"] = QPixmap(":/recursos/images/nivel2/carbon.png");
        sprites["tren"] = QPixmap(":/recursos/images/nivel2/tren.png");
        // FIN NIVEL 2

        verificarCarga();
    }

    // Obtener sprite por nombre
    QPixmap getSprite(const QString& nombre) const {
        if (sprites.contains(nombre)) {
            return sprites[nombre];
        } else {
            qWarning() << "Sprite no encontrado:" << nombre;
            return QPixmap(); // Retorna pixmap vacío
        }
    }

    // Verificar que todos los sprites se cargaron correctamente
    void verificarCarga() const {
        int cargados = 0;
        int fallidos = 0;

        for (auto it = sprites.begin(); it != sprites.end(); ++it) {
            if (it.value().isNull()) {
                qWarning() << "Sprite no cargado:" << it.key();
                fallidos++;
            } else {
                cargados++;
            }
        }

        qDebug() << "Sprites cargados:" << cargados;
        if (fallidos > 0) {
            qWarning() << "Sprites fallidos:" << fallidos;
        }
    }

private:
    ResourceManager() {}
    ~ResourceManager() {}

    // Evitar copias
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    QMap<QString, QPixmap> sprites;
};

#endif // RESOURCEMANAGER_H
